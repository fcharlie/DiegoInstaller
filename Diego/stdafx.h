// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>

class ErrorMessage {
public:
	ErrorMessage(DWORD errid) :lastError(errid), release_(false)
	{
		if (FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, errid,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPWSTR)&buf, 0, nullptr) == 0) {
			buf = L"Unknown error";
		} else {
			release_ = true;
		}
	}
	~ErrorMessage()
	{
		if (release_) {
			LocalFree(buf);
		}
	}
	const wchar_t *message()const
	{
		return buf;
	}
	DWORD LastError()const { return lastError; }
private:
	DWORD lastError;
	LPWSTR buf;
	bool release_;
	char reserved[sizeof(intptr_t) - sizeof(bool)];
};

struct PrivateContext {
	HWND hWnd;
	std::wstring file;
	std::wstring dir;
};

bool FileDiscoverWindow(
	HWND hParent,
	std::wstring &filename,
	const wchar_t *pszWindowTitle);
bool FolderOpenWindow(
	HWND hParent,
	std::wstring &folder,
	const wchar_t *pszWindowTitle);

INT CALLBACK DiegoInstallUIHandler(
	LPVOID  pvContext,
	UINT    iMessageType,
	LPCWSTR szMessage
	);

extern bool bRequireCancel;

// TODO: reference additional headers your program requires here
