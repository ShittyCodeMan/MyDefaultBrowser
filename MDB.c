#define UNICODE

#include <windows.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/INCREMENTAL:NO")
#pragma comment(linker, "/MERGE:.RDATA=.TEXT")

#define ClassName TEXT("My Default Browser")
#define WindowTitle TEXT("Browser Launcher")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int argc;
LPTSTR* argv;
HWND hlist;


void WinMainCRTStartup()
{
	HINSTANCE hinst;
	MSG msg;
	HWND hwnd;

	WNDCLASSEX wc;

	hinst = GetModuleHandle(NULL);

	if (CreateMutex(NULL, FALSE, TEXT("MDB_MUTEX_ANTI-MULEXE")) && (GetLastError() == ERROR_ALREADY_EXISTS)) {
		hwnd = FindWindow(ClassName, WindowTitle);
		if (IsIconic(hwnd)) {
			ShowWindowAsync(hwnd, SW_RESTORE);
		}
		SetForegroundWindow(hwnd);
		hlist = FindWindowEx(hwnd, NULL, TEXT("LISTBOX"), NULL);
		argv = CommandLineToArgvW(GetCommandLine(), &argc);
		SendMessage(hlist, LB_ADDSTRING, 0, (LPARAM)argv[argc-1]);
		GlobalFree(argv);
		ExitProcess(0);
		return;
	}
	
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinst;
	wc.hIcon         = NULL;
	wc.hIconSm       = NULL;
	wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ClassName;

	if (!RegisterClassEx(&wc)) return;

	hwnd = CreateWindow(ClassName, WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 130
		+ GetSystemMetrics(SM_CYMIN),
		NULL, NULL, hinst, NULL);

	if (!hwnd) return;

	ShowWindow(hwnd, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(msg.wParam);
	return;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR argApp[512], argUrl[512], argCmd[512];
	int i, ItemSel[128];
	
	switch (msg) {
	case WM_DESTROY:
		//PostQuitMessage(0);
		ExitProcess(0);
		break;
	case WM_CREATE:
		argv = CommandLineToArgvW(GetCommandLine(), &argc);
		hlist = CreateWindow(
			TEXT("LISTBOX"), argv[argc-1],
			WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_MULTIPLESEL,
			2, 2, 375, 100,
			hwnd, (HMENU)1,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		SendMessage(hlist, LB_ADDSTRING, 0, (LPARAM)argv[argc-1]);
		GlobalFree(argv);
		SendMessage(hlist, LB_SETSEL, TRUE, 0);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Chrome"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2 + 72 * 0, 104, 70, 22,
			hwnd, (HMENU)2,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("IE"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2 + 72 * 1, 104, 70, 22,
			hwnd, (HMENU)3,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Jane"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2 + 72 * 2, 104, 70, 22,
			hwnd, (HMENU)4,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Jane2"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2 + 72 * 3, 104, 70, 22,
			hwnd, (HMENU)5,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		break;
	case WM_COMMAND:
		if (HIWORD(wp) != BN_CLICKED) {
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		switch(LOWORD(wp)) {
		case (HMENU)2:
			lstrcpyn(argApp, TEXT("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), sizeof(argApp));
			break;
		case (HMENU)3:
			lstrcpyn(argApp, TEXT("C:\\Program Files\\Internet Explorer\\iexplore.exe"), sizeof(argApp));
			break;
		case (HMENU)4:
			lstrcpyn(argApp, TEXT("C:\\Program Files (x86)\\Jane Style\\Jane2ch.exe"), sizeof(argApp));
			break;
		case (HMENU)5:
			lstrcpyn(argApp, TEXT("C:\\Jane Style\\Jane2ch.exe"), sizeof(argApp));
			break;
		default:
			return DefWindowProc(hwnd, msg, wp, lp);
		}
		SecureZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		SecureZeroMemory(&argUrl, sizeof(argUrl));
		
		for (i = SendMessage(hlist, LB_GETSELITEMS, SendMessage(hlist, LB_GETSELCOUNT, 0, 0) - 1, (LPARAM)ItemSel); i >= 0; i--) {
			SendMessage(hlist, LB_GETTEXT, ItemSel[i], (LPARAM)argUrl);
			SendMessage(hlist, LB_DELETESTRING, ItemSel[i], 0);
			lstrcat(lstrcpyn(argCmd, TEXT("-new "), sizeof(argCmd)), argUrl);
			CreateProcess(argApp, argCmd,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
			);
		}
		if (SendMessage(hlist, LB_GETCOUNT, 0, 0) == 0) {
			DestroyWindow(hwnd);
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}
