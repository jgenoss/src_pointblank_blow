#include "stdafx.h"
#include "i3TDKPropertySubItemComboBox.h"
#include "i3Base/i3StringListBuffer.h"

I3_CLASS_INSTANCE( i3TDKPropertySubItemComboBox);

i3TDKPropertySubItemComboBox::i3TDKPropertySubItemComboBox( SubItemComboBoxInitStruct * pInitStruct)
{
	Create( pInitStruct);
}

i3TDKPropertySubItemComboBox::~i3TDKPropertySubItemComboBox( void)
{
}

BOOL i3TDKPropertySubItemComboBox::Create( SubItemComboBoxInitStruct * pInitStruct)
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

	ClearElement();

	return i3TDKPropertySubItem::Create( &init);
}

void i3TDKPropertySubItemComboBox::Reset()
{
	i3TDKPropertySubItem::Reset();

	m_WndStyle = 0;
	m_nSelectIdx = 0;

	ClearElement();
}

void i3TDKPropertySubItemComboBox::CopyTo( i3TDKPropertySubItemComboBox * pSubItem)
{
//	INT32 BufferSize = GetStringListBufferSize(m_ElementList);
//	char * pBuffer = (char *) i3MemAlloc(BufferSize );		// i3MemAlloc( m_ElementList.GetBufferSize());
//	CopyFromStringListToBuffer(m_ElementList, pBuffer, BufferSize );
//	CopyFromBufferToStringList(pSubItem->GetElementList(), pBuffer);
//	I3MEM_SAFE_FREE( pBuffer);

	pSubItem->m_ElementList = this->m_ElementList;

	i3TDKPropertySubItem::CopyTo( pSubItem);
}

void i3TDKPropertySubItemComboBox::OnUpdate( SubItemUpdateInfo * pInfo)
{
	TCHAR szTemp[256];

	m_nSelectIdx = m_ComboBox.GetSelectedIdx();
	m_ComboBox.GetElement( m_nSelectIdx, szTemp);

	SetLabel( szTemp);
}

void i3TDKPropertySubItemComboBox::OnRelease( void)
{
	ClearElement();
	m_ComboBox.DestroyWindow();

	i3TDKPropertySubItem::OnRelease();
}

void i3TDKPropertySubItemComboBox::OnDraw( LPDRAWITEMSTRUCT lpDrawItemStruct, RECT * pRect, SubItemColorSet * pColorSet)
{
	if( m_ComboBox.IsActive())	return;					//	Edit가 활성화 되면 기본 Item은 Draw할 필요없다.

	i3TDKPropertySubItem::OnDraw( lpDrawItemStruct, pRect, pColorSet); 
}

void i3TDKPropertySubItemComboBox::OnHit( CWnd * pParentWnd, RECT * pRect, DWORD dwStyle)
{
	//	ComboBox 생성
	if( m_ComboBox.Create( m_WndStyle | dwStyle | WS_VISIBLE, *pRect, pParentWnd, m_ID) == FALSE)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not create combobox window.");
	}

	//	ComboxBox 초기화
	LCCBInitStruct InitStruct;
	
	InitStruct._nItem = m_nRowIdx;
	InitStruct._nSubItem = m_nColumnIdx;
	InitStruct._nType = LCCB_KILLFOCUSDESTROY;

	m_ComboBox.Init( &InitStruct); 
	
	INT32 x = pRect->left;
	INT32 y = pRect->top;
	INT32 w = pRect->right - pRect->left;
	INT32 h = pRect->bottom - pRect->top;
	INT32 v = 0;

	if( m_WndStyle & CB_WNDSTYLE_DISABLENOSCROLL)
		v = h * (INT32)m_ElementList.size();

	m_ComboBox.MoveWindow( x, y, w, h + v);

	//	ComboBox Element 추가
	for( size_t i = 0; i < m_ElementList.size(); i++)
	{
		m_ComboBox.AddElement( m_ElementList[i].c_str() );
	}
	m_ComboBox.SelectElement( m_nSelectIdx);
	m_ComboBox.ShowDropDown( TRUE);

	//I3TRACE( "i3TDKPropertySubItemComboBox::OnHit\n");
}


