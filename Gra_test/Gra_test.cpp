// Gra_test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Gra_test.h"
#include "window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	app::window CurrentWindow(hInstance, 800, 600);

    // Initialize global strings
	CurrentWindow.MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!CurrentWindow.InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRATEST));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		CurrentWindow.RenderFrame();
    }

    return (int) msg.wParam;
}
