
#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

namespace graphics
{
	class d3d
	{
	public:
		d3d() = delete;
		d3d(HWND hWnd);
		~d3d();
		void RenderFrame(void);
	private:
		bool vsyncflag;
		int videomemory;
		char videocarddescription[128];
		IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
		ID3D11Device *dev;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
		ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
	};
}

