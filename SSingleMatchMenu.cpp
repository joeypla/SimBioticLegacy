#include "SGameServer.h"

#include "SSingleMatchMenu.h"

#include "psGui.h"
#include "SCore.h"
#include "SMainMenu.h"




#include "psGameClient.h"
#include "SSGame.h"

#include "GameConfigInterface.h"

static SText* symbioteTitle = 0;
static SText* createGameTitle = 0;

static psGui::Frame* mainFrame = 0;
static psGui::Label* mapSelectionLabel = 0;
static psGui::SimpleButton* mapButtons[4];
static psGui::Label* gameModeSelectionLabel = 0;
static psGui::SimpleButton* gameModeButtons[4];


static int mapSelection = 0;
static void mapButton0Click(void);
static void mapButton1Click(void);
static void mapButton2Click(void);
static void mapButton3Click(void);

static int gameModeSelection = 0;
static void gameModeButton0Click(void);
static void gameModeButton1Click(void);
static void gameModeButton2Click(void);
static void gameModeButton3Click(void);

static void resetSelectionButtonColors(void);

std::string mapPaths[4];
static psGui::Label* configsLabel = 0;
static psGui::Label* scoreLimitLabel = 0;
static psGui::Label* timeLimitLabel = 0;
static psGui::Label* preMatchTimeLabel = 0;
static psGui::Label* respawnDelayLabel = 0;
static psGui::Slider* scoreLimitSlider = 0;
static psGui::Slider* timeLimitSlider = 0;
static psGui::Slider* preMatchTimeSlider = 0;
static psGui::Slider* respawnDelaySlider = 0;
static psGui::Label* scoreLimitValue = 0;
static psGui::Label* timeLimitValue = 0;
static psGui::Label* preMatchTimeValue = 0;
static psGui::Label* respawnDelayValue = 0;
 
static psGui::SimpleButton* backButton = 0;
static psGui::SimpleButton* startButton = 0;

static void backClick(void);
static void startClick(void);
SSingleMatchMenu::SSingleMatchMenu(void)
{
	//Create map paths
	mapPaths[0] = "../Resources/Levels/Allocation/allocation.psm";
	mapPaths[1] = "../Resources/Levels/Core/core.psm";
	mapPaths[2] = "../Resources/Levels/Greensteel/greensteel.psm";
	mapPaths[3] = "../Resources/Levels/Atomic/atomic.psm";
	symbioteTitle = SRender::getSingleton()->createText();
	symbioteTitle->setColor(0xffffffff);
	symbioteTitle->setSize(20);
	symbioteTitle->setDimensions(0.8f, 0.2f);
	symbioteTitle->setPosition(-0.5f, 0.85f);
	symbioteTitle->setText(L"SimBiotic");
	symbioteTitle->setVisible(true);
	symbioteTitle->setAlignment(DT_VCENTER | DT_LEFT);

	createGameTitle = SRender::getSingleton()->createText();
	createGameTitle->setColor(0xffffffff);
	createGameTitle->setSize(15);
	createGameTitle->setDimensions(0.5f, 0.2f);
	createGameTitle->setPosition(-0.65f, 0.6f);
	createGameTitle->setText(L"Start Match");
	createGameTitle->setVisible(true);
	createGameTitle->setAlignment(DT_VCENTER | DT_LEFT);

	mainFrame = new psGui::Frame(0.0f, 0.0f, 0.6f, 1.0f);
	mainFrame->setColor(0x8f4f4f4f);

	mapSelectionLabel = new psGui::Label();
	mapSelectionLabel->setSize(0.5f, 0.05f);
	mapSelectionLabel->setTextColor(0xffffffff);
	mapSelectionLabel->setRelativePosition(-0.15f, 0.45f);
	mapSelectionLabel->setText(L"Select Map");
	mapSelectionLabel->setTextSize(6);
	mapSelectionLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	mainFrame->addComponent(mapSelectionLabel);

	for (int i = 0; i < 4; i++)
	{
		mapButtons[i] = new psGui::SimpleButton();
		mapButtons[i]->setSize(0.2f, 0.05f);
		mapButtons[i]->setRelativePosition(-0.8f, 0.0f);
		mapButtons[i]->setColor(0x3f8f0fef);
		mapButtons[i]->setCaptionColor(0xffffffff);
		
		
		mainFrame->addComponent(mapButtons[i]);
	}
	mapButtons[0]->setRelativePosition(-0.15f, 0.35f);
	mapButtons[1]->setRelativePosition(-0.15f, 0.25f);
	mapButtons[2]->setRelativePosition(-0.15f, 0.15f);
	mapButtons[3]->setRelativePosition(-0.15f, 0.05f);
	 
	mapButtons[0]->setCaption(L"Allocation");
	mapButtons[1]->setCaption(L"Core");
	mapButtons[2]->setCaption(L"Greensteel");
	mapButtons[3]->setCaption(L"Atomic");

	mapButtons[0]->setFunctionPointer(mapButton0Click);
	mapButtons[1]->setFunctionPointer(mapButton1Click);
	mapButtons[2]->setFunctionPointer(mapButton2Click);
	mapButtons[3]->setFunctionPointer(mapButton3Click);

	gameModeSelectionLabel = new psGui::Label();
	gameModeSelectionLabel->setSize(0.5f, 0.05f);
	gameModeSelectionLabel->setTextColor(0xffffffff);
	gameModeSelectionLabel->setRelativePosition(-0.15f, -0.05f);
	gameModeSelectionLabel->setText(L"Select Game Mode");
	gameModeSelectionLabel->setTextSize(6);
	gameModeSelectionLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	mainFrame->addComponent(gameModeSelectionLabel);
	for (int i = 0; i < 4; i++)
	{
		gameModeButtons[i] = new psGui::SimpleButton();
		gameModeButtons[i]->setSize(0.2f, 0.05f);
		gameModeButtons[i]->setRelativePosition(-0.8f, 0.0f);
		gameModeButtons[i]->setColor(0x3f8f0fef);
		gameModeButtons[i]->setCaptionColor(0xffffffff);
		
		//mapButtons[i]->setFunctionPointer(joinGameClick);
		mainFrame->addComponent(gameModeButtons[i]);
	}
	gameModeButtons[0]->setRelativePosition(-0.15f, -0.1f);
	gameModeButtons[1]->setRelativePosition(-0.15f, -0.2f);
	gameModeButtons[2]->setRelativePosition(-0.15f, -0.3f);
	gameModeButtons[3]->setRelativePosition(-0.15f, -0.4f);

	gameModeButtons[0]->setCaption(L"Deathmatch");
	gameModeButtons[1]->setCaption(L"Team Deathmatch");
	gameModeButtons[2]->setCaption(L"Sector Control");
	gameModeButtons[3]->setCaption(L"Capture the Orb");

	gameModeButtons[0]->setFunctionPointer(gameModeButton0Click);
	gameModeButtons[1]->setFunctionPointer(gameModeButton1Click);
	gameModeButtons[2]->setFunctionPointer(gameModeButton2Click);
	gameModeButtons[3]->setFunctionPointer(gameModeButton3Click);


	/* game configs */
	configsLabel = new psGui::Label();
	configsLabel->setSize(0.5f, 0.05f); 
	configsLabel->setRelativePosition(0.2f, 0.45f);
	configsLabel->setText(L"Game Configuration");
	configsLabel->setTextSize(5);
	configsLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	mainFrame->addComponent(configsLabel);

	scoreLimitLabel = new psGui::Label();
	scoreLimitLabel->setSize(0.2f, 0.05f);
	scoreLimitLabel->setTextColor(0xffffffff);
	scoreLimitLabel->setRelativePosition(0.05f, 0.35f);
	scoreLimitLabel->setText(L"Score Limit");
	scoreLimitLabel->setTextSize(5);
	scoreLimitLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	mainFrame->addComponent(scoreLimitLabel);

	scoreLimitSlider = new psGui::Slider();
	scoreLimitSlider->setMin(10);
	scoreLimitSlider->setMax(100);
	scoreLimitSlider->setLineColor(0xcf1f1f1f);
	scoreLimitSlider->setSliderColor(0x6f6f6f6f);
	scoreLimitSlider->setSize(0.11f, 0.03f);
	scoreLimitSlider->setLineThickness(0.005f);
	scoreLimitSlider->setSliderThickness(0.007f);
	scoreLimitSlider->setRelativePosition(0.16f, 0.35f);
	mainFrame->addComponent(scoreLimitSlider);

	scoreLimitValue = new psGui::Label();
	scoreLimitValue->setSize(0.2f, 0.05f);
	scoreLimitValue->setTextColor(0xffffffff);
	scoreLimitValue->setRelativePosition(0.17f, 0.35f);
	scoreLimitValue->setText(L"1000");
	scoreLimitValue->setTextSize(5);
	scoreLimitValue->setTextAlignment(DT_RIGHT| DT_VCENTER);
	mainFrame->addComponent(scoreLimitValue);

	//time limit
	timeLimitLabel = new psGui::Label();
	timeLimitLabel->setSize(0.2f, 0.05f);
	timeLimitLabel->setTextColor(0xffffffff);
	timeLimitLabel->setRelativePosition(0.05f, 0.25f);
	timeLimitLabel->setText(L"Time Limit");
	timeLimitLabel->setTextSize(5);
	timeLimitLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	mainFrame->addComponent(timeLimitLabel);

	timeLimitSlider = new psGui::Slider();
	timeLimitSlider->setMin(1);
	timeLimitSlider->setMax(20);
	timeLimitSlider->setLineColor(0xcf1f1f1f);
	timeLimitSlider->setSliderColor(0x6f6f6f6f);
	timeLimitSlider->setSize(0.11f, 0.03f);
	timeLimitSlider->setLineThickness(0.005f);
	timeLimitSlider->setSliderThickness(0.007f);
	timeLimitSlider->setRelativePosition(0.16f, 0.25f);
	mainFrame->addComponent(timeLimitSlider);

	timeLimitValue = new psGui::Label();
	timeLimitValue->setSize(0.2f, 0.05f);
	timeLimitValue->setTextColor(0xffffffff);
	timeLimitValue->setRelativePosition(0.17f, 0.25f);
	timeLimitValue->setText(L"1000");
	timeLimitValue->setTextSize(5);
	timeLimitValue->setTextAlignment(DT_RIGHT| DT_VCENTER);
	mainFrame->addComponent(timeLimitValue);

	//spawn delay
	respawnDelayLabel = new psGui::Label();
	respawnDelayLabel->setSize(0.2f, 0.05f);
	respawnDelayLabel->setTextColor(0xffffffff);
	respawnDelayLabel->setRelativePosition(0.05f, 0.15f);
	respawnDelayLabel->setText(L"Respawn Delay");
	respawnDelayLabel->setTextSize(5);
	respawnDelayLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	mainFrame->addComponent(respawnDelayLabel);

	respawnDelaySlider = new psGui::Slider();
	respawnDelaySlider->setMin(0);
	respawnDelaySlider->setMax(30);
	respawnDelaySlider->setLineColor(0xcf1f1f1f);
	respawnDelaySlider->setSliderColor(0x6f6f6f6f);
	respawnDelaySlider->setSize(0.11f, 0.03f);
	respawnDelaySlider->setLineThickness(0.005f);
	respawnDelaySlider->setSliderThickness(0.007f);
	respawnDelaySlider->setRelativePosition(0.16f, 0.15f);
	mainFrame->addComponent(respawnDelaySlider);

	respawnDelayValue = new psGui::Label();
	respawnDelayValue->setSize(0.2f, 0.05f);
	respawnDelayValue->setTextColor(0xffffffff);
	respawnDelayValue->setRelativePosition(0.17f, 0.15f);
	respawnDelayValue->setText(L"1000");
	respawnDelayValue->setTextSize(5);
	respawnDelayValue->setTextAlignment(DT_RIGHT| DT_VCENTER);
	mainFrame->addComponent(respawnDelayValue);

	/* back and start buttons */
	backButton = new psGui::SimpleButton();
	backButton->setSize(0.15f, 0.05f);
	backButton->setRelativePosition(0.2f, -0.45f);
	backButton->setColor(0x8f3f3f3f);
	backButton->setCaptionColor(0xffffffff);
	backButton->setCaption(L"Back");
	backButton->setFunctionPointer(backClick);
	backButton->setCaptionSize(4);
	mainFrame->addComponent(backButton);

	startButton = new psGui::SimpleButton();
	startButton->setSize(0.15f, 0.05f);
	startButton->setRelativePosition(0.05f, -0.45f);
	startButton->setColor(0x8f3f3f3f);
	startButton->setCaptionColor(0xffffffff);
	startButton->setCaption(L"Start");
	startButton->setFunctionPointer(startClick);
	startButton->setCaptionSize(4); 
	mainFrame->addComponent(startButton);

	mainFrame->setPosition(0.0f, 0.0f);

	resetSelectionButtonColors();
}

SSingleMatchMenu::~SSingleMatchMenu(void)
{
	 
	SRender::getSingleton()->releaseText(symbioteTitle);
	SRender::getSingleton()->releaseText(createGameTitle);

	delete mainFrame;
	delete mapSelectionLabel;
	for (int i = 0; i < 4; i++) delete mapButtons[i];
	delete gameModeSelectionLabel;
	for (int i = 0; i < 4; i++) delete gameModeButtons[i];

	delete configsLabel;
	delete scoreLimitLabel;
	delete scoreLimitSlider;
	delete scoreLimitValue;
	delete timeLimitLabel;
	delete timeLimitSlider;
	delete timeLimitValue;
	delete respawnDelayLabel;
	delete respawnDelaySlider;
	delete respawnDelayValue;

	delete backButton;
	delete startButton;
}

bool SSingleMatchMenu::renderToDevice(const LPDIRECT3DDEVICE9 dev)
{
	SRender::getSingleton()->render();
	return true;
}

bool SSingleMatchMenu::updateInterface(DWORD lastTimeTaken)
{
	mainFrame->update();

	//update the slider values for game configs
	scoreLimitValue->setText(std::to_wstring(scoreLimitSlider->getValue() * 100).c_str());
	timeLimitValue->setText(std::to_wstring(timeLimitSlider->getValue()).c_str());
	respawnDelayValue->setText(std::to_wstring(respawnDelaySlider->getValue()).c_str());
	return true;
}

void SSingleMatchMenu::receiveSystemMessage(UINT message, UINT wParam)
{
	mainFrame->sendSystemMessage(message, wParam);
}

void mapButton0Click(void)
{
	mapSelection = 0;
	resetSelectionButtonColors();
}

void mapButton1Click(void)
{
	mapSelection = 1;
	resetSelectionButtonColors();
}

void mapButton2Click(void)
{
	mapSelection = 2;
	resetSelectionButtonColors();
}

void mapButton3Click(void)
{
	mapSelection = 3;
	resetSelectionButtonColors();
}


void gameModeButton0Click(void)
{
	gameModeSelection = 0;
	resetSelectionButtonColors();
}

void gameModeButton1Click(void)
{
	gameModeSelection = 1;
	resetSelectionButtonColors();
}

void gameModeButton2Click(void)
{
	gameModeSelection = 2;
	resetSelectionButtonColors();
}

void gameModeButton3Click(void)
{
	gameModeSelection = 3;
	resetSelectionButtonColors();
}

void backClick(void)
{
	S_Core::goToInterfaceObject(new SMainMenu());
}


void startClick(void)
{
	//SGameServerInitInfo* data = SGameServer::pGlobalInitInfo();
	////map path dealio is just for now, it makes it pretty stuck for those maps
	//data->mapPath = mapPaths[mapSelection];
	//data->gameMode = gameModeSelection;
	//data->scoreLimit = scoreLimitSlider->getValue() * 100;
	//data->timeLimit = timeLimitSlider->getValue();
	//data->spawnDelay = respawnDelaySlider->getValue();
	//data->preMatchTime = preMatchTimeSlider->getValue();
	//create the seperate game server thread

	SGameServerInitInfo* serverParameters = new SGameServerInitInfo();
	//serverParameters->mapPath = "../Resources/Levels/Birth/birth.psm";
	CreateThread( NULL, 0, SGameServerThread, NULL, 0, NULL);
	Sleep(1000);
	//load up the game engine with server
	ps::GameInitInfo gii;   
	gii.ip = "127.0.0.1";


	gii.screenName = GlobalConfiguration::getSingleton()->str_screenName();
	int serverResponse = SGameClient::getSingleton()->connectToGameServer(&gii);
	
	S_Core::goToInterfaceObject(new SSGame(&gii));

}

void resetSelectionButtonColors(void)
{
	for (int i = 0; i < 4; i++)
	{
		mapButtons[i]->setColor(0x8f3f3f3f);
	}

	for (int i = 0; i < 4; i++)
	{
		gameModeButtons[i]->setColor(0x8f3f3f3f);
	}

	if (mapSelection > -1 && mapSelection < 4) mapButtons[mapSelection]->setColor(0x8f5f5f5f);
	if (gameModeSelection > -1 && gameModeSelection < 4) gameModeButtons[gameModeSelection]->setColor(0x8f5f5f5f); 
}