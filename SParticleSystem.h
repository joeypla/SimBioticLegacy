#ifndef SPARTICLESYSTEM_H
#define SPARTICLESYSTEM_H

#include <d3dx9.h>
#include <d3d9.h>
#include "psMath.h"
#include <vector>
#define SYMBIOTE_MAX_PARTICLES 10000

#define SPARTICLES_SMOKE1 0
struct ParticleVertex{
	float x, y, z;
	DWORD color;
	float u1, v1;
};
struct SParticle{
	LPDIRECT3DTEXTURE9 texture;
	psMath::float3 position;
	psMath::float3 velocity;
	psMath::float3 acceleration;
	float color[4];
	float dColor[4];
	float scale;
	float dScale;
	float life;
	float dLife;
	bool active;
};
class SParticleSystem{
public:
	
	SParticleSystem(LPDIRECT3DDEVICE9 dev);
	~SParticleSystem(void);

	SParticle* reserveGlowParticle(void);
	void releaseGlowParticle(SParticle* particle);


	void emitParticle(SParticle* data);
	void emitGlowParticle(SParticle* data);

	void update(void);
	void updateGlowParticles(void);

	void render(void);
	void renderGlowParticles(void);

	void setup(void);
private:
	
	std::vector <LPDIRECT3DTEXTURE9> particleTextures;
	LPD3DXEFFECT     glowParticleShader;
	LPD3DXEFFECT        smokeParticleShader;


	LPDIRECT3DTEXTURE9 basicParticleTexture;
	LPDIRECT3DTEXTURE9 basicGlowParticleTexture;

	LPDIRECT3DVERTEXBUFFER9 quadVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 quadIndexBuffer;
	LPDIRECT3DDEVICE9 _dev;
	SParticle particles[SYMBIOTE_MAX_PARTICLES];
	SParticle glowParticles[SYMBIOTE_MAX_PARTICLES];
};
#endif