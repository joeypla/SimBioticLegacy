#ifndef PS_GAME_INIT_INFO_H
#define PS_GAME_INIT_INFO_H
#include <string>

#define PS_GAMETYPE_DEATHMATCH     0
#define PS_GAMETYPE_TEAMDEATHMATCH 1
#define PS_GAMETYPE_SECTORCONTROL  2
#define PS_GAMETYPE_CAPTHEFLAG     3
namespace ps{
	/* All is public because the class can't really break. It is
	passed as an argument to setup the game, if any of the fields
	are invalid, then the setup will just fail. Maybe I should have
	just kept it a struct, but whatever, it boils down to the same
	thing */

	
	class GameInitInfo
	{
	public:
		GameInitInfo(void);
		~GameInitInfo(void);

		std::string                      ip;
		unsigned short                 port;
		int                clientActorIndex;
		int                 numberOfPlayers;
		std::string               levelPath;
		std::string               lobbyName;
		int                            type;
		std::string              screenName;
		bool                     bTimeLimit;
		bool                    bScoreLimit;
		unsigned long             timeLimit;
		int                      scoreLimit;
		unsigned long          prematchTime;
	};
}

#endif