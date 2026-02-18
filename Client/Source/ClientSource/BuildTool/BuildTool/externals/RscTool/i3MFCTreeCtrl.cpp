#include "stdafx.h"

#include "i3MFCTreeCtrl.h"

#include "i3Base/string/compare/generic_is_equal.h"

#include "i3Base/string/ext/strtok_fast.h"

#include "IconInfo.h"

#include "util/PathTokenizer.h"
#include <stack>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(i3MFCTreeCtrl, CTreeCtrl)

namespace
{
	struct i3MFCTreeCtrlCallback_Null : i3MFCTreeCtrlCallback
	{

	};

	i3MFCTreeCtrlCallback_Null	g_callback_null;
}

i3MFCTreeCtrl::i3MFCTreeCtrl() : m_callback(&g_callback_null),
 m_pTreeDragImage(NULL), m_hDragItem(NULL)
{

}

i3MFCTreeCtrl::~i3MFCTreeCtrl()
{
	delete m_pTreeDragImage;
}

BEGIN_MESSAGE_MAP(i3MFCTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(i3ShellTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &i3MFCTreeCtrl::OnTvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &i3MFCTreeCtrl::OnTvnEndlabeledit)
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &i3MFCTreeCtrl::OnNMCustomDraw)
END_MESSAGE_MAP()

int i3MFCTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL	i3MFCTreeCtrl::CreateEx(CWnd* pParent, UINT nID, DWORD dwAddStyle)
{

	CRect rectDummy (0, 0, 0, 0);
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_TRACKSELECT | //TVS_HASLINES | //TVS_NOHSCROLL
		TVS_HASBUTTONS | TVS_SHOWSELALWAYS 
		| TVS_FULLROWSELECT 
		| dwAddStyle;		// TVS_SHOWSELALWAYS 일단 추가해봄.(나름 필요한 옵션임)
	
	BOOL bRes =  CTreeCtrl::Create(dwViewStyle, rectDummy, pParent, nID);
	::SetWindowTheme(this->GetSafeHwnd(), L"explorer", 0);

	this->SetExtendedStyle(//TVS_EX_AUTOHSCROLL | 
		TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER
		, //TVS_EX_AUTOHSCROLL  | 
		TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER);


	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) > 0)
	{
		SHFILEINFO sfi;
		SetImageList(CImageList::FromHandle((HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)), 0);
	}


	return bRes;
}



void	i3MFCTreeCtrl::SetSize( int cx, int cy)
{
	this->SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOACTIVATE | SWP_NOZORDER );
}

void	i3MFCTreeCtrl::SetRect( const CRect& rc)
{
	this->SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}


HTREEITEM	i3MFCTreeCtrl::AddRootItem(const wchar_t* szItemName, void* userData)
{
	return i3MFCTreeCtrl::AddItem(TVI_ROOT, szItemName, userData);
}


// 부모 아이템에 +마크가 자동으로 안박히는것 같다.. 그 부분 처리를 한다..
HTREEITEM	i3MFCTreeCtrl::AddItem(HTREEITEM parentItem, const wchar_t* szItemName, void* userData)
{
	TV_ITEM tvItem;
	tvItem.mask = TVIF_PARAM | TVIF_TEXT |  TVIF_IMAGE | TVIF_SELECTEDIMAGE;		
	tvItem.lParam = LPARAM(userData);
	tvItem.pszText = const_cast<wchar_t*>(szItemName);
	tvItem.iImage = GetFolderItemIconIndex(); //0;					// OnGetItemIcon ...
	tvItem.iSelectedImage =  GetFolderItemIconIndex_Open();

	TV_INSERTSTRUCT tvInsert;
	tvInsert.item = tvItem;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.hParent = parentItem; 
	
	BOOL	  parent_has_children = (parentItem != TVI_ROOT ) ? this->ItemHasChildren(parentItem) : FALSE;

	HTREEITEM res =  CTreeCtrl::InsertItem(&tvInsert);	
	
	if (parent_has_children == FALSE)
	{
	//	memset(&tvItem, 0, sizeof(TV_ITEM));
		tvItem.hItem = parentItem;
		tvItem.mask = TVIF_CHILDREN;
		tvItem.cChildren = 1;
		this->SetItem(&tvItem);			// 부모에 +마크 박기..
	}

	return res;
}

BOOL	i3MFCTreeCtrl::DeleteItem(HTREEITEM hItem)		// 기본클래스 함수와 이름이 같은데 비가상이므로 가려지게 된다..
{
	HTREEITEM hParent =	this->GetParentItem(hItem);
	
	BOOL bRes = CTreeCtrl::DeleteItem(hItem);

	// 부모에 박힌 +마크를 제거할지 결정해야한다..(자식이 더 없다면 제거...)
	HTREEITEM first_children = this->GetChildItem(hParent);
	if (first_children == 0)
	{
		TV_ITEM tvItem;
		tvItem.hItem = hParent;
		tvItem.mask = TVIF_CHILDREN;
		tvItem.cChildren = 0;
		this->SetItem(&tvItem);			// 부모에 +마크 박기..
	}

	return bRes;
}


BOOL i3MFCTreeCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	if (message == WM_NOTIFY) 
	{
		LPNMHDR lpnmh = LPNMHDR(lParam);
		
		if (lpnmh->code == TVN_SELCHANGED )
		{
			HTREEITEM item = this->GetSelectedItem();
			void*     item_data = (item) ? reinterpret_cast<void*>(this->GetItemData(item)) : NULL;
			m_callback->OnSelChanged(this, item, item_data);
			
			return TRUE;
		}
		
		if (lpnmh->code == NM_CLICK)
		{
			HTREEITEM item = this->GetSelectedItem();
			void*     item_data = (item) ? reinterpret_cast<void*>(this->GetItemData(item)) : NULL;
			m_callback->OnItemLClicked(this, item, item_data);

			return TRUE;
		}
	}

	return CTreeCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}



//
// 기존 CreateDragImage함수가 엉망으로 동작해서..따로 만듬..
// 기존 공개소스가 도움은 되었으나, 해결책까지는 되지 않았음....
// 아래에 처럼 CompatibleDC 2개를 두고, TransparentBlt까지 찍어줘야 일단 렌더링이 된다..
//


CImageList*	i3MFCTreeCtrl::CreateDragImageEx(HTREEITEM hitem)
{
	CClientDC	dc(this);
	CDC			mem_dc;
	CBitmap		bitmap;
	CRect		rc_bitmap;
	
	int			cx_text, cy_text;
	int			cx_image, cy_image;

	this->GetItemRect(hitem, &rc_bitmap, FALSE);	// 전체 rect가 전달된다.  FALSE인수 결과값은 한줄 전체 결과값이라 사실 션찮다.
													// 션찮기 때문에 rc_bitmap은 공간확보를 제외한 다른 경우엔 쓰지 말것..
													// 혹은 rc_image와 rc_text의 inflate_rectr를 써도 좋지만..일단 이걸로 종료..
	{
		CRect rc_text;
		this->GetItemRect(hitem, &rc_text, TRUE);		//
		cx_text = rc_text.Width();
		cy_text = rc_text.Height();
	}
	
	cx_image = GetSystemMetrics(SM_CXSMICON);
	cy_image = GetSystemMetrics(SM_CYSMICON);

	if (!mem_dc.CreateCompatibleDC(&dc) )
		return NULL;
	if (!bitmap.CreateCompatibleBitmap(&dc, rc_bitmap.Width(), rc_bitmap.Height()) )
		return NULL;
	
	CBitmap* old_bitmap = mem_dc.SelectObject(&bitmap);

	mem_dc.FillSolidRect(0,0,rc_bitmap.Width(), rc_bitmap.Height(), RGB(0,255,0) );		// 키칼라로 일단 도배..
	
	TCHAR str_maxPath[MAX_PATH];
	TVITEM tvItem;	::memset(&tvItem, 0, sizeof(TVITEM));
	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_STATE;
	tvItem.hItem = hitem;
	tvItem.pszText = str_maxPath;
	tvItem.cchTextMax = MAX_PATH;
	tvItem.stateMask = TVIS_OVERLAYMASK;
	this->GetItem(&tvItem);
	
	CImageList* single_image_list = this->GetImageList( TVSIL_NORMAL );

	if (single_image_list )
	{
		
		CDC mem_dc2;
		CBitmap bitmap2;
		
		mem_dc2.CreateCompatibleDC(&dc);
		
		bitmap2.CreateCompatibleBitmap(&dc, cx_image, cy_image);

		CBitmap* old_bitmap2 = mem_dc2.SelectObject(&bitmap2);

		mem_dc2.FillSolidRect(0,0,cx_image, cy_image, RGB(0,255,0) );
	
		CPoint pt(0,0); 
		
		single_image_list->Draw(&mem_dc2, tvItem.iImage, pt, ILD_TRANSPARENT | ( tvItem.state & TVIS_OVERLAYMASK )  );


		mem_dc.TransparentBlt(0, 0, cx_image, cy_image, &mem_dc2, 0,0,
						cx_image, cy_image , RGB(0,255,0) );
					
		mem_dc2.SelectObject(old_bitmap2);

		bitmap2.DeleteObject();
	}

	// TEXT 출력 ..일단 나중에..
	CFont* pFont = this->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfQuality = NONANTIALIASED_QUALITY;		// 일단 이걸로...

	CFont newFont;
	newFont.CreateFontIndirect(&lf);
	CFont* oldFont = mem_dc.SelectObject(&newFont);
	
	{
		CRect rc;
		rc.left = cx_image + 4;		// 4는 매직넘버 ..일단 방치..
		rc.top  = 0;
		rc.right = rc.left + cx_text;
		rc.bottom = rc.top + cy_text;
		mem_dc.DrawText(str_maxPath, -1, rc, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX); //DT_END_ELLIPSIS | DT_MODIFYSTRING);
	}
	mem_dc.SelectObject(oldFont);
	mem_dc.SelectObject(old_bitmap);

	CImageList* result_img_list = new CImageList;

	result_img_list->Create(rc_bitmap.Width(), rc_bitmap.Height(), ILC_COLOR32 | ILC_MASK, 0, 1);

	result_img_list->Add(&bitmap, RGB(0,255,0) );

	bitmap.DeleteObject();

	return result_img_list;
}

void i3MFCTreeCtrl::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	if (m_pTreeDragImage)
	{
		m_pTreeDragImage->DeleteImageList();
		delete m_pTreeDragImage;
	}

	m_pTreeDragImage = this->CreateDragImageEx(pNMTreeView->itemNew.hItem);

	CRect rc;
	this->GetItemRect(pNMTreeView->itemNew.hItem, &rc, TRUE);

	int cx_image = GetSystemMetrics(SM_CXSMICON);

	m_pTreeDragImage->BeginDrag(0, CPoint(
		pNMTreeView->ptDrag.x - rc.left + cx_image + 2, 
		pNMTreeView->ptDrag.y - rc.top - 1) );

	m_pTreeDragImage->DragEnter(GetDesktopWindow(), pNMTreeView->ptDrag);
	
	this->SetCapture();

	m_hDragItem = pNMTreeView->itemNew.hItem;

	*pResult = 0;
}

void i3MFCTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnMouseMove(nFlags, point);

	// TODO: Add your message handler code here and/or call default
	
	if (m_pTreeDragImage)
	{
		CPoint p = point;
		ClientToScreen(&p);
		
		m_pTreeDragImage->DragMove(p);
		
		m_pTreeDragImage->DragShowNolock(FALSE); //DragLeave(this);
		
		CWnd* pDropWnd = WindowFromPoint(p);

		if (pDropWnd == this)
		{
			HTREEITEM hitem = this->HitTest(point);
	
			if (hitem != this->GetDropHilightItem())
			{
				this->SelectDropTarget(hitem);
			}
		}
		else
		{
			this->SelectDropTarget(NULL);
			m_callback->OnDragMove(this, m_hDragItem, pDropWnd, p);			// 콜백 넘김...
		}

		m_pTreeDragImage->DragShowNolock(TRUE); //DragEnter(this, p);
	}
}



void i3MFCTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_pTreeDragImage)
	{
		::ReleaseCapture();

		m_pTreeDragImage->DragLeave(this);
		m_pTreeDragImage->EndDrag();

		m_pTreeDragImage->DeleteImageList();
		delete m_pTreeDragImage;
		m_pTreeDragImage = NULL;

//		HTREEITEM tgtItem = this->GetDropHilightItem();

		this->SelectDropTarget(NULL);

		//
		// 여기에 할일을 심어야 한다..
		CPoint pt_screen(point);
		ClientToScreen(&pt_screen);

		CWnd* pDropWnd = WindowFromPoint(pt_screen);
		
		m_callback->OnDragEnd(this, m_hDragItem, pDropWnd, pt_screen);
		
		m_hDragItem = NULL;
		
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}


void i3MFCTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM item = this->GetSelectedItem();
	void*     item_data = (item) ? reinterpret_cast<void*>(this->GetItemData(item)) : NULL;
	
	if (item != nullptr)
		m_callback->OnLButtonDown(this, item, item_data);

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void i3MFCTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint pt_screen(point);
	ClientToScreen(&pt_screen);

	m_callback->OnLButtonDblClk(this, pt_screen);

	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void i3MFCTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_callback->OnContextMenu(this, point);
}

void i3MFCTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	SetFocus();		
//	CTreeCtrl::OnRButtonDown(nFlags, point);
// 기본클래스 함수호출을 제거해야, OnContextMenu가 잘 작동한다..
}


void i3MFCTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	bool apply = true;

	m_callback->OnEndLabelEdit(this, 
		pTVDispInfo->item.hItem, 
		pTVDispInfo->item.pszText, apply);

	*pResult = apply;
}

HTREEITEM	i3MFCTreeCtrl::FindSiblingItemByName( HTREEITEM hStart, const wchar_t* szItemName)
{

	for (  ; hStart != NULL ; hStart = this->GetNextSiblingItem(hStart) )
	{
		CString strTreeName = GetItemText(hStart);
		if ( strTreeName == szItemName )
			return hStart;
	}

	return NULL;	
}

namespace
{
	HTREEITEM FindOrCreateItemByNameList_Traverse( i3MFCTreeCtrl* ctrl, HTREEITEM hParent, 
		const i3::vector<i3::wstring>& nameList, int iLevel, i3MFCTreeCtrl_FindCallback* cb)
	{
		const int num_nameList = int(nameList.size() );
		if ( iLevel >= num_nameList )
			return hParent;

		bool bLastLevel = ( iLevel + 1 == num_nameList );
		
		CString str; 

		const i3::wstring& target_str = nameList[iLevel];
		
		for (HTREEITEM item = ctrl->GetChildItem(hParent) ; item != NULL; item = ctrl->GetNextSiblingItem(item) )
		{
			str = ctrl->GetItemText( item );
			
			i3::const_wchar_range rng( LPCWSTR(str) , LPCWSTR(str) + str.GetLength() );

			if ( i3::generic_is_equal( rng, target_str ) )
			{
				if (bLastLevel)
				{
					cb->OnAlreadyItemCreated(ctrl, target_str, hParent, item, iLevel);
				}

				cb->OnTraverse(ctrl, target_str, hParent, item, iLevel );
				return FindOrCreateItemByNameList_Traverse(ctrl, item, nameList, ++iLevel, cb);
			}
		}
		
		HTREEITEM new_item = ctrl->AddItem( hParent, target_str.c_str(), 0);
		
		cb->OnNewItem(ctrl, target_str, hParent, new_item, iLevel);
		cb->OnTraverse(ctrl, target_str, hParent, new_item, iLevel);
		return FindOrCreateItemByNameList_Traverse(ctrl, new_item, nameList, ++iLevel, cb);
		// 없으면 생성하고 계속 진행..
	}

	HTREEITEM FindOrRenameItemByNameList_Traverse(i3MFCTreeCtrl* ctrl, HTREEITEM hParent, 
		const i3::vector<i3::wstring>& nameList, const i3::wstring& new_name, int iLevel, i3MFCTreeCtrl_FindCallback* cb)
	{
		const int num_nameList = int(nameList.size() );
		if ( iLevel >= num_nameList )
			return hParent;

		bool bLastLevel = ( iLevel + 1 == num_nameList );

		CString str; 

		const i3::wstring& target_str = nameList[iLevel];

		for (HTREEITEM item = ctrl->GetChildItem(hParent) ; item != NULL; item = ctrl->GetNextSiblingItem(item) )
		{
			str = ctrl->GetItemText( item );

			i3::const_wchar_range rng( LPCWSTR(str) , LPCWSTR(str) + str.GetLength() );

			if ( i3::generic_is_equal( rng, target_str ) )
			{
				if ( bLastLevel )
				{
					ctrl->SetItemText(item, new_name.c_str());
					cb->OnRenameItem( ctrl, target_str, new_name, hParent, item, iLevel );
				}

				cb->OnTraverse(ctrl, target_str, hParent, item, iLevel );
				return FindOrRenameItemByNameList_Traverse(ctrl, item, nameList, new_name, ++iLevel, cb);
			}
			else
			if ( bLastLevel )
			{
				if ( i3::generic_is_equal( rng, new_name ) )		// 이미 새 이름으로 교체된 상태..
				{
					cb->OnAlreadyItemRenamed( ctrl, target_str, new_name, hParent, item, iLevel);
					cb->OnTraverse(ctrl, new_name, hParent, item, iLevel );

					return FindOrRenameItemByNameList_Traverse(ctrl, item, nameList, new_name, ++iLevel, cb);
				}
			}
		}

		HTREEITEM new_item = ctrl->AddItem( hParent, target_str.c_str(), 0);
		cb->OnNewItem(ctrl, target_str, hParent, new_item, iLevel);
		
		if ( bLastLevel )
		{
			ctrl->SetItemText(new_item, new_name.c_str());
			cb->OnRenameItem(ctrl, target_str, new_name, hParent, new_item, iLevel);
		}
			
		cb->OnTraverse(ctrl, new_name, hParent, new_item, iLevel);
		
		return FindOrRenameItemByNameList_Traverse(ctrl, new_item, nameList, new_name, ++iLevel, cb);
		// 없으면 생성하고 계속 진행..
	}
		

	BOOL DeleteItemByNameList_Traverse( i3MFCTreeCtrl* ctrl, HTREEITEM hParent, 
		const i3::vector<i3::wstring>& nameList, int iLevel, i3MFCTreeCtrl_FindCallback* cb, bool ForceCreateItem)
	{
		const int num_nameList = int(nameList.size() );
		if ( iLevel >= num_nameList )
			return FALSE;

		bool bLastLevel = ( iLevel + 1 == num_nameList );

		CString str; 
		const i3::wstring& target_str = nameList[iLevel];

		for (HTREEITEM item = ctrl->GetChildItem(hParent) ; item != NULL; item = ctrl->GetNextSiblingItem(item) )
		{
			str = ctrl->GetItemText( item );

			i3::const_wchar_range rng( LPCWSTR(str) , LPCWSTR(str) + str.GetLength() );

			if ( i3::generic_is_equal( rng, target_str ) )
			{
				cb->OnTraverse(ctrl, target_str, hParent, item, iLevel );
				
				if (bLastLevel)
				{
					cb->BeforeDeleteItem(ctrl, target_str, hParent, item, iLevel);
					ctrl->DeleteItem(item);
					return TRUE;				// TRUE로 빨리 끊고, 더 이상 진행하면 안된다.
				}

				return DeleteItemByNameList_Traverse(ctrl, item, nameList, ++iLevel, cb, ForceCreateItem);
			}
		}
		
		if ( ForceCreateItem == false || bLastLevel )		// 못찾으면 못찾은 상태에서 못찾는 콜백때리고, 즉시 FALSE 리턴처리한다..
		{
			cb->OnNotFoundItemToDelete(ctrl, target_str, hParent, iLevel);
			return FALSE;
		}
		
		HTREEITEM new_item = ctrl->AddItem( hParent, target_str.c_str(), 0);
		
		cb->OnNewItem(ctrl, target_str, hParent, new_item, iLevel);
		cb->OnTraverse(ctrl, target_str, hParent, new_item, iLevel);

		return DeleteItemByNameList_Traverse(ctrl, new_item, nameList, ++iLevel, cb, ForceCreateItem);
		// 없으면 생성하고 계속 진행..
	}




}

BOOL	i3MFCTreeCtrl::DeleteItemByNameList( const i3::vector<i3::wstring>& nameList, i3MFCTreeCtrl_FindCallback* cb, bool ForceCreateItem)
{
	return DeleteItemByNameList_Traverse(this, this->GetRootItem(), nameList, 0, cb, ForceCreateItem);	
}


HTREEITEM	i3MFCTreeCtrl::FindOrCreateItemByNameList( const i3::vector<i3::wstring>& nameList, i3MFCTreeCtrl_FindCallback* cb)
{
	// 루트는 검색에서 제외한다..
	return FindOrCreateItemByNameList_Traverse( this, this->GetRootItem(), nameList, 0, cb);
}

HTREEITEM   i3MFCTreeCtrl::FindOrRenameItemByNameList( const i3::vector<i3::wstring>& nameList, const i3::wstring& new_name, 
													  i3MFCTreeCtrl_FindCallback* cb)
{
	return FindOrRenameItemByNameList_Traverse(this, this->GetRootItem(), nameList, new_name, 0, cb);
}

HTREEITEM	i3MFCTreeCtrl::FindOrCreateItemByNameList( const i3::wstring& path, i3MFCTreeCtrl_FindCallback* cb)
{
	i3::vector<i3::wstring> tree_names;
	i3::strtok_fast(tree_names, path, L"\\/");
	return FindOrCreateItemByNameList(tree_names, cb);
}

HTREEITEM   i3MFCTreeCtrl::FindOrRenameItemByNameList( const i3::wstring& path, const i3::wstring& new_name, i3MFCTreeCtrl_FindCallback* cb)
{
	i3::vector<i3::wstring> tree_names;
	i3::strtok_fast(tree_names, path, L"\\/");
	return FindOrRenameItemByNameList(tree_names, new_name, cb);
}

BOOL		i3MFCTreeCtrl::DeleteItemByNameList( const i3::wstring& path, i3MFCTreeCtrl_FindCallback* cb, bool ForceCreateItem )
{
	i3::vector<i3::wstring> tree_names;
	i3::strtok_fast(tree_names, path, L"\\/");
	return DeleteItemByNameList(tree_names, cb, ForceCreateItem);
}

void i3MFCTreeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	this->SetFocus();
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void i3MFCTreeCtrl::OnNMCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_callback->OnNMCustomDraw(this, pNMHDR, pResult);
}

namespace
{
	HTREEITEM GetNextTreeItem(const CTreeCtrl& treeCtrl, HTREEITEM hItem)
	{
		// has this item got any children
		if (treeCtrl.ItemHasChildren(hItem))
		{
			return treeCtrl.GetNextItem(hItem, TVGN_CHILD);
		}
		else if (treeCtrl.GetNextItem(hItem, TVGN_NEXT) != NULL)
		{
			// the next item at this level
			return treeCtrl.GetNextItem(hItem, TVGN_NEXT);
		}
		else
		{
			// return the next item after our parent
			hItem = treeCtrl.GetParentItem(hItem);
			if (hItem == NULL)
			{
				// no parent
				return NULL;
			}
			while (hItem != treeCtrl.GetRootItem() && treeCtrl.GetNextItem(hItem, TVGN_NEXT) == NULL)
			{
				hItem = treeCtrl.GetParentItem(hItem);
			}
			// next item that follows our parent
			return treeCtrl.GetNextItem(hItem, TVGN_NEXT);
		}
	};
}

namespace CTreeCtrlUtil
{
	void ExpandTreeAllItems(CTreeCtrl& treeCtrl)
	{
		HTREEITEM hRootItem = treeCtrl.GetRootItem();
		HTREEITEM hItem = hRootItem;

		while (hItem)
		{
			if (treeCtrl.ItemHasChildren(hItem))
			{
				treeCtrl.Expand(hItem, TVE_EXPAND);
			}
			hItem = GetNextTreeItem(treeCtrl, hItem);
		}
	}

	HTREEITEM FindTreeItem(CTreeCtrl& treeCtrl, const std::wstring& wstrItemName)
	{
		HTREEITEM hRootItem = treeCtrl.GetRootItem();
		HTREEITEM hItem = hRootItem;

		std::wstring wstrTreeItem;
		while (hItem)
		{
			wstrTreeItem = treeCtrl.GetItemText(hItem).GetString();

			if (i3::generic_is_iequal(wstrItemName.c_str(), wstrTreeItem))
				return hItem;

			hItem = GetNextTreeItem(treeCtrl, hItem);
		}

		return NULL;
	}

	HTREEITEM FindTreeItem_Path(CTreeCtrl& treeCtrl, const std::wstring& wstrPath)
	{
		if (wstrPath.empty())
			return NULL;

		HTREEITEM hRootItem = treeCtrl.GetRootItem();
		HTREEITEM hItem = hRootItem;

		std::vector<std::wstring> wstrTokenList;
		GetPathTokenList(wstrPath, wstrTokenList);

		std::vector<std::wstring> wstrReverseTokenList;
		std::reverse_copy(wstrTokenList.begin(), wstrTokenList.end(), std::back_inserter(wstrReverseTokenList));

		std::wstring wstrTreeItem;
		HTREEITEM hItemCheck;
		bool checkOK = true;

		while (hItem)
		{	
			hItemCheck = hItem;
			checkOK = true;

			for (size_t i = 0; i < wstrReverseTokenList.size(); i++)	// 경로 토큰 여러개.
			{
				if (hItemCheck == NULL || hItemCheck == hRootItem)
				{
					checkOK = false;
					break;
				}

				wstrTreeItem = treeCtrl.GetItemText(hItemCheck);
				const size_t pos = wstrTreeItem.find(L",");
				if (pos != std::wstring::npos)
					wstrTreeItem.erase(pos, wstrTreeItem.length());

				if (!i3::generic_is_iequal(wstrReverseTokenList[i], wstrTreeItem))
				{
					checkOK = false;
					break;
				}

				hItemCheck = treeCtrl.GetParentItem(hItemCheck);
			}

			if (checkOK)
				return hItem;

			hItem = GetNextTreeItem(treeCtrl, hItem);
		}

		return NULL;
	}

	std::string ExtractFileRelPathByItem(CTreeCtrl* ctrl, HTREEITEM hItem)
	{
		if (hItem == NULL)
			return std::string();

		std::stack<CString> itemNameStack;

		HTREEITEM hRootItem = ctrl->GetRootItem();
		HTREEITEM hParentItem = ctrl->GetParentItem(hItem);

		CString wstrItemName = ctrl->GetItemText(hItem);
		itemNameStack.push(wstrItemName);

		CString wstr;
		while (hParentItem != NULL && hParentItem != hRootItem)
		{
			wstr = ctrl->GetItemText(hParentItem);
			itemNameStack.push(wstr);
			hParentItem = ctrl->GetParentItem(hParentItem);
		}

		std::wstring wstrRelPath;
		while (!itemNameStack.empty())
		{
			wstrRelPath += itemNameStack.top();
			wstrRelPath += L"\\";
			itemNameStack.pop();
		}

		if (!wstrRelPath.empty())
			wstrRelPath.erase(wstrRelPath.end() - 1);	// 마지막 L"\\" 제거.

		std::string strRelPath;
		i3::utf16_to_mb(wstrRelPath, strRelPath);

		return strRelPath;
	}
}