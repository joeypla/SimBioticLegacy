#include "SRender.h"

SMaterial::SMaterial(LPDIRECT3DTEXTURE9 color, LPDIRECT3DTEXTURE9 normal, LPDIRECT3DTEXTURE9 specular)
{

	_colorMap = color;
	_normalMap = normal;
	_specularMap = specular;
}

LPDIRECT3DTEXTURE9 SMaterial::colorMap(void)
{
	return _colorMap;
}

LPDIRECT3DTEXTURE9 SMaterial::normalMap(void)
{
	return _normalMap;
}

LPDIRECT3DTEXTURE9 SMaterial::specularMap(void)
{
	return _specularMap;
}

void SMaterial::performScreenSpaceReflection(bool b)
{
	_performScreenSpaceReflection = b;
}