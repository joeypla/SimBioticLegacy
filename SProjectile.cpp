#include "SProjectile.h"

#include "SRender.h"

#include "SymbiotePacketStructs.h"
SProjectile::SProjectile(void)
{
	_singularParticle = 0;
	emittedLight = 0;//Zero for original templates obviously
	_lightColor = psMath::float3(0.0f, 0.0f, 0.0f);
	_lightRadius = 1.0f;

	_singularParticle = SRender::getSingleton()->getParticleSystem()->reserveGlowParticle();

	if (_singularParticle != 0)
	{
		_singularParticle->color[0] = 1.0f;
		_singularParticle->color[1] = 1.0f;
		_singularParticle->color[2] = 1.0f;
		_singularParticle->color[3] = 1.0f;
		_singularParticle->dColor[0] = 0.0f;
		_singularParticle->dColor[1] = 0.0f;
		_singularParticle->dColor[2] = 0.0f;
		_singularParticle->dColor[3] = 0.0f;
		_singularParticle->scale = 3.0f;
		_singularParticle->dScale = 0.0f;
		_singularParticle->velocity = psMath::float3(0.0f, 0.0f, 0.0f);
		_singularParticle->acceleration = psMath::float3(0.0f, 0.0f, 0.0f);
	}

	_trailParticle = 0;

	velocity.x = 0.0f;
	velocity.y = 0.0f;
	velocity.z = 0.0f;

	acceleration.x = 0.0f;
	acceleration.y = 0.0f;
	acceleration.z = 0.0f;

	radius = 0.0f;
	damage = 0;
	healthMultiplier = 1.0f;
	shieldMultiplier = 1.0f;
	terminatesAfterTime = false;
}

SProjectile::SProjectile(SProjectile* original)
{
	SGameObject::SGameObject(original);

	_singularParticle = original->_singularParticle;
	_trailParticle = original->_trailParticle;
	radius = original->radius;
	damage = original->damage;
	healthMultiplier = original->healthMultiplier;
	shieldMultiplier = original->shieldMultiplier;
	terminatesAfterTime = original->terminatesAfterTime;
	acceleration.x = 0.0f;
	acceleration.y = 0.0f;
	acceleration.z = 0.0f;


	_lightColor = original->_lightColor;
	_lightRadius = original->_lightRadius;

	emittedLight = SRender::getSingleton()->createLight();
	emittedLight->setColor(_lightColor.x, _lightColor.y, _lightColor.z);
	emittedLight->setRadius(_lightRadius);
}
SProjectile::~SProjectile(void)
{
	SGameObject::~SGameObject();

	SRender::getSingleton()->getParticleSystem()->releaseGlowParticle(_singularParticle);
	SRender::getSingleton()->releaseLight(emittedLight);
}

void SProjectile::serverUpdate(void)
{
}

#include "SLOG.h"
#include "GameData.h"
void SProjectile::clientUpdate(ps::GameData* pGameData)
{
	
	velocity += acceleration;
	position += velocity;

	emittedLight->setPosition(position.x, position.y, position.z);
	//_singularParticle->acceleration = acceleration;
	//_singularParticle->velocity = velocity;
	if (_singularParticle != 0) _singularParticle->position = position;

	_trailParticle->position = position;
	SRender::getSingleton()->getParticleSystem()->emitParticle(_trailParticle);
	_trailParticle->position += velocity / 2.0f;
	SRender::getSingleton()->getParticleSystem()->emitParticle(_trailParticle);


	for (int i = 0; i < 20; i++)
		{
			SParticle particle;
			particle.position.x = position.x;
			particle.position.y = position.y;
			particle.position.z = position.z;
			particle.velocity.x = ((rand() % 100) - 50) * 0.001f;
			particle.velocity.y = ((rand() % 100) - 50) * 0.001f;
			particle.velocity.z = ((rand() % 100) - 50) * 0.001f;
			particle.life = 10.0f;
			particle.dLife = 0.2f;
			particle.acceleration.x = 0.0f;
			particle.acceleration.y = 0.0f;
			particle.acceleration.z = 0.0f;
			particle.scale = 0.1f;
			particle.dScale = 0.00f;
			particle.color[0] = 1.0f;
			particle.color[1] = 0.6f;
			particle.color[2] = 0.0f;
			particle.color[3] = 1.0f;
			particle.dColor[0] = 0.0f;
			particle.dColor[1] = 0.005f;
			particle.dColor[2] = 0.01f;
			particle.dColor[3] = -0.02f;
			SRender::getSingleton()->getParticleSystem()->emitParticle(&particle);
		}


	if ((int)position.x >= 0 && (int)position.x < pGameData->pWorldData()->pVoxelData()->x())
		if ((int)position.y >= 0 && (int)position.y < pGameData->pWorldData()->pVoxelData()->y())
			if ((int)position.z >= 0 && (int)position.z < pGameData->pWorldData()->pVoxelData()->z())
	if (pGameData->pWorldData()->pVoxelData()->data((int)position.x, (int)position.y, (int)position.z))
	{
		_isDead = true;
		while (pGameData->pWorldData()->pVoxelData()->data((int)position.x, (int)position.y, (int)position.z))
		{
			position -= velocity*0.01f;
		}
		for (int i = 0; i < 100; i++)
		{
			SParticle particle;
			particle.position.x = position.x;
			particle.position.y = position.y;
			particle.position.z = position.z;
			particle.velocity.x = ((rand() % 100) - 50) * 0.001f;
			particle.velocity.y = ((rand() % 100) - 50) * 0.001f;
			particle.velocity.z = ((rand() % 100) - 50) * 0.001f;
			particle.life = 10.0f;
			particle.dLife = 0.2f;
			particle.acceleration.x = 0.0f;
			particle.acceleration.y = 0.0f;
			particle.acceleration.z = 0.0f;
			particle.scale = 2.0f;
			particle.dScale = 0.01f;
			particle.color[0] = 1.0f;
			particle.color[1] = 0.6f;
			particle.color[2] = 0.0f;
			particle.color[3] = 1.0f;
			particle.dColor[0] = 0.0f;
			particle.dColor[1] = 0.005f;
			particle.dColor[2] = 0.01f;
			particle.dColor[3] = -0.02f;
			SRender::getSingleton()->getParticleSystem()->emitParticle(&particle);
		}
		_isDead = true;
	}
}

void SProjectile::serverSerializeData(psNetworking::ServerOutGameObject* pData)
{
	//Super
	SGameObject::serverSerializeData(pData);

	pData->dx = velocity.x;
	pData->dy = velocity.y;
	pData->dz = velocity.z;

	pData->ax = acceleration.x;
	pData->ay = acceleration.y;
	pData->az = acceleration.z;

}

void SProjectile::clientDeserializeData(psNetworking::ServerOutGameObject* pData)
{
	//Super
	SGameObject::clientDeserializeData(pData);


	velocity.x = pData->dx;
	velocity.y = pData->dy;
	velocity.z = pData->dz;

	acceleration.x = pData->ax;
	acceleration.y = pData->ay;
	acceleration.z = pData->az;
}

void SProjectile::setTrailParticle(SParticle* trailParticle)
{
	_trailParticle = trailParticle;
}