#ifndef SMUTABLEMESH_H
#define SMUTABLEMESH_H

#include "SMesh.h"
class SMutableMesh : public SMesh{
public:
	SMutableMesh(void);
	~SMutableMesh(void);
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
	//this method takes a path and loads+renders into atlas block0.png, block1.png etc.
	//all the way until blockn.png where n = textureCount-1.
	int createAtlasFromPath(const wchar_t* path);
	bool OLD_createAtlasFromPath(const wchar_t* path, int textureCount);
	void updateGeometry(ps::VoxelData* voxelData);
	LPDIRECT3DTEXTURE9 x_GetAtlas(void);
private:
	int faceCount;
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	LPDIRECT3DTEXTURE9 atlas;
	LPDIRECT3DTEXTURE9 atlasN;
	LPDIRECT3DTEXTURE9 atlasS;
};
#endif