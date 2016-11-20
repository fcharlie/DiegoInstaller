// Diego.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Diego.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <CommCtrl.h>
#include <vector>
#include <mutex>
#include <thread>
#include <Msi.h>
#include <PathCch.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DIEGO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIEGO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIEGO));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

#define WINDOWEXSTYLE WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY
#define EDITBOXSTYLE  WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE |WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL
#define PUSHBUTTONSTYLE BS_PUSHBUTTON | BS_TEXT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE
#define PROGRESSRATESTYLE WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_VISIBLE

#define WS_NORESIZEWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_MINIMIZEBOX )

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowExW(0UL, szWindowClass, szTitle, WS_NORESIZEWINDOW,
						   CW_USEDEFAULT, 0, 720, 450, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	::DragAcceptFiles(hWnd, TRUE);

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logFont = { 0 };
	GetObjectW(hFont, sizeof(logFont), &logFont);
	DeleteObject(hFont);
	hFont = NULL;
	logFont.lfHeight = 20;
	logFont.lfWeight = FW_NORMAL;
	wcscpy_s(logFont.lfFaceName, L"Segoe UI");
	hFont = CreateFontIndirectW(&logFont);
	auto LambdaCreateWindow = [&](LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
								  int X, int Y, int nWidth, int nHeight, HMENU hMenu)->HWND{
		auto hw = CreateWindowExW(WINDOWEXSTYLE, lpClassName, lpWindowName,
								  dwStyle, X, Y, nWidth, nHeight, hWnd, hMenu, hInst, nullptr);
		if (hw) {
			::SendMessageW(hw, WM_SETFONT, (WPARAM)hFont, 0);
		}
		return hw;
	};
	auto LambdaCreateWindowEdge = [&](LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
									  int X, int Y, int nWidth, int nHeight, HMENU hMenu)->HWND{
		auto hw = CreateWindowExW(WINDOWEXSTYLE | WS_EX_CLIENTEDGE, lpClassName, lpWindowName,
								  dwStyle, X, Y, nWidth, nHeight, hWnd, hMenu, hInst, nullptr);
		if (hw) {
			::SendMessageW(hw, WM_SETFONT, (WPARAM)hFont, 0);
		}
		return hw;
	};
	HWND hStatic1 = LambdaCreateWindow(WC_STATICW, L"Package\t\xD83D\xDCE6:", 
									  WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 50, 90, 27, nullptr);
	HWND hStatic2 = LambdaCreateWindow(WC_STATICW, L"Folder\t\xD83D\xDCC1:",
									  WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 100, 90, 27, nullptr);
	HWND hStatic3 = LambdaCreateWindow(WC_STATICW, 
									  L"\x263B \x2665 Copyright \x0A9 2016.Force Charlie.All Rights Reserved.",
									  WS_CHILD | WS_VISIBLE | SS_LEFT, 125, 345, 420, 27, nullptr);
	HWND hUriEdit = LambdaCreateWindowEdge(WC_EDITW, L"", EDITBOXSTYLE, 125, 50, 420, 27, HMENU(IDC_PACKAGE_URI_EDIT));
	HWND hDirEdit = LambdaCreateWindowEdge(WC_EDITW, L"", EDITBOXSTYLE, 125, 100, 420, 27, HMENU(IDC_FOLDER_URI_EDIT));
	HWND hUriButton = LambdaCreateWindow(WC_BUTTONW, L"View...", PUSHBUTTONSTYLE, 560, 50, 90, 27, HMENU(IDC_PACKAGE_VIEW_BUTTON));
	HWND hDirButton = LambdaCreateWindow(WC_BUTTONW, L"Folder...", PUSHBUTTONSTYLE, 560, 100, 90, 27, HMENU(IDC_FOLDER_URI_BUTTON));
	HWND hProgress = LambdaCreateWindow(PROGRESS_CLASSW, L"", PROGRESSRATESTYLE, 125, 180, 420, 27, HMENU(IDC_PROCESS_RATE));
	HWND hOK = LambdaCreateWindow(WC_BUTTONW, L"Start", PUSHBUTTONSTYLE, 125, 270, 205, 30, HMENU(IDC_OPTION_BUTTON_OK));
	HWND hCancel = LambdaCreateWindow(WC_BUTTONW, L"Cancel", PUSHBUTTONSTYLE, 340, 270, 205, 30, HMENU(IDC_OPTION_BUTTON_CANCEL));

	{
		int numArgc = 0;
		auto Argv = CommandLineToArgvW(GetCommandLineW(), &numArgc);
		if (Argv) {
			if (numArgc >= 2 && PathFileExistsW(Argv[1])) {
				::SetWindowTextW(hUriEdit, Argv[1]);
			}
			LocalFree(Argv);
		}
	}
	return TRUE;
}
inline bool FolderIsEmpty(const std::wstring &dir)
{
	std::wstring dir_(dir);
	dir_.append(L"\\*.*");
	WIN32_FIND_DATAW fdata;
	auto hFind = FindFirstFileW(dir_.c_str(), &fdata);
	if (hFind == INVALID_HANDLE_VALUE)
		return true;
	BOOL ret = true;
	while (ret) {
		if (wcscmp(fdata.cFileName, L".") != 0
			&& wcscmp(fdata.cFileName, L"..") != 0) {
			FindClose(hFind);
			return false;
		}
		ret = FindNextFileW(hFind, &fdata);
	}
	FindClose(hFind);
	return true;
}

std::mutex g_mtx;

bool InitializeContext(HWND hWnd, const std::wstring &file, const std::wstring &dir)
{
	static PrivateContext context;
	///
	if (!g_mtx.try_lock()) {
		return false;
	}
	context.hWnd = hWnd;
	context.file.assign(file);
	context.dir.assign(dir);
	g_mtx.unlock();
	std::thread([&]{
		std::lock_guard<std::mutex> lock(g_mtx);
		auto filename = PathFindFileNameW(context.file.c_str());
		WCHAR fileName[MAX_PATH];
		wcscpy_s(fileName, filename);
		PathRemoveExtensionW(fileName);
		std::wstring mdir(context.dir);
		if (!FolderIsEmpty(mdir)) {
			mdir.append(L"\\").append(fileName);
		}
		if (!PathFileExistsW(mdir.c_str())) {
			if (!CreateDirectoryW(mdir.c_str(), nullptr)) {
				ErrorMessage error(GetLastError());
				///Error
				MessageBoxW(context.hWnd, L"Error", error.message(),MB_OK|MB_ICONERROR);
				return;
			}
		}
		std::vector<wchar_t> Command(PATHCCH_MAX_CCH);
		swprintf_s(Command.data(), Command.capacity(), L"ACTION=ADMIN TARGETDIR=\"%s\"", mdir.c_str());
		MsiSetInternalUI(INSTALLUILEVEL(INSTALLUILEVEL_NONE | INSTALLUILEVEL_SOURCERESONLY), NULL);
		MsiSetExternalUIW(DiegoInstallUIHandler,
						  INSTALLLOGMODE_PROGRESS | INSTALLLOGMODE_FATALEXIT | INSTALLLOGMODE_ERROR
						  | INSTALLLOGMODE_WARNING | INSTALLLOGMODE_USER | INSTALLLOGMODE_INFO
						  | INSTALLLOGMODE_RESOLVESOURCE | INSTALLLOGMODE_OUTOFDISKSPACE
						  | INSTALLLOGMODE_ACTIONSTART | INSTALLLOGMODE_ACTIONDATA
						  | INSTALLLOGMODE_COMMONDATA | INSTALLLOGMODE_PROGRESS | INSTALLLOGMODE_INITIALIZE
						  | INSTALLLOGMODE_TERMINATE | INSTALLLOGMODE_SHOWDIALOG,
						  &context);
		auto err = MsiInstallProductW(context.file.c_str(), Command.data());
		std::wstring title2(szTitle);
		if (err == ERROR_SUCCESS) {
			title2.append(L" (Completed)");
			SetWindowTextW(context.hWnd,title2.c_str());
		} else {
			title2.append(L" (Failure)");
			SetWindowTextW(context.hWnd,title2.c_str());
			ErrorMessage err(GetLastError());
			MessageBoxW(context.hWnd, L"Failure", err.message(),MB_OK|MB_ICONERROR);
		}
	}).detach();
	return true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
		case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
			case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
			case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
			case IDC_PACKAGE_VIEW_BUTTON:{
				std::wstring pkgfile;
				if (FileDiscoverWindow(hWnd, pkgfile, L"Get Microsoft Installer Package")) {
					::SetWindowTextW(GetDlgItem(hWnd,IDC_PACKAGE_URI_EDIT), pkgfile.c_str());
				}
			}break;
			case IDC_FOLDER_URI_BUTTON:{
				std::wstring folder;
				if (FolderOpenWindow(hWnd, folder, L"")) {
					::SetWindowTextW(::GetDlgItem(hWnd, IDC_FOLDER_URI_EDIT), folder.c_str());
				}
			}break;
			case IDC_OPTION_BUTTON_OK:{
				WCHAR packfile[4096];
				WCHAR folder[4096];
				auto len = ::GetWindowTextW(::GetDlgItem(hWnd, IDC_PACKAGE_URI_EDIT), packfile, 4096);
				if (len == 0) {
					MessageBoxW(hWnd, L"Package Path Empty", L"Please Input Package Path", MB_OK|MB_ICONERROR);
					return S_FALSE;
				}
				auto len2 = ::GetWindowTextW(::GetDlgItem(hWnd, IDC_FOLDER_URI_EDIT), folder, 4096);
				if (len2 == 0) {
					MessageBoxW(hWnd, L"Folder Path Empty", L"Please Input Folder Path", MB_OK | MB_ICONERROR);
					return S_FALSE;
				}
				InitializeContext(hWnd, packfile, folder);
			}break;
			case IDC_OPTION_BUTTON_CANCEL:{
				bRequireCancel = true;
			} break;
			default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_CTLCOLORSTATIC:
		{
			return 0;  
		}
		case WM_PAINT:{
			hdc = BeginPaint(hWnd, &ps);
			HPEN hpenOld = static_cast<HPEN>(SelectObject(hdc, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
			RECT rect;
			GetWindowRect(hWnd,&rect);
			SetDCPenColor(hdc, RGB(244, 191, 117));
			Rectangle(hdc, 20, 10, rect.right - rect.left - 40, 155);
			Rectangle(hdc, 20, 155, rect.right - rect.left - 40, 385);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
		}

		break;
		case WM_DROPFILES:{
			const LPCWSTR PackageSubffix[] = { L".msi", L".msp" };
			HDROP hDrop = (HDROP)wParam;
			UINT nfilecounts = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			WCHAR dropfile_name[MAX_PATH];
			std::vector<std::wstring> filelist;
			for (UINT i = 0; i < nfilecounts; i++) {
				DragQueryFileW(hDrop, i, dropfile_name, MAX_PATH);
				if (PathFindSuffixArrayW(dropfile_name, PackageSubffix, ARRAYSIZE(PackageSubffix))) {
					filelist.push_back(dropfile_name);
				}
				if (!filelist.empty()) {
					::SetWindowTextW(::GetDlgItem(hWnd, IDC_PACKAGE_URI_EDIT), filelist[0].c_str());
				}
			}
			DragFinish(hDrop);
			::InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_DESTROY:
		PostQuitMessage(0);
		break;
		default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG:
		return (INT_PTR)TRUE;

		case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
