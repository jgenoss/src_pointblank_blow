
// SolFS_TestDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "HListBox.h"

///////////////////////////////
/* CHList Box */
void CHListBox::updateWidth(LPCTSTR s)
{
	CClientDC dc(this);
	CFont * f = CListBox::GetFont();
	dc.SelectObject(f);
	CSize sz = dc.GetTextExtent(s, (int)_tcslen(s));
	sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
	if(sz.cx > m_Width)
	{ /* extend */
		m_Width = sz.cx;
		CListBox::SetHorizontalExtent(m_Width);
	} /* extend */

	SetTopIndex( GetCount()-1 );

}

int CHListBox::AddString(LPCTSTR s)
{
	int result = CListBox::AddString(s);
	if(result < 0)
		return result;
	updateWidth(s);

	m_OrderList.push_back( m_OrderList.size() );

	return result;
}

int CHListBox::AppendString(LPCTSTR s)
{
	int size = GetCount();

	return InsertString(size, s);
}


int CHListBox::InsertString(int i, LPCTSTR s)
{
	int result = CListBox::InsertString(i, s);
	if(result < 0)
		return result;
	updateWidth(s);

	m_OrderList.push_back( m_OrderList.size() );

	return result;
}

void CHListBox::ResetContent()
{
	CListBox::ResetContent();
	m_Width = 0;
	m_OrderList.clear();
}

void CHListBox::SetCurrentSel(int index)
{
	int size = GetCount();
	if( size == 0 )
		return;
	else if( size <= index )
		CListBox::SetCurSel(index-1);
	else
		CListBox::SetCurSel(index);
}


int CHListBox::GetCurrentSel() const
{
	return CListBox::GetCurSel();
}

int CHListBox::DeleteString(int n)
{
	int result = CListBox::DeleteString(n);
	if(result < 0)
		return result;
	CClientDC dc(this);

	CFont * f = CListBox::GetFont();
	dc.SelectObject(f);

	m_Width = 0;
	for(int i = 0; i < CListBox::GetCount(); i++)
	{ /* scan strings */
		CString s;
		CListBox::GetText(i, s);
		CSize sz = dc.GetTextExtent(s);
		sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
		if(sz.cx > m_Width)
			m_Width = sz.cx;
	} /* scan strings */
	CListBox::SetHorizontalExtent(m_Width);

	SetCurrentSel(n);

	return result;
}

CString CHListBox::GetCurrentString(int nIndex)
{
	CString str;

	int size = GetCount();
	if( nIndex < size )
		GetText(nIndex, str);

	return str;
}

CString CHListBox::GetCurrentString()
{
	//위치를 얻어 온다.
	int index = GetCurrentSel();

	if( index == -1 )
		return CString();

	CString str;
	GetText(index, str);
	return str;
}

void CHListBox::DeleteCurrentItem()
{
	int index = GetCurrentSel();

	if( index == -1 )
		return;

	DeleteString(index);
}

StringVector CHListBox::GetAllStrings() const
{
	StringVector list;
	int size = GetCount();
	for(int i=0;i<size;i++)
	{
		CString str;
		GetText(i, str);
		list.push_back( str.GetString() );
	}
	return list;
}

void CHListBox::_UpString()
{
	int index = GetCurrentSel();
	if( index == 0 )
		return;

	CString str = GetCurrentString(index);
	DeleteString(index);

	CListBox::InsertString(index-1, str);
	CListBox::SetCurSel(index);

	std::swap( m_OrderList[index], m_OrderList[index-1] );
}

void CHListBox::_DownString()
{
	int index = GetCurrentSel();
	if( index+1 == GetCount() )
		return;

	CString str = GetCurrentString(index);
	DeleteString(index);

	CListBox::InsertString(index+1, str);
	CListBox::SetCurSel(index);

	std::swap( m_OrderList[index], m_OrderList[index+1] );
}

BOOL CHListBox::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_UP:
			if (IsCTRLpressed())
			{
				if( GetCurrentSel() == -1 )
					break;

				if( m_bEnableSwap )
					_UpString();
			}
			break;
		case VK_DOWN:
			if (IsCTRLpressed())
			{
				if( GetCurrentSel() == -1 )
					break;

				if( m_bEnableSwap )
					_DownString();
			}
			break;

		case VK_DELETE:
				DeleteCurrentItem();
			break;
		}
		break;
	}

	return CListBox::PreTranslateMessage(pMsg);
}
