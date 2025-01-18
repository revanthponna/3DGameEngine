#include "GameServer.h"

using namespace std;

int main()
{
	cGameServer gameServer = cGameServer();
	gameServer.InitializeGameServer();
	gameServer.UpdateGameServer();
	gameServer.CleanupGameServer();
	return 0;
}