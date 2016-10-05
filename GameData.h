#ifndef PS_GAME_DATA_H
#define PS_GAME_DATA_H

#include "World.h"
#include "Actor.h"
#include "SLeaderboard.h"
#include "GameInitInfo.h"
#include "SGameObject.h"
#include <map>
namespace ps{
	class GameMode;

	class GameData{
	public:
		GameData(GameInitInfo* gii);
		~GameData(void);

		bool validState(void);
		int gameState;
		
		World* pWorldData();

		
		int actorCount(void);
		Actor* pActorAtIndex(int index);
		Actor* pClientActor(void);

		ps::GameMode* pGameMode(void);

		std::map<unsigned int, SGameObject*>& getObjectData(void);
		void addOrModifyObject(psNetworking::ServerOutGameObject* pData);

		unsigned char getMinutesLeft(void);
		unsigned char getSecondsLeft(void);

		void setMinutesLeft(unsigned char);
		void setSecondsLeft(unsigned char);
	private:
		
		bool _validState;
		World* _pWorldData;

		int _clientActorIndex;//this is the index that the player on this computer is on the server
		//and everywhere. All actor data sets are identical across all machines. I know, its awesome.
		int _actorCount;
		Actor** _actorData;

		std::map<unsigned int, SGameObject*> _objectData;

		ps::GameMode* _pGameMode;

		SLeaderboard* _pLeaderboard;

		unsigned char _minutesLeft;
		unsigned char _secondsLeft;
	};
};

#endif