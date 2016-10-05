#ifndef SLOBBYSERVER_H
#define SLOBBYSERVER_H

#include "SGameServer.h"
struct SLobbyServerClient
{
	std::string name;
	sockaddr_in address;
};
class SLobbyServer
{
public:
	SLobbyServer(void);
	~SLobbyServer(void);
	void run(void);
private:
	SGameServer* _gameServer;
	SLobbyServerClient* _clients[8];
protected:
};
#endif