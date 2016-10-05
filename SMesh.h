#ifndef SMESH_H
#define SMESH_H

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#include "VoxelData.h"
class SMaterialSet;

class SMesh
{
public:
	SMesh(void);
	~SMesh(void);
	void setDevice(LPDIRECT3DDEVICE9 dev);//not to be used outside SRENDER please
	virtual void render(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPDIRECT3DDEVICE9 dev);

	virtual void renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader);

	virtual void renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader);
	virtual void renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader);//used to render depth map for shadows...
	//Various automatic loading methods
	
	bool loadMeshFromVoxelData(ps::VoxelData* voxelData);
	bool loadMeshFromX(std::wstring path);
	bool loadMeshFromFBX(std::string path);
	bool createSkyboxMesh(void);

	SMaterialSet* getMaterialSet(void);
	void setMaterialSet(SMaterialSet* set);

	//very specific and not universal - used for maps only, and not scalable to anything else
	void calculateStaticLightVertexDiffuse64(ps::VoxelData* voxelData);

	bool getDeferred(void);
private:
	int _staticMeshType;
	LPD3DXMESH mesh;
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	
	int perTextureFaceCount[64];//applicable to voxel geometry meshes, used for rendering.
	//In vb memory, vertices are partitioned by material. We need to know how many primitives
	//per material to offset the index in the buffer during rendering.
	
protected:
	SMaterialSet* materialSet;
	LPDIRECT3DDEVICE9 dev;
	bool _deferredObject;
};
#endif