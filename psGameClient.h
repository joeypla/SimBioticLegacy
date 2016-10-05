#ifndef PS_GAME_CLIENT_H
#define PS_GAME_CLIENT_H



#include "GameData.h"


//namespace psGameClient{
//	
//
//	//Connects to the central symbiote base server
//	bool connectToTestGameServer(void);
//	int connectToGameServer(ps::GameInitInfo* gii);
//	unsigned char getClientIndex(void);
//
//	//send GamePacket
//	
//	bool sendActorStatePacket(ps::Actor* actor);
//	void sendImmediateShotPacket(unsigned char shooterIndex, unsigned char victimIndex, unsigned char dmg);
//	//updates - listens and shit
//	void update(ps::GameData* pGameData);
//}


#define SGAMECLIENT_UPDATECONNECTION_NOTHING 0
#define SGAMECLIENT_UPDATECONNECTION_SUCCESSFULJOIN 1
#define SGAMECLIENT_UPDATECONNECTION_LOBBYFULL 2
#define SGAMECLIENT_UPDATECONNECTION_NORESPONSE 3
#define SGAMECLIENT_UPDATECONNECTION_STILLTRYING 4

class SGameClient
{
public:
	SGameClient(void);
	~SGameClient(void);
	static SGameClient* getSingleton(void);

	void sendNATPunchthroughRequest(unsigned int addr, unsigned short port);
	bool connectToGameServer(ps::GameInitInfo* gii);
	void hostServer(ps::GameInitInfo* gii);//This function feels especially out of place, but it sort of makes sense in a way

	bool sendActorStatePacket(ps::Actor* actor);
	void sendImmediateShotPacket(unsigned char shooterIndex, unsigned char victimIndex, unsigned char dmg);
	void sendAbilityUsePacket(SAbility* ability, ps::Actor* actor);

	void update(ps::GameData* pGameData);//if null, just connecting
	int updateConnection(void);//for connecting to a lobby, false if nothing, true if success
	void cancelConnecting(void);
	int getClientIndex(void);
	ps::GameInitInfo* getLastGameInitInfo(void);
private:
	static SGameClient* _singleton;

	ps::GameInitInfo _lastGii;
	bool _inProcessOfConnecting;
	DWORD _lastConnectCalled;
	SOCKET _mainSocket;
	sockaddr_in _localAddress;
	sockaddr_in _centralServerAddress;

	sockaddr_in _gameServerAddress;

	int _clientIndex;
protected:
};
#endif