#include "cEffect.h"
#include "sContext.h"
#include "cShader.h"
#include "cRenderState.h"

#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Graphics::Effect::Load(const std::string vertexShaderPath, const std::string fragmentShaderPath, Effect*& o_Effect)
{
	sizeof(Effect);
	auto result = Results::Success;

	Effect* newEffect = nullptr;
	cScopeGuard scopeGuard([&o_Effect, &result, &vertexShaderPath, &fragmentShaderPath, &newEffect]
		{
			if (result)
			{
				EAE6320_ASSERT(newEffect != nullptr);
				o_Effect = newEffect;
			}
			else
			{
				if (newEffect)
				{
					newEffect->DecrementReferenceCount();
					newEffect = nullptr;
				}
				o_Effect = nullptr;
			}
		});

	// Allocating an effect
	{
		newEffect = new (std::nothrow) Effect();
		if (!newEffect)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect");
			Logging::OutputError("Failed to allocate memory for the effect");
			return result;
		}
	}

	// Initializing the effect with shaders
	if (!(result = newEffect->LoadShaderFiles(vertexShaderPath, fragmentShaderPath)))
	{
		EAE6320_ASSERTF(false, "Initialization of new effect failed");
		return result;
	}

	return result;
}

eae6320::Graphics::Effect::~Effect()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	eae6320::cResult result = eae6320::Results::Success;
	CleanUpShaderFiles(result);
	EAE6320_ASSERT(result);
}

eae6320::cResult eae6320::Graphics::Effect::LoadShaderFiles(const std::string vertexShaderPath, const std::string fragmentShaderPath) {
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(vertexShaderPath,
		s_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	else {
		eae6320::Logging::OutputMessage("Vertex shader successfully loaded");
	}
	if (!(result = eae6320::Graphics::cShader::Load(fragmentShaderPath,
		s_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	else {
		eae6320::Logging::OutputMessage("Fragment shader successfully loaded");
	}
	{
		constexpr auto renderStateBits = []
			{
				uint8_t renderStateBits = 0;

				eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
				eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
				eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
				eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

				return renderStateBits;
			}();
		if (!(result = s_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

	// OpenGL specific initialization code
	result = InitializeShadingData(result);

	return result;
}

void eae6320::Graphics::Effect::CleanUpShaderFiles(cResult result) {

	if (s_vertexShader)
	{
		s_vertexShader->DecrementReferenceCount();
		s_vertexShader = nullptr;
	}
	if (s_fragmentShader)
	{
		s_fragmentShader->DecrementReferenceCount();
		s_fragmentShader = nullptr;
	}

	DeleteProgram(result);
}