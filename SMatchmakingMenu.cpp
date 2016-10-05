#define _WINSOCKAPI_
#include "SMatchmakingMenu.h"

#include "psGameClient.h"
#include "SCreateLobbyMenu.h"
#include "GameConfigInterface.h"
#include "SCore.h"
#include "SSGame.h"
static void joinClick(void);
static void createLobbyClick(void);
static void joinManualClick(void);
static SMatchmakingMenu* lastInstance = 0;

SMatchmakingMenu::SMatchmakingMenu(void)
{
	lastInstance = this;
	createLobbyMenu.setParentMenu(this);
	joinLobbyMenu.setParentMenu(this);

	_frame = new psGui::Frame(0.0f, 0.0f, 1.3f, 1.3f);
	_frame->setColor(0x8f2f2f2f);
	
	_internetServerBox = new psGui::Listbox(20, -0.13f, -0.1f, 1.0f, 1.0f, 0.05f, 4, 0.05f);
	_internetServerBox->setColumnTitle(0, L"Lobby");
	_internetServerBox->setColumnTitle(1, L"Host");
	_internetServerBox->setColumnTitle(2, L"IP Address");
	_internetServerBox->setColumnTitle(3, L"Population");
	_internetServerBox->setBackColor1(0xcf6f6f6f);
	_internetServerBox->setBackColor2(0xcf8f8f8f);
	_internetServerBox->setHeaderColor(0xcf4f4f4f);
	_internetServerBox->setScrollBackColor(0xcf3f3f3f);
	_internetServerBox->setScrollBarColor(0xcf5f5f5f);
	_frame->addComponent(_internetServerBox);


	_netStatusText = SRender::getSingleton()->createText();
	_netStatusText->setDimensions(0.8f, 0.1f);
	_netStatusText->setColor(0xffffffff);
	_netStatusText->setText(L"Net Status: Connecting...");
	_netStatusText->setSize(9);
	_netStatusText->setPosition(0.5f, 0.75f);
	_netStatusText->setAlignment(DT_LEFT | DT_VCENTER);

	_netMessageText = SRender::getSingleton()->createText();
	_netMessageText->setDimensions(1.2f, 0.2f);
	_netMessageText->setColor(0xffffffff);
	_netMessageText->setSize(6);
	_netMessageText->setPosition(0.0f, 0.5f);
	_netMessageText->setAlignment(DT_WORDBREAK);

	bool netConnection = netClient.netConnect();
	if (netConnection) _netStatusText->setText(L"Net Status: Connected");
	else _netStatusText->setText(L"Net Status: LAN ONLY");

	////JUST FOR TESTING
	//psGui::ListboxEntry entry;
	//entry.values.push_back(L"COLUMN 1");
	//entry.values.push_back(L"COLUMN 2");
	//entry.values.push_back(L"COLUMN 3");
	//entry.values.push_back(L"COLUMN 4");
	//for (int i = 0; i < 75; i++)
	//_internetServerBox->addRow(&entry);



	_joinLobbyButton = new psGui::SimpleButton();
	_joinLobbyButton->setSize(0.26f, 0.07f);
	_joinLobbyButton->setRelativePosition(0.51f, 0.3f);
	_joinLobbyButton->setColor(0x4f3f3f3f);
	_joinLobbyButton->setCaptionSize(7);
	_joinLobbyButton->setCaptionColor(0xffffffff);
	_joinLobbyButton->setCaption(L"Join Lobby");
	_joinLobbyButton->setFunctionPointer(joinClick);
	_frame->addComponent(_joinLobbyButton);

	_createLobbyButton = new psGui::SimpleButton();
	_createLobbyButton->setSize(0.26f, 0.07f);
	_createLobbyButton->setRelativePosition(0.51f, 0.2f);
	_createLobbyButton->setColor(0x4f3f3f3f);
	_createLobbyButton->setCaptionSize(7);
	_createLobbyButton->setCaptionColor(0xffffffff);
	_createLobbyButton->setCaption(L"Create Lobby..");
	_createLobbyButton->setFunctionPointer(createLobbyClick);
	_frame->addComponent(_createLobbyButton);

	_joinManualButton = new psGui::SimpleButton();
	_joinManualButton->setSize(0.26f, 0.07f);
	_joinManualButton->setRelativePosition(0.51f, 0.1f);
	_joinManualButton->setColor(0x4f3f3f3f);
	_joinManualButton->setCaptionSize(7);
	_joinManualButton->setCaptionColor(0xffffffff);
	_joinManualButton->setCaption(L"Join IP..");
	_joinManualButton->setFunctionPointer(joinManualClick);
	_frame->addComponent(_joinManualButton);

	_refreshButton = new psGui::SimpleButton();
	_refreshButton->setSize(0.26f, 0.07f);
	_refreshButton->setRelativePosition(0.51f, -0.5f);
	_refreshButton->setColor(0x4f3f3f3f);
	_refreshButton->setCaptionSize(7);
	_refreshButton->setCaptionColor(0xffffffff);
	_refreshButton->setCaption(L"Refresh");
	//_joinLobbyButton->setFunctionPointer(joinGameClick);
	_frame->addComponent(_refreshButton);


	_frame->setPosition(0.0f, 0.0f);







	//The connecting to lobby frame
	_lobbyConnectingFrame = new psGui::Frame(0.0f, 0.0f, 0.6f, 0.15f);
	_lobbyConnectingFrame->setColor(0x9f1f1f1f);
	_lobbyConnectingFrame->setVisible(false);

	_lobbyConnectingLabel = new psGui::Label();
	_lobbyConnectingLabel->setSize(0.8f, 0.2f);
	_lobbyConnectingLabel->setTextColor(0xffffffff);
	_lobbyConnectingLabel->setRelativePosition(0.0f, 0.0f);
	_lobbyConnectingLabel->setText(L"Connecting to Lobby...");
	_lobbyConnectingLabel->setTextSize(7);
	_lobbyConnectingLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	_lobbyConnectingFrame->addComponent(_lobbyConnectingLabel);

	float aspectRatio = SRender::getSingleton()->getWidth() / SRender::getSingleton()->getHeight();
	_lobbyConnectingSymbol = new psGui::Simple();
	_lobbyConnectingSymbol->setColor(0xffffffff);
	_lobbyConnectingSymbol->setSize(0.15f / aspectRatio, 0.15f);
	_lobbyConnectingSymbol->setRelativePosition(-0.23f, 0.0f);
	_lobbyConnectingSymbol->setTextureFromPath(L"../Resources/Textures/logoMini.png");
	_lobbyConnectingFrame->addComponent(_lobbyConnectingSymbol);

	_lobbyConnectingCancel = new psGui::SimpleButton();
	_lobbyConnectingCancel->setSize(0.05f, 0.05f * aspectRatio);
	_lobbyConnectingCancel->setRelativePosition(0.26f, 0.1f);
	_lobbyConnectingCancel->setColor(0x4f3f3f3f);
	_lobbyConnectingCancel->setCaptionSize(7);
	_lobbyConnectingCancel->setCaptionColor(0xffffffff);
	_lobbyConnectingCancel->setCaption(L"X");
	//_joinLobbyButton->setFunctionPointer(joinGameClick);
	_lobbyConnectingFrame->addComponent(_lobbyConnectingCancel);

	_lobbyConnectingFrame->setPosition(0.0f, 0.0f);
	_lobbyConnectingFrame->setVisible(false);
}

SMatchmakingMenu::~SMatchmakingMenu(void)
{
	delete _frame;
	_frame = 0;
	delete _internetServerBox;
	delete _joinLobbyButton;
	delete _createLobbyButton;
	delete _joinManualButton;
	delete _refreshButton;
	delete _lobbyConnectingFrame;
	delete _lobbyConnectingLabel;
	delete _lobbyConnectingSymbol;

	SRender::getSingleton()->releaseText(_netStatusText);
	SRender::getSingleton()->releaseText(_netMessageText);
}

void SMatchmakingMenu::setFrameVisibility(bool visibility)
{
	_frame->setVisible(visibility);

	_netMessageText->setVisible(visibility);

	if (visibility)
	{
		_lobbyConnectingFrame->setVisible(false);
	}
}

bool SMatchmakingMenu::getFrameVisibility(void)
{
	if (_frame == 0) return false;
	return _frame->getVisible();
}

void SMatchmakingMenu::receiveSystemMessage(UINT message, UINT param)
{
	if (_frame->getVisible()) _frame->sendSystemMessage(message, param);

	if (createLobbyMenu.getFrameVisibility()) createLobbyMenu.receiveSystemMessage(message, param);
	if (joinLobbyMenu.getFrameVisibility()) joinLobbyMenu.receiveSystemMessage(message, param);
}

void SMatchmakingMenu::update(void)
{
	static float connectingSymbolRotation = 0.0f;
	connectingSymbolRotation += 0.04f;
	_lobbyConnectingSymbol->setRotation(connectingSymbolRotation);

	netClient.update();
	if (netClient.getInternetLobbiesChanged())
	{
		std::map<int, SimBioticLobby>& lobbyList = netClient.getInternetLobbies();
		_internetServerBox->clear();
		//iterate through the map and add all the rows into the internet listbox
		std::wstringstream tempStream;
		tempStream.str(L"");
		std::map<int, SimBioticLobby>::iterator it;
		for (it = lobbyList.begin(); it != lobbyList.end(); it++)
		{
			psGui::ListboxEntry entry;
			std::wstring lobbyName(it->second.name.begin(), it->second.name.end());
			std::wstring creatorName(it->second.creatorName.begin(), it->second.creatorName.end());
			int playerCount = it->second.playerCount;
			int maxPlayerCount = it->second.maxPlayerCount;
			
			entry.values.push_back(lobbyName);
			entry.values.push_back(creatorName);
			unsigned int ipAddress = it->second.ipAddress;
			unsigned char* chIP = (unsigned char*)&ipAddress;
			tempStream.str(L"");
			tempStream<<(int)chIP[0]<<L"."<<(int)chIP[1]<<L"."<<(int)chIP[2]<<L"."<<(int)chIP[3]<<":"<<it->second.port;
			entry.values.push_back(tempStream.str());
			tempStream.str(L"");
			tempStream<<playerCount<<" ("<<maxPlayerCount<<")";
			entry.values.push_back(tempStream.str());

			_internetServerBox->addRow(&entry);
		}
	}
	if (netClient.getDailyMessageChanged())
	{
		std::string message = netClient.getDailyMessage();
		std::wstring wMessage(message.begin(), message.end());
		_netMessageText->setText(wMessage.c_str());
	}
	if (_frame->getVisible()) _frame->update();

	if (createLobbyMenu.getFrameVisibility()) createLobbyMenu.update();
	if (joinLobbyMenu.getFrameVisibility()) joinLobbyMenu.update();



	int result = SGameClient::getSingleton()->updateConnection();
	if (result == SGAMECLIENT_UPDATECONNECTION_NORESPONSE)
	{
		_lobbyConnectingLabel->setText(L"No Response From Lobby");
	} else if (result == SGAMECLIENT_UPDATECONNECTION_LOBBYFULL)
	{
		_lobbyConnectingLabel->setText(L"Lobby is Full");
	} else if (result == SGAMECLIENT_UPDATECONNECTION_SUCCESSFULJOIN)
	{
		S_Core::goToInterfaceObject(new SSGame(SGameClient::getSingleton()->getLastGameInitInfo()));
	}
} 

SCreateLobbyMenu& SMatchmakingMenu::getCreateLobbyMenu(void)
{
	return createLobbyMenu;
}

SJoinLobbyMenu& SMatchmakingMenu::getJoinLobbyMenu(void)
{
	return joinLobbyMenu;
}

SimBioticNetClient& SMatchmakingMenu::getNetClient(void)
{
	return netClient;
}

psGui::Listbox* SMatchmakingMenu::getInternetListbox(void){return _internetServerBox;}

void SMatchmakingMenu::showConnecting(void)
{
	_lobbyConnectingFrame->setVisible(true);
	_lobbyConnectingLabel->setText(L"Connecting to lobby...");
}
#include "SLOG.h"
void joinClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	std::string address = lastInstance->getInternetListbox()->getStrAtIndexByCol(2);
	//here we seperate the string into the ip and port
	std::istringstream ss(address);
	std::string token;

	std::getline(ss, token, ':');
	sockaddr_in targetAddress;
	targetAddress.sin_addr.S_un.S_addr = inet_addr(token.c_str());

	std::getline(ss, token, ':');
	targetAddress.sin_port = atoi(token.c_str());
	//lastInstance->getNetClient().sendNatPunchthroughRequest(ip);
	ps::GameInitInfo gii;
	gii.ip = inet_ntoa(targetAddress.sin_addr);
	gii.port = targetAddress.sin_port;
	gii.screenName = GlobalConfiguration::getSingleton()->str_screenName();

	SGameClient::getSingleton()->connectToGameServer(&gii);

	lastInstance->setFrameVisibility(false);

	S_LOG::write(inet_ntoa(targetAddress.sin_addr));
	S_LOG::write(std::to_string(targetAddress.sin_port).c_str());

	lastInstance->showConnecting();

}
void createLobbyClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->setFrameVisibility(false);
	lastInstance->getCreateLobbyMenu().setFrameVisibility(true);
}

void joinManualClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->setFrameVisibility(false);
	lastInstance->getJoinLobbyMenu().setFrameVisibility(true);
}