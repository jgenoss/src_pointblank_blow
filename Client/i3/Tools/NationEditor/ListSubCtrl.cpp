// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListSubCtrl.h"

#include "ListSubComboBox.h"

IMPLEMENT_DYNAMIC(CListSubCtrl, CListCtrl)

CListSubCtrl::CListSubCtrl() : mSubCtrl(0)
{
	m_CurrentItem = -1, m_CurrentSubItem = -1;
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
}

////////////////////////////////////////////////////////
//CListCtrl

BEGIN_MESSAGE_MAP(CListSubCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CListSubCtrl::OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_CLICK, &CListSubCtrl::OnNMClickList)
	ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &CListSubCtrl::OnLvnBeginScroll)
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
	_tcscpy_s( logFont.lfFaceName, _T("±¼¸²") );

	Set_LogFont( &logFont );

	NotifyHeaderColumn();	

	return 0;
}

void CListSubCtrl::NotifyHeaderColumn()
{
	Clear_Columns();
}

void CListSubCtrl::ResetComboString()
{
	m_ComboStringList.RemoveAll();
}

void CListSubCtrl::AddComboString( const CString & String )
{
	m_ComboStringList.AddTail( String );
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
			CListCtrl::DeleteColumn( 0 );

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
		mColumnContexts[ i ]->Show = false;
	}
}

void CListSubCtrl::Show_Column(LPCTSTR Name)
{
	int ContextIndex = Get_ColumnContextIndex(Name);
	
	if( ContextIndex == -1 )
		return;

	mColumnContexts[ ContextIndex ]->Show = true;
}

void CListSubCtrl::Show_AllColumns()
{
	size_t count = mColumnContexts.size();
	for(size_t i = 0; i<count; i++)
	{
		mColumnContexts[ i ]->Show = true;
	}

	NotifyHeaderColumn();
}


int CListSubCtrl::Get_ColumnIndex(LPCTSTR Name)
{
	int Ret = 0;
	size_t Size = mColumnContexts.size();
	for(size_t i = 0; i<Size; ++i)
	{
		if( mColumnContexts[i]->Show )
		{
			if( _tcscmp(mColumnContexts[i]->Title,Name) == 0 )
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
		if( _tcscmp(mColumnContexts[i]->Title,Name) == 0 )
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

	return Size;
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

bool CListSubCtrl::Set_Cell(int nRow, int nCol, LPTSTR Value)
{
	ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = Value;
	lvItem.iSubItem = nCol;

	CListCtrl::SetItem( &lvItem );

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
	ASSERT( nCol < (int)mColumnContexts.size() && L"Column Size Error" );

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

	if(m_CurrentItem != -1 && m_CurrentSubItem != -1)
	{
		CellSubType Type = mColumnContexts[ m_CurrentSubItem ]->Cst;

		if( Type == CST_NONE )
			Type = PreSubControlType(m_CurrentItem, m_CurrentSubItem);

		StartSubControl(m_CurrentItem, m_CurrentSubItem, Type );
	}

	*pResult = 0;
}

void CListSubCtrl::OnLvnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* temp = (LV_DISPINFO*) pNMHDR;
	
	int nItem = temp->item.iItem;
	int nSubItem = temp->item.iSubItem;
	LPTSTR String = temp->item.pszText;

	if( String )
		EndSubControl(nItem, nSubItem, String );

	mSubCtrl = NULL;
	
	*pResult = 0;
}


int CListSubCtrl::Is_HeaderClicked(POINT pt)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);   
	int nColumnCount = pHeader->GetItemCount();

	ScreenToClient(&pt);

	for(int i = 0; i < nColumnCount; ++i)   
	{
		CRect rect;   
		pHeader->GetItemRect(i, &rect);

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
	}

	return CListCtrl::PreTranslateMessage(pMsg);
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

	switch( CellType )
	{
	case CST_COBMO:
		{
			dwStyle |=	CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
			rect.bottom += 15 * nHeight;

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
	}
}

CListSubCtrl::CellSubType CListSubCtrl::PreSubControlType(int Row, int Col)
{
	return CST_NONE;
}

void CListSubCtrl::PreSubEditControl(int Row, int Col, CListSubEdit * pDialog, const CRect& rect, const CString & CellText)
{
}

void CListSubCtrl::PreSubComboBoxControl(int Row, int Col, CListSubComboBox * pComboBox, const CRect& rect, const CString & CellText)
{
}

void CListSubCtrl::PreSubDialogControl(int Row, int Col, CListSubDialog * pDialog, const CRect& rect, const CString & CellText)
{
}

void CListSubCtrl::EndSubControl(int nRow, int nCol, LPTSTR String)
{
	Set_Cell(nRow, nCol, String);
}