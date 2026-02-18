// MultiExportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MultiExportDialog.h"
#include "resource.h"
#include "i3Export.h"
#include "OptionInfo.h"
#include "OptionDialog.h"
#include "i3TDKFolderSelectDialog.h"

#include "i3Base/string/ext/parse_multiselect_filename.h"
#include "i3Base/string/ext/extract_filename.h"

static i3::vector<i3::rc_string> s_FileList;
static char					s_szPrefix[128];

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

static void _SetOptionToDialog( HWND hwnd, i3tool::exporter::pack::OPTION_INFO * pOption)
{
	s_FileList.clear();

	// Selected File count
	{
		_SetTextCtrl( hwnd, IDC_FILECOUNT, "0 files");
	}

	// Last Export Path
	_SetTextCtrl( hwnd, IDC_ED_EXPORT_PATH, pOption->m_szLastPath);
}

static void _GetOptionFromDialog( HWND hwnd, i3tool::exporter::pack::OPTION_INFO * pOption)
{
	// Export Path
	_GetTextCtrl( hwnd, IDC_ED_EXPORT_PATH, pOption->m_szLastPath, sizeof(pOption->m_szLastPath));

	// Prefix
	_GetTextCtrl( hwnd, IDC_ED_PREFIX, s_szPrefix, sizeof(s_szPrefix));
}

static char s_szFiles[65536];

static void _OnSelectFiles( HWND hwnd)
{
	OPENFILENAME ofn;

	memset( &ofn, 0, sizeof(ofn));

	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= hwnd;
	ofn.hInstance			= hInstance;
	ofn.lpstrFilter			= "MAX Files (*.MAX)\x0*.MAX\x0";
	ofn.lpstrFile			= s_szFiles;
	ofn.nMaxFile			= sizeof(s_szFiles) - 1;
	ofn.lpstrTitle			= "Select files to export";
	ofn.Flags				= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if( GetOpenFileName( &ofn))
	{
		i3::vector<i3::rc_string> tempList;
		char szFull[MAX_PATH];

//		i3String::ParseMultiSelectFileName( s_szFiles, tempList);
		i3::parse_multiselect_filename(s_szFiles, tempList);

		if( tempList.size() == 1)
		{
			s_FileList.push_back( tempList[0]);
		}
		else
		{
			for(size_t i = 1; i < tempList.size(); i++)
			{
				sprintf( szFull, "%s\\%s", tempList[0].c_str(), tempList[i].c_str());

				s_FileList.push_back( szFull);
			}
		}

		char conv[256];
		sprintf( conv, "%d files", (INT32)s_FileList.size());

		_SetTextCtrl( hwnd, IDC_FILECOUNT, conv);
	}
}

static void _OnOption( HWND hwnd)
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_OPTION), hwnd, _OptionDialogProc, (LPARAM) g_pExport);
}

static void _OnExportPath( HWND hwnd)
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( hwnd, "Export할 Folder를 선택하세요"))
	{
		_SetTextCtrl( hwnd, IDC_ED_EXPORT_PATH, dlg.GetSelectedFolderPath());
	}
}

static void _OnExport( HWND hwnd)
{

	Interface * ip = GetCOREInterface();
	char szName[MAX_PATH];
	i3tool::exporter::pack::OPTION_INFO * pOption = g_pExport->getOption();

	g_pExport->setUpdateOptionEnable( false);
	g_pExport->setResetAnimExcludeListEnable( false);

	for(size_t i = 0; i < s_FileList.size(); i++)
	{
		I3TRACE( "%s\n", s_FileList[i].c_str());

		ip->FileReset( TRUE);

		if( ip->LoadFromFile( s_FileList[i].c_str(), TRUE) != 0)
		{
			i3::extract_filetitle( s_FileList[i], szName);

			sprintf( pOption->m_szLastName, "%s%s.I3S", s_szPrefix, szName);

			g_pExport->DoExport( pOption->m_szLastPath, pOption->m_szLastName);
		}
	}

	g_pExport->setResetAnimExcludeListEnable( true);
	g_pExport->setUpdateOptionEnable( true);
}

BOOL CALLBACK  _MultiExportDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;
	i3tool::exporter::pack::OPTION_INFO * pOption;

	switch( msg) 
	{
		case WM_INITDIALOG:
			{
				CenterWindow( hwnd, GetParent( hwnd));

				pOption = g_pExport->getOption();
				_SetOptionToDialog( hwnd, pOption);
			}
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			pOption = g_pExport->getOption();

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case IDC_BTN_SELECT :	_OnSelectFiles( hwnd);	break;
					case IDC_BTN_OPTION :	_OnOption( hwnd);	break;
					case IDC_BTN_EXPORT_PATH :	_OnExportPath( hwnd);	break;

					case IDC_BTN_GO :
						_GetOptionFromDialog( hwnd, pOption);

						_OnExport( hwnd);
						break;

					case IDCANCEL :
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
