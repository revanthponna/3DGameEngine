#include "../cEffect.h"
#include "Includes.h"

#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

namespace eae6320
{
	namespace Graphics
	{
		cResult Effect::InitializeShadingData(cResult result) {

			// Create a program
			eae6320::cScopeGuard scopeGuard_program([&result, this]
				{
					if (!result)
					{
						if (s_programId != 0)
						{
							glDeleteProgram(s_programId);
							const auto errorCode = glGetError();
							if (errorCode != GL_NO_ERROR)
							{
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
							}
							s_programId = 0;
						}
					}
				});
			
			{
				EAE6320_ASSERT(s_programId == 0)
				s_programId = glCreateProgram();
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
				else if (s_programId == 0)
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("OpenGL failed to create a program");
					return result;
				}
			}
			// Attach the shaders to the program
			{
				// Vertex
				{
					EAE6320_ASSERT((s_vertexShader != nullptr) && (s_vertexShader->m_shaderId != 0));
					glAttachShader(s_programId, s_vertexShader->m_shaderId);
					const auto errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return result;
					}
				}
				// Fragment
				{
					EAE6320_ASSERT((s_fragmentShader != nullptr) && (s_fragmentShader->m_shaderId != 0));
					glAttachShader(s_programId, s_fragmentShader->m_shaderId);
					const auto errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return result;
					}
				}
			}
			// Link the program
			{
				glLinkProgram(s_programId);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					// Get link info
					// (this won't be used unless linking fails
					// but it can be useful to look at when debugging)
					std::string linkInfo;
					{
						GLint infoSize;
						glGetProgramiv(s_programId, GL_INFO_LOG_LENGTH, &infoSize);
						const auto errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (infoSize > 0)
							{
								auto* const info = new (std::nothrow) GLchar[infoSize];
								if (info)
								{
									eae6320::cScopeGuard scopeGuard_info([info]
										{
											delete[] info;
										});
									constexpr GLsizei* const dontReturnLength = nullptr;
									glGetProgramInfoLog(s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info);
									const auto errorCode = glGetError();
									if (errorCode == GL_NO_ERROR)
									{
										linkInfo = info;
									}
									else
									{
										result = eae6320::Results::Failure;
										EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
										eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
											reinterpret_cast<const char*>(gluErrorString(errorCode)));
										return result;
									}
								}
								else
								{
									result = eae6320::Results::OutOfMemory;
									EAE6320_ASSERTF(false, "Couldn't allocate memory for the program link info");
									eae6320::Logging::OutputError("Failed to allocate memory for the program link info");
									return result;
								}
							}
						}
						else
						{
							result = eae6320::Results::Failure;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							return result;
						}
					}
					// Check to see if there were link errors
					GLint didLinkingSucceed;
					{
						glGetProgramiv(s_programId, GL_LINK_STATUS, &didLinkingSucceed);
						const auto errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							if (didLinkingSucceed == GL_FALSE)
							{
								result = eae6320::Results::Failure;
								EAE6320_ASSERTF(false, linkInfo.c_str());
								eae6320::Logging::OutputError("The program failed to link: %s",
									linkInfo.c_str());
								return result;
							}
						}
						else
						{
							result = eae6320::Results::Failure;
							EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
							eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
								reinterpret_cast<const char*>(gluErrorString(errorCode)));
							return result;
						}
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}

			return result;
		}

		void Effect::DeleteProgram(cResult result) {
			if (s_programId != 0)
			{
				glDeleteProgram(s_programId);
				const auto errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					if (result)
					{
						result = eae6320::Results::Failure;
					}
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_programId = 0;
			}
		}

		void Effect::BindShadingData() {
			{
				EAE6320_ASSERT(s_programId != 0);
				glUseProgram(s_programId);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
			// Render state
			{
				s_renderState.Bind();
			}
		}
	}
}