#ifndef SMATERIAL_H
#define SMATERIAL_H

#include <d3d9.h>
class SMaterial
{
public:
	SMaterial(LPDIRECT3DTEXTURE9 color, LPDIRECT3DTEXTURE9 normal, LPDIRECT3DTEXTURE9 specular);
	~SMaterial(void);

	LPDIRECT3DTEXTURE9 colorMap();
	LPDIRECT3DTEXTURE9 normalMap();
	LPDIRECT3DTEXTURE9 specularMap();
	void performScreenSpaceReflection(bool b);
private:
	//colorMap holds RGBA - RGB are colors and A represents the emittance of the pixel in question.
	//A is used for the bloom effect - Areas with high A will appear to glow. Later on, if Alpha transparency
	//becomes a necessity, something else will need to be figured out.
	LPDIRECT3DTEXTURE9 _colorMap;
	LPDIRECT3DTEXTURE9 _normalMap;
	LPDIRECT3DTEXTURE9 _specularMap;

	bool _performScreenSpaceReflection;
};


#endif