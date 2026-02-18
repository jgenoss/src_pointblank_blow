#include "stdafx.h"
#include "i3TDKPropertySubItem.h"

#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( i3TDKPropertySubItem);

i3TDKPropertySubItem::i3TDKPropertySubItem( void)
{
	Reset();
}

i3TDKPropertySubItem::i3TDKPropertySubItem( SubItemInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

i3TDKPropertySubItem::~i3TDKPropertySubItem( void)
{
	
}

BOOL i3TDKPropertySubItem::Create( SubItemInitStruct * pInitStruct)
{
	SetID( pInitStruct->_id);
	SetStyle( pInitStruct->_style);
	SetLabel( pInitStruct->_label);
	SetUserData( pInitStruct->_data);
	SetRowIndex( pInitStruct->_row);
	SetColumnIndex( pInitStruct->_column);

	return TRUE;
}

void i3TDKPropertySubItem::Reset( void)
{
	m_Style = 0;
	m_ID = -1;
	m_Type = 0;
	m_szLabel[0] = '\0';
	m_colorFont = RGB(0, 0, 0);		//	기본 검정

	m_pUserData = nullptr;

	m_nRowIdx = -1;
	m_nColumnIdx = -1;
}

void i3TDKPropertySubItem::CopyTo( i3TDKPropertySubItem * pPropertySubItem)	
{	
	pPropertySubItem->SetStyle( GetStyle());
	pPropertySubItem->SetID( GetID());
	pPropertySubItem->SetType( GetType());
	pPropertySubItem->SetLabel( GetLabel());
	pPropertySubItem->SetUserData( GetUserData());

	pPropertySubItem->SetRowIndex( GetRowIndex());
	pPropertySubItem->SetColumnIndex( GetColumnIndex());
}

void i3TDKPropertySubItem::OnRelease( void)
{
	
}

void i3TDKPropertySubItem::OnUpdate( SubItemUpdateInfo * pInfo)
{
	//	Label 업데이트
	SetLabel( pInfo->_pszText);
}

void i3TDKPropertySubItem::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	HDC		hdc			= lpDrawItemStruct->hDC;
	//INT32	nItemID		= lpDrawItemStruct->itemID;
	//RECT	rtItem		= lpDrawItemStruct->rcItem;
	UINT32	nItemState	= lpDrawItemStruct->itemState;

	HBRUSH  brush = nullptr;
	UINT32 nTextAlign = DT_LEFT;	//	기본 텍스트 왼쪽 정렬

	//	아이템 BK 색상
	if( nItemState & ODS_SELECTED)
	{		
		brush = CreateSolidBrush( pColorSet->_ColorSelect);	
	}	
	else
	{
		brush = CreateSolidBrush( pColorSet->_ColorBK);	
	}

	FillRect( hdc, pRect, brush);

	SetBkMode( hdc, TRANSPARENT);	

	pRect->left += 3;
	
	//	Lable 출력
	const TCHAR* pszLabel = GetLabel();

	COLORREF OldColor = GetTextColor( hdc);
	//	Text Color
	if( m_colorFont != OldColor)
		SetTextColor( hdc, m_colorFont);

	if( GetStyle() & SUBITEM_STYLE_TEXTALIGN_LEFT)
		nTextAlign = DT_LEFT;
	else if( GetStyle() & SUBITEM_STYLE_TEXTALIGN_RIGHT)
		nTextAlign = DT_RIGHT;
	else if( GetStyle() & SUBITEM_STYLE_TEXTALIGN_CENTER)
		nTextAlign = DT_TOP;

	DrawText( hdc, pszLabel, i3::generic_string_size( pszLabel) , pRect, nTextAlign | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	if( m_colorFont != OldColor)
		SetTextColor( hdc, OldColor);

	DeleteObject( brush);
}

void i3TDKPropertySubItem::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
}

void i3TDKPropertySubItem::SetToolTipText( const TCHAR* pszText)
{
#ifdef _UNICODE
	m_wstrToolTip = pszText;
#else
	i3::mb_to_utf16( pszText, m_wstrToolTip);
#endif
}

void i3TDKPropertySubItem::SetLabel( const TCHAR * pLabel)	
{	
	if( pLabel != nullptr)	i3::safe_string_copy( m_szLabel, pLabel, SUBITEM_MAX_STRING);	
	else				i3::safe_string_copy( m_szLabel, _T(""), SUBITEM_MAX_STRING);	

	if( GetStyle() & SUBITEM_STYLE_LABELTOOLTIP)	SetToolTipText( GetLabel());
}

void i3TDKPropertySubItem::SetLabel( const I3COLOR * pColor)	
{	
	if( pColor)
		i3::snprintf( m_szLabel, SUBITEM_MAX_STRING, _T("RGB(%d, %d, %d, %d)"), pColor->r, pColor->g, pColor->b, pColor->a);	
	else
		i3::safe_string_copy( m_szLabel, _T("Invalid"), SUBITEM_MAX_STRING);	

	if( GetStyle() & SUBITEM_STYLE_LABELTOOLTIP)	SetToolTipText( GetLabel());
}

void i3TDKPropertySubItem::SetLabel( const REAL32	val)	
{	
	i3::snprintf( m_szLabel, SUBITEM_MAX_STRING, _T("%f"), val);		

	if( GetStyle() & SUBITEM_STYLE_LABELTOOLTIP)	SetToolTipText( GetLabel());
}

void i3TDKPropertySubItem::SetLabel( const UINT32	val)
{	
	i3::snprintf( m_szLabel, SUBITEM_MAX_STRING, _T("%d"), val);		

	if( GetStyle() & SUBITEM_STYLE_LABELTOOLTIP)	SetToolTipText( GetLabel());
}

void i3TDKPropertySubItem::SetLabel( const INT32	val)	
{	
	i3::snprintf( m_szLabel, SUBITEM_MAX_STRING, _T("%d"), val);		
	
	if( GetStyle() & SUBITEM_STYLE_LABELTOOLTIP)	SetToolTipText( GetLabel());
}


