#include "SAbility.h"

#include "SRender.h"
static const int NUMTIERS = 1;
static const int TIER1_FLUX = 250;

#include "Actor.h"
#include "GameData.h"
SAbility_Blink::SAbility_Blink(void)
{
	SAbility::SAbility();
}

SAbility_Blink::~SAbility_Blink(void)
{
	SAbility::~SAbility();
}

void SAbility_Blink::serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction)
{
}

void SAbility_Blink::serverUpdate(void)
{
}

void SAbility_Blink::serverOnImpact(void)
{
}

void SAbility_Blink::clientCast(ps::GameData* pGameData, ps::Actor* pCaster)
{
	SAbility::clientCast(pGameData, pCaster);

	pCaster->x += pCaster->view()->dx * 20.0f;
	pCaster->y += pCaster->view()->dy * 20.0f;
	pCaster->z += pCaster->view()->dz * 20.0f;
}

void SAbility_Blink::clientUpdate(void)
{
}

void SAbility_Blink::clientOnImpact(void)
{
}