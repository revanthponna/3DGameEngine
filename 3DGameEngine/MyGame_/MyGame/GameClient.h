#pragma once

#include <string>
#include <vector>
#include <Engine/Math/sVector.h>
#include <Engine/Assets/GameObject.h>
typedef unsigned __int64 SOCKET;

#if defined(EAE6320_PLATFORM_WINDOWS)
#include <Engine/Windows/Includes.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif

#define SCENE_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 16

void _PrintError(const char* file, int line);
#define PrintError() _PrintError(__FILE__, __LINE__)

class GameObject;

struct Player
{
	char is_Alive;
	eae6320::Math::sVector position;
};

class GameClient
{
public:
	GameClient();
	~GameClient();

	static GameClient* Instance();

	void Update();
	void SetPosition(int id, float& x, float& y, float& z, float delta_time);
	void CreateSocket(std::string ip, int port);
	void SendToServer(char* data, int numBytes);

	std::vector<Player> players;
	std::vector<eae6320::Assets::GameObject*>* playerObjects;

	int client_id = 0;
	unsigned int numPlayers = 0;
	unsigned int send_id = 0;
	unsigned int state_id = 0;

private:

	// Platform-specific networking functions
	void SetSocketOptions(std::string ip, int port);
	void Send(char* data, int numBytes);
	void InitializeNetworking();
	void CleanupNetworking();
	void SetNonBlockingSocket(SOCKET socket);
	void Recv();

#if defined(EAE6320_PLATFORM_WINDOWS)
	SOCKET mServerSocket;
#endif
};

