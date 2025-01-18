#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#include <Engine/Math/sVector.h>

class cGameServer
{
public:
	void InitializeGameServer();
	void UpdateGameServer();
	void CleanupGameServer();

private:
#if defined (EAE6320_PLATFORM_WINDOWS)
	SOCKET listening;
	fd_set master;
#endif
};
