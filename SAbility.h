#ifndef SABILITY_H
#define SABILITY_H

#define SABILITY_CASTTYPE_PASSIVE 0
#define SABILITY_CASTTYPE_ONCE 1
#define SABILITY_CASTTYPE_TOGGLE 2

#define SABILITY_NUMTIERS 3
namespace ps
{
	class GameData;
	class Actor;
};

#include <map>
#include <string>
class SImage;
#include <vector>
#include "SAUDIO.h"

class SGameServer;
class SAbility
{
public:
	SAbility(void);
	~SAbility(void);
	static void loadAbilities(void);
	static void unloadAbilities(void);

	static SAbility* new_instance(std::string name, bool isServerInstance);
	static void delete_instance(SAbility* instance);

	
	virtual void serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction);
	virtual void serverUpdate(void);
	virtual void serverOnImpact(void);//this is really only used if a projectile is launched

	virtual void clientCast(ps::GameData* pGameData, ps::Actor* pCaster);
	virtual void clientUpdate(void);
	virtual void clientOnImpact(void);

	int getFlux(int tier);
	int getCooldown(int tier);
	int getDuration(int tier);

	bool getActive(void);
	int getCastType(void);
	int getNumTiers(void);

	std::string getName(void);
	std::string getProperName(void);
	SImage* getIcon(void);
	psAudio::Sound* getCastSound(void);
private:
	
	static std::map<std::string, SAbility*> _abilities;
protected:
	
	std::string _name;//internal name
	std::string _properName;//public name

	bool _isActive;
	int _castType;
	int _numTiers;
	std::vector <int> _flux;
	std::vector <int> _cooldown;
	std::vector <int> _duration;
	SImage* _icon;

	psAudio::Sound* _castSound;
};
class SAbility_Firebolt : public SAbility
{
public:

	SAbility_Firebolt(void);
	SAbility_Firebolt(SAbility* original);

	~SAbility_Firebolt(void);

	void serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction);
	void serverUpdate(void);
	void serverOnImpact(void);

	void clientCast(ps::GameData* pGameData, ps::Actor* pCaster);
	void clientUpdate(void);
	void clientOnImpact(void);
private:
protected:
};
class SAbility_Blink : public SAbility
{
public:
	static SAbility* new_instance(void);
	static SAbility* delete_instance(void);

	SAbility_Blink(void);
	SAbility_Blink(SAbility* original);

	~SAbility_Blink(void);
	void serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction);
	void serverUpdate(void);
	void serverOnImpact(void);

	void clientCast(ps::GameData* pGameData, ps::Actor* pCaster);
	void clientUpdate(void);
	void clientOnImpact(void);

private:
	
protected:
	
};


class SAbility_Wallhack : public SAbility{
public:
	static SAbility* new_instance(void);
	static SAbility* delete_instance(void);
	SAbility_Wallhack(void);
	

	~SAbility_Wallhack(void);
	void serverCast(SGameServer* pServerData, int casterIndex, psMath::float3 position, psMath::float3 direction);
	void serverUpdate(void);
	void serverOnImpact(void);

	void clientCast(ps::GameData* pGameData, ps::Actor* pCaster);
	void clientUpdate(void);
	void clientOnImpact(void);
private:
protected:
	
};

class SAbility_GravityShift : public SAbility{
public:
	bool cast(void);
private:
protected:
};
#endif