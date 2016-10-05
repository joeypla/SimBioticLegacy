#ifndef SYMBIOTE_PHYSICS_H
#define SYMBIOTE_PHYSICS_H

//#include "MapViewer.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>
namespace SPhysics{
	D3DXVECTOR3 getResponseProjectionVector(D3DXVECTOR3* characterPosition, float character_XL, float character_YL, float character_ZL, D3DXVECTOR3* cubePositionArray, int arrayLength);
}

#endif