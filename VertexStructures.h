#ifndef VERTEXSTRUCTURES_H
#define VERTEXSTRUCTURES_H

#include <d3dx9.h>
struct SimpleVertex
{
	float x;
	float y;
	float z;
};

struct ColoredVertex
{
	float x;
	float y;
	float z;

	DWORD diffuse;
};

struct TexturedVertex
{
	float x;
	float y;
	float z;

	float u;
	float v;
};
struct ColoredTexturedVertex{
	float x;
	float y;
	float z;

	DWORD diffuse;

	float u1;
	float v1;
};
struct DetailedTexturedVertex
{
	float x;
	float y;
	float z;

	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 binormal;

	float u1;
	float v1;
};
struct TexturedDirectionalVertex
{
	float x;
	float y;
	float z;

	D3DXVECTOR3 normal;

	DWORD diffuse;

	float u1;
	float v1;
};
#endif