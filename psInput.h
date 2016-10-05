#ifndef PS_INPUT_H
#define PS_INPUT_H

#include "GameData.h"

namespace psInput{
	void update(ps::GameData* gameData);
	void receiveSystemMessage(UINT message, UINT param);
};
#endif