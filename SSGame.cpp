#include "SSGame.h"
#include "SLOG.h"

#include "SAbility.h"
#include "SCharacter.h"
SSGame::SSGame(void)
{
	ps::GameInitInfo gii;
	gii.type = 0;
	gii.numberOfPlayers = 4;
	gii.levelPath = "../Resources/Levels/Beach/beach.psm";
	gii.clientActorIndex = 1;

	init(&gii);
}

SSGame::SSGame(ps::GameInitInfo* gii)
{
	
	init(gii);
}

SSGame::~SSGame(void)
{
}

#include "SInput.h"
bool SSGame::updateInterface(DWORD lastTimeTaken)
{
	if (S_INPUT::keyPressed(DIK_K)) return false;//the safe key. Its not always easy to quit fast ;)
	if (!_game->update()) return false;
	return true;
}

bool SSGame::renderToDevice(LPDIRECT3DDEVICE9 dev)
{
	SRender::getSingleton()->render();
	return true;
}

void SSGame::receiveSystemMessage(UINT message, UINT param)
{
	_game->receiveSystemMessage(message, param);
}

bool SSGame::init(ps::GameInitInfo* gii)
{
	SAbility::loadAbilities();

	SCharacter::loadCharacters();

	SGameObject::loadGameObjectTemplates();


	_game = new ps::Game(gii);
	if (_game->validState()) return true;
	return false;
}