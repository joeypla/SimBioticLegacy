#include "SAbility.h"

static const int NUMTIERS = 1;
static const int TIER1_FLUX = 500;

#include "SRender.h"

#include "GameData.h"
#include "GameConfigInterface.h"
SAbility_Wallhack::SAbility_Wallhack(void)
{
	SAbility::SAbility();
}

SAbility_Wallhack::~SAbility_Wallhack(void)
{
	SAbility::~SAbility();
}

void SAbility_Wallhack::serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction)
{
}

void SAbility_Wallhack::serverUpdate(void)
{
}

void SAbility_Wallhack::serverOnImpact(void)
{
}

void SAbility_Wallhack::clientCast(ps::GameData* pGameData, ps::Actor* pCaster)
{
	SAbility::clientCast(pGameData, pCaster);
	for (int i = 0; i < GlobalConfiguration::getSingleton()->iMaxPlayers(); i++) pGameData->pActorAtIndex(i)->setWallhacked(true);
}

void SAbility_Wallhack::clientUpdate(void)
{
}

void SAbility_Wallhack::clientOnImpact(void)
{
}