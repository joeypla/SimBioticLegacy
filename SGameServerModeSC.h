#ifndef SGAMESERVERMODESC_H
#define SGAMESERVERMODESC_H

#include "SGameServerMode.h"
class SGameServerMode_SC : public SGameServerMode
{
public:
	SGameServerMode_SC(void);
	~SGameServerMode_SC(void);
	int update(SGameServer* server);
	void spawnPlayer(SGameServer* server, SGameServerPlayer* player);

	
private:
	int score1;
	int score2;

	//balance starts at the extreme of the owner (0 team 1, 100 neutral, 200 team 2)

	int balanceA;
	int balanceB;
	int balanceC;
	

	unsigned char ownerA;
	unsigned char ownerB;
	unsigned char ownerC;

	
};
#endif