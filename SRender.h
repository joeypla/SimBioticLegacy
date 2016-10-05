#ifndef SRENDER_H
#define SRENDER_H

//include d3d9 headers
#include <d3d9.h>
#include <d3dx9.h>
//include std headers
#include <string>
#include <vector>
//other headers
#include "VoxelData.h"
#include "SParticleSystem.h"
#include "VertexStructures.h"
#include "SLight.h"
#include "SCamera.h"
#include "SMesh.h"
#include "SMaterialSet.h"
#include "SMaterial.h"
#include "SMutableMesh.h"
#include "SText.h"
#include "S2D.h"
#include "S3D.h"
//Vertex Format Definitions
#define SRENDER_POS (D3DFVF_XYZ)
#define SRENDER_POSDIF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define SRENDER_POSTX1 (D3DFVF_XYZ | D3DFVF_TEX1)
#define SRENDER_POSDIFTX1 (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define SRENDER_POSNORTX1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define SRENDER_POSNORDIFTX1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define SRENDER_MAXLIGHTEFFECTS 400

#define SRENDER_STATICMESHTYPE_NONE     0
#define SRENDER_STATICMESHTYPE_STANDARD 1
#define SRENDER_STATICMESHTYPE_VOXEL    2

//forward decls
class SDeferredRenderer;



struct LightEffectData
{
	float x, y, z;
	float dx, dy, dz;
	float ax, ay, az;

	float color[3];
	float radius;
	float dColor[3];
	float lifetime;
};
//Structs for voxel sets. Used for creating meshes.
struct SRENDER_VOXELSET2{char voxels[2][2][2];};
struct SRENDER_VOXELSET4{char voxels[4][4][4];};
struct SRENDER_VOXELSET8{char voxels[8][8][8];};
struct SRENDER_VOXELSET16{char voxels[16][16][16];};
struct SRENDER_VOXELSET32{char voxels[32][32][32];};
struct SRENDER_VOXELSET64{char voxels[64][64][64];};

#define SRENDER_UPROTATIONTYPE_XNEGATIVE 0
#define SRENDER_UPROTATIONTYPE_XPOSITIVE 1
#define SRENDER_UPROTATIONTYPE_YNEGATIVE 2
#define SRENDER_UPROTATIONTYPE_YPOSITIVE 3
#define SRENDER_UPROTATIONTYPE_ZNEGATIVE 4
#define SRENDER_UPROTATIONTYPE_ZPOSITIVE 5
class SImage
{
	friend class S2D;
public:
	SImage(void);
	SImage(std::wstring path);
	SImage(std::wstring path, int width, int height);
	~SImage(void);
private:
	LPDIRECT3DTEXTURE9 texture;
};











class SGridMesh : public SMesh{
public:
	SGridMesh(void);
	~SGridMesh(void);
	void render(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPDIRECT3DDEVICE9 dev);

	void renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader);

	void renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader);
	void renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader);//used to render depth map for shadows...
	bool loadGrid(int x, int y, int orientation, DWORD color);
private:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	int xCount, yCount;
};
class SLineBoxMesh : public SMesh{
public:
	SLineBoxMesh(void);
	~SLineBoxMesh(void);
	void render(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPDIRECT3DDEVICE9 dev);

	void renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader);

	void renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader);
	void renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader);//used to render depth map for shadows...
	bool loadBox(DWORD color);
private:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
};







/* light effects work similar to particles in the sense that
they can be easily spawned and forgotten about. This works nicely in a deferred renderer :) */
struct LightEffect{
	float x, y, z;
	float dx, dy, dz;
	float ax, ay, az;
	float lifetime;
	float color[3];
	float dColor[3];
	float radius;
	float dRadius;
};

//Render Definitions
#define SRENDER_OBJECTCONTAINERSIZE 512
#define SRENDER_MESHCONTAINERSIZE 256
#define SRENDER_MATERIALSETCONTAINERSIZE 256
#define SRENDER_MAXLIGHTS 8
class SRender
{
	friend class SMaterialSet;
	friend class SMesh;
	friend class SMutableMesh;
	friend class SAnimatedMesh;
	friend class SDeferredRenderer;
	friend class SLight;
	friend class S3D;
	friend class S2D;
	friend class SImage;
	friend class SParticleSystem;
public:
	//vertex declarations
	LPDIRECT3DVERTEXDECLARATION9 VD_POS_NOR_TX1_TAN_BIN(void);
	LPDIRECT3DVERTEXDECLARATION9 VD_POS_TX1(void);
	LPDIRECT3DVERTEXDECLARATION9 VD_POS_DIF_TX1(void);
	static SRender* getSingleton(void);
	static bool createSingleton();
	SRender(void);
	~SRender(void);
	void setup(LPDIRECT3DDEVICE9 device);
	void render(void);
	void destroy(void);

	SMaterialSet* createMaterialSet(std::wstring path);
	void releaseMaterialSet(SMaterialSet* set);

	SMesh* createMesh(void);
	SMutableMesh* createMutableMesh(void);
	SGridMesh* createGridMesh(void);
	SLineBoxMesh* createLineBoxMesh(void);
	SAnimatedMesh* createAnimatedMesh(void);

	void releaseMesh(SMesh* mesh);

	S3D* createS3D(void);
	void releaseS3D(S3D* s3d);

	S2D* createS2D(void);
	void releaseS2D(S2D* s2d);

	SText* createText(void);
	void releaseText(SText* text);

	SLight* createLight(void);
	SLight* createLight(SLight& referenceLight);

	
	SLight** getStaticLights(void);
	void releaseLight(SLight* light);

	void setCamera(SCamera* camera);
	SCamera* getCurrentCamera(void);

	void setWireframe(bool b);
	void setClearColor(DWORD color);

	void setProjection(float fov, float nearZ, float farZ);
	D3DXMATRIX getProjection(void);

	D3DXMATRIX getView(void);

	float getWidth(void);
	float getHeight(void);

	void setAmbientColor(char r, char g, char b);
	void setAmbientColor(float r, float g, float b);

	void setFogColor(float r, float g, float b);
	void setFogStart(float f);
	void setFogEnd(float f);

	void setLight(int index, const SLight& light);

	void createLightEffect(
		float x, float y, float z, 
		float dx, float dy, float dz,
		float ax, float ay, float az,
		float cr, float cg, float cb);

	void createLightEffect(LightEffectData* data);

	void emitParticle(SParticle* data);
	int d_GetS2DUsed(void);
	int d_GetS3DUsed(void);
	int d_GetSTextUsed(void);

	SParticleSystem* getParticleSystem(void);
private:
	LPDIRECT3DVERTEXDECLARATION9 _VD_POS_NOR_TX1_TAN_BIN;
	LPDIRECT3DVERTEXDECLARATION9 _VD_POS_TX1;
	LPDIRECT3DVERTEXDECLARATION9 _VD_POS_DIF_TX1;
	LPDIRECT3DDEVICE9 dev;
	static SRender* singleton;
	//Members that SRender needs in its process.
	void setupConfigs(void);

	bool bBloomEnabled;

	int iWidth;
	int iHeight;

	float fZNear;
	float fZFar;

	float fFOV;

	DWORD clearColor;
	bool wireframe;

	float fogColor[3];
	float fFogStart;
	float fFogEnd;

	float ambientColor[3];

	float focalDistance;
	float focalWidth;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	//old stuff is used for motion blurring in deferred rendering
	D3DXMATRIX oldMatView;
	D3DXMATRIX oldMatProj;
	//render targets in order of rendering - if an effect is disabled, it is simply
	//removed from the render chain. This is the order if everything was enabled.
	void setupRenderTargets(void);
	LPDIRECT3DTEXTURE9 targetBase;
	LPDIRECT3DTEXTURE9 targetBloom;
	LPDIRECT3DTEXTURE9 targetFinal;

	LPDIRECT3DSURFACE9 surfaceBase;
	LPDIRECT3DSURFACE9 surfaceBloom;
	LPDIRECT3DSURFACE9 surfaceFinal;

	//members for quad - used in post-processing, menus etc.
	void setupQuad(void);
	LPDIRECT3DVERTEXBUFFER9 quadVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 quadIndexBuffer;

	//Default Material - Going to expand on XMaterial system
	//but not important for now
	void setupDefaultMaterial(void);
	D3DMATERIAL9 defaultMaterial;

	//Objects to be rendered - By rendering order.
	void setupObjectContainers(void);
	S3D** obj3Ds;
	S2D** obj2Ds;
	SText** objTexts;
	SLight** objLights;
	LightEffect** lightEffects;

	//Meshes - used in rendering objects.
	void setupMeshContainers(void);
	SMesh** meshes;
	//Material Sets - used in rendering objects.
	void setupMaterialSetContainers(void);
	SMaterialSet** materialSets;

	//Shaders - used for pretty much everything because I just
	//got the hang of them and now realise how awesome they are.
	void setupShaders(void);
	LPD3DXEFFECT shaderBasic;

	//Camera - pretty important and self explanatory
	SCamera* camera;

	//Deferred Rendering Object
	SDeferredRenderer* deferredRenderingObject;

	//Particle System Object
	SParticleSystem* particleSystemObject;
};


#endif