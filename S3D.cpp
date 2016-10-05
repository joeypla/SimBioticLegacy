#include "SRender.h"

#include "Cal3D/model.h"
S3D::S3D(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rx = 0.0f;
	ry = 0.0f;
	rz = 0.0f;
	sx = 1.0f;
	sy = 1.0f;
	sz = 1.0f;
	cx = 0.0f;
	cy = 0.0f;
	cz = 0.0f;
	castsShadow = false;
	visible = true;
	mesh = 0;
	model = 0;

	D3DXMatrixIdentity(&reference);
	wallhacked = false;
}

S3D::~S3D(void)
{
}
void S3D::setPosition(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

psMath::float3 S3D::getPosition(void)
{
	psMath::float3 position;
	D3DXVec3TransformCoord(&position, &psMath::float3(x, y, z), &reference);
	return position;
}
void S3D::setXRotation(float rot){rx = rot;}
void S3D::setYRotation(float rot){ry = rot;}
void S3D::setZRotation(float rot){rz = rot;}
void S3D::setXScale(float scale){sx = scale;}
void S3D::setYScale(float scale){sy = scale;}
void S3D::setZScale(float scale){sz = scale;}

void S3D::setScale(float scale)
{
	sx = scale;
	sy = scale;
	sz = scale;
}

void S3D::setVisible(bool b)
{
	visible = b;
}

void S3D::setCenter(float cx, float cy, float cz)
{
	this->cx = cx;
	this->cy = cy;
	this->cz = cz;
}

void S3D::render(LPDIRECT3DDEVICE9 dev, LPD3DXEFFECT shader)
{
	if (!visible) return;
	
	if (mesh==0) return;
	if (mesh->getDeferred() && shader == 0) return;

	if (!mesh->getDeferred() || shader == 0)
	{
		mesh->render( &reference, x, y, z,
				  rx, ry, rz,
				  sx, sy, sz,
				  cx, cy, cz,
				  dev);
	}

	else

	{
		D3DXMATRIX matRotX, matRotY, matRotZ, rotation, scale, translate, world;
		D3DXMatrixRotationX(&matRotX, rx);
		D3DXMatrixRotationY(&matRotY, ry);
		D3DXMatrixRotationZ(&matRotZ, rz);
		rotation = matRotX * matRotY * matRotZ;
		D3DXMatrixScaling(&scale, sx, sy, sz);
		D3DXMatrixTranslation(&translate, x, y, z);
		world = scale * rotation * translate * reference;
		shader->SetBool("wallhacked", wallhacked);
		shader->SetValue("wallhackedColor", &wallhackedColor, sizeof(wallhackedColor));

		dev->SetRenderState(D3DRS_ZENABLE, !wallhacked);

		if (model == 0)
		{
			mesh->renderDeferred(&world, &oldWorldTransform, shader);
		}
		else
		{
			//just render everything here from cal3d
		}
		//update old world transform
		oldWorldTransform = world;
	}
}

void S3D::setMesh(SMesh* mesh)
{
	this->mesh = mesh;

	
}
#include "SAnimatedMesh.h"
void S3D::setAnimatedMesh(SAnimatedMesh* animatedMesh)
{
	this->mesh = (SMesh*)animatedMesh;

	model = new CalModel(animatedMesh->coreModel);
	//model->attachMesh(animatedMesh->coreModel->
}
SMesh* S3D::getMesh(void)
{
	return mesh;
}

void S3D::setReference(D3DXMATRIX ref)
{
	reference = ref;
}

void S3D::renderShadowMap(LPD3DXEFFECT shader)
{
	if (!visible) return;
	if (mesh==0) return;
	if (shader==0) return;
	D3DXMATRIX matRotX, matRotY, matRotZ, rotation, scale, translate, world;
	D3DXMatrixRotationX(&matRotX, rx);
	D3DXMatrixRotationY(&matRotY, ry); 
	D3DXMatrixRotationZ(&matRotZ, rz);
	rotation = matRotX * matRotY * matRotZ;
	D3DXMatrixScaling(&scale, sx, sy, sz);
	D3DXMatrixTranslation(&translate, x, y, z);
	world = scale * rotation * translate * reference;
	mesh->renderDeferredDepth(&world, shader);
}

void S3D::setWallhacked(bool wallhacked, psMath::float3 wallhackedColor)
{
	this->wallhacked = wallhacked;
	this->wallhackedColor = wallhackedColor;
}