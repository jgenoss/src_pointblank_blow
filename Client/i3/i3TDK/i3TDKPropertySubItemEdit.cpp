#include "stdafx.h"
#include "i3TDKPropertySubItemEdit.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemEdit);

i3TDKPropertySubItemEdit::i3TDKPropertySubItemEdit( SubItemEditInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

BOOL i3TDKPropertySubItemEdit::Create( SubItemEditInitStruct * pInitStruct)
{
	Reset();

	SubItemInitStruct init;
	init._id = pInitStruct->_id;
	init._row = pInitStruct->_row;
	init._column = pInitStruct->_column;
	init._style = pInitStruct->_style;   
	init._label = pInitStruct->_label;
	init._data = pInitStruct->_data;

	m_WndStyle = pInitStruct->_WndStyle;
		
	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemEdit::Reset()
{
	i3TDKPropertySubItem::Reset();
}

void i3TDKPropertySubItemEdit::CopyTo( i3TDKPropertySubItemEdit * pSubItem)
{
	i3TDKPropertySubItem::CopyTo( pSubItem);
}

void i3TDKPropertySubItemEdit::OnUpdate( SubItemUpdateInfo * pInfo)
{
	TCHAR szTemp[SUBITEM_MAX_STRING];
	if( m_Style & EDIT_STYLE_FLOATING_POINT)
	{
		i3::snprintf( szTemp, SUBITEM_MAX_STRING, _T("%f"), (REAL32) i3::atof(pInfo->_pszText));
	}
	else if( m_Style & EDIT_STYLE_INTEGER)
	{
		i3::snprintf( szTemp, SUBITEM_MAX_STRING, _T("%d"), (INT32) i3::atoi(pInfo->_pszText));
	}
	else
	{
		i3::string_ncopy_nullpad( szTemp, pInfo->_pszText, 1024 );
	}

	//	Label 업데이트
	SetLabel( szTemp);
}

void i3TDKPropertySubItemEdit::OnRelease( void)
{
	m_Edit.DestroyWindow();

	i3TDKPropertySubItem::OnRelease();
}

void i3TDKPropertySubItemEdit::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	if( m_Edit.IsActive())	return;					//	Edit가 활성화 되면 기본 Item은 Draw할 필요없다.
	
	i3TDKPropertySubItem::OnDraw( lpDrawItemStruct, pRect, pColorSet); 
}

void i3TDKPropertySubItemEdit::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
	//	EditBox 생성
	if( m_Edit.Create( m_WndStyle | dwStyle | WS_VISIBLE, *pRect, pParentWnd, m_ID) == FALSE)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not create edit window.");
	}

	//	EditBox 초기화
	LCEBInitStruct InitStruct;
	
	InitStruct._nItem = m_nRowIdx;
	InitStruct._nSubItem = m_nColumnIdx;
	InitStruct._pszInitText = GetLabel();
	InitStruct._nType = LCEB_FISRTSELECTION | LCEB_KILLFOCUSDESTROY;
	
	m_Edit.Init( &InitStruct);	

	//I3TRACE( "i3TDKPropertySubItemEdit::OnHit\n");
}

