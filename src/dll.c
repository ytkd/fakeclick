#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include "cmd.h"

#define TYPE_TIMEOUT 500

static HWND vhwndApp = NULL;
static HHOOK vMouseHook = NULL;
static HHOOK vKeyHook = NULL;
static HANDLE vMtx = NULL;;
static unsigned int vPostTime = 0;
static unsigned int vfEnable  = 0;
static unsigned int vfButton = 0;
static POINT vLastPt = {0, 0};
static unsigned int vfCount = 0;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam);
int __stdcall InstallHook(HINSTANCE h, HWND hwnd);
int __stdcall UninstallHook(void);

BOOL __stdcall DllEntryPoint(HINSTANCE hInst, DWORD dwReason, void *p)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		UninstallHook();
		break;
	}
	return TRUE;
}


int __stdcall InstallHook(HINSTANCE h, HWND hwnd)
{
	vhwndApp = hwnd;
	if (vMouseHook == NULL) {
		vMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, h, 0);
	}
	if (vKeyHook == NULL) {
		vKeyHook = SetWindowsHookEx(WH_KEYBOARD, KeyProc, h, 0);
	}
	vMtx = CreateMutex(NULL, FALSE, NULL);
	return (int)vMouseHook;
}

int __stdcall UninstallHook(void)
{
	if (vMouseHook) {
		UnhookWindowsHookEx(vMouseHook);
		vMouseHook = 0;
	}
	if (vKeyHook) {
		UnhookWindowsHookEx(vKeyHook);
		vKeyHook = 0;
	}
	CloseHandle(vMtx);
	return 0;
}



LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	POINT *pt;
	if (nCode < 0) {
		return CallNextHookEx(vMouseHook, nCode, wParam, lParam);
	}
	if (WM_MOUSEFIRST <= wParam && wParam <=0x020e) {
		pt = &((LPMOUSEHOOKSTRUCT)lParam)->pt;
		if (pt->x != vLastPt.x || pt->y != vLastPt.y) {
			vLastPt = *pt;
			vPostTime = GetTickCount();
			vfEnable = 1;
		}
	}

	return CallNextHookEx(vMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	int stat;
	int ev;
	int button;

	stat = (lParam >> 30) & 3;

	if (nCode < 0) {
		return CallNextHookEx(vKeyHook, nCode, wParam, lParam);
	}
	if (TYPE_TIMEOUT < (GetTickCount() - vPostTime) && (vfButton  ==  0)) {
		return CallNextHookEx(vKeyHook, nCode, wParam, lParam);
	}
	if (vfEnable  ==  0) {
		return CallNextHookEx(vKeyHook, nCode, wParam, lParam);
	}

	vPostTime = GetTickCount();

	ev = 0;
	switch (wParam) {
	case 'F':
	case 'J':/*  Primary Button  */
		ev = IDM_LDOWN;
		button  = 1;
		vfEnable = 1;
		break;
	case 'D':
	case 'K':/*  Secondary Button  */
		ev = IDM_RDOWN;
		button  = 2;
		vfEnable = 1;
		break;
	case VK_SHIFT:
	case VK_CONTROL:
	case VK_MENU:
		break;
	default:
		vfEnable = 0;
		break;
	}

	if (vfEnable) {
		/* bit 31: transit state. 0: pressed  1: released
                   bit 30: prev state.    0: released 1: pressed
		 */
		stat = (lParam >>30) & 3;
		switch (stat) {
		case 0:/*  release to pressed  */
			if (!(vfButton & button)) {
				vfButton |= button;
			}
			break;

		case 1:
		case 2:
			ev = 0;
			break;

		case 3:/*  press to release  */
			if (vfButton & button) {
				vfButton &= ~button;
				ev++; /* change down event to up */
			}
			break;
		}
		if (ev) {
			vfCount++;
			switch (ev) {
			case IDM_LDOWN:
				ev = MOUSEEVENTF_LEFTDOWN;
				break;
			case IDM_LUP:
				ev = MOUSEEVENTF_LEFTUP;
				break;
			case IDM_RDOWN:
				ev = MOUSEEVENTF_RIGHTDOWN;
				break;
			case IDM_RUP:
				ev = MOUSEEVENTF_RIGHTUP;
				break;
			}
			mouse_event(ev, 0, 0, 0, 0);

			/* break chain. */
			return 1;
		}
	}

	return CallNextHookEx(vKeyHook, nCode, wParam, lParam);
}


int __stdcall get_stat(DWORD *stat)
{
	stat[0] = vPostTime;
	stat[1] = vfEnable;
	stat[2] = vfButton;
	stat[3] = vfCount;

	return 0;
}



