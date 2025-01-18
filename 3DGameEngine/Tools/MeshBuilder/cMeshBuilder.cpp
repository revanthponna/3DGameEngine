#include "cMeshBuilder.h"

#include <Engine\Platform\Platform.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Asserts/Asserts.h>
#include <iostream>
#include <fstream>

using namespace eae6320;

cResult Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	lua_State* luaState = nullptr;
	Graphics::VertexFormats::sVertex_mesh* vertexData = nullptr;
	uint16_t* indexData = nullptr;
	uint16_t numVertices = 0;
	uint16_t numIndices = 0;
	std::string errorMessage;
	cResult result = eae6320::Results::Success;

	cScopeGuard scopeGuard_onExit([&luaState]
	{
		lua_pop(luaState, 1);

		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack
			// regardless of any errors
			EAE6320_ASSERT(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = nullptr;
		}
	});

	if (LoadLuaFile(m_path_source, luaState) != eae6320::Results::Success)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to load Lua file.");
		result = Results::Failure;
		return result;
	}

	// The table is now at index - 1

	// Right now the asset table is at -1
	// After the following table operation it will be at -2
	// and the 'vertices' table will be at -1
	constexpr auto* const key = "vertices";
	lua_pushstring(luaState, key);
	lua_gettable(luaState, -2);

	// We can create a scope guard immediately as soon as the new table has been pushed
	// to guarantee that it will be popped when we are done with it:
	//eae6320::cScopeGuard scopeGuard_popVertices([luaState]
	//{
	//	lua_pop(luaState, 1);
	//});

	// Additionally, I try not to do any further stack manipulation in this function
	// and call other functions that assume the "vertices" table is at -1
	// but don't know or care about the rest of the stack
	if (lua_istable(luaState, -1))
	{
		if (!(LoadVerticesFromTable(*luaState, vertexData, numVertices))) {
			OutputErrorMessageWithFileInfo(m_path_source, "Failed to load vertices.");
			result = Results::Failure;
			return result;
		}
	}
	else
	{
		std::cerr << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(luaState, -1) << ")" << std::endl;
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to load vertices.");
		result = Results::Failure;
		return result;
	}

	// Popping the vertex table
	lua_pop(luaState, 1);

	constexpr auto* const key2 = "indices";
	lua_pushstring(luaState, key2);
	lua_gettable(luaState, -2);

	eae6320::cScopeGuard scopeGuard_popIndices([luaState]
	{
		lua_pop(luaState, 1);
	});

	if (lua_istable(luaState, -1))
	{
		if (!(LoadIndicesFromTable(*luaState, indexData, numIndices))) {
			OutputErrorMessageWithFileInfo(m_path_source, "Failed to load indices.");
			result = Results::Failure;
			return result;
		}
	}
	else
	{
		std::cerr << "The value at \"" << key2 << "\" must be a table "
			"(instead of a " << luaL_typename(luaState, -1) << ")" << std::endl;
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to load indices.");
		result = Results::Failure;
		return result;
	}

#ifdef EAE6320_PLATFORM_GL
	ConvertLeftHandedToRightHanded(numVertices, vertexData, numIndices, indexData);
#endif // EAE6320_PLATFORM_GL

	// Closing Lua State
	// lua_close(luaState);

	// Opening binary file for writing
	std::ofstream outputFile(m_path_target, std::ios::binary);
	if (!outputFile)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to open binary output file.");
		return eae6320::Results::Failure;
	}

	// Writing number of vertices
	outputFile.write(reinterpret_cast<const char*>(&numVertices), sizeof(numVertices));

	// Writing vertex data
	outputFile.write(reinterpret_cast<const char*>(vertexData), sizeof(Graphics::VertexFormats::sVertex_mesh) * numVertices);

	// Writing number of indices
	outputFile.write(reinterpret_cast<const char*>(&numIndices), sizeof(numIndices));

	// Writing index data
	outputFile.write(reinterpret_cast<const char*>(indexData), sizeof(uint16_t) * numIndices);

	outputFile.close();

	delete[] vertexData;
	delete[] indexData;

	return eae6320::Results::Success;
}

cResult Assets::cMeshBuilder::LoadLuaFile(const std::string& pFilePath, lua_State*& pLuaState)
{
	auto result = eae6320::Results::Success;

	// Creating a new Lua State
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			std::cerr << "Failed to create a new Lua State" << std::endl;
			return result;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, pFilePath.c_str());
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Popping the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET; // Return everything that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file must return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")" << std::endl;
					// Popping the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)" << std::endl;
				// Popping every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << lua_tostring(luaState, -1) << std::endl;
			// Popping the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached, then it means the asset file was successfully loaded
	pLuaState = luaState;

	return result;
}

cResult Assets::cMeshBuilder::LoadVerticesFromTable(lua_State& io_luaState, Graphics::VertexFormats::sVertex_mesh*& o_Vertices, uint16_t& pNumVertices)
{
	auto result = eae6320::Results::Success;

	Graphics::VertexFormats::sVertex_mesh* vertexData;

	cScopeGuard scopeGuard_clearVertexOnFailure([result, vertexData]
	{
		if (result != eae6320::Results::Success)
			delete (vertexData);
	});

	std::cout << "Iterating through every vertex:" << std::endl;
	const auto vertexCount = luaL_len(&io_luaState, -1);
	pNumVertices = (uint16_t)vertexCount;

	vertexData = new Graphics::VertexFormats::sVertex_mesh[pNumVertices];
	int ndx = 0;

	for (uint16_t i = 1; i <= pNumVertices; i++)
	{
		float values[3]{};

		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);

		// Getting the row table at index i
		//lua_rawgeti(&io_luaState, -1, i);

		// Checking if it's a table (inner array)
		if (lua_istable(&io_luaState, -1))
		{
			// Iterating through each number in the row
			for (int j = 1; j <= 3; j++) {
				lua_pushinteger(&io_luaState, j);
				lua_gettable(&io_luaState, -2);

				//lua_rawgeti(&io_luaState, -1, j);

				// Checking if it's a number
				if (lua_isnumber(&io_luaState, -1))

					values[j - 1] = (float)lua_tonumber(&io_luaState, -1);
				else
					result = eae6320::Results::Failure;

				lua_pop(&io_luaState, 1); // Popping the number value
			}

			vertexData[i - 1] = values;
		}
		else
			result = eae6320::Results::Failure;

		lua_pop(&io_luaState, 1); // Popping the row table
	}

	o_Vertices = vertexData;
	return result;
}

cResult Assets::cMeshBuilder::LoadIndicesFromTable(lua_State& io_luaState, uint16_t*& o_Indices, uint16_t& pNumIndices)
{
	auto result = eae6320::Results::Success;
	uint16_t* indices;

	eae6320::cScopeGuard scopeGuard_clearVertexOnFailure([result, indices]
		{
			if (result != eae6320::Results::Success)
				delete (indices);
		});

	std::cout << "Iterating through every index:" << std::endl;
	const auto numIndices = luaL_len(&io_luaState, -1);
	pNumIndices = (uint16_t)numIndices;

	indices = new uint16_t[pNumIndices];

	for (uint16_t i = 1; i <= pNumIndices; i++)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);

		// Checking if it's a number
		if (lua_isinteger(&io_luaState, -1))

			indices[i - 1] = (uint16_t)lua_tointeger(&io_luaState, -1);
		else
			result = eae6320::Results::Failure;

		lua_pop(&io_luaState, 1); // Popping the number value
	}

	o_Indices = indices;

	return result;
}

void Assets::cMeshBuilder::ConvertLeftHandedToRightHanded(uint16_t pNumVertices, Graphics::VertexFormats::sVertex_mesh* pVertexData, uint16_t pNumIndices, uint16_t* pIndexData) {
	for (uint16_t i = 0; i < pNumVertices; ++i)
		pVertexData[i].z = -pVertexData[i].z;

	for (uint16_t i = 0; i < pNumIndices; i += 3) {

		uint16_t temp = pIndexData[i + 1];
		pIndexData[i + 1] = pIndexData[i + 2];
		pIndexData[i + 2] = temp;
	}
}