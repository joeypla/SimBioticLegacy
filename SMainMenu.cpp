

#define _WINSOCKAPI_

#include "SMainMenu.h"
#include "SRender.h"
#include "psGui.h"
#include "SInput.h"
#include "World.h"

#include "SSGame.h"
#include "psGameClient.h"
#include "GameInitInfo.h"
#include "SSingleMatchMenu.h"
#include "pseBase.h"

static SRender* rdr;
static SCamera* cam = 0;
static S2D* symbioteTitle = 0;

static ps::World* backgroundWorld;

static psGui::Frame* connectionFrame;
static psGui::Label* connectionLabel;

static S2D* bottomHeader;
static S2D* topHeader;
static SImage* selectionImage;
std::vector<std::wstring> mainInfoStrings;
static psGui::Label* mainInfo;
static psGui::Frame* mainButtonsPanel;
static psGui::Simple* topLine;
static psGui::SimpleButton* quitButton;
static psGui::SimpleButton* optionsButton;
static psGui::SimpleButton* createGameButton;
static psGui::SimpleButton* mapEditorButton;

static psGui::Frame* jsFrame;
static psGui::Simple* jsHeader;
static psGui::Label* jsHeaderTitle;
static psGui::TextField* jsIP;
static psGui::SimpleButton* jsJoinButton;
static psGui::SimpleButton* jsCancelButton;


static psGui::Frame* charFrame;
static psGui::Label* charNameLabel;
static psGui::TextField* charNameField;
static psGui::SimpleButton* charClose;



static psGui::Frame* cgFrame;
static psGui::Simple* cgHeader;
static psGui::Label* cgHeaderTitle;

void joinGameClick(void);
void createGameClick(void);
void modCharClick(void);
void mapEditorClick(void);
void optionsClick(void);
void quitClick(void);

void jsJoinClick(void);
void jsCancelClick(void);

bool shouldQuit = false;

static SMainMenu* lastInstance = 0;
#include "SAnimatedMesh.h"
static SAnimatedMesh* animatedMesh = 0;
SMainMenu::SMainMenu(void){
	lastInstance = this;

	
	_enthusiastLogo = SRender::getSingleton()->createS2D();
	_enthusiastLogo->setTextureFromPath(L"../Resources/Textures/elogoblack.png");
	_enthusiastLogo->setDimensions(0.4f, 0.2f);
	_enthusiastLogo->setPosition(0.8f, -0.72f);
	_enthusiastLogo->setColor(0x9fffffff);
	_enthusiastLogo->setVisible(true);

	optionsMenu = new SOptionsMenu();
	matchmakingMenu = new SMatchmakingMenu();
	setup();
	

	//Just for testing out proper cal3d loading for now
	animatedMesh = new SAnimatedMesh();
	std::vector<std::string> meshPaths;
	meshPaths.push_back("../Resources/Meshes/Cally/cally_calf_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_calf_right.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_chest.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_foot_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_foot_right.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_hand_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_hand_right.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_head.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_lowerarm_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_lowerarm_right.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_neck.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_pelvis.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_ponytail.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_thigh_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_thigh_right.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_upperarm_left.cmf");
	meshPaths.push_back("../Resources/Meshes/Cally/cally_upperarm_right.cmf");

	std::vector<std::string> animationPaths;
	animationPaths.push_back("../Resources/Meshes/Cally/cally_idle.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_jog.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_shoot_arrow.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_strut.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_tornado_kick.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_walk.caf");
	animationPaths.push_back("../Resources/Meshes/Cally/cally_wave.caf");


	bool success = animatedMesh->createModel("testModel",
		"../Resources/Meshes/SimSoldier/cally.csf", 
		meshPaths,
		animationPaths);

	
}
SMainMenu::~SMainMenu(void){
	
	SRender::getSingleton()->releaseS2D(symbioteTitle);
	SRender::getSingleton()->releaseS2D(_enthusiastLogo);
	SRender::getSingleton()->releaseS2D(bottomHeader);
	SRender::getSingleton()->releaseS2D(topHeader);
	
	delete backgroundWorld;

	delete mainInfo;
	delete topLine;
	delete mainButtonsPanel;
	delete quitButton;
	delete optionsButton;
	delete mapEditorButton;
	delete createGameButton;

	delete jsFrame;
	delete jsHeader;
	delete jsHeaderTitle;
	delete jsIP;
	delete jsJoinButton;
	delete jsCancelButton;

	delete connectionFrame;
	delete connectionLabel;

	delete optionsMenu;
	delete matchmakingMenu;
	//delete modCharObject;
}



bool SMainMenu::renderToDevice(LPDIRECT3DDEVICE9 dev){
	rdr->render();
	return true;
}

bool SMainMenu::updateInterface(DWORD time){
	if (shouldQuit) return false;
	
	
	cam->rotateView(0.0f, 0.002f, 0.0f, 2);
	//update sidepanel frame thing
	if (createGameButton->getMouseWithin())
		mainInfo->setText(mainInfoStrings[0].c_str());

	if (mapEditorButton->getMouseWithin())
		mainInfo->setText(mainInfoStrings[1].c_str());

	if (optionsButton->getMouseWithin())
		mainInfo->setText(mainInfoStrings[2].c_str());

	if (quitButton->getMouseWithin())
		mainInfo->setText(mainInfoStrings[3].c_str());

	mainButtonsPanel->update();

	//update join server thing if need be
	if (jsFrame->getVisible()) jsFrame->update();

	//update character frame if need be
	if (modCharObject.getFrameVisibility()) modCharObject.update();


	if (optionsMenu->getFrameVisibility()) optionsMenu->update();



	matchmakingMenu->update();

	//top and bottom panels beginning start thing
	return true;
}
void SMainMenu::receiveSystemMessage(UINT message, UINT param)
{
	mainButtonsPanel->sendSystemMessage(message, param);
	if (jsFrame->getVisible()) jsFrame->sendSystemMessage(message, param);
	if (optionsMenu->getFrameVisibility()) optionsMenu->receiveSystemMessage(message, param);
	matchmakingMenu->receiveSystemMessage(message, param);
}


void SMainMenu::setup(void){
	ShowCursor(true);
	SetCursorPos(300,300);

	rdr = SRender::getSingleton();
	
	
	//load music
	//psAudio::Sound* mainTheme = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Music/main0.mp3");
	//mainTheme->setMode(FMOD_LOOP_NORMAL);
	//S_AUDIO::getSingleton()->playSound(mainTheme);

	cam = new SCamera();
	rdr->setCamera(cam);
	cam->setPosition(32.0f, 3.5f, 32.0f);
	cam->setDirection(0.0f, 0.5f, 1.0f, 2);
	cam->setUpVector(0.0f, 1.0f, 0.0f);
	
	
	backgroundWorld = ps::World::new_World("../Resources/Levels/White/white.psm");

	//Title creation
	symbioteTitle = SRender::getSingleton()->createS2D();
	symbioteTitle->setColor(0xcfffffff);
	float aspectRatio = SRender::getSingleton()->getHeight() / SRender::getSingleton()->getWidth();
	symbioteTitle->setDimensions(0.4f * aspectRatio, 0.4f);
	symbioteTitle->setPosition(-0.84f, 0.67f);
	symbioteTitle->setTextureFromPath(L"../Resources/Textures/Menu/mainicon.png");
	symbioteTitle->setVisible(true);


	//top header
	topHeader = SRender::getSingleton()->createS2D();
	topHeader->setColor(0xffffffff);
	topHeader->setDimensions(2.0f, 0.15f);
	topHeader->setPosition(0.0f, 0.925f);
	topHeader->setTextureFromPath(L"../Resources/Textures/Menu/maintop.png");
	topHeader->setVisible(true);

	//bottom header
	bottomHeader = SRender::getSingleton()->createS2D();
	bottomHeader->setColor(0xffffffff);
	bottomHeader->setDimensions(2.0f, 0.15f);
	bottomHeader->setPosition(0.0f, -0.925f);
	bottomHeader->setTextureFromPath(L"../Resources/Textures/Menu/mainbottom.png");
	bottomHeader->setVisible(true);
	//MAIN PANEL CREATION
	selectionImage = new SImage(L"../Resources/Textures/Menu/selection.png");

	mainButtonsPanel = new psGui::Frame(-0.8, 0.0f, 0.3f, 1.4f);
	mainButtonsPanel->setColor(0x9f1f1f1f);
	
	mainInfoStrings.push_back(L"Select a server to play on, or host your own.");
	mainInfoStrings.push_back(L"Jump into the SimBiotic Map Editor to build your own levels.");
	mainInfoStrings.push_back(L"Not happy with performance? Don't like the controls? Change the settings.");
	mainInfoStrings.push_back(L"Hate the game? You don't have to play it.");

	mainInfo = new psGui::Label();
	mainInfo->setSize(0.26f, 0.2f);
	mainInfo->setTextColor(0xffffffff);
	mainInfo->setRelativePosition(0.0f, 0.32f);
	mainInfo->setText(L"Change the settings of the game.");
	mainInfo->setTextSize(5);
	mainInfo->setTextAlignment(DT_LEFT | DT_WORDBREAK);
	mainButtonsPanel->addComponent(mainInfo);

	topLine = new psGui::Simple();
	topLine->setSize(0.26f, 0.005f);
	topLine->setColor(0xffffffff);
	topLine->setRelativePosition(0.0f, 0.3f);
	topLine->setImage(selectionImage);
	mainButtonsPanel->addComponent(topLine);

	createGameButton = new psGui::SimpleButton();
	createGameButton->setTextHorizontalProportion(0.95f);
	createGameButton->setTextAlignment(DT_LEFT | DT_VCENTER);
	createGameButton->setSize(0.26f, 0.045f);
	createGameButton->setRelativePosition(0.0f, 0.25f);
	createGameButton->setColor(0x4f3f3f3f);
	createGameButton->setCaptionColor(0xffffffff);
	createGameButton->setCaptionSize(7);
	createGameButton->setCaption(L"SIMBIOTIC NET");
	createGameButton->setFunctionPointer(createGameClick);
	createGameButton->setInnerImage(selectionImage);
	createGameButton->setInnerColor(0xffffffff);
	createGameButton->setOuterColor(0x00000000);
	mainButtonsPanel->addComponent(createGameButton);

	mapEditorButton = new psGui::SimpleButton();
	mapEditorButton->setTextHorizontalProportion(0.95f);
	mapEditorButton->setTextAlignment(DT_LEFT | DT_VCENTER);
	mapEditorButton->setSize(0.26f, 0.045f);
	mapEditorButton->setRelativePosition(0.0f, 0.2f);
	mapEditorButton->setColor(0x4f3f3f3f);
	mapEditorButton->setCaptionColor(0xffffffff);
	mapEditorButton->setCaptionSize(7);
	mapEditorButton->setCaption(L"MAP EDITOR");
	mapEditorButton->setFunctionPointer(mapEditorClick);
	mapEditorButton->setInnerImage(selectionImage);
	mapEditorButton->setInnerColor(0xffffffff);
	mapEditorButton->setOuterColor(0x00000000);
	mainButtonsPanel->addComponent(mapEditorButton);

	optionsButton = new psGui::SimpleButton();
	optionsButton->setTextHorizontalProportion(0.95f);
	optionsButton->setTextAlignment(DT_LEFT | DT_VCENTER);
	optionsButton->setSize(0.26f, 0.045f);
	optionsButton->setRelativePosition(0.0f, 0.15f);
	optionsButton->setColor(0x4f3f3f3f);
	optionsButton->setCaptionSize(7);
	optionsButton->setCaptionColor(0xffffffff);
	optionsButton->setCaption(L"OPTIONS");
	optionsButton->setFunctionPointer(optionsClick);
	optionsButton->setInnerImage(selectionImage);
	optionsButton->setInnerColor(0xffffffff);
	optionsButton->setOuterColor(0x00000000);
	mainButtonsPanel->addComponent(optionsButton);

	quitButton = new psGui::SimpleButton();
	quitButton->setTextHorizontalProportion(0.95f);
	quitButton->setTextAlignment(DT_LEFT | DT_VCENTER);
	quitButton->setSize(0.26f, 0.045f);
	quitButton->setRelativePosition(0.0f, 0.1f);
	quitButton->setColor(0x4f3f3f3f);
	quitButton->setCaption(L"QUIT");
	quitButton->setCaptionSize(7);
	quitButton->setCaptionColor(0xffffffff);
	quitButton->setFunctionPointer(quitClick); 
	quitButton->setInnerImage(selectionImage);
	quitButton->setInnerColor(0xffffffff);
	quitButton->setOuterColor(0x00000000);
	mainButtonsPanel->addComponent(quitButton);


	mainButtonsPanel->setVisible(true);
	mainButtonsPanel->setColor(0x00000000);
	mainButtonsPanel->setPosition(-0.8f, -0.75f);


	//JOIN GAME FRAME CREATION
	jsFrame = new psGui::Frame(0.0f, 0.0f, 0.5f, 0.5f);
	jsFrame->setColor(0xcf1f1f1f);

	jsHeader = new psGui::Simple();
	jsHeader->setSize(0.5f, 0.05f);
	jsHeader->setColor(0x9f0f0f10);
	jsHeader->setRelativePosition(0.0f, 0.225f);
	jsFrame->addComponent(jsHeader);

	jsHeaderTitle = new psGui::Label();
	jsHeaderTitle->setSize(0.5f, 0.05f);
	jsHeaderTitle->setTextColor(0xffffffff);
	jsHeaderTitle->setRelativePosition(0.0f, 0.225f);
	jsHeaderTitle->setText(L"Join Game Server");
	jsHeaderTitle->setTextSize(6);
	jsHeaderTitle->setTextAlignment(DT_CENTER | DT_VCENTER);
	jsFrame->addComponent(jsHeaderTitle);

	jsIP = new psGui::TextField();
	jsIP->setSize(0.3f, 0.05f);
	jsIP->setFieldColor(0xcf2f2f2f);
	jsIP->setTextColor(0xffffffff);
	jsIP->setTextSize(6);
	jsIP->setRelativePosition(0.0f, 0.0f);
	jsIP->setActive(false);
	jsFrame->addComponent(jsIP);

	jsJoinButton = new psGui::SimpleButton();
	jsJoinButton->setSize(0.2f, 0.05f);
	jsJoinButton->setRelativePosition(-0.13f, -0.2f);
	jsJoinButton->setColor(0x3f8f0fef);
	jsJoinButton->setCaptionColor(0xffffffff);
	jsJoinButton->setCaption(L"Join");
	jsJoinButton->setFunctionPointer(jsJoinClick);
	jsJoinButton->setCaptionSize(4);
	jsFrame->addComponent(jsJoinButton);

	jsCancelButton = new psGui::SimpleButton();
	jsCancelButton->setSize(0.2f, 0.05f);
	jsCancelButton->setRelativePosition(0.13f, -0.2f);
	jsCancelButton->setColor(0x3f8f0fef);
	jsCancelButton->setCaptionColor(0xffffffff);
	jsCancelButton->setCaption(L"Cancel");
	jsCancelButton->setFunctionPointer(jsCancelClick);
	jsCancelButton->setCaptionSize(4);
	jsFrame->addComponent(jsCancelButton);

	jsFrame->setPosition(0.0f, 0.0f);
	jsFrame->setVisible(false);


	connectionFrame = new psGui::Frame(0.0f, 0.0f, 0.8f, 0.2f);
	connectionFrame->setColor(0x9f1f1f1f);
	connectionFrame->setVisible(false);

	connectionLabel = new psGui::Label();
	connectionLabel->setSize(0.8f, 0.2f);
	connectionLabel->setTextColor(0xffffffff);
	connectionLabel->setRelativePosition(0.0f, 0.0f);
	connectionLabel->setText(L"connocting poopie");
	connectionLabel->setTextSize(6);
	connectionLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	connectionFrame->addComponent(connectionLabel);

	connectionFrame->setPosition(0.0f, 0.0f);
	connectionFrame->setVisible(false);

	
}

void joinGameClick(void)
{
	jsFrame->setVisible(true);
	jsIP->setActive(true);

	connectionFrame->setVisible(false);
	//lastInstance->modCharObject.setFrameVisibility(false);
}

void createGameClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->getMatchmakingMenu()->setFrameVisibility(true);
	lastInstance->getOptionsMenu()->setFrameVisibility(false);
	lastInstance->getMatchmakingMenu()->getCreateLobbyMenu().setFrameVisibility(false);

	//connectionFrame->setVisible(false);
	////modCharObject.setFrameVisibility(false);
	//S_Core::goToInterfaceObject(new SSingleMatchMenu());
}

void modCharClick(void)
{
	jsFrame->setVisible(false);
	jsIP->setActive(false);
	//modCharObject->setFrameVisibility(true);
}
void mapEditorClick(void)
{
	S_Core::goToInterfaceObject(new SEditor());
}

SOptionsMenu* SMainMenu::getOptionsMenu(void)
{
	return optionsMenu;
}
SMatchmakingMenu* SMainMenu::getMatchmakingMenu(void)
{
	return matchmakingMenu;
}

void SMainMenu::setManualJoinFrameVisibility(bool visible)
{
	jsFrame->setVisible(visible);
}
void optionsClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	lastInstance->getOptionsMenu()->setFrameVisibility(true);//!lastInstance->getOptionsMenu()->getFrameVisibility());
	lastInstance->getMatchmakingMenu()->setFrameVisibility(false);
	lastInstance->getMatchmakingMenu()->getCreateLobbyMenu().setFrameVisibility(false);
	lastInstance->getMatchmakingMenu()->getJoinLobbyMenu().setFrameVisibility(false);
}

void quitClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	shouldQuit = true;
}
#include "GameConfigInterface.h"
void jsJoinClick(void)
{
	//put the string from the text field into a normal string

	std::wstring firstString = jsIP->getText();
	std::string converted(firstString.begin(), firstString.end()); 

	jsFrame->setVisible(false);
	connectionFrame->setVisible(true);
	connectionLabel->setText(L"connocting poopie");
	SRender::getSingleton()->render();
	S_GUI::forcePresent();

	ps::GameInitInfo gii;
	gii.ip = converted.c_str();
	gii.screenName = GlobalConfiguration::getSingleton()->str_screenName();
	int response = SGameClient::getSingleton()->connectToGameServer(&gii);
	
	if (response == -1) connectionLabel->setText(L"No response from server");
	if (response == 1) connectionLabel->setText(L"Server is Full");
	if (response == 0)
	{
		S_Core::goToInterfaceObject(new SSGame(&gii));
	}
}

void jsCancelClick(void)
{
	
	jsFrame->setVisible(false);
	jsIP->setActive(false);
}