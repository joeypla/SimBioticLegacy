#include "SAnimatedMesh.h"

#include "srAnimation.h"
SAnimatedMesh::SAnimatedMesh(void)
{
	_deferredObject = true;
	coreModel = 0;
	
}

SAnimatedMesh::~SAnimatedMesh(void)
{
	if (coreModel != 0)
	{
		delete coreModel;
	}
}

bool SAnimatedMesh::createModel(std::string modelName, std::string skeletonPath, std::vector<std::string>& meshPaths, std::vector<std::string>& animationPaths)
{
	coreModel = new CalCoreModel(modelName);

	if (!coreModel->loadCoreSkeleton(skeletonPath)) return false;

	for (int i = 0; i < animationPaths.size(); i++)
	{
		int id = coreModel->loadCoreAnimation(animationPaths.at(i));
		if (id == -1) return false;
		animationIDs.push_back(id);
	}

	for (int i = 0; i < meshPaths.size(); i++)
	{
		int id = coreModel->loadCoreMesh(meshPaths.at(i));
		if (id == -1) return false;
		submeshIDs.push_back(id);
	}


	std::stringstream ss;
	//ss<<"Model created... Has "<<coreModel->getCoreSkeleton()->
	return true;
}


#include <sstream>
#include "SLOG.h"
bool SAnimatedMesh::loadFromFileX(std::wstring filename)
{
	
	return true; 
}


void SAnimatedMesh::render(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPDIRECT3DDEVICE9 dev)
{
	
}

void SAnimatedMesh::renderDeferred(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader)
{

	

}

 void SAnimatedMesh::renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader)
 {
	 SRender* r = SRender::getSingleton();
	
	//setup matrices that need setting up
	D3DXMATRIX worldView;
	worldView = (*world) * r->matView;
	D3DXMATRIX worldViewInv;
	D3DXMatrixInverse(&worldViewInv, NULL, &worldView);
	static D3DXMATRIX worldViewIT;
	D3DXMatrixTranspose(&worldViewIT, &worldViewInv);

	D3DXMATRIX oldWorldViewProjection = (*oldWorld) * r->oldMatView * r->oldMatProj;

	shader->SetMatrix("world", world);
	shader->SetMatrix("view", &r->matView);
	shader->SetMatrix("projection", &r->matProj);
	shader->SetMatrix("worldViewIT", &worldViewIT);
	shader->SetMatrix("oldWorldViewProjection", &oldWorldViewProjection);

	r->dev->SetTexture(0, materialSet->materialAtIndex(0)->colorMap());
	r->dev->SetTexture(1, materialSet->materialAtIndex(0)->normalMap());
	r->dev->SetTexture(2, materialSet->materialAtIndex(0)->specularMap());

	
	
 }