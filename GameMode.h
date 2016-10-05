#ifndef PS_GAME_MODE_H
#define PS_GAME_MODE_H

#include "GameData.h"
#include "SLeaderboard.h"
namespace ps
{
	class GameMode
	{
	public:
		static GameMode* new_Gamemode(const ps::GameInitInfo* gii);
		virtual ~GameMode(void);

		virtual int update(ps::GameData* pGameData);
		virtual void respawnActor(ps::Actor* actor, ps::GameData* pGameData);
		virtual void spawnAllActors(ps::GameData* pGameData);

		std::string modeName;

		int teamScores[6];
		SLeaderboard* pLeaderboard(void);
	protected:
		SLeaderboard* _pLeaderboard;
	};
};
#endif