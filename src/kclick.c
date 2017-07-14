#define STRICT
#include <windows.h>

#include "cmd.h"

#define TRAYMESSAGE (WM_APP+100)
#define BK_LSHIFT 0x01
#define BK_RSHIFT 0x02
#define BK_SHIFT (BK_LSHIFT|BK_RSHIFT)
#define BK_LCTRL  0x04
#define BK_RCTRL  0x08
#define BK_CTRL (BK_LCTRL|BK_RCTRL)

#define W_CLASS "kbClick"
#define TIMER_WAIT  1000
#define TIMER_WAIT2 300
#define TIMER_WAIT3 8000
#define ID_TIMER  1000
#define ID_TIMER2 1001
#define ID_TIMER3 1002


static NOTIFYICONDATA ni = {
	sizeof(NOTIFYICONDATA),
	NULL,
	1,
	NIF_MESSAGE|NIF_ICON|NIF_TIP,
	TRAYMESSAGE,
	NULL,
	"キーでクリックするやつ"
};

static HMENU hMenu;

static HANDLE dll;

HWND init_application(HINSTANCE hInst);
void uninstall_hook(void);

#ifdef _DEBUG
void Entry(void);
/*
*  Entry point (debug)
*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
	Entry();
	return 0;
}
#endif


void Entry(void)
{
	MSG msg;
	HINSTANCE hInst;

	hInst=GetModuleHandle(NULL);

	if (!init_application(hInst)) {
		ExitProcess(0);
	}
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(msg.wParam);
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;

	switch (msg) {
	case WM_CREATE:
		ni.hWnd=hwnd;
		ni.hIcon = (HICON)LoadIcon(NULL, IDI_WINLOGO);
		Shell_NotifyIcon(NIM_ADD, &ni);
		hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MF_STRING, IDM_QUIT, "終了");
		break;
	case WM_COMMAND:
		switch(wParam){
		case IDM_QUIT:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case TRAYMESSAGE:
		if (wParam == 1 && lParam == WM_LBUTTONDOWN) {
			SetForegroundWindow(hwnd);
			GetCursorPos(&pt);
			TrackPopupMenu(hMenu, 0, pt.x, pt.y, 0, hwnd, NULL);
		}
		break;

	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &ni);
		if (dll) {
			uninstall_hook();
			FreeLibrary(dll);
		}
		DestroyMenu(hMenu);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

HWND init_application(HINSTANCE hInst)
{
	HWND hwnd;
	static WNDCLASS wc = {
		0,
		WndProc,
		0,
		0,
		NULL,/*  Instance  */
		NULL,/*  ICON  */
		NULL,/*  Cursor  */
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		W_CLASS
	};
	int (WINAPI *install)(HANDLE, HWND);
	int ret;

	if (FindWindow(W_CLASS, NULL)) {
		return NULL;
	}

	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		W_CLASS
		,""
		,WS_POPUP
		,10
		,10
		,10
		,10
		,NULL
		,NULL
		,hInst
		,NULL);

	if (!hwnd) {
		return NULL;
	}

	dll = LoadLibrary("FMHOOK.DLL");
	if (!dll) {
		MessageBox(NULL, "can't load DLL.", "Error", 0);
		goto CANTLOAD;
	}

	install = GetProcAddress(dll, "InstallHook");
	if (!install) {
		MessageBox(NULL, "can't bind procedure.", "Error", 0);
		goto CANTLOAD;
	}
	ret = install(dll, hwnd);
	if (!ret) {
		MessageBox(NULL, "can't bind procedure.", "Error", 0);
		goto CANTLOAD;
	}

	return hwnd;

CANTLOAD:
	DestroyWindow(hwnd);

	return NULL;
}

void uninstall_hook(void)
{
	void (CALLBACK *uninst)(void);

	uninst = (void (CALLBACK *)(void))GetProcAddress(dll, "UninstallHook");
	if (uninst) {
		uninst();
	}
}
