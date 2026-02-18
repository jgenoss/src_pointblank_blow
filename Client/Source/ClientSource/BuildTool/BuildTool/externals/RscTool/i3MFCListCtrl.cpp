#include "stdafx.h"
#include "i3MFCListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(i3MFCListCtrl, CMFCListCtrl)

BEGIN_MESSAGE_MAP(i3MFCListCtrl, CMFCListCtrl)

	ON_NOTIFY_REFLECT(NM_DBLCLK, &i3MFCListCtrl::OnDblClk)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &i3MFCListCtrl::OnLvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, &i3MFCListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &i3MFCListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &i3MFCListCtrl::OnLvnEndlabeledit)
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


namespace
{
	struct i3MFCListCtrlCallback_Null : i3MFCListCtrlCallback
	{

	};

	i3MFCListCtrlCallback_Null	g_callback_null;
}


i3MFCListCtrl::i3MFCListCtrl() : m_callback(&g_callback_null), m_droptarget_index(-1),
m_pListDragImage(NULL)
{
		
}

i3MFCListCtrl::~i3MFCListCtrl()
{
	delete m_pListDragImage;
}

BOOL	i3MFCListCtrl::Create(CWnd* pParent, UINT nID)
{
	return i3MFCListCtrl::CreateEx(pParent, nID, 0);
}

BOOL	i3MFCListCtrl::CreateEx(CWnd* pParent, UINT nID, DWORD dwStyle, DWORD dwExStyle)
{
	BOOL bRes = CMFCListCtrl::Create(WS_CHILD|WS_VISIBLE |
		                             WS_CLIPCHILDREN| //WS_CLIPSIBLINGS| 
									 LVS_REPORT //| LVS_NOCOLUMNHEADER // | LVS_SHOWSELALWAYS 
									 | dwStyle, CRect(0,0,0,0), pParent, nID);


	::SetWindowTheme(this->GetSafeHwnd(), L"explorer", 0);

	this->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | dwExStyle);

	this->ModifyStyle(0, LVS_SHAREIMAGELISTS);
	this->SetImageList(CImageList::FromHandle(GetShellImageList_Large()), LVSIL_NORMAL);
	this->SetImageList(CImageList::FromHandle(GetShellImageList_Small()), LVSIL_SMALL);

	return bRes;	
}

void	i3MFCListCtrl::HideHeader()
{
	this->ModifyStyle(0, LVS_NOCOLUMNHEADER, 0);
}

void	i3MFCListCtrl::SetSize( int cx, int cy)
{
	this->SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);	
}

HIMAGELIST i3MFCListCtrl::GetShellImageList_Large()
{
	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) == 0)
	{
		return NULL;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);
	return hImageList;

}

HIMAGELIST i3MFCListCtrl::GetShellImageList_Small()
{
	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory(szWinDir, MAX_PATH) == 0)
	{
		return NULL;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo(szWinDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX |SHGFI_SMALLICON);
	return hImageList;

}

void	i3MFCListCtrl::AddItem( const wchar_t* strName, void* Data )
{
	LVITEM lvItem;
	::memset(&lvItem, 0, sizeof(lvItem));

	lvItem.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;

	lvItem.lParam = LPARAM(Data);
	lvItem.iItem = this->GetItemCount();
	lvItem.pszText = const_cast<wchar_t*>(strName);
	lvItem.iImage  = 0;
	
	this->InsertItem(&lvItem);
}


BOOL i3MFCListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
//	return TRUE;
	return CMFCListCtrl::OnEraseBkgnd(pDC);
}

void i3MFCListCtrl::OnDblClk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	int nItem = GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);	// 멀티셀렉션에 대해서는 좀더 나중에 처리하고 일단 선택된 1개만..

	m_callback->OnDblClk(this, nItem);
	
	*pResult = 0;
}

void	i3MFCListCtrl::SetDropTarget(int idx)
{
	if (m_droptarget_index != idx )
	{
		if (m_droptarget_index != -1)
			this->SetItemState(m_droptarget_index, 0, LVIS_DROPHILITED);
		if (idx != -1)
			this->SetItemState(idx, LVIS_DROPHILITED, LVIS_DROPHILITED);
		m_droptarget_index = idx;
	}
}

int		i3MFCListCtrl::GetDropTarget() const
{
	return m_droptarget_index;
}

int		i3MFCListCtrl::GetFirstSelectedItem() const
{
	POSITION pos = this->GetFirstSelectedItemPosition();
	int idx = -1;
	if (pos != NULL)
		idx = this->GetNextSelectedItem(pos);
	return idx;
}

BOOL	i3MFCListCtrl::GetAllSelectedItems(i3::vector<int>& inout) const
{
	POSITION pos = this->GetFirstSelectedItemPosition();
	
	if (pos == NULL )
		return FALSE;

	inout.push_back(this->GetNextSelectedItem(pos));
	
	while(pos != NULL)
		inout.push_back(this->GetNextSelectedItem(pos));
	
	return TRUE;
}

void	i3MFCListCtrl::UnselectAll()
{
	i3::vector<int> sel_list;
	if ( i3MFCListCtrl::GetAllSelectedItems(sel_list) )
	{
		const size_t num = sel_list.size();
		for( size_t i = 0 ; i < num ; ++i )
		{
			this->SetItemState(sel_list[i], 0, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}



void i3MFCListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if (m_pListDragImage)
	{
		m_pListDragImage->DeleteImageList();
		delete m_pListDragImage;
	}
	
	// 여기서부터 다시 할것..
	if ( this->GetAllSelectedItems(m_DragIndexList) == FALSE)
		return;

	POINT pt;

	m_pListDragImage = this->CreateDragImageEx(m_DragIndexList, pt);
	
	CBitmap bitmap, bitmapmask;
		
	m_pListDragImage->BeginDrag(0, 
		CPoint(pNMLV->ptAction.x - pt.x, pNMLV->ptAction.y - pt.y));

	m_pListDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

	SetCapture();


	*pResult = 0;
}

void i3MFCListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCListCtrl::OnMouseMove(nFlags, point);

	if (m_pListDragImage)
	{
		CPoint p = point;
		ClientToScreen(&p);

		m_pListDragImage->DragMove(p);

		m_pListDragImage->DragShowNolock(FALSE);
		
	
		CWnd* pDropWnd = WindowFromPoint(p);

		if (pDropWnd == this)
		{
			int DropIdx = this->HitTest(point);
			
			if ( DropIdx != this->GetDropTarget() )
			{
				this->SetDropTarget(DropIdx);
			}
		}
		else
		{
			this->SetDropTarget(-1);
			m_callback->OnDragMove(this, m_DragIndexList, pDropWnd, p);
		}
		
		m_pListDragImage->DragShowNolock(TRUE);

	}

}

CImageList*	i3MFCListCtrl::CreateDragImageEx(const i3::vector<int>& sel_list, POINT& out_offset)
{

	DWORD dwStyle = DWORD(GetWindowLongPtr(this->GetSafeHwnd(), GWL_STYLE)) & LVS_TYPEMASK;
	
	const size_t num_sel = sel_list.size();
	
	CRect rcComplete(0,0,0,0);
	
	GetItemRect(sel_list[0], rcComplete, LVIR_BOUNDS);	// 첫번째것은 디폴트로 담음..

	CRect singleRect;

	for (size_t i = 1 ; i < num_sel ; ++i )		// 시작인덱스 1 // 첫번째것은 아까 처리됨..
	{
		int nIdx = sel_list[i];
		GetItemRect(nIdx, singleRect, LVIR_BOUNDS);
		rcComplete.UnionRect(rcComplete, singleRect);		// rect 확장..
	}
	
	CClientDC	dc(this);
	CDC			mem_dc;
	CBitmap		bitmap;

	if (!mem_dc.CreateCompatibleDC(&dc) )
		return NULL;

	if (!bitmap.CreateCompatibleBitmap(&dc, rcComplete.Width(), rcComplete.Height() ) )
		return NULL;
	
	CBitmap* old_bitmap = mem_dc.SelectObject(&bitmap);

	mem_dc.FillSolidRect(0,0,rcComplete.Width(), rcComplete.Height(), RGB(0,255,0) );
	
	// 텍스트 출력을 위한 폰트 ..
	CFont* pFont = this->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfQuality = NONANTIALIASED_QUALITY;		// 일단 이걸로...

	CFont newFont;
	newFont.CreateFontIndirect(&lf);
	CFont* oldFont = mem_dc.SelectObject(&newFont);
	//

	TCHAR str_maxPath[MAX_PATH];

	for (size_t i = 0 ; i < num_sel ; ++i)
	{
		int nIdx = sel_list[i];
		LVITEM item = {0};
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
		item.iItem = nIdx;
		item.pszText = str_maxPath;
		item.cchTextMax = MAX_PATH;
		item.stateMask = LVIS_OVERLAYMASK;
		this->GetItem(&item);

		// 아이콘 그림..
		CImageList* single_img_list = 
			this->GetImageList( (dwStyle == LVS_ICON) ? LVSIL_NORMAL : LVSIL_SMALL );
		
		// 아이콘 존재시..
		if (single_img_list)
		{
			CRect rectIcon;
			this->GetItemRect(nIdx, rectIcon, LVIR_ICON);

			IMAGEINFO info;
			single_img_list->GetImageInfo(item.iImage, &info);
			
			int img_width = info.rcImage.right - info.rcImage.left;
			int img_height= info.rcImage.bottom - info.rcImage.top;


			CDC mem_dc2;
			CBitmap bitmap2;
			mem_dc2.CreateCompatibleDC(&dc);
			bitmap2.CreateCompatibleBitmap(&dc, img_width, img_height );

			CBitmap* old_bitmap2 = mem_dc2.SelectObject(&bitmap2);

			mem_dc2.FillSolidRect(0,0,img_width, img_height, RGB(0,255,0));
			
			single_img_list->Draw(&mem_dc2, item.iImage, CPoint(0,0), 
				ILD_TRANSPARENT | ( item.state & LVIS_OVERLAYMASK ) );
			
			CPoint pt;
		
			pt.x = (  rectIcon.left - rcComplete.left 
				    + rectIcon.right - rcComplete.left ) / 2
					- img_width / 2;

			pt.y = (  rectIcon.top - rcComplete.top 
				    + rectIcon.bottom - rcComplete.top ) / 2
					- img_height / 2
					+ ((dwStyle == LVS_ICON) ? 2 : 0 );

			mem_dc.TransparentBlt(pt.x, pt.y, img_width, img_height,
				&mem_dc2, 0,0, img_width, img_height, RGB(0,255,0) );
			
			mem_dc2.SelectObject(old_bitmap2);

			bitmap2.DeleteObject();
		}
		// 텍스트 출력..

		CRect textRect;
		this->GetItemRect( nIdx, &textRect, LVIR_LABEL);

		textRect.top -= rcComplete.top - 2;
		textRect.bottom -= rcComplete.top + 1;
		textRect.left -= rcComplete.left - 2;
		textRect.right -= rcComplete.left;

		DWORD flag = DT_END_ELLIPSIS | DT_MODIFYSTRING;
		if (dwStyle == LVS_ICON )
			flag |= DT_CENTER | DT_WORDBREAK;
		
		mem_dc.DrawText(str_maxPath, -1, textRect, flag);
	}
	
	mem_dc.SelectObject(oldFont);
	mem_dc.SelectObject(old_bitmap);
	
	CImageList* result_img_list = new CImageList;

	result_img_list->Create(rcComplete.Width(), rcComplete.Height(), ILC_COLOR32| ILC_MASK, 0, 1);
	result_img_list->Add(&bitmap, RGB(0,255,0));
	bitmap.DeleteObject();

	out_offset.x = rcComplete.left;
	out_offset.y = rcComplete.top;

	return result_img_list;
}


void i3MFCListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_pListDragImage)
	{
		::ReleaseCapture();

		m_pListDragImage->DragLeave(this);
		m_pListDragImage->EndDrag();

		m_pListDragImage->DeleteImageList();
		delete m_pListDragImage;
		m_pListDragImage = NULL;
		
		this->SetDropTarget(-1);

		CPoint pt_screen(point);
		ClientToScreen(&pt_screen);

		CWnd* pDropWnd = WindowFromPoint(pt_screen);

		// 셀렉션 전체를 인수로 취해야함..
		
		m_callback->OnDragEnd(this, m_DragIndexList, pDropWnd, pt_screen);

		m_DragIndexList.clear();	
	}


	CMFCListCtrl::OnLButtonUp(nFlags, point);
}


void i3MFCListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	 LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_callback->OnClick(this, pNMItemActivate->iItem, pNMItemActivate->iSubItem);

	*pResult = 0;
}

// 아래 함수는 외부에서 엄청 자동 호출되는 함수이므로 조건분기를 잘 따져야함..

void i3MFCListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (pNMLV->uChanged & LVIF_STATE)
	{
		if ( pNMLV->uNewState & LVIS_SELECTED )
		{
			m_callback->OnSelected(this, pNMLV->iItem);
		}
		else
		if ( pNMLV->uOldState & LVIS_SELECTED )
		{
			m_callback->OnDeselected(this, pNMLV->iItem);
		}
		
	}




	*pResult = 0;
}


void i3MFCListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool apply = true;

	m_callback->OnEndLabelEdit(this, 
		pDispInfo->item.iItem, 
		pDispInfo->item.iSubItem,
		pDispInfo->item.pszText, apply);

	*pResult = apply;
}

void	i3MFCListCtrl::RemoveAllItems()		// DeleteAllItems 대용으로 일단 구현해둔다..
{
	this->SetRedraw(FALSE);

	const int count = this->GetItemCount();
	for ( int i = 0 ; i < count ; ++i )
	{	
		this->DeleteItem(count - 1 - i);
	}
		
	this->SetRedraw(TRUE);
}


void i3MFCListCtrl::OnContextMenu(CWnd* pWnd, CPoint point )
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_callback->OnContextMenu(this, point);

}

BOOL	i3MFCListCtrl::IsLastItemIndexVisible() const
{
	int num_idx = this->GetItemCount();
	if (num_idx == 0) return TRUE;			// FALSE -> TRUE로 변경..(아이템이 없더라도, 보이는게 가능함을 의미)

	int first = this->GetTopIndex();
	int last =  first + this->GetCountPerPage() + 1;
	if ( last > num_idx ) 
		last = num_idx;

	return ( num_idx-1 >= first &&  num_idx-1 < last );
}

void i3MFCListCtrl::SelectAll()
{
	DWORD dwStyle = this->GetStyle();
	if ( dwStyle & LVS_SINGLESEL )
		return;

	const int numItem = this->GetItemCount();

	for( int i = 0 ; i < numItem ; ++i )
	{
		this->SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
		
}

void i3MFCListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		switch( nChar)
		{
		case 'A':
			{
				if ( (nFlags & 0x4000) != 0x4000 )	// 최초 눌렸을때만..처리..
					this->SelectAll();
			}
		break;
		default:
		break;
		}
	}

	CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

