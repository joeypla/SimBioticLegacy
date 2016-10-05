#define _WINSOCKAPI_
#include "SMainMenu.h"
#include "Game.h"

#include "psInput.h"
#include "psPhysics.h"
#include "psLogic.h"
#include "psHUD.h"

#include "GMDeathmatch.h"
#include "GMSectorControl.h"

#include "psGameClient.h"
using namespace ps;

S3D* stairs;
#include "SAnimatedMesh.h"
#include "Weapon.h"

Game::Game(GameInitInfo* gii)
{
	/*assume not valid until we are done */
	_validState = false;

	/* start unpaused */
	_paused = false;

	/* shouldn't just quit */
	_shouldQuit = false;
	//what we do here is send this function the GameInitInfo so that it can what needs to be filled in from the server
	//can do so. This way, the rest of the stuff that operates will use the GII info fresh from the server.
	 
	WeaponBase::loadAllBaseWeapons();

	_pGameData = new GameData(gii);

	switch (gii->type)
	{
	case PS_GAMETYPE_DEATHMATCH:
		_pGameMode = new ps::GMDeathmatch(gii);
		break;
	case PS_GAMETYPE_SECTORCONTROL:
		_pGameMode = new ps::GMSectorControl(gii);
		break;
	default:
		_pGameMode = 0;
		break;
	}
	//Setup SubModules that need setup
	psHUD::setup();

	 

	//spawn all actors (beginning only) - dependant on game mode ofcourse
	_pGameMode->spawnAllActors(_pGameData);


	_pGameMainMenu = new GameMainMenu();
	
}

Game::~Game(void)
{
}
#include "SInput.h"
SLight* spotlightTest = 0;
bool Game::update(void)
{
	if (_shouldQuit) return false;


	//while the game is going on, the mouse should be invisible and not move
	if (!_paused) SetCursorPos(500,500);
	

	SGameClient::getSingleton()->update(_pGameData);//gets server data and updates from that

	if (!_paused) psInput::update(_pGameData);
	else {
		int lastMainMenuEvent = _pGameMainMenu->update();
		if (lastMainMenuEvent == GAMEMAINMENU_EVENT_RESUME) unpause();
		//if (lastMainMenuEvent == GAMEMAINMENU_EVENT_LEAVELOBBY);
		if (lastMainMenuEvent == GAMEMAINMENU_EVENT_QUIT) _shouldQuit = true;
	}


	psPhysics::update(_pGameData);

	psLogic::update(_pGameData);

	_pGameData->pGameMode()->update(_pGameData);

	psHUD::update(_pGameData);

	
	//again, like in the constructor, just for testing 
	static int test = 0;
	test++;
	//if (test > 0)
	//{
		SGameClient::getSingleton()->sendActorStatePacket(_pGameData->pClientActor());
		test = 0;
	//}
	

	//bool wallhack = S_INPUT::keyPressed(DIK_N);
		//for (int i = 0; i < 8; i++) _pGameData->pActorAtIndex(i)->setWallhacked(wallhack);


	
	return true;
}

void Game::receiveSystemMessage(UINT message, UINT param)
{
	if (message == WM_KEYDOWN && param == VK_ESCAPE)
	{
		_paused = !_paused;
		_pGameMainMenu->setFrameVisibility(_paused);
		ShowCursor(_paused);
	}

	if (_paused) _pGameMainMenu->receiveSystemMessage(message, param);
	else psInput::receiveSystemMessage(message, param);
}
bool Game::validState(void)
{
	return _validState;

}

void Game::pause(void)
{
	_paused = true;
	_pGameMainMenu->setFrameVisibility(_paused);
	ShowCursor(_paused);
}

void Game::unpause(void)
{
	_paused = false;
	_pGameMainMenu->setFrameVisibility(_paused);
	ShowCursor(_paused);
}

