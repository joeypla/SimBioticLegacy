#ifndef SEDITORGRID_H
#define SEDITORGRID_H

#include <d3d9.h>
#include <d3dx9.h>

#define SEDITOR_GRID_ORIENTATION_X 0
#define SEDITOR_GRID_ORIENTATION_Y 1
#define SEDITOR_GRID_ORIENTATION_Z 2

#include "SEditorUniversal.h"
class SEditorGrid{
public:
	SEditorGrid(void);
	~SEditorGrid(void);

	void draw(void);
	void updateGrid(void);

	void incrementGridLevel(void);
	void decrementGridLevel(void);
	float getGridLevel(void);

	void setOrientation(int orientation);
	int getOrientation();
	bool getSameSideSelection(void);
	void toggleSameSideSelection(void);
	SEDITOR_BLOCKSELECTION getGridSelection(int x, int y); //x and y are mouse screen coordinates
private:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

	int gridLevel;
	int gridOrientation;

	bool sameSideSelection;
	void buildGrid(void);
	
	

};
#endif