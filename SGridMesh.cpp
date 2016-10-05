#include "SRender.h"

SGridMesh::SGridMesh(void)
{
	_deferredObject = false;
}

SGridMesh::~SGridMesh(void)
{
}

void SGridMesh::render(D3DXMATRIX* ref, float x, float y, float z,
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
	dev->SetIndices(0);
	int vertCount = 260;
	dev->DrawPrimitive(D3DPT_LINELIST, 0, vertCount / 2);
}
bool SGridMesh::loadGrid(int xspan, int yspan, int orientation, DWORD color)
{
	xCount = xspan + 1;
	yCount = yspan + 1;
	vertexBuffer = NULL;
	int vertCount = (xspan+1)*2 + (yspan+1)*2;
	dev->CreateVertexBuffer(vertCount*sizeof(ColoredVertex),
		0,
		SRENDER_POSDIF,
		D3DPOOL_MANAGED,
		&vertexBuffer,
		0);

	ColoredVertex* verts;
	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	switch (orientation)
	{
	case 0:
		for (int y = 0; y<(xspan+1); y++)
		{
			static int i = 0;
			//front
			verts[i].diffuse = color;
			verts[i].x = 0.0f; verts[i].y = (float)y; verts[i].z = 0.0f;
			//back
			verts[i+1].diffuse = color;
			verts[i+1].x = 0.0f; verts[i+1].y = (float)y; verts[i+1].z = 64.0f;
			if (y == 32)
			{
				verts[i].diffuse = 0xffffffff;
				verts[i+1].diffuse = 0xffffffff;
			}
			i = i + 2;
		}

		for (int z = 0; z<65; z++)
		{
			static int j = 65*2;
			//left
			verts[j].diffuse = color;
			verts[j].x = 0.0f; verts[j].y = 0.0f; verts[j].z = (float)z;
			//right
			verts[j+1].diffuse = color;
			verts[j+1].x = 0.0f; verts[j+1].y = 64.0f; verts[j+1].z = (float)z;
			if (z == 32)
			{
				verts[j].diffuse = 0xffffffff;
				verts[j+1].diffuse = 0xffffffff;
			}
			j = j + 2;
		}
		break;
	case 1:
		for (int x = 0; x<(xspan+1); x++)
		{
			static int i = 0;
			//front
			verts[i].diffuse = color;
			verts[i].x = (float)x; verts[i].y = 0.0f; verts[i].z = 0.0f;
			//back
			verts[i+1].diffuse = color;
			verts[i+1].x = (float)x; verts[i+1].y = 0.0f; verts[i+1].z = 64.0f;
			if (x == 32)
			{
				verts[i].diffuse = 0xffffffff;
				verts[i+1].diffuse = 0xffffffff;
			}
			i = i + 2;
		}

		for (int z = 0; z<65; z++)
		{
			static int j = 65*2;
			//left
			verts[j].diffuse = color;
			verts[j].x = 0.0f; verts[j].y = 0.0f; verts[j].z = (float)z;
			//right
			verts[j+1].diffuse = color;
			verts[j+1].x = 64.0f; verts[j+1].y = 0.0f; verts[j+1].z = (float)z;
			if (z == 32)
			{
				verts[j].diffuse = 0xffffffff;
				verts[j+1].diffuse = 0xffffffff;
			}
			j = j + 2;
		}
		break;
	case 2:
		for (int x = 0; x<(xspan+1); x++)
		{
			static int i = 0;
			//front
			verts[i].diffuse = color;
			verts[i].x = (float)x; verts[i].y = 0.0f; verts[i].z = 0.0f;
			//back
			verts[i+1].diffuse = color;
			verts[i+1].x = (float)x; verts[i+1].y = 64.0f; verts[i+1].z = 0.0f;
			//SET CENTER LINES WHITE
			if (x == 32)
			{
				verts[i].diffuse = 0xffffffff;
				verts[i+1].diffuse = 0xffffffff;
			}
			i = i + 2;
		}

		for (int y = 0; y<65; y++)
		{
			static int j = 65*2;
			//left
			verts[j].diffuse = color;
			verts[j].x = 0.0f; verts[j].y = (float)y; verts[j].z = 0.0f;
			//right
			verts[j+1].diffuse = color;
			verts[j+1].x = 64.0f; verts[j+1].y = (float)y; verts[j+1].z = 0.0f;
			if (y == 32)
			{
				verts[j].diffuse = 0xffffffff;
				verts[j+1].diffuse = 0xffffffff;
			}
			j = j + 2;
		}
		break;
	default:
		break;
	}
	vertexBuffer->Unlock();
	return true;
}

void SGridMesh::renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader)
{
}

void SGridMesh::renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader)
{
}

void SGridMesh::renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader)
{
}