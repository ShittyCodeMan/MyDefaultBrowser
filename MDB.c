#define UNICODE

#include <windows.h>
#include <Shlobj.h>
#pragma comment(linker, "/subsystem:windows")
#pragma comment(linker, "/NODEFAULTLIB")
#pragma comment(linker, "/INCREMENTAL:NO")

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int argc;
LPTSTR* argv;
HWND hedit;

void WinMainCRTStartup()
{
	LPCTSTR pClassName = TEXT("My Default Browser");
	HINSTANCE hinst;
	MSG msg;
	HWND hwnd;

	WNDCLASSEX wc;

	hinst = GetModuleHandle(NULL);

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
	wc.lpszClassName = pClassName;

	if (!RegisterClassEx(&wc)) return;

	hwnd = CreateWindow(pClassName, TEXT("Browser Launcher"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 50
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
		GlobalFree(argv);
		//PostQuitMessage(0);
		ExitProcess(0);
		break;
	case WM_CREATE:
		argv = CommandLineToArgvW(GetCommandLine(), &argc);
		hedit = CreateWindow(
			TEXT("EDIT"), argv[argc-1],
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_READONLY,
			2, 2, 375, 22,
			hwnd, (HMENU)1,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Chrome"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			2, 26, 70, 22,
			hwnd, (HMENU)2,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("IE"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			74, 26, 70, 22,
			hwnd, (HMENU)3,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		CreateWindow(
			TEXT("BUTTON"), TEXT("Jane"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_CENTER,
			146, 26, 70, 22,
			hwnd, (HMENU)4,
			((LPCREATESTRUCT)(lp))->hInstance, NULL
		);
		break;
	case WM_COMMAND:
		SecureZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		SecureZeroMemory(&argUrl, sizeof(argUrl));
		GetWindowText(hedit, argUrl, sizeof(argUrl));
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
