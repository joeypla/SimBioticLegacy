#ifndef SSGAME_H
#define SSGAME_H

#include "SInterface.h"

#include "SRender.h"
#include "GameInitInfo.h"
#include "Game.h"



#include "GameInitInfo.h"
class SSGame: public S_Interface{
public:
	SSGame(void);
	SSGame(ps::GameInitInfo* gii);
	~SSGame(void);

	bool updateInterface(DWORD lastTimeTaken);
	bool renderToDevice(LPDIRECT3DDEVICE9 dev);
	void receiveSystemMessage(UINT message, UINT param);


	bool init(ps::GameInitInfo* details);
private:

	ps::Game* _game;

	S2D*   startLevelBackground;
	SText* startLevelName;
	SText* startLevelMessage;
};

#endif