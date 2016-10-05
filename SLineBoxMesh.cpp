#include "SRender.h"

SLineBoxMesh::SLineBoxMesh(void)
{
	_deferredObject = false;
}

SLineBoxMesh::~SLineBoxMesh(void)
{
}

void SLineBoxMesh::render(D3DXMATRIX* ref, float x, float y, float z,
					 float rx, float ry, float rz,
					 float sx, float sy, float sz,
					 float cx, float cy, float cz,
					 LPDIRECT3DDEVICE9 dev)
{

	dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3DXMATRIX translation;
	D3DXMATRIX scaling;
	D3DXMATRIX rotationX, rotationY, rotationZ;
	D3DXMatrixTranslation(&translation, x, y, z);
	D3DXMatrixScaling(&scaling, sx, sy, sz);
	D3DXMatrixRotationX(&rotationX, rx);
	D3DXMatrixRotationY(&rotationY, ry);
	D3DXMatrixRotationZ(&rotationZ, rz);
	dev->SetTransform(D3DTS_WORLD, &(rotationX*rotationY*rotationZ*translation));
	dev->SetFVF(SRENDER_POSDIF);
	dev->SetStreamSource(0, vertexBuffer, 0, sizeof(ColoredVertex));
	dev->SetTexture(0, NULL);
	dev->SetIndices(indexBuffer);
	dev->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12);
}
bool SLineBoxMesh::loadBox(DWORD color)
{
	ColoredVertex* lineVerts = new ColoredVertex[8];
	dev->CreateVertexBuffer(sizeof(ColoredVertex)*8, 0, SRENDER_POSDIF, D3DPOOL_MANAGED, &vertexBuffer, 0);
	vertexBuffer->Lock(0, 0, (void**)&lineVerts, 0);
	lineVerts[0].x = -0.5f; lineVerts[0].y = 0.5f; lineVerts[0].z = -0.5f; lineVerts[0].diffuse = color;
	lineVerts[1].x = 0.5f; lineVerts[1].y = 0.5f; lineVerts[1].z = -0.5f; lineVerts[1].diffuse = color;
	lineVerts[2].x = -0.5f; lineVerts[2].y = -0.5f; lineVerts[2].z = -0.5f; lineVerts[2].diffuse = color;
	lineVerts[3].x = 0.5f; lineVerts[3].y = -0.5f; lineVerts[3].z = -0.5f; lineVerts[3].diffuse = color;

	lineVerts[4].x = -0.5f; lineVerts[4].y = 0.5f; lineVerts[4].z = 0.5f; lineVerts[4].diffuse = color;
	lineVerts[5].x = 0.5f; lineVerts[5].y = 0.5f; lineVerts[5].z = 0.5f; lineVerts[5].diffuse = color;
	lineVerts[6].x = -0.5f; lineVerts[6].y = -0.5f; lineVerts[6].z = 0.5f; lineVerts[6].diffuse = color;
	lineVerts[7].x = 0.5f; lineVerts[7].y = -0.5f; lineVerts[7].z = 0.5f; lineVerts[7].diffuse = color;
	vertexBuffer->Unlock();

	short* lineIndices = new short[24];
	dev->CreateIndexBuffer(sizeof(short)*24, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0);
	indexBuffer->Lock(0, 0, (void**)&lineIndices, 0);
	lineIndices[0] = 0; lineIndices[1] = 1;//front-topline
	lineIndices[2] = 1; lineIndices[3] = 3;//front-rightline
	lineIndices[4] = 0; lineIndices[5] = 2;//front-leftline
	lineIndices[6] = 2; lineIndices[7] = 3;//front-bottomline

	lineIndices[8] = 4; lineIndices[9] = 5;//back-topline
	lineIndices[10] = 5; lineIndices[11] = 7;//back-rightline
	lineIndices[12] = 4; lineIndices[13] = 6;//back-leftline
	lineIndices[14] = 6; lineIndices[15] = 7;//back-bottomline

	lineIndices[16] = 2; lineIndices[17] = 6;//leftside-bottom
	lineIndices[18] = 0; lineIndices[19] = 4;//leftside-top
	lineIndices[20] = 1; lineIndices[21] = 5;//rightside-top
	lineIndices[22] = 3; lineIndices[23] = 7;//rightside-bottom
	indexBuffer->Unlock();
	return true;
}

void SLineBoxMesh::renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader)
{
}

void SLineBoxMesh::renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader)
{
}

void SLineBoxMesh::renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader)
{
}