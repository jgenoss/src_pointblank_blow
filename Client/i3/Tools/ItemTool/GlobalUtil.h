#pragma once

#include "ItemToolMainFrameWork.h"
#include "ItemToolSubFrameWork.h"
#include "ItemToolViewer.h"
#include "Chara.h"
#include "PropertyEditor/PefDataList.h"
#include "i3Base/string/ext/contain_string_head.h"

extern cItemToolViewer*			g_pViewer;
extern cItemToolMainFrameWork *	g_pMainSCFramework;
extern cItemToolSubFrameWork*	g_pWeaponSCFramework;
extern cItemToolSubFrameWork*	g_pItemSCFramework;
extern CChara *					g_pChara;
extern cPefDataList *			g_pPefList;

extern HWND	g_HwndWeaponProp;
extern HWND	g_HwndDlgTimeTrack;
extern HWND g_HwndWeaponResList;
extern HWND g_HwndDlgWeaponEditor;
extern HWND g_HwndDlgAIDriver;
extern HWND g_HwndDlgAIEnumList;
extern HWND g_HwndDockPropertyWnd;

extern CString g_strRootPath;
extern CString g_strCurrentDir;

//폴더위치 정의
//리턴값을 CString으로 받아야함
#define STR_GUI_WEAPONSHAPE_PATH	g_strRootPath+"\\"+"Gui\\WeaponShape"
#define STR_GUI_ITEM_PATH			g_strRootPath+"\\"+"Gui\\Item"
#define STR_WEAPON_PATH				g_strRootPath+"\\Weapon"

typedef std::map<CString, std::vector<CString> > mStrVecCon;
extern mStrVecCon g_mAIDrive;

typedef std::map<CString, CString > mStrStrCon;
extern mStrStrCon g_mOverlapedEditorFile;


namespace GUTIL
{
	static void NormalizeCmdArg( char *szOut, const char *szIn )
	{
		while( *szIn )
		{
			if( *szIn != '"' )
			{
				*szOut = *szIn;
				szOut++;
			}
			szIn++;
		}
		*szOut = 0;
	}

	static HRESULT SHPathToPidl(LPCTSTR szPath, LPITEMIDLIST* ppidl)
	{
		LPSHELLFOLDER pShellFolder = NULL;
		OLECHAR wszPath[MAX_PATH] = {0};
		ULONG nCharsParsed = 0;

		HRESULT hr = SHGetDesktopFolder(&pShellFolder);

		if (FAILED(hr))
			return FALSE;

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, -1, wszPath, MAX_PATH);

		hr = pShellFolder->ParseDisplayName(NULL,NULL,wszPath,&nCharsParsed,ppidl, NULL);

		pShellFolder->Release();

		return hr;
	}

	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM pData)
	{
		TCHAR szPath[ MAX_PATH ] = { 0, }; 
		switch(uMsg)
		{
		case BFFM_INITIALIZED:      // 폴더 선택 대화상자를 초기화 할 때, 초기 경로 설정 
			::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)pData ); 
			break;
			// BROWSEINFO 구조체의 ulFlags 값에 BIF_STATUSTEXT 가 설정된 경우 호출 
			// 단, BIF_NEWDIALOGSTYLE 가 설정되어 있을 경우 호출되지 않음 
		case BFFM_SELCHANGED:       // 사용자가 폴더를 선택할 경우 대화상자에 선택된 경로 표시 
			::SHGetPathFromIDList( (LPCITEMIDLIST)lParam, szPath ); 
			::SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath ); 
			break; 
		case BFFM_VALIDATEFAILED:   // 에디터 콘트롤에서 폴더 이름을 잘못 입력한 경우 호출 
			::MessageBox( hWnd, _T( "해당 폴더를 찾을 수 없습니다." ), _T( "오류" ),MB_ICONERROR | MB_OK ); 
			break; 
		}
		return 0;
	};

	//루트 위치를 지정한다
	static const void SetWorkingFolder(bool bRoot = true, char* szRootPath = "")
	{
		wchar_t   pszPathname[MAX_PATH];
		BROWSEINFO  bInfo;
		::memset(&bInfo, 0, sizeof(bInfo));

		LPITEMIDLIST pidl = NULL;
		SHPathToPidl(szRootPath, &pidl);

		bInfo.pidlRoot		= pidl;
		bInfo.hwndOwner		= NULL;
		bInfo.lpszTitle		= _T("기본 경로 디렉토리를 선택하세요");
		bInfo.ulFlags		=BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE; 
		bInfo.lpfn			= BrowseCallbackProc;
		//bInfo.lParam		= (LPARAM)szRootPath;

		ITEMIDLIST* pildBrowse = ::SHBrowseForFolder( &bInfo );

		if( NULL != pildBrowse )	
		{
			if( FALSE == ::SHGetPathFromIDListW( pildBrowse, pszPathname ) )
				return;

			g_strRootPath = pszPathname;
			char szTmpPath[MAX_PATH];

			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pszPathname, -1, szTmpPath, MAX_PATH, NULL, NULL);

			if (bRoot)
				AfxGetApp()->WriteProfileString( "Option", "WorkPath", szTmpPath);
		}
	}


	//해당 파일의 이름을 가진 모든 파일을 뽑아낸다.
	static const void GetFileListFromFolder(std::vector<CString>& vDestFileList,
		const CString& strFolder, const CString& strKeyName)
	{
		CString strKey = strKeyName;
		strKey.MakeLower();

		char prevPath[ MAX_PATH ];
		::GetCurrentDirectory( MAX_PATH, prevPath );
		::SetCurrentDirectory( strFolder );

		DWORD dwAtt = ::GetFileAttributes( (LPSTR)(LPCSTR)strFolder );

		if( ((DWORD)-1) == dwAtt )
			return;

		WIN32_FIND_DATA findData;
		HANDLE hf = ::FindFirstFile( _T("*.*"), &findData );

		if( INVALID_HANDLE_VALUE == hf )
			return;

		do
		{
			CString strfileName = findData.cFileName;

			if( strfileName == "." || strfileName == ".." || strfileName == ".svn" )
				continue;

			strfileName.MakeLower();

			if (i3::contain_string_head((LPSTR)(LPCSTR)strfileName, strKey) )
				vDestFileList.push_back(strfileName);

		} while( TRUE == ::FindNextFile( hf, &findData ) );

		::FindClose( hf );
		::SetCurrentDirectory( prevPath );
	}

	static CString InsertOverlapedFile(const CString& folderName, const CString& fileName)
	{
		CString strTempFileName = folderName;
		strTempFileName.MakeLower();

		g_mOverlapedEditorFile[strTempFileName] = fileName;
		return strTempFileName;
	}

	static CString GetOverlapedFile(const CString& fileName)
	{
		return g_mOverlapedEditorFile[fileName];
	}
}
