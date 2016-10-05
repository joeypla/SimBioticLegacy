 #ifndef SDeferredRenderer_H
#define SDeferredRenderer_H

#include <d3d9.h>
#include <d3dx9.h>

#include <string>

#include "psMath.h"
class SMaterialSet;
class SMesh;
class S3D;

class SDeferredRenderer
{
public:
	SDeferredRenderer(void);
	~SDeferredRenderer(void);

	bool compileFX(std::wstring path, LPD3DXEFFECT* effect);
	void render(void);

	void setSkybox(SMaterialSet* material);
private:
	D3DXMATRIX                             view;
	D3DXMATRIX                             proj;
	D3DXMATRIX                         lastView;
	D3DXMATRIX                         lastProj;

	bool                           _setup(void);
	
	void             _render_ClearGBuffer(void);
	void                   _render_Skybox(void);
	void                  _render_GBuffer(void);
	void               _render_ShadowMaps(void);
	void              _render_LightBuffer(void);
	void              _render_BloomBuffer(void);
	void                     _render_SSAO(void);
	void               _render_MotionBlur(void);
	void           _render_downscaleScene(void);
	void       _render_blurDownscaledScene(void);
	void                    _render_Final(void);
	void          _render_combineFinalDOF(void);

	LPD3DXEFFECT               _sh_ClearGBuffer;
	LPD3DXEFFECT                    _sh_GBuffer;
	LPD3DXEFFECT                 _sh_AmbientFog;
	LPD3DXEFFECT                _sh_Light_Point;
	LPD3DXEFFECT            _sh_Light_Spotlight;
	LPD3DXEFFECT           _sh_Light_DepthWrite;
	LPD3DXEFFECT                       _sh_SSAO;
	LPD3DXEFFECT                   _sh_SSAOblur;
	LPD3DXEFFECT           _sh_Bloom_Horizontal;
	LPD3DXEFFECT             _sh_Bloom_Vertical;
	LPD3DXEFFECT                 _sh_gaussBlurX;
	LPD3DXEFFECT                 _sh_gaussBlurY;
	LPD3DXEFFECT                 _sh_MotionBlur;
	LPD3DXEFFECT                _sh_downscale16;
	LPD3DXEFFECT                    _sh_Compose;
	LPD3DXEFFECT            _sh_combineFinalDOF;

	LPDIRECT3DTEXTURE9           _rt_GBuffer[4];
	LPDIRECT3DTEXTURE9          _rt_LightBuffer;
	LPDIRECT3DTEXTURE9          _rt_BloomBuffer;
	LPDIRECT3DTEXTURE9    _rt_BloomBlurChain[3];
	LPDIRECT3DTEXTURE9                 _rt_SSAO;
	LPDIRECT3DTEXTURE9             _rt_SSAOBlur;
	LPDIRECT3DTEXTURE9           _rt_MotionBlur;
	LPDIRECT3DTEXTURE9      _rt_downscaledScene;
	LPDIRECT3DTEXTURE9 _rt_downscaledSceneBlurredH;
	LPDIRECT3DTEXTURE9 _rt_downscaledSceneBlurredHV;
	LPDIRECT3DTEXTURE9                _rt_Final;

	LPDIRECT3DSURFACE9           _sf_backbuffer;
	LPDIRECT3DSURFACE9           _sf_GBuffer[4];
	LPDIRECT3DSURFACE9          _sf_LightBuffer;
	LPDIRECT3DSURFACE9          _sf_BloomBuffer;
	LPDIRECT3DSURFACE9    _sf_BloomBlurChain[3];
	LPDIRECT3DSURFACE9                 _sf_SSAO;
	LPDIRECT3DSURFACE9             _sf_SSAOBlur;
	LPDIRECT3DSURFACE9           _sf_MotionBlur;
	LPDIRECT3DSURFACE9       _sf_downscaledScene;
	LPDIRECT3DSURFACE9 _sf_downscaledSceneBlurredH;
	LPDIRECT3DSURFACE9 _sf_downscaledSceneBlurredHV;
	LPDIRECT3DSURFACE9                _sf_Final;

	LPDIRECT3DVERTEXBUFFER9             _vb_quad;
	LPDIRECT3DINDEXBUFFER9              _ib_quad;

	LPDIRECT3DVERTEXBUFFER9           _vb_skybox;
	LPDIRECT3DINDEXBUFFER9           _ib_skybox;

	LPD3DXMESH                       _geom_point;
	LPD3DXMESH                   _geom_spotlight;
	LPDIRECT3DVERTEXBUFFER9        _vb_spotlight;
	LPDIRECT3DINDEXBUFFER9         _ib_spotlight;
	LPDIRECT3DTEXTURE9               _SSAO_noise;

	void               _updateLightEffects(void);

	psMath::float3                ssaoKernel[64];

	S3D*                            skyboxObject;
	SMesh*                            skyboxMesh;
	SMaterialSet*                 skyboxMaterial;
};

#endif