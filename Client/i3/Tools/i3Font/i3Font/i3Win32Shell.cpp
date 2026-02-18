#include "stdafx.h"

#if defined( I3_WINDOWS)
#include <shlobj.h>
#include "i3Win32Shell.h"

i3Win32FolderSelectDialog::i3Win32FolderSelectDialog(void)
{
	pMalloc = NULL;
}

i3Win32FolderSelectDialog::~i3Win32FolderSelectDialog(void)
{
	if( pMalloc != NULL)
		Release();
}

BOOL i3Win32FolderSelectDialog::Init(void)
{
	HRESULT Rv;

	if( pMalloc == NULL) {
		Rv = SHGetMalloc( &pMalloc);
		if( FAILED( Rv)) {
			MessageBox( NULL, "Not enough memory resource for shell command.", "Error", MB_OK);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL i3Win32FolderSelectDialog::Release(void)
{
	if( pMalloc != NULL) {
		pMalloc->Release();
		pMalloc = NULL;
	}

	return TRUE;
}

BOOL i3Win32FolderSelectDialog::Execute( HWND hwndOwner, LPCTSTR lpszCaption, UINT Flag,
									BFFCALLBACK UserProc,
									int Root)
{
	HRESULT Rv;
	BOOL Result = FALSE;
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

	if( Root != NULL) {
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

	Result = TRUE;

ExitPart:
	if( pIdl != NULL)
		pMalloc->Free( pIdl);

	return Result;
}

#endif