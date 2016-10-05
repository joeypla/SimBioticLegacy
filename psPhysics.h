#ifndef PS_PHYSICS_H
#define PS_PHYSICS_H

#include <d3dx9.h>
#include "SSGame.h"
#include "GameData.h"
namespace psPhysics{
	void update(ps::GameData* gameData);

	struct DualBoxCollisionInfo
	{
		bool colliding;
		D3DXVECTOR3 smallestCorrection;
	};

	DualBoxCollisionInfo getDualBoxCollision(D3DXVECTOR3 box1Position, D3DXVECTOR3 box1Size, D3DXVECTOR3 box2Position, D3DXVECTOR3 box2Size);
	D3DXVECTOR3 getBoxVoxelFieldCorrection(D3DXVECTOR3 boxPos, D3DXVECTOR3 boxSize, ps::VoxelData* field);

	bool isPointWithinBox(D3DXVECTOR3 point, D3DXVECTOR3 boxPosition, D3DXVECTOR3 boxSize);
};
#endif