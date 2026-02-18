// i3TDKListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKListCtrl.h"
#include "i3TDKUpdateSystem.h"


// i3TDKListCtrl

IMPLEMENT_DYNAMIC(i3TDKListCtrl, CListCtrl)

BOOL i3TDKListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN)
	{
		if( pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE ||
			GetKeyState( VK_CONTROL))
		{
			::TranslateMessage( pMsg);
			::DispatchMessage( pMsg);
			return TRUE;
		}
	}
	else if ( pMsg->message == WM_MOUSEMOVE)
	{
		m_ToolTip.RelayEvent( pMsg);
	}

	return CListCtrl::PreTranslateMessage( pMsg);
}

BOOL i3TDKListCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID)
{
	//	항목의 데이터를 직접 관리하기 위해 LVS_OWNERDATA를 사용.
	dwStyle |= WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_OWNERDATA;  

	return CListCtrl::Create( dwStyle, rect, pParentWnd, ID);
}

void i3TDKListCtrl::AddExtenedStyle( DWORD dwAddStyle)
{
	SetExtendedStyle( GetExtendedStyle() | dwAddStyle );
}

void i3TDKListCtrl::ClearAll( void)
{
	m_pSelectedSubItem = nullptr;
	
	ClearItem();	
	ClearColumn();	
}

void i3TDKListCtrl::ClearItem( void)
{
	
	for( size_t nItem = 0; nItem < m_SubItemList.size(); nItem++)
	{
		i3::vector<i3TDKPropertySubItem*>& ItemList = m_SubItemList[nItem];

		//	SubItem 삭제
		for( size_t nSubItem = 0; nSubItem < ItemList.size(); nSubItem++)
		{
			i3TDKPropertySubItem * pItem = ItemList[nSubItem];

			if( pItem)
			{
				pItem->OnRelease();
				I3_SAFE_RELEASE( pItem);
			}
		}
		ItemList.clear();
	}
	m_SubItemList.clear();

	DeleteAllItems();
}

void i3TDKListCtrl::ClearColumn( void)
{
	for( INT32 i = 0; i < GetColumnTitleCount(); i++)
	{
		ColumnTitleInfo * pColumn = m_ColumnList[i];
		I3MEM_SAFE_FREE( pColumn);
		DeleteColumn(0);
	}
	m_ColumnList.clear();	
}

void i3TDKListCtrl::SetLogFont( const LOGFONT * pLogFont)
{
	if( !m_hWnd)
	{
		I3TRACE("Warning! Not yet created i3TDKListCtrl.\n");
		return;
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect( pLogFont);
	SetFont( &m_Font);
}

INT32 i3TDKListCtrl::AddColumnTitle( const TCHAR* pszColumn, INT32 nWidthRate, UINT32 nFmt)
{
	CRect rt;
	GetClientRect( &rt);

	INT32 nWitdh = (INT32)( rt.Width() * ((REAL32) nWidthRate * 0.01f));

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = nFmt;
	col.cx = nWitdh;
	col.pszText = (LPTSTR) pszColumn;

	//	컬럼 추가
	InsertColumn( GetColumnTitleCount(), &col);

	ColumnTitleInfo * pNewCol = (ColumnTitleInfo *) i3MemAlloc( sizeof( ColumnTitleInfo));
	i3::safe_string_copy( pNewCol->_szName, pszColumn, 256);
	pNewCol->_nWidthRate = nWidthRate;

	m_ColumnList.push_back(pNewCol);

	return GetColumnTitleCount();
}

INT32 i3TDKListCtrl::DeleteColumnTitle( INT32 nColumnIdx)
{
	if( DeleteColumn( nColumnIdx))
	{
		ColumnTitleInfo * pColumn = m_ColumnList[nColumnIdx];
		
		if( pColumn)
		{
			I3MEM_SAFE_FREE( pColumn);
			m_ColumnList.erase( m_ColumnList.begin() + nColumnIdx);			
		}
	}
	
	return GetColumnTitleCount();
}

INT32 i3TDKListCtrl::NewSubItem( INT32 nAddCount, INT32 nStartIdx)
{	
	INT32 nLastIndex = -1;
	INT32 nAddIdx = 0;
	INT32 nRowCount = GetItemCount();
	INT32 nItemCount = 0;

	if( GetColumnTitleCount() == 0)
	{
		I3PRINTLOG(I3LOG_FATAL, "[i3TDKListCtrl::NewSubItem] warning! Column is zero. need more columns.");

		return -1;
	}

	for( nItemCount = 0; nItemCount < nAddCount; nItemCount++)
	{
	//	List * pColList = List::new_object();
		i3::vector<i3TDKPropertySubItem *> ColList(  GetColumnTitleCount() );
		
		if( nStartIdx == -1 || nRowCount < nStartIdx)
		{
			nAddIdx = GetItemCount();
			m_SubItemList.push_back( i3::vector<i3TDKPropertySubItem *>() );					//	뒤부터 추가 Index			
			i3::swap(m_SubItemList.back(), ColList);
		}
		else
		{
			nAddIdx = nStartIdx + nItemCount;
			m_SubItemList.insert( m_SubItemList.begin() + nAddIdx, i3::vector<i3TDKPropertySubItem*>());
			i3::swap( m_SubItemList[nAddIdx], ColList);
//			m_SubItemList.Insert( nAddIdx, (void *) pColList);		//	중간부터 추가 index			
		}

		//	ListCtrl에 아이템 추가
		nLastIndex = InsertItem( nAddIdx, _T("") );	
	}

	return nLastIndex;
}

BOOL i3TDKListCtrl::SetSubItem( INT32 nRowIdx, INT32 nColumnIdx, i3TDKPropertySubItem * pSubItem)
{
	I3ASSERT( nRowIdx >= 0 && nColumnIdx >= 0);

	if( pSubItem)   
	{
		if( (INT32) m_SubItemList.size() <= nRowIdx)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Error! Invalid Row Index! (Set: %d / Cnt: %d)", (INT32)m_SubItemList.size(), nRowIdx);
			return FALSE;
		}

		i3::vector<i3TDKPropertySubItem *>& ColList = m_SubItemList[nRowIdx];		//	Get SubItem List

		
		if( (INT32)ColList.size() <= nColumnIdx)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Error! Invalid Column Index! (Set: %d / Cnt: %d)", (INT32)ColList.size(), nColumnIdx);
			return FALSE;
		}

		i3TDKPropertySubItem * pPrevItem = ColList[nColumnIdx];
		I3_SAFE_RELEASE( pPrevItem);
		I3_SAFE_ADDREF( pSubItem);
		i3::vu::set_value_force(ColList, nColumnIdx, pSubItem);
		//pColList->SetItem( nColumnIdx, pSubItem);
		
		// LVS_OWNERDATA를 사용하므로 SetItemText를 사용하지 않는다.
		//return SetItemText( nRowIdx, nColumnIdx, pSubItem->GetLabel());	
		return TRUE;

	}

	return FALSE;
}

i3TDKPropertySubItemStatic * i3TDKListCtrl::SetSubItemStatic( SubItemStaticInitStruct * pInitStruct)
{
	i3TDKPropertySubItemStatic * pNewItem = i3TDKPropertySubItemStatic::new_object();

	pNewItem->SetID( pInitStruct->_id);
	pNewItem->SetStyle( pInitStruct->_style);
	pNewItem->SetType( pInitStruct->_type);
	pNewItem->SetFontColor( pInitStruct->_font_color);
	pNewItem->SetLabel( pInitStruct->_label);
	pNewItem->SetUserData( pInitStruct->_data);
	pNewItem->SetRowIndex( pInitStruct->_row);
	pNewItem->SetColumnIndex( pInitStruct->_column);

	SetSubItem( pInitStruct->_row, pInitStruct->_column, pNewItem);

	return pNewItem;
}

i3TDKPropertySubItemEdit * i3TDKListCtrl::SetSubItemEdit( SubItemEditInitStruct * pInitStruct)
{
	i3TDKPropertySubItemEdit * pNewItem = i3TDKPropertySubItemEdit::new_object();

	pNewItem->SetID( pInitStruct->_id);
	pNewItem->SetStyle( pInitStruct->_style);
	pNewItem->SetType( pInitStruct->_type);
	pNewItem->SetFontColor( pInitStruct->_font_color);
	pNewItem->SetLabel( pInitStruct->_label);
	pNewItem->SetUserData( pInitStruct->_data);
	pNewItem->SetRowIndex( pInitStruct->_row);
	pNewItem->SetColumnIndex( pInitStruct->_column);

	SetSubItem( pInitStruct->_row, pInitStruct->_column, pNewItem);

	return pNewItem;
}

i3TDKPropertySubItemComboBox * i3TDKListCtrl::SetSubItemComboBox( SubItemComboBoxInitStruct * pInitStruct)
{
	i3TDKPropertySubItemComboBox * pNewItem = i3TDKPropertySubItemComboBox::new_object();

	pNewItem->SetID( pInitStruct->_id);
	pNewItem->SetStyle( pInitStruct->_style);
	pNewItem->SetType( pInitStruct->_type);
	pNewItem->SetFontColor( pInitStruct->_font_color);
	pNewItem->SetLabel( pInitStruct->_label);
	pNewItem->SetUserData( pInitStruct->_data);
	pNewItem->SetRowIndex( pInitStruct->_row);
	pNewItem->SetColumnIndex( pInitStruct->_column);

	SetSubItem( pInitStruct->_row, pInitStruct->_column, pNewItem);

	return pNewItem;
}

void i3TDKListCtrl::UpdateSubItem( SubItemUpdateInfo * pInfo, BOOL bMessageToParent)
{	
	INT32 nRowIdx = pInfo->_nRowIdx;
	INT32 nColumnIdx = pInfo->_nColumnIdx;

	//I3ASSERT( nRowIdx >= 0 && nColumnIdx >= 0);
	if( (INT32) m_SubItemList.size() <= nRowIdx)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Update Error! Invalid Row Index! (Set: %d / Cnt: %d)", (INT32) m_SubItemList.size(), nRowIdx);
		return;
	}

	i3::vector<i3TDKPropertySubItem*>& ColList = m_SubItemList[nRowIdx];			//	Get SubItem List

	if( (INT32)ColList.size() <= nColumnIdx)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Update Error! Invalid Column Index! (Set: %d / Cnt: %d)", (INT32)ColList.size(), nColumnIdx);
		return;
	}

	i3TDKPropertySubItem * pSubItem = ColList[nColumnIdx];	//	Get SubItem
	if( pSubItem)		
	{
		//	SubItem 업데이트 (EditControl과 Item의 동기화)
		pSubItem->OnUpdate( pInfo);

		// LVS_OWNERDATA를 사용하므로 SetItemText를 사용하지 않는다.
		//SetItemText( nRowIdx, nColumnIdx, pSubItem->GetLabel());		//	ListCtrl Label 갱신				
	
		//	TRUE이면 부모에게 해당 SubItem을 업데이트 전달
		if( bMessageToParent)
			i3TDK::Update(m_hWnd, I3_TDK_UPDATE_EDIT, pSubItem, i3TDKPropertySubItem::static_meta());

		RedrawItems( pSubItem->GetRowIndex(), pSubItem->GetRowIndex());
	}

}

BOOL i3TDKListCtrl::DeleteSubItem( INT32 nRowIdx)
{
	
	I3ASSERT( nRowIdx >= 0);

	i3::vector<i3TDKPropertySubItem*>& ItemList = m_SubItemList[nRowIdx];		//	Get SubItem List
		
	//	SubItem 삭제
	for(size_t nSubItem = 0; nSubItem < ItemList.size(); nSubItem++)
	{
		i3TDKPropertySubItem * pItem = ItemList[nSubItem];
		pItem->OnRelease();

		I3_SAFE_RELEASE( pItem);
	}
	ItemList.clear();

	m_SubItemList.erase(m_SubItemList.begin() + nRowIdx);

	return DeleteItem( nRowIdx);
}

i3TDKPropertySubItem * i3TDKListCtrl::GetSubItem( INT32 nRowIdx, INT32 nColumnIdx)
{
	I3ASSERT( nRowIdx >= 0 && nColumnIdx >= 0);

	if( (INT32)m_SubItemList.size() - 1 < nRowIdx)	return nullptr;

	//	각 Column에 담긴 SubItem List
	i3::vector<i3TDKPropertySubItem*>& SubItemList = m_SubItemList[nRowIdx];
	
	return SubItemList[nColumnIdx];

}

const TCHAR * i3TDKListCtrl::GetItemLabel( INT32 nRowIdx, INT32 nColumnIdx)
{
	I3ASSERT( nRowIdx >= 0 && nColumnIdx >= 0);

	i3TDKPropertySubItem * pSubItem = GetSubItem( nRowIdx, nColumnIdx);

	if( pSubItem)
		return pSubItem->GetLabel();
	else
		return nullptr;
}

BEGIN_MESSAGE_MAP(i3TDKListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_DRAWITEM()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnLvnBeginlabeledit)
	ON_WM_MEASUREITEM()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnLvnGetInfoTip)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	//ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()



// i3TDKListCtrl message handlers

int i3TDKListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//	Default Font Setting
	LOGFONT logFont;
	memset( &logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight			= 11;
	logFont.lfWeight			= FW_DONTCARE;
	logFont.lfItalic			= FALSE;
	logFont.lfCharSet			= DEFAULT_CHARSET;
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	i3::generic_string_copy( logFont.lfFaceName, _T("굴림") );

	SetLogFont( &logFont);

	//	Extend style set
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT | LVS_EX_SUBITEMIMAGES | 
					 LVS_EX_INFOTIP;

	SetExtendedStyle( dwExStyle  );

	EnableToolTips();

	//	TDK 업데이트 시스템 등록
	i3TDK::RegisterUpdate( GetParent()->GetSafeHwnd(), i3TDKPropertySubItem::static_meta(), I3_TDK_UPDATE_EDIT);

	m_ToolTip.Create( this);
	//m_ToolTip.AddTool( this);
	m_ToolTip.SetDelayTime( 2000);
	//m_ToolTip.SetTextFont( &logFont);
	m_ToolTip.SetFont( &m_Font);
	m_ToolTip.Activate(TRUE);

	return 0;
}

void i3TDKListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	for( INT32 i = 0; i < GetColumnTitleCount(); i++)
	{
		ColumnTitleInfo * pColumn = m_ColumnList[i];
	
		//REAL32 nOldWidth = GetColumnWidth( i);
		//pColumn->_nWidthRate = (INT32)( nOldWidth / cx * 100);

		INT32 nWidth = (INT32)( cx * ((REAL32) pColumn->_nWidthRate * 0.01f));

		SetColumnWidth( i, nWidth); 
	}

	SetFocus();

	if( m_pSelectedSubItem)
	{
		if( m_pSelectedSubItem->IsActiveEdit())		
			m_pSelectedSubItem->DestroyEdit( this);
	}
}	

void i3TDKListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();

	ClearAll();
}

void i3TDKListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{	
	//	선택된 아이템 처리
	CPoint pt;
	
	//	클릭된 좌표 변환
	GetCursorPos( &pt);
	ScreenToClient( &pt);

	LVHITTESTINFO lvh;
	lvh.pt = pt;
	SubItemHitTest( &lvh);

	//	이전 Edit는 제거한다.
	if( m_pSelectedSubItem)
	{
		if( m_pSelectedSubItem->IsActiveEdit())		
			m_pSelectedSubItem->DestroyEdit( this);
	}

	if( lvh.flags & LVHT_ONITEM)
	{
		INT32 nRowIdx = lvh.iItem;
		INT32 nColumnIdx = lvh.iSubItem;

		I3ASSERT( nRowIdx >= 0 && nColumnIdx >= 0);

		i3::vector<i3TDKPropertySubItem*>& SubItem = m_SubItemList[nRowIdx];
	
		i3TDKPropertySubItem * pFocusedItem = SubItem[nColumnIdx];

		//	두번째 클릭된 아이템은 입력모드 처리
		if( m_pSelectedSubItem == pFocusedItem && pFocusedItem != nullptr)
		{								
			RECT rt, rcClient;
			INT32 nOffSet = 0;

			//컬럼 옵셋 가져오기			
			for( INT32 i = 0; i < nColumnIdx; i++)
			{
				nOffSet += GetColumnWidth( i);
			}  
			
			GetItemRect( nRowIdx, &rt, LVIR_BOUNDS);	//	Item의 영역을 가져온다.

			//컬럼을 보이기 위해 필요하면 스크롤한다.			
			GetClientRect( &rcClient);
			if( nOffSet + rt.left < 0 || nOffSet + rt.left > rcClient.right)
			{
				CSize size;
				size.cx	= nOffSet + rt.left;
				size.cy	= 0;
				Scroll( size);
				rt.left -= size.cx;
			}

			rt.left += nOffSet;
			rt.right = rt.left + GetColumnWidth( nColumnIdx);
			if( rt.right > rcClient.right)	
				rt.right = rcClient.right;

			rt.top -= 2;
			rt.bottom += 1;

			//	SubItem Hit
			pFocusedItem->OnHit( this, &rt, WS_BORDER | WS_CHILD | WS_VISIBLE);
			
			return;
		}
		//	첫번째 클릭된 아이템
		else
		{				
			m_pSelectedSubItem = pFocusedItem;

			if( m_pSelectedSubItem != nullptr)
			{
				LVSETINFOTIP infotip;
				memset( &infotip, 0, sizeof(infotip));
				infotip.iItem = m_pSelectedSubItem->GetRowIndex();
				infotip.iSubItem = m_pSelectedSubItem->GetColumnIndex();
				infotip.pszText = (LPWSTR) m_pSelectedSubItem->GetLabel();

				SetInfoTip( &infotip);
			}
		}

	}	
	*pResult = 0;
}

void i3TDKListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//HDC			hdc			= lpDrawItemStruct->hDC;
	i3TDKPropertySubItem *	pSubItem	= nullptr;//(i3TDKPropertySubItem *) lpDrawItemStruct->itemData;

	CRect rtDraw;
	RECT rtBounds, rtIcon, rtLabel;
	INT32 nItem = lpDrawItemStruct->itemID;

	//	출력 색상
	COLORREF colorBKSelect= RGB( 150, 150, 220);
	COLORREF colorBKOdd = RGB( 226, 237, 246);
	COLORREF colorBKEven= RGB( 254, 253, 246);

	//	출력 영역
	GetItemRect( nItem, &rtBounds, LVIR_BOUNDS);
	GetItemRect( nItem, &rtLabel, LVIR_LABEL);
	GetItemRect( nItem, &rtIcon, LVIR_ICON);	
		
	//	한 라인씩 그린다. (DrawItem 함수가 한라인 단위로 호출되므로)
	for( INT32 i = 0; i < GetColumnTitleCount(); i++)
	{
		pSubItem = GetSubItem( nItem, i);	//	Get SubItem
		GetSubItemRect( nItem, i, LVIR_LABEL, rtDraw);		//	SubItem 출력 영역

		if( pSubItem)
		{
			RECT rt = { rtDraw.left, rtDraw.top, rtDraw.right, rtDraw.bottom };
			
			SubItemColorSet ColorSet;
			memset( &ColorSet, 0, sizeof(ColorSet));
			ColorSet._ColorBK = (nItem % 2 ? colorBKOdd : colorBKEven);
			ColorSet._ColorSelect = colorBKSelect;

			//	SubItem Draw
			pSubItem->OnDraw( lpDrawItemStruct, &rt, &ColorSet);
		}
	}
}

void i3TDKListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	I3TRACE("OnDrawItem\n");

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void i3TDKListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	SubItemUpdateInfo updateInfo;
	memset( &updateInfo, 0, sizeof( updateInfo));

	updateInfo._nRowIdx		= pDispInfo->item.iItem;
	updateInfo._nColumnIdx	= pDispInfo->item.iSubItem;
	updateInfo._pszText		= pDispInfo->item.pszText;

	//	해당 PropertyItem를 갱신과 동시에 부모에게 업데이트 메시지를 보낸다.
	UpdateSubItem( &updateInfo, TRUE);
	
	//I3TRACE("LabelEdit(%d / %d) : %s\n", pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
}

void i3TDKListCtrl::OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	(void) pDispInfo;
}

void i3TDKListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void i3TDKListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

INT_PTR i3TDKListCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	CRect rt;
	GetClientRect( &rt);

	i3TDKListCtrl * pListCtrl = (i3TDKListCtrl *) this;

	if( rt.PtInRect( point))		
	{
		if( GetItemCount())
		{
			INT32 nTopIndex = GetTopIndex();
			INT32 nBottomIndex = nTopIndex + GetCountPerPage();
			if( nBottomIndex > GetItemCount())
				nBottomIndex = GetItemCount();

			//	라인 단위 영역 검사
			for( INT32 i = nTopIndex; i <= nBottomIndex; i++)
			{
				//	컬럼 단위 영역 검사
				for( INT32 j = 0; j < pListCtrl->GetColumnTitleCount(); j++)
				{
					pListCtrl->GetSubItemRect( i, j, LVIR_LABEL, rt);

					if( rt.PtInRect( point))
					{
						pListCtrl->SetSelectedColumn( j);  

						pTI->hwnd = m_hWnd;
						pTI->uId = (UINT) ( i + 1);
						pTI->lpszText = LPSTR_TEXTCALLBACK;
						pTI->rect = rt;

						return pTI->uId;
					}
				}
			}
		}
	}

	return -1;
	//return CListCtrl::OnToolHitTest(point, pTI);
}

void i3TDKListCtrl::OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	(void) pGetInfoTip;
}

BOOL i3TDKListCtrl::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	UINT nID = pNMHDR->idFrom;

	if(nID == 0)	  	// Notification in NT from automatically
		return FALSE;   	// created tooltip

	INT32 nColumn = GetSelectedColumn();		//	Column Number
	
	i3TDKPropertySubItem * pSubItem = GetSubItem( nID - 1, nColumn);
	if( pSubItem)
	{
		if( pSubItem->GetToolTipText())
		{
			//	carriage return/line feed combination \r\n
			HWND hwndCtrl = pTTTW->hdr.hwndFrom;
			::SendMessage( hwndCtrl, TTM_SETMAXTIPWIDTH, 0, 256);

			//	Set ToolTip Text in the SubItem
			pTTTW->lpszText = (LPWSTR) pSubItem->GetToolTipText();
		}
	}

	return TRUE;
}

BOOL i3TDKListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHEADER pNMHD = (LPNMHEADER)lParam;

	switch(pNMHD->hdr.code)
	{
		// Detects column resizing.
		case HDN_ITEMCHANGEDA:
		case HDN_ITEMCHANGEDW:
			{			
				SetFocus();

				//	
				if( m_pSelectedSubItem)
				{
					if( m_pSelectedSubItem->IsActiveEdit())		
						m_pSelectedSubItem->DestroyEdit( this);
				}
			}
			break;
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}
