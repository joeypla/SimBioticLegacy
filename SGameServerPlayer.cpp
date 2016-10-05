#include "SGameServerPlayer.h"

#include "SCharacter.h"
SGameServerPlayer::SGameServerPlayer(std::string name, sockaddr_in address)
{

	state = 1;
	state2 = 0;

	_flux = 0;
	_name = name;
	_address = address;

	x = 1000.0f;
	y = 1000.0f;
	z = 1000.0f;

	gravity = 2;
	dirx = 0.0f;
	diry = 0.0f;
	dirz = 0.0f;

	health = 100;
	shield = 100;

	kills = 0;
	deaths = 0;
	score = 0;
	modeSpecificVal1 = 0;
	modeSpecificVal2 = 0;
	modeSpecificVal3 = 0;

	shotByLast = 200;

	timeOfLastDeath = 0;
	_lastSeen = GetTickCount();

	team = 0;
	_pCharacter = SCharacter::new_instance("paragon", true);
}

SGameServerPlayer::~SGameServerPlayer(void)
{
}

void SGameServerPlayer::updateFromPacket(psNetworking::ClientOutPacket_PlayerUpdate_UDP* packet)
{
	x = packet->actor.x;
	y = packet->actor.y;
	z = packet->actor.z;

	dirx = packet->actor.dirx;
	diry = packet->actor.diry;
	dirz = packet->actor.dirz;

	gravity = packet->actor.gravity;
	state2 = packet->actor.state2;
	_lastSeen = GetTickCount();
}

void SGameServerPlayer::update(void)
{

	//update abilities if they are active (or passive with enough flux)
	if (_pCharacter != 0)
	{
		SAbility* ability;
		int i = 0;
		for(;;)
		{
			ability = _pCharacter->getAbility(i);

			if (ability == NULL)
				break;

			if (ability->getActive() || (ability->getCastType() == SABILITY_CASTTYPE_PASSIVE && _flux >= ability->getFlux(0)))
				ability->serverUpdate();

			i++;
		}
	}
}
sockaddr_in* SGameServerPlayer::pAddress(void)
{
	return &_address;
}

std::string SGameServerPlayer::name(void)
{
	return _name; 
}

bool SGameServerPlayer::getActive(void)
{
	DWORD currentTime = GetTickCount();
	if (currentTime - _lastSeen > 2000) return false;
	return true;
}

void SGameServerPlayer::increaseFlux(int amount)
{
	_flux += amount;
	if (_flux > 1200) _flux = 1200;
}

void SGameServerPlayer::zeroFlux(void)
{
	_flux = 0;
}

int SGameServerPlayer::getFlux(void)
{
	return _flux;
}

SCharacter* SGameServerPlayer::getCharacter(void)
{
	return _pCharacter;
}