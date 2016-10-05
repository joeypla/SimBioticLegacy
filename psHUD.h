#ifndef PS_HUD_H
#define PS_HUD_H

#include "GameData.h"

namespace psHUD{
	void setup(void);
	void update(ps::GameData* gameData);
	void award(int amount, const wchar_t* info);
	void showLastEnemy(ps::Actor* lastEnemy); 
	void resetHitmarker(void);
};
#endif