#include "SymbioteWinsock.h"

#include "SLOG.h"

#include <WinSock2.h>
//#include <windows.h>
static WSADATA winsock;
bool psNetworking::init(void)
{
	WSAStartup(MAKEWORD(2, 2), &winsock);    // Start Winsock
	 
	if(LOBYTE(winsock.wVersion) != 2 ||  HIBYTE(winsock.wVersion) != 2)    // Check version
	{
		WSACleanup();
		S_LOG::write("Winsock not initialized due to wrong version");
		return false;
	}
	S_LOG::write("Winsock initialized.");
	return true;
	
}

bool psNetworking::destroy(void)
{
	WSACleanup();
	return true;
}