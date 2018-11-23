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

		if (screenWidth <= 0 or screenHeight <= 0)
		{
			throw "Incorrect window size.";
		}

		HRESULT result{};
		IDXGIFactory* factory{};
		IDXGIAdapter* adapter{};
		IDXGIOutput* adapterOutput{};
		UINT numModes{}, i{}, numerator{}, denominator{}, stringLength{};
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
			if (displayModeList[i].Width == static_cast<UINT>(screenWidth))
			{
				if (displayModeList[i].Height == static_cast<UINT>(screenHeight))
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

		// The swap chain is the front and back buffer to which the graphics will be drawn.
		// Generally you use a single back buffer, do all your drawing to it, and then swap it to the front buffer which then displays on the user's screen. 
		// That is why it is called a swap chain.

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
			cleanup(d3delems::devicecontext);
			throw "Back buffer init failed.";
		}

		// use the back buffer address to create the render target
		result = dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
		if (FAILED(result))
		{
			cleanup(d3delems::devicecontext);
			throw "Render target create failed.";
		}

		pBackBuffer->Release();

		// set the render target as the back buffer
		devcon->OMSetRenderTargets(1, &backbuffer, NULL);

		// The viewport also needs to be setup so that Direct3D can map clip space coordinates to the render target space. 
		// Set this to be the entire size of the window.

		// Set the viewport
		D3D11_VIEWPORT viewport{};
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.Width = static_cast<FLOAT>(screenWidth);
		viewport.Height = static_cast<FLOAT>(screenHeight);

		devcon->RSSetViewports(1, &viewport);

		// We will also need to set up a depth buffer description.
		// We'll use this to create a depth buffer so that our polygons can be rendered properly in 3D space.
		// At the same time we will attach a stencil buffer to our depth buffer.
		// The stencil buffer can be used to achieve effects such as motion blur, volumetric shadows, and other things.
		
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

		// Create the texture for the depth buffer using the filled out description.
		result = dev->CreateTexture2D(&depthBufferDesc, NULL, &depthstencilbuffer);
		if (FAILED(result))
		{
			cleanup(d3delems::backbuffer);
			throw "Unable to create depth stencil buffer.";
		}

		// Now we need to setup the depth stencil description.
		// This allows us to control what type of depth test Direct3D will do for each pixel.
		
		// Initialize the description of the stencil state.
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		result = dev->CreateDepthStencilState(&depthStencilDesc, &depthstencilstate);
		if (FAILED(result))
		{
			cleanup(d3delems::depthstencilbuffer);
			throw "Unable to create depth stencil state.";
		}

		// Set the depth stencil state.
		devcon->OMSetDepthStencilState(depthstencilstate, 1);

		// Creating depth stencil description 
		// so that Direct3D knows to use the depth buffer as a depth stencil texture.

		// Initailze the depth stencil view.
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		// Set up the depth stencil view description.
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		// Create the depth stencil view.
		result = dev->CreateDepthStencilView(depthstencilbuffer, &depthStencilViewDesc, &depthstencilview);
		if (FAILED(result))
		{
			cleanup(d3delems::depthstencilstate);
			throw "Unable to create depth stencil view.";
		}

		// OMSetRenderTargets will bind the render target view and the depth stencil buffer to the output render pipeline.
		// This way the graphics that the pipeline renders will get drawn to the back buffer.

		// Bind the render target view and depth stencil buffer to the output render pipeline.
		devcon->OMSetRenderTargets(1, &backbuffer, depthstencilview);

		// Creating rasterizer state.
		// It will give the control over how polygons are rendered.
		// We can do things like make our scenes render in wireframe mode or have DirectX draw both the front and back faces of polygons. 
		// By default DirectX already has a rasterizer state set up and working the exact same
		// as the one below but you have no control to change it unless you set up one yourself.

		// Setup the raster description which will determine how and what polygons will be drawn.
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		// Create the rasterizer state from the description we just filled out.
		result = dev->CreateRasterizerState(&rasterDesc, &rasterstate);
		if (FAILED(result))
		{
			cleanup(d3delems::depthstencilview);
			throw "Unable to create rasterizer state.";
		}

		// Now set the rasterizer state.
		devcon->RSSetState(rasterstate);

		// Creating projection matrix.
		// The projection matrix is used to translate the 3D scene into the 2D viewport space.
		// Keeping the copy of this matrix so that it can be passed to shaders that will be used to render the scenes.
	
		// Setup the projection matrix.
		fieldOfView = static_cast<FLOAT>(D3DX_PI / 4.0f);
		screenAspect = static_cast<FLOAT>(screenWidth) / static_cast<FLOAT>(screenHeight);

		// Create the projection matrix for 3D rendering.
		D3DXMatrixPerspectiveFovLH(&projectionmatrix, fieldOfView, screenAspect, screenNear, screenDepth);

		// Creating the world matrix.
		// This matrix is used to convert the vertices of objects into vertices in the 3D scene.
		// This matrix will also be used to rotate, translate, and scale the objects in 3D space. 
		// From the start we will just initialize the matrix to the identity matrix and keep a copy of it in this object. 
		// The copy will be needed to be passed to the shaders for rendering.

		// Initialize the world matrix to the identity matrix.
		D3DXMatrixIdentity(&worldmatrix);

		// Creating ortographic matrix.
		// This matrix is used for rendering 2D elements like user interfaces on the screen
		// allowing us to skip the 3D rendering.

		// Create an orthographic projection matrix for 2D rendering.
		D3DXMatrixOrthoLH(&orthomatrix,
			static_cast<FLOAT>(screenWidth),
			static_cast<FLOAT>(screenHeight),
			screenNear,
			screenDepth);
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

	void d3d::cleanup(d3delems start)
	{
		// Exceptions can be thrown if releasing swapchain in fullscreen mode
		swapchain->SetFullscreenState(false, NULL);
		switch (start)
		{
		case d3delems::all:
		case d3delems::rasterstate:
			rasterstate->Release();
		case d3delems::depthstencilview:
			depthstencilview->Release();
		case d3delems::depthstencilstate:
			depthstencilstate->Release();
		case d3delems::depthstencilbuffer:
			depthstencilbuffer->Release();
		case d3delems::backbuffer:
			backbuffer->Release();
		case d3delems::devicecontext:
			devcon->Release();
		case d3delems::device:
			dev->Release();
		case d3delems::swapchain:
			swapchain->Release();
		}
	}

	d3d::~d3d()
	{
		cleanup(d3delems::all);
	}
}
