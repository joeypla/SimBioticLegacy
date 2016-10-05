#ifndef SGAMESERVERMODETDM_H
#define SGAMESERVERMODETDM_H

#include "SGameServerMode.h"
class SGameServerMode_TDM : public SGameServerMode
{
public:
	SGameServerMode_TDM(void);
	~SGameServerMode_TDM(void);
	int update(SGameServer* server);
	void spawnPlayer(SGameServer* server, SGameServerPlayer* player);
private:

};
#endif