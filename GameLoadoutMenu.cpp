                                                                                                                                                                                                       #include "GameLoadoutMenu.h"

static GameLoadoutMenu* lastInstance = 0;

static void magentorClick(void);
static void conspiracyClick(void);
static void prophetClick(void);

static void team1Click(void);
static void team2Click(void);
#include "Game.h"
GameLoadoutMenu::GameLoadoutMenu(void)
{
	
	lastInstance = this;

	_frame = new psGui::Frame(0.f, 0.f, 0.6f, 0.6f);
	_frame->setColor(0x9f1f1f1f);

	_magentorButton = new psGui::SimpleButton();
	_magentorButton->setSize(0.26f, 0.07f);
	_magentorButton->setRelativePosition(0.0f, 0.2f);
	_magentorButton->setColor(0x4f3f3f3f);
	_magentorButton->setCaptionSize(7);
	_magentorButton->setCaptionColor(0xffffffff);
	_magentorButton->setCaption(L"Magentor");
	//_magentorButton->setFunctionPointer(resumeClick);
	_frame->addComponent(_magentorButton);

	_conspiracyButton = new psGui::SimpleButton();
	_conspiracyButton->setSize(0.26f, 0.07f);
	_conspiracyButton->setRelativePosition(0.0f, 0.1f);
	_conspiracyButton->setColor(0x4f3f3f3f);
	_conspiracyButton->setCaption(L"Conspiracy");
	_conspiracyButton->setCaptionSize(7);
	_conspiracyButton->setCaptionColor(0xffffffff);
	//_conspiracyButton->setFunctionPointer(leaveLobbyClick);
	_frame->addComponent(_conspiracyButton);

	_prophetButton = new psGui::SimpleButton();
	_prophetButton->setSize(0.26f, 0.07f);
	_prophetButton->setRelativePosition(0.0f, 0.0f);
	_prophetButton->setColor(0x4f3f3f3f);
	_prophetButton->setCaption(L"Prophet");
	_prophetButton->setCaptionSize(7);
	_prophetButton->setCaptionColor(0xffffffff);
	//_prophetButton->setFunctionPointer(quitClick);
	_frame->addComponent(_prophetButton);

	_frame->setPosition(0.0f, 0.0f);
	_frame->setVisible(false);
}

GameLoadoutMenu::~GameLoadoutMenu(void)
{
	delete _frame;
	delete _magentorButton;
	delete _conspiracyButton;
	delete _prophetButton;
}


void GameLoadoutMenu::receiveSystemMessage(UINT message, UINT param)
{
	if (_frame->getVisible()) _frame->sendSystemMessage(message, param);
}

int GameLoadoutMenu::update(void)
{
	if (_frame->getVisible()) _frame->update();

	if (actions.size() < 1) return 0;

	int actionToSend = actions.front();
	actions.pop_front();
	return actionToSend;
}

void GameLoadoutMenu::setFrameVisibility(bool visibility)
{
	_frame->setVisible(visibility);
}

bool GameLoadoutMenu::getFrameVisibility(void)
{
	return _frame->getVisible();
}

void GameLoadoutMenu::createEvent(int event)
{
	actions.emplace_back(event);
}

void magentorClick(void)
{
}

void conspiracyClick(void)
{
}

void prophetClick(void)
{
}

void team1Click(void)
{
}

void team2Click(void)
{
}

