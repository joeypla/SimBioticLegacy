 #include "psHUD.h"
#include "SInput.h"
#include "SLOG.h"
#include <sstream>

#include "SGameStateDefines.h"

static SText* gameStateText=   0;

//Crosshair lines
static S2D* crosshair_left=    0;
static S2D* crosshair_right=   0;
static S2D* crosshair_top=     0;
static S2D* crosshair_bottom=  0;
static S2D* crosshair_dot=     0;

//Hitmarker
static S2D* hitmarker=         0;
static float hitmarkerLife=    0.0f;

//Character icon display
static S2D* character_icon=    0;
//Abilities and flux
static S2D* flux_back=         0;
static S2D* flux_bar=          0;
static S2D* abilityIcon[4];
//shield bar
static SText* shield_number=   0;
static S2D* shield_back=       0;
static S2D* shield_display=    0;
//health bar
static SText* health_number=   0;
static S2D* health_back=       0;
static S2D* health_display=    0;

//AMMO AND SHIT
static SText* equipment_ammo=  0;
static SText* equipment_weapon=0;
static S2D* equipment_large=   0;
static S2D* equipment_small=   0;

//award text and shit
static float award_fadeFactor= 0.0f;
static SText* award_amount=    0;
static SText* award_info=      0;

//enemy stats bar
static ps::Actor*       enemy_last = 0;
static float enemy_fadeFactor = 0.0f;
static SText* enemy_name=      0;
static S2D* enemy_backbar=     0;
static S2D* enemy_bar=         0;

//MAIN TOP AND BOTTOM PANEL THINGS
static S2D* bottomPanel=       0;
static S2D* topPanel=          0;

//HUD gravity shifter
static S2D* gravityShifterBack=0;

//debug
static SText* debug_playerDir= 0;
static SText* debug_playerUp=  0;
static SText* debug_playerDerivedStrafe= 0;

static psAudio::Sound* abilityAchieve1= 0;
static psAudio::Sound* abilityAchieve2= 0;
static psAudio::Sound* abilityAchieve3= 0;
static psAudio::Sound* abilityAchieve4= 0;
void psHUD::setup(void)
{
	float aspectRatio = (float)SRender::getSingleton()->getWidth() / (float)SRender::getSingleton()->getHeight();

	gameStateText = SRender::getSingleton()->createText();
	gameStateText->setPosition(0.0f, 0.9f);
	gameStateText->setText(L"Game begins in 49");
	gameStateText->setAlignment(DT_CENTER | DT_VCENTER);
	gameStateText->setDimensions(0.4f, 0.1f);
	gameStateText->setColor(0xffffffff);
	gameStateText->setSize(10);
	gameStateText->setVisible(true);

	//Shield
	shield_number = SRender::getSingleton()->createText();
	shield_number->setDimensions(0.3f, 0.05f);
	shield_number->setPosition(-0.8f, -0.8f);
	shield_number->setText(L"100");
	shield_number->setSize(7);
	shield_number->setAlignment(DT_VCENTER|DT_CENTER);
	shield_number->setColor(0xdfffffff);
	shield_number->setVisible(false);

	shield_back = SRender::getSingleton()->createS2D();
	shield_back->setColor(0x8f2f2f2f);
	shield_back->setPosition(-0.8f, -0.8f);
	shield_back->setDimensions(0.31f, 0.05f);
	shield_back->setVisible(false);

	shield_display = SRender::getSingleton()->createS2D();
	shield_display->setColor(0x8f009fcf);
	shield_display->setPosition(-0.8f, -0.8f);
	shield_display->setDimensions(0.3f, 0.04f);
	shield_display->setVisible(false);
	//Health
	health_number = SRender::getSingleton()->createText();
	health_number->setDimensions(0.3f, 0.05f);
	health_number->setPosition(-0.8f, -0.9f);
	health_number->setText(L"100");
	health_number->setSize(7);
	health_number->setAlignment(DT_VCENTER|DT_CENTER);
	health_number->setColor(0xdfffffff);
	health_number->setVisible(false);

	health_back = SRender::getSingleton()->createS2D();
	health_back->setColor(0x8f2f2f2f);
	health_back->setPosition(-0.8f, -0.9f);
	health_back->setDimensions(0.31f, 0.05f);
	health_back->setVisible(false);

	health_display = SRender::getSingleton()->createS2D();
	health_display->setColor(0x5fcf0000);
	health_display->setPosition(-0.8f, -0.9f);
	health_display->setDimensions(0.3f, 0.04f);
	health_display->setVisible(false);
	//Crosshair
	crosshair_left = SRender::getSingleton()->createS2D();
	crosshair_left->setColor(0xaf9f9f9f);
	crosshair_left->setDimensions(0.02f, 0.003f);
	crosshair_left->setPosition(-0.05f, 0.0f);
	crosshair_right = SRender::getSingleton()->createS2D();
	crosshair_right->setColor(0xaf9f9f9f);
	crosshair_right->setDimensions(0.02f, 0.003f);
	crosshair_right->setPosition(0.05f, 0.0f);
	crosshair_top = SRender::getSingleton()->createS2D();
	crosshair_top->setColor(0xaf9f9f9f);
	crosshair_top->setDimensions(0.003f / aspectRatio, 0.03f);
	crosshair_top->setPosition(0.0f, 0.075f);
	crosshair_bottom = SRender::getSingleton()->createS2D();
	crosshair_bottom->setColor(0xaf9f9f9f);
	crosshair_bottom->setDimensions(0.003f / aspectRatio, 0.03f);
	crosshair_bottom->setPosition(0.0f, -0.075f);
	crosshair_dot = SRender::getSingleton()->createS2D();
	crosshair_dot->setColor(0xaf9f0000);
	crosshair_dot->setDimensions(0.01f/aspectRatio, 0.01f);
	crosshair_dot->setPosition(0.0f, 0.0f);
	crosshair_dot->setTextureFromPath(L"../Resources/Textures/Particles/basicGlowParticle.png");
	//hitmarker
	hitmarker = SRender::getSingleton()->createS2D();
	hitmarker->setColor(0xaf9f9f9f);
	hitmarker->setDimensions(0.02f, 0.003f);
	hitmarker->setPosition(0.0f, -0.05f);

	//equipment
	equipment_ammo = SRender::getSingleton()->createText();
	equipment_ammo->setPosition(0.8f, 0.76f);
	equipment_ammo->setAlignment(DT_CENTER | DT_VCENTER);
	equipment_ammo->setDimensions(0.2f, 0.1f);
	equipment_ammo->setColor(0x8fffffff);
	equipment_ammo->setSize(10);
	equipment_ammo->setVisible(true);

	equipment_weapon = SRender::getSingleton()->createText();
	equipment_weapon->setPosition(0.8f, -0.6f);
	equipment_weapon->setAlignment(DT_RIGHT | DT_VCENTER);
	equipment_weapon->setDimensions(0.1f, 0.02f);
	equipment_weapon->setColor(0xff000000);
	equipment_weapon->setSize(10);
	equipment_weapon->setVisible(false);


	equipment_large = SRender::getSingleton()->createS2D();
	equipment_large->setColor(0xffffffff);
	equipment_large->setDimensions(0.25f, 0.2f);
	equipment_large->setPosition(0.8f, 0.8f);
	equipment_large->setVisible(true);
	equipment_large->setTextureFromPath(L"../Resources/Textures/HUD/weaponback.png");

	equipment_small = SRender::getSingleton()->createS2D();
	equipment_small->setColor(0xaf9f9f9f);
	equipment_small->setDimensions(0.2f, 0.1f);
	equipment_small->setPosition(0.8f, -0.9f);
	equipment_small->setVisible(false);

	award_amount = SRender::getSingleton()->createText();
	award_amount->setPosition(0.0f, 0.6f);
	award_amount->setText(L"+100");
	award_amount->setAlignment(DT_CENTER | DT_VCENTER);
	award_amount->setDimensions(0.5f, 0.1f);
	award_amount->setColor(0xff000000);
	award_amount->setSize(10);

	award_info = SRender::getSingleton()->createText();
	award_info->setPosition(0.0f, 0.7f);
	award_info->setText(L"Termination");
	award_info->setAlignment(DT_CENTER | DT_VCENTER);
	award_info->setDimensions(0.5f, 0.1f);
	award_info->setColor(0xff000000);
	award_info->setSize(10);

	enemy_backbar = SRender::getSingleton()->createS2D();
	enemy_backbar->setColor(0x8f2f2f2f);
	enemy_backbar->setPosition(0.0f, 0.9f);
	enemy_backbar->setDimensions(0.33f, 0.06f);
	enemy_backbar->setVisible(true);

	enemy_bar = SRender::getSingleton()->createS2D();
	enemy_bar->setColor(0x8f0000ff);
	enemy_bar->setPosition(0.0f, 0.9f);
	enemy_bar->setDimensions(0.31f, 0.05f);
	enemy_bar->setVisible(true);

	//FACE VISOR PANELS
	bottomPanel = SRender::getSingleton()->createS2D();
	bottomPanel->setColor(0xffffffff);
	bottomPanel->setPosition(0.0f, -0.9f);
	bottomPanel->setDimensions(2.0f, 0.2f);
	bottomPanel->setVisible(true);
	bottomPanel->setTextureFromPath(L"../Resources/Textures/HUD/bottom.png");

	topPanel = SRender::getSingleton()->createS2D();
	topPanel->setColor(0xffffffff);
	topPanel->setPosition(0.0f, 0.9f);
	topPanel->setDimensions(2.0f, 0.2f);
	topPanel->setVisible(true);
	topPanel->setTextureFromPath(L"../Resources/Textures/HUD/top.png");

	//Gravity Shifter Back
	gravityShifterBack = SRender::getSingleton()->createS2D();
	gravityShifterBack->setColor(0xffffffff);
	gravityShifterBack->setPosition(0.0f, 0.75f);
	gravityShifterBack->setDimensions(0.2f/aspectRatio, 0.2f);
	gravityShifterBack->setVisible(true);
	gravityShifterBack->setTextureFromPath(L"../Resources/Textures/HUD/gravityShifter.png");

	////debug
	debug_playerDir = SRender::getSingleton()->createText();
	debug_playerDir->setDimensions(0.4f, 0.2f);
	debug_playerDir->setPosition(-0.8f, 0.9f);
	debug_playerDir->setText(L"100");
	debug_playerDir->setSize(6);
	debug_playerDir->setAlignment(DT_VCENTER|DT_LEFT);
	debug_playerDir->setColor(0xdfffffff);
	debug_playerDir->setVisible(false);

	debug_playerUp = SRender::getSingleton()->createText();
	debug_playerUp->setDimensions(0.4f, 0.2f);
	debug_playerUp->setPosition(-0.8f, 0.85f);
	debug_playerUp->setText(L"100");
	debug_playerUp->setSize(6);
	debug_playerUp->setAlignment(DT_VCENTER|DT_LEFT);
	debug_playerUp->setColor(0xdfffffff);
	debug_playerUp->setVisible(false);

	debug_playerDerivedStrafe = SRender::getSingleton()->createText();
	debug_playerDerivedStrafe->setDimensions(0.4f, 0.2f);
	debug_playerDerivedStrafe->setPosition(-0.8f, 0.8f);
	debug_playerDerivedStrafe->setText(L"100");
	debug_playerDerivedStrafe->setSize(4 );
	debug_playerDerivedStrafe->setAlignment(DT_VCENTER|DT_LEFT);
	debug_playerDerivedStrafe->setColor(0xdfffffff);
	debug_playerDerivedStrafe->setVisible(false);

	character_icon = SRender::getSingleton()->createS2D();
	character_icon->setColor(0xffffffff);
	character_icon->setPosition(-0.8f, -0.8f);
	character_icon->setDimensions(0.2f, 0.2f*aspectRatio);
	character_icon->setVisible(false);

	//Flux bar stuff
	flux_back = SRender::getSingleton()->createS2D();
	flux_back->setColor(0x8f2f2f2f);
	flux_back->setPosition(0.0f, -0.9f);
	flux_back->setDimensions(1.31f, 0.05f);
	flux_back->setVisible(false);

	flux_bar = SRender::getSingleton()->createS2D();
	flux_bar->setColor(0x9fffffff);
	flux_bar->setPosition(0.0f, -0.9f);
	flux_bar->setDimensions(1.3f, 0.04f);
	flux_bar->setVisible(false);

	for (int i = 0; i < 4; i++)
	{
		abilityIcon[i] = SRender::getSingleton()->createS2D();
		abilityIcon[i]->setColor(0xffffffff);
		abilityIcon[i]->setPosition(0.0f, -0.9f);
		abilityIcon[i]->setDimensions(0.1f, 0.1f*aspectRatio);
		abilityIcon[i]->setVisible(false);
	}

	abilityAchieve1 = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/Abilities/achieve1.wav");
	abilityAchieve2 = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/Abilities/achieve1.wav");
	abilityAchieve3 = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/Abilities/achieve1.wav");
	abilityAchieve4 = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/Abilities/achieve1.wav");
}



void psHUD::update(ps::GameData* pGameData)
{
	//update game state text
	if (pGameData->gameState == SGAMESTATE_PREMATCH)
	{
		std::wstringstream ss;
		ss<<L"The game will begin in "<<(unsigned int)pGameData->getMinutesLeft()<<":"<<(unsigned int)pGameData->getSecondsLeft();
		gameStateText->setText(ss.str().c_str());
	} else if (pGameData->gameState == SGAMESTATE_PLAYING)
	{
		std::wstringstream ss;
		ss<<L"Sector Control - "<<(unsigned int)pGameData->getMinutesLeft()<<":"<<(unsigned int)pGameData->getSecondsLeft();
		gameStateText->setText(ss.str().c_str());
	} else if (pGameData->gameState == SGAMESTATE_OVER)
	{
		gameStateText->setText(L"Match Over");
	}
	//update crosshairs according to weapon spread
	//get weapon

	//HUD applies to THIS CLIENT ONLY - therefore we have to take the actor that the client IS on the server
	ps::Actor* player = pGameData->pClientActor();
	ps::Weapon* weapon;

	if (player->weaponSelected() == 1)
		weapon = player->weapon1();
	else if(player->weaponSelected() == 2)
		weapon = player->weapon2();
	else
		weapon = 0;

	float aspectRatio = (float)SRender::getSingleton()->getWidth() / (float)SRender::getSingleton()->getHeight();
	crosshair_left->setPosition(-weapon->spread, 0.0f);
	crosshair_right->setPosition(weapon->spread, 0.0f);
	crosshair_top->setPosition(0.0f, weapon->spread * aspectRatio);
	crosshair_bottom->setPosition(0.0f, -weapon->spread * aspectRatio);
	if (weapon != 0)
	{
		if (weapon->aiming > 0.9f)
		{
			crosshair_dot->setVisible(true);
			crosshair_left->setVisible(false);
			crosshair_right->setVisible(false);
			crosshair_bottom->setVisible(false);
			crosshair_top->setVisible(false);
			if (weapon->hasDot())
			{
				crosshair_dot->setVisible(true);
			}
		} 
		else
		{
			crosshair_dot->setVisible(false);
			crosshair_left->setVisible(true);
			crosshair_right->setVisible(true);
			crosshair_bottom->setVisible(true);
			crosshair_top->setVisible(true);
		}
		
	} 
	else 
	{
		crosshair_dot->setVisible(false);
		crosshair_left->setVisible(false);
		crosshair_right->setVisible(false);
		crosshair_bottom->setVisible(false);
		crosshair_top->setVisible(false); 
	}

	equipment_ammo->setText(std::to_wstring(weapon->ammoMag()).c_str());
	equipment_weapon->setText(weapon->name());

	//update hitmarker
	hitmarkerLife -= 0.1f;
	if (hitmarkerLife < 0.0f) hitmarkerLife = 0.0f;
	float markerAlpha = (hitmarkerLife > 1.0f) ? 1.0f : hitmarkerLife;
	hitmarker->setColor(D3DCOLOR_ARGB((int)(markerAlpha*255.0f), 150, 150, 150));


	////debug
	std::wstringstream debugStringStream;
	debugStringStream.str(L"");
	debugStringStream<<L"Position: "<<player->x<<L", "<<player->y<<L", "<<player->z;
	debug_playerDir->setText(debugStringStream.str().c_str());

	debugStringStream.str(L"");
	debugStringStream<<L"viewUp: "<<player->view()->ux<<L", "<<player->view()->uy<<L", "<<player->view()->uz;
	debug_playerUp->setText(debugStringStream.str().c_str());

	debugStringStream.str(L"");
	psMath::float3 dStrafe = psMath::cross(&psMath::float3(player->view()->dx, player->view()->dy, player->view()->dz), &psMath::float3(player->view()->ux, player->view()->uy, player->view()->uz));
	debugStringStream<<L"derivedStrafe: "<<dStrafe.x<<L", "<<dStrafe.y<<L", "<<dStrafe.z;
	debug_playerDerivedStrafe->setText(debugStringStream.str().c_str());

	
	
	//update health and shield
	health_number->setText(std::to_wstring(player->health).c_str());
	shield_number->setText(std::to_wstring(player->shield).c_str());
	
	health_display->setDimensions(0.3f * (float)player->health / 100.0f, 0.05f);
	shield_display->setDimensions(0.3f * (float)player->shield / 100.0f, 0.05f);
	//update award thingy
	award_fadeFactor -= 0.1f;
	if (award_fadeFactor < 0.0f) award_fadeFactor = 0.0f;
	if (award_fadeFactor > 1.0f)
	{
		award_info->setColor(0xffffffff);
		award_amount->setColor(0xffffffff);
	} else if (award_fadeFactor > 0.01f && award_fadeFactor <= 1.0f)
	{
		award_info->setColor((int)(award_fadeFactor*255), 255, 255, 255);
		award_amount->setColor((int)(award_fadeFactor*255), 255, 255, 255);
	} else 
	{
		award_info->setColor(0x00ffffff);
		award_amount->setColor(0x00ffffff);
	}


	//update enemy bar
	enemy_fadeFactor -= 0.1f;
	if (enemy_fadeFactor < 0.0f) enemy_fadeFactor = 0.0f;

	float alphaValue = 0.0f;
	if (enemy_fadeFactor > 1.0f)
	{
		alphaValue = 1.0f;
	} else if (award_fadeFactor > 0.01f && award_fadeFactor <= 1.0f)
	{
		alphaValue = award_fadeFactor;
	} else 
	{
		alphaValue = 0.0f;
	}

	//if the shield is higher than 0 display that, otherwise we display the health
	if (enemy_last != 0)
	{
		  
		if (enemy_last->shield > 0)
		{
			enemy_bar->setColor(D3DCOLOR_ARGB((int)(alphaValue * 0.7f * 255), 0, 127, 250));
			enemy_bar->setDimensions(0.31f * (float)enemy_last->shield / 100.0f, 0.05f);
		} else 
		{
			enemy_bar->setColor(D3DCOLOR_ARGB((int)(alphaValue * 0.7f * 255), 255, 30, 30));
			enemy_bar->setDimensions(0.31f * (float)enemy_last->health / 100.0f, 0.05f);
		}
	}
	enemy_backbar->setColor(D3DCOLOR_ARGB((int)(alphaValue * 0.7f * 255), 60, 60, 60));
	enemy_backbar->setVisible(false);
	enemy_bar->setVisible(false);



	//character icon
	SCharacter* playerCharacter = player->getCharacter();
	if (playerCharacter != 0 && playerCharacter->getIcon() != 0)
		character_icon->setImage(playerCharacter->getIcon());
	//update flux and ability stuff

	//flux bar
	float length = (float)player->flux / 1200.0f * 1.3f;
	flux_bar->setDimensions(length, 0.04f);
	flux_bar->setPosition(0.0f -  (1.3f*0.5f - length*0.5f), -0.9f);

	//abilities
	
	if (playerCharacter != NULL)
	{
		
		int i = 0;
		for (;;)
		{
			SAbility* ability = player->getCharacter()->getAbility(i);
			if (ability == 0) break;

			abilityIcon[i]->setImage(ability->getIcon());
			int flux = ability->getFlux(0);
			float fluxRatio = (float)flux / 1200.0f;
			float offset = 1.3f*0.5f * (fluxRatio - 0.5f) * 2.0f;

			abilityIcon[i]->setPosition(offset, -0.8f);
			if (flux <= player->flux) abilityIcon[i]->setColor(0xffffffff);
			else abilityIcon[i]->setColor(0x7fffffff);

			///TOOOO CHANGE BACK TO TRUE
			abilityIcon[i]->setVisible(false);

			i++;
			if (i > 3) break;
		}
		if (i == 2) S_LOG::write("the character is shown to have two abilities!");
	}




	static float gravityShiftShow = 0.0f;
	if (S_INPUT::keyPressed(DIK_F))
		gravityShiftShow += 0.1f;
	else gravityShiftShow -= 0.1f;

	if (gravityShiftShow > 1.0f) gravityShiftShow = 1.0f;
	if (gravityShiftShow < 0.0f) gravityShiftShow = 0.0f;

	gravityShifterBack->setPosition(0.0f, 0.75f + 0.2f*(1.0f - gravityShiftShow));
	gravityShifterBack->setColor(D3DCOLOR_ARGB((int)(gravityShiftShow*255.0f), 255, 255, 255));
}

void psHUD::resetHitmarker(void)
{
	hitmarkerLife = 2.0f;
	
}

void psHUD::award(int amount, const wchar_t* info)
{
	award_info->setText(info);
	award_amount->setText(std::to_wstring(amount).c_str());
	award_fadeFactor = 10.0f;

}

void psHUD::showLastEnemy(ps::Actor* lastEnemy)
{
	if (lastEnemy == 0) return;
	enemy_fadeFactor = 10.0f;
	enemy_last = lastEnemy;
	//first we decide what to display, health or shield.
	
}