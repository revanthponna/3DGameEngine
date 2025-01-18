--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Fragment/animatedColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/constantColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},
	meshes = 
	{
		"Meshes/square.mesh", "Meshes/triangle.mesh", "Meshes/cone.mesh", "Meshes/ball.mesh", "Meshes/ring.mesh",
		"Meshes/helix.mesh", "Meshes/plane.mesh", "Meshes/player.mesh"
	},
}
