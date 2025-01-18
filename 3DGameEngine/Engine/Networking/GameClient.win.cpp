#include "GameClient.h"
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void cGameClient::InitializeGameClient()
{
	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
	}

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		OutputDebugStringA("Can't create socket\n");
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		OutputDebugStringA("Can't connect to server\n");
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
	}
}

void cGameClient::UpdateGameClient(eae6320::Math::sVector playerPosition, eae6320::Math::sVector receivedPosition)
{
	int sendResult = send(sock, reinterpret_cast<char*>(&playerPosition), sizeof(eae6320::Math::sVector), 0);
	if (sendResult == SOCKET_ERROR)
	{
		OutputDebugStringA("Failed to send position\n");
		cerr << "Failed to send position, Err #" << WSAGetLastError() << endl;
	}

	// Wait for position updates from other players
	int bytesReceived = recv(sock, reinterpret_cast<char*>(&receivedPosition), sizeof(eae6320::Math::sVector), 0);
	if (bytesReceived > 0)
	{
		OutputDebugStringA("Received positions from server\n");
		// Display received position of another player
		cout << "Other Player Position - X: " << receivedPosition.x << ", Y: " << receivedPosition.y << ", Z: " << receivedPosition.z << endl;
		// Update positions in the game world as needed
	}
}

void cGameClient::CleanupGameClient()
{
	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}