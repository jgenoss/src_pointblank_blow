#include "i3FrameworkPCH.h"
#include "i3UIScrollBar.h"
#include "i3UIManager.h"
#include "../i3Framework.h"

// ============================================================================
//
// i3UIScrollBar : 버튼
//
// ============================================================================

extern "C"
{
	int uiScroll_getScrollPos(lua_State* L)
	{
		i3UIScrollBar* pCtrl = (i3UIScrollBar*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			i3Lua::PushInteger( L, pCtrl->getScrollPos());
			return 1;
		}
		i3Lua::PushInteger( L, 0);
		return 1;
	}
	
	int uiScroll_setScrollPos(lua_State* L)
	{
		i3UIScrollBar* pCtrl = (i3UIScrollBar*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			INT32 setVal = i3Lua::GetIntegerArg(L, 2);
			pCtrl->setScrollPos(setVal);
		}
		return 0;
	}

	int uiScroll_setScrollRange(lua_State* L)
	{
		i3UIScrollBar* pCtrl = (i3UIScrollBar*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			INT32 minVal = i3Lua::GetIntegerArg(L, 2);	INT32 maxVal = i3Lua::GetIntegerArg(L, 3);
			pCtrl->setScrollRange(minVal, maxVal);
		}
		return 0;
	}

	int	uiScroll_setScrollPage(lua_State* L)
	{
		i3UIScrollBar* pCtrl = (i3UIScrollBar*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			INT32 Val = i3Lua::GetIntegerArg(L, 2);
			pCtrl->setPageSize(Val);
		}
		return 0;
	}
	
	int	uiScroll_setScrollTick(lua_State* L)
	{
		i3UIScrollBar* pCtrl = (i3UIScrollBar*)i3Lua::GetUserDataArg( L, 1);

		if( pCtrl != nullptr)
		{
			INT32 Val = i3Lua::GetIntegerArg(L, 2);
			pCtrl->setTickSize(Val);
		}
		return 0;
	}

}

LuaFuncReg i3UIScrollGlue[] = 
{
	{"uiScroll_getScrollPos",	uiScroll_getScrollPos},
	{"uiScroll_setScrollPos",	uiScroll_setScrollPos},
	{"uiScroll_setScrollRange",	uiScroll_setScrollRange},
	{ "uiScroll_setScrollPage", uiScroll_setScrollPage },
	{ "uiScroll_setScrollTick", uiScroll_setScrollTick },
	{nullptr, nullptr}
};


void	i3UIScrollBar::RegisterLuaFunction(LuaState* pState)		// 11.05.19.수빈 루아함수 몇개가 필요해 급히 만듭니다.
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIScrollGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIScrollGlue[i].name, i3UIScrollGlue[i].func);
	}
}


I3_CLASS_INSTANCE( i3UIScrollBar);

i3UIScrollBar::~i3UIScrollBar()
{
	if( m_pBtnPrev != nullptr)
	{
		RemoveChild( m_pBtnPrev);
		I3_SAFE_RELEASE( m_pBtnPrev);
	}

	if( m_pBtnNext != nullptr)
	{
		RemoveChild( m_pBtnNext);
		I3_SAFE_RELEASE( m_pBtnNext);
	}

	if( m_pBtnTrack != nullptr)
	{
		RemoveChild( m_pBtnTrack);
		I3_SAFE_RELEASE( m_pBtnTrack);
	}
}

void	i3UIScrollBar::_UpdateShapeState(void)
{
	i3UIControl::_UpdateShapeState();

	setShapeVisible( I3UI_SCROLLBAR_SHAPE_BKGND, true);
}

void	i3UIScrollBar::_UpdateScrollBar(bool bForce)
{
	if( m_pBtnPrev == nullptr || m_pBtnNext == nullptr || m_pBtnTrack == nullptr)
	{
		//I3TRACE("scroll bar가 완전히 생성되지 않았습니다.\n");
		return;
	}

	if( m_ValuePage >= m_ValueMax - m_ValueMin)
	{
		addState( I3UI_STATE_DEACTIVATED);
		m_pBtnTrack->EnableCtrl(false);
		if (i3UI::getToolDevMode() == false)
			return;
	}
	else
	{
		removeState( I3UI_STATE_DEACTIVATED);
		m_pBtnTrack->EnableCtrl(true);
	}

	INT32 range = m_ValueMax - m_ValueMin;
	REAL32 trackPos = 0.0f;
	REAL32 fLen = 0.0f;	//양 끝 버튼을 제외한 길이

	if( range > 0)
	{
		if( getScrollType() == I3UI_SCROLLBAR_STYLE_HORZ)
		{
			fLen = m_Size.x - m_pBtnPrev->getWidth() - m_pBtnNext->getWidth();	
			m_PageSizeInPx = ((REAL32)m_ValuePage * fLen) / range;
		}
		else
		{
			fLen = m_Size.y - m_pBtnPrev->getHeight() - m_pBtnNext->getHeight();	
			m_PageSizeInPx = ((REAL32)m_ValuePage * fLen) / range;
		}

		trackPos = ((REAL32)m_CurValue * fLen) / range;
	}
	else
	{
		m_PageSizeInPx = 0.0f;
	}

	REAL32 minSize = m_PageSizeInPx;

	// Shape들의 위치 조정
	if( getScrollType() == I3UI_SCROLLBAR_STYLE_HORZ)
	{
		// 수평 Scroll
		m_pBtnPrev->setPos( 0.0f, 0.0f);
		m_pBtnNext->setPos( m_Size.x - m_pBtnNext->getWidth(), 0.0f);

		if( minSize < getX( m_pBtnNext->getSize()))
		{
			trackPos -= getX( m_pBtnNext->getSize()) - minSize;
			if( trackPos < 0.f)
				trackPos = 0.f;
			minSize = getX( m_pBtnNext->getSize());
		}

		m_pBtnTrack->setPos( m_pBtnPrev->getWidth() + trackPos, 0.0f);
		m_pBtnTrack->setSize( minSize, m_Size.y);
	}
	else
	{
		// 수직 Scroll
		m_pBtnPrev->setPos( 0.0f, 0.0f);
		m_pBtnNext->setPos( 0.0f, m_Size.y - m_pBtnNext->getHeight());

		if( minSize < getY( m_pBtnNext->getSize()))
		{
			trackPos -= getY( m_pBtnNext->getSize()) - minSize;
			if( trackPos < 0.f)
				trackPos = 0.f;
			minSize = getY( m_pBtnNext->getSize());
		}

		m_pBtnTrack->setPos( 0.0f, trackPos + m_pBtnPrev->getHeight());
		m_pBtnTrack->setSize( m_Size.x, minSize);
	}
}

void i3UIScrollBar::setScrollType( I3UI_SCROLLBAR_STYLE type)
{
	m_ScrollType = type;

	_UpdateScrollBar();
}

void	i3UIScrollBar::setScrollRange( INT32 minValue, INT32 maxValue)
{
	m_ValueMin = minValue;
	m_ValueMax = maxValue;

	_UpdateScrollBar();
}

void	i3UIScrollBar::setScrollPos( INT32 pos)
{
	INT32 nPrev = m_CurValue;
	
	m_CurValue = MINMAX( m_ValueMin, pos, m_ValueMax - m_ValuePage);

	if( nPrev != m_CurValue)
	{
		Notify( I3UI_EVT_SCROLL, m_CurValue, 0, true);
		//ParentNotify( I3UI_EVT_SCROLL, m_CurValue, 0);		// 원본코드 복귀..11.05.19.수빈.
	}

	_UpdateScrollBar();
	_UpdateShapeState();
}

void	i3UIScrollBar::setPageSize( INT32 val)
{
	m_ValuePage = MINMAX( 0, val, m_ValueMax - m_ValueMin);

	_UpdateScrollBar();
}

void i3UIScrollBar::setTickSize( INT32 val)
{
	m_ValueTick = val;

	_UpdateScrollBar();
}

void i3UIScrollBar::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	I3ASSERT( m_pTemplate != nullptr);

	if( m_pTemplate == nullptr)
		return;

	m_ScrollType = ((i3UITemplate_ScrollBar*)m_pTemplate)->getScrollBarStyle();

	//Prev Button
	if( m_pBtnPrev == nullptr)
	{
		m_pBtnPrev = i3UIButton::new_object();
		m_pBtnPrev->SetName("Prev");
		AddControl( m_pBtnPrev);
		m_pBtnPrev->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);	//부속 컨트롤은 저장되면 아니된다.
		m_pBtnPrev->addStyle( I3UI_STYLE_SLAVE);
	}

	//Next Button
	if( m_pBtnNext == nullptr)
	{
		m_pBtnNext = i3UIButton::new_object();
		m_pBtnNext->SetName("Next");
		AddControl( m_pBtnNext);
		m_pBtnNext->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pBtnNext->addStyle( I3UI_STYLE_SLAVE);
	}

	//Track Button
	if( m_pBtnTrack == nullptr)
	{
		m_pBtnTrack = i3UIButton::new_object();
		m_pBtnTrack->SetName("Track");
		AddControl( m_pBtnTrack);
		m_pBtnTrack->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pBtnTrack->addStyle( I3UI_STYLE_SLAVE);
	}

	if( m_pTemplate->getSubTemplateCount() > 0)
	{
		m_pBtnPrev->SetTemplate( m_pTemplate->getSubTemplate(I3UI_SCROLLBAR_BTN_PREV));
		m_pBtnNext->SetTemplate( m_pTemplate->getSubTemplate(I3UI_SCROLLBAR_BTN_NEXT));
		m_pBtnTrack->SetTemplate( m_pTemplate->getSubTemplate(I3UI_SCROLLBAR_BTN_TRACK));
	}

	// Update
	_UpdateScrollBar();
}

void i3UIScrollBar::OnDraw( i3UIRenderer * pRenderer)
{
	i3UIControl::OnDraw( pRenderer);
}


void i3UIScrollBar::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	_UpdateScrollBar( true);

	if( m_pBtnPrev != nullptr)	m_pBtnPrev->EnableCtrl( bEnable);
	if( m_pBtnNext != nullptr)	m_pBtnNext->EnableCtrl( bEnable);
	if( m_pBtnTrack != nullptr)
	{
		m_pBtnTrack->EnableCtrl( bEnable && m_pBtnTrack->isActivated() );
	}
}

//Scroll Bar의 빈공간을 입력하는 경우 처리
void i3UIScrollBar::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDown( nFlag, point);

	REAL32 left = m_AccPos.x;	// point는 FrameWindow 기준, getPositionX는 Parent기준. 따라서 getTotalPosX();
	REAL32 top	= m_AccPos.y;
	REAL32 right = left + getWidth();
	REAL32 bottom = top + getHeight();
	
	if( m_ScrollType == I3UI_SCROLLBAR_STYLE_VERT)
	{
		if( point.x > left && point.x < right)
		{
			if( point.y > top + m_pBtnPrev->getHeight() && point.y < bottom - m_pBtnNext->getHeight() )
			{
				REAL32 trackTop = top + m_pBtnTrack->getPos()->y;
				REAL32 trackBottom = trackTop + m_pBtnTrack->getHeight();
				
				if( point.y < trackTop)
				{
					setScrollPos( getScrollPos() - m_ValuePage);
				}
				else if( point.y > trackBottom)
				{
					setScrollPos( getScrollPos() + m_ValuePage);
				}
			}
		}
	}
	else	//HScroll
	{
		if( point.y > top && point.y < bottom)
		{
			if( point.x > left + m_pBtnPrev->getWidth() && point.x < right - m_pBtnNext->getWidth() )
			{
				REAL32 trackLeft = left + m_pBtnTrack->getPos()->x;
				REAL32 trackRight = trackLeft + m_pBtnTrack->getWidth();
				
				if( point.x < trackLeft)
				{
					setScrollPos( getScrollPos() - m_ValuePage);
				}
				else if( point.x > trackRight)
				{
					setScrollPos( getScrollPos() + m_ValuePage);
				}
			}
		}
	}
}

void i3UIScrollBar::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonUp( nFlag, point);

	// 마우스 입력을 Capture 해제한다.
	//i3Framework::ReleaseCapture();
}

void i3UIScrollBar::OnRButtonDown( UINT32 nFlag, POINT point)
{
}

void i3UIScrollBar::OnRButtonUp( UINT32 nFlag, POINT point)
{
}

void i3UIScrollBar::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UIControl::OnEnterMouse( x, y, keys);
}

void i3UIScrollBar::OnLeaveMouse(void)
{
	i3UIControl::OnLeaveMouse();
}

void i3UIScrollBar::OnKeyDown( UINT32 nKey)
{
}

void i3UIScrollBar::OnKeyUp( UINT32 nKey)
{
}

void i3UIScrollBar::OnMove( REAL32 x, REAL32 y)
{
	i3UIControl::OnMove( x, y);

	_UpdateScrollBar();
}

void i3UIScrollBar::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);

	REAL32 size = (m_ScrollType == I3UI_SCROLLBAR_STYLE_HORZ) ? cy : cx;

	if( m_pBtnPrev != nullptr && !m_bDisablePrevNextButton )
		m_pBtnPrev->setSize( size, size);

	if( m_pBtnNext != nullptr && !m_bDisablePrevNextButton )
		m_pBtnNext->setSize( size, size);

	//OnSize 는 setSize에서 호출되는 함수이며 실제로 변수의 값이 업데이트 되기 직전에 호출된다. 
	//따라서 cx != m_Size.x 인 상태이므로 _UpdateScrollBar안에서 getWidth() 사용시 잘못된 값을 리턴하게 된다. 
	//그러므로 _UpdateScrollBar 호출 전에 강제로 변수를 업데이트해주어야 한다.
	m_Size.x = cx;
	m_Size.y = cy;

	_UpdateScrollBar();
}

void i3UIScrollBar::SetAlpha(UINT8 alpha)
{
	I3COLOR* col = 0;

	col = i3UIControl::getColor(); col->a = alpha; i3UIControl::setColor(col);
	col = m_pBtnPrev->getColor(); col->a = alpha; m_pBtnPrev->setColor(col);
	col = m_pBtnNext->getColor(); col->a = alpha; m_pBtnNext->setColor(col);
	col = m_pBtnTrack->getColor(); col->a = alpha; m_pBtnTrack->setColor(col);
}
void i3UIScrollBar::SetAlpha(REAL32 alpha)
{
	i3UIScrollBar::SetAlpha( (UINT8)(alpha * 255.f) );
}

void i3UIScrollBar::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_BTN_PUSHED:
		{
			if( pCtrl == m_pBtnPrev)
				_OnPrevPushed();
			else if( pCtrl == m_pBtnNext)
				_OnNextPushed();
		}
		break;
	case I3UI_EVT_DRAGSTART:
		{
			if( pCtrl == m_pBtnTrack)
				_OnTrackDragStart( param1, param2);
		}
		break;
	case I3UI_EVT_DRAGGING:
		{
			if( pCtrl == m_pBtnTrack)
				_OnTrackDragging( param1, param2);
		}
		break;

	case I3UI_EVT_MOUSEWHEEL:
		{
			if( param1 > 0)
				_OnPrevPushed();
			else if( param1 < 0)
				_OnNextPushed();
		}
		break;

	case I3UI_EVT_KILLFOCUS :
		{
			ParentNotify( I3UI_EVT_KILLFOCUS, param1, param2);
		}
		break;
	default:
		break;
	}

	i3UIControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIScrollBar::_OnPrevPushed( void)
{
	setScrollPos( getScrollPos()-m_ValueTick );
}

void i3UIScrollBar::_OnNextPushed( void)
{
	setScrollPos( getScrollPos()+m_ValueTick );
}

void i3UIScrollBar::_OnTrackDragging( INT32 x, INT32 y)
{
	INT32 nValidDist = 50;	
	
	REAL32 left = m_AccPos.x;
	REAL32 top = m_AccPos.y;
	REAL32 right = left + getWidth();
	REAL32 bottom = top + getHeight();

	//Valid Area
	if( x > left - nValidDist && x < right + nValidDist
		&& y > top - nValidDist && y < bottom + nValidDist)
	{
		//pixel unit != scroll unit
		//ScrollBar의 Pixel unit Height()을 구해야함.
		INT32 range = m_ValueMax - m_ValueMin;
		INT32 nPosOffset = 0;

		if( m_ScrollType == I3UI_SCROLLBAR_STYLE_VERT)
		{
			//bgHeight : range = offset : nPosOffset
			REAL32 bgHeight = getHeight() - m_pBtnPrev->getHeight() - m_pBtnNext->getHeight();
			REAL32 offset = (REAL32)(y - m_nDragStartY);
			
			nPosOffset = (INT32)(range * offset / bgHeight);
		}
		else
		{
			REAL32 bgWidth = getWidth() - m_pBtnPrev->getWidth() - m_pBtnNext->getWidth();
			REAL32 offset = (REAL32)(x - m_nDragStartX);
			
			nPosOffset = (INT32)(range * offset / bgWidth);
		}

		setScrollPos( m_nDragStartPos + nPosOffset);
	}
	else
	{
		setScrollPos( m_nDragStartPos);
	}
}

void i3UIScrollBar::_OnTrackDragStart( INT32 x, INT32 y)
{
	m_nDragStartPos = getScrollPos();
	m_nDragStartX = x;
	m_nDragStartY = y;
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UISCROLLBAR
	{
		INT8		m_ID[4] = { 'U', 'S', 'B', '1' };
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif
UINT32 i3UIScrollBar::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UISCROLLBAR data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIScrollBar::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UISCROLLBAR data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

bool i3UIScrollBar::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	return bResult;
}

bool i3UIScrollBar::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	return bResult;
}

void i3UIScrollBar::GetScrollRect( i3::pack::RECT* pRt)
{
	*pRt = m_pRects[I3UI_SCROLLBAR_SHAPE_BKGND];

	pRt->left	+= m_AccPos.x;
	pRt->top	+= m_AccPos.y;
}