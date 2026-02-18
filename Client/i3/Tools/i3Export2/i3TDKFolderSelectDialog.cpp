#include "stdafx.h"

#if defined( I3_WINDOWS)
#include <shlobj.h>
#include "i3TDKFolderSelectDialog.h"

// 콜백함수
int CALLBACK i3TDKFolderSelectDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
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

i3TDKFolderSelectDialog::i3TDKFolderSelectDialog(void)
{
	pMalloc = NULL;
	szDir[0] = 0;
	szDirName[0] = 0;
}

i3TDKFolderSelectDialog::~i3TDKFolderSelectDialog(void)
{
	if( pMalloc != NULL)
		Release();
}

bool i3TDKFolderSelectDialog::Init(void)
{
	HRESULT Rv;

	if( pMalloc == NULL) {
		Rv = SHGetMalloc( &pMalloc);
		if( FAILED( Rv)) {
			MessageBox( NULL, "Not enough memory resource for shell command.", "Error", MB_OK);
			return false;
		}
	}

	return true;
}

bool i3TDKFolderSelectDialog::Release(void)
{
	if( pMalloc != NULL) {
		pMalloc->Release();
		pMalloc = NULL;
	}

	return true;
}

bool i3TDKFolderSelectDialog::Execute( HWND hwndOwner, LPCTSTR lpszCaption, UINT Flag,
									BFFCALLBACK UserProc, int Root, LPCTSTR lpszInitDir )
{
	HRESULT Rv;
	bool Result = false;
	BROWSEINFO BrowseInfo;
    LPITEMIDLIST pIdl = NULL, pRootIdl = NULL;

	if( Init() == FALSE) {
		goto ExitPart;
	}

    memset( &BrowseInfo, 0, sizeof(BROWSEINFO));

    BrowseInfo.hwndOwner = hwndOwner;
	BrowseInfo.pszDisplayName = szDirName;
    BrowseInfo.ulFlags = Flag;
    BrowseInfo.lpfn = UserProc;
	BrowseInfo.lParam = (LPARAM)lpszInitDir;

	if( UserProc == NULL )
	{
		BrowseInfo.lpfn = BrowseCallbackProc;
	}

	if( Root != 0 ) {
		Rv = SHGetSpecialFolderLocation( hwndOwner, Root, &pRootIdl);
		if( FAILED(Rv)) {
			MessageBox( hwndOwner, "Could not organize the root element.", "Error", MB_OK);
			goto ExitPart;
		}
	}
	BrowseInfo.pidlRoot = pRootIdl;
    
	if( lpszCaption == NULL)
		BrowseInfo.lpszTitle = "Folder";
	else
		BrowseInfo.lpszTitle = lpszCaption;

	// Process...(blocked)
    pIdl = SHBrowseForFolder( &BrowseInfo);
    if( pIdl == NULL) {
        goto ExitPart;
    }

    if( SHGetPathFromIDList( pIdl, szDir) == FALSE) {
    	MessageBox( hwndOwner, "Invalid PIDL value.", "Error", MB_OK);
		goto ExitPart;
    }

	Result = true;

ExitPart:
	if( pIdl != NULL)
		pMalloc->Free( pIdl);

	return Result;
}

#endif