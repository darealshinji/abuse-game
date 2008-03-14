#include "video.hpp"
#include "macs.hpp"
#include "event.hpp"

#include "dprint.hpp"
#include "crtdbg.h"

#include "windows.h"

int Win_Mouse_X;
int Win_Mouse_Y;
int Win_Mouse_Z;
int Win_Mouse_Buttons;

#define DIRECTINPUT_VERSION 0x0700
#include "dinput.h"

#define DIKEYBOARD_BUFFERSIZE 32
#define DIMOUSE_BUFFERSIZE 256
#define MOUSEX_SENSITIVITY 1.35f
#define MOUSEY_SENSITIVITY 1.35f

#define DI_ERROR_THRESHOLD 20
#define DI_MAX_ERRORS 100
#define KEY_REPEAT_TIME 70
#define KEY_REPEAT_START 300

extern Win_Mouse_X;
extern Win_Mouse_Y;
extern Win_Mouse_Buttons;
extern HINSTANCE g_hInstance;
extern HWND hWnd;

LPDIRECTINPUT7        g_lpDI = NULL;
LPDIRECTINPUTDEVICE7  g_lpDIKeyboard = NULL;
LPDIRECTINPUTDEVICE7  g_lpDIMouse = NULL;
int HaveFocus = 1;

int SetDIAcquire = 0;

int DoneThread = 0;
DWORD EventThreadID = 0;

int CheckDIKeyboard();
int CheckDIMouse();
BOOL WINAPI DI_Init();
void WINAPI KillDInput();
void DI_Unacquire();
void DI_Acquire();

int g_KeyRepeating = 0;
char g_LastKey = 0;
DWORD g_KeyRepeatTimer = 0;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
unsigned long __stdcall EventThread(void* Data)
{
    DI_Init();
    while (!DoneThread) {
        Sleep(10); // Check every 10 ms = 100fps
        
        if (SetDIAcquire == DI_ACQUIRE_STATE) {
            DI_Acquire();
        }
        else if (SetDIAcquire == DI_UNACQUIRE_STATE) {
            g_LastKey = 0; // Don't keep repeating a key after an alt-tab
            DI_Unacquire();
        }

        if (HaveFocus) {
            CheckDIKeyboard();
            CheckDIMouse();
        }
    }
    KillDInput();
    PostQuitMessage(0);
    ExitThread(0);
    return 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void WINAPI KillDInput()
{
    _RPT0(_CRT_WARN, "KillDInput -- Killing DirectInput\n");
    if (g_lpDI) {
        if (g_lpDIKeyboard) {
            /* Always unacquire device before calling Release().              */
             g_lpDIKeyboard->Unacquire();
             g_lpDIKeyboard->Release();
             g_lpDIKeyboard = NULL;
        }
        if (g_lpDIMouse) {
            g_lpDIMouse->Unacquire();
            g_lpDIMouse->Release();
            g_lpDIMouse = NULL;
        }
        g_lpDI->Release();
        g_lpDI = NULL;
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DI_Unacquire()
{
    if (!HaveFocus)
        return;
    if (g_lpDIKeyboard)
        g_lpDIKeyboard->Unacquire();
    if (g_lpDIMouse)
        g_lpDIMouse->Unacquire();
    HaveFocus = 0;
    _RPT0(_CRT_WARN, "DI_Unacquire -- unacquiring mouse and keyboard\n");
    SetDIAcquire = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void DI_Acquire()
{
    if (HaveFocus)
        return;
    if (g_lpDIKeyboard)
        g_lpDIKeyboard->Acquire();
    if (g_lpDIMouse)
        g_lpDIMouse->Acquire();
    HaveFocus = 1;
    _RPT0(_CRT_WARN, "DI_Acquire -- Acquiring mouse and keyboard\n");
    SetDIAcquire = 0;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
BOOL WINAPI DI_Init()
{
    HRESULT hRet;
 
    // Create the DirectInput object.
    hRet = DirectInputCreateEx(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput7, (void**) &g_lpDI, NULL);
    if FAILED(hRet)
        return FALSE;
 
    // Retrieve a pointer to an IDirectInputDevice7 interface
    hRet = g_lpDI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7, (void**) &g_lpDIKeyboard, NULL);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
 
    // Set the data format using the predefined keyboard data
    // format provided by the DirectInput object for keyboards.
    hRet = g_lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
 
    // Set the cooperative level
    hRet = g_lpDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
 
    DIPROPDWORD  dipdw;
    ZeroMemory(&dipdw, sizeof(dipdw));
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = DIKEYBOARD_BUFFERSIZE;
    hRet = g_lpDIKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

    // Get access to the input device.
    hRet = g_lpDIKeyboard->Acquire();
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }

    hRet = g_lpDI->CreateDeviceEx(GUID_SysMouse, IID_IDirectInputDevice7, (void**) &g_lpDIMouse, NULL);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
    
    hRet = g_lpDIMouse->SetDataFormat(&c_dfDIMouse2);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
    
    hRet = g_lpDIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }
    
    dipdw.dwData = DIMOUSE_BUFFERSIZE;
    hRet = g_lpDIMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }

    hRet = g_lpDIMouse->Acquire();
    if FAILED(hRet) {
        KillDInput();
        return FALSE;
    }

    return TRUE;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void event_handler::flush_screen()
{
    mouse->mouse_sprite()->x = mouse->drawx();
    mouse->mouse_sprite()->y = mouse->drawy();
    mouse->mouse_sprite()->get_background();
    mouse->mouse_sprite()->draw();
    update_dirty(screen);
    mouse->mouse_sprite()->restore_background();
    Finish_Frame();
}

unsigned char shift_pressed;

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int key_translate(unsigned char ch)
{
    switch (ch) {
    case 30 : return shift_pressed ? 'A' : 'a'; break;
    case 48 : return shift_pressed ? 'B' : 'b'; break;
    case 46 : return shift_pressed ? 'C' : 'c'; break;
    case 32 : return shift_pressed ? 'D' : 'd'; break;
    case 18 : return shift_pressed ? 'E' : 'e'; break;
    case 33 : return shift_pressed ? 'F' : 'f'; break;
    case 34 : return shift_pressed ? 'G' : 'g'; break;
    case 35 : return shift_pressed ? 'H' : 'h'; break;
    case 23 : return shift_pressed ? 'I' : 'i'; break;
    case 36 : return shift_pressed ? 'J' : 'j'; break;
    case 37 : return shift_pressed ? 'K' : 'k'; break;
    case 38 : return shift_pressed ? 'L' : 'l'; break;
    case 50 : return shift_pressed ? 'M' : 'm'; break;
    case 49 : return shift_pressed ? 'N' : 'n'; break;
    case 24 : return shift_pressed ? 'O' : 'o'; break;
    case 25 : return shift_pressed ? 'P' : 'p'; break;
    case 16 : return shift_pressed ? 'Q' : 'q'; break;
    case 19 : return shift_pressed ? 'R' : 'r'; break;
    case 31 : return shift_pressed ? 'S' : 's'; break;
    case 20 : return shift_pressed ? 'T' : 't'; break;
    case 22 : return shift_pressed ? 'U' : 'u'; break;
    case 47 : return shift_pressed ? 'V' : 'v'; break;
    case 17 : return shift_pressed ? 'W' : 'w'; break;
    case 45 : return shift_pressed ? 'X' : 'x'; break;
    case 21 : return shift_pressed ? 'Y' : 'y'; break;
    case 44 : return shift_pressed ? 'Z' : 'z'; break;
    case 51 : return shift_pressed ? '<' : ','; break;
    case 52 : return shift_pressed ? '>' : '.'; break;
    case 53 : return shift_pressed ? '?' : '/'; break;      
    case 39 : return shift_pressed ? ':' : ';'; break;
    case 40 : return shift_pressed ? '"' : '\''; break;
    case 26 : return shift_pressed ? '{' : '['; break;
    case 27 : return shift_pressed ? '}' : ']'; break;
    case 43 : return shift_pressed ? '|' : '\\'; break;
    case 13 : return shift_pressed ? '+' : '='; break;
    case 12 : return shift_pressed ? '_' : '-'; break;      
    case 11 : return shift_pressed ? ')' : '0'; break;
    case 10 : return shift_pressed ? '(' : '9'; break;
    case  9 : return shift_pressed ? '*' : '8'; break;
    case  8 : return shift_pressed ? '&' : '7'; break;
    case  7 : return shift_pressed ? '^' : '6'; break;
    case  6 : return shift_pressed ? '%' : '5'; break;
    case  5 : return shift_pressed ? '$' : '4'; break;
    case  4 : return shift_pressed ? '#' : '3'; break;
    case  3 : return shift_pressed ? '@' : '2'; break;
    case  2 : return shift_pressed ? '!' : '1'; break;
    case 41 : return shift_pressed ? '~' : '`'; break;
     
    case 14 : return JK_BACKSPACE; break;
    case 15 : return JK_TAB; break;
    case 28 : return JK_ENTER; break;
    case 1 : return JK_ESC; break;
    case 57 : return JK_SPACE; break;
      
    case 72 : return JK_UP; break;
    case 80 : return JK_DOWN; break;
    case 75 : return JK_LEFT; break;
    case 77 : return JK_RIGHT; break;
    case 29 : return JK_CTRL_L; break;
    case 56 : return JK_ALT_L; break;
    case 42 : return JK_SHIFT_L; break;
    case 54 : return JK_SHIFT_R; break;
    case 58 : return JK_CAPS; break;
    case 69 : return JK_NUM_LOCK; break;
    case 71 : return JK_HOME; break;
    case 79 : return JK_END; break;
    case 83 : return JK_DEL; break;
    case 82 : return JK_INSERT; break;

    case 59 : return JK_F1; break;
    case 60 : return JK_F2; break;
    case 61 : return JK_F3; break;
    case 62 : return JK_F4; break;
    case 63 : return JK_F5; break;
    case 64 : return JK_F6; break;
    case 65 : return JK_F7; break;
    case 66 : return JK_F8; break;
    case 67 : return JK_F9; break;
    case 68 : return JK_F10; break;
    case 73 : return JK_PAGEUP; break;
    case 81 : return JK_PAGEDOWN; break;
    default :
        return 0;      
    }
}  

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int CheckDIMouse() {
    if (!g_lpDIMouse)
        return 0;
    static float MouseXAccum = 0, MouseYAccum = 0;
    static int errcount = 0;
    HRESULT hRet;
    DIDEVICEOBJECTDATA DIMouseEvents[DIMOUSE_BUFFERSIZE];
    DWORD Count = DIMOUSE_BUFFERSIZE;
    hRet = g_lpDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), DIMouseEvents, &Count, 0);
    if (hRet != DI_OK)
        if (hRet == DIERR_INPUTLOST || hRet == DIERR_NOTACQUIRED)
            return 0;
        else {
            errcount++;
            if ((errcount % DI_ERROR_THRESHOLD) == 0)
                dprintf("CheckDIMouse -- Error getting mouse data.  (hRet = 0x%x)\n", hRet);
            if (errcount == DI_MAX_ERRORS)
                WriteErr(0, "Too many errors with DirectInput -- shutting down the engine to be safe\n");
            return 0;
        }
    else
        errcount = 0;
        for (DWORD i = 0; i < Count; i++) {
            switch (DIMouseEvents[i].dwOfs) {
                case DIMOFS_X:
                    MouseXAccum += (int) DIMouseEvents[i].dwData;
                    break;
                case DIMOFS_Y:
                    MouseYAccum += (int) DIMouseEvents[i].dwData;
                    break;
                case DIMOFS_Z:
                    if ((int) DIMouseEvents[i].dwData > 0)
                        Win_Mouse_Z++;
                    else if ((int) DIMouseEvents[i].dwData < 0)
                        Win_Mouse_Z--;
                    break;
                case DIMOFS_BUTTON0:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 1;
                    else
                        Win_Mouse_Buttons &= ~1;
                    break;
                case DIMOFS_BUTTON1:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 2;
                    else
                        Win_Mouse_Buttons &= ~2;
                    break;
                case DIMOFS_BUTTON2:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 4;
                    else
                        Win_Mouse_Buttons &= ~4;
                    break;
                case DIMOFS_BUTTON3:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 8;
                    else
                        Win_Mouse_Buttons &= ~8;
                    break;
                case DIMOFS_BUTTON4:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 16;
                    else
                        Win_Mouse_Buttons &= ~16;
                    break;
                case DIMOFS_BUTTON5:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 32;
                    else
                        Win_Mouse_Buttons &= ~32;
                    break;
                case DIMOFS_BUTTON6:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 64;
                    else
                        Win_Mouse_Buttons &= ~64;
                    break;
                case DIMOFS_BUTTON7:
                    if (DIMouseEvents[i].dwData & 0x80)
                        Win_Mouse_Buttons |= 128;
                    else
                        Win_Mouse_Buttons &= ~128;
                    break;
            }

        }
        int quotient;
        quotient = (int) (MouseXAccum / MOUSEX_SENSITIVITY);
        Win_Mouse_X += quotient;
        MouseXAccum = MouseXAccum - ((float) quotient * MOUSEX_SENSITIVITY);
        if (Win_Mouse_X > (signed) xres)
            Win_Mouse_X = xres;
        else if (Win_Mouse_X < 0)
            Win_Mouse_X = 0;
        quotient = (int) (MouseYAccum / MOUSEY_SENSITIVITY);
        Win_Mouse_Y += quotient;
        MouseYAccum = MouseYAccum - ((float) quotient * MOUSEY_SENSITIVITY);
        if (Win_Mouse_Y > (signed) yres)
            Win_Mouse_Y = yres;
        else if (Win_Mouse_Y < 0)
            Win_Mouse_Y = 0;

    return 1;
}

unsigned char key_que[256], key_que_head = 0, key_que_tail = 0,
                alt_flag = 0, ctrl_flag = 0, shift_flag = 0;
static int new_key = 0;              
/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void keyboard_handler(unsigned char k)
{  
    if (k & 128) {
        g_KeyRepeating = g_LastKey = 0;
    }
    else if (k != g_LastKey) {
        g_KeyRepeating = 0;
        g_LastKey = k;
        g_KeyRepeatTimer = GetTickCount();
    }
    if (alt_flag && (k == DIK_TAB || k == DIK_RETURN)) {
        if (k == DIK_RETURN)
            ToggleWindowedMode();
        return;
    }
    if (k != 224)                               // 224 is extended keyboard info
                                                // we can just throw it away
        key_que[key_que_head++] = k;            // stick it in the que  
    switch (k & 127) {
        case 56 : alt_flag = !(k & 128);    break;
        case 29 : ctrl_flag = !(k & 128);   break;
        case 54 :
        case 42 : shift_flag = !(k & 128);  break;
    }
    new_key = 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int CheckDIKeyboard() {
    if (!g_lpDIKeyboard)
        return 0;
    HRESULT hRet;
    DIDEVICEOBJECTDATA DIKeys[DIKEYBOARD_BUFFERSIZE];
    DWORD Count = DIKEYBOARD_BUFFERSIZE;
    hRet = g_lpDIKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), DIKeys, &Count, 0);
    if (hRet != DI_OK)
        if (hRet == DIERR_INPUTLOST || hRet == DIERR_NOTACQUIRED)
            return 0;
        else
            dprintf("CheckDIKeyboard -- Error getting keyboard data.  (hRet = 0x%x)\n", hRet);
    else
        for (DWORD i = 0; i < Count; i++)
            keyboard_handler((unsigned char) ((~DIKeys[i].dwData & 0x80) + (DIKeys[i].dwOfs & 0x7f)));
    
    if (!HaveFocus) // Don't do key repeats when we don't have focus
        return 1;
    DWORD CurrentTime = GetTickCount();
    if (g_LastKey && !g_KeyRepeating && g_KeyRepeatTimer + KEY_REPEAT_START >= CurrentTime) {
        g_KeyRepeatTimer += KEY_REPEAT_START;
        g_KeyRepeating = 1;
    }
    if (g_KeyRepeating)
        while (g_KeyRepeatTimer + KEY_REPEAT_TIME < CurrentTime) {
            keyboard_handler(g_LastKey);
            g_KeyRepeatTimer += KEY_REPEAT_TIME;
        }
    return 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int event_handler::get_key_flags()
{
    if (!new_key)
        return 0;
    new_key = 0;
    return 1;
}

char *key_names[]= {"Up Arrow","Down Arrow","Left Arrow","Right Arrow",
                    "Left Ctrl","Right Ctrl","Left Alt","Right Alt",
                    "Left Shift","Right Shift","Caps Lock","Num Lock",
                    "Home","End","Del","F1","F2","F3","F4","F5","F6",
                    "F7","F8","F9","F10"};

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
char *jkey_name(int key)
{
    static char sing[2];
    if (key > 255)
        return key_names[key - 256];
    else if (key == JK_BACKSPACE)
        return "Backspace";
    else if (key == JK_TAB)
        return "Tab";
    else if (key == JK_ENTER)
        return "Enter";
    else if (key == JK_ESC)
        return"Esc";
    else if (key == JK_SPACE)
        return "Space";
    else if (isprint(key)) {
        sing[0]=key;
        sing[1]=0;
        return sing;
    }
    return NULL;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int event_handler::event_waiting()
{
//    int hr;
//    MSG inMSG;
//    CheckDIKeyboard();
//    CheckDIMouse();
//    hr = PeekMessage(&inMSG, NULL, 0, 0, PM_REMOVE | PM_NOYIELD);
//    if (hr > 0) {
//        TranslateMessage(&inMSG);
//        DispatchMessage(&inMSG);
//    }
    event *evt;
  
    while (key_que_head != key_que_tail) {
        int up = key_que[key_que_tail] & 128,
            key = key_que[key_que_tail] & 127;
      
        evt = new event;
   
        evt->key = key_translate(key);
        if (up) {
            evt->type = EV_KEYRELEASE;
        }
        else { 
            evt->type = EV_KEY;
        }
        if (evt->key == JK_SHIFT_L || evt->key == JK_SHIFT_R)
            shift_pressed = !up;    
        events.add_end((linked_node*) evt);     
        ewaiting = 1;     
        key_que_tail++;
    }
    if (mhere) {
        mouse->update();
        if (mouse->lastx() != mouse->x() || mouse->lasty() != mouse->y() || mouse->last_button() != mouse->button() || mouse->z() != mouse->lastz())
            ewaiting = 1;
    }
    return ewaiting;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
event_handler::event_handler(image *screen, palette *pal)
{
//    DI_Init();
    CHECK(screen && pal);
    mouse = new JCMouse(screen, pal);
    mhere = mouse->exsist();
    last_keystat = get_key_flags();
    ewaiting = 0;
    CreateThread(NULL, 0, EventThread, NULL, 0, &EventThreadID);
    SetThreadPriority((void*) EventThreadID, THREAD_PRIORITY_TIME_CRITICAL);
}

/* --------------------------------------------------------------------------- 
event_handler::get_event 
    11/20/2000 -- Jeremy Scott
    This is going to be ugly.  Hooking into the Windows messaging system will
    probably be one of the biggest chores of porting this code.

    Looked like it worked beautifully in DOS ...  Unfortunately, Win32 ain't
    DOS.

    1/7/2000 -- JS
    Actually wasn't bad.  Just added the peek/translate/dispatch calls to hook
    into Windows ...  Then added the CheckDIx() functions for DirectInput.  No
    problem.

 ------------------------------------------------------------------------- /**/
void event_handler::get_event(event &ev)
{
    CheckDIKeyboard();
    CheckDIMouse();
/*    int hr;
    MSG inMSG;
    hr = PeekMessage(&inMSG, NULL, 0, 0, PM_REMOVE | PM_NOYIELD);
    if (hr > 0) {
        TranslateMessage(&inMSG);
        DispatchMessage(&inMSG);
    } /**/
    int kf, kf_change = 0;
    event *ep;
    while (!ewaiting)
        event_waiting();

    ep = (event*) events.first();
    if (ep) {
        ev = *ep;
        events.unlink((linked_node*) ep);
        delete ep;
        ewaiting = events.first() != NULL;
    }
    else {
        kf = get_key_flags();
        if (kf != last_keystat) {
            last_keystat = kf;
            kf_change = 1;
            ev.type = EV_KEY_SPECIAL;
            ev.key_special.shift = kf & 2;
            ev.key_special.ctrl = kf & 4;
            ev.key_special.alt = kf & 8;
        }
        else if (mhere && (mouse->last_button() != mouse->button()))
            ev.type = EV_MOUSE_BUTTON;
        else if (mhere && (mouse->lastx() != mouse->x() || mouse->lasty() != mouse->y() || mouse->lastz() != mouse->z()))
            ev.type = EV_MOUSE_MOVE;
        else 
            ev.type = EV_SPURIOUS;

        if (ev.type == EV_MOUSE_MOVE) {
            mouse->mouse_sprite()->x = mouse->x();
            mouse->mouse_sprite()->y = mouse->y();
        }    
        ewaiting = 0;
    }
    ev.mouse_move.x = mouse->x();
    ev.mouse_move.y = mouse->y();
    ev.mouse_move.z = mouse->z() - mouse->lastz();
    ev.mouse_button = mouse->button();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void event_handler::add_redraw(int X1, int Y1, int X2, int Y2, void *Start)
{
    event *ev;
    ev = new event;
    ev->type = EV_REDRAW;
    ev->redraw.x1 = X1; ev->redraw.x2 = X2;
    ev->redraw.y1 = Y1; ev->redraw.y2 = Y2; 
    ev->redraw.start = Start;
    events.add_end((linked_node*) ev);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
event_handler::~event_handler()
{
    delete mouse;
    DoneThread = 1;
    DWORD hRet = WaitForSingleObject((void*) EventThreadID, 1000);
}