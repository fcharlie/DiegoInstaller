#include "stdafx.h"
#include <string>
#include <Uxtheme.h>
#include <ShlObj.h>
#include <tchar.h>
#include <strsafe.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atlsimpstr.h>
#pragma warning(disable:4091)
#include <Shlwapi.h>


using namespace ATL;

typedef COMDLG_FILTERSPEC FilterSpec;

const FilterSpec filterSpec[] =
{
	{ L"Windows Installer Package (*.msi;*.msp)", L"*.msi;*.msp" },
	{ L"All Files (*.*)", L"*.*" }
};

void ReportErrorMessage(LPCWSTR pszFunction, HRESULT hr)
{
	wchar_t szBuffer[65535] = { 0 };
	if (SUCCEEDED(StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), L"Call: %s Failed w/hr 0x%08lx ,Please Checker Error Code!", pszFunction, hr))) {
		int nB = 0;
		TaskDialog(nullptr, GetModuleHandle(nullptr),
				   L"Failed information",
				   L"Call Function Failed:",
				   szBuffer,
				   TDCBF_OK_BUTTON,
				   TD_ERROR_ICON,
				   &nB);
	}
}
bool FileDiscoverWindow(
	HWND hParent,
	std::wstring &filename,
	const wchar_t *pszWindowTitle)
{
	HRESULT hr;
	bool br = false;
	CComPtr<IFileOpenDialog> pWindow;
	// Create the file-open dialog COM object.
	hr = pWindow.CoCreateInstance(__uuidof(FileOpenDialog));
	if (FAILED(hr)) {
		ReportErrorMessage(L"FileOpenWindowProvider", hr);
		return false;
	}

	// Set the dialog's caption text and the available file types.
	// NOTE: Error handling omitted here for clarity.
	hr = pWindow->SetFileTypes(sizeof(filterSpec) / sizeof(filterSpec[0]), filterSpec);
	hr = pWindow->SetFileTypeIndex(1);
	pWindow->SetTitle(pszWindowTitle ? pszWindowTitle : L"Open File Provider");
	hr = pWindow->Show(hParent);
	if (SUCCEEDED(hr)) {
		CComPtr<IShellItem> pItem;
		hr = pWindow->GetResult(&pItem);
		if (SUCCEEDED(hr)) {
			PWSTR pwsz = NULL;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
			if (SUCCEEDED(hr)) {
				filename = pwsz;
				br = true;
				CoTaskMemFree(pwsz);
			}
		}
	}
	return br;
}

bool FolderOpenWindow(
	HWND hParent,
	std::wstring &folder,
	const wchar_t *pszWindowTitle)
{
	IFileDialog *pfd;
	HRESULT hr = S_OK;
	bool bRet = false;
	if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
		DWORD dwOptions;
		if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
			pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
		}
		pfd->SetTitle(pszWindowTitle ? pszWindowTitle : L"Open Folder");
		if (SUCCEEDED(pfd->Show(hParent))) {
			IShellItem *psi;
			if (SUCCEEDED(pfd->GetResult(&psi))) {
				PWSTR pwsz = NULL;
				hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
				if (SUCCEEDED(hr)) {
					folder = pwsz;
					bRet = true;
					CoTaskMemFree(pwsz);
				}
				psi->Release();
			}
		}
		pfd->Release();
	}
	return bRet;
}