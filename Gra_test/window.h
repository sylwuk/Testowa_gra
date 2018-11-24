// window.h : include file for window specific operations like
// creating and registering a main window
#pragma once

#include "stdafx.h"
#include "resource.h"
#include "d3d.h"
#include <memory>

namespace app
{
	constexpr UINT32 MAX_LOADSTRING{ 100 };

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	class window
	{
	public:
		window() = delete;
		window(HINSTANCE hInstance, int windowHeight, int windowWidth, float scrDepth, float scrNear);
		~window();
		ATOM MyRegisterClass(HINSTANCE hInstance);
		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
		void RenderFrame(void);
	private:
		HINSTANCE hInst{};                              // current instance
		WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
		WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
		std::unique_ptr<graphics::d3d> pD3d;
		int height{}, width{};
		float screendepth{}, screennear{};
	};
}