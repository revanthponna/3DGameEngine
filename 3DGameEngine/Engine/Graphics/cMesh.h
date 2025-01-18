#pragma once

#include "cVertexFormat.h"
#include "cConstantBuffer.h"
#include "VertexFormats.h"


#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <External/Lua/Includes.h>
#include <string>

namespace eae6320
{
	namespace Graphics
	{
		struct GeometryData {
			eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData;
			uint16_t* indexData;
			uint16_t numVertices;
			uint16_t numIndexes;
		};

		class Mesh
		{
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Mesh)
		public:
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Public Methods
			static Graphics::Mesh* CreateMeshFromFile(const std::string& pFilePath);
			static cResult Load(GeometryData& pInitData, Mesh*& o_Mesh);
			void DrawGeometry();

		private:
			cResult InitializeGeometry(GeometryData& pInitData);
			void CleanUpGeometry(cResult result);

			// Constructor and Destructor
			Mesh() = default;
			~Mesh();

			// constexpr void ConvertRightHandedToLeftHanded(GeometryData& pGeometryData);

			// Geometry Data
			// 

#if defined(EAE6320_PLATFORM_D3D)
			eae6320::Graphics::cVertexFormat* s_vertexFormat = nullptr;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
			// Index Buffer
			ID3D11Buffer* s_indexBuffer = nullptr;
#elif defined(EAE6320_PLATFORM_GL)
			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
			// A index buffer holds the index to refer to from vertex buffer for rendering
			GLuint s_IndexBufferId = 0;
#endif
			unsigned int numIndexes = 0;
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}

/*constexpr void eae6320::Graphics::Mesh::ConvertRightHandedToLeftHanded(GeometryData& pGeometryData) {
	for (uint16_t i = 0; i < pGeometryData.numVertices; ++i)
		pGeometryData.vertexData[i].z = -pGeometryData.vertexData[i].z;

	for (uint16_t i = 0; i < pGeometryData.numIndexes; i += 3) {

		uint16_t temp = pGeometryData.indexData[i + 1];
		pGeometryData.indexData[i + 1] = pGeometryData.indexData[i + 2];
		pGeometryData.indexData[i + 2] = temp;
	}
}*/

