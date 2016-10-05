#include "LGIDX11.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

bool adAPT::LGI_DX11::initialize(const LGIProperties& properties) {
    DXGI_SWAP_CHAIN_DESC swapChainProperties;

    // clear out the struct for use
    ZeroMemory(&swapChainProperties, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    swapChainProperties.BufferCount = 1;                                    // one back buffer
    swapChainProperties.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    swapChainProperties.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    swapChainProperties.OutputWindow = FindWindow(
		std::wstring(properties.windowClassName.begin(), properties.windowClassName.end()).c_str(),
		std::wstring(properties.windowName.begin(), properties.windowName.end()).c_str());

    swapChainProperties.SampleDesc.Count = 4;                               // how many multisamples
    swapChainProperties.Windowed = TRUE;                                    // windowed/full-screen mode

    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &swapChainProperties,
                                  &_swapChain,
                                  &_device,
                                  NULL,
                                  &_deviceContext);

	return true;
}

bool adAPT::LGI_DX11::destroy(void) {
	_swapChain->Release();
    _device->Release();
    _deviceContext->Release();

	return true;
}