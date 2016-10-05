#ifndef PS_GAME_H
#define PS_GAME_H

#include "GameInitInfo.h"
#include "GameData.h"
#include "GameMode.h"

#include "GameMainMenu.h"
namespace ps
{
	class Game{
	public:
		Game(ps::GameInitInfo* gii);
		~Game(void);

		bool update(void);
		void receiveSystemMessage(UINT message, UINT param);

		bool validState(void);

		void pause(void);
		void unpause(void);
	private:
		bool _validState;

		bool _shouldQuit;
		bool _paused;//obviously you cant pause multiplayer, but this is whether or not input go to game basically

		GameData* _pGameData;

		GameMode* _pGameMode;

		GameMainMenu* _pGameMainMenu;
	};
};
#endif
