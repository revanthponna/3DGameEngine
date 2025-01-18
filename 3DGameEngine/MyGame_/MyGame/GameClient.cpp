#include "GameClient.h"
#include <iostream>
#include <Engine/Math/Functions.h>

GameClient* GameClient::Instance()
{
	static GameClient instance;
	return &instance;
}

GameClient::GameClient()
{
	players.resize(8);
	InitializeNetworking();
}

GameClient::~GameClient()
{
	CleanupNetworking();
}

void GameClient::CreateSocket(std::string ip, int port)
{
	SetSocketOptions(ip, port);
}

void GameClient::Update()
{
	Recv();
}

void GameClient::SendToServer(char* data, int numBytes)
{
	Send(data, numBytes);
}