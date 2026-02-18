#include "stdafx.h"
#include "i3TDKPropertySubItemColor.h"
#include "i3TDKColorPicker.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemColor);

i3TDKPropertySubItemColor::i3TDKPropertySubItemColor( void)
{
	Reset();
}

i3TDKPropertySubItemColor::i3TDKPropertySubItemColor( SubItemColorInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

i3TDKPropertySubItemColor::~i3TDKPropertySubItemColor( void)
{
}


BOOL i3TDKPropertySubItemColor::Create( SubItemColorInitStruct * pInitStruct)
{	
	Reset();

	SubItemInitStruct init;
	init._id = pInitStruct->_id;
	init._row = pInitStruct->_row;
	init._column = pInitStruct->_column;
	init._style = pInitStruct->_style;
	init._label = pInitStruct->_label;
	init._data = pInitStruct->_data;

	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemColor::Reset()
{
	i3TDKPropertySubItem::Reset();

	m_Color.r = 0;
	m_Color.g = 0;
	m_Color.b = 0;
	m_Color.a = 255;
}

void i3TDKPropertySubItemColor::CopyTo( i3TDKPropertySubItemColor * pSubItem)
{
	pSubItem->SetColor( GetColor());

	i3TDKPropertySubItem::CopyTo( pSubItem);
}

void i3TDKPropertySubItemColor::OnRelease( void)
{
	i3TDKPropertySubItem::OnRelease();
}

void i3TDKPropertySubItemColor::OnUpdate( SubItemUpdateInfo * pInfo)
{
	i3TDKPropertySubItem::OnUpdate( pInfo);
}

void i3TDKPropertySubItemColor::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	HDC		hdc			= lpDrawItemStruct->hDC;
	UINT32	nItemState	= lpDrawItemStruct->itemState;

	HBRUSH  brush = nullptr;
	UINT32 nTextAlign = DT_LEFT;	//	기본 텍스트 왼쪽 정렬
	CRect	rtBox;	

	//	아이템 BK 색상
	if( nItemState & ODS_SELECTED)
	{		
		brush = CreateSolidBrush( pColorSet->_ColorSelect);	
	}	
	else
	{
		brush = CreateSolidBrush( pColorSet->_ColorBK);	
	}

	FillRect( hdc, pRect, brush);	//	배경색

	//	텍스트 앞에 색상 상자를 그린다.
	{		
		rtBox.left = pRect->left + 3; 
		rtBox.top = pRect->top + 1;
		rtBox.right = pRect->left + 30;
		rtBox.bottom = pRect->bottom - 1;
		
		COLORREF	colorRect = RGB( m_Color.r, m_Color.g, m_Color.b);	
		HBRUSH		colorBrush = CreateSolidBrush( colorRect);
		HBRUSH		colorOldBrush = (HBRUSH) SelectObject( hdc, colorBrush);

		//FillRect( hdc, &colorArea, colorBrush);
		Rectangle( hdc, rtBox.left, rtBox.top, rtBox.right, rtBox.bottom);
		
		SelectObject( hdc, colorOldBrush);
		DeleteObject( colorBrush);
	}

	SetBkMode( hdc, TRANSPARENT);	

	pRect->left = rtBox.right + 5;
	
	//	Lable 출력
	const TCHAR* pszLabel = GetLabel();

	DrawText( hdc, pszLabel, i3::generic_string_size(pszLabel), pRect, nTextAlign | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	DeleteObject( brush);		
}

void i3TDKPropertySubItemColor::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
	//CHOOSECOLOR color;
	//memset( &color, 0, sizeof( CHOOSECOLOR));
	//color.lStructSize = sizeof( CHOOSECOLOR);
	//color.hwndOwner = pParentWnd->GetSafeHwnd();  
	const TCHAR* pszLabel = nullptr;

	//	색상 공용 컨트롤로 입력	

	i3TDKColorPicker Dlg;
	//CColorDialog Dlg( 0, 0, pParentWnd);
	//Dlg.SetCurrentColor( RGB( m_Color.r, m_Color.g, m_Color.b));
	
	COLORREAL color;
	i3Color::Set( &color, &m_Color);

	if( Dlg.Execute( &color))
	//if( ChooseColor( &color))
	{
		//	선택한 색상으로 변경
		//COLORREF SelectedColor = *color.lpCustColors;

		i3Color::Set( &m_Color, Dlg.getSelectedColor());
		/*
		COLORREF SelectedColor = Dlg.GetColor();
		m_Color.r = GetRValue( SelectedColor);
		m_Color.g = GetGValue( SelectedColor);
		m_Color.b = GetBValue( SelectedColor);
		*/

		SetLabel( &m_Color);
		pszLabel = GetLabel();

		//	부모 윈도우에세 LVN_ENDLABELEDIT 이벤트를 보낸다.
		LV_DISPINFO		lvdi;
		lvdi.hdr.hwndFrom	= pParentWnd->GetSafeHwnd();
		lvdi.hdr.idFrom		= pParentWnd->GetDlgCtrlID();
		lvdi.hdr.code		= LVN_ENDLABELEDIT;

		lvdi.item.mask		= LVIF_TEXT;
		lvdi.item.iItem		= m_nRowIdx;
		lvdi.item.iSubItem	= m_nColumnIdx;
		lvdi.item.pszText	= (TCHAR*)pszLabel;
		lvdi.item.cchTextMax= i3::generic_string_size( pszLabel);

		pParentWnd->SendMessage( WM_NOTIFY, 0, (LPARAM) &lvdi); 

		pParentWnd->SetFocus();	// KillFocus
	}	
}

void i3TDKPropertySubItemColor::SetLabel( const I3COLOR * pColor)	
{	
	if( pColor != nullptr)
		SetColor( (I3COLOR *) pColor);

	i3TDKPropertySubItem::SetLabel( pColor);
}

