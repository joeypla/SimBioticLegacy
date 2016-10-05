#define _WINSOCKAPI_
#include "SCreateLobbyMenu.h"

#include "SMatchmakingMenu.h"

static void createClick(void);
static void backClick(void);

static SCreateLobbyMenu* lastInstance = 0;
SCreateLobbyMenu::SCreateLobbyMenu(void)
{
	lastInstance = this;
	_pMatchmakingMenu = 0;
	_frame = new psGui::Frame(0.0f, 0.0f, 0.5f, 0.5f);
	_frame->setColor(0xcf1f1f1f);

	_frameHeader = new psGui::Simple();
	_frameHeader->setSize(0.5f, 0.05f);
	_frameHeader->setColor(0x9f0f0f10);
	_frameHeader->setRelativePosition(0.0f, 0.225f);
	_frame->addComponent(_frameHeader);

	_headerTitle = new psGui::Label();
	_headerTitle->setSize(0.5f, 0.05f);
	_headerTitle->setTextColor(0xffffffff);
	_headerTitle->setRelativePosition(0.0f, 0.225f);
	_headerTitle->setText(L"Create Game Lobby");
	_headerTitle->setTextSize(6);
	_headerTitle->setTextAlignment(DT_CENTER | DT_VCENTER);
	_frame->addComponent(_headerTitle);

	_lobbyName = new psGui::TextField();
	_lobbyName->setSize(0.3f, 0.05f);
	_lobbyName->setFieldColor(0xcf2f2f2f);
	_lobbyName->setTextColor(0xffffffff);
	_lobbyName->setTextSize(6);
	_lobbyName->setRelativePosition(0.0f, 0.0f);
	_lobbyName->setActive(true);
	_frame->addComponent(_lobbyName);

	_createButton = new psGui::SimpleButton();
	_createButton->setSize(0.2f, 0.05f);
	_createButton->setRelativePosition(-0.13f, -0.2f);
	_createButton->setColor(0x3f8f0fef);
	_createButton->setCaptionColor(0xffffffff);
	_createButton->setCaption(L"Create");
	_createButton->setFunctionPointer(createClick);
	_createButton->setCaptionSize(4);
	_frame->addComponent(_createButton);

	_cancelButton = new psGui::SimpleButton();
	_cancelButton->setSize(0.2f, 0.05f);
	_cancelButton->setRelativePosition(0.13f, -0.2f);
	_cancelButton->setColor(0x3f8f0fef);
	_cancelButton->setCaptionColor(0xffffffff);
	_cancelButton->setCaption(L"Cancel");
	_cancelButton->setFunctionPointer(backClick);
	_cancelButton->setCaptionSize(4);
	_frame->addComponent(_cancelButton);

	_frame->setPosition(0.0f, 0.0f);
	_frame->setVisible(false);
}

SCreateLobbyMenu::~SCreateLobbyMenu(void)
{
	delete _frame;
	delete _frameHeader;
	delete _headerTitle;
	delete _lobbyName;
	delete _createButton;
	delete _cancelButton;
}

psGui::TextField* SCreateLobbyMenu::nameField(void)
{
	return _lobbyName;
}
void SCreateLobbyMenu::setFrameVisibility(bool visible)
{
	_frame->setVisible(visible);
}

bool SCreateLobbyMenu::getFrameVisibility(void)
{
	return _frame->getVisible();
}

void SCreateLobbyMenu::update(void)
{
	_frame->update();
}

void SCreateLobbyMenu::receiveSystemMessage(UINT message, UINT param)
{
	_frame->sendSystemMessage(message, param);
}

void SCreateLobbyMenu::setParentMenu(SMatchmakingMenu* pMatchmakingMenu)
{
	_pMatchmakingMenu = pMatchmakingMenu;
}

SMatchmakingMenu* SCreateLobbyMenu::getParentMenu(void)
{
	return _pMatchmakingMenu;
}

#include "SGameServer.h"
#include "psGameClient.h"
#include "GameConfigInterface.h"
#include "SCore.h"
#include "SSGame.h"
void createClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	ps::GameInitInfo gii;   
	gii.ip = "127.0.0.1";
	gii.levelPath = "../Resources/Levels/White/white.psm";
	gii.port = 12371;
	gii.clientActorIndex = 0;
	gii.screenName = GlobalConfiguration::getSingleton()->str_screenName();
	std::wstring wLobbyName = lastInstance->nameField()->getText();
	gii.lobbyName = std::string(wLobbyName.begin(), wLobbyName.end());
	gii.bTimeLimit = true;
	gii.timeLimit = 600;
	gii.prematchTime = 10;
	//gii.pre
	SGameClient::getSingleton()->hostServer(&gii);
	
	
	
	S_Core::goToInterfaceObject(new SSGame(&gii));
}

void backClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->setFrameVisibility(false);
	lastInstance->getParentMenu()->setFrameVisibility(true);
}