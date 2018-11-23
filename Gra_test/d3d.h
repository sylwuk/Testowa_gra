
#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

namespace graphics
{
	constexpr unsigned int MAX_NAMESTRING{ 128 };

	class d3d
	{
	public:
		enum class d3delems : short
		{
			swapchain, device, devicecontext, backbuffer, depthstencilbuffer,
			depthstencilstate, depthstencilview, rasterstate, all
		};

		d3d() = delete;
		d3d(HWND hWnd,
			int screenWidth,
			int screenHeight,
			bool vsync,
			bool fullscreen,
			float screenDepth,
			float screenNear);
		~d3d();
		void RenderFrame(void);
	private:
		void cleanup(d3delems start);
		bool vsyncflag{};
		int videomemory{};
		char videocarddesc[MAX_NAMESTRING];
		IDXGISwapChain *swapchain{};             // the pointer to the swap chain interface
		ID3D11Device *dev{};                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext *devcon{};           // the pointer to our Direct3D device context
		ID3D11RenderTargetView *backbuffer{};    // the pointer to our back buffer
		ID3D11Texture2D *depthstencilbuffer{};
		ID3D11DepthStencilState *depthstencilstate{};
		ID3D11DepthStencilView *depthstencilview{};
		ID3D11RasterizerState *rasterstate{};
		D3DXMATRIX projectionmatrix{};
		D3DXMATRIX worldmatrix{};
		D3DXMATRIX orthomatrix{};
	};
}

