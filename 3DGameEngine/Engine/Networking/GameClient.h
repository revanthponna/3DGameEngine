#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <Engine/Math/sVector.h>

class cGameClient
{
public:
	void InitializeGameClient();
	void UpdateGameClient(eae6320::Math::sVector playerPosition, eae6320::Math::sVector receivedPosition);
	void CleanupGameClient();

private:
#if defined (EAE6320_PLATFORM_WINDOWS)
	SOCKET sock;
#endif
};
