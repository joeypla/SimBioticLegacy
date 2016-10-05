#ifndef PS_LOGIC_H
#define PS_LOGIC_H

#include "GameData.h"
namespace psLogic{
	void update(ps::GameData* gameData);
	void awardActorPoints(ps::Actor* actor, int points);
	void actorFireWeapon(ps::Actor* actor, ps::GameData* pGameData);
};

#endif