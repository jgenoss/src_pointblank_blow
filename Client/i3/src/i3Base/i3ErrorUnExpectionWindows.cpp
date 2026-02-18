// 20070419 김현우

#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3System.h"
#include "i3BaseMeta.h"

#include "i3SMTPMail.h"
#include "i3String.h"

#if defined( I3_WINDOWS )

#include "res/resource.h"

static char *		s_pszEmailContentsBuffer;

INT_PTR CALLBACK _i3DefPopupExProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM /* lParam */)
{
	char szdmpFile[64] = { "i3BugReport.i3b" };
	switch( msg)
	{	
		case WM_INITDIALOG:
			SetFocus( hwnd );
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			break;

		case WM_COMMAND :
			{
				UINT id = LOWORD( wParam);
				UINT code = HIWORD( wParam);

				if( code == BN_CLICKED)
				{
					switch( id )
					{
						case IDC_B_SEND :								
							i3System::SendMailBugReport( s_pszEmailContentsBuffer, szdmpFile );
							DeleteFile( szdmpFile );
							MessageBox( hwnd, "메일이 전송되었습니다\r\n감사합니다", "완료", MB_OK );
							EndDialog( hwnd, IDOK);
							break;
						case IDOK :
							DeleteFile( szdmpFile );
							EndDialog( hwnd, IDOK);
							break;
					}
				}
			}
			break;
			/*
		case WM_LBUTTONUP:
			i3System::SendMailBugReport( s_pszEmailContentsBuffer );
			// 메일 전송이 제대로 되었는지 확인하기 위해 삽입
#if !defined( i3DEBUG )
			DeleteFile( "i3BugReport.i3b" );
#endif
			MessageBox( hwnd, "메일이 전송되었습니다\r\n감사합니다", "완료", MB_OK );
			// 메일 전송이 제대로 되었는지 확인하기 위해 삽입

			EndDialog( hwnd, IDOK);
			break;
			*/
	}

	return FALSE;
}

I3ERROR_HANDLE DefWindowsExpectionProc( char * pszBuffer )
{
	INT_PTR rv;

	s_pszEmailContentsBuffer = pszBuffer;
	
#if defined( I3_WINDOWS)
	#if defined( I3_DLL)
		rv = ::DialogBox( g_hInstBase, MAKEINTRESOURCE( IDD_UNEXPECTION), NULL, _i3DefPopupExProc);
	#else
		rv = ::DialogBox( NULL, MAKEINTRESOURCE( IDD_UNEXPECTION), NULL, _i3DefPopupExProc);
	#endif
#endif

	if( rv == IDCANCEL)
	{
		return I3ERROR_HANDLE_CONTINUE;
	}

	return I3ERROR_HANDLE_BREAK;
}
#endif
