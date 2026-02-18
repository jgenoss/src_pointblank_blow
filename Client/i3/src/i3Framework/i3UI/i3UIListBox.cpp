#include "i3FrameworkPCH.h"
#include "i3UIListBox.h"
#include "i3UIManager.h"	
#include "i3UITemplate_ListBox.h"
#include "../i3Framework.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/itl/range/algorithm/find.h"

I3_CLASS_INSTANCE( i3UIListBox);

i3UIListBox::i3UIListBox() 
{
	setTextAutoLineFeed(true);
}

i3UIListBox::~i3UIListBox()
{
	I3_SAFE_RELEASE( m_pUIScrollBar);

	Clear();
}

bool i3UIListBox::Clear( void)
{
	m_strListBox.clear();

	_SetText();

	//Shape disable
	m_nCurSel = -1;
	_UpdateShapeSelected( m_nCurSel);

	m_nCurMouseOn = -1;
	_UpdateShapeOnMouse( m_nCurMouseOn);

	//ScrollPos to 0
	if( m_pUIScrollBar != nullptr)
	{
		m_pUIScrollBar->EnableCtrl( false);
	}

	return true;
}

INT32 i3UIListBox::_AddItem( const WCHAR16 * pszItem)
{
	m_strListBox.push_back( pszItem);
	return m_strListBox.size() - 1;
}

INT32 i3UIListBox::AddItem( const WCHAR16* pszItem)
{
	INT32 nResult = _AddItem( pszItem);

	_SetText();

	return nResult;
}

INT32 i3UIListBox::InsertItem( INT32 nIdx, const WCHAR16* pszItem)
{
	m_strListBox.insert(m_strListBox.begin() + nIdx, pszItem);

	INT32 nResult = nIdx;
	_SetText();
	return nResult;
}

bool i3UIListBox::RemoveItem( const WCHAR16* pszItem)
{
	const size_t num = m_strListBox.size();

	for( size_t i=0; i< num; ++i )
	{
		if( i3::generic_is_equal( pszItem, m_strListBox[i] ) )				// iequal -> equal로 변경 
		{
			m_strListBox.erase(m_strListBox.begin() + i);
			_SetText();
			return true;
		}
	}

	return false;
}

bool i3UIListBox::DeleteItem( INT32 nIdx)
{
	//index check
	if( nIdx >= 0 && nIdx < (INT32)m_strListBox.size())
	{
		m_strListBox.erase( m_strListBox.begin() + nIdx);
		_SetText();
		return true;
	}

	return false;
}

bool i3UIListBox::GetItem( INT32 nIdx, WCHAR16* pBuf, INT32 nBufSize)
{
	if( nIdx >= 0 && nIdx < (INT32)m_strListBox.size())
	{
		i3::safe_string_copy( pBuf, m_strListBox[nIdx], nBufSize);
		return true;
	}
	else
	{
		pBuf[0] = 0;
	}

	return false;
}

bool i3UIListBox::SetItem( INT32 nIdx, const WCHAR16* pszItem)
{
	if( nIdx >= 0 && nIdx < (INT32)m_strListBox.size())
	{
		m_strListBox[nIdx] = pszItem;
		return true;
	}

	return false;
}



void i3UIListBox::_UpdateScrollBarValue( void)
{
	if( m_pUIScrollBar == nullptr)
		return;

	INT32 fontHeight = (INT32)m_strListBox.size();			
	if( m_pText != nullptr)
	{
		fontHeight  = m_pText->getMaxLine();
		fontHeight *= m_pText->getDefFontHeight();
	}

	if( fontHeight <= (INT32)(m_pRects[(INT32)I3UI_LISTBOX_SHAPE_CLIENTAREA].bottom))
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
	INT32 listBoxH = (INT32)(m_pRects[(INT32)I3UI_LISTBOX_SHAPE_CLIENTAREA].bottom);
	m_pUIScrollBar->setPageSize( listBoxH);

	//Cur Pos
	m_pUIScrollBar->setScrollPos( m_TextPosY);

	_UpdateScrollBarRect();
}

void i3UIListBox::_UpdateScrollBarRect( void)
{
	if( m_pUIScrollBar != nullptr)
	{
		REAL32 x = getWidth() - m_fWidthScrollBar;
		REAL32 y = 0.0f;

		m_pUIScrollBar->SetSizeNoNotify( m_fWidthScrollBar, getHeight()-m_fMarginDownScrollBar);
		m_pUIScrollBar->SetPosNoNotify( x-m_fMarginRightScrollBar, y + m_fMarginUpScrollBar);
	}
}

INT32 i3UIListBox::_GetItemIndexByPos( POINT point)	
{
	if( m_pRects == nullptr || m_pTemplate == nullptr || m_pText == nullptr)
		return -1;

	INT32 x = point.x;
	INT32 y = point.y;

	i3::pack::RECT rect;
	m_pTemplate->GetTextArea( m_pRects, &rect);

	REAL32 left		= m_AccPos.x + rect.left;
	REAL32 top		= m_AccPos.y + rect.top;

	REAL32 right	= left + rect.right;
	
	if( m_pUIScrollBar != nullptr && m_pUIScrollBar->isEnable())
		right = left + getWidth() - m_pUIScrollBar->getWidth();
	
	REAL32 bottom	= top + rect.bottom;
	
	if( x > left && x < right)
	{
		if( y > top && y < bottom)
		{
			INT32 fFontHeight		= m_pText->getDefFontHeight();
		
			INT32 y1 = -m_pText->getShowY();

			//상대좌표
			x = x - (INT32)left;	
			y = y - (INT32)top;
			
			const INT32 numCarrage = m_pText->getNumCarriageLine();
			I3ASSERT(numCarrage == (INT32)m_strListBox.size());
			
			if (!this->isTextAutoLineFeed())
			{
				for( INT32 i=0; i< numCarrage ; ++i )
				{
					if( y >= y1 && y < y1 + fFontHeight)
						return i;
					y1 += fFontHeight;
				}
			}
			else
			{
				for( INT32 i=0; i< numCarrage ; ++i )
				{
					INT32 numLineFeed = m_pText->getNumLineFeedOfCarriageLine(i);
					INT32	thisFontHeight = numLineFeed * fFontHeight;

					if( y >= y1 && y < y1 + thisFontHeight)
						return i;

					y1 += thisFontHeight;
				}
			}
		}
	}
	
	return -1;
}


// 여기서 리스트박스는 SpaceOfLettersY와 linePitch가 고려되지 않았던 상태이며, 일단 그것을 그대로 방치하기로 한다. (2015.01.05.수빈)

//아이템의 OnMouse 상태, Selected 상태를 표현하기 위해
//아이템의 Rect를 상대좌표로 반환한다. 
//아이템이 잘려져 있는 것까지 계산한다. 
i3::pack::RECT i3UIListBox::_GetItemRectByIndex( INT32 idx)	
{
	i3::pack::RECT rt;	//rt는 left, top, right, bottom
	
	if( idx < 0 || idx >= (INT32)m_strListBox.size())
		return rt;

	if( m_pText == nullptr)
		return rt;
	
	REAL32 fFontHeight = (REAL32)(m_pText->getDefFontHeight());		//default...

	i3::pack::RECT rect;
	m_pTemplate->GetTextArea( m_pRects, &rect);	//rect는 left, top, width, height
	
	//left
	rt.left = rect.left;

	//right
	if( m_pUIScrollBar->isEnable())
		rt.right = getWidth() - m_pUIScrollBar->getWidth()- m_fMarginRightScrollBar;
	else
		rt.right = rt.left + rect.right;	//rect.right => width of text area

	// top과 bottom은 수정되어야한다.
	if (!this->isTextAutoLineFeed() )
	{
		//top
		rt.top = rect.top + fFontHeight * idx - m_TextPosY;
		//bottom
		rt.bottom = rt.top + fFontHeight;
	}
	else	// 멀티라인의 경우 단순한 폰트높이로 처리되지 않음..
	{
		// top
		rt.top = rect.top;
		I3ASSERT((INT32)m_strListBox.size() == m_pText->getNumCarriageLine() );

		for ( INT32 i = 0 ; i < idx ; ++i )
		{
			INT32 numFeed = m_pText->getNumLineFeedOfCarriageLine(i);
			rt.top += fFontHeight * numFeed;
		}
		rt.top -= m_TextPosY;
		
		// bottom
		INT32 numFeed =	m_pText->getNumLineFeedOfCarriageLine(idx);
		rt.bottom = rt.top + numFeed * fFontHeight;
	}

	//rt.top = max( rt.top, rect.top);
	//rt.bottom = min( rt.bottom, rect.top + rect.bottom);
	rt.top = MINMAX( rect.top, rt.top, rect.top + rect.bottom);
	rt.bottom = MINMAX( rect.top, rt.bottom, rect.top + rect.bottom);
	
	return rt;
}


void i3UIListBox::_UpdateShapeState(void)
{
	i3UICaptionControl::_UpdateShapeState();

	INT32 i;
	for( i = 0; i < I3UI_LISTBOX_SHAPE_DISABLE_CLIENTAREA; ++i)
	{
		setShapeVisible( i, isValidShape(i));
	}

	if( !isActivated())
	{
		setShapeVisible( I3UI_LISTBOX_SHAPE_DISABLE_CLIENTAREA, true);
		setShapeVisible( I3UI_LISTBOX_SHAPE_CLIENTAREA, false);
	}
	else
	{
		setShapeVisible( I3UI_LISTBOX_SHAPE_DISABLE_CLIENTAREA, false);
		setShapeVisible( I3UI_LISTBOX_SHAPE_CLIENTAREA, true);
	}

	_UpdateText();
	
	_UpdateScrollBarValue();

	//Update Shape for OnMouse Item

	//Update Shape for Selected Item
}

void i3UIListBox::_UpdateShapeSelected( INT32 idx)
{
	if( idx > -1)
	{
		i3::pack::RECT rt = _GetItemRectByIndex( idx);

		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_SELECTED].left = rt.left;
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_SELECTED].top = i3Math::Max( rt.top, 0.0f);
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_SELECTED].right = rt.right - rt.left;
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_SELECTED].bottom = rt.bottom - rt.top;

		setShapeVisible( I3UI_LISTBOX_SHAPE_ITEM_SELECTED, true);
	}
	else
	{
		setShapeVisible( I3UI_LISTBOX_SHAPE_ITEM_SELECTED, false);
	}
	
	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIListBox::_UpdateShapeOnMouse( INT32 idx)
{
	if( idx > -1)
	{
		i3::pack::RECT rt = _GetItemRectByIndex( idx);

		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE].left = rt.left;
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE].top = rt.top;
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE].right = rt.right - rt.left;
		m_pRects[I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE].bottom = rt.bottom - rt.top;

		setShapeVisible( I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE, true);	
	}
	else
	{
		setShapeVisible( I3UI_LISTBOX_SHAPE_ITEM_ONMOUSE, false);
	}
	
	addState( I3UI_STATE_NEED_UPDATE);
}


//
// m_nCurSel, m_nCurMouseOn 변경에 의해 _UpdateText가 호출되는것 같지는 않다.. (해당 변수가 변경되면, SetText() 재호출이 요구됨..)
// 2014.12.16.수빈.
//

void i3UIListBox::_UpdateText( void)
{
	if( m_pText == nullptr)
		return;
	
	m_pText->SetShowPos( 0, m_TextPosY);	// Scroll
	
	//Text의 위치가 변경되면 Block의 위치도 변경되어야한다. 
	_UpdateShapeSelected( m_nCurSel);
	//_UpdateShapeOnMouse( m_nCurMouseOn);
}

void i3UIListBox::OnBindTemplate(void)
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	if( m_pUIScrollBar == nullptr)
	{
		m_pUIScrollBar = i3UIScrollBar::new_object();
		AddControl( m_pUIScrollBar);
		m_pUIScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pUIScrollBar->addStyle( I3UI_STYLE_SLAVE);
	}

	m_pUIScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_LISTBOX_SUBTEMPLATE_VSCROLL));
}

void i3UIListBox::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);

	if( bEnable)
	{
		addState( I3UI_STATE_NEED_UPDATE);
	}
	else
	{
		if( m_pUIScrollBar != nullptr)	m_pUIScrollBar->EnableCtrl( bEnable);
	}
}

namespace
{
	// 셀렉션이나 마우스오버에 의해 파셜색상이 들어가게 되면, true
	static void UpdatePartialColor(i3UIText* pText, 
		INT32 ListIndex, DWORD ColorIdx, 
		const i3::vector<INT32>& strTextOffsetList)
	{
		if (strTextOffsetList.empty())
			return;

		INT32 beg_idx =  strTextOffsetList[ListIndex];
		INT32 end_idx = strTextOffsetList[ListIndex+1];

//		if (!SrcToRefinedMapper.empty())
//		{
//			beg_idx = SrcToRefinedMapper[ beg_idx ];
//			end_idx = SrcToRefinedMapper[ end_idx ];
//		}

		for (INT32 i = beg_idx ; i < end_idx ; ++i )
			pText->ModifyRefinedPartialTextColor(i, ColorIdx);
	}

}

void i3UIListBox::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonUp( nFlag, point);

	if (m_nCurSel != -1 )
	{
		DWORD clrToRemove = (m_nCurSel != m_nCurMouseOn) ? 0 : m_textclrTableIdx_MouseOver;
		UpdatePartialColor(getUIText(), m_nCurSel, clrToRemove, m_strTextOffsetList);
	}

	m_nCurSel = _GetItemIndexByPos( point);

	if (m_nCurSel != -1)
	{
		UpdatePartialColor(getUIText(), m_nCurSel, m_textclrTableIdx_Selected, 
			m_strTextOffsetList);
	}

	_UpdateShapeSelected( m_nCurSel);
	
	if( m_nCurSel > -1)
		ParentNotify( I3UI_EVT_SELITEM, m_nCurSel, 0);
}

void i3UIListBox::OnMouseMove( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnMouseMove( nFlag, point);

	if (m_nCurMouseOn != -1)
	{
		UpdatePartialColor(getUIText(), m_nCurMouseOn, 0, m_strTextOffsetList);
	}

	m_nCurMouseOn = _GetItemIndexByPos( point);

	if (m_nCurMouseOn != -1)
	{
		UpdatePartialColor(getUIText(), m_nCurMouseOn, m_textclrTableIdx_MouseOver, 
			m_strTextOffsetList);
	}

	_UpdateShapeOnMouse( m_nCurMouseOn);

	m_CursorPos = point;
}

void i3UIListBox::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	i3UIManager* pManager = i3UI::getManager();
	if( pManager != nullptr)
		pManager->resetOnMouseTarget();

	i3GameNode* pParent = getParent();
	while( pParent != nullptr && (!i3::same_of<i3UIFrameWnd*>(pParent) || !((i3UIFrameWnd*)pParent)->isTopFrameWnd() ) )
		pParent = pParent->getParent();
	
	if( pParent != nullptr)
	{
		i3UIFrameWnd* pFrameWnd = (i3UIFrameWnd*)pParent;
		pFrameWnd->SetFocusCtrl( this);
	}
}

void i3UIListBox::OnMouseWheel( UINT32 nFlag, short zDelta, POINT point)
{
	if( m_pUIScrollBar != nullptr)
	{
		if( m_pUIScrollBar->isEnable())
		{
			INT32 offset = zDelta > 0 ? -10 : 10;

			INT32 nCurPos = m_pUIScrollBar->getScrollPos();

			m_pUIScrollBar->setScrollPos( nCurPos + offset);

			I3TRACE("Before wheel: %d\n", m_nCurMouseOn);

			//setScrollPos => ParentNotify => _GetItemIndexByPos 이므로 스크롤 된 후에 제대로 계산됨.
			//GetCursorPos(
			/*i3Framework* pFramework = i3Framework::getCurrentFramework();
			I3ASSERT( pFramework != nullptr);uiLV_SetButtonName

			i3Viewer* pViewer = pFramework->GetViewer();
			I3ASSERT( pViewer != nullptr);

			POINT pt = point;
			ScreenToClient( pViewer->GetWindowHandle(), &pt);*/

			if (m_nCurMouseOn != -1)
			{
				UpdatePartialColor(getUIText(), m_nCurMouseOn, 0, m_strTextOffsetList);
			}

			m_nCurMouseOn = _GetItemIndexByPos( m_CursorPos);

			if (m_nCurMouseOn != -1)
			{
				UpdatePartialColor(getUIText(), m_nCurMouseOn, m_textclrTableIdx_MouseOver, 
					m_strTextOffsetList);
			}

			_UpdateShapeOnMouse( m_nCurMouseOn);
			//_UpdateShapeState();

			I3TRACE("After wheel: %d\n", m_nCurMouseOn);
		}
	}
}

void i3UIListBox::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	m_TextPosY = 0;

	_UpdateText();
	_UpdateScrollBarValue();
}

bool i3UIListBox::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	return i3UICaptionControl::OnKillFocus( pByCtrl);

	
}

//EditBox, Button, ListBox의 부모가 Combo인 경우를 처리하기 위한 코드가 3번이나 중복된다.
//구조가 변경되면 아예 i3UIControl에서 처리하는게 더 좋을수도...(no need to check parent's type)
//I3_GAMENODE_STYLE_VOLATILE, I3UI_STYLE_SLAVE인 경우 중에도 예외가 있을 수 있다고 판단하여
//지금은 일단 이렇게 처리...
//See i3UIButton::OnSelect or i3UIFrameWnd::OnBindTemplate

i3UIControl* i3UIListBox::OnSelect( void)
{
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) && (getStyle() & I3UI_STYLE_SLAVE))
	{
		i3GameNode* pParent = getParent();
		if( pParent != nullptr)
		{
			if( i3::kind_of<i3UIComboBase*>(pParent) ) /* add Here */ 
			{
				return (i3UIControl*)pParent;
			}
		}
	}

	return this;
}

void i3UIListBox::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_SCROLL :
		m_TextPosY = param1;
		_UpdateText();
		break;

	case I3UI_EVT_KILLFOCUS :	ParentNotify( I3UI_EVT_KILLFOCUS, param1, param2);	break;
	}

	i3UICaptionControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIListBox::UpdateRenderObjects()
{
	i3UICaptionControl::UpdateRenderObjects();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UILISTBOX
	{
		UINT8		m_ID[4] = { 'U', 'L', 'B', '2' };	// ULB1 대신 ULB2를 이제부터 사용....ULB1은 호환성을 위해서만...로딩에서 처리.. 2014.12.16.수빈.
		UINT32		m_clrText_MouseOver = 0;
		UINT32		m_clrText_Selected = 0;
		UINT32		pad[30] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIListBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UILISTBOX data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
															
	data.m_clrText_MouseOver = reinterpret_cast<UINT32&>(m_TextColor_MouseOver);
	data.m_clrText_Selected  = reinterpret_cast<UINT32&>(m_TextColor_Selected);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIListBox::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UILISTBOX data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	char strID[ sizeof(data.m_ID) + 1];	
	::memcpy(strID, data.m_ID, sizeof(data.m_ID));
	strID[sizeof(data.m_ID)] = 0;

	if (i3::generic_is_equal( strID, "ULB2") )
	{
		m_TextColor_MouseOver = reinterpret_cast<I3COLOR&>(data.m_clrText_MouseOver);
		m_TextColor_Selected = reinterpret_cast<I3COLOR&>(data.m_clrText_Selected);
	}
	else
	{
		DWORD dwClr = this->getTextColor();

		m_TextColor_MouseOver = reinterpret_cast<I3COLOR&>(dwClr);
		m_TextColor_Selected = reinterpret_cast<I3COLOR&>(dwClr);
	}

	Result += Rc;

	return Result;
}

//Combo 밑에 있는 ListBox 저장이 안됨..
bool i3UIListBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	// wchar -> char
	char buf[MAX_PATH*2];	buf[0] = 0;
	i3String::WideCharToMultiByte( CP_UTF8, 0, m_strDefaultItems.c_str(), m_strDefaultItems.length(), buf, sizeof( buf), nullptr, nullptr);
	pXML->addAttr( "DefaultItems", buf);

	pXML->addAttr( "TextColor_MouseOver", reinterpret_cast<UINT32&>(m_TextColor_MouseOver) );
	pXML->addAttr( "TextColor_Selected", reinterpret_cast<UINT32&>(m_TextColor_Selected) );

	return bResult;
}

bool i3UIListBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	// char -> wchar
	const char* temp = pXML->getAttr( "DefaultItems");
	if( temp != nullptr)
	{
		WCHAR16	wstrRes[I3UI_ITEMLIST_SIZE];
		int res_len = i3String::MultiByteToWideChar( CP_UTF8, 0, temp, i3::generic_string_size( temp), wstrRes, _countof( wstrRes));
		if (res_len > 0)
			m_strDefaultItems.assign(wstrRes, res_len);
	}

	UINT32 clrRead;
	if ( pXML->getAttr("TextColor_MouseOver", &clrRead) )
		m_TextColor_MouseOver = reinterpret_cast<I3COLOR&>(clrRead);
	else
	{
		DWORD dwClr = this->getTextColor();
		m_TextColor_MouseOver = reinterpret_cast<I3COLOR&>(dwClr);
	}
	if ( pXML->getAttr("TextColor_Selected", &clrRead) )
		m_TextColor_Selected = reinterpret_cast<I3COLOR&>(clrRead);
	else
	{
		DWORD dwClr = this->getTextColor();
		m_TextColor_Selected = reinterpret_cast<I3COLOR&>(dwClr);
	}

	return bResult;
}

void i3UIListBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UIListBox*>(pDest));

	i3UIListBox* pListBox = (i3UIListBox*)pDest;

	I3TRACE("i3UIListBox::CopyTo: %d\n", m_strDefaultItems.size() );	//	sizeof( m_szDefaultItems) / sizeof( WCHAR16));

	pListBox->m_strDefaultItems = m_strDefaultItems;
	pListBox->m_TextColor_MouseOver = m_TextColor_MouseOver;
	pListBox->m_TextColor_Selected = m_TextColor_Selected;
}

bool i3UIListBox::GetTextRect( i3::pack::RECT* pRect)
{
	bool bResult = false;

	if( m_pText != nullptr && m_pTemplate != nullptr)
	{
		i3::pack::RECT rt;
		bResult = m_pTemplate->GetTextArea( m_pRects, &rt);

		if( bResult)
		{
			rt.left += m_AccPos.x;
			rt.top	+= m_AccPos.y;
			
			rt.left += m_SpaceX;
			rt.top += m_SpaceY;
			rt.right -= 2*m_SpaceX;
			rt.bottom -= 2*m_SpaceY;

			rt.right = max( rt.right, 0);
			rt.bottom = max( rt.bottom, 0);

			if( m_pUIScrollBar->isEnable())
			{
				i3::pack::RECT ScrollRt;
				m_pUIScrollBar->GetScrollRect( &ScrollRt);

				if( ScrollRt.left < rt.left + rt.right)	// right가 width..
				{
					rt.right -= (rt.left + rt.right - ScrollRt.left);
				}
			}
		}

		*pRect = rt;
	}

	return bResult;
}

//m_szDefault에 들어있는 string을 parsing하여 순서대로 m_ListBox에 추가합니다. 
void i3UIListBox::_InsertDefaultItemsToList( void)
{
	Clear();	//

	const WCHAR16*	pCur = m_strDefaultItems.c_str();
	i3::custom_stack_wstring<I3UI_ITEMLIST_SIZE>::type	ResStr;

//	WCHAR16		pBuf[ I3UI_ITEMLIST_SIZE];
//	INT32 i = 0;

	while( *pCur != 0)
	{
		if( *pCur == ';')
		{
			// pBuf[i] = 0;
			_AddItem( ResStr.c_str() );	//
			ResStr.clear();
			//i = 0;
		}
		else
		{
			//pBuf[i++] = *pCur;
			ResStr.push_back(*pCur);
		}
		
		pCur++;
	}

//	if( i > 0)
	if (!ResStr.empty())
	{
		//pBuf[i] = 0;
		_AddItem( ResStr.c_str());	//
	}

	_SetText();
}

void i3UIListBox::CalculateAutoSizeByItem( REAL32 rLimitSizeX, REAL32 rLimitSizeY)
{
	if( getText() == nullptr)
		return;


	REAL32 MaxWidth = rLimitSizeX;
	REAL32 ScrollBarWidth = 0.f;

	if( m_pUIScrollBar != nullptr)
	{
		ScrollBarWidth = m_fWidthScrollBar;
	}

	if (!this->isTextAutoLineFeed())		// 멀티라인인 경우 가로 확장 처리는 하지 않는다..
	{
		i3::const_wchar_range temp_rng = this->getTextRange();

		i3::stack_wstring strTemp(temp_rng.begin(), temp_rng.end());

		for( size_t i = 0; i < m_strListBox.size(); i++)			// 이 처리는 약간의 병목이 예상되지만...그냥 놔둠(2014.12.16.수빈)
		{
			const WCHAR16* pItem = m_strListBox[i].c_str();
			SetText( pItem);

			REAL32 extand = getUIText()->getExtentWidth() + getTextSpaceX()*2 + ScrollBarWidth;

			if( MaxWidth < extand)
				MaxWidth = extand;
		}

		SetText(strTemp);
		i3UIListBox::_UpdateSelMouseOverTextColor();
	}



	REAL32 rHeight = getHeight();

	if( rHeight < rLimitSizeY)
		rHeight = rLimitSizeY;

	
	REAL32 rTemp = 0.f;
	
	if (!this->isTextAutoLineFeed())
	{
		rTemp = (REAL32)(getUIText()->getDefFontHeight() * (INT32)m_strListBox.size());
	}
	else	// 멀티라인인 경우 라인피드여부에 의해 세로줄수가 증가할수 있다..기존 i3UIText에 m_MaxLine을 사용..
	{
		rTemp = (REAL32)(getUIText()->getDefFontHeight() * getUIText()->getMaxLine());
	}
	
	rTemp += (m_pRects[I3UI_LISTBOX_SHAPE_TOP].bottom);
	rTemp += (m_pRects[I3UI_LISTBOX_SHAPE_BOTTOM].bottom);

	// 더 작은 경우는 작게..
	if( rTemp < rHeight)
		rHeight = rTemp;

	if( getWidth() != MaxWidth ||
		getHeight() != rHeight )
	{
		setSize( MaxWidth, rHeight);
	}
}

//m_ListBox의 아이템들을 '\n'으로 연결하여 
//하나의 문자열로 만든 후, SetText를 호출합니다. 

void i3UIListBox::_SetText( void)
{
	i3::stack_wstring strTemp;
	
	if ( !m_strListBox.empty() )
	{
		
		const size_t num_item_min_1 = m_strListBox.size() - 1;
		size_t i = 0;
		for( ; i < num_item_min_1; ++i )
		{
			m_strTextOffsetList.push_back((INT32)strTemp.size());
			const i3::rc_wstring& strItem = m_strListBox[i];
			strTemp.append( strItem);
			strTemp.append(1, L'\n');
		}
		
		m_strTextOffsetList.push_back((INT32)strTemp.size());
		const i3::rc_wstring& strItem = m_strListBox[i];
		strTemp.append(strItem);
		m_strTextOffsetList.push_back((INT32)strTemp.size());
	}
	else
	{
		m_strTextOffsetList.clear();
	}

	SetText( strTemp );			// i3UIText의 파셜색상구현은 SetText() 호출뒤엔 리셋되버리므로, 재설정이 필요함...
	
	i3UIListBox::_UpdateSelMouseOverTextColor();

}

namespace
{
	void		ModifyStrTextOffsetListByColorSequence(i3UIText* pText, i3::vector<INT32>& outStrTextOffsetList)
	{
		const i3::wstring& refinedText = pText->getRefinedText();
		
		if ( !refinedText.empty() )		
		{
			outStrTextOffsetList.clear();	outStrTextOffsetList.push_back(0);		// 처음은 무조건 0이 들어가야한다..(항상)

			i3::const_wchar_range rng = refinedText;
			i3::const_wchar_range::iterator it_orig_beg = rng.begin();
			i3::const_wchar_range::iterator it = it_orig_beg;
			while ( (it = i3::range::find(rng, L'\n')) != rng.end() )
			{
				outStrTextOffsetList.push_back( it - it_orig_beg );					// \n 위치
				rng.set_begin(++it);
			}

			outStrTextOffsetList.push_back(refinedText.size());						// 마지막은 정제된 문자열의 길이가 들어가야한다..(항상)
		}

	}


}


void	i3UIListBox::_UpdateSelMouseOverTextColor()
{
	i3UIText* pText = getUIText();
	
	if (!pText)
		return;

	if (pText->IsPartialTextColorApplied() == false)	// 리셋된 경우 재설정요구..
	{
//		pText->BuildSrcStringIntoRefinedStringMapper(m_SrcToRefinedMapper);

		ModifyStrTextOffsetListByColorSequence(pText, m_strTextOffsetList);
	
		m_textclrTableIdx_MouseOver	= pText->AddPartialTextColor(m_TextColor_MouseOver);
		m_textclrTableIdx_Selected	= pText->AddPartialTextColor(m_TextColor_Selected);

		if (m_nCurSel != -1)
		{
			UpdatePartialColor(pText, m_nCurSel, m_textclrTableIdx_Selected, 
				m_strTextOffsetList);
		}

		if (m_nCurMouseOn != -1 && m_nCurSel != m_nCurMouseOn )
		{
			UpdatePartialColor(pText, m_nCurMouseOn, m_textclrTableIdx_MouseOver, 
				m_strTextOffsetList);
		}
	}
}


void i3UIListBox::SetCurSel( INT32 nIdx)
{
	if (m_nCurSel == nIdx )
		return;

	i3UIText* pText = getUIText();

	if ( pText )
	{
		if (pText->IsPartialTextColorApplied() == false)
		{
//			pText->BuildSrcStringIntoRefinedStringMapper(m_SrcToRefinedMapper);

			ModifyStrTextOffsetListByColorSequence(pText, m_strTextOffsetList);

			m_textclrTableIdx_MouseOver	= pText->AddPartialTextColor(m_TextColor_MouseOver);
			m_textclrTableIdx_Selected	= pText->AddPartialTextColor(m_TextColor_Selected);
		}
		// 셀렉션 체인지이므로, 기존 셀렉션이 있으면 리셋해준다.
		// 마우스 오버와 같다면 마우스 오버로 돌린다..
		DWORD clrIdx_ToRemove = 0;
		if (m_nCurSel == m_nCurMouseOn)
			clrIdx_ToRemove = m_textclrTableIdx_MouseOver;

		if (m_nCurSel != -1)
			UpdatePartialColor(pText, m_nCurSel, clrIdx_ToRemove, 
				m_strTextOffsetList);
		if (nIdx != -1)
			UpdatePartialColor(pText, nIdx, m_textclrTableIdx_Selected,
				m_strTextOffsetList);
	}

	m_nCurSel = nIdx;
}


void i3UIListBox::Init( void)
{
	i3UICaptionControl::Init();

	_InsertDefaultItemsToList();
	
	//_SetText();
}

void i3UIListBox::OnInitAfterLoad()
{
	i3UICaptionControl::OnInitAfterLoad();

	//_InsertDefaultItemsToList();
}

void	i3UIListBox::GripDraw( HDC hdc )
{
	i3UICaptionControl::GripDraw(hdc);
	
	POINT pt[4];
	if( m_pText == nullptr)
		return;

	i3::pack::RECT trect2;
	REAL32 bottom;
	m_pTemplate->GetTextArea( m_pRects, &trect2);
	INT32 fFontHeight = m_pText->getDefFontHeight();
	bottom = m_AccPos.y + trect2.top + trect2.bottom;

	trect2.left		= m_AccPos.x + trect2.left;	
	trect2.top		= m_AccPos.y + trect2.top;
	trect2.right	= trect2.left+ trect2.right;
	trect2.bottom	= trect2.top+ fFontHeight;

	for( size_t i=0; i< m_strListBox.size(); i++)
	{
		i3::pack::RECT trect = trect2;
		trect.bottom += fFontHeight * i;

		if( trect.bottom > bottom+ fFontHeight )
			break;

		pt[0].x = static_cast<LONG>(trect.left);	pt[0].y = static_cast<LONG>(trect.top);
		pt[1].x = static_cast<LONG>(trect.right);	pt[1].y = static_cast<LONG>(trect.top);
		pt[2].x = static_cast<LONG>(trect.right);	pt[2].y = static_cast<LONG>(trect.bottom);
		pt[3].x = static_cast<LONG>(trect.left);	pt[3].y = static_cast<LONG>(trect.bottom);
		::Polygon( hdc, pt, 4);
	}
}

#if defined( I3_DEBUG)
void i3UIListBox::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UIListBox");
	AddMFCProperty_RcStringW( List, m_strDefaultItems, "Initial Items", "자동으로 ListBox에 추가될 스트링을 ';'로 구분하여 입력하십시오");

	AddMFCProperty_Real32( List, &m_fWidthScrollBar, "Scrollbar Width", "Scrollbar의 Width값");
	AddMFCProperty_Real32( List, &m_fMarginUpScrollBar, "Scrollbar MarginUp", "Scrollbar의 MarginUp값");
	AddMFCProperty_Real32( List, &m_fMarginDownScrollBar, "Scrollbar MarginDown", "Scrollbar의 MarginDown값");
	AddMFCProperty_Real32( List, &m_fMarginRightScrollBar, "Scrollbar MarginRight", "Scrollbar의 MarginRight값");
	
	AddMFCProperty_Color(List, &m_TextColor_MouseOver, "TextColor MouseOver", "마우스오버된 텍스트의 색상");
	AddMFCProperty_Color(List, &m_TextColor_Selected, "TextColor Selected", "셀렉션된 텍스트의 색상");

	AddMFCProperty_EndGroup( List);
}

void i3UIListBox::OnTDKPropertyChanged( const char* szName)
{
	if(i3::generic_is_equal( szName, "Initial Items") )
	{
		_InsertDefaultItemsToList();
	}
	else if( i3::generic_is_equal( szName, "Scrollbar Width") )			// 이건 왜 하나 뿐일까..
	{
		_UpdateScrollBarValue();
	}
	else if (	i3::generic_is_equal(szName, "TextColor MouseOver") )
	{
		i3UIText* pText = getUIText();
		if (pText)
			pText->ModifyPartialTextColor(m_textclrTableIdx_MouseOver, m_TextColor_MouseOver);
	}
	else if ( i3::generic_is_equal(szName, "TextColor Selected") )
	{
		i3UIText* pText = getUIText();
		if (pText)
			pText->ModifyPartialTextColor(m_textclrTableIdx_Selected, m_TextColor_Selected);
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

void	i3UIListBox::SetTextColor_Selected(I3COLOR clr)
{
	m_TextColor_Selected = clr;
	i3UIText* pText = getUIText();
	if (pText)
		pText->ModifyPartialTextColor(m_textclrTableIdx_Selected, m_TextColor_Selected);
}

void	i3UIListBox::SetTextColor_MouseOver(I3COLOR clr)
{
	m_TextColor_MouseOver = clr;
	i3UIText* pText = getUIText();
	if (pText)
		pText->ModifyPartialTextColor(m_textclrTableIdx_MouseOver, m_TextColor_MouseOver);
}


void i3UIListBox::GetDefaultItems( WCHAR16* pBuf, INT32 nBufSize)
{
	i3::safe_string_copy(pBuf, m_strDefaultItems, nBufSize);
}

void	i3UIListBox::SetDefaultItems( const i3::rc_wstring& strItems)
{
	m_strDefaultItems = strItems;

	//if( bAddToBox)
	{
		//_ItemsToStringList();
		_SetText();
	}
}


/////////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UILB_init( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		pListBox->Init();
	}
	
	return 0;
}

extern "C" int i3UILB_Clear( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		pListBox->Clear();
	}
	
	return 0;
}

extern "C" int i3UILB_AddItem( lua_State* L)
{
	char szItem[ 260];

	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);

	if( pListBox != nullptr)
	{
		if( i3Lua::GetStringArg( L, 2, szItem, sizeof( szItem), "i3UILB_AddItem") != nullptr)
		{
			WCHAR16 wItem[MAX_PATH];
			i3String::MultiByteToWideChar( CP_UTF8, 0, szItem, i3::generic_string_size( szItem), wItem, _countof( wItem));
			
			INT32 result = pListBox->AddItem( wItem);

			i3Lua::PushInteger( L, result);
		}
	}
	
	return 1;
}


extern "C" int i3UILB_InsertItem( lua_State* L)
{
	char szItem[ 260];
	WCHAR16 wItem[ 260];

	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);

	if( pListBox != nullptr)
	{
		INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);

		if( i3Lua::GetStringArg( L, 3, szItem, sizeof( szItem), "i3UILB_InsertItem") != nullptr)
		{	
			MultiByteToWideChar( CP_UTF8, 0, szItem, i3::generic_string_size( szItem), wItem, _countof( wItem));

			INT32 result = pListBox->InsertItem( nIdx, wItem);

			i3Lua::PushInteger( L, result);
		}
	}
	
	return 1;
}

extern "C" int i3UILB_DeleteItem( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);

		INT32 result = pListBox->DeleteItem( nIdx);

		i3Lua::PushInteger( L, result);
	}
	
	return 1;
}

extern "C" int i3UILB_RemoveItem( lua_State* L)
{
	char szItem[ 260];
	WCHAR16 wItem[MAX_PATH];

	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);

	if( pListBox != nullptr)
	{
		if( i3Lua::GetStringArg( L, 3, szItem, sizeof( szItem), "i3UILB_InsertItem") != nullptr)
		{	
			MultiByteToWideChar( CP_UTF8, 0, szItem, i3::generic_string_size( szItem), wItem, _countof( wItem));

			INT32 result = pListBox->RemoveItem( wItem);

			i3Lua::PushInteger( L, result);


		}
	}
	
	return 1;
}

extern "C" int i3UILB_GetItem( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);

		WCHAR16 wItem[MAX_PATH] = {0};
		pListBox->GetItem( nIdx, wItem, _countof( wItem));	

		char szBuf[MAX_PATH];
		i3String::WideCharToMultiByte( CP_UTF8, 0, wItem, _countof( wItem), szBuf, sizeof( szBuf),nullptr, nullptr);

		i3Lua::PushString( L, szBuf);
	}
	
	return 1;
}

extern "C" int i3UILB_SetItem( lua_State* L)
{
	char szItem[ 260];
	WCHAR16 wItem[ 260];

	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);

	if( pListBox != nullptr)
	{
		INT32 nIdx = i3Lua::GetIntegerArg( L, 2, -1);

		if( i3Lua::GetStringArg( L, 3, szItem, sizeof( szItem), "i3UILB_SetItem") != nullptr)
		{
			i3String::MultiByteToWideChar( CP_UTF8, 0, szItem, i3::generic_string_size( szItem), wItem, _countof( wItem));

			pListBox->SetItem( nIdx, wItem);
		}
	}
	
	return 0;
}

extern "C" int i3UILB_GetCurSel( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		INT32 nSel = pListBox->GetCurSel();

		i3Lua::PushInteger( L, nSel);
	}
	
	return 1;
}

extern "C" int i3UILB_SetCurSel( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		INT32 nSel = i3Lua::GetIntegerArg( L, 2, -1);

		pListBox->SetCurSel( nSel);
	}
	
	return 0;
}

extern "C" int i3UILB_GetItemCount( lua_State* L)
{
	i3UIListBox* pListBox = (i3UIListBox*)i3Lua::GetUserDataArg( L, 1);
	if( pListBox != nullptr)
	{
		INT32 nCount = pListBox->GetItemCount();

		i3Lua::PushInteger( L, nCount);
	}
	
	return 1;
}

LuaFuncReg i3UIListBoxGlue[] = 
{
	{	"uiLB_init",		i3UILB_init			},
	{	"uiLB_Clear",		i3UILB_Clear		},

	{	"uiLB_AddItem",		i3UILB_AddItem		},
	{	"uiLB_InsertItem",	i3UILB_InsertItem	},
	{	"uiLB_DeleteItem",	i3UILB_DeleteItem	},
	{	"uiLB_RemoveItem",	i3UILB_RemoveItem	},
	{	"uiLB_GetItem",		i3UILB_GetItem		},
	{	"uiLB_SetItem",		i3UILB_SetItem		},

	{	"uiLB_GetCurSel",	i3UILB_GetCurSel	},
	{	"uiLB_SetCurSel",	i3UILB_SetCurSel	},

	{	"uiLB_GetItemCount", i3UILB_GetItemCount	},

	{nullptr, nullptr}
};

void i3UIListBox::RegisterLuaFunction( LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIListBoxGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIListBoxGlue[i].name, i3UIListBoxGlue[i].func);
	}
}

// Test Automation 관련
i3UIControl * i3UIListBox::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	if (m_nCurSel == -1)
		return nullptr;

	if (isActivated() && isVisible())
	{
		i3::rc_wstring str = m_strListBox[m_nCurSel];

		if (bMustBeSame)
		{
			if (_wcsicmp(str.c_str(), pszText) == 0)
				return this;
		}
		else
		{
			if (wcsstr(str.c_str(), pszText) != nullptr)
				return this;
		}
	}

	return nullptr;			// Base를 호출하지 않음.
}

bool i3UIListBox::TA_Select(const wchar_t * pszText, bool bMustBeSame)
{
	for (UINT32 i = 0; i < m_strListBox.size(); i++)
	{
		i3::rc_wstring str = m_strListBox[i];

		if (bMustBeSame)
		{
			if (_wcsicmp(str.c_str(), pszText) == 0)
			{
				SetCurSel(i);
				return true;
			}
		}
		else
		{
			if (wcsstr(str.c_str(), pszText) != nullptr)
			{
				SetCurSel(i);
				return true;
			}
		}
	}

	return false;
}