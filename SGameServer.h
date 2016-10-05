#ifndef SGAMESERVER_H
#define SGAMESERVER_H

#include <string>

#include "SGameServerPlayer.h"
#include "SGameServerMode.h"
#include "SGameServerWorld.h"
#include "SGameObject.h"
#include <map>
#include <deque>
#define SGAMESERVER_MAXPLAYERS 8
#define SGAMESERVER_PORT 12371 





struct SGameServerInitInfo
{
	//std::string mapPath;
	//std::string hostName;
	char mapPath[64];
	char hostName[32];
	char lobbyName[32];
	int gameMode;
	int scoreLimit;
	int timeLimit;
	int preMatchTime;
	int spawnDelay;
};
class SGameServer
{
public:
	SGameServer(SGameServerInitInfo* data);
	~SGameServer(void);
	bool run(void);
	static SGameServerInitInfo* pGlobalInitInfo(void);
	SGameServerPlayer* pPlayerAtIndex(int index);
	SGameServerWorld* pWorld(void);

	int scoreLimit(void);

	SGameObject* spawnObject(int templateNumber);
private:
	static SGameServerInitInfo _globalInitInfo;
	//To know that there hasn't been any problems
	bool _usable;
	//Server specific networking members
	sockaddr_in centralNetServerAddress;
	SOCKET socketUDP;
	SOCKET socketTCP;


	std::string _lobbyName;

	std::string _mapPath;
	int _gameMode;
	int _scoreLimit;
	int _timeLimit;
	int _preMatchTime;
	int _spawnDelay;
	

	unsigned char _gameState;
	unsigned int _time;
	SGameServerPlayer* _players[SGAMESERVER_MAXPLAYERS];

	unsigned int currentObjectID;
	std::map<int, SGameObject*>::iterator outDataIndex;
	std::map<unsigned int, SGameObject*> _objects;
	std::deque<SGameObject*> _newObjects;

	SGameServerMode* _mode;
	SGameServerWorld* _world;
	DWORD _timeWhenStarted;//time used in pretty much everything. Gets reset after the prematch

	void updateLogic(void);
	void checkForInactivePlayers(void);
	int getPlayerCount(void);

	void acceptNextConnection(void);
	void sendNetPresencePacket(void);
	void receiveUDPPacket(void);
	void processUDPPacket_connectionRequest(psNetworking::ClientOutPacket_UDP_ConnectRequest* packet, sockaddr_in* source);
	void processUDPPacket_playerUpdate(psNetworking::ClientOutPacket_UDP* packet);
	void processUDPPacket_playerHit(psNetworking::ClientOutPacket_UDP* packet);
	void processUDPPacket_playerUseAbility(psNetworking::ClientOutPacket_UDP* packet);
	void processUDPPacket_udpPunchthrough(psNetworking::SimBioticNetServerOut_PunchthroughRequest* packet);
	void sendOutSnapshotToPlayers(void);
	void sendOutDisplayNameUpdateToPlayers(void);
};

DWORD WINAPI SGameServerThread(LPVOID lpParam);
#endif