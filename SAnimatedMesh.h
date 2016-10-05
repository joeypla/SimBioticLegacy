#ifndef SANIMATEDMESH_H
#define SANIMATEDMESH_H
#include "SRender.h"

#include "srAnimation.h"

#include "Cal3D/coremodel.h"

class SAnimatedMesh: public SMesh{
	friend class S3D;
public:
	SAnimatedMesh(void);
	
	bool createModel(std::string modelName, std::string skeletonPath, std::vector<std::string>& meshPaths, std::vector<std::string>& animationPaths);

	~SAnimatedMesh(void);
	void render(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPDIRECT3DDEVICE9 dev);

	void renderDeferred(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader);

	void renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader);
	//this method takes a path and loads+renders into atlas block0.png, block1.png etc.
	//all the way until blockn.png where n = textureCount-1.
	
	bool loadFromFileX(std::wstring filename);
	bool loadFromFBXFile(std::string filename);
private:
	
	CalCoreModel* coreModel;
	std::vector<int> animationIDs;
	std::vector<int> submeshIDs;
};
#endif