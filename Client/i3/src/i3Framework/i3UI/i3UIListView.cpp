#include "i3FrameworkPCH.h"
#include "i3UIListView.h"
#include "i3UIManager.h"	
#include "i3UITemplate_ListView.h"
#include "../i3Framework.h"

I3_CLASS_INSTANCE( i3UIListView, i3UICaptionControl);

i3UIListView::i3UIListView()
{
	m_fWidthScrollBar = 20.0f;
	
	m_pUIScrollBar = NULL;
}

i3UIListView::~i3UIListView()
{
	I3_SAFE_RELEASE( m_pUIScrollBar);
}

void i3UIListView::_UpdateScrollBarValue( void)
{
	I3ASSERT( m_pUIScrollBar != NULL);

	INT32 fontHeight = m_strList.GetCount();
	if( m_pText != NULL)
	{
		fontHeight *= m_pText->getDefFontHeight();
	}

	if( fontHeight <= (INT32)(m_pRects[(INT32)I3UI_ListView_SHAPE_CLIENTAREA].bottom))
	{
		m_pUIScrollBar->EnableCtrl( false);
		return;
	}
	else
	{
		if( m_pUIScrollBar->isEnable() == false)
		{
			m_pUIScrollBar->EnableCtrl( true);
		}
	}

	//Range
	m_pUIScrollBar->setScrollRange( 0, fontHeight);

	//Page Size
	INT32 ListViewH = (INT32)(m_pRects[(INT32)I3UI_ListView_SHAPE_CLIENTAREA].bottom);
	m_pUIScrollBar->setPageSize( ListViewH);

	//Cur Pos
	m_pUIScrollBar->setScrollPos( m_TextPosY);

	_UpdateScrollBarRect();
}

INT32 i3UIListView::_GetItemIndexByPos( INT32 x, INT32 y)	//»ó´ëÁÂÇ¥
{
	if( m_pText == NULL)
		return -1;
	
	INT32 fFontHeight = m_pText->getDefFontHeight();

	INT32 y1 = -m_pText->getShowY();
	
	for( INT32 i=0; i< m_strList.GetCount(); i++)
	{
		if( y >= y1 && y < y1 + fFontHeight)
			return i;

		y1 += fFontHeight;
	}
	
	return -1;
}


I3RECT i3UIListView::_GetItemRectByIndex( INT32 idx)	// »ó´ëÁÂÇ¥
{
	I3RECT rt = {0.0f, };
	
	if( m_pText == NULL)
		return rt;
	
	REAL32 fFontHeight = (REAL32)(m_pText->getDefFontHeight());
	
	if( idx < m_strList.GetCount())
	{
		rt.left = 0.0f;
		rt.top = m_pText->getShowY() + fFontHeight * idx;
		
		rt.right = getWidth() - m_pUIScrollBar->getWidth();
		rt.bottom = rt.top + fFontHeight;
	}

	return rt;
}

void i3UIListView::_SetBlock( I3RECT* pRt)
{
	//I3TRACE( "i3UIListView::_SetBlock: %f, (right)%f, %f, (bottom)%f\n", pRt->left, pRt->right, pRt->top, pRt->bottom);
}

void i3UIListView::OnBindTemplate(void)
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == NULL)
		return;

	I3ASSERT( m_pUIScrollBar == NULL);

	m_pUIScrollBar = i3UIScrollBar::NewObject();
	m_pUIScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_ListView_SUBTEMPLATE_VSCROLL));
	
	AddChild( m_pUIScrollBar);

	m_pUIScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	m_pUIScrollBar->addStyle( I3UI_STYLE_SLAVE);
}


void i3UIListView::_UpdateText( void)
{
	if( m_pText == NULL)
		return;

	// Scroll..
	m_pText->SetShowPos( 0, m_TextPosY);
}

void i3UIListView::_UpdateShapeState(void)
{
	INT32 i;
	for( i = 0; i < I3UI_ListView_SHAPE_DISABLE_CLIENTAREA; ++i)
	{
		setShapeVisible( i, isValidShape(i));
	}

	if( !isActivated())
	{
		setShapeVisible( I3UI_ListView_SHAPE_DISABLE_CLIENTAREA, true);
		setShapeVisible( I3UI_ListView_SHAPE_CLIENTAREA, false);
	}

	_UpdateText();
	_UpdateScrollBarValue();
}

void i3UIListView::_UpdateScrollBarRect( void)
{
	REAL32 x = getWidth() - m_fWidthScrollBar;
	REAL32 y = 0.0f;

	m_pUIScrollBar->setSize( m_fWidthScrollBar, getHeight());
	m_pUIScrollBar->setPos( x, y);
}

void i3UIListView::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);

	if( bEnable)
	{
		m_bNeedUpdate = true;
	}
	else
	{
		if( m_pUIScrollBar != NULL)	m_pUIScrollBar->EnableCtrl( bEnable);
	}
}

void i3UIListView::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonDown( nFlag, point);

	INT32 x = point.x;
	INT32 y = point.y;

	if( m_pRects == NULL || m_pTemplate == NULL)
		return;

	I3RECT rect;
	m_pTemplate->GetTextArea( m_pRects, &rect);

	REAL32 left		= m_AccPos.x + rect.left;
	REAL32 top		= m_AccPos.y + rect.top;
	REAL32 right	= left + rect.right;
	REAL32 bottom	= top + rect.bottom;
	
	if( x > left && x < right)
	{
		if( y > top && y < bottom)
		{
			INT32 idx = _GetItemIndexByPos( x - (INT32)left, y - (INT32)top);		//»ó´ëÁÂÇ¥
			if( idx != -1)
			{
				I3RECT rt = _GetItemRectByIndex( idx);				//»ó´ëÁÂÇ¥

				rt.left += left;
				rt.top += top;
				rt.right += left;	// right
				rt.bottom += top;	// bottom

				_SetBlock( &rt);

				ParentNotify( I3UI_EVT_SELITEM, idx, 0);
			}
		}
	}
}

void i3UIListView::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	m_TextPosY = 0;

	_UpdateText();
	_UpdateScrollBarValue();
}

void i3UIListView::OnNotify( i3UIControl * pCtrl, UINT32 event, INT32 param1, INT32 param2)
{
	if( event == I3UI_EVT_SCROLL)
	{
		m_TextPosY = param1;

		_UpdateText();
	}

	i3UICaptionControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIListView::GetItem( INT32 idx, char* pBuf, INT32 nBufSize)
{
	I3_BOUNDCHK( idx, m_strList.GetCount());

	i3String::NCopy( pBuf, (const char*)m_strList.GetItem( idx), nBufSize);
	//i3String::CopyASCIItoWCHAR( pBuf, (const char*)(m_strList.GetItem(idx)) );
}

void i3UIListView::UpdateRenderObjects()
{
	i3UICaptionControl::UpdateRenderObjects();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tag_i3Persist_UIListView
{
	UINT8		m_ID[4];
	UINT32		pad[32];
}I3_PERSIST_UILIST_VIEW;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIListView::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_UILIST_VIEW data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof( data));
	i3mem::Copy( data.m_ID, "ULB1", 4);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIListView::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_UILIST_VIEW data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

//Combo ¹Ø¿¡ ÀÖ´Â ListView ÀúÀåÀÌ ¾ÈµÊ..
bool i3UIListView::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;
	

	pXML->addAttr( "Items", m_Items);


	return bResult;
}

bool i3UIListView::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	const char* temp = pXML->getAttr( "Items");
	i3String::NCopy( m_Items, temp, sizeof( m_Items));

	//AddItemsToStringList();

	return bResult;
}

bool i3UIListView::GetTextRect( I3RECT* pRect)
{
	bool bResult = false;

	if( m_pText != NULL && m_pTemplate != NULL)
	{
		I3RECT rt;
		bResult = m_pTemplate->GetTextArea( m_pRects, &rt);

		if( bResult)
		{
			rt.left += m_AccPos.x;
			rt.top	+= m_AccPos.y;

			if( m_pUIScrollBar->isEnable())
			{
				I3RECT ScrollRt;
				m_pUIScrollBar->GetScrollRect( &ScrollRt);

				if( ScrollRt.left < rt.left + rt.right)	// right°¡ width..
				{
					rt.right -= (rt.left + rt.right - ScrollRt.left);
				}
			}
		}

		*pRect = rt;
	}

	return bResult;
}

void i3UIListView::AddItemsToStringList( void)
{
	m_strList.Clear();

	char* pCur = m_Items;
	char pBuf[64];
	INT32 i = 0;

	while( *pCur != 0)
	{
		if( *pCur == ';')
		{
			pBuf[i] = 0;
			m_strList.Add( pBuf);
			i = 0;
		}
		else
		{
			pBuf[i] = *pCur;
			i++;
		}
		
		pCur++;
	}

	if( i > 0)
	{
		pBuf[i] = 0;
		m_strList.Add( pBuf);
	}
	

	char temp[32];
	char buf[256];
	INT32 pos = 0;
	INT32 len = 0;

	for( INT32 i=0; i < m_strList.GetCount(); i++)
	{
		const char* pStr = (const char*)m_strList.GetItem( i);
		
		i3String::NCopy( temp, pStr, sizeof( temp));
		len = i3String::Length( temp);
		//i3String::CopyASCIItoWCHAR( temp, pStr);
		//len = i3String::LengthW( temp);

		if( i==0)
			i3String::NCopy( buf, temp, len);
		else
			i3String::InsertStr( buf, temp, pos, len);

		pos += len;

		i3String::InsertStr( buf, "\n", pos, 1);
		pos++;
	}

	WCHAR16 pDest[256];
	MultiByteToWideChar( CP_ACP, MB_COMPOSITE, buf, sizeof( buf), pDest, sizeof( pDest));
	SetText( pDest);

	ParentNotify( I3UI_EVT_SELITEM, 0, 0);
}

void i3UIListView::Init( void)
{
	i3UICaptionControl::Init();

	AddItemsToStringList();
}

void i3UIListView::OnInitAfterLoad()
{
	i3UICaptionControl::OnInitAfterLoad();

	AddItemsToStringList();
}

void i3UIListView::GetTDKGridProperties(i3List * pList)
{
	i3UICaptionControl::GetTDKGridProperties( pList);

	PROPERTYINFO* pProp = GetGridProp_BeginGroup( "i3UIListView");
	pList->Add( pProp);

	pProp = GetGridProp_String( m_Items, sizeof( m_Items), "Items");
	pList->Add( pProp);

	pProp = GetGridProp_EndGroup();
	pList->Add( pProp);
}

void i3UIListView::ApplyChangesFromPropertyGrid( void)
{
	i3UICaptionControl::ApplyChangesFromPropertyGrid();

	// m_Items
	AddItemsToStringList();
}

void i3UIListView::GetItems( char* pBuf)
{
	i3String::NCopy( pBuf, m_Items, sizeof( m_Items));
}

void i3UIListView::SetItems( char* pItems)
{
	i3String::NCopy( m_Items, pItems, sizeof( m_Items));
	AddItemsToStringList();
}

//ListView
// Disable Ã³¸®
// Text º¹»ç Çö»ó( ÀÜ»ó³²À½..)
