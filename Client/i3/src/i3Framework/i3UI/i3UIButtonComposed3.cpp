#include "i3FrameworkPCH.h"
#include "i3UIButtonComposed3.h"
#include "i3UIManager.h"
#include "i3UITemplate_ButtonComposed3.h"
#include "../i3Framework.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

// ============================================================================
//
// i3UIButton : 버튼
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIButtonComposed3);

i3UIButtonComposed3::i3UIButtonComposed3() : m_CurShape(I3UI_BUTTON_COMP3_SHAPE_NORMAL_LEFT)
{
	for( int i = 0; i < MAX_I3UIBUTTONCOMPOSED3_COLOR; ++i )
	{
		m_ButtonTextColor[i].SetRGBA( 0xffffffff );
	}
}


void i3UIButtonComposed3::_UpdateShapeState(void)
{
	m_CurShape = I3UI_BUTTON_COMP3_SHAPE_NORMAL_LEFT;

	int nBtnTextColorType = I3UIBUTTONCOMPOSED3_CR_IDLE;

	if( isActivated())
	{
		if( isSelected())
		{
			m_CurShape = I3UI_BUTTON_COMP3_SHAPE_PUSHED_LEFT;
			nBtnTextColorType = I3UIBUTTONCOMPOSED3_CR_SELECTED;
		}
		else if( isState( I3UI_STATE_ONMOUSE))	
		{
			m_CurShape = I3UI_BUTTON_COMP3_SHAPE_ONMOUSED_LEFT;
			nBtnTextColorType = I3UIBUTTONCOMPOSED3_CR_ONMOUSE;
		}
	}
	else
	{
		m_CurShape = I3UI_BUTTON_COMP3_SHAPE_DISABLED_LEFT;
		nBtnTextColorType = I3UIBUTTONCOMPOSED3_CR_DISABLE;
	}

	for( INT32 i = 0; i < I3UI_BUTTON_COMP3_SHAPE_COUNT; i++)
	{
		if( i == (INT32)m_CurShape)
		{
			setShapeVisible( i, isValidShape( i));	//C4389
			i++;
			setShapeVisible( i, isValidShape( i));	//C4389
			i++;
			setShapeVisible( i, isValidShape( i));	//C4389
		}
		else
		{
			setShapeVisible( i, false);
		}
	}
	addState( I3UI_STATE_NEED_UPDATE);

	i3UICaptionControl::_UpdateShapeState();
	i3UICaptionControl::SetTextColor( &m_ButtonTextColor[ nBtnTextColorType ] );
}

void i3UIButtonComposed3::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonDown( nFlag, point);
	
	if ( m_bSkipUnselect )		// 셀렉션 고정된 경우..중복눌림이라고 변수에 기록..
	{
		m_bSkipUnselect_PushedCurrSel = isState(I3UI_STATE_SELECTED);
	}

	addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);

//	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);
	Notify( I3UI_EVT_BTN_PUSHED, 0, 0);
}

void i3UIButtonComposed3::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnRButtonDown( nFlag, point);

	addState( I3UI_STATE_NEED_UPDATE);
}

bool i3UIButtonComposed3::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	bool bRet = i3UICaptionControl::OnKillFocus( pByCtrl);

	if (!m_bSkipUnselect)
		removeState( I3UI_STATE_SELECTED);			// 현재 이 버튼이 탭버튼처럼 사용되어지는 경우가 너무 많아서..
	else											// 포커스를 잃어도 셀렉션을 잃으면 곤란하기 때문에 옵션으로 구분짓습니다.....(11.07.05.수빈)
	{
		if (isState(I3UI_STATE_LBTN_DRAG) && !m_bSkipUnselect_PushedCurrSel)		// LButton상태에서 셀렉션설정없이 포커스 해제되면 셀렉션 끕니다.
			removeState( I3UI_STATE_SELECTED);
	}

	removeState( I3UI_STATE_LBTN_DRAG);
	addState( I3UI_STATE_NEED_UPDATE);

	return bRet;
}

bool i3UIButtonComposed3::OnSetFocus()
{
	i3UICaptionControl::OnSetFocus();

	addState( I3UI_STATE_NEED_UPDATE);

	return true;
}

void i3UIButtonComposed3::OnLButtonUp( UINT32 nFlag, POINT point)
{
	//i3UIShape * pShape = getShape( m_CurShape);
	bool bLBDrag = isState( I3UI_STATE_LBTN_DRAG);
	REAL32 sx = m_Size.x; REAL32 sy = m_Size.y;

	// 위 계산식은 다소 틀린것 같습니다..(이미지 사이즈와 콘트롤 사이즈는 충분이 다를수 있으며 버그 확인했습니다... 11.07.06.수빈)
	// OnLButtonDown은 기존 콘트롤 구현으로 넘기는데..이건 그냥 m_Size를 쓰므로, 여기서도 그냥 그것을 쓰겠습니다.  
	if( point.x <= m_AccPos.x || point.x >= m_AccPos.x + sx || point.y <= m_AccPos.y || point.y >= m_AccPos.y + sy)
	{
		OnLeaveMouse();	// Leave Mouse 처리
		if ( bLBDrag == true && !m_bSkipUnselect_PushedCurrSel) // 이전상태에서 LBUTTON눌림이 있었는지를 확인하고 처리합니다.(11.10.10.수빈)
			removeState(I3UI_STATE_SELECTED);			// 마우스오버에서 벗어난 뗀 동작은 셀렉션 제거합니다.(11.07.06.수빈)
	}
	
	if (!m_bSkipUnselect)
		removeState( I3UI_STATE_SELECTED);

	addState( I3UI_STATE_NEED_UPDATE);

	if(isActivated())
		i3UICaptionControl::OnLButtonUp( nFlag, point);	// 기본함수호출을 뒤에 두었습니다....루아함수 콜백이 이 안에 있어...하드코딩 처리가 모두 반영되고 
														// 루아 호출이 나중에 오는 것이 더 직관적이며, 루아에서 상태를 재설정 가능하게 처리하는게 더 좋을듯 합니다..
}														// (11.07.05.수빈)

void i3UIButtonComposed3::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	if(isActivated())
		i3UICaptionControl::OnLButtonDblClk( nFlag, point);	// 버튼에서 비활성화상태에서 이벤트가 들어와 예외 처리합니다.
}

void i3UIButtonComposed3::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	if( isState( I3UI_STATE_LBTN_DRAG))
	{
		addState( I3UI_STATE_SELECTED);
	}

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonComposed3::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonComposed3::OnKeyDown( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			{
			}
			break;
	}

	i3UICaptionControl::OnKeyDown( nKey);
}

void i3UIButtonComposed3::OnKeyUp( UINT32 nKey)
{
	i3UICaptionControl::OnKeyUp( nKey);
}

void i3UIButtonComposed3::OnMove( REAL32 x, REAL32 y)
{
	i3UICaptionControl::OnMove( x, y);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonComposed3::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UIBUTTON
	{
		INT8		m_ID[4] = { 'U', 'B', 'T', '2' };
		UINT32		m_textColor_Normal = 0;
		UINT32		m_textColor_MouseOver = 0;
		UINT32		m_textColor_Selected = 0;
		UINT32		m_textColor_Disable = 0;

		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

namespace
{
	UINT32& FileIDToUINT(INT8 (&strID)[4] ) {  return reinterpret_cast<UINT32&>(strID); }
	const UINT32& FileIDToUINT(const char (&strID)[5] ) {  return reinterpret_cast<const UINT32&>(strID); }

	const UINT32& UBT1 = FileIDToUINT("UBT1");
	const UINT32& UBT2 = FileIDToUINT("UBT2");
	const UINT32& UBT3 = FileIDToUINT("UBT3");
}


UINT32 i3UIButtonComposed3::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UIBUTTON data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
		
	data.m_textColor_Normal		= GetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_IDLE);
	data.m_textColor_MouseOver  = GetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_ONMOUSE );
	data.m_textColor_Selected	= GetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_SELECTED );
	data.m_textColor_Disable	= GetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_DISABLE);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIButtonComposed3::OnLoad( i3ResourceFile * pResFile)
{
	i3_prof_func();

	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UIBUTTON data;

	Result = i3UICaptionControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	UINT32 myID = FileIDToUINT(data.m_ID);

	if (myID == UBT2)
	{
		SetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_IDLE, data.m_textColor_Normal);
		SetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_ONMOUSE, data.m_textColor_MouseOver);
		SetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_SELECTED, data.m_textColor_Selected);
		SetButtonTextColor(I3UIBUTTONCOMPOSED3_CR_DISABLE, data.m_textColor_Disable);
	}
	else
	{
		UINT32 uColor = getTextColor();

		for( int i = 0; i < MAX_I3UIBUTTONCOMPOSED3_COLOR; ++i )
		{
			SetButtonTextColor( (I3UIBUTTONCOMPOSED3_COLOR_TYPE) i, uColor );
		}
	}

	return Result;
}

bool i3UIButtonComposed3::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	I3COLOR clr = m_ButtonTextColor[ I3UIBUTTONCOMPOSED3_CR_IDLE];
	pXML->addAttr("ButtonText_Normal_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Normal_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Normal_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Normal_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTONCOMPOSED3_CR_ONMOUSE];
	pXML->addAttr("ButtonText_MouseOver_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTONCOMPOSED3_CR_SELECTED];
	pXML->addAttr("ButtonText_Selected_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Selected_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Selected_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Selected_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTONCOMPOSED3_CR_DISABLE];
	pXML->addAttr("ButtonText_Disable_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Disable_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Disable_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Disable_ColorA", (UINT32) i3Color::GetA(&clr));


	return bResult;
}

bool i3UIButtonComposed3::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	UINT32 r = 255, g = 255, b = 255, a = 255;
	
	if (pXML->getAttr("ButtonText_Normal_ColorR", &r) )
	{
		pXML->getAttr("ButtonText_Normal_ColorG", &g);
		pXML->getAttr("ButtonText_Normal_ColorB", &b);
		pXML->getAttr("ButtonText_Normal_ColorA", &a);
		I3COLOR color;
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_IDLE ] = color;

		pXML->getAttr("ButtonText_MouseOver_ColorR", &r);
		pXML->getAttr("ButtonText_MouseOver_ColorG", &g);
		pXML->getAttr("ButtonText_MouseOver_ColorB", &b);
		pXML->getAttr("ButtonText_MouseOver_ColorA", &a);
		
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_ONMOUSE ] = color;


		pXML->getAttr("ButtonText_Selected_ColorR", &r);
		pXML->getAttr("ButtonText_Selected_ColorG", &g);
		pXML->getAttr("ButtonText_Selected_ColorB", &b);
		pXML->getAttr("ButtonText_Selected_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_SELECTED ] = color;

		pXML->getAttr("ButtonText_Disable_ColorR", &r);
		pXML->getAttr("ButtonText_Disable_ColorG", &g);
		pXML->getAttr("ButtonText_Disable_ColorB", &b);
		pXML->getAttr("ButtonText_Disable_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_DISABLE ] = color;

	}
	else
	{
		I3COLOR clr = m_TextColor;
		for (INT32 i = 0; i < MAX_I3UIBUTTONCOMPOSED3_COLOR ; ++i)
			m_ButtonTextColor[i] = clr;
	}

	return bResult;
}

void i3UIButtonComposed3::OnInitAfterLoad()
{
	i3UICaptionControl::OnInitAfterLoad();
}

void i3UIButtonComposed3::Init( void)
{
	addState( I3UI_STATE_NEED_UPDATE);
}

INT32 i3UIButtonComposed3::OnCommand(LuaState * pState)
{
	char szCom[ 260];

	i3UIControl::OnCommand( pState);

	if( i3Lua::GetStringArg(pState, 2, szCom, sizeof( szCom), nullptr) != nullptr)
	{
		I3TRACE("DO %s\n", szCom);
		return 1;
	}

	return 0;
}

void i3UIButtonComposed3::set_disable_text_color(const COLOR & color)
{
	m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_DISABLE] = color;
}

void i3UIButtonComposed3::disable_btn() //버튼 상태가 비활성화 상태로 변한다.
{
	setInputDisable(true);
	addState( I3UI_STATE_DEACTIVATED|I3UI_STATE_NEED_UPDATE);
	i3UICaptionControl::SetTextColor( &m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_DISABLE] );
}
void i3UIButtonComposed3::enable_btn() //버튼 상태가 활성화 상태로 변한다.
{
	setInputDisable(false);
	removeState(I3UI_STATE_DEACTIVATED);
	setModifyState();
}

void i3UIButtonComposed3::RemoveOnMouse()
{
	removeState(I3UI_STATE_ONMOUSE);
}


/////////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
	int uiButtonComposed3_skipUnselect(lua_State* L)
	{
		i3UIButtonComposed3* pCtrl = (i3UIButtonComposed3*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bEnabled = i3Lua::GetBooleanArg( L, 2, 0);
			pCtrl->SkipUnselect( bEnabled );
		}
		return 0;
	}

	int uiButtonComposed3_isSkipUnselect(lua_State* L)
	{
		i3UIButtonComposed3* pCtrl = (i3UIButtonComposed3*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, pCtrl->IsSkipUnselect() );
			return 1;
		}
		return 1;
	}

	int uiButtonComposed3_SetButttonTextColor(lua_State* L)
	{
		i3UIButtonComposed3* pCtrl = (i3UIButtonComposed3*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			UINT8 r = (UINT8)i3Lua::GetNumberArg( L, 3, 0);
			UINT8 g = (UINT8)i3Lua::GetNumberArg( L, 4, 0);
			UINT8 b = (UINT8)i3Lua::GetNumberArg( L, 5, 0);
			UINT8 a = (UINT8)i3Lua::GetNumberArg( L, 6, 0);

			COLOR cr;
			i3Color::Set( &cr, r, g, b, a );

			pCtrl->SetButtonTextColor( (i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_COLOR_TYPE) nType, cr );
		}

		return 0;
	}

	int uiButtonComposed3_SetButttonTextColor4Byte(lua_State* L)
	{
		i3UIButtonComposed3* pCtrl = (i3UIButtonComposed3*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			INT32 rgba  = i3Lua::GetIntegerArg( L, 3 );

			pCtrl->SetButtonTextColor( (i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_COLOR_TYPE) nType, rgba );
		}

		return 0;
	}

	int uiButtonComposed3_GetButttonTextColor(lua_State* L)
	{
		i3UIButtonComposed3* pCtrl = (i3UIButtonComposed3*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			UINT32 uColor = pCtrl->GetButtonTextColor( (i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_COLOR_TYPE) nType );

			COLOR cr;
			i3Color::ConvertRGBA32( uColor, &cr);

			i3Lua::PushInteger(L, (INT32)(cr.r));
			i3Lua::PushInteger(L, (INT32)(cr.g));
			i3Lua::PushInteger(L, (INT32)(cr.b));
			i3Lua::PushInteger(L, (INT32)(cr.a));

			return 4;
		}

		return 0;
	}

	int uiButtonComposed3_removeOnMouse(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			pCtrl->removeState(I3UI_STATE_ONMOUSE);
		}

		return 0;
	}
}

LuaFuncReg i3UIButtonComposed3Glue[] = 
{
	{	"uiButtonComposed3_skipUnselect",				uiButtonComposed3_skipUnselect	},
	{	"uiButtonComposed3_isSkipUnselect",				uiButtonComposed3_isSkipUnselect	},
	{	"uiButtonComposed3_SetButttonTextColor",		uiButtonComposed3_SetButttonTextColor	},
	{	"uiButtonComposed3_GetButttonTextColor",		uiButtonComposed3_GetButttonTextColor	},
	{	"uiButtonComposed3_SetButttonTextColor4Byte",	uiButtonComposed3_SetButttonTextColor4Byte	},
	{	"uiButtonComposed3_removeOnMouse",	uiButtonComposed3_removeOnMouse	},
	{nullptr, nullptr}
};


void i3UIButtonComposed3::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIButtonComposed3Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIButtonComposed3Glue[i].name, i3UIButtonComposed3Glue[i].func);
	}
}

UINT32 i3UIButtonComposed3::GetButtonTextColor( I3UIBUTTONCOMPOSED3_COLOR_TYPE _type )
{
	COLOR cr = m_ButtonTextColor[ I3UIBUTTONCOMPOSED3_CR_IDLE ];

	switch( _type )
	{
	case	I3UIBUTTONCOMPOSED3_CR_ONMOUSE:		
	case	I3UIBUTTONCOMPOSED3_CR_SELECTED:
	case	I3UIBUTTONCOMPOSED3_CR_DISABLE:
			cr = m_ButtonTextColor[ _type ];
			break;
	}

	return i3Color::ConvertRGBA32(  &cr );
}

void i3UIButtonComposed3::SetButtonTextColor( I3UIBUTTONCOMPOSED3_COLOR_TYPE _type, UINT32 uColor )
{
	COLOR cr;
	cr.SetRGBA(uColor );

	SetButtonTextColor( _type, cr );
}

void i3UIButtonComposed3::SetButtonTextColor( I3UIBUTTONCOMPOSED3_COLOR_TYPE _type, const COLOR& cr )
{
	switch( _type )
	{
	case	I3UIBUTTONCOMPOSED3_CR_ONMOUSE:			
	case	I3UIBUTTONCOMPOSED3_CR_SELECTED:
	case	I3UIBUTTONCOMPOSED3_CR_IDLE:
	case	I3UIBUTTONCOMPOSED3_CR_DISABLE:
			m_ButtonTextColor[ _type ] = cr;
			break;
	}

	addState( I3UI_STATE_NEED_UPDATE);
}

void	i3UIButtonComposed3::SetButtonTextColorAll(const COLOR& cr)
{
	for (INT32 i = 0 ; i < MAX_I3UIBUTTONCOMPOSED3_COLOR ; ++i)
	{
		m_ButtonTextColor[i] = cr;
	}
	addState( I3UI_STATE_NEED_UPDATE);
}

void	i3UIButtonComposed3::SetButtonTextColorAll(UINT32 uColor)
{
	COLOR cr;
	cr.SetRGBA(uColor );
	i3UIButtonComposed3::SetButtonTextColorAll(cr);
}


void	i3UIButtonComposed3::SetTextColor( I3UIBUTTONCOMPOSED3_COLOR_TYPE _type, UINT32 uColor)
{
	i3UIButtonComposed3::SetButtonTextColor(_type, uColor);
}

void	i3UIButtonComposed3::SetTextColor( I3UIBUTTONCOMPOSED3_COLOR_TYPE _type, COLOR* pCol)
{
	i3UIButtonComposed3::SetButtonTextColor(_type, *pCol );
}

/*
void i3UIButtonComposed3::SetTextColor( UINT32 uColor )
{
	i3UICaptionControl::SetTextColor( uColor );

	for( int i = 0; i < MAX_I3UIBUTTONCOMPOSED3_COLOR; ++i )
	{
		SetButtonTextColor( (I3UIBUTTONCOMPOSED3_COLOR_TYPE)i, uColor );
	}
}

void i3UIButtonComposed3::SetTextColor( COLOR* pCol )
{
	i3UICaptionControl::SetTextColor( pCol );

	for( int i = 0; i < MAX_I3UIBUTTONCOMPOSED3_COLOR; ++i )
	{
		if( pCol )
			SetButtonTextColor( (I3UIBUTTONCOMPOSED3_COLOR_TYPE)i, *pCol );
	}
}
*/

bool i3UIButtonComposed3::GetTextRect( i3::pack::RECT* pRect)
{
	bool res = i3UICaptionControl::GetTextRect(pRect);

	if (m_pTemplate && m_pText)
	{
		if(m_CurShape == I3UI_BUTTON_COMP3_SHAPE_PUSHED_LEFT)
		{
			pRect->left	+= m_pTemplate->getShape(I3UI_BUTTON_COMP3_SHAPE_PUSHED_LEFT)->getTextOffsetX();
			pRect->top	+= m_pTemplate->getShape(I3UI_BUTTON_COMP3_SHAPE_PUSHED_LEFT)->getTextOffsetY();
		}

		pRect->right  -= m_pTemplate->getShape(I3UI_BUTTON_COMP3_SHAPE_NORMAL_LEFT)->getTextOffsetX();
		pRect->bottom -= m_pTemplate->getShape(I3UI_BUTTON_COMP3_SHAPE_NORMAL_LEFT)->getTextOffsetY();
	}
	return res;
}

#if defined( I3_DEBUG)
void i3UIButtonComposed3::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIButtonComposed3");
	
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_IDLE], "Text Color NORMAL");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_ONMOUSE], "Text Color MOUSEOVER");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_SELECTED], "Text Color SELECTED");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTONCOMPOSED3_CR_DISABLE], "Text Color DISABLE");

	AddMFCProperty_EndGroup( List);
}

void	i3UIButtonComposed3::OnTDKPropertyChanged( const char* szName)
{
	if ( i3::generic_is_equal(szName, "Text Color NORMAL") ||
		 i3::generic_is_equal(szName, "Text Color MOUSEOVER") ||
		 i3::generic_is_equal(szName, "Text Color SELECTED") ||
		 i3::generic_is_equal(szName, "Text Color DISABLE") )
	{
		setModifyState();	// 이거 갖고 안되면 UpdateShapeState()를 호출해야됨..
	}	
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged(szName);
	}
}

#endif

void	i3UIButtonComposed3::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method )
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of< i3UIButtonComposed3* >(pDest));
	i3UIButtonComposed3* pButton = (i3UIButtonComposed3*)pDest;

	pButton->m_CurShape = m_CurShape;
	
	for (INT32 i = 0 ; i < MAX_I3UIBUTTONCOMPOSED3_COLOR; ++i)
	{
		pButton->m_ButtonTextColor[i] = this->m_ButtonTextColor[i];
	}

}
