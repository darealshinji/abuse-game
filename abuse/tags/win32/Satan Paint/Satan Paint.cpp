// Satan Paint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "macs.hpp"
#include "event.hpp"
#include "jwindow.hpp"

// Foward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;
HINSTANCE g_hInstance;

int EngineFinished = 0;
extern int _make_argv(char *p, char ***argv);
extern char** _argv;
extern int _argc;
extern int ErrCode;
extern char* QuitMessage;

char ExeName[2048];
start_spaint(int argc, char **argv);

//-----------------------------------------------------------------------------
unsigned long __stdcall SPaintThread(void* Data)
{
    start_spaint(_argc, _argv);
    EngineFinished = 1;
    PostMessage(hWnd, WM_SETFOCUS, 0, 0);
    PostQuitMessage(0);
    ExitThread(0);
    return 0;
}

//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    DWORD SPaintThreadID = 0;
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;
    GetModuleFileName(NULL, ExeName, 2048);
    g_hInstance = hInstance;
    _argv = (char**) malloc(2 * sizeof(char*));
	_argv[0] = ExeName;     /* fill in program name */
	_argc = _make_argv(lpCmdLine, &_argv);
	_argv[_argc] = NULL;
   
    set_mode(19, _argc, _argv); // Set the video mode
    CreateThread(NULL, NULL, SPaintThread, NULL, NULL, &SPaintThreadID);

	MSG msg;
	HACCEL hAccelTable;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR) IDC_SATANPAINT);

	// Main message loop:
    while (GetMessage(&msg, NULL, 0, 0) && !EngineFinished) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
    if (EngineFinished)
        Sleep(100); // Give other threads time to exit; ugly, but it'll work for now
    set_dprinter(NULL);
    KillDInput();
    close_graphics();
    if (ErrCode)
        MessageBox(NULL, QuitMessage, "Error", MB_OK);

	ExitProcess(0);
    return 0;
}

//-----------------------------------------------------------------------------
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//-----------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance; // Store instance handle in our global variable

    WNDCLASS                    wc;

    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SATANPAINT));
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wc.lpszMenuName = "Satan Paint";
    wc.lpszClassName = "Satan Paint";
    RegisterClass(&wc);

    // Create a window
    hWnd = CreateWindowEx(WS_EX_TOPMOST,
                          "Satan Paint",
                          "Satan Paint",
                          WS_POPUP,
                          0,
                          0,
                          320,
                          200,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);
    SetCursor(NULL);
    return 1;
}

//-----------------------------------------------------------------------------
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int focused = 0;
    switch (message) {
        case WM_DESTROY:
            // Clean up and close the app
            PostQuitMessage(0);
            return 0L;

        case WM_SETCURSOR:
            // Turn off the cursor since this is a full-screen app
            SetCursor(NULL);
            return 0;

        case WM_SHOWWINDOW:
            WriteErr(wParam, "Going away");
            return 0;

        case WM_ACTIVATEAPP:
            if (wParam == FALSE) {
                SetDIAcquire = DI_UNACQUIRE_STATE;
                focused = 0;
            }
            else {
                if (wm && !focused)
                    wm->push_event(new event(EV_SCREEN_REFRESH));
                focused = 1;
                SetDIAcquire = DI_ACQUIRE_STATE;
            }
            return 0;
        case WM_QUIT:
            PostQuitMessage(wParam);
            break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}