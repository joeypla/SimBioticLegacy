#include "SRender.h"

SCamera::SCamera(void)
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	dx = 1.0f;
	dy = 0.0f;
	dz = 0.0f;
	ux = 0.0f;
	uy = 1.0f;
	uz = 0.0f;
	fov = 0.8f;
}

SCamera::~SCamera(void)
{
}
void SCamera::setPosition(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
void SCamera::setUpVector(float x, float y, float z)
{
	ux = x;
	uy = y;
	uz = z;
}
void SCamera::setDirection(float x, float y, float z, int upConfig)
{

	dx = x;
	dy = y;
	dz = z;

	//now we need to recalculate up vector
	ux = 0.0f;
	uy = 0.0f;
	uz = 0.0f;
	switch (upConfig)
	{
	case 0:
		ux = 1.0f;
	case 1:
		ux = -1.0f;
	case 2:
		uy = 1.0f;
	case 3:
		uy = -1.0f;
	case 4:
		uz = 1.0f;
	case 5:
		uz = -1.0f;
		break;
	default:
		break;
	}
	//first calculate strafe
	D3DXVECTOR3 strafe;
	D3DXVec3Cross(&strafe, &D3DXVECTOR3(dx, dy, dz), &D3DXVECTOR3(ux, uy, uz));

	D3DXVECTOR3 newUp;
	D3DXVec3Cross(&newUp, &strafe, &D3DXVECTOR3(dx, dy, dz));
	ux = newUp.x; 
	uy = newUp.y;
	uz = newUp.z;
}

void SCamera::lookAt(float x, float y, float z)
{
	dx = x - this->x;
	dy = y - this->y;
	dz = z - this->z;

	//should normalize here
	D3DXVECTOR3 vec(dx, dy, dz);
	D3DXVec3Normalize(&vec, &vec);
	dx = vec.x;
	dy = vec.y;
	dz = vec.z;
}

D3DXVECTOR3 SCamera::getPositionX(void){return D3DXVECTOR3(x, y, z);}
D3DXVECTOR3 SCamera::getUpVectorX(void){return D3DXVECTOR3(ux, uy, uz);}
D3DXVECTOR3 SCamera::getDirectionX(void){return D3DXVECTOR3(dx, dy, dz);}
D3DXVECTOR3 SCamera::getLookAtX(void){return D3DXVECTOR3(x + dx, y + dy, z + dz);}

void SCamera::rotateView(float strafe, float up, float direction, int upRotationType)
{
	
	//find strafe vector
	D3DXVECTOR3 vecStrafe;
	D3DXVec3Cross(&vecStrafe, &getDirectionX(), &getUpVectorX());
	D3DXVec3Normalize(&vecStrafe, &vecStrafe);

	//create matrix for strafe rotation
	D3DXMATRIX matStrafeRotation;
	D3DXMatrixRotationAxis(&matStrafeRotation, &vecStrafe, strafe);

	//create matrix for up rotation - dependant on gravity
	D3DXMATRIX matUpRotation;
	if (upRotationType == SRENDER_UPROTATIONTYPE_XNEGATIVE) D3DXMatrixRotationX(&matUpRotation, up);
	if (upRotationType == SRENDER_UPROTATIONTYPE_XPOSITIVE) D3DXMatrixRotationX(&matUpRotation, -up);
	if (upRotationType == SRENDER_UPROTATIONTYPE_YNEGATIVE) D3DXMatrixRotationY(&matUpRotation, up);
	if (upRotationType == SRENDER_UPROTATIONTYPE_YPOSITIVE) D3DXMatrixRotationY(&matUpRotation, -up);
	if (upRotationType == SRENDER_UPROTATIONTYPE_ZNEGATIVE) D3DXMatrixRotationZ(&matUpRotation, up);
	if (upRotationType == SRENDER_UPROTATIONTYPE_ZPOSITIVE) D3DXMatrixRotationZ(&matUpRotation, -up);

	//create matrix for directional rotation
	D3DXMATRIX matDirRot;
	D3DXMatrixRotationAxis(&matDirRot, &D3DXVECTOR3(dx, dy, dz), direction);

	//D3DXMatrixRotationY(&matUpRotation, up);
	//now update direction
	D3DXVECTOR3 oldDirection = getDirectionX();
	D3DXVECTOR3 oldUp = getUpVectorX();

	D3DXVECTOR3 newDirection, newUp;
	D3DXVec3TransformCoord(&newDirection, &oldDirection, &(matStrafeRotation*matUpRotation));
	D3DXVec3TransformCoord(&newUp, &oldUp, &(matStrafeRotation*matUpRotation*matDirRot));

	D3DXVec3Normalize(&newDirection, &newDirection);
	D3DXVec3Normalize(&newUp, &newUp);
	dx = newDirection.x, dy = newDirection.y, dz = newDirection.z;
	setUpVector(newUp.x, newUp.y, newUp.z);
}

D3DXMATRIX SCamera::x_getMat(void)
{
	D3DXMATRIX mat;
	D3DXMatrixLookAtLH(&mat, &getPositionX(), &getLookAtX(), &getUpVectorX());
	return mat;
}
D3DXMATRIX SCamera::x_getInverseViewMat(void)
{
	D3DXMATRIX view, invView;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(x, y, z), &D3DXVECTOR3(x + dx, y + dy, z + dz), &D3DXVECTOR3(ux, uy, uz));
	D3DXMatrixInverse(&invView, NULL, &view);
	return invView;
}

D3DXVECTOR3 SCamera::x_getStrafeVector(void)
{
	D3DXVECTOR3 dir(dx, dy, dz);
	D3DXVECTOR3 up(ux, uy, uz);
	D3DXVECTOR3 strafe;

	D3DXVec3Cross(&strafe, &dir, &up);
	D3DXVec3Normalize(&strafe, &strafe);
	return strafe;

}

void SCamera::fixUpVector(int upRotationType)
{
	D3DXVECTOR3 unitUpVector;
	unitUpVector.x = 0.0f;
	unitUpVector.y = 0.0f;
	unitUpVector.z = 0.0f;

	switch (upRotationType)
	{
	case 0:
		x = 1.0f;
		break;
	case 1:
		x = -1.0f;
		break;
	case 2:
		y = 1.0f;
		break;
	case 3:
		y = -1.0f;
		break;
	case 4:
		z = 1.0f;
		break;
	case 5:
		z = -1.0f;
		break;
	default:
		break;
	}
	D3DXVECTOR3 dir;
	dir.x = dx;
	dir.y = dy;
	dir.z = dz;

	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 strafe;

	D3DXVec3Cross(&strafe, &dir, &unitUpVector);
	D3DXVec3Normalize(&strafe, &strafe);
	D3DXVECTOR3 newUp;
	D3DXVec3Cross(&newUp, &dir, &strafe);
	D3DXVec3Normalize(&newUp, &newUp);
	ux = newUp.x;
	uy = newUp.y;
	uz = newUp.z;
}