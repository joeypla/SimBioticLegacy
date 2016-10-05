#ifndef PS_MECHANICS_H
#define PS_MECHANICS_H

#include "GameData.h"
namespace psMechanics
{
	void update(ps::GameData* pGameData);

	void fireWeaponRay(ps::Actor* shooter, ps::GameData* pGameData);
};
#endif