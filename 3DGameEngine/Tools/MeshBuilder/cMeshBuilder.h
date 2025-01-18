#pragma once

/*
	This class builds meshes
*/

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <External/Lua/Includes.h>
#include <Engine/Graphics/VertexFormats.h>

//#include <Engine/Graphics/Configuration.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;
			static cResult LoadLuaFile(const std::string& pFilePath, lua_State*& pLuaState);
			static cResult LoadVerticesFromTable(lua_State& io_luaState, Graphics::VertexFormats::sVertex_mesh*& o_Vertices, uint16_t& pNumVertices);
			static cResult LoadIndicesFromTable(lua_State& io_luaState, uint16_t*& o_Indices, uint16_t& pNumIndices);
			void ConvertLeftHandedToRightHanded(uint16_t pNumVertices, Graphics::VertexFormats::sVertex_mesh* pVertexData, uint16_t pNumIndices, uint16_t* pIndexData);
		};
	}
}

