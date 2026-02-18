#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3BaseMeta.h"

#if defined( I3_WINDOWS)

#include "res/resource.h"

static I3LOG_TYPE	s_ErrorType;
static char *		s_pszWhere;
static char *		s_pszMsg;
static char *		s_pszSysError;

extern void *				s_hwndMain;

INT_PTR CALLBACK _i3DefPopupProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM /* lParam */)
{
	HWND hwndCtrl;

	switch( msg)
	{
		case WM_INITDIALOG :
			hwndCtrl = GetDlgItem( hwnd, IDC_ED_WHERE);
			SetWindowText( hwndCtrl, s_pszWhere);

			hwndCtrl = GetDlgItem( hwnd, IDC_ED_MSG);
			SetWindowText( hwndCtrl, s_pszMsg);

			hwndCtrl = GetDlgItem( hwnd, IDC_ED_SYS);
			SetWindowText( hwndCtrl, s_pszSysError);
			return true;

		case WM_COMMAND :
			{
				UINT id = LOWORD( wParam);
				UINT code = HIWORD( wParam);

				if( code == BN_CLICKED)
				{
					switch( id)
					{
						case IDOK :		EndDialog( hwnd, IDOK);
							break;

						case IDCANCEL :	EndDialog( hwnd, IDCANCEL);
							break;
					}
				}
			}
			break;
	}

	return FALSE;
}

I3ERROR_HANDLE DefWindowsLogProc( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * /* pUserData */)
{
	INT_PTR rv;

	s_ErrorType = type;
	s_pszWhere = pszWhere;
	s_pszMsg = pszMsg;

	//  [9/20/2012 dosun.lee]
	// 크래쉬 나서 수정
	if( 0 == ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							GetLastError(),
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
							(LPTSTR) &s_pszSysError,
							0,
							NULL ) )
	{
		return I3ERROR_HANDLE_CONTINUE;
	}

#if defined( I3_WINDOWS)
	#if defined( I3_DLL)
		rv = ::DialogBox( g_hInstBase, MAKEINTRESOURCE( IDD_LOGPOPUP), (HWND) s_hwndMain, _i3DefPopupProc);
	#else
		rv = ::DialogBox( NULL, MAKEINTRESOURCE( IDD_LOGPOPUP), s_hwndMain, _i3DefPopupProc);
	#endif
#endif

	LocalFree( s_pszSysError);

	if( rv == IDCANCEL)
	{
		return I3ERROR_HANDLE_CONTINUE;
	}

	return I3ERROR_HANDLE_BREAK;
}
#endif
