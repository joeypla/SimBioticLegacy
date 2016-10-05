#ifndef SR_ANIMATION_H
#define SR_ANIMATION_H
#include <d3dx9.h>

namespace sr{
	struct SR_FRAME : public D3DXFRAME
	{
		D3DXMATRIX combinedTransformationMatrix;
	};

	struct SR_MESHCONTAINER : public D3DXMESHCONTAINER
	{
		D3DXMATRIX** ppFrameMatrices;
		LPD3DXMESH pFinalMesh;
		LPDIRECT3DTEXTURE9* pTextures;
	};

	class AnimatedMeshAllocater : public ID3DXAllocateHierarchy
	{
	public:
		AnimatedMeshAllocater(void);
		~AnimatedMeshAllocater(void);

		STDMETHOD (CreateFrame)(LPCSTR Name, LPD3DXFRAME* ppNewFrame);

		STDMETHOD (CreateMeshContainer)(
			LPCSTR Name,
			CONST D3DXMESHDATA* pMeshData,
			CONST D3DXMATERIAL* pMaterials,
			CONST D3DXEFFECTINSTANCE* pEffectInstances,
			DWORD numMaterials,
			CONST DWORD* pAdjacency,
			LPD3DXSKININFO pSkinInfo,
			LPD3DXMESHCONTAINER* ppNewMeshContainer);

		STDMETHOD (DestroyFrame)(LPD3DXFRAME pFrameToFree);
		STDMETHOD (DestroyMeshContainer)(LPD3DXMESHCONTAINER pMeshContainerToFree);
		DWORD getMaxFrames(void);
	private:
		DWORD MaxFrames;

	};
};

#endif