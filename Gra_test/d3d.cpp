#include "stdafx.h"
#include "d3d.h"
#include <exception>

namespace graphics
{

	d3d::d3d(HWND hWnd,
		int screenWidth,
		int screenHeight,
		bool vsync,
		bool fullscreen,
		float screenDepth,
		float screenNear) :
		vsyncflag(vsync)
	{
		HRESULT result{};
		IDXGIFactory* factory{};
		IDXGIAdapter* adapter{};
		IDXGIOutput* adapterOutput{};
		unsigned int numModes{}, i{}, numerator{}, denominator{}, stringLength{};
		DXGI_MODE_DESC* displayModeList{};
		DXGI_ADAPTER_DESC adapterDesc{};
		D3D_FEATURE_LEVEL featureLevel{};
		ID3D11Texture2D* backBufferPtr{};
		D3D11_TEXTURE2D_DESC depthBufferDesc{};
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		D3D11_RASTERIZER_DESC rasterDesc{};
		float fieldOfView{}, screenAspect{};

		// Create a DirectX graphics interface factory.
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			throw "Unable to create dxgi factory.";
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			factory->Release();
			factory = nullptr;
			throw "Unable to create graphics adapter.";
		}

		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			factory->Release();
			factory = nullptr;
			adapter->Release();
			adapter = nullptr;
			throw "Unable to get adapter outputs.";
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result))
		{
			factory->Release();
			factory = nullptr;
			adapter->Release();
			adapter = nullptr;
			adapterOutput->Release();
			adapterOutput = nullptr;
			throw "Unable to get number of display modes.";
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			factory->Release();
			factory = nullptr;
			adapter->Release();
			adapter = nullptr;
			adapterOutput->Release();
			adapterOutput = nullptr;
			throw "Unable to create display mode list.";
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			factory->Release();
			factory = nullptr;
			adapter->Release();
			adapter = nullptr;
			adapterOutput->Release();
			adapterOutput = nullptr;
			delete[] displayModeList;
			displayModeList = nullptr;
			throw "Unable to fill display mode list.";
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				if (displayModeList[i].Height == (unsigned int)screenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Get the adapter (video card) description.
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			factory->Release();
			factory = nullptr;
			adapter->Release();
			adapter = nullptr;
			adapterOutput->Release();
			adapterOutput = nullptr;
			delete[] displayModeList;
			displayModeList = nullptr;
			throw "Unable to get video card description.";
		}

		// Store the dedicated video card memory in megabytes.
		videomemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		int error = wcstombs_s(&stringLength, videocarddesc, 128, adapterDesc.Description, 128);
		
		factory->Release();
		factory = nullptr;
		adapter->Release();
		adapter = nullptr;
		adapterOutput->Release();
		adapterOutput = nullptr;
		delete[] displayModeList;
		displayModeList = nullptr;

		if (error != 0)
		{
			throw "Unable to get video card name.";
		}

		// create a struct to hold information about the swap chain
		DXGI_SWAP_CHAIN_DESC scd{};

		// clear out the struct for use
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		// fill the swap chain description struct
		scd.BufferCount = 1;                                    // one back buffer
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
		scd.OutputWindow = hWnd;                                // the window to be used
		scd.SampleDesc.Count = 4;                               // how many multisamples
		scd.Windowed = TRUE;                                    // windowed/full-screen mode
		scd.BufferDesc.Height = screenHeight;
		scd.BufferDesc.Width = screenWidth;
		scd.SampleDesc.Count = 1;								// Turn multisampling off.
		scd.SampleDesc.Quality = 0;								// -||-
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Set the scan line ordering and scaling to unspecified.
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;				// Discard the back buffer contents after presenting.
		scd.Flags = 0;											// Don't set the advanced flags.

		// Set the refresh rate of the back buffer.
		if (vsyncflag)
		{
			scd.BufferDesc.RefreshRate.Numerator = numerator;
			scd.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			scd.BufferDesc.RefreshRate.Numerator = 0;
			scd.BufferDesc.RefreshRate.Denominator = 1;
		}

		// Set to full screen or windowed mode.
		if (fullscreen)
		{
			scd.Windowed = false;
		}
		else
		{
			scd.Windowed = true;
		}

		// create a device, device context and swap chain using the information in the scd struct
		result = D3D11CreateDeviceAndSwapChain(NULL,
					 D3D_DRIVER_TYPE_HARDWARE,
					 NULL,
					 NULL,
					 NULL,
					 NULL,
					 D3D11_SDK_VERSION,
					 &scd,
					 &swapchain,
					 &dev,
					 NULL,
					 &devcon);

		if (FAILED(result))
		{
			throw "Swap chain create failure.";
		}

		// get the address of the back buffer
		ID3D11Texture2D *pBackBuffer;
		result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		if (FAILED(result))
		{
			swapchain->Release();
			dev->Release();
			devcon->Release();
			throw "Back buffer init failed.";
		}

		// use the back buffer address to create the render target
		result = dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
		
		if (FAILED(result))
		{
			swapchain->Release();
			dev->Release();
			devcon->Release();
			backbuffer->Release();
			throw "Render target create failed.";
		}

		pBackBuffer->Release();

		// set the render target as the back buffer
		devcon->OMSetRenderTargets(1, &backbuffer, NULL);

		// Set the viewport
		D3D11_VIEWPORT viewport{};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = screenWidth;
		viewport.Height = screenHeight;

		devcon->RSSetViewports(1, &viewport);

		// Initialize the description of the depth buffer.
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
	}

	// this is the function used to render a single frame
	void d3d::RenderFrame()
	{
		// clear the back buffer to a deep blue
		devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

		// do 3D rendering on the back buffer here

		// switch the back buffer and the front buffer
		swapchain->Present(0, 0);
	}

	d3d::~d3d()
	{
		swapchain->Release();
		backbuffer->Release();
		dev->Release();
		devcon->Release();
	}
}
