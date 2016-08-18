#define UNICODE

#include <windows.h>
#include <Shlobj.h>
#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/INCREMENTAL:NO")

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

	CreateMutex(NULL, FALSE, TEXT("MDB_MUTEX_ANTI-MULEXE"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		hlist = FindWindowEx(FindWindow(ClassName, WindowTitle), NULL, TEXT("LISTBOX"), NULL);
		argv = CommandLineToArgvW(GetCommandLine(), &argc);
		SendMessage(hlist, LB_ADDSTRING, 0, (LPARAM)argv[argc-1]);
		GlobalFree(argv);
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
	return;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR argUrl[512], argCmd[512];
	
	switch (msg) {
	case WM_DESTROY:
		//PostQuitMessage(0);
		ExitProcess(0);
		break;
	case WM_CREATE:
		argv = CommandLineToArgvW(GetCommandLine(), &argc);
		hlist = CreateWindow(
			TEXT("LISTBOX"), argv[argc-1],
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			2, 2, 375, 100,
			hwnd, (HMENU)1,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		SendMessage(hlist, LB_ADDSTRING, 0, (LPARAM)argv[argc-1]);
		GlobalFree(argv);
		SendMessage(hlist, LB_SETCURSEL, 0, 0);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Chrome"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2, 104, 70, 22,
			hwnd, (HMENU)2,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("IE"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			74, 104, 70, 22,
			hwnd, (HMENU)3,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Jane"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			146, 104, 70, 22,
			hwnd, (HMENU)4,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		break;
	case WM_COMMAND:
		SecureZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		SecureZeroMemory(&argUrl, sizeof(argUrl));
		SendMessage(hlist, LB_GETTEXT,
			SendMessage(hlist, LB_GETCURSEL, 0, 0),
			(LPARAM)argUrl
		);
		GetWindowText(hlist, argUrl, sizeof(argUrl));
		lstrcat(lstrcpyn(argCmd, TEXT("-new "), sizeof(argCmd)), argUrl);
		switch(LOWORD(wp)) {
		case (HMENU)2:
			CreateProcess(TEXT("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), argCmd,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
			);
			break;
		case (HMENU)3:
			CreateProcess(TEXT("C:\\Program Files\\Internet Explorer\\iexplore.exe"), argCmd,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
			);
			break;
		case (HMENU)4:
			CreateProcess(TEXT("C:\\Program Files (x86)\\Jane Style\\Jane2ch.exe"), argCmd,
				NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi
			);
			break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}
