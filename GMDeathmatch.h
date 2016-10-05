#pragma once

#ifndef GMDEATHMATCH_H
#define GMDEATHMATCH_H

#include "GameInitInfo.h"
#include "GameMode.h"
namespace ps{
	struct GM_DEATHMATCH_ACTOR
	{
		int index;
		std::wstring name;
		int kills;
		int deaths;
		int score;

		bool operator < (const GM_DEATHMATCH_ACTOR& actor) const;
	};
	class GMDeathmatch : public GameMode
	{
	public:
		GMDeathmatch(const ps::GameInitInfo* gii);
		~GMDeathmatch(void);

		int update(ps::GameData* pGameData);
		void respawnActor(ps::Actor* actor, ps::GameData* pGameData);
		void spawnAllActors(ps::GameData* pGameData);
	private:
		std::vector<GM_DEATHMATCH_ACTOR*> actorData;//actor data is limited data required for game mode that needs to be synced up with the original
	};
};
#endif