#include "stdafx.h"
#include "Resource.h"
#include <Msi.h>
#include <CommCtrl.h>

int FGetInteger(WCHAR*& rpch)
{
	WCHAR* pchPrev = rpch;
	while (*rpch && *rpch != L' ')
		rpch++;
	*rpch = L'\0';
	int i = _wtoi(pchPrev);
	return i;
}

BOOL ParseProgressString(LPWSTR sz, int* pnFields)
{
	WCHAR* pch = sz;
	if (0 == *pch)
		return FALSE; // no msg

	while (*pch != 0) {
		WCHAR chField = *pch++;
		pch++; // for ':'
		pch++; // for sp
		switch (chField) {
			case L'1': // field 1
			{
				// progress message type
				if (0 == isdigit(*pch))
					return FALSE; // blank record
				pnFields[0] = *pch++ - L'0';
				break;
			}
			case L'2': // field 2
			{
				pnFields[1] = FGetInteger(pch);
				if (pnFields[0] == 2 || pnFields[0] == 3)
					return TRUE; // done processing
				break;
			}
			case '3': // field 3
			{
				pnFields[2] = FGetInteger(pch);
				if (pnFields[0] == 1)
					return TRUE; // done processing
				break;
			}
			case '4': // field 4
			{
				pnFields[3] = FGetInteger(pch);
				return TRUE; // done processing
			}
			default: // unknown field
			{
				return FALSE;
			}
		}
		pch++; // for space (' ') between fields
	}

	return TRUE;
}

bool bRequireCancel = false;
bool bFirstTime = true;
bool bEnableActionData = false;
bool mForwardProgress{ false };
bool mScriptInProgress{ false };
int mProgressTotal{ 0 };
int mProgress{ 0 };
int iField[4];
int iCurPos;

INT CALLBACK DiegoInstallUIHandler(
	LPVOID  pvContext,
	UINT    iMessageType,
	LPCWSTR szMessage
	)
{
	auto context = reinterpret_cast<PrivateContext *>(pvContext);
	if (bFirstTime) {
		UINT r1 = MsiSetInternalUI(INSTALLUILEVEL_BASIC, NULL);
		bFirstTime = false;
	}
	if (bRequireCancel)
		return IDCANCEL;
	if (!szMessage)
		return 0;
	INSTALLMESSAGE mt;
	UINT uiFlags;

	mt = (INSTALLMESSAGE)(0xFF000000 & (UINT)iMessageType);
	uiFlags = 0x00FFFFFF & iMessageType;
	switch (mt) {
		case INSTALLMESSAGE_FATALEXIT:
		MessageBoxW(context->hWnd, L"Install Failed !", szMessage, MB_OK|MB_ICONERROR);
		return 1;
		case INSTALLMESSAGE_ERROR:
		{
			/* Get error message here and display it*/
			// language and caption can be obtained from common data msg
			MessageBeep(uiFlags & MB_ICONMASK);
			return ::MessageBoxEx(context->hWnd, szMessage, TEXT("Error"), uiFlags, LANG_NEUTRAL);
		}
		case INSTALLMESSAGE_WARNING:
		MessageBoxW(context->hWnd, L"Warning", szMessage, MB_OK | MB_ICONWARNING);
		return 0;
		case INSTALLMESSAGE_USER:
		/* Get user message here */
		// parse uiFlags to get Message Box Styles Flag and return appopriate value, IDOK, IDYES, etc.
		return IDOK;

		case INSTALLMESSAGE_INFO:
		return IDOK;

		case INSTALLMESSAGE_FILESINUSE:
		/* Display FilesInUse dialog */
		// parse the message text to provide the names of the 
		// applications that the user can close so that the 
		// files are no longer in use.
		return 0;

		case INSTALLMESSAGE_RESOLVESOURCE:
		/* ALWAYS return 0 for ResolveSource */
		return 0;

		case INSTALLMESSAGE_OUTOFDISKSPACE:
		/* Get user message here */
		return IDOK;

		case INSTALLMESSAGE_ACTIONSTART:
		/* New action started, any action data is sent by this new action */
		bEnableActionData = FALSE;
		return IDOK;
		break;
		case INSTALLMESSAGE_ACTIONDATA:
		if (0 == mProgressTotal)
			return IDOK;
		if (bEnableActionData) {
			::SendMessageW(GetDlgItem(context->hWnd,IDC_PROCESS_RATE), PBM_STEPIT, 0, 0);
		}
		return IDOK;
		break;
		case INSTALLMESSAGE_PROGRESS:{
			if (ParseProgressString(const_cast<LPWSTR>(szMessage), iField)) {
				switch (iField[0]) {
					case 0:
					{
						mProgressTotal = iField[1];
						if (iField[2] == 0)
							mForwardProgress = true;
						else
							mForwardProgress = false;
						mProgress = mForwardProgress ? 0 : mProgressTotal;
						::SendMessageW(GetDlgItem(context->hWnd, IDC_PROCESS_RATE), 
									   PBM_SETRANGE32, 0, mProgressTotal);
						::SendMessageW(GetDlgItem(context->hWnd, IDC_PROCESS_RATE), 
									   PBM_SETPOS, mScriptInProgress ? mProgressTotal : mProgress, 0);
						iCurPos = 0;
						mScriptInProgress = (iField[3] == 1) ? TRUE : FALSE;
					}break;
					case 1:{
						if (iField[2]) {
							::SendMessageW(GetDlgItem(context->hWnd, IDC_PROCESS_RATE), 
										   PBM_SETSTEP, mForwardProgress ? iField[1] : -1 * iField[1], 0);
							bEnableActionData = TRUE;
						} else {
							bEnableActionData = FALSE;
						}
					}break;
					case 2:
					{
						if (0 == mProgressTotal)
							break;

						iCurPos += iField[1];
						::SendMessageW(GetDlgItem(context->hWnd, IDC_PROCESS_RATE), 
									   PBM_SETPOS, mForwardProgress ? iCurPos : -1 * iCurPos, 0);

					}
					break;
					default:
					break;
				}
			}
		}
									 return IDOK;
		case INSTALLMESSAGE_INITIALIZE:
		return IDOK;

		// Sent after UI termination, no string data
		case INSTALLMESSAGE_TERMINATE:
		return IDOK;

		//Sent prior to display of authored dialog or wizard
		case INSTALLMESSAGE_SHOWDIALOG:
		return IDOK;
		default:
		break;
	}
	return 0;
}