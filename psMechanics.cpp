#include "psMechanics.h"

/* only to use to view (camera) */
#include "SRender.h"
/*==============================*/
void psMechanics::update(ps::GameData* pGameData)
{
	/* update automatic shooting for all players */
	for (int i = 0; i < pGameData->actorCount(); i++)
	{
		ps::Actor* a = pGameData->pActorAtIndex(i);
		//if (a->
	}
}

void psMechanics::fireWeaponRay(ps::Actor* shooter, ps::GameData* pGameData)
{
	SCamera* view = shooter->view();
}