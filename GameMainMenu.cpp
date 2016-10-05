#include "GameMainMenu.h"

static GameMainMenu* lastInstance = 0;

static void resumeClick(void);
static void loadoutConfigClick(void);
static void leaveLobbyClick(void);
static void quitClick(void);

#include "Game.h"
GameMainMenu::GameMainMenu(void)
{
	
	lastInstance = this;

	_frame = new psGui::Frame(-0.8, 0.0f, 0.3f, 1.4f);
	_frame->setColor(0x9f1f1f1f);

	_resumeButton = new psGui::SimpleButton();
	_resumeButton->setSize(0.26f, 0.07f);
	_resumeButton->setRelativePosition(0.0f, 0.2f);
	_resumeButton->setColor(0x4f3f3f3f);
	_resumeButton->setCaptionSize(7);
	_resumeButton->setCaptionColor(0xffffffff);
	_resumeButton->setCaption(L"Resume");
	_resumeButton->setFunctionPointer(resumeClick);
	_frame->addComponent(_resumeButton);

	_loadoutConfigButton = new psGui::SimpleButton();
	_loadoutConfigButton->setSize(0.26f, 0.07f);
	_loadoutConfigButton->setRelativePosition(0.0f, 0.1f);
	_loadoutConfigButton->setColor(0x4f3f3f3f);
	_loadoutConfigButton->setCaptionSize(7);
	_loadoutConfigButton->setCaptionColor(0xffffffff);
	_loadoutConfigButton->setCaption(L"Loadout...");
	_loadoutConfigButton->setFunctionPointer(resumeClick);
	_frame->addComponent(_loadoutConfigButton);

	_leaveLobbyButton = new psGui::SimpleButton();
	_leaveLobbyButton->setSize(0.26f, 0.07f);
	_leaveLobbyButton->setRelativePosition(0.0f, 0.0f);
	_leaveLobbyButton->setColor(0x4f3f3f3f);
	_leaveLobbyButton->setCaption(L"Leave Lobby");
	_leaveLobbyButton->setCaptionSize(7);
	_leaveLobbyButton->setCaptionColor(0xffffffff);
	_leaveLobbyButton->setFunctionPointer(leaveLobbyClick);
	_frame->addComponent(_leaveLobbyButton);

	_quitButton = new psGui::SimpleButton();
	_quitButton->setSize(0.26f, 0.07f);
	_quitButton->setRelativePosition(0.0f, -0.1f);
	_quitButton->setColor(0x4f3f3f3f);
	_quitButton->setCaption(L"Quit");
	_quitButton->setCaptionSize(7);
	_quitButton->setCaptionColor(0xffffffff);
	_quitButton->setFunctionPointer(quitClick);
	_frame->addComponent(_quitButton);

	_frame->setPosition(-0.8f, 0.0f);

	_loadoutMenu = new GameLoadoutMenu();
}

GameMainMenu::~GameMainMenu(void)
{
	delete _frame;
	delete _resumeButton;
	delete _leaveLobbyButton;
	delete _quitButton;
}


void GameMainMenu::receiveSystemMessage(UINT message, UINT param)
{
	if (_frame->getVisible()) _frame->sendSystemMessage(message, param);
}

int GameMainMenu::update(void)
{
	if (_frame->getVisible()) _frame->update();

	if (actions.size() < 1) return 0;

	int actionToSend = actions.front();
	actions.pop_front();
	return actionToSend;
}

void GameMainMenu::setFrameVisibility(bool visibility)
{
	_frame->setVisible(visibility);
}

bool GameMainMenu::getFrameVisibility(void)
{
	return _frame->getVisible();
}

void GameMainMenu::createEvent(int event)
{
	actions.emplace_back(event);
}
void resumeClick(void)
{
	lastInstance->createEvent(GAMEMAINMENU_EVENT_RESUME);
}

void leaveLobbyClick(void)
{
	lastInstance->createEvent(GAMEMAINMENU_EVENT_LEAVELOBBY);
}

void quitClick(void)
{
	lastInstance->createEvent(GAMEMAINMENU_EVENT_QUIT);
}