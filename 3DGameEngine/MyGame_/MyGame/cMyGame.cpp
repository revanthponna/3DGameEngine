// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/Graphics.h>
#include <Engine/Logging/Logging.h>
#include <Engine/UserInput/UserInput.h>

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	PressedKeyCode = UserInput::KeyCodes::None;

	// Is the user pressing the key?
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	{
		PressedKeyCode = UserInput::KeyCodes::Space;
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Tab))
	{
		PressedKeyCode = UserInput::KeyCodes::Tab;
	}
	else
	{
		activeGameObjects[0]->SetMesh(MeshEffectPairs[0].mesh);
	}

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		camera.AddVelocity(Math::sVector(0.0f, 0.0f, -playerVelocity));

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		camera.AddVelocity(Math::sVector(0.0f, 0.0f, playerVelocity));

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		camera.AddVelocity(Math::sVector(-playerVelocity, 0.0f, 0.0f));

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		camera.AddVelocity(Math::sVector(playerVelocity, 0.0f, 0.0f));

	if (UserInput::IsKeyPressed('W'))
		activeGameObjects[0]->AddVelocity(Math::sVector(0.0f, playerVelocity, 0.0f));

	if (UserInput::IsKeyPressed('A'))
		activeGameObjects[0]->AddVelocity(Math::sVector(-playerVelocity, 0.0f, 0.0f));

	if (UserInput::IsKeyPressed('S'))
		activeGameObjects[0]->AddVelocity(Math::sVector(0.0f, -playerVelocity, 0.0f));

	if (UserInput::IsKeyPressed('D'))
		activeGameObjects[0]->AddVelocity(Math::sVector(playerVelocity, 0.0f, 0.0f));
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(float i_elapsedSecondCount_sinceLastUpdate)
{
	for (Assets::GameObject* currObj : activeGameObjects)
	{
		currObj->Update(i_elapsedSecondCount_sinceLastUpdate);
	}

	camera.Update(i_elapsedSecondCount_sinceLastUpdate);
	eae6320::Math::sVector playerPosition = player->GetPosition();
	eae6320::Math::sVector receivedPosition;
	// Update GameClient
	gameClient.UpdateGameClient(playerPosition, receivedPosition);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	eae6320::cResult result;

	Logging::Initialize();

	Logging::OutputMessage("Initialization Successful");

	// Initialize GameClient
	gameClient.InitializeGameClient();

	// Initializing the shading data
	{
		if (!(result = eae6320::Graphics::Effect::Load("data/Shaders/Vertex/standard.shader", "data/Shaders/Fragment/constantColor.shader", MeshEffectPairs[0].effect)))
		{
			EAE6320_ASSERTF(false, "Can't initialize graphics without the shading data");
			return result;
		}
	}

	// Initializing the geometry
	{
		MeshEffectPairs[0].mesh = Graphics::Mesh::CreateMeshFromFile("data/meshes/player.mesh");

		if (!MeshEffectPairs[0].mesh)
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
			return eae6320::Results::Failure;
		}
	}

	activeGameObjects.reserve(10);

	player = new eae6320::Assets::GameObject(MeshEffectPairs[0].mesh, MeshEffectPairs[0].effect, Physics::sRigidBodyState());

	activeGameObjects.push_back(player);

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	while (!activeGameObjects.empty())
	{
		Assets::GameObject* currObj = activeGameObjects.back();
		activeGameObjects.pop_back();
		delete currObj;
	}

	for (unsigned int i = 0; i < numMeshEffectPairs; i++)
	{
		MeshEffectPairs[i].mesh->DecrementReferenceCount();
		MeshEffectPairs[i].mesh = nullptr;

		MeshEffectPairs[i].effect->DecrementReferenceCount();
		MeshEffectPairs[i].effect = nullptr;
	}

	Logging::CleanUp();

	Logging::OutputMessage("Clean Up Successful");

	// Cleanup GameClient
	gameClient.CleanupGameClient();

	return Results::Success;
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	Graphics::ObjectRenderData* renderableObjects = new Graphics::ObjectRenderData[activeGameObjects.size()];

	unsigned int i = 0;

	for (Assets::GameObject* currObj : activeGameObjects)
	{
		renderableObjects[i].mesh = currObj->GetMesh();
		renderableObjects[i].effect = currObj->GetEffect();
		renderableObjects[i].drawData.g_transform_localToWorld = currObj->GetTransformLocalToWorld(i_elapsedSecondCount_sinceLastSimulationUpdate);

		++i;
	}

	switch (PressedKeyCode) {
		case UserInput::KeyCodes::Space:
		
		default:
			// Setting background color to blue
			Graphics::SetBackgroundColor(Graphics::Blue);
			break;
	}

	Graphics::SetRenderableObjects(renderableObjects, (uint16_t)activeGameObjects.size());
	Graphics::SetCameraTransforms(camera.CreateWorldToCameraTransform(i_elapsedSecondCount_sinceLastSimulationUpdate), camera.CreateCameraToProjectedTransform_perspective());
}
