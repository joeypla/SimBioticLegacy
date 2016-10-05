#include "LowLevelGraphicsInterface.h"

#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx10.h>

namespace adAPT {
	class LGI_DX11 : public LowLevelGraphicsInterface {
	public:
		virtual bool initialize(const LGIProperties& properties);
		virtual bool destroy(void);

		virtual bool setRenderTarget(const RenderTarget* renderTarget);
		virtual bool setStreamSource(const VertexBuffer* vertexBuffer);
	private:
		IDXGISwapChain* _swapChain;
		ID3D11Device* _device;
		ID3D11DeviceContext* _deviceContext;
	};
};