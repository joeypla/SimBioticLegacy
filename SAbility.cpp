#include "SAbility.h"
#include "SRender.h"
std::map<std::string, SAbility*> SAbility::_abilities;

//Wallhack - parameters
static const int WALLHACK_TIER1_FLUX = 250;

//Blink - parameters
static const int BLINK_TIER1_FLUX = 600;

//Firebolt - parameters
static const int FIREBOLT_TIER1_FLUX = 0;
#include "psGameClient.h"
SAbility* SAbility::new_instance(std::string name, bool isServerInstance)
{
	//find it in the map to make sure it exists
	std::map<std::string, SAbility*>::iterator it = _abilities.find(name);
	if (it == _abilities.end()) return NULL;

	
	SAbility* instance = 0;
	SAbility* original = it->second;

	//Here we allocate a different class so that ability specific functions can be overriden
	if (name.compare("wallhack") == 0) instance = new SAbility_Wallhack();
	if (name.compare("blink") == 0) instance = new SAbility_Blink();
	if (name.compare("firebolt") == 0) instance = new SAbility_Firebolt();

	if (instance == 0) return NULL;

	instance->_isActive = false;
	instance->_name = original->_name;
	instance->_properName = original->_properName;
	instance->_flux = original->_flux;
	instance->_castType = original->_castType;
	instance->_cooldown = original->_cooldown;
	instance->_numTiers = original->_numTiers;
	instance->_duration = original->_duration;

	if (!isServerInstance)
	{
		instance->_icon = original->_icon;
		instance->_castSound = original->_castSound;
	}
	return instance;
}
void SAbility::loadAbilities(void)
{
	//Wallhack
	SAbility_Wallhack* wallhack = new SAbility_Wallhack();
	wallhack->_name = "wallhack";
	wallhack->_properName = "Wallhack";
	wallhack->_numTiers = 1;
	wallhack->_flux.push_back(WALLHACK_TIER1_FLUX);
	wallhack->_icon = new SImage(L"../Resources/Textures/Abilities/Wallhack/icon.jpg");
	wallhack->_castSound = S_AUDIO::getSingleton()->create3DSound("../Resources/Audio/Sounds/Abilities/Wallhack/cast.wav");
	_abilities.insert(std::pair<std::string, SAbility*>(wallhack->_name, wallhack));

	//Blink
	SAbility_Blink* blink = new SAbility_Blink();
	blink->_name = "blink";
	blink->_properName = "Blink";
	blink->_numTiers = 1;
	blink->_flux.push_back(BLINK_TIER1_FLUX);
	blink->_icon = new SImage(L"../Resources/Textures/Abilities/Blink/icon.png");
	blink->_castSound = S_AUDIO::getSingleton()->create3DSound("../Resources/Audio/Sounds/Abilities/Blink/cast.mp3");
	_abilities.insert(std::pair<std::string, SAbility*>(blink->_name, blink));

	//Firebolt
	SAbility_Firebolt* firebolt = new SAbility_Firebolt();
	firebolt->_name = "firebolt";
	firebolt->_properName = "Fire Bolt";
	firebolt->_numTiers = 1;
	firebolt->_flux.push_back(FIREBOLT_TIER1_FLUX);
	firebolt->_icon = new SImage(L"../Resources/Textures/Abilities/Firebolt/icon.png");
	firebolt->_castSound = wallhack->_castSound;
	_abilities.insert(std::pair<std::string, SAbility*>(firebolt->_name, firebolt));

}

void SAbility::unloadAbilities(void)
{
}

SAbility::SAbility(void)
{
	_isActive = false;
	_castType = 0;
	_numTiers = 1;
	_icon = NULL;
	_castSound = NULL;
}

SAbility::~SAbility(void)
{
}

void SAbility::serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction)
{
}

void SAbility::serverUpdate(void)
{
}

void SAbility::serverOnImpact(void)
{
}

void SAbility::clientCast(ps::GameData* pGameData, ps::Actor* pCaster)
{
	SGameClient::getSingleton()->sendAbilityUsePacket(this, pCaster);
}

void SAbility::clientUpdate(void)
{
}

void SAbility::clientOnImpact(void)
{
}

bool SAbility::getActive(void)
{
	return _isActive;
}
int SAbility::getCastType(void)
{
	return _castType;
}
int SAbility::getFlux(int index)
{
	if (index < 0 || index >= _flux.size()) return 0;

	return _flux.at(index);
}

std::string SAbility::getName(void)
{
	return _name;
}

std::string SAbility::getProperName(void)
{
	return _properName;
}

SImage* SAbility::getIcon(void)
{
	return _icon;
}

psAudio::Sound* SAbility::getCastSound(void)
{
	return _castSound;
}