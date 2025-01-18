#pragma once

#include "cShader.h"
#include "cRenderState.h"

#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <string>

namespace eae6320
{
	namespace Graphics
	{
		class Effect
		{
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(Effect)
		public:
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()

			// Public Methods
			static cResult Load(const std::string vertexShaderPath, const std::string fragmentShaderPath, Effect*& o_Effect);
			void BindShadingData();

		private:
			// Constructor and Destructor
			Effect() = default;
			~Effect();

			cResult LoadShaderFiles(const std::string vertexShaderPath, const std::string fragmentShaderPath);
			cResult InitializeShadingData(cResult result);
			void CleanUpShaderFiles(cResult result);
			void DeleteProgram(cResult result);

			// Shading Data
			//-------------

			eae6320::Graphics::cShader* s_vertexShader = nullptr;
			eae6320::Graphics::cShader* s_fragmentShader = nullptr;

#if defined(EAE6320_PLATFORM_GL)
		public:
				GLuint s_programId = 0;
#endif
		private:
				EAE6320_ASSETS_DECLAREREFERENCECOUNT()
				eae6320::Graphics::cRenderState s_renderState;
		};
	}
}
