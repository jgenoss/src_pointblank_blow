#include "i3CommonType.h"
#include "i3DefExternRefResolveWin32.h"
#include "i3Memory.h"
#include "i3String.h"
#include "i3System.h"

#if defined( I3_WINDOWS)
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

static char s_szPrevSearchPath[MAX_PATH] = { 0, };

typedef struct _tagWin32FindInfo
{
	char	*		m_pszWorkBase;
	char *			m_pszName;
	char			m_szPath[ MAX_PATH];
	INT32			m_FindCount;
} WIN32_FIND_INFO;

static BOOL _FindProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	WIN32_FIND_INFO * pInfo = (WIN32_FIND_INFO *) pUserData;

	if( i3String::Compare( pInfo->m_pszName, pFileInfo->cFileName) == 0)
	{
		pInfo->m_FindCount ++;
		i3String::Format( pInfo->m_szPath, sizeof(pInfo->m_szPath), "%s/%s", pszPath, pFileInfo->cFileName);
		I3TRACE( "FOUND : %s\n", pInfo->m_szPath);
	}

	return TRUE;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	CHAR lpszDir[MAX_PATH];
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);    //기본 폴더가 선택되도록 한다.
		break;
	case BFFM_SELCHANGED:
		if(SHGetPathFromIDList((LPITEMIDLIST)lParam, lpszDir))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)lpszDir); //현재 선택한 폴더를 상태창에 나타낸다.
		}
		break;
	}
	return 0;
}

static BOOL _FolderSelect( const char * pszInitDir, char * pszDir, const char * pszName)
{
	LPMALLOC pMalloc = NULL;
	HRESULT Rv;
	BOOL Result = FALSE;
	BROWSEINFO BrowseInfo;
    LPITEMIDLIST pIdl = NULL, pRootIdl = NULL;
	char szDirName[ MAX_PATH];
	char szTitle[ MAX_PATH << 1];

	Rv = SHGetMalloc( &pMalloc);
	if( FAILED( Rv)) 
	{
		MessageBox( NULL, "Not enough memory resource for shell command.", "Error", MB_OK);
		return FALSE;
	}

    memset( &BrowseInfo, 0, sizeof(BROWSEINFO));

    BrowseInfo.hwndOwner = NULL;
	BrowseInfo.pszDisplayName = szDirName;
    BrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
    BrowseInfo.lpfn = BrowseCallbackProc;
	BrowseInfo.lParam = (LPARAM)pszInitDir;
	BrowseInfo.pidlRoot = pRootIdl;

	sprintf( szTitle, "%s 파일의 위치를 선택해주세요.\r\n취소를 하면 파일을 직접 선택할 수 있습니다.", pszName);
	BrowseInfo.lpszTitle = szTitle;

	// Process...(blocked)
    pIdl = SHBrowseForFolder( &BrowseInfo);
    if( pIdl == NULL) 
	{
        goto ExitPart;
    }

    if( SHGetPathFromIDList( pIdl, pszDir) == FALSE) 
	{
    	MessageBox( NULL, "Invalid PIDL value.", "Error", MB_OK);
		goto ExitPart;
    }

	Result = TRUE;

ExitPart:
	if( pIdl != NULL)
		pMalloc->Free( pIdl);

	if( pMalloc != NULL) 
	{
		pMalloc->Release();
		pMalloc = NULL;
	}

	return Result;
}

I3_EXPORT_BASE
bool _DefExternResolveProcWin32( const char * pszPath, char * pszNewPath, INT32 lenBuff, const char * pszWorkPath, bool bSearch)
{
	OPENFILENAME ofn;
	char szExt[128], szFilter[256];
	char szName[ MAX_PATH];
	INT32 i;
	bool bFolderSelect = true;

	ofn.lpstrTitle		= "Referenced external file";

	if( bSearch)
	{
		WIN32_FIND_INFO info;

		// 지정된 Directory를 검색한다.
		if( s_szPrevSearchPath[0] != 0)
		{
			bFolderSelect = false;
		}

		do
		{
			if( bFolderSelect)
			{
				char szTemp[ MAX_PATH];

				i3String::SplitFileName( pszPath, szTemp, TRUE);
				if( _FolderSelect( pszWorkPath, s_szPrevSearchPath, szTemp) == FALSE)
				{
					bFolderSelect = false;
					bSearch = false;
				}
			}

			if( bSearch)
			{
				i3String::SplitFileName( pszPath, szName, TRUE);

				info.m_pszWorkBase = s_szPrevSearchPath;
				info.m_pszName = szName;
				info.m_FindCount = 0;
				info.m_szPath[0] = 0;

				i3System::LookForFiles( s_szPrevSearchPath, szName, (FINDFILEPROC) _FindProc, &info);

				if( info.m_FindCount == 1)
				{
					i3String::NCopy( pszNewPath, info.m_szPath, lenBuff);
					return true;
				}
				else if( info.m_FindCount > 1)
				{
					ofn.lpstrTitle		= "Referenced external file (중복된 파일로 인해 자동 검색 실패)";
					bSearch = false;
				}
				else
				{
					bFolderSelect = true;
				}
			}
		} while( bFolderSelect || bSearch);
	}

	i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);

	i3String::ToUpper( szExt);

	// Filter String
	{
		sprintf( szFilter, "%s File:*.%s:All files:*.*:", szExt, szExt);

		for( i = 0; szFilter[i] != 0; i++)
		{
			if( szFilter[i] == ':')
				szFilter[i] = 0;
		}
	}

	i3String::SplitFileName( pszPath, pszNewPath, TRUE);
	//i3String::NCopy( pszNewPath, pszPath, lenBuff);

	i3mem::FillZero( &ofn, sizeof(ofn));

	ofn.lStructSize		= sizeof(ofn);
	ofn.hwndOwner		= NULL;
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= pszNewPath;
	ofn.nMaxFile		= lenBuff;
	ofn.Flags			= OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST;

	BOOL rv = GetOpenFileName( &ofn);

	if( rv == TRUE)
	{
		// 선택을 했다면 해당 Folder를 재검색 대상으로 기록해둔다.
		i3String::SplitPath( (const char *) pszNewPath, s_szPrevSearchPath);
	}

	return rv == TRUE;
}

#endif
