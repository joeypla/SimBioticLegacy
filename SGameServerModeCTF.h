#ifndef SGAMESERVERMODECTF_H
#define SGAMESERVERMODECTF_H

#include "SGameServerMode.h"
class SGameServerMode_CTF : public SGameServerMode
{
public:
	SGameServerMode_CTF(void);
	~SGameServerMode_CTF(void);
	int update(SGameServer* server);
	void spawnPlayer(SGameServer* server, SGameServerPlayer* player);
private:

};
#endif