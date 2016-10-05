#include "SEditorGrid.h"

#include "SGUI.h"

SEditorGrid::SEditorGrid(void){
	gridLevel = 5;
	sameSideSelection = false;
	gridOrientation = SEDITOR_GRID_ORIENTATION_Y;
	buildGrid();
}

SEditorGrid::~SEditorGrid(void){
}

void SEditorGrid::buildGrid(void){
	vertexBuffer = NULL;
	S_GUI::getDevice()->CreateVertexBuffer((MAP_DIMENSION+1)*4*sizeof(SEDITOR_SIMPLEVERTEX),
		0,
		SEDITOR_SIMPLEFVF,
		D3DPOOL_MANAGED,
		&vertexBuffer,
		0);

	SEDITOR_SIMPLEVERTEX* verts = new SEDITOR_SIMPLEVERTEX[(MAP_DIMENSION+1)*4];
	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	for (int x = 0; x<(MAP_DIMENSION+1); x++){
		static int i = 0;
			//front
			verts[i].color = 0xffffffff;
			verts[i].x = (float)x; verts[i].y = 0.0f; verts[i].z = 0.0f;
			//back
			verts[i+1].color = 0xffffffff;
			verts[i+1].x = (float)x; verts[i+1].y = 0.0f; verts[i+1].z = 64.0f;
			i = i + 2;
	}

	for (int z = 0; z<(MAP_DIMENSION+1); z++){
		static int j = (MAP_DIMENSION+1)*2;
			//left
			verts[j].color = 0xffffffff;
			verts[j].x = 0.0f; verts[j].y = 0.0f; verts[j].z = (float)z;
			//right
			verts[j+1].color = 0xffffffff;
			verts[j+1].x = 64.0f; verts[j+1].y = 0.0f; verts[j+1].z = (float)z;
			j = j + 2;
		}
	vertexBuffer->Unlock();
}

void SEditorGrid::updateGrid(void){

	SEDITOR_SIMPLEVERTEX* verts;
	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_X){
		int i = 0;
		for (int x = 0; x<(MAP_DIMENSION+1); x++){
			//static int i = 0;
			//front
			verts[i].color = 0xffffffff;
			verts[i].x = 0.0f; verts[i].y = 0.0f; verts[i].z = (float)x;
			//back
			verts[i+1].color = 0xffffffff;
			verts[i+1].x = 0.0f; verts[i+1].y = 64.0f; verts[i+1].z = (float)x;
			i = i + 2;
		}
		int j = (MAP_DIMENSION+1)*2;
		for (int z = 0; z<(MAP_DIMENSION+1); z++){
			//static int j = (MAP_DIMENSION+1)*2;
			//left
			verts[j].color = 0xffffffff;
			verts[j].x = 0.0f; verts[j].y = (float)z; verts[j].z = 0.0f;
			//right
			verts[j+1].color = 0xffffffff;
			verts[j+1].x = 0.0f; verts[j+1].y = (float)z; verts[j+1].z = 64.0f;
			j = j + 2;
		}
	}


	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Y){
		int i = 0;
		for (int x = 0; x<(MAP_DIMENSION+1); x++){
			//static int i = 0;
			//front
			verts[i].color = 0xffffffff;
			verts[i].x = (float)x; verts[i].y = 0.0f; verts[i].z = 0.0f;
			//back
			verts[i+1].color = 0xffffffff;
			verts[i+1].x = (float)x; verts[i+1].y = 0.0f; verts[i+1].z = 64.0f;
			i = i + 2;
		}
		int j = (MAP_DIMENSION+1)*2;
		for (int z = 0; z<(MAP_DIMENSION+1); z++){
			//static int j = (MAP_DIMENSION+1)*2;
			//left
			verts[j].color = 0xffffffff;
			verts[j].x = 0.0f; verts[j].y = 0.0f; verts[j].z = (float)z;
			//right
			verts[j+1].color = 0xffffffff;
			verts[j+1].x = 64.0f; verts[j+1].y = 0.0f; verts[j+1].z = (float)z;
			j = j + 2;
		}
	}


	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Z){
		int i = 0;
		for (int x = 0; x<(MAP_DIMENSION+1); x++){
			//static int i = 0;
			//front
			verts[i].color = 0xffffffff;
			verts[i].x = (float)x; verts[i].y = 0.0f; verts[i].z = 0.0f;
			//back
			verts[i+1].color = 0xffffffff;
			verts[i+1].x = (float)x; verts[i+1].y = 64.0f; verts[i+1].z = 0.0f;
			i = i + 2;
		}
		int j = (MAP_DIMENSION+1)*2;
		for (int z = 0; z<(MAP_DIMENSION+1); z++){
			//static int j = (MAP_DIMENSION+1)*2;
			//left
			verts[j].color = 0xffffffff;
			verts[j].x = 0.0f; verts[j].y = (float)z; verts[j].z = 0.0f;
			//right
			verts[j+1].color = 0xffffffff;
			verts[j+1].x = 64.0f; verts[j+1].y = (float)z; verts[j+1].z = 0.0f;
			j = j + 2;
		}
	}
	vertexBuffer->Unlock();
}

void SEditorGrid::draw(void){
	S_GUI::getDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	D3DXMATRIX matTranslateGrid;
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_X) D3DXMatrixTranslation(&matTranslateGrid, (float)gridLevel, 0.0f, 0.0f);
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Y) D3DXMatrixTranslation(&matTranslateGrid, 0.0f, (float)gridLevel, 0.0f);
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Z) D3DXMatrixTranslation(&matTranslateGrid, 0.0f, 0.0f, (float)gridLevel);
	S_GUI::getDevice()->SetTransform(D3DTS_WORLD, &matTranslateGrid);
	S_GUI::getDevice()->SetFVF(SEDITOR_SIMPLEFVF);
	S_GUI::getDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(SEDITOR_SIMPLEVERTEX));
	S_GUI::getDevice()->SetIndices(NULL);
	S_GUI::getDevice()->SetTexture(0, NULL);
	S_GUI::getDevice()->DrawPrimitive(D3DPT_LINELIST, 0, (MAP_DIMENSION+1)*2);
	D3DXMatrixIdentity(&matTranslateGrid);
	S_GUI::getDevice()->SetTransform(D3DTS_WORLD, &matTranslateGrid);
	S_GUI::getDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void SEditorGrid::incrementGridLevel(void){
	if (gridLevel < MAP_DIMENSION-1){
		gridLevel++;
	}
}

void SEditorGrid::decrementGridLevel(void){
	if (gridLevel > 0){
		gridLevel--;
	}
}

void SEditorGrid::setOrientation(int orientation){
	gridOrientation = orientation;
}
int SEditorGrid::getOrientation(void){
	return gridOrientation;
}
bool SEditorGrid::getSameSideSelection(void){
	return sameSideSelection;
}
void SEditorGrid::toggleSameSideSelection(void){
	sameSideSelection = !sameSideSelection;
}
SEDITOR_BLOCKSELECTION SEditorGrid::getGridSelection(int x, int y){
	D3DXMATRIX matProj;
	S_GUI::getDevice()->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXVECTOR3 v;
	v.x = (((2.0f*x)/1680)-1.0f)/matProj._11;
	v.y = -(((2.0f*y)/1050)-1.0f)/matProj._22;
	v.z = 1.0f;

	D3DXMATRIX m, view;
	S_GUI::getDevice()->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixInverse(&m, NULL, &view);

	D3DXVECTOR3 rayDir, rayOrigin;
	rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

	D3DXVECTOR3 p1 = rayOrigin;

	SEDITOR_BLOCKSELECTION blockSelect;
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_X){
		if (rayOrigin.x > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.x < (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.x = gridLevel;
			} else {
				blockSelect.x = gridLevel - 1;
			}
		}
		if (rayOrigin.x < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.x > (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.x = gridLevel - 1;
			} else {
				blockSelect.x = gridLevel;
			}
		}
	}
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Y){
		if (rayOrigin.y > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.y < (float)gridLevel) break;
			}
			blockSelect.x = (int) p1.x;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.y = gridLevel;
			} else {
				blockSelect.y = gridLevel - 1;
			}
		}
		if (rayOrigin.y < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.y > (float)gridLevel) break;
			}
			blockSelect.x = (int) p1.x;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.y = gridLevel - 1;
			} else {
				blockSelect.y = gridLevel;
			}
		}
	}
	if (gridOrientation==SEDITOR_GRID_ORIENTATION_Z){
		if (rayOrigin.z > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.z < (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.x = (int) p1.x;
			if (sameSideSelection){
				blockSelect.z = gridLevel;
			} else {
				blockSelect.z = gridLevel - 1;
			}
		}
		if (rayOrigin.z < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.z > (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.x = (int) p1.x;
			if (sameSideSelection){
				blockSelect.z = gridLevel - 1;
			} else {
				blockSelect.z = gridLevel;
			}
		}
	}
	
	
	if (blockSelect.x < 0 || blockSelect.x > MAP_DIMENSION-1 || blockSelect.y < 0 || blockSelect.y > MAP_DIMENSION-1 || blockSelect.z < 0 || blockSelect.z > MAP_DIMENSION-1){
		blockSelect.x = -1; //in the main editor, if it receives x = -1, it doesnt place a block :)
	}
	return blockSelect;
}

float SEditorGrid::getGridLevel(void){
	return (float)gridLevel;
}