#ifndef SGAMESERVERMODE_H
#define SGAMESERVERMODE_H

class SGameServer;
class SGameServerPlayer;

class SGameServerMode
{
public:
	SGameServerMode(void);
	~SGameServerMode(void);
	virtual int update(SGameServer* data);
	virtual void spawnPlayer(SGameServer* data, SGameServerPlayer* player);
private:
};
#endif