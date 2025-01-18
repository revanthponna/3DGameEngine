#pragma warning(disable:4996)
#include "GameClient.h"
#include <Engine/Windows/Includes.h>
#include <Engine/Logging/Logging.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

struct sockaddr_in si_other;

#define SET(to, from, type) memcpy(&(to), &(from), sizeof(type)); index += sizeof(type)
#define INIT_INDEX(start) int index = start

void _PrintError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

void GameClient::InitializeNetworking()
{
	WSAData WSAData;
	int iResult;
	int Port = 2120;

	// Step #0 Initializing WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		PrintError();
		return;
	}
}

void GameClient::CleanupNetworking()
{
	closesocket(mServerSocket);
	WSACleanup();
}

void GameClient::SetNonBlockingSocket(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		PrintError();
		return;
	}
}

void GameClient::Recv()
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[SCENE_BUFFER_SIZE];
	std::cout << "Attempting recvfrom on socket: " << mServerSocket << " with buffer size: " << SCENE_BUFFER_SIZE << std::endl;
	int result = recvfrom(mServerSocket, buffer, SCENE_BUFFER_SIZE, 0, (struct sockaddr*)&si_other, &slen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			std::cout << "WSAEWOULDBLOCK: No data to read at this moment." << std::endl;
			return;
		}
		PrintError();
		memset(buffer, '\0', SCENE_BUFFER_SIZE);
		return;
	}

	// Logging the raw buffer content received
	/*std::cout << "Raw buffer content received: ";
	for (int i = 0; i < result; i++)
	{
		std::cout << std::hex << (int)buffer[i] << " ";
	}
	std::cout << std::dec << std::endl;  // Return to decimal output*/

	INIT_INDEX(0);

	unsigned int state = 0;
	SET(client_id, buffer[index], unsigned int);

	std::cout << "Client_id: " << client_id << std::endl;

	// Client side reconciliation
	SET(state, buffer[index], unsigned int);
	std::cout << "Parsed state: " << state << ", current state_id: " << state_id << std::endl;
	if (state < state_id)
	{
		std::cout << "Skipping packet due to stale state. Received state: " << state << ", Expected minimum: " << state_id << std::endl;
		return;
	}
	state_id = state;

	// Getting Player Data
	memcpy(&numPlayers, &(buffer[index]), sizeof(unsigned int));
	index += sizeof(unsigned int);
	std::cout << "Number of players received: " << numPlayers << std::endl;

	for (unsigned int i = 0; i < numPlayers; i++)
	{
		float x, y, z;
		char is_Alive;
		SET(is_Alive, buffer[index], char);
		SET(x, buffer[index], float);
		SET(y, buffer[index], float);
		SET(z, buffer[index], float);
		players[i].position.x = x;
		players[i].position.y = y;
		players[i].position.z = z;
		players[i].is_Alive = is_Alive;

		// Log each player's parsed position and state
		std::cout << "Player " << i << " - is_Alive: " << (int)is_Alive
			<< ", Position: (" << x << ", " << y << ", " << z << ")" << std::endl;
	}

	// Logging completed processing of the buffer
	std::cout << "Finished processing buffer for client_id: " << client_id << std::endl;
}

void GameClient::Send(char* data, int numBytes)
{
	char buffer[INPUT_BUFFER_SIZE];
	// std::cout << " Copying over to buffer" << std::endl;
	memset(buffer, '\0', INPUT_BUFFER_SIZE);

	memcpy(&(buffer[0]), &send_id, sizeof(unsigned int));
	++send_id;

	for (int i = 0; i < numBytes; i++)
	{
		buffer[4 + i] = data[i];
	}

	int result = sendto(mServerSocket, buffer, INPUT_BUFFER_SIZE, 0, (struct sockaddr*)&si_other, sizeof(si_other));
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		PrintError();
		exit(1);
	}

	if (result == 0)
	{
		printf("Disconnected...\n");
		exit(1);
	}
}

void GameClient::SetSocketOptions(std::string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == INVALID_SOCKET)
	{
		PrintError();
		return;
	}

	std::cout << "Socket created: " << mServerSocket << std::endl;
	// Setup si_other with server information for sending data to the server
	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);                // Server's listening port
	inet_pton(AF_INET, ip.c_str(), &si_other.sin_addr);

	SetNonBlockingSocket(mServerSocket);

	std::cout << "Socket bound to IP: " << ip << " on Port: " << port << std::endl;
	/*for (size_t i = 0; i < players.size(); i++)
	{
		std::cout << "Client Player " << i << " Position: "
			<< players[i].position.x << ", "
			<< players[i].position.y << ", "
			<< players[i].position.z << std::endl;
	}*/
}
