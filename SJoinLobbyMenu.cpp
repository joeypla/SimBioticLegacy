#define _WINSOCKAPI_
#include "SJoinLobbyMenu.h"

#include "SMatchmakingMenu.h"
#include "SAUDIO.h"

#include "GameInitInfo.h"
#include "psGameClient.h"

#include "GameConfigInterface.h"
static void joinClick(void);
static void backClick(void);

static SJoinLobbyMenu* lastInstance = 0;
SJoinLobbyMenu::SJoinLobbyMenu(void)
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
	_headerTitle->setText(L"Join Lobby by IP");
	_headerTitle->setTextSize(6);
	_headerTitle->setTextAlignment(DT_CENTER | DT_VCENTER);
	_frame->addComponent(_headerTitle);

	_IPField = new psGui::TextField();
	_IPField->setSize(0.3f, 0.05f);
	_IPField->setFieldColor(0xcf2f2f2f);
	_IPField->setTextColor(0xffffffff);
	_IPField->setTextSize(6);
	_IPField->setRelativePosition(0.0f, 0.0f);
	_IPField->setActive(true);
	_frame->addComponent(_IPField);

	_joinButton = new psGui::SimpleButton();
	_joinButton->setSize(0.2f, 0.05f);
	_joinButton->setRelativePosition(-0.13f, -0.2f);
	_joinButton->setColor(0x3f8f0fef);
	_joinButton->setCaptionColor(0xffffffff);
	_joinButton->setCaption(L"Join");
	_joinButton->setFunctionPointer(joinClick);
	_joinButton->setCaptionSize(4);
	_frame->addComponent(_joinButton);

	_backButton = new psGui::SimpleButton();
	_backButton->setSize(0.2f, 0.05f);
	_backButton->setRelativePosition(0.13f, -0.2f);
	_backButton->setColor(0x3f8f0fef);
	_backButton->setCaptionColor(0xffffffff);
	_backButton->setCaption(L"Cancel");
	_backButton->setFunctionPointer(backClick);
	_backButton->setCaptionSize(4);
	_frame->addComponent(_backButton);

	_frame->setPosition(0.0f, 0.0f);
	_frame->setVisible(false);
}

SJoinLobbyMenu::~SJoinLobbyMenu(void)
{
	delete _frame;
	delete _frameHeader;
	delete _headerTitle;
	delete _IPField;
	delete _joinButton;
	delete _backButton;
}

void SJoinLobbyMenu::setFrameVisibility(bool visible)
{
	_frame->setVisible(visible);
}

bool SJoinLobbyMenu::getFrameVisibility(void)
{
	return _frame->getVisible();
}

void SJoinLobbyMenu::update(void)
{
	_frame->update();
}

void SJoinLobbyMenu::receiveSystemMessage(UINT message, UINT param)
{
	_frame->sendSystemMessage(message, param);
}

void SJoinLobbyMenu::setParentMenu(SMatchmakingMenu* pMatchmakingMenu)
{
	_pMatchmakingMenu = pMatchmakingMenu;
}

SMatchmakingMenu* SJoinLobbyMenu::getParentMenu(void)
{
	return _pMatchmakingMenu;
}

std::string SJoinLobbyMenu::getIP(void)
{
	std::wstring wIpString = _IPField->getText();
	std::string ipString(wIpString.begin(), wIpString.end());
	return ipString;
}
void joinClick(void)
{
	S_AUDIO::getSingleton()->playClick0();

	ps::GameInitInfo gii;
	std::string ipString = lastInstance->getIP();
	gii.ip = ipString;
	gii.port = 12371;
	gii.screenName = GlobalConfiguration::getSingleton()->str_screenName();

	SGameClient::getSingleton()->connectToGameServer(&gii);

	lastInstance->setFrameVisibility(false);

	
	lastInstance->getParentMenu()->setFrameVisibility(false);
	lastInstance->getParentMenu()->showConnecting();
}

void backClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->setFrameVisibility(false);
	lastInstance->getParentMenu()->setFrameVisibility(true);
}