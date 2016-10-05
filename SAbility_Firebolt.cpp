#include "SGameServer.h"
#include "SAbility.h"

#include "SRender.h"
static const int NUMTIERS = 1;
static const int TIER1_FLUX = 250;

#include "Actor.h"
#include "GameData.h"


#include "SProjectile.h"
SAbility_Firebolt::SAbility_Firebolt(void)
{
	SAbility::SAbility();
}

SAbility_Firebolt::~SAbility_Firebolt(void)
{
	SAbility::~SAbility();
}

void SAbility_Firebolt::serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction)
{
	SProjectile* firebolt = (SProjectile*)pServerData->spawnObject(SGAMEOBJECT_NEWINSTANCE_FIREBOLT);
	firebolt->position = position;
	firebolt->velocity = direction;
	
}

void SAbility_Firebolt::serverUpdate(void)
{
}

void SAbility_Firebolt::serverOnImpact(void)
{
}

void SAbility_Firebolt::clientCast(ps::GameData* pGameData, ps::Actor* pCaster)
{
	SAbility::clientCast(pGameData, pCaster);

	
}

void SAbility_Firebolt::clientUpdate(void)
{
}

void SAbility_Firebolt::clientOnImpact(void)
{
}