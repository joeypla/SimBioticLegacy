#ifndef SGAMESERVERPLAYER_H
#define SGAMESERVERPLAYER_H

#include <string>



//#include "HeaderGuardedWinsock2.h"
#include <WinSock2.h>


#include "SymbiotePacketStructs.h"
class SCharacter;
class SGameServerPlayer
{
public:
	SGameServerPlayer(std::string name, sockaddr_in address);
	~SGameServerPlayer(void);

	void updateFromPacket(psNetworking::ClientOutPacket_PlayerUpdate_UDP* player);
	void update(void);

	unsigned char state;
	unsigned char state2;
	float x, y, z;
	float dirx, diry, dirz;
	unsigned char gravity;
	unsigned char health, shield;

	unsigned char kills, deaths;
	unsigned int score;
	unsigned char modeSpecificVal1;
	unsigned char modeSpecificVal2;
	unsigned char modeSpecificVal3;

	unsigned char shotByLast;

	DWORD timeOfLastDeath;//used for the spawn delay.

	sockaddr_in* pAddress(void);

	std::string name(void);

	bool getActive(void);

	void increaseFlux(int amount);
	void zeroFlux(void);
	int getFlux(void);

	unsigned char team;
	SCharacter* getCharacter(void);
private:
	int _flux;
	std::string _name;
	//Networking members
	SOCKET _socketTCP;
	sockaddr_in _address;
	DWORD _lastSeen;
	//Actual player properties

	
	SCharacter* _pCharacter;

	
};
#endif 