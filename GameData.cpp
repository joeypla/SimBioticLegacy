#include "GameData.h"

using namespace ps;


#include "GameMode.h"


#include "SGameStateDefines.h"

#include "SymbiotePacketStructs.h"
GameData::GameData(GameInitInfo* gii)
{
	gameState = SGAMESTATE_PLAYING;
	_pWorldData = World::new_World(gii->levelPath);

	_actorCount = 8;
	_actorData = new Actor*[8];
	
	for (int i = 0; i < _actorCount; i++)
		_actorData[i] = new Actor(false);
	
	//THis should be moved somewhere else... kinda awkward.
	_clientActorIndex = gii->clientActorIndex;
	_actorData[_clientActorIndex]->isPlayer = true;
	std::wstring clientName(gii->screenName.begin(), gii->screenName.end());
	_actorData[_clientActorIndex]->setName(clientName.c_str());

	SRender::getSingleton()->setCamera(_actorData[gii->clientActorIndex]->view());

	/* Now find game mode and create the gamemode object */
	_pGameMode = GameMode::new_Gamemode(gii);

	
	_minutesLeft = 0;
	_secondsLeft = 0;

}

GameData::~GameData(void)
{

}

World* GameData::pWorldData(void)
{
	return _pWorldData;
}

int GameData::actorCount(void)
{
	return _actorCount;
}
Actor* GameData::pActorAtIndex(int index)
{
	if (index >= _actorCount || index < 0) return 0;

	return _actorData[index];

}

Actor* GameData::pClientActor(void)
{
	
	return pActorAtIndex(_clientActorIndex);
}
ps::GameMode* GameData::pGameMode(void)
{
	return _pGameMode;
}

std::map<unsigned int, SGameObject*>& GameData::getObjectData(void)
{
	return _objectData;
}
#include "SLOG.h"
void GameData::addOrModifyObject(psNetworking::ServerOutGameObject* pData)
{
	
	//First see if the object exists
	unsigned int ID = pData->id;
	unsigned int templateNumber = pData->objectTemplate;
	std::map<unsigned int, SGameObject*>::iterator it = _objectData.find(ID);

	SGameObject* object = 0;
	if (it == _objectData.end())
	{
		object = SGameObject::new_instance(templateNumber, false, ID);
		_objectData.insert(std::pair<unsigned int, SGameObject*>(object->getID(), object));
	}
	else object = it->second;


	if (object != 0)
		object->clientDeserializeData(pData);
}

unsigned char GameData::getMinutesLeft(void){return _minutesLeft;}
unsigned char GameData::getSecondsLeft(void){return _secondsLeft;}

void GameData::setMinutesLeft(unsigned char minutes){_minutesLeft = minutes;}
void GameData::setSecondsLeft(unsigned char minutes){_secondsLeft = minutes;}