#include "stdafx.h"
#include "pidl_util.h"

#include "i3shellmanager.h"

// fly exporter에서 갖고온 소스... (오버레이 아이콘의 인덱스가 필요해서 일단 넣어봄)

int GetItemIconOverlayIndex(const TCHAR* aPath)
{
    if (aPath == NULL)
        return -1;

	LPITEMIDLIST sFullPidl;

	if (FAILED(i3ShellManager::i()->ItemFromPath(aPath, sFullPidl)))
		return -1;
	
   int sOverlayIndex = GetItemIconOverlayIndex(sFullPidl);

   i3ShellManager::i()->FreeItem(sFullPidl);

   return sOverlayIndex;
}

int GetItemIconOverlayIndex(LPCITEMIDLIST aFullPidl)
{
    if (aFullPidl == NULL)
        return -1;

    int    sOverlayIndex = -1;
 //   bool   sResult;
    LPSHELLFOLDER sShellFolder  = NULL;
    LPCITEMIDLIST sPidl         = aFullPidl;
	
	int num_item = i3ShellManager::i()->GetItemCount(sPidl) - 1;

	for ( int i = 0; i < num_item; ++i)
	{
		sPidl = i3ShellManager::i()->GetNextItem(sPidl);
	}

	LPSHELLFOLDER desktopFolder;
	::SHGetDesktopFolder(&desktopFolder);

	LPITEMIDLIST parentpidl;
	i3ShellManager::i()->GetParentItem(aFullPidl, parentpidl);

	HRESULT hr = desktopFolder->BindToObject(parentpidl, NULL, IID_IShellFolder, (LPVOID*)&sShellFolder);

	i3ShellManager::i()->FreeItem(parentpidl);

	if ( SUCCEEDED(hr) )
	{
		sOverlayIndex = GetItemIconOverlayIndex(sShellFolder, sPidl);
		sShellFolder->Release();
	}

	desktopFolder->Release();
    return sOverlayIndex;
}

// This function must be called by worker thread.
// If it's called by UI thread, then it may be locked in the network folder on windows vista.
int GetItemIconOverlayIndex(LPSHELLFOLDER aShellFolder, LPCITEMIDLIST aPidl)
{
    if (!aShellFolder || !aPidl)
        return -1;

 //   if (IsFileSystem(aShellFolder, aPidl) == XPR_FALSE)
 //     return -1;

    HRESULT sHResult;
    int sOverlayIndex = -1;

    IShellIconOverlay* sShellIconOverlay = NULL;

    sHResult = aShellFolder->QueryInterface(IID_IShellIconOverlay, (LPVOID *)&sShellIconOverlay);
    if (SUCCEEDED(sHResult) && sShellIconOverlay != NULL)
    {
        sHResult = sShellIconOverlay->GetOverlayIndex(aPidl, &sOverlayIndex);

        // S_FALSE(0x00000001) - No overlay exists for this file.
        if (sHResult != S_OK)
            sOverlayIndex = -1;
		
		sShellIconOverlay->Release();
    }

    return sOverlayIndex;
}


