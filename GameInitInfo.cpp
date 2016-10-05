#include "GameInitInfo.h"

using namespace ps;

GameInitInfo::GameInitInfo(void)
{
	numberOfPlayers = 1;
	levelPath = "NoPath";
	type = 0;
	bTimeLimit = true;
	bScoreLimit = false;
	timeLimit = 36000;
	scoreLimit = 1500;
}

GameInitInfo::~GameInitInfo(void)
{
	//Nothing to do here
}