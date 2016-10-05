#define _WINSOCKAPI_
#include "SPreMenu.h"
#include "SCore.h"
#include "SMainMenu.h"

static SRender* rdr;
static S2D* background;
static S2D* enthusiast;
static S2D* overlay;
LPDIRECT3DTEXTURE9 enthusiastTexture;
SCamera* cam;
DWORD timeWhenStarted;
SPreMenu::SPreMenu(void){
	_enthusiastSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/enthu.wav");
	setup();
}

SPreMenu::~SPreMenu(void){
	
	delete cam;

	rdr->releaseS2D(background);
	rdr->releaseS2D(enthusiast);
	rdr->releaseS2D(overlay);
}

bool SPreMenu::renderToDevice(const LPDIRECT3DDEVICE9 dev){
	rdr->render();
	return true;
}

bool SPreMenu::updateInterface(DWORD lastTimeTaken){
	static bool enthusiastSaid = false;
	if (!enthusiastSaid)
	{
		S_AUDIO::getSingleton()->playSoundOnce(_enthusiastSound);
		enthusiastSaid = true;
	}

	//find time from when pre screen started
	DWORD nowTime = GetTickCount();
	DWORD elapsed = nowTime - timeWhenStarted;

	if (elapsed < 5000)
	{

		int value = elapsed - 2500;
		float alphaAmount = (float)value / 2500.0f;
		float visibility = 1.0f - abs(alphaAmount);
		//if (alphaAmount > 1.0f) alphaAmount = 1.0f;
		enthusiast->setColor(D3DCOLOR_ARGB((char)(visibility*255.0f), 255, 255, 255));
	}

	if (elapsed > 5000)
	{
		S_Core::goToInterfaceObject(new SMainMenu());
	}
	return true;
}

void SPreMenu::receiveSystemMessage(UINT message, UINT wParam){
}

void SPreMenu::setup(void){
	ShowCursor(false);
	rdr = SRender::getSingleton();
	rdr->setClearColor(0xff000000);

	background = rdr->createS2D();
	background->setColor(0xff000000);
	background->setDimensions(2.0f, 2.0f);
	background->setPosition(0.0f, 0.0f);
	background->setVisible(true);

	enthusiast = rdr->createS2D();
	enthusiast->setTextureFromPath(L"../Resources/Textures/elogoblack.jpg");
	enthusiast->setDimensions(1.8f, 1.2f);
	enthusiast->setPosition(0.0f, 0.0f);
	enthusiast->setColor(0x00ffffff);
	enthusiast->setVisible(true);

	overlay = rdr->createS2D();
	overlay->setColor(0x00000000);
	overlay->setDimensions(2.0f, 2.0f);
	overlay->setPosition(0.0f, 0.0f);
	overlay->setVisible(true);

	cam = new SCamera();
	rdr->setCamera(cam);
	timeWhenStarted = GetTickCount();
}