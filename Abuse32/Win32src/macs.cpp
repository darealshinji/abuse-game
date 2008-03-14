#include "macs.hpp"

char ErrMsg[1024];
char *QuitMessage;
int ErrCode = 0;

extern HWND hWnd;
#ifndef _SPAINT
extern int AbuseFinished;
void FreeDirectPlay();
#endif

void Shutdown(int Line, char* SrcFile, int code) {
    if (!code) {
        ExitProcess(0);
        exit(0);
    }
    set_dprinter(NULL);
    KillDInput();
#ifndef _SPAINT
    FreeDirectPlay();
#endif
    QuitMessage = (char*) malloc(4096);
    sprintf(QuitMessage, "Abuse has encountered an unrecoverable error and is exiting.\n\nError on line %d of %s:\n\nDescription:\n%s", Line, SrcFile, ErrMsg);
    ErrCode = code;
#ifndef _SPAINT
    AbuseFinished = 1;
#endif
    PostMessage(hWnd, WM_QUIT, code, 0);
    ExitThread(code);
}