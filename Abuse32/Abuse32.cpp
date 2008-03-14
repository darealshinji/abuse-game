//-----------------------------------------------------------------------------
// File: DDEx1.CPP
//
// Desc: Direct Draw example program 1.  Creates a Direct Draw 
//       object and then a primary surface with a back buffer.
//       Slowly flips between the primary surface and the back
//       buffer.  Press F12 to terminate the program.
//
// Copyright (c) 1995-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <windows.h>
#include "Abuse32.h"
#include <stdio.h>
#include <stdarg.h>
#include "game.hpp"
#include "event.hpp"
#include "jwindow.hpp"

//-----------------------------------------------------------------------------
// Local definitions
//-----------------------------------------------------------------------------
#define NAME                "Abuse"
#define TITLE               "Abuse"

HWND hWnd;
HINSTANCE g_hInstance;

extern int HaveFocus;
//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
long FAR PASCAL
WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
static HRESULT
InitApp(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASS                    wc;

    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ABUSE32));
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass(&wc);

    // Create a window
    hWnd = CreateWindowEx(WS_EX_TOPMOST,
                          NAME,
                          TITLE,
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

int AbuseFinished = 0;
int _make_argv(char *p, char ***argv);
extern char** _argv;
extern int _argc;
char ExeName[2048];

//-----------------------------------------------------------------------------
unsigned long __stdcall AbuseThread(void* Data)
{
    start_abuse(_argc, _argv);
    AbuseFinished = 1;
    PostMessage(hWnd, WM_QUIT, 0, 0);
    PostQuitMessage(0);
    ExitThread(0);
    return 0;
}

extern int ErrCode;
extern char* QuitMessage;
//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Initialization, message loop
//-----------------------------------------------------------------------------
int PASCAL
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    DWORD AbuseThreadID = 0;
    if (!InitApp(hInstance, nCmdShow))
        return FALSE;
    GetModuleFileName(NULL, ExeName, 2048);
    g_hInstance = hInstance;
    _argv = (char**) malloc(2 * sizeof(char*));
	_argv[0] = ExeName;     /* fill in program name */
	_argc = _make_argv(lpCmdLine, &_argv);
	_argv[_argc] = NULL;
   
    set_mode(19, _argc, _argv); // Set the video mode

//    start_abuse(_argc, _argv);
    CreateThread(NULL, NULL, AbuseThread, NULL, NULL, &AbuseThreadID);
    
	MSG msg;
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR) IDC_ABUSE32);

    while (GetMessage(&msg, NULL, 0, 0) && !AbuseFinished) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
    WaitForSingleObject((HANDLE) AbuseThreadID, INFINITE);  /**/
    if (AbuseFinished)
        Sleep(100); // Give other threads time to exit; ugly, but it'll work for now
    set_dprinter(NULL);
    KillDInput();
    close_graphics();
    if (ErrCode)
        MessageBox(NULL, QuitMessage, "Error in Abuse engine", MB_OK);

	ExitProcess(0);
    return 0;
}
