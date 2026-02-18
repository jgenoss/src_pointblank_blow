#include "stdafx.h"

#if defined( I3_WINDOWS)
#include <shlobj.h>
#include "i3TDKFolderSelectDialog.h"

// 콜백함수
int CALLBACK i3TDKFolderSelectDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR lpszDir[MAX_PATH];
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


i3TDKFolderSelectDialog::~i3TDKFolderSelectDialog(void)
{
	if( pMalloc != nullptr)
		Release();
}

BOOL i3TDKFolderSelectDialog::Init(void)
{
	HRESULT Rv;

	if( pMalloc == nullptr) {
		Rv = SHGetMalloc( &pMalloc);
		if( FAILED( Rv)) {
			MessageBox( nullptr, _T("Not enough memory resource for shell command."), _T("Error"), MB_OK);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL i3TDKFolderSelectDialog::Release(void)
{
	if( pMalloc != nullptr) {
		pMalloc->Release();
		pMalloc = nullptr;
	}

	return TRUE;
}

BOOL i3TDKFolderSelectDialog::Execute( HWND hwndOwner, LPCTSTR lpszCaption, UINT Flag,
									BFFCALLBACK UserProc, int Root, LPCTSTR lpszInitDir )
{
	HRESULT Rv;
	BOOL Result = FALSE;
	BROWSEINFO BrowseInfo;
    LPITEMIDLIST pIdl = nullptr, pRootIdl = nullptr;

	if( Init() == FALSE) {
		goto ExitPart;
	}

    memset( &BrowseInfo, 0, sizeof(BROWSEINFO));

    BrowseInfo.hwndOwner = hwndOwner;
	BrowseInfo.pszDisplayName = szDirName;
    BrowseInfo.ulFlags = Flag;
    BrowseInfo.lpfn = UserProc;
	BrowseInfo.lParam = (LPARAM)lpszInitDir;

	if( UserProc == nullptr )
	{
		BrowseInfo.lpfn = BrowseCallbackProc;
	}

	if( Root != 0 ) {
		Rv = SHGetSpecialFolderLocation( hwndOwner, Root, &pRootIdl);
		if( FAILED(Rv)) {
			MessageBox( hwndOwner, _T("Could not organize the root element."), _T("Error"), MB_OK);
			goto ExitPart;
		}
	}
	BrowseInfo.pidlRoot = pRootIdl;
    
	if( lpszCaption == nullptr)
		BrowseInfo.lpszTitle = _T("Folder");
	else
		BrowseInfo.lpszTitle = lpszCaption;

	// Process...(blocked)
    pIdl = SHBrowseForFolder( &BrowseInfo);
    if( pIdl == nullptr) {
        goto ExitPart;
    }

    if( SHGetPathFromIDList( pIdl, szDir) == FALSE) {
    	MessageBox( hwndOwner, _T("Invalid PIDL value."), _T("Error"), MB_OK);
		goto ExitPart;
    }

	Result = TRUE;

ExitPart:
	if( pIdl != nullptr)
		pMalloc->Free( pIdl);

	return Result;
}

#endif