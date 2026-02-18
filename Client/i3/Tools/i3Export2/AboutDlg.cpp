#include "stdafx.h"

BOOL CALLBACK  _AboutDlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;

	switch( msg) 
	{
		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case IDOK :
						EndDialog( hwnd, id);
						break;
				}
			}
			break;

		default:
			return false;
	}
	return true;
}