// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "i3shelltreectrl.h"
#include "i3shelllistctrl.h"
#include "i3shellmanager.h"

#include "pidl_util.h"

#include "i3MFCWorkAround.h"

#include "i3Base/string/ext/generic_string_copy.h"
#include "i3Base/itl/range/algorithm/replace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// i3ShellTreeCtrl

IMPLEMENT_DYNAMIC(i3ShellTreeCtrl, i3MFCTreeCtrl)

IContextMenu2* i3ShellTreeCtrl::m_pContextMenu2 = NULL;

i3ShellTreeCtrl::i3ShellTreeCtrl() 
{									 		
	m_bContextMenu = TRUE;
	m_hwndRelatedList = NULL;
	m_bNoNotify = FALSE;
	m_dwFlags = SHCONTF_FOLDERS;
}

i3ShellTreeCtrl::~i3ShellTreeCtrl()
{

}

void i3ShellTreeCtrl::SetRootPath(const i3::rc_wstring& strRootPath)
{
	m_strRootPath = strRootPath;
}

BEGIN_MESSAGE_MAP(i3ShellTreeCtrl, i3MFCTreeCtrl)
	//{{AFX_MSG_MAP(i3ShellTreeCtrl)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &i3ShellTreeCtrl::OnItemexpanding)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, &i3ShellTreeCtrl::OnDeleteitem)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// i3ShellTreeCtrl message handlers

int i3ShellTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3MFCTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//
	// XP급 이상에서 드래그드롭 텍스트 버그가 존재하는것 같음..
	// 이를 위한 workround로 폰트변경을 하면 처리가 된다고 한다..
	//
	//this->SetFont(const_cast<CFont*>(i3MFCWorkAround::i()->GetFontTreeCtrl()), TRUE);

	if (i3ShellManager::i() == NULL)
	{
		TRACE0("You need to initialize i3ShellManager first\n");
		return -1;
	}

	InitTree();
	return 0;
}

void i3ShellTreeCtrl::SetRelatedList(i3ShellListCtrl* pShellList)
{
	ASSERT_VALID(this);

	m_hwndRelatedList = (pShellList == NULL) ? NULL : pShellList->GetSafeHwnd();
	if (pShellList != NULL)
	{
		pShellList->m_hwndRelatedTree = GetSafeHwnd();
	}

	// 트리뷰 기준으로 맞출것..
	HTREEITEM item = this->GetSelectedItem();
	if ( item != NULL)
	{
		LPAFX_SHELLITEMINFO info = (LPAFX_SHELLITEMINFO)this->GetItemData(item);
		pShellList->DisplayFolder( info );
	}
	
}

i3ShellListCtrl* i3ShellTreeCtrl::GetRelatedList() const
{
	ASSERT_VALID(this);

	if (m_hwndRelatedList == NULL || !::IsWindow(m_hwndRelatedList))
	{
		return NULL;
	}

	i3ShellListCtrl* pList = DYNAMIC_DOWNCAST(i3ShellListCtrl, CWnd::FromHandlePermanent(m_hwndRelatedList));

	return pList;
}

void i3ShellTreeCtrl::Refresh()
{
	ASSERT_VALID(this);

	DeleteAllItems();

	GetRootItems();
	TreeView_SetScrollTime(GetSafeHwnd(), 100);
}

BOOL i3ShellTreeCtrl::GetRootItems()
{
	ASSERT_VALID(this);
	ENSURE(i3ShellManager::i() != NULL);
	ASSERT_VALID(i3ShellManager::i());


	LPITEMIDLIST pidl_desktop;
	if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl_desktop)))
	{
		return FALSE;
	}

	LPITEMIDLIST	pidl;
	LPSHELLFOLDER	parentFolder;


	if (m_strRootPath.empty() )
	{
		if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
		{
			return FALSE;
		}
		parentFolder = NULL;
	}
	else
	{
		i3::stack_wstring wstrPath = m_strRootPath;
		i3::range::replace(wstrPath, L'/', L'\\');

		if (FAILED(i3ShellManager::i()->ItemFromPath( wstrPath.c_str(), pidl) ) )
		{
			return FALSE;
		}
		
		LPITEMIDLIST parentpidl;
		i3ShellManager::i()->GetParentItem(pidl, parentpidl);
		
		// Get the desktop's IShellFolder:
		LPSHELLFOLDER pDesktop;
		if (FAILED(SHGetDesktopFolder(&pDesktop)))
		{
			return FALSE;
		}
	
		HRESULT hr = pDesktop->BindToObject(parentpidl, NULL, IID_IShellFolder, (LPVOID*)&parentFolder);
		
		pDesktop->Release();

		i3ShellManager::i()->FreeItem(parentpidl);
	}
	
	
	// Fill in the TVITEM structure for this item:
	TVITEM tvItem;
	memset(&tvItem, 0, sizeof(TVITEM));

	tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

	// Put the private information in the lParam:
	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));
	ENSURE(pItem != NULL);



	int num_item = i3ShellManager::i()->GetItemCount(pidl) - 1;

	LPCITEMIDLIST pidl_rel  = pidl;

	for ( int i = 0; i < num_item; ++i)
	{
		pidl_rel = i3ShellManager::i()->GetNextItem(pidl_rel);
	}

	pItem->pidlRel = i3ShellManager::i()->CopyItem(pidl_rel);
	pItem->pidlFQ = i3ShellManager::i()->CopyItem(pidl);
	
	i3ShellManager::i()->FreeItem(pidl);

	// The desktop doesn't have a parent folder, so make this NULL:
	pItem->pParentFolder = parentFolder;
	tvItem.lParam = (LPARAM)pItem;

	CString strItem = OnGetItemText(pItem);
	tvItem.pszText = strItem.GetBuffer(strItem.GetLength());
	tvItem.iImage = OnGetItemIcon(pItem, FALSE);
	tvItem.iSelectedImage = OnGetItemIcon(pItem, TRUE);

	// Assume the desktop has children:
	tvItem.cChildren = TRUE;

	int idx = GetItemIconOverlayIndex(pItem->pidlFQ);
	if (idx >= 0 )
	{
		tvItem.mask |= TVIF_STATE;
		tvItem.stateMask |= TVIS_OVERLAYMASK;
		tvItem.state |= INDEXTOOVERLAYMASK(idx);
	}

	// Fill in the TV_INSERTSTRUCT structure for this item:
	TVINSERTSTRUCT tvInsert;

	tvInsert.item = tvItem;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.hParent = TVI_ROOT;

	// Add the item:
	HTREEITEM hParentItem = InsertItem(&tvInsert);

	// Go ahead and expand this item:
	Expand(hParentItem, TVE_EXPAND);


	return TRUE;
}

BOOL i3ShellTreeCtrl::GetChildItems(HTREEITEM hParentItem)
{
	ASSERT_VALID(this);

	CWaitCursor wait;

	// Get the parent item's pidl:
	TVITEM tvItem;
	ZeroMemory(&tvItem, sizeof(tvItem));

	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = hParentItem;

	if (!GetItem(&tvItem))
	{
		return FALSE;
	}

	SetRedraw(FALSE);

	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) tvItem.lParam;
	ENSURE(pItem != NULL);

	LPSHELLFOLDER pParentFolder = NULL;
	HRESULT hr;

	// If the parent folder is NULL, then we are at the root
	// of the namespace, so the parent of this item is the desktop folder
	if (pItem->pParentFolder == NULL)
	{
		hr = SHGetDesktopFolder(&pParentFolder);
	}
	else
	{
		// Otherwise we need to get the IShellFolder for this item:
		hr = pItem->pParentFolder->BindToObject(pItem->pidlRel, NULL, IID_IShellFolder, (LPVOID*) &pParentFolder);
	}

	if (FAILED(hr))
	{
		SetRedraw();
		return FALSE;
	}

	EnumObjects(hParentItem, pParentFolder, pItem->pidlFQ);

	// Sort the new items:
	TV_SORTCB tvSort;

	tvSort.hParent = hParentItem;
	tvSort.lpfnCompare = CompareProc;
	tvSort.lParam = 0;

	SortChildrenCB(&tvSort);

	SetRedraw();
	RedrawWindow();

	pParentFolder->Release();
	return TRUE;
}

HRESULT i3ShellTreeCtrl::EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent)
{
	ASSERT_VALID(this);
	ASSERT_VALID(i3ShellManager::i());

	LPSHELLFOLDER pDesktopFolder = NULL;
	SHGetDesktopFolder(&pDesktopFolder);

	LPITEMIDLIST pidl_Recycle = NULL, pidl_Control1 = NULL, pidl_Control2 = NULL, pidl_Network = NULL;
	bool         bDesktopParent = false;
	if ( pParentFolder == pDesktopFolder )
	{
		ULONG nEaten=0;
		ULONG nCPAttrib = 0;
		SHGetFolderLocation( NULL, CSIDL_BITBUCKET, NULL, 0, &pidl_Recycle );	//휴지통
		//CSIDL_CONTROLS -> 이 항목은 제어판이 맞기는 한데, 모든 제어판 항목이라고 등록되어있다. 제어판과는 다른것으로 보인다.
		HRESULT hr = pParentFolder->ParseDisplayName(NULL, NULL, _T("::{26EE0668-A00A-44D7-9371-BEB064C98683}"), &nEaten, &pidl_Control1, &nCPAttrib);	//control panel. vista
		SHGetFolderLocation( NULL, CSIDL_NETWORK, NULL, 0, &pidl_Network );	//네트워크
		nEaten = 0; nCPAttrib = 0;
		hr = pParentFolder->ParseDisplayName(NULL, NULL, _T("::{21EC2020-3AEA-1069-A2DD-08002B30309D}"), &nEaten, &pidl_Control2, &nCPAttrib);	//제어판 xp.

		bDesktopParent = true;
	}
	pDesktopFolder->Release();

	LPENUMIDLIST pEnum;
	HRESULT hr = pParentFolder->EnumObjects(NULL, m_dwFlags, &pEnum);
	if (FAILED(hr))
	{
		return hr;
	}

	LPITEMIDLIST pidlTemp;
	DWORD dwFetched = 1;

	// Enumerate the item's PIDLs:
	while (SUCCEEDED(pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
	{
		if (bDesktopParent)
		{
			if( ILIsEqual( pidlTemp, pidl_Recycle ) || ILIsEqual( pidlTemp, pidl_Network ) )
				continue;

//			if( (CFrameWndEx*)AfxGetMainWnd()->isWinVerLargerThan6() )
			{
				if( ILIsEqual( pidlTemp, pidl_Control1 ) || ILIsEqual( pidlTemp, pidl_Control2 ) ) 
					continue;
			}
//			else
//			{
//				if( ILIsEqual( pidlTemp, pidl_Control2 ) )
//					continue;
//			}
		}

		DWORD dwAttribs = SFGAO_STREAM;							// 압축파일폴더 표시 제외..(제외시키지 않으면 압축푸느라고 시간 다 까먹음)
		pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*) &pidlTemp, &dwAttribs);
		
		if ( dwAttribs & SFGAO_STREAM )
			continue;

		TVITEM tvItem;
		ZeroMemory(&tvItem, sizeof(tvItem));

		// Fill in the TV_ITEM structure for this item:
		tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

		// AddRef the parent folder so it's pointer stays valid:
		pParentFolder->AddRef();

		// Put the private information in the lParam:
		LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO)GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));
		ENSURE(pItem != NULL);

		pItem->pidlRel = pidlTemp;
		pItem->pidlFQ = i3ShellManager::i()->ConcatenateItem(pidlParent, pidlTemp);

		pItem->pParentFolder = pParentFolder;
		tvItem.lParam = (LPARAM)pItem;

		CString strItem = OnGetItemText(pItem);
		tvItem.pszText = strItem.GetBuffer(strItem.GetLength());
		tvItem.iImage = OnGetItemIcon(pItem, FALSE);
		tvItem.iSelectedImage = OnGetItemIcon(pItem, TRUE);

		// Determine if the item has children:
		//DWORD 
		dwAttribs = SFGAO_SHARE | SFGAO_HASSUBFOLDER ; // | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME;

		pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*) &pidlTemp, &dwAttribs);

//		tvItem.cChildren = (dwAttribs & SFGAO_HASSUBFOLDER);
		// 자식의 압축파일과 히든 폴더를 제외시켜주어야한다.. (원본 MFC는 이게 누락됨)
		// 다소 지저분한 소스코드이지만, 현재로서는 불가피한 선택이다.. (또한번 서브폴더를 뒤져서 히든이나 압축파일만 존재하는 경우 제끼는 루틴임)

		if ( dwAttribs & SFGAO_HASSUBFOLDER )
		{
			LPSHELLFOLDER pFolder;
		    hr = pParentFolder->BindToObject(pidlTemp, NULL, IID_IShellFolder, (LPVOID*) &pFolder);
			
			if ( SUCCEEDED(hr) )
			{
				LPENUMIDLIST pEnum2 = NULL;
				DWORD dwHiddenFlags = m_dwFlags; // | SHCONTF_INCLUDEHIDDEN;
				hr = pFolder->EnumObjects(NULL, dwHiddenFlags, &pEnum2);
				
				if  ( SUCCEEDED(hr) && hr != S_FALSE )
				{
					LPITEMIDLIST pidlTemp2;
					DWORD dwFetched2 = 1;
				
					while (SUCCEEDED(pEnum2->Next(1, &pidlTemp2, &dwFetched2)) && dwFetched2)   // 
					{
						DWORD dwAttribs2 = SFGAO_STREAM; // | SFGAO_HIDDEN ;		
						pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*) &pidlTemp2, &dwAttribs2);
				
						if ( ! ( dwAttribs2 & (SFGAO_STREAM))) // | SFGAO_HIDDEN) ) )
						{
							tvItem.cChildren = 1;		
							break;
						}
						dwFetched2 = 0;
					}
					
					pEnum2->Release();
				}

				pFolder->Release();
			}
		}
	
		int idx = GetItemIconOverlayIndex(pItem->pidlFQ);
		if (idx >= 0 )
		{
			tvItem.mask |= TVIF_STATE;
			tvItem.stateMask |= TVIS_OVERLAYMASK;
			tvItem.state |= INDEXTOOVERLAYMASK(idx);
		}
		else	// Determine if the item is shared:
		if (dwAttribs & SFGAO_SHARE)
		{
			tvItem.mask |= TVIF_STATE;
			tvItem.stateMask |= TVIS_OVERLAYMASK;
			tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
		}
		
		// Fill in the TV_INSERTSTRUCT structure for this item:
		TVINSERTSTRUCT tvInsert;

		tvInsert.item = tvItem;
		tvInsert.hInsertAfter = TVI_LAST;
		tvInsert.hParent = hParentItem;

		InsertItem(&tvInsert);
		dwFetched = 0;

	}

	pEnum->Release();

	if (bDesktopParent)
	{
		if( pidl_Control1 != NULL )
			CoTaskMemFree( pidl_Control1 );
		if( pidl_Control2 != NULL )
			CoTaskMemFree( pidl_Control2 );
	}

	return S_OK;
}

int CALLBACK i3ShellTreeCtrl::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNREFERENCED_PARAMETER(lParamSort);

	LPAFX_SHELLITEMINFO pItem1 = (LPAFX_SHELLITEMINFO)lParam1;
	LPAFX_SHELLITEMINFO pItem2 = (LPAFX_SHELLITEMINFO)lParam2;

	HRESULT hr = pItem1->pParentFolder->CompareIDs(0, pItem1->pidlRel, pItem2->pidlRel);

	if (FAILED(hr))
	{
		return 0;
	}

	return(short)SCODE_CODE(GetScode(hr));
}

void i3ShellTreeCtrl::OnShowContextMenu(CPoint point)
{
	if (m_pContextMenu2 != NULL)
	{
		return;
	}

	if (!m_bContextMenu)
	{
		Default();
		return;
	}

	HTREEITEM hItem = NULL;
	if (point.x == -1 && point.y == -1)
	{
		CRect rectItem;

		if ((hItem = GetSelectedItem()) != NULL && GetItemRect(hItem, rectItem, FALSE))
		{
			point.x = rectItem.left;
			point.y = rectItem.bottom + 1;

			ClientToScreen(&point);
		}
	}
	else
	{
		CPoint ptClient = point;
		ScreenToClient(&ptClient);

		UINT nFlags = 0;
		hItem = HitTest(ptClient, &nFlags);
	}

	if (hItem == NULL)
	{
		return;
	}

	TVITEM tvItem;

	ZeroMemory(&tvItem, sizeof(tvItem));
	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = hItem;

	if (!GetItem(&tvItem))
	{
		return;
	}

	LPAFX_SHELLITEMINFO pInfo = (LPAFX_SHELLITEMINFO)tvItem.lParam;
	if (pInfo == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	IShellFolder* psfFolder = pInfo->pParentFolder;

	if (psfFolder == NULL)
	{
		ENSURE(SUCCEEDED(SHGetDesktopFolder(&psfFolder)));
	}
	else
	{
		psfFolder->AddRef();
	}

	if (psfFolder != NULL)
	{
		HWND hwndParent = GetParent()->GetSafeHwnd();
		IContextMenu* pcm = NULL;

		HRESULT hr = psfFolder->GetUIObjectOf(hwndParent, 1, (LPCITEMIDLIST*)&pInfo->pidlRel, IID_IContextMenu, NULL, (LPVOID*)&pcm);

		if (SUCCEEDED(hr))
		{
			HMENU hPopup = CreatePopupMenu();
			if (hPopup != NULL)
			{
				hr = pcm->QueryContextMenu(hPopup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);

				if (SUCCEEDED(hr))
				{
					pcm->QueryInterface(IID_IContextMenu2, (LPVOID*)&m_pContextMenu2);

					HWND hwndThis = GetSafeHwnd();
					UINT idCmd = TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, GetSafeHwnd(), NULL);

					if (::IsWindow(hwndThis))
					{
						if (m_pContextMenu2 != NULL)
						{
							m_pContextMenu2->Release();
							m_pContextMenu2 = NULL;
						}

						if (idCmd != 0)
						{
							CWaitCursor wait;

							CMINVOKECOMMANDINFO cmi;
							cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
							cmi.fMask = 0;
							cmi.hwnd = hwndParent;
							cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
							cmi.lpParameters = NULL;
							cmi.lpDirectory = NULL;
							cmi.nShow = SW_SHOWNORMAL;
							cmi.dwHotKey = 0;
							cmi.hIcon = NULL;

							hr = pcm->InvokeCommand(&cmi);

							if (SUCCEEDED(hr) && GetParent() != NULL)
							{
								GetParent()->SendMessage(I3_WM_ON_AFTER_SHELL_COMMAND, (WPARAM) idCmd);
							}

							SetFocus();
						}
					}
				}
			}

			if (pcm != NULL)
			{
				pcm->Release();
				pcm = NULL;
			}
		}

		if (psfFolder != NULL)
		{
			psfFolder->Release();
			psfFolder = NULL;
		}
	}
}

CString i3ShellTreeCtrl::OnGetItemText(LPAFX_SHELLITEMINFO pItem)
{
	ENSURE(pItem != NULL);

	SHFILEINFO sfi;

	if (SHGetFileInfo((LPCTSTR) pItem->pidlFQ, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
	{
		return sfi.szDisplayName;
	}

	return _T("???");
}

int i3ShellTreeCtrl::OnGetItemIcon(LPAFX_SHELLITEMINFO pItem, BOOL bSelected)
{
	ENSURE(pItem != NULL);

	SHFILEINFO sfi;

	UINT uiFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;

	if (bSelected)
	{
		uiFlags |= SHGFI_OPENICON;
	}
	else
	{
		uiFlags |= SHGFI_LINKOVERLAY;
	}

	if (SHGetFileInfo((LPCTSTR)pItem->pidlFQ, 0, &sfi, sizeof(sfi), uiFlags))
	{
		return sfi.iIcon;
	}

	return -1;
}

void i3ShellTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ENSURE(pNMTreeView != NULL);

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	ENSURE(hItem != NULL);

	switch (pNMTreeView->action)
	{
	case TVE_EXPAND:
		GetChildItems(hItem);

		if (GetChildItem(hItem) == NULL)
		{
			// Remove '+':
			TV_ITEM tvItem;
			ZeroMemory(&tvItem, sizeof(tvItem));

			tvItem.hItem = hItem;
			tvItem.mask = TVIF_CHILDREN;

			SetItem(&tvItem);
		}
		break;

	case TVE_COLLAPSE:
		{
			for (HTREEITEM hItemSel = GetSelectedItem(); hItemSel != NULL;)
			{
				HTREEITEM hParentItem = GetParentItem(hItemSel);

				if (hParentItem == hItem)
				{
					SelectItem(hItem);
					break;
				}

				hItemSel = hParentItem;
			}

			//remove all of the items from this node
			Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
		}
		break;
	}

	*pResult = 0;
}

void i3ShellTreeCtrl::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT_VALID(i3ShellManager::i());

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ENSURE(pNMTreeView != NULL);

	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) pNMTreeView->itemOld.lParam;

	// Free up the pidls that we allocated:
	i3ShellManager::i()->FreeItem(pItem->pidlFQ);
	i3ShellManager::i()->FreeItem(pItem->pidlRel);

	// This may be NULL if this is the root item:
	if (pItem->pParentFolder != NULL)
	{
		pItem->pParentFolder->Release();
	}

	GlobalFree((HGLOBAL) pItem);
//	delete pItem;

	*pResult = 0;
}

void i3ShellTreeCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (m_bContextMenu)
	{
		OnShowContextMenu(point);
	}
	else
	{
		Default();
	}
}

BOOL i3ShellTreeCtrl::SelectPath(LPCTSTR lpszPath)
{
	ASSERT_VALID(this);
	ASSERT_VALID(i3ShellManager::i());
	ENSURE(lpszPath != NULL);

	LPITEMIDLIST pidl;
	if (FAILED(i3ShellManager::i()->ItemFromPath(lpszPath, pidl)))
	{
		return FALSE;
	}

	BOOL bRes = SelectPath(pidl);
	i3ShellManager::i()->FreeItem(pidl);

	return bRes;
}

BOOL i3ShellTreeCtrl::SelectPath(LPCITEMIDLIST lpidl)
{
	BOOL bRes = FALSE;

	ASSERT_VALID(this);
	ASSERT_VALID(i3ShellManager::i());

	if (lpidl == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	HTREEITEM htreeItem = GetRootItem();

	SetRedraw(FALSE);

	if (i3ShellManager::i()->GetItemCount(lpidl) == 0)
	{
		// Desktop
	}
	else
	{
		LPAFX_SHELLITEMINFO pItemRoot = (LPAFX_SHELLITEMINFO) GetItemData(htreeItem);
		
		if (!IsEqualPidl(pItemRoot->pidlFQ, lpidl) )
		{
			LPCITEMIDLIST lpidlCurr = lpidl;
			LPITEMIDLIST lpidlParent;

			CList<LPITEMIDLIST,LPITEMIDLIST> lstItems;
			lstItems.AddHead(i3ShellManager::i()->CopyItem(lpidl));

			while (i3ShellManager::i()->GetParentItem(lpidlCurr, lpidlParent) > 0)
			{
				if ( IsEqualPidl(lpidlParent, pItemRoot->pidlFQ) )
				{
					i3ShellManager::i()->FreeItem(lpidlParent);
					break;
				}

				lstItems.AddHead(lpidlParent);
				lpidlCurr = lpidlParent;
			}
		
			for (POSITION pos = lstItems.GetHeadPosition(); pos != NULL;)
			{
				LPITEMIDLIST lpidlList = lstItems.GetNext(pos);

				if (htreeItem != NULL)
				{
					if (GetChildItem(htreeItem) == NULL)	// 실제로는 존재하나, 표시만 안된 경우를 의미한다..(이경우 모두 표시하게끔 처리됨)
					{
						Expand(htreeItem, TVE_EXPAND);
					}

					BOOL bFound = FALSE;

					for (HTREEITEM hTreeChild = GetChildItem(htreeItem); !bFound && hTreeChild != NULL; hTreeChild = GetNextSiblingItem(hTreeChild))
					{
						LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GetItemData(hTreeChild);
						if (pItem == NULL)
						{
							continue;
						}

						SHFILEINFO sfi1;
						SHFILEINFO sfi2;

						if (SHGetFileInfo((LPCTSTR) pItem->pidlFQ, 0, &sfi1, sizeof(sfi1), SHGFI_PIDL | SHGFI_DISPLAYNAME) &&
							SHGetFileInfo((LPCTSTR) lpidlList, 0, &sfi2, sizeof(sfi2), SHGFI_PIDL | SHGFI_DISPLAYNAME) && lstrcmp(sfi1.szDisplayName, sfi2.szDisplayName) == 0)
						{
							bFound = TRUE;
							htreeItem = hTreeChild;
						}
					}

					if (!bFound)
					{
						htreeItem = NULL;
					}
				}

				i3ShellManager::i()->FreeItem(lpidlList);
			}
		}
		else
		{
			// IsEqualPidl(pItemRoot->pidlFQ, lpidl) 인 경우  htreeItem은 루트를 유지할것..
		}
	}

	if (htreeItem != NULL)
	{
		m_bNoNotify = TRUE;

		SelectItem(htreeItem);

		if (GetChildItem(htreeItem) == NULL)
		{
			Expand(htreeItem, TVE_EXPAND);
		}

		EnsureVisible(htreeItem);

		m_bNoNotify = FALSE;
		bRes = TRUE;
	}

	SetRedraw();
	RedrawWindow();

	return bRes;
}

BOOL i3ShellTreeCtrl::SelectRoot()
{
	HTREEITEM item = this->GetRootItem();

	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GetItemData(item);
	if (pItem == NULL || pItem->pidlFQ == NULL || pItem->pidlRel == NULL)
	{
		return FALSE;
	}
	
	return this->SelectPath(pItem->pidlFQ);
}



BOOL i3ShellTreeCtrl::GetItemPath(CString& strPath, HTREEITEM htreeItem) const
{
	ASSERT_VALID(this);

	BOOL bRes = FALSE;
	strPath.Empty();

	if (htreeItem == NULL)
	{
		htreeItem = GetSelectedItem();
	}

	if (htreeItem == NULL)
	{
		return FALSE;
	}

	LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GetItemData(htreeItem);
	if (pItem == NULL || pItem->pidlFQ == NULL || pItem->pidlRel == NULL)
	{
		return FALSE;
	}

	LPSHELLFOLDER lpShellFolder = NULL;
	HRESULT hRes;

	if (pItem->pParentFolder == NULL)
	{
		hRes = SHGetDesktopFolder(&lpShellFolder);
	}
	else
	{
		hRes = pItem->pParentFolder->BindToObject(pItem->pidlRel, 0, IID_IShellFolder, (LPVOID*) &lpShellFolder);
	}

	if (FAILED(hRes))
	{
		return FALSE;
	}

	ULONG uAttribs = SFGAO_FILESYSTEM;
	if (pItem->pParentFolder != NULL)
	{
		pItem->pParentFolder->GetAttributesOf(1, (const struct _ITEMIDLIST **)&pItem->pidlFQ, &uAttribs);
	}
	// Else - assume desktop

	if ((uAttribs & SFGAO_FILESYSTEM) != 0)
	{
		TCHAR szFolderName [MAX_PATH];
		if (SHGetPathFromIDList(pItem->pidlFQ, szFolderName))
		{
			strPath = szFolderName;
			bRes = TRUE;
		}
	}

	if (lpShellFolder != NULL)
	{
		lpShellFolder->Release();
	}

	return bRes;
}

void i3ShellTreeCtrl::OnRButtonDown(UINT /*nFlags*/, CPoint point)
{
	SetFocus();
	UINT nFlags = 0;
	SelectItem(HitTest(point, &nFlags));
}

void i3ShellTreeCtrl::EnableShellContextMenu(BOOL bEnable)
{
	m_bContextMenu = bEnable;
}

BOOL i3ShellTreeCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (message == WM_NOTIFY && !m_bNoNotify)
	{
		LPNMHDR lpnmh = (LPNMHDR) lParam;
		ENSURE(lpnmh != NULL);

		if (lpnmh->code == TVN_SELCHANGED)
		{
			i3ShellListCtrl* pRelatedShellList = GetRelatedList();

			if (pRelatedShellList != NULL && GetSelectedItem() != NULL)
			{
				ASSERT_VALID(pRelatedShellList);
				LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO) GetItemData(GetSelectedItem());

				pRelatedShellList->m_bNoNotify = TRUE;
				pRelatedShellList->DisplayFolder(pItem);
				pRelatedShellList->m_bNoNotify = FALSE;

				return TRUE;
			}
		}
	}

	return i3MFCTreeCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

void i3ShellTreeCtrl::OnDestroy()
{
	i3ShellListCtrl* pList = GetRelatedList();
	if (pList != NULL)
	{
		pList->m_hwndRelatedTree = NULL;
	}

	i3MFCTreeCtrl::OnDestroy();
}

LRESULT i3ShellTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITMENUPOPUP:
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
		if (m_pContextMenu2 != NULL)
		{
			m_pContextMenu2->HandleMenuMsg(message, wParam, lParam);
			return 0;
		}
		break;
	}

	return i3MFCTreeCtrl::WindowProc(message, wParam, lParam);
}

void i3ShellTreeCtrl::PreSubclassWindow()
{
	i3MFCTreeCtrl::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		InitTree();
	}
}

void i3ShellTreeCtrl::InitTree()
{
	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) > 0)
	{
		SHFILEINFO sfi;
		SetImageList(CImageList::FromHandle((HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)), 0);
	}

	Refresh();
}

void i3ShellTreeCtrl::SetFlags(DWORD dwFlags, BOOL bRefresh)
{
	ASSERT_VALID(this);
	m_dwFlags = dwFlags;

	if (bRefresh && GetSafeHwnd() != NULL)
	{
		Refresh();
	}
}
