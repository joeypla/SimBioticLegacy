#ifndef SCAMERA_H
#define SCAMERA_H

#include "psMath.h"
class SCamera
{
public:
	SCamera(void);
	~SCamera(void);
	void setPosition(float x, float y, float z);
	void setUpVector(float x, float y, float z);
	void setDirection(float x, float y, float z, int upConfig);
	void lookAt(float x, float y, float z);

	//functions used for directX. Still usable wherever, but probably whatever.
	psMath::float3 getPositionX(void);
	psMath::float3 getUpVectorX(void);
	psMath::float3 getDirectionX(void);
	psMath::float3 getLookAtX(void);

	void rotateView(float strafe, float up, float direction, int upRotationType);
	void fixUpVector(int upRotationType);

	float x;
	float y;
	float z;

	float ux;
	float uy;
	float uz;

	float dx;
	float dy;
	float dz;
	psMath::float4x4 x_getMat(void);
	psMath::float4x4 x_getInverseViewMat(void);
	psMath::float3 x_getStrafeVector(void);
	float fov;
private:
	
};

#endif