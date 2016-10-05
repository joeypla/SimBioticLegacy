#ifndef SGAMESERVERMODEDM_H
#define SGAMESERVERMODEDM_H

#include "SGameServerMode.h"
class SGameServerMode_DM : public SGameServerMode
{
public:
	SGameServerMode_DM(void);
	~SGameServerMode_DM(void);
	int update(SGameServer* server);
	void spawnPlayer(SGameServer* server, SGameServerPlayer* player);
private:

};
#endif