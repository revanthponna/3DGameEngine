/*
	This is a custom fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

// Constant Buffers
//=================

DeclareConstantBuffer (g_constantBuffer_frame, 0)
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
};

// Entry Point
//============

#if defined (EAE6320_PLATFORM_GL)
out vec4 o_color;
#endif

void main(
#if defined (EAE6320_PLATFORM_D3D)

	// Input
	//======

	in const float4 i_fragmentPosition : SV_POSITION,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : SV_TARGET
#endif
)
{
	// Calculate animated color values using sine and cosine functions
	float red = 0.5 + 0.5 * sin(g_elapsedSecondCount_simulationTime);
	float green = 0.5 + 0.5 * cos(g_elapsedSecondCount_simulationTime);
	float blue = 0.5 + 0.5 * sin(g_elapsedSecondCount_simulationTime * 0.5);

	// Output animated color
	o_color = float4(
		// RGB (color)
		red, green, blue,
		// Alpha (opacity)
		1.0 );
}
