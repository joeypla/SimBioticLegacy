#ifndef SLIGHT_H
#define SLIGHT_H

#include <d3d9.h>
#include "psMath.h"
#define SRENDER_LIGHT_TYPE_POINT 0
#define SRENDER_LIGHT_TYPE_SPOTLIGHT 1
class SLight
{
public:
	SLight(void);
	SLight(SLight& referenceLight);
	~SLight(void);
	//SLight operator=(const SLight* light);

	void setPosition(float x, float y, float z);
	void setColor(float r, float g, float b);
	void setAmbientFactor(float f);
	void setAttenuation(float att0, float att1, float att2);
	void setRadius(float r);
	void quickSet(float x, float y, float z, float r, float g, float b, float af, float att0, float att1, float att2, float radius);
//things are public for now, doesn't matter if floats are
	//changed and no matter the values, no errors will be
	//caused.
	void renderShadowMap(void);
	LPDIRECT3DTEXTURE9 shadowMap;
	LPDIRECT3DSURFACE9 shadowMapSurface;

	LPDIRECT3DTEXTURE9 filteredShadowMap;
	LPDIRECT3DSURFACE9 filteredShadowMapSurface;

	int type;

	float x;
	float y;
	float z;

	float dx;
	float dy;
	float dz;

	float theta;
	float phi;

	float r;
	float g;
	float b;

	float ambientFactor;

	float att0;
	float att1;
	float att2;

	float radius;
	float coneAngle;

	float getImportance(psMath::float3 eyePosition);
};

#endif