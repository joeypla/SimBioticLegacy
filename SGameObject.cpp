#include "SGameObject.h"


#include "SLOG.h"

std::map<unsigned int, SGameObject*> SGameObject::_templates;

#include "SProjectile.h"

#include "SymbiotePacketStructs.h"

#include "SParticleSystem.h"

void SGameObject::loadGameObjectTemplates(void)
{
	//Firebolt
	SProjectile* firebolt = new SProjectile();
	firebolt->templateNumber = SGAMEOBJECT_NEWINSTANCE_FIREBOLT;
	firebolt->radius = 10.0f;
	firebolt->damage = 500;
	firebolt->terminatesAfterTime = false;
	firebolt->shieldMultiplier = 1.0f;
	firebolt->healthMultiplier = 1.0f;
	firebolt->_lightColor = psMath::float3(1.0f, 0.7f, 0.0f);
	firebolt->_lightRadius = 20.0f;

	SParticle* fireboltTrailParticle = new SParticle();
	fireboltTrailParticle->acceleration = psMath::float3(0.0f, 0.0f, 0.0f);
	fireboltTrailParticle->velocity = psMath::float3(0.0f, 0.0f, 0.0f);
	fireboltTrailParticle->life = 10.0f;
	fireboltTrailParticle->dLife = 0.6f;
	fireboltTrailParticle->color[0] = 0.6f;
	fireboltTrailParticle->color[1] = 0.4f;
	fireboltTrailParticle->color[2] = 0.0f;
	fireboltTrailParticle->color[3] = 1.0f;
	fireboltTrailParticle->dColor[0] = 0.0f;
	fireboltTrailParticle->dColor[1] = 0.0f;
	fireboltTrailParticle->dColor[2] = 0.0f;
	fireboltTrailParticle->dColor[3] = -0.06f;
	fireboltTrailParticle->scale = 0.3f;
	fireboltTrailParticle->dScale = 0.01f;

	firebolt->_trailParticle = fireboltTrailParticle;

	_templates.insert(std::pair<unsigned int, SGameObject*>(firebolt->templateNumber, firebolt));

	//Health Grenade
	SProjectile* healthGrenade = new SProjectile();
	healthGrenade->templateNumber = SGAMEOBJECT_NEWINSTANCE_HEALTHGRENADE;
	healthGrenade->radius = 10.0f;
	healthGrenade->damage = 200;
	healthGrenade->terminatesAfterTime = true;
	healthGrenade->shieldMultiplier = 0.5f;
	healthGrenade->healthMultiplier = 2.0f;

	SParticle* healthGrenadeTrailParticle = new SParticle();
	healthGrenadeTrailParticle->acceleration = psMath::float3(0.0f, 0.0f, 0.0f);
	healthGrenadeTrailParticle->velocity = psMath::float3(0.0f, 0.0f, 0.0f);
	healthGrenadeTrailParticle->life = 10.0f;
	healthGrenadeTrailParticle->dLife = 0.6f;
	healthGrenadeTrailParticle->color[0] = 0.6f;
	healthGrenadeTrailParticle->color[1] = 0.4f;
	healthGrenadeTrailParticle->color[2] = 0.0f;
	healthGrenadeTrailParticle->color[3] = 1.0f;
	healthGrenadeTrailParticle->dColor[0] = 0.0f;
	healthGrenadeTrailParticle->dColor[1] = 0.0f;
	healthGrenadeTrailParticle->dColor[2] = 0.0f;
	healthGrenadeTrailParticle->dColor[3] = -0.06f;
	healthGrenadeTrailParticle->scale = 0.3f;
	healthGrenadeTrailParticle->dScale = 0.01f;

	healthGrenade->_trailParticle = healthGrenadeTrailParticle;
	_templates.insert(std::pair<unsigned int, SGameObject*>(healthGrenade->templateNumber, healthGrenade));
	//Shield Grenade
	SProjectile* shieldGrenade = new SProjectile();
	shieldGrenade->templateNumber = SGAMEOBJECT_NEWINSTANCE_SHIELDGRENADE;
	shieldGrenade->radius = 10.0f;
	shieldGrenade->damage = 200;
	shieldGrenade->terminatesAfterTime = true;
	shieldGrenade->shieldMultiplier = 2.0f;
	shieldGrenade->healthMultiplier = 0.5f;

	_templates.insert(std::pair<unsigned int, SGameObject*>(shieldGrenade->templateNumber, shieldGrenade));
}

void SGameObject::unloadGameObjectTemplates(void)
{
	
}
SGameObject* SGameObject::new_instance(unsigned int templateNumber, bool isServerInstance, unsigned int ID)
{
	SGameObject* object = 0;

	//find template number in static map
	std::map<unsigned int, SGameObject*>::iterator it = _templates.find(templateNumber);

	if (it == _templates.end())
		return 0;


	SGameObject* original = it->second;

	switch (templateNumber)
	{
	case SGAMEOBJECT_NEWINSTANCE_FIREBOLT:
		object = new SProjectile((SProjectile*)original);
		break;
	case SGAMEOBJECT_NEWINSTANCE_HEALTHGRENADE:
		object = new SProjectile((SProjectile*)original);
		break;
	case SGAMEOBJECT_NEWINSTANCE_SHIELDGRENADE:
		object = new SProjectile((SProjectile*)original);
		break;
	default:
		break;
	}

	if (object != 0)
	{
		object->templateNumber = templateNumber;
		object->ID = ID;
	}


	return object;
}
SGameObject::SGameObject(void)
{
	ID = 0;
	templateNumber = 0;
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	_isServerInstance = false;
	_isDead = false;
}

SGameObject::SGameObject(SGameObject* original)
{
	ID = 0;
	templateNumber = original->templateNumber;
	
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	_isServerInstance = false;
	_isDead = false;
}
SGameObject::~SGameObject(void)
{
}

void SGameObject::clientUpdate(ps::GameData* pGameData)
{
	
}

void SGameObject::serverUpdate(void)
{
}

unsigned int SGameObject::getID(void)
{
	return ID;
}

unsigned int SGameObject::getTemplateNumber(void)
{
	return templateNumber;
}

void SGameObject::serverSerializeData(psNetworking::ServerOutGameObject* pData)
{
	pData->id = ID;
	pData->objectTemplate = templateNumber;
	pData->x = position.x;
	pData->y = position.y;
	pData->z = position.z;
}

void SGameObject::clientDeserializeData(psNetworking::ServerOutGameObject* pData)
{
	position.x = pData->x;
	position.y = pData->y;
	position.z = pData->z;
}

bool SGameObject::getIsDead(void)
{
	return _isDead;
}