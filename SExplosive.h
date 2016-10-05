#ifndef SEXPLOSIVE_H
#define SEXPLOSIVE_H

#include "SProjectile.h"

class SExplosive : public SProjectile
{
public:
	SExplosive(void);
	~SExplosive(void);
	void clientUpdate(ps::GameData* pGameData);
	void serverUpdate(void);

	float radius;
	int damage;
	float healthMultiplier;
	float shieldMultiplier;
private:
protected:
};
#endif