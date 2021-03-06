#ifndef SEDITUNI_H
#define SEDITUNI_H

#define MAP_DIMENSION 64
#define SEDITOR_VOXELFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define SEDITOR_SIMPLEFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct SEDITOR_BLOCKSELECTION{
	int x, y, z;
};
struct SEDITOR_SIMPLEVERTEX{
	float x, y, z;
	DWORD color;
};
struct SEDITOR_VOXELVERTEX{
	float x, y, z;
	D3DXVECTOR3 normal;
	DWORD color;
	float u1, v1;
};
#endif