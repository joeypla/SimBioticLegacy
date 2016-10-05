#include "GameMode.h"

#include "GMDeathmatch.h"
#include "GMSectorControl.h"

ps::GameMode* ps::GameMode::new_Gamemode(const ps::GameInitInfo* gii)
{
	if (gii->type == 0) return new ps::GMDeathmatch(gii);
	if (gii->type == 2) return new ps::GMSectorControl(gii);
}

int ps::GameMode::update(ps::GameData* pGameData)
{
	return 0;
}

void ps::GameMode::respawnActor(ps::Actor* actor, ps::GameData* pGameData)
{
}

void ps::GameMode::spawnAllActors(ps::GameData* pGameData)
{
}


ps::GameMode::~GameMode(void)
{
}

SLeaderboard* ps::GameMode::pLeaderboard(void)
{
	return _pLeaderboard;
}