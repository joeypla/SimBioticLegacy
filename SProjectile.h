#ifndef SPROJECTILE_H
#define SPROJECTILE_H

#include "SGameObject.h"

struct SParticle;
class SLight;
class SProjectile : public SGameObject
{
public:
	SProjectile(void);
	SProjectile(SProjectile* original);
	~SProjectile(void);
	virtual void clientUpdate(ps::GameData* pGameData);
	virtual void serverUpdate(void);
	virtual void serverSerializeData(psNetworking::ServerOutGameObject* pData);
	virtual void clientDeserializeData(psNetworking::ServerOutGameObject* pData);

	psMath::float3 velocity;
	psMath::float3 acceleration;

	bool terminatesAfterTime;

	float radius;
	int damage;
	float healthMultiplier;
	float shieldMultiplier;

	SParticle* _singularParticle;
	SParticle* _trailParticle;

	psMath::float3 _lightColor;
	float _lightRadius;
	void setTrailParticle(SParticle* trailParticle);
private:
	SLight* emittedLight;
	
protected:
	
};
#endif