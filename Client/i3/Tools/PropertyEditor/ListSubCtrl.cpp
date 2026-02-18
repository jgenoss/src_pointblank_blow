// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListSubCtrl.h"

#include "ListSubEdit.h"
#include "ListSubComboBox.h"
#include "ListSubDialog.h"

#include "AckDialog.h"

static char * szTypeString[] = 
{
	"정수형",
	"실수형",
	"문자열",
	"벡터 2D",
	"벡터 3D",
	"벡터 4D",
	"색상",
	"매트릭스",
};

IMPLEMENT_DYNAMIC(CListSubCtrl, CListCtrl)

CListSubCtrl::CListSubCtrl() : mSubCtrl(0)
{
	m_CurrentItem = -1, m_CurrentSubItem = -1;
	m_bFocusDraw = false;
	m_bFocusDrawLine = false;
	m_bIntenalFocusDraw = false;
}

CListSubCtrl::~CListSubCtrl()
{
	m_Font.DeleteObject();

	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		delete mColumnContexts[ i ];
	}
	mColumnContexts.clear();
	mColumnSize.clear();
}

////////////////////////////////////////////////////////
//CListCtrl

BEGIN_MESSAGE_MAP(CListSubCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CListSubCtrl::OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CListSubCtrl::OnLvnColumnclickList)
	ON_NOTIFY_REFLECT(NM_CLICK, &CListSubCtrl::OnNMClickList)
	ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CListSubCtrl::OnLvnBeginScroll)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CListSubCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListSubCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CListSubCtrl::OnNMRClick)
	ON_NOTIFY_REFLECT(NM_KILLFOCUS, &CListSubCtrl::OnNMKillfocus)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, &CListSubCtrl::OnNMSetfocus)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


BOOL CListSubCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID)
{
	dwStyle |= WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT;  

	return CListCtrl::Create( dwStyle, rect, pParentWnd, ID);
}


int CListSubCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetExtendedStyle(
		LVS_EX_FULLROWSELECT | 
		LVS_EX_GRIDLINES | 
		LVS_EX_TWOCLICKACTIVATE );

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
	_tcscpy_s( logFont.lfFaceName, _T("굴림") );

	Set_LogFont( &logFont );

	NotifyHeaderColumn(CListSubCtrl::ACT_CHANGEKEY);	

	return 0;
}

void CListSubCtrl::_Internal_NotifyHeaderColumn()
{
	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		CListCtrl::DeleteColumn( 0 );
	}

//	INT32 nColWidth = 100;
	for(size_t i = 0; i<count; i++)
	{
		if( mColumnContexts[i]->Show )
		{
			LVCOLUMN list;

			list.mask =  LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
			list.fmt = LVCFMT_LEFT;
			list.cx = DEFAULT_COLUMNSIZE;
			list.pszText   = mColumnContexts[i]->Title;
			list.iSubItem = i;

			CListCtrl::InsertColumn( i, &list);
		}
	}
}

void CListSubCtrl::NotifyHeaderColumn(ActionType action)
{
	Clear_Columns();

	switch(action)
	{
	case CListSubCtrl::ACT_CHANGEKEY :
		{
			Set_Column( _T("Data Name"), DEFAULT_COLUMNSIZE, CST_NONE);
			Set_Column( _T("Data Type"), DEFAULT_COLUMNSIZE, CST_NONE);
		}
		break;
	case CListSubCtrl::ACT_COPYCELL:
		{
			Set_Column( _T("Data Name"), mColumnSize[0], CST_NONE);
			Set_Column( _T("Data Type"), mColumnSize[1], CST_NONE);
		}
		break;
	}
}

void CListSubCtrl::Increase_LogFont()
{
	CFont *pFont = GetFont();
	if(NULL != pFont)
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT));
		pFont->GetLogFont(&logFont);
		++logFont.lfHeight;

		if( logFont.lfHeight >16 )
			logFont.lfHeight = 16;

		Set_LogFont( &logFont );
	}
}

void CListSubCtrl::Decrease_LogFont()
{
	CFont *pFont = GetFont();
	if(NULL != pFont)
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT));
		pFont->GetLogFont(&logFont);
		--logFont.lfHeight;
		if( logFont.lfHeight < 10 )
			logFont.lfHeight = 10;
		Set_LogFont( &logFont );
	}
}

void CListSubCtrl::Set_LogFont( const LOGFONT * pLogFont)
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


void CListSubCtrl::Clear_Columns()
{
	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		if( mColumnContexts[i]->Show )
		{
			mColumnSize.push_back(CListCtrl::GetColumnWidth(0));
			CListCtrl::DeleteColumn( 0 );
		}

		delete mColumnContexts[ i ];
	}
	mColumnContexts.clear();
}

void CListSubCtrl::Clear_Cells()
{
	CListSubCtrl::DeleteAllItems();
}

void CListSubCtrl::Erase_Column(LPCTSTR Name)
{
	int ContextIndex = Get_ColumnContextIndex(Name);
	
	if( ContextIndex == -1 )
		return;

	int Index = Get_ColumnIndex(Name);
	CListSubCtrl::DeleteColumn( Index );

	delete mColumnContexts[ ContextIndex ];
	mColumnContexts.erase( mColumnContexts.begin() + ContextIndex  );
}

void CListSubCtrl::Hide_Column(LPCTSTR Name)
{
	int ContextIndex = Get_ColumnContextIndex(Name);
	
	if( ContextIndex == -1 )
		return;

	int Index = Get_ColumnIndex(Name);
	CListSubCtrl::DeleteColumn( Index );

	mColumnContexts[ ContextIndex ]->Show = false;
}

void CListSubCtrl::Hide_AllColumns()
{
	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		CListSubCtrl::DeleteColumn( i );
		mColumnContexts[ i ]->Show = false;
	}
}

int	CListSubCtrl::GetLastColumnSize(int nCol)
{
	if (mColumnSize.size() > nCol)
		return mColumnSize[nCol];
	else
		return DEFAULT_COLUMNSIZE;
}

void CListSubCtrl::Show_Column(LPCTSTR Name)
{
	int ContextIndex = Get_ColumnContextIndex(Name);
	
	if( ContextIndex == -1 )
		return;

	mColumnContexts[ ContextIndex ]->Show = true;

	_Internal_NotifyHeaderColumn();
}

void CListSubCtrl::Show_AllColumns()
{
	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		mColumnContexts[ i ]->Show = true;
	}

	_Internal_NotifyHeaderColumn();
}


int CListSubCtrl::Get_ColumnIndex(LPCTSTR Name)
{
	int Ret = 0;
	size_t Size = mColumnContexts.size();
	for(size_t i = 0; i<Size; ++i)
	{
		if( mColumnContexts[i]->Show )
		{
			if( _tcsncmp(mColumnContexts[i]->Title,Name, _tcslen(Name) ) == 0 )
				return Ret;

			++Ret;
		}
	}

	return -1;
}

int CListSubCtrl::Get_ColumnContextIndex(LPCTSTR Name)
{
	size_t Size = mColumnContexts.size();
	for(size_t i = 0; i<Size; ++i)
	{
		if( _tcsncmp(mColumnContexts[i]->Title,Name, _tcslen(Name) ) == 0 )
			return (int)i;
	}

	return -1;
}

int CListSubCtrl::Get_ColumnCount() const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);   
	return pHeader->GetItemCount();   
}

int CListSubCtrl::Get_ColumnContextIndex(int nCol)
{
	++nCol;
	size_t Size = mColumnContexts.size();
	for(size_t i = 0; i<Size; ++i)
	{
		if( mColumnContexts[i]->Show )
			--nCol;
		if( nCol == 0 )
			return i;
	}

	return -1;
}

int CListSubCtrl::Get_ColumnContextCount() const
{
	return (int)mColumnContexts.size();
}

int CListSubCtrl::Get_CellCount() const
{
	return CListCtrl::GetItemCount();
}

void CListSubCtrl::Set_Column(LPTSTR Title, int Length, CellSubType CellTyte, UINT Mask, UINT Format)
{
	LVCOLUMN list;

	list.mask =  Mask;
	list.fmt = Format;
	list.cx = Length;
	list.pszText   = Title;
	list.iSubItem = Get_ColumnCount();

	CListCtrl::InsertColumn( Get_ColumnCount(), &list);

	ColumnContext* Context = new ColumnContext;
	_tcscpy_s(Context->Title, Title);
	Context->Cst = CellTyte;
	Context->SortAscend = false;
	Context->Show = true;
	
	mColumnContexts.push_back( Context );
}

void CListSubCtrl::Modify_ColumnText(int nCol, LPCTSTR Title)
{
	LVCOLUMN list;

	list.mask = LVCF_TEXT ;
	list.fmt = LVCFMT_LEFT;
	list.pszText = (LPTSTR)Title;

	CListCtrl::SetColumn( nCol, &list);

	nCol = Get_ColumnContextIndex(nCol);

	_tcscpy_s(mColumnContexts[nCol]->Title, MAX_PATH, Title);
}

void CListSubCtrl::Modify_ColumnText()
{
	LVCOLUMN list;

	list.mask = LVCF_TEXT ;
	list.fmt = LVCFMT_LEFT;

	int ColumnSize = Get_ColumnCount();
	int Index = 0;
	for( int i = 0; i < ColumnSize; ++i )
	{
		if( mColumnContexts[i]->Show )
		{
			list.pszText = mColumnContexts[i]->Title;
			CListCtrl::SetColumn( Index, &list);
			++Index;
		}
	}
}

CString CListSubCtrl::Get_ColumnText(int nCol)
{
	ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

	int Index = Get_ColumnContextIndex( nCol );

	if( Index == -1 )
		return CString();

	return CString(mColumnContexts[Index]->Title);
}

CString CListSubCtrl::Get_ColumnContextText(int nCol)
{
	ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

	return CString(mColumnContexts[nCol]->Title);
}

void CListSubCtrl::_SetCellColor( LVITEM& lvItem )
{
	// GD 요청사항
	if(lvItem.iSubItem > 2)
	{
		CString temp_Text;
		temp_Text = GetItemText(lvItem.iItem, 2);
		if( (temp_Text == lvItem.pszText) == false)
		{
			COLORREF color;
			INT32 color_count = 0;

			if( different_cell_list.empty())
			{
				different_cell_list.push_back(lvItem.iSubItem);
			}
			else
			{
				size_t i = 0;
				for(i = 0; i < different_cell_list.size(); i++)
				{
					temp_Text = GetItemText(lvItem.iItem, different_cell_list[i]);
					if( (temp_Text == lvItem.pszText) == true)
					{
						color_count = i;
						break;
					}
				}

				if(i == different_cell_list.size())
				{
					color_count = i;
					different_cell_list.push_back(lvItem.iSubItem);
				}
			}

			switch(color_count)
			{
			case 0:
				color = RGB(255,0,0);
				break;
			case 1:
				color = RGB(0,255,0);
				break;
			case 2:
				color = RGB(0,0,255);
				break;
			case 3:
				color = RGB(255,0,255);
				break;
			case 4:
				color = RGB(0,255,255);
				break;
			case 5:
				color = RGB(255,255,0);
				break;
			case 6:
				color = RGB(100,100,255);
				break;
			case 7:
				color = RGB(100,0,50);
				break;
			case 8:
				color = RGB(0,100,55);
				break;
			case 9:
				color = RGB(70,10,100);
				break;
			case 10:
				color = RGB(100,200,6);
				break;
			case 11:
				color = RGB(80,10,255);
				break;
			case 12:
			default:
				color = RGB(0,0,0);
				break;
			}
			Add_CustomDraw(lvItem.iItem, lvItem.iSubItem, DAT_MODIFY, color);
		}
	}
	else
	{
		different_cell_list.clear();
	}
}

bool CListSubCtrl::Set_Cell(int nRow, int nCol, LPTSTR Value)
{
	ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = Value;
	lvItem.iSubItem = nCol;

	CListCtrl::SetItem( &lvItem );
	
	_SetCellColor(lvItem);

	return true;
}

void CListSubCtrl::Insert_Cell(int nRow, LPTSTR Value)
{
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = Value;
	lvItem.iSubItem = 0;

	CListCtrl::InsertItem( &lvItem );
}

bool CListSubCtrl::Is_HeaderShow(int nCol)
{
	if( nCol >= (int)mColumnContexts.size() )
		return false;

	//ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

	return mColumnContexts[ nCol ]->Show;
}

void CListSubCtrl::OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	LVHITTESTINFO ht;
    ht.pt = pNMListView->ptAction;
    SubItemHitTest(&ht);

	m_CurrentItem = ht.iItem;
	m_CurrentSubItem = ht.iSubItem;

	OnLvnCellLClickList(ht.iItem, ht.iSubItem, ht.pt);

	*pResult = 0;
}

void CListSubCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	LVHITTESTINFO ht;
    ht.pt = pNMListView->ptAction;
    SubItemHitTest(&ht);

	m_CurrentItem = ht.iItem;
	m_CurrentSubItem = ht.iSubItem;

	OnLvnCellRClickList(pNMHDR, pResult);

	*pResult = 0;
}

void CListSubCtrl::OnLvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	Invalidate();
	LV_DISPINFO* temp = (LV_DISPINFO*) pNMHDR;
	
	int nItem = temp->item.iItem;
	int nSubItem = temp->item.iSubItem;
	LPTSTR String = temp->item.pszText;

	if( String )
	{
		CString tmpStr = Get_CellText(nItem, nSubItem);
		if( isSameSubControl(nItem, nSubItem, String) == false)
			Add_CustomDraw(nItem, nSubItem, DAT_MODIFY, RGB(255,0,0));

		EndSubControl(nItem, nSubItem, String );
		
		//디폴트 값을 변환할 경우 자동으로 카피라이트!
		if(nSubItem == 2)
		{
			for(int i = 3 ; i < Get_ColumnCount() ; i++ )
			{
				if( isSameSubControl(nItem, i, String) == false)
					Add_CustomDraw(nItem, i, DAT_MODIFY, RGB(255,0,0));

					EndSubControl(nItem, i, String);
			}
		}
	}

	mSubCtrl = NULL;
	
	*pResult = 0;
}

CRect CListSubCtrl::_Internal_GetSubItemRect(int nCol, int nSumItem)
{
	CRect rect;

	GetSubItemRect(nCol, nSumItem, LVIR_BOUNDS, rect);

	if( nSumItem == 0 )
	{
		CRect rect1;
		GetSubItemRect(nCol, 1, LVIR_BOUNDS, rect1);

		rect.right = rect1.left;
	}

	return rect;
}

int CListSubCtrl::Is_HeaderClicked(POINT pt)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	ScreenToClient(&pt);

	CRect rect;
	pHeader->GetItemRect(0, &rect);
	int Height = rect.Height();

	for(int i = 0; i < nColumnCount; ++i)   
	{
		rect = _Internal_GetSubItemRect(0, i);
		rect.top = 0;
		rect.bottom = Height;

		if( rect.PtInRect(pt) )
			return i;
	}   

	return -1;
}

void CListSubCtrl::OnLvnBeginScroll(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( mSubCtrl )
	{
		if( mSubCtrl->IsWindowVisible() )
			mSubCtrl->SendMessage(WM_KILLFOCUS, VK_ESCAPE);
	}

	*pResult = 0;
}

void CListSubCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	LVHITTESTINFO ht;
    ht.pt = pNMListView->ptAction;
    SubItemHitTest(&ht);

	m_CurrentItem = ht.iItem;
	m_CurrentSubItem = ht.iSubItem;

	if(m_CurrentItem != -1 && m_CurrentSubItem != -1)
	{
		CellSubType Type = mColumnContexts[ m_CurrentSubItem ]->Cst;

		if( Type == CST_NONE )
			Type = PreSubControlType(m_CurrentItem, m_CurrentSubItem);

		StartSubControl(m_CurrentItem, m_CurrentSubItem, Type );
	}

	*pResult = 0;
}

void CListSubCtrl::Clear_CustomDraw(AnalysisType atype)
{
	m_CustomDrawBank[atype].clear();
}

void CListSubCtrl::Clear_AllCustomDraw()
{
	for(size_t atype=0; atype<DAT_TYPE_MAX; atype++)
		m_CustomDrawBank[atype].clear();
}

void CListSubCtrl::Add_CustomDraw(int nItem, int nSubItem, AnalysisType atype, COLORREF cTextBk, COLORREF cText)
{
	CustomDrawContext Context;

	Context.nItem = nItem;
	Context.nSubItem = nSubItem;
	Context.cText = cText;
	Context.cTextBk = cTextBk;

	m_CustomDrawBank[atype].push_back( Context );
}
void CListSubCtrl::Del_CustomDraw(int nItem, int nSubItem, AnalysisType atype)
{
	size_t size = m_CustomDrawBank[atype].size();
	for(size_t i=0;i<size;++i)
	{
		if( m_CustomDrawBank[atype][i].nItem == nItem && 
			m_CustomDrawBank[atype][i].nSubItem == nSubItem )
		{
			m_CustomDrawBank[atype].erase( m_CustomDrawBank[atype].begin() + i );
			return;
		}
	}
}

bool CListSubCtrl::Get_CustomDrawCol(int nItem, int nSubItem, COLORREF & ref_cTextBk, COLORREF & ref_cText)
{
	ref_cText = CLR_DEFAULT;
	ref_cTextBk = CLR_DEFAULT;
	
	for(size_t atype=0; atype<DAT_TYPE_MAX; atype++)
	{
		size_t size = m_CustomDrawBank[atype].size();
		for(size_t i=0;i<size;++i)
		{
			if( m_CustomDrawBank[atype][i].nItem == nItem && 
				m_CustomDrawBank[atype][i].nSubItem == nSubItem )
			{
				ref_cText = m_CustomDrawBank[atype][i].cText;
				ref_cTextBk = m_CustomDrawBank[atype][i].cTextBk;
				break;
			}
		}
	}

	return false;
}

void CListSubCtrl::Invalidate_CustomDrawCol()
{
	RedrawItems(0, GetItemCount() );
}

void CListSubCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw  = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			int Item = (int)lpLVCustomDraw->nmcd.dwItemSpec;
			int SubItem = (int)lpLVCustomDraw->iSubItem;

			lpLVCustomDraw->clrText = CLR_DEFAULT;
			lpLVCustomDraw->clrTextBk = CLR_DEFAULT;

			Get_CustomDrawCol(Item, SubItem, lpLVCustomDraw->clrTextBk, lpLVCustomDraw->clrText);
		}
		break;

	default: break;    
	}

	// TODO: Add your control notification handler code here
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}


BOOL CListSubCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_F2:
			if(m_CurrentItem != -1 && m_CurrentSubItem != -1)
			{
				CellSubType Type = mColumnContexts[ m_CurrentSubItem ]->Cst;

				if( Type == CST_NONE )
					Type = PreSubControlType(m_CurrentItem, m_CurrentSubItem);

				StartSubControl(m_CurrentItem, m_CurrentSubItem, Type );
				return true;
			}
		case VK_ADD:	case VK_PRIOR:
			if( GetAsyncKeyState( VK_CONTROL ) )
				Increase_LogFont();
			break;
		case VK_SUBTRACT:	case VK_NEXT:
			if( GetAsyncKeyState( VK_CONTROL ) )
				Decrease_LogFont();
			break;
		}
		break;

	case WM_RBUTTONDOWN:
		{
			int Index = Is_HeaderClicked(pMsg->pt);
			if( Index != -1 )
				OnLvnColumnRClickList(Index, pMsg->pt);
		}
		break;
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}


void CListSubCtrl::Modify_UnSortedColumnText()
{
	int ColumnSize = Get_ColumnCount();
	for( int i = 0; i < ColumnSize; ++i )
	{
		CString Text( mColumnContexts[i]->Title );

		int Size = Text.GetLength();
		int Index = Text.ReverseFind( _T(' ') );

		if( Size == Index + 2 ||  Size == Index + 3)
			Text.Delete( Index, Size - Index );

		_tcscpy_s(mColumnContexts[ i ]->Title, MAX_PATH, Text);
	}

	Modify_ColumnText();
}
void CListSubCtrl::Modify_SortedColumnText(int nCol, bool bAscending)
{
	//현재 고쳐야 할 이름
	CString Text = Get_ColumnText( nCol );

	int Size = Text.GetLength();
	int Index = Text.ReverseFind( _T(' ') );

	if( Size == Index + 2 ||  Size == Index + 3)
	{
		Text.Delete( Index, Size - Index );
	}

	if( bAscending )
		Text.Append( _T(" ▲") );
	else
		Text.Append( _T(" ▼") );

	Index = Get_ColumnContextIndex( nCol );
	_tcscpy_s(mColumnContexts[ Index ]->Title, MAX_PATH, Text);

	Modify_ColumnText();
}

void CListSubCtrl::OnLvnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	Invalidate();
	LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE) pNMHDR;

	//get the column number
	m_CurrentSubItem = temp->iSubItem;
	if( m_CurrentSubItem == -1)
		return;

	Sort_Cell(m_CurrentSubItem, mColumnContexts[ m_CurrentSubItem ]->SortAscend );

	mColumnContexts[ m_CurrentSubItem ]->SortAscend = !mColumnContexts[ m_CurrentSubItem ]->SortAscend;

	Modify_UnSortedColumnText();
	Modify_SortedColumnText(m_CurrentSubItem, mColumnContexts[ m_CurrentSubItem ]->SortAscend );

	m_CurrentItem = -1;
	m_CurrentSubItem = -1;

	*pResult = 0;
}

void CListSubCtrl::Sort_Swap(int low, int high)
{
	// rowText 변수가 한줄에 대한 모든 컬럼문자열을 가지게 됩니다.
	CStringArray rowText;

	// 줄들을 교환한다.
	LV_ITEM lvitemlo, lvitemhi;

	int nColCount = Get_ColumnCount();
	rowText.SetSize( nColCount );

	int i;
	for( i=0; i<nColCount; i++)
		rowText[i] = GetItemText(low, i);

	lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvitemlo.iItem = low;
	lvitemlo.iSubItem = 0;
	lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
		LVIS_FOCUSED | LVIS_SELECTED |
		LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
	lvitemhi = lvitemlo;
	lvitemhi.iItem = high;

	CListCtrl::GetItem( &lvitemlo );
	CListCtrl::GetItem( &lvitemhi );

	for( i=0; i<nColCount; i++)
		CListCtrl::SetItemText(low, i, GetItemText(high, i));

	lvitemhi.iItem = low;
	CListCtrl::SetItem( &lvitemhi );

	for( i=0; i<nColCount; i++)
		CListCtrl::SetItemText(high, i, rowText[i]);

	lvitemlo.iItem = high;
	CListCtrl::SetItem( &lvitemlo );


	void* pData = m_DataPtr[ low ];
	m_DataPtr[ low ] = m_DataPtr[ high ];
	m_DataPtr[ high ] = pData;
}


// SortTextItems - 리스트를 컬럼 텍스트에 따라 정렬하는 함수
// Returns - 성공시 TRUE 리턴
// nCol - 소트할 문자열을 가지고 있는 컬럼번호
// bAscending - 소트순서 지정
// low - 조사 시작 줄 - 기본값은 0
// high - 조사 마지막줄 - -1은 마지막줄을 가리킵니다.
BOOL CListSubCtrl::Sort_Cell( int nCol, BOOL bAscending,
								int low /*= 0*/, int high /*= -1*/ )
{
	if( nCol >= Get_ColumnCount() )
		return FALSE;

	if( high == -1 ) high = GetItemCount() - 1;
	int lo = low;
	int hi = high;
	CString midItem, textItem;
	if( hi <= lo ) return FALSE;
	midItem = GetItemText( low, nCol );
	// 인덱스들이 교차될때까지 리스트를 돕니다(Loop).
	while( lo <= hi )
	{
		//왼쪽 인덱스부터 시작하여 구역 요소보다 크거나 같은 첫째 요소를 찾음.
		//if( bAscending )
		//	while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
		//		++lo;
		//else
		//	while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
		//		++lo;

		//오른쪽 인덱스부터 시작하여 구역 요소보다 크거나 같은 요소를 찾음.
		//if( bAscending )
		//	while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
		//		--hi;
		//else
		//	while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
		//		--hi;
		if( bAscending )
		{
			while( lo < high )
			{
				textItem = GetItemText(lo, nCol);
				if( textItem == midItem )		break;
				if( Sort_Compare(textItem, midItem, bAscending, lo, low) )	break;
				++lo;
			}
			
			while( hi > low )
			{
				textItem = GetItemText(hi, nCol);
				if( textItem == midItem )		break;
				if( !Sort_Compare(textItem, midItem, bAscending, hi, low) )	break;
				--hi;
			}
		}
		else
		{
			while( lo < high )
			{
				textItem = GetItemText(lo, nCol);
				if( textItem == midItem )		break;
				if( !Sort_Compare(textItem, midItem, bAscending, lo, low) )	break;
				++lo;
			}
			
			while( hi > low )
			{
				textItem = GetItemText(hi, nCol);
				if( textItem == midItem )		break;
				if( Sort_Compare(textItem, midItem, bAscending, hi, low) )	break;
				--hi;
			}
		}

		// 만약 인덱스가 교차되지 않았다면 교환하고, 만약 아이템이 같지않다면,
		if( lo <= hi )
		{
			// 아이템이 같지 않을때만 교환한다.
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol) )
			{
				Sort_Swap(lo, hi);
			}
			++lo;
			--hi;
		}
	}

	// 만약 오른쪽 인덱스가 배열의 왼쪽 끝에 닿지 않았다면 왼쪽 구역을 
	// 정렬해야 한다.
	if( low < hi )
		Sort_Cell( nCol, bAscending , low, hi);

	// 만약 왼쪽 인덱스가 배열의 오른쪽 끝에 닿지 않았다면 오른쪽 구역을
	// 정렬해야 한다.
	if( lo < high )
		Sort_Cell( nCol, bAscending , lo, high );

	return TRUE;
}

CString CListSubCtrl::Get_CellText(int Row, int Col)
{
	return CListCtrl::GetItemText(Row, Col);
}


void CListSubCtrl::StartSubControl(int nItem, int nCol, CellSubType CellType)
{
	if(!EnsureVisible(nItem, TRUE)) return;

	int nColumnCount = Get_ColumnCount();   
	if(nCol >= nColumnCount || GetColumnWidth(nCol) < 10)   
		return;   

	int offset = 0;
	for(int i = 0; i < nCol; ++i)   
	{   
		offset += GetColumnWidth(i);
	}   

	CRect rect;   
	GetItemRect(nItem, &rect, LVIR_BOUNDS);   

	CRect rcClient;   
	GetClientRect(&rcClient);   
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)   
	{
		CSize size;   
		size.cx = offset + rect.left;   
		size.cy = 0;   
		Scroll(size);   
		rect.left -= size.cx;   
	}   

	rect.left += offset + 3;
	rect.right = rect.left + GetColumnWidth(nCol) - 5;
	int nHeight = rect.bottom - rect.top;   
	if(rect.right > rcClient.right) 
		rect.right = rcClient.right;   

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE;   

	CString CellText = Get_CellText(nItem ,nCol);

	m_bIntenalFocusDraw = true;
	m_bFocusDraw = false;
	Clear_CustomDraw(DAT_COMPARE);
	Clear_CustomDraw(DAT_MATCH);

	switch( CellType )
	{
	case CST_EDIT:
		{
			CListSubEdit* pSub = new CListSubEdit(nItem, nCol, CellText);   
			
			dwStyle |= ES_AUTOHSCROLL;

			pSub->Create(dwStyle, rect, this, 201);

			PreSubEditControl(nItem, nCol, pSub, rect, CellText);
			
			mSubCtrl = pSub;
		}
		break;
	case CST_COBMO:
		{
			dwStyle |= CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
			rect.bottom += 5 * nHeight;

			int Index = 0;
			POSITION Pos = m_ComboStringList.GetHeadPosition();
			while( Pos != NULL ) 
			{
				if( m_ComboStringList.GetNext( Pos ) == CellText )
					break;
				++Index;
			}

			CListSubComboBox* pSub = new CListSubComboBox(nItem, nCol, &m_ComboStringList, Index);   
			
			pSub->Create(dwStyle, rect, this, 200);
			pSub->SetItemHeight(-1, nHeight);
			pSub->SetHorizontalExtent(GetColumnWidth(nCol));   

			PreSubComboBoxControl(nItem, nCol, pSub, rect, CellText);

			mSubCtrl = pSub;
		}
		break;
	case CST_DIALOG:
		{
			m_bIntenalFocusDraw = false;
			m_bFocusDraw = true;
			m_bFocusDrawLine = false;

			CListSubDialog Sub(nItem, nCol, this);
			PreSubDialogControl(nItem, nCol, &Sub, rect, CellText);

			if( Sub.DoModal() == IDOK )
			{
				CString ret = Sub.GetTextEdit();
				EndSubControl(nItem, nCol, (LPTSTR)ret.GetString() );
			}

			m_bFocusDraw = false;
		}
		break;
	}
}

void CListSubCtrl::EndSubControl(int Row, int Col, LPTSTR String)
{
	Set_Cell(Row, Col, String);
}

void CListSubCtrl::ResetDataPtr()
{
	m_DataPtr.clear();
}
void CListSubCtrl::AddDataPtr(void* Data)
{
	m_DataPtr.push_back( Data );
}
void* CListSubCtrl::GetDataPtr(size_t Index)
{
	return m_DataPtr[Index];
}
size_t CListSubCtrl::GetDataPtrSize() const
{
	return m_DataPtr.size();
}
void CListSubCtrl::DelDataPtr(size_t Index)
{
	m_DataPtr.erase( m_DataPtr.begin() + Index );
}

void CListSubCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !m_bIntenalFocusDraw && !m_bFocusDraw )
		m_bFocusDraw = PreKillFocus();

	if( m_bFocusDraw && m_CurrentItem != -1 && m_CurrentSubItem != -1)
	{
		Clear_AllCustomDraw();
		if( m_bFocusDrawLine )
		{
			for(int i = 0; i<Get_ColumnCount(); ++i)
				Add_CustomDraw(m_CurrentItem, i, DAT_SELECT , RGB(0, 162,232));
		}
		else
			Add_CustomDraw(m_CurrentItem, m_CurrentSubItem, DAT_SELECT, RGB(0, 162,232) );

		RedrawItems(0, GetItemCount() );
	}

	*pResult = 0;
}

void CListSubCtrl::OnNMSetfocus(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( m_bFocusDraw && m_CurrentItem != -1 && m_CurrentSubItem != -1)
	{
		Clear_AllCustomDraw();
		RedrawItems(0, GetItemCount() );
		m_bFocusDraw = false;
		m_CurrentSubItem = false;
	}

	*pResult = 0;
}

BOOL CListSubCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nFlags != MK_CONTROL) return FALSE;

	if (zDelta > 0)
		Increase_LogFont();
	else
		Decrease_LogFont();

	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
