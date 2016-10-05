#include "SRender.h"

#include <sstream>
SMaterialSet::SMaterialSet(std::wstring path)
{
	/* 0 or null all fields */
	_iMaterials = 0;
	_materials = 0;

	std::wstringstream pathstreamColor;
	std::wstringstream pathstreamNormal;
	std::wstringstream pathstreamSpecular;
	pathstreamColor.str(L"");
	pathstreamNormal.str(L"");
	pathstreamSpecular.str(L"");

	/* size of materialset is dependent on the amount of COLORMAPS because they are the fundamental */
	int successfulImports = 0;
	LPDIRECT3DTEXTURE9* importTexGroupColor = new LPDIRECT3DTEXTURE9[64];
	LPDIRECT3DTEXTURE9* importTexGroupNormal = new LPDIRECT3DTEXTURE9[64];
	LPDIRECT3DTEXTURE9* importTexGroupSpecular = new LPDIRECT3DTEXTURE9[64];
	
	for (int i = 0; i < 64; i++)
	{
		importTexGroupColor[i] = NULL;
		importTexGroupNormal[i] = NULL;
		importTexGroupSpecular[i] = NULL;

		pathstreamColor.str(L"");
		pathstreamNormal.str(L"");
		pathstreamSpecular.str(L"");
		
		pathstreamColor<<path<<L"/block"<<i<<L".png";
		pathstreamNormal<<path<<L"/block"<<i<<L"n.png";
		pathstreamSpecular<<path<<L"/block"<<i<<L"s.png";
		D3DXCreateTextureFromFile(SRender::getSingleton()->dev, pathstreamColor.str().c_str(), &importTexGroupColor[i]);
		D3DXCreateTextureFromFile(SRender::getSingleton()->dev, pathstreamNormal.str().c_str(), &importTexGroupNormal[i]);
		D3DXCreateTextureFromFile(SRender::getSingleton()->dev, pathstreamSpecular.str().c_str(), &importTexGroupSpecular[i]);

		if (importTexGroupColor[i] != NULL)
		{
			successfulImports++;
		} else {
			break;
		}
	}

	_iMaterials = successfulImports;

	if (_iMaterials > 0)
	{
		_materials = new SMaterial*[_iMaterials];
		for (int i = 0; i < _iMaterials; i++)
		{
			/* normal is the only thing that really shouldn't ever be null */
			if (importTexGroupNormal[i] == NULL)
			{
				D3DXCreateTextureFromFile(SRender::getSingleton()->dev, L"../Resources/Textures/defaultNormalMap.png", &importTexGroupNormal[i]);
			}

			_materials[i] = new SMaterial(importTexGroupColor[i], importTexGroupNormal[i], importTexGroupSpecular[i]);
		}
	}

	delete [] importTexGroupColor;
	delete [] importTexGroupNormal;
	delete [] importTexGroupSpecular;
}
int SMaterialSet::iMaterials(void)
{
	return _iMaterials;
}
SMaterial* SMaterialSet::materialAtIndex(int index)
{
	if (index < 0 || index >= _iMaterials) return 0;
	return _materials[index];
}