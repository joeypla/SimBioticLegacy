#include "SRender.h"

SLight::SLight(void)
{
	type = SRENDER_LIGHT_TYPE_POINT;
	SRender* r = SRender::getSingleton();
	D3DXCreateTexture(r->dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &shadowMap);
	shadowMap->GetSurfaceLevel(0, &shadowMapSurface);

	D3DXCreateTexture(r->dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &filteredShadowMap);
	filteredShadowMap->GetSurfaceLevel(0, &filteredShadowMapSurface);

	ambientFactor = 0.0f;

}

SLight::SLight(SLight& referenceLight)
{
	type = referenceLight.type;
	ambientFactor = referenceLight.ambientFactor;
	x = referenceLight.x;
	y = referenceLight.y;
	z = referenceLight.z;

	dx = referenceLight.dx;
	dy = referenceLight.dy;
	dz = referenceLight.dz;

	r = referenceLight.r;
	g = referenceLight.g;
	b = referenceLight.b;

	radius = referenceLight.radius;
}
SLight::~SLight(void)
{
	shadowMap->Release();
}

void SLight::setPosition(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void SLight::setColor(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void SLight::setAmbientFactor(float f)
{
	ambientFactor = f;
}

void SLight::setAttenuation(float att0, float att1, float att2)
{
	this->att0 = att0;
	this->att1 = att1;
	this->att2 = att2;
}

void SLight::setRadius(float r)
{
	radius = r;
}

void SLight::quickSet(
	float x,
	float y,
	float z,
	float r,
	float g,
	float b,
	float ambientFactor,
	float att0,
	float att1,
	float att2,
	float radius)
{
	this->x = x;
	this->y = y;
	this->z = z;

	this->r = r;
	this->g = g;
	this->b = b;

	this->ambientFactor = ambientFactor;
	
	this->att0 = att0;
	this->att1 = att1;
	this->att2 = att2;

	this->radius = radius;
}

float SLight::getImportance(psMath::float3 eyePosition)
{
	float x2 = eyePosition.x;
	float y2 = eyePosition.y;
	float z2 = eyePosition.z;

	float distance = sqrt(((x2 - x)*(x2 - x)) + ((y2 - y)*(y2 - y)) + ((z2 - z)*(z2 - z)));
	float importance = radius / distance;
	return importance;
}

void SLight::renderShadowMap(void)
{
}