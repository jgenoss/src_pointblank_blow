// ListViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "ListViewCtrl.h"
#include "ComboxExForListCtrl.h"
#include "EditCtrlForListCtrl.h"
#include ".\listviewctrl.h"

#include "i3Base/string/ext/utf16_to_mb.h"
// CListViewCtrl

IMPLEMENT_DYNAMIC(CListViewCtrl, CListCtrl)
CListViewCtrl::CListViewCtrl()
{
	m_pCurCtrl = NULL;
}

CListViewCtrl::~CListViewCtrl()
{
}


BEGIN_MESSAGE_MAP(CListViewCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
	ON_WM_SIZE()
END_MESSAGE_MAP()


/*
// CListViewCtrl message handlers
INT32 CListViewCtrl::HitTestEx( CPoint &point, INT32 * col)
{
	INT32 nColumn	= 0;
	INT32 row		= HitTest( point, NULL);

	if( col)	*col = 0;
	if( row != -1)
	{
		return row;
	}

	if( (GetWindowLong( m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
		return row;

	//화면에 보이는 처음과 끝의 row 알아내기
	row = GetTopIndex();
	INT32	bottom = row + GetCountPerPage();
	if( bottom > GetItemCount())
		bottom = GetItemCount();

	//컬럼갯수 알아내기
	CHeaderCtrl * pHeader	= (CHeaderCtrl*)GetDlgItem(0);
	INT32 nColumnCount		= pHeader->GetItemCount();

	//Row들간에 루프 돌기
	for( ; row <= bottom; row++)
	{
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS);
		if( rect.PtInRect( point))
		{
			//컬럼찾기
			for( nColumn = 0; nColumn< nColumnCount; ++nColumn)
			{
				INT32 nColWidth = GetColumnWidth( nColumn);
				if(( point.x > rect.left ) && (point.x <= (rect.left + nColWidth)))
				{
					if( col) *col = nColumn;

					return row;
				}

				rect.left += nColWidth;
			}
		}
	}

	return -1;
}
*/
int CListViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void CListViewCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	LVITEM * plvItem = &pDispInfo->item;

	if( plvItem->pszText != NULL)
	{
		char szName[MAX_PATH];
	//	char szString[MAX_PATH];

		I3ASSERT( m_pCurField != NULL);
		m_pCurField->m_FieldData = 0;

		SetItemText( plvItem->iItem, plvItem->iSubItem, plvItem->pszText);

		if( m_pCurField->m_nFieldStyle & I3RSC_FIELD_TYPE_STRING)
		{
			m_pCurField->m_FieldData = i3::rc_string(plvItem->pszText);
		}
		else if( m_pCurField->m_nFieldStyle & I3RSC_FIELD_TYPE_INDEX)
		{
			i3RegKey * pKey = NULL;

			if( g_pRegistry != NULL)
			{
				pKey = FIND_REG_KEY( REG_LAYER_ROOT);
			}
			if( pKey != NULL)
			{
				i3::rc_wstring	wstrReg;
				i3::string str;
				INT32 nLayerCount = pKey->GetDataCount();
				for(INT32 i =0; i< nLayerCount; ++i)
				{
					
					sprintf( szName, "Layer%d", i);
					FIND_REG_DATA( pKey, szName, wstrReg );
					
					i3::utf16_to_mb(wstrReg, str);

					if( i3::generic_is_iequal( plvItem->pszText, str) )
					{
						m_pCurField->m_FieldData = i;
						i = nLayerCount;
					}
				}
			}
			else
			{
				
				for(INT32 i =0; i< I3RSC_LAYER_TYPECOUNT; ++i)
				{
					if( i3::generic_is_iequal( plvItem->pszText, g_LayerTypeDef[i] ) )
					{
						m_pCurField->m_FieldData = i - 1;
						break;
					}
				}
			}
		}
		else if( m_pCurField->m_nFieldStyle & I3RSC_FIELD_TYPE_VALUE)
		{
			m_pCurField->m_FieldData = atoi( plvItem->pszText);
		}
		else if( m_pCurField->m_nFieldStyle & I3RSC_FIELD_TYPE_REALVALUE)
		{
			m_pCurField->m_FieldData = (REAL32)atof( plvItem->pszText);
		}
	}

	m_pCurCtrl = NULL;

	*pResult = 0;
}

CEdit *	CListViewCtrl::EditSubLabel( RSC_FIELD_INFO * pInfo, INT32 nItem, INT32 nCol)
{
	if( !EnsureVisible( nItem, TRUE))	return NULL;

	m_pCurField = pInfo;

	//nCol이 유효한지 확인
	CHeaderCtrl * pHeader		= (CHeaderCtrl*)GetDlgItem(0);
	INT32		  nColumnCount	= pHeader->GetItemCount();

	if( nCol >= nColumnCount || GetColumnWidth( nCol) < 5)
			return NULL;

	//컬럼 옵셋 가져오기
	INT32 nOffSet = 0;
	for( INT32 i = 0; i < nCol; i++)
	{
		nOffSet += GetColumnWidth( i);
	}

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS);

	//컬럼을 보이기 위해 필요하면 스크롤한다.
	CRect rcClient;
	GetClientRect( &rcClient);
	if( nOffSet + rect.left < 0 || nOffSet + rect.left > rcClient.right)
	{
		CSize size;
		size.cx	= nOffSet + rect.left;
		size.cy	= 0;
		Scroll( size);
		rect.left -= size.cx;
	}

	//컬럼의 정렬방식 알아내기
	LV_COLUMN lvCol;
	lvCol.mask	= LVCF_FMT;
	GetColumn( nCol, &lvCol);

	DWORD dwStyle;
	if((lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if(( lvCol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle	= ES_RIGHT;
	else
		dwStyle	= ES_CENTER;

	rect.left += nOffSet;;
	rect.right = rect.left + GetColumnWidth( nCol);
	if( rect.right > rcClient.right)	rect.right	= rcClient.right;

	dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	CEdit	* pEdit = new CEditCtrlForListCtrl( nItem, nCol, GetItemText( nItem, nCol));
	pEdit->Create( dwStyle, rect, this, 1);

	m_pCurCtrl	= (CWnd*)pEdit;

	return pEdit;
}

CComboBox *	CListViewCtrl::SelectValue( RSC_FIELD_INFO * pInfo, INT32 nItem, INT32 nCol, CStringList &lstItems, INT32 nSel)
{
	if( !EnsureVisible( nItem, TRUE))	return NULL;

	m_pCurField = pInfo;

	//nCol이 유효한지 확인
	CHeaderCtrl * pHeader = (CHeaderCtrl*)GetDlgItem(0);
	INT32 nColumnCount = pHeader->GetItemCount();
	if( nCol >= nColumnCount || GetColumnWidth( nCol) < 10)
	{
		return NULL;
	}

	//Column OffSet 가져오기
	INT32 nOffSet = 0;
	for( INT32 i = 0; i< nCol; ++i)
	{
		nOffSet += GetColumnWidth( i);
	}

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS);

	//컬럼을 보이기 위해 필요하면 스크롤 한다.
	CRect rcClient;
	GetClientRect( &rcClient);
	if( nOffSet + rect.left < 0 || nOffSet + rect.left > rcClient.right)
	{
		CSize size;
		size.cx	= nOffSet + rect.left;
		size.cy	= 0;
		Scroll( size);
		rect.left	-= size.cx;
	}

	rect.left	+= nOffSet;
	rect.right	= rect.left + GetColumnWidth( nCol);

	INT32 nHeight	= rect.bottom - rect.top;
	rect.bottom		+= 5 * nHeight;

	if( rect.right > rcClient.right)	rect.right = rcClient.right;

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
					CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;

	CComboBox * pList = new CComboxExForListCtrl( nItem, nCol, &lstItems, nSel);
	pList->Create( dwStyle, rect, this, 1);
	pList->SetItemHeight( -1, nHeight);
	pList->SetHorizontalExtent( GetColumnWidth( nCol));

	m_pCurCtrl	= (CWnd*)pList;

	return pList;
}


void CListViewCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	if( m_pCurCtrl != NULL)
	{
		::SendMessage( m_pCurCtrl->GetSafeHwnd(), WM_KILLFOCUS, 0, 0);
	}
	// TODO: Add your message handler code here
}
