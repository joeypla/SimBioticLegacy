#include "GMDeathmatch.h"

#include "SInput.h"
#include <algorithm>



ps::GMDeathmatch::GMDeathmatch(const ps::GameInitInfo* gii)
{
	_pLeaderboard = 0;
	//init leaderboard
	LeaderboardInitData lid;
	lid.gameMode = "Deathmatch";
	lid.mapName = "Birth";
	lid.teamColors[0] = 0xffff0000;
	lid.teamColors[1] = 0xff0000ff;
	lid.numValues = 3;
	lid.valueNames[0] = L"Kills";
	lid.valueNames[1] = L"Deaths";
	lid.valueNames[2] = L"Ratio";
	lid.valueNames[3] = L"NONE";
	lid.valueSources[0] = LEADERBOARD_INFOTYPE_SOURCE_KILLS;
	lid.valueSources[1] = LEADERBOARD_INFOTYPE_SOURCE_DEATHS;
	lid.valueSources[2] = LEADERBOARD_INFOTYPE_SOURCE_RATIO;
	_pLeaderboard = new SLeaderboard(&lid);
	//Set name of gamemode
	modeName = "Deathmatch";
	


	

	
}

int ps::GMDeathmatch::update(ps::GameData* pGameData)
{
	_pLeaderboard->update(pGameData);
	return 0;
}

void ps::GMDeathmatch::respawnActor(ps::Actor* actor, ps::GameData* pGameData)
{
}

void ps::GMDeathmatch::spawnAllActors(ps::GameData* pGameData)
{
	
	//We are going to sync the local actor data here with the game actor data
	//in this function. I know this doesn't seem EXACTLY like an init function,
	//but i needed access to game data and didn't feel like having a completely
	//new function to do that. This seems alright for now.
	
	//extract all spawns
	ps::World* wData = pGameData->pWorldData();
	int totalSpawns = 0;
	//create a list of spawns thats larger than the actual spawns, but only use whats needed.
	ps::SetPoint** extractedSpawnPoints = new ps::SetPoint*[wData->setPointCount()];
	for (int i = 0; i < wData->setPointCount(); i++)
	{
		if (wData->getSetPointAtIndex(i) != 0)
		{
			if (wData->getSetPointAtIndex(i)->use >= 0 && wData->getSetPointAtIndex(i)->use < 12)
			{
				extractedSpawnPoints[totalSpawns] = wData->getSetPointAtIndex(i);
				totalSpawns++;
			}
		}
	}


	
	int actorCount = pGameData->actorCount();
	int spawnIndex = rand() % totalSpawns;
	for (int i = 0; i < actorCount; i++)
	{
		//spawn actor at set point
		ps::Actor* actor = pGameData->pActorAtIndex(i);
		actor->spawn(pGameData, extractedSpawnPoints[spawnIndex]);
		spawnIndex++;
		if (spawnIndex >= totalSpawns) spawnIndex = 0;


	} 

}
ps::GMDeathmatch::~GMDeathmatch(void)
{
}
