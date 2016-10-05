#ifndef S3D_H
#define S3D_H

#include "psMath.h"

#include "SMesh.h"

class CalModel;
class SAnimatedMesh;

class S3D{
public:
	S3D(void);
	~S3D(void);
	void setReference(D3DXMATRIX ref);
	void setPosition(float x, float y, float z);
	psMath::float3 getPosition(void);

	void setXRotation(float rot);
	void setYRotation(float rot);
	void setZRotation(float rot);
	void setXScale(float scale);
	void setYScale(float scale);
	void setZScale(float scale);
	void setScale(float scale);
	void setCenter(float cx, float cy, float cz);
	void setVisible(bool b);
	void setWallhacked(bool wallhacked, psMath::float3 color);

	void setMesh(SMesh* mesh);
	void setAnimatedMesh(SAnimatedMesh* animatedMesh);

	SMesh* getMesh(void);

	void render(LPDIRECT3DDEVICE9 dev, LPD3DXEFFECT shader);
	void renderShadowMap(LPD3DXEFFECT shader);
	bool castsShadow;
private:
	
	SMesh* mesh;
	CalModel* model;

	psMath::float4x4 oldWorldTransform;//used for velocity map in gbuffer
	psMath::float4x4 reference;
	float x;
	float y;
	float z;
	psMath::float4x4 matRotation;
	float rx;
	float ry;
	float rz;

	float sx;
	float sy;
	float sz;

	float cx;
	float cy;
	float cz;

	bool visible;//should be drawn or not
	bool wallhacked;
	psMath::float3 wallhackedColor;
};

#endif