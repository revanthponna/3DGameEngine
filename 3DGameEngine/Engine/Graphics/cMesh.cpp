#include "cMesh.h"

#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Platform/Platform.h>
#include <new>
#include <iostream>

eae6320::Graphics::Mesh* eae6320::Graphics::Mesh::CreateMeshFromFile(const std::string& pFilePath)
{
	Mesh* mesh = nullptr;
	eae6320::Platform::sDataFromFile dataFromFile;
	cResult result = eae6320::Platform::LoadBinaryFile(pFilePath.c_str(), dataFromFile);
	if (result != eae6320::Results::Success)
	{
		EAE6320_ASSERTF(false, "Can't load binary file.");
		eae6320::Logging::OutputMessage("Can't load binary file.");
		return mesh;
	}

	// Initializing offsets
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	// 1. Extracting the number of vertices (uint16_t)
	uint16_t vertexCount;
	memcpy(&vertexCount, reinterpret_cast<void*>(currentOffset), sizeof(vertexCount));
	currentOffset += sizeof(vertexCount); // Moving offset to the vertex data

	// 2. Extracting the vertex array (VertexFormats::sVertex_mesh)
	const auto* const vertexArray = reinterpret_cast<VertexFormats::sVertex_mesh*>(currentOffset);
	currentOffset += sizeof(VertexFormats::sVertex_mesh) * vertexCount; // Moving offset to number of indices

	// 3. Extracting the number of indices (uint16_t)
	uint16_t indexCount;
	memcpy(&indexCount, reinterpret_cast<void*>(currentOffset), sizeof(indexCount));
	currentOffset += sizeof(indexCount); // Moving offset to the index data

	// 4. Extracting the index array (uint16_t)
	const auto* const indexArray = reinterpret_cast<uint16_t*>(currentOffset);
	currentOffset += sizeof(uint16_t) * indexCount; // Now at the end of the binary chunk

	// At this point, we have the vertex and index data, so we can initialize the mesh
	GeometryData MeshInitData;
	MeshInitData.numVertices = vertexCount;
	MeshInitData.vertexData = new VertexFormats::sVertex_mesh[vertexCount];
	memcpy(MeshInitData.vertexData, vertexArray, sizeof(VertexFormats::sVertex_mesh) * vertexCount);

	MeshInitData.numIndexes = indexCount;
	MeshInitData.indexData = new uint16_t[indexCount];
	memcpy(MeshInitData.indexData, indexArray, sizeof(uint16_t) * indexCount);

	if (!(eae6320::Graphics::Mesh::Load(MeshInitData, mesh)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
		return mesh;
	}

	return mesh;
}

eae6320::cResult eae6320::Graphics::Mesh::Load(GeometryData& pInitData, Mesh*& o_Mesh)
{
	auto result = Results::Success;

	Mesh* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_Mesh, &result, &pInitData, &newMesh]
	{
		if (result)
		{
			EAE6320_ASSERT(newMesh != nullptr);
			o_Mesh = newMesh;
		}
		else
		{
			if (newMesh)
			{
				newMesh->DecrementReferenceCount();
				newMesh = nullptr;
			}
			o_Mesh = nullptr;
		}
	});

	// Allocating a mesh
	{
		newMesh = new (std::nothrow) Mesh();
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			return result;
		}
	}

	// Initializing the platform-specific graphics API mesh geometry
	result = newMesh->InitializeGeometry(pInitData);

	if (!result)
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		return result;
	}

	return result;
}

eae6320::Graphics::Mesh::~Mesh()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	eae6320::cResult result = eae6320::Results::Success;
	CleanUpGeometry(result);
	EAE6320_ASSERT(result);
}
