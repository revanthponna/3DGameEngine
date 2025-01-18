#pragma once

#if defined( EAE6320_PLATFORM_D3D )
#include "Direct3D/Includes.h"
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL/Includes.h"
#endif

#include "Configuration.h"
#include "ColorHexCodes.h"
#include <Engine/Results/Results.h>
#include <Engine/Concurrency/cEvent.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
#include <Engine/Windows/Includes.h>
#endif

namespace eae6320
{
	namespace Graphics
	{
		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow = NULL;
#if defined( EAE6320_PLATFORM_D3D )
			uint16_t resolutionWidth = 0, resolutionHeight = 0;
#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication = NULL;
#endif
#endif
		};

		class RenderContext 
		{
		public:
			// Constructor and destructor
			RenderContext();
			~RenderContext();

			// Public Methods
			void ClearBuffers(unsigned int pHexCode);
			cResult InitializeViewsCheck(cResult result, const sInitializationParameters& i_initializationParameters);
			void SwapBuffer();
			void CleanUpViews();
		};
	}
}
