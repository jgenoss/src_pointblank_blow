// MultiExportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MultiExportDialog.h"
#include "resource.h"
#include "i3Export.h"
#include "OptionInfo.h"
#include "OptionDialog.h"
#include "i3TDKFolderSelectDialog.h"

static COLOR				s_Ambient	= COLOR( 128, 128, 128, 255);
static COLOR				s_Diffuse	= COLOR( 255, 255, 255, 255);
static COLOR				s_Specular	= COLOR( 255, 255, 255, 255);
static INT32				s_SpecularLv = 64;
static INT32				s_Glossiness = 50;
static HBRUSH				s_hbrAmbient = NULL;
static HBRUSH				s_hbrDiffuse = NULL;
static HBRUSH				s_hbrSpecular = NULL;
static HWND					s_hwndDlg = NULL;

inline void _SetTextCtrl( HWND hwndDialog, UINT id, char * pszText)
{
	HWND hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		::SetWindowText( hwndCtrl, pszText);
	}
}

inline void _GetTextCtrl( HWND hwndDialog, UINT id, char * pszText, INT32 len)
{
	HWND hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		::GetWindowText( hwndCtrl, pszText, len);
	}
}

static void Rec_AdjustMtl( Mtl * pMtl)
{
	Color c;
	INT32 i;

	c.r = (REAL32)s_Ambient.r / 255.0f;
	c.g = (REAL32)s_Ambient.g / 255.0f;
	c.b = (REAL32)s_Ambient.b / 255.0f;
	pMtl->SetAmbient( c, 0);

	c.r = (REAL32)s_Diffuse.r / 255.0f;
	c.g = (REAL32)s_Diffuse.g / 255.0f;
	c.b = (REAL32)s_Diffuse.b / 255.0f;
	pMtl->SetDiffuse( c, 0);

	c.r = (REAL32)s_Specular.r / 255.0f;
	c.g = (REAL32)s_Specular.g / 255.0f;
	c.b = (REAL32)s_Specular.b / 255.0f;
	pMtl->SetSpecular( c, 0);

	pMtl->SetShininess( (REAL32) s_Glossiness * 0.01f, 0);

	if (pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0))
	{
		StdMat* std = (StdMat *)pMtl;

		std->SetShinStr( (REAL32) s_SpecularLv * 0.01f, 0);
	}

	for( i = 0; i < pMtl->NumSubMtls(); i++)
	{
		Mtl * pSubMtl = pMtl->GetSubMtl( i);

		if( pSubMtl != NULL)
		{
			Rec_AdjustMtl( pSubMtl);
		}
	}

}

static bool _AdjustAllMtlProc( INT32 Level, INode * pNode, void * pUserData)
{
	Mtl * pMtl = pNode->GetMtl();

	if( pMtl != NULL)
	{
		Rec_AdjustMtl( pMtl);
	}

	return true;
}

static void _OnAdjust( HWND hwnd)
{
	// Specular Level의 Update
	{
		char conv[32];

		_GetTextCtrl( s_hwndDlg, IDC_ED_S_LEVEL, conv, sizeof(conv) - 1);
		s_SpecularLv = atoi( conv);

		_GetTextCtrl( s_hwndDlg, IDC_ED_GLOSS, conv, sizeof(conv) - 1);
		s_Glossiness = atoi( conv);
	}

	// Traverse
	{
		i3ExportTracer Tracer;

		Tracer.Trace( GetCOREInterface()->GetRootNode(), _AdjustAllMtlProc, NULL);
	}
}

static void _UpdateAmbient(void)
{
	if( s_hbrAmbient != NULL)
	{
		::DeleteObject( s_hbrAmbient);
	}

	s_hbrAmbient = CreateSolidBrush( RGB( s_Ambient.r, s_Ambient.g, s_Ambient.b));

	HWND hwndCtrl;
	char conv[32];

	sprintf( conv, "%d", s_Ambient.r);
	_SetTextCtrl( s_hwndDlg, IDC_ED_A_R, conv);

	sprintf( conv, "%d", s_Ambient.g);
	_SetTextCtrl( s_hwndDlg, IDC_ED_A_G, conv);

	sprintf( conv, "%d", s_Ambient.b);
	_SetTextCtrl( s_hwndDlg, IDC_ED_A_B, conv);

	hwndCtrl = GetDlgItem( s_hwndDlg, IDC_ST_A);
	if( hwndCtrl != NULL)
	{
		InvalidateRect( hwndCtrl, NULL, TRUE);
	}
}

static void _UpdateDiffuse(void)
{
	if( s_hbrDiffuse != NULL)
	{
		::DeleteObject( s_hbrDiffuse);
	}

	s_hbrDiffuse = CreateSolidBrush( RGB( s_Diffuse.r, s_Diffuse.g, s_Diffuse.b));

	HWND hwndCtrl;
	char conv[32];

	sprintf( conv, "%d", s_Diffuse.r);
	_SetTextCtrl( s_hwndDlg, IDC_ED_D_R, conv);

	sprintf( conv, "%d", s_Diffuse.g);
	_SetTextCtrl( s_hwndDlg, IDC_ED_D_G, conv);

	sprintf( conv, "%d", s_Diffuse.b);
	_SetTextCtrl( s_hwndDlg, IDC_ED_D_B, conv);

	hwndCtrl = GetDlgItem( s_hwndDlg, IDC_ST_D);
	if( hwndCtrl != NULL)
	{
		InvalidateRect( hwndCtrl, NULL, TRUE);
	}
}

static void _UpdateSpecular(void)
{
	if( s_hbrSpecular != NULL)
	{
		::DeleteObject( s_hbrSpecular);
	}

	s_hbrSpecular = CreateSolidBrush( RGB( s_Specular.r, s_Specular.g, s_Specular.b));

	HWND hwndCtrl;
	char conv[32];

	sprintf( conv, "%d", s_Specular.r);
	_SetTextCtrl( s_hwndDlg, IDC_ED_S_R, conv);

	sprintf( conv, "%d", s_Specular.g);
	_SetTextCtrl( s_hwndDlg, IDC_ED_S_G, conv);

	sprintf( conv, "%d", s_Specular.b);
	_SetTextCtrl( s_hwndDlg, IDC_ED_S_B, conv);

	hwndCtrl = GetDlgItem( s_hwndDlg, IDC_ST_S);
	if( hwndCtrl != NULL)
	{
		InvalidateRect( hwndCtrl, NULL, TRUE);
	}
}

static void _UpdateSpecularLevel(void)
{
	char conv[32];

	sprintf( conv, "%d", s_SpecularLv);
	_SetTextCtrl( s_hwndDlg, IDC_ED_S_LEVEL, conv);

	sprintf( conv, "%d", s_Glossiness);
	_SetTextCtrl( s_hwndDlg, IDC_ED_GLOSS, conv);
}

static bool _SelectColor( COLOR * pColor)
{
	BOOL Rv;
	COLORREF col;

	col = RGB( pColor->r, pColor->g, pColor->b);

	Rv = HSVDlg_Do( s_hwndDlg, &col, NULL, NULL, "색상 선택"); 

	if( Rv == TRUE)
	{
		pColor->r = GetRValue( col);
		pColor->g = GetGValue( col);
		pColor->b = GetBValue( col);
		return true;
	}

	return false;
}

static LRESULT CALLBACK s_AmbientProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{
		case WM_PAINT :
			{
				PAINTSTRUCT ps;
				HDC dc;
				RECT rt;

				dc = BeginPaint( hwnd, &ps);

				GetClientRect( hwnd, &rt);
				FillRect( dc, &rt, s_hbrAmbient);

				EndPaint( hwnd, &ps);
			}
			return 0;

		case WM_LBUTTONDBLCLK :
			if( _SelectColor( &s_Ambient))
			{
				_UpdateAmbient();
			}
			break;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK s_DiffuseProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{
		case WM_PAINT :
			{
				PAINTSTRUCT ps;
				HDC dc;
				RECT rt;

				dc = BeginPaint( hwnd, &ps);

				GetClientRect( hwnd, &rt);
				FillRect( dc, &rt, s_hbrDiffuse);

				EndPaint( hwnd, &ps);
			}
			return 0;

		case WM_LBUTTONDBLCLK :
			if( _SelectColor( &s_Diffuse))
			{
				_UpdateDiffuse();
			}
			break;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK s_SpecularProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{
		case WM_PAINT :
			{
				PAINTSTRUCT ps;
				HDC dc;
				RECT rt;

				dc = BeginPaint( hwnd, &ps);

				GetClientRect( hwnd, &rt);
				FillRect( dc, &rt, s_hbrSpecular);

				EndPaint( hwnd, &ps);
			}
			return 0;

		case WM_LBUTTONDBLCLK :
			if( _SelectColor( &s_Specular))
			{
				_UpdateSpecular();
			}
			break;
	}

	return DefWindowProc( hwnd, msg, wParam, lParam);
}


BOOL CALLBACK  _AdjustAllMaterialDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;

	switch( msg) 
	{
		case WM_INITDIALOG:
			{
				s_hwndDlg = hwnd;
				CenterWindow( hwnd, GetParent( hwnd));

				// Ambient
				{
					HWND hwndCtrl = GetDlgItem( hwnd, IDC_ST_A);

					SetWindowLong( hwndCtrl, GWL_WNDPROC, (LONG) s_AmbientProc);
				}
				_UpdateAmbient();

				// Diffuse
				{
					HWND hwndCtrl = GetDlgItem( hwnd, IDC_ST_D);

					SetWindowLong( hwndCtrl, GWL_WNDPROC, (LONG) s_DiffuseProc);
				}
				_UpdateDiffuse();

				// Specular
				{
					HWND hwndCtrl = GetDlgItem( hwnd, IDC_ST_S);

					SetWindowLong( hwndCtrl, GWL_WNDPROC, (LONG) s_SpecularProc);
				}
				_UpdateSpecular();

				// Specular Level
				_UpdateSpecularLevel();
			}
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case IDOK :
						_OnAdjust( hwnd);

					case IDCANCEL :
						EndDialog( hwnd, id);
						break;
				}
			}
			break;

		case WM_DESTROY :
			I3TRACE( "Destroyed\n");
			break;

		default:
			return false;
	}
	return true;
}
