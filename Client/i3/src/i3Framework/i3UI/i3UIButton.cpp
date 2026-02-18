#include "i3FrameworkPCH.h"
#include "i3UIButton.h"
#include "i3UIManager.h"
#include "i3UITemplate_Button.h"
#include "../i3Framework.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

// ============================================================================
//
// i3UIButton : 버튼
//
// ============================================================================

I3_CLASS_INSTANCE( i3UIButton);

i3UIButton::i3UIButton() : m_CurShape(I3UI_BUTTON_SHAPE_NORMAL)
{
	for( int i = 0; i < MAX_I3UIBUTTON_COLOR; ++i )
	{
		m_ButtonTextColor[i].SetRGBA( 0xffffffff );
	}
}

i3UIButton::~i3UIButton()
{
}

void i3UIButton::_UpdateShapeState(void)
{
	i3UICaptionControl::_UpdateShapeState();

	int nBtnTextColorType = I3UIBUTTON_CR_IDLE;
	
	if(!GetUIControl())
	{
		if (isPushFlag())
		{
			m_CurShape = I3UI_BUTTON_SHAPE_SELECTEDNORMAL;
			nBtnTextColorType = I3UIBUTTON_CR_SELECTEDNORMAL;
		}
		else
		{
			m_CurShape = I3UI_BUTTON_SHAPE_NORMAL;
			nBtnTextColorType = I3UIBUTTON_CR_IDLE;
		}
		
		if( isActivated())
		{
			if ( m_bSkipUnselect == false)
			{
				if( isState( I3UI_STATE_ONMOUSE))
				{
					if( isSelected())
					{
						m_CurShape = I3UI_BUTTON_SHAPE_PUSHED;
						nBtnTextColorType = I3UIBUTTON_CR_SELECTED;
					}
					else
					{
						m_CurShape = I3UI_BUTTON_SHAPE_ONMOUSED;
						nBtnTextColorType = I3UIBUTTON_CR_ONMOUSE;
					}
				}
			}
			else // if ( m_bSkipUnselect == true )			// 셀렉션 유지기능이 선택되고 셀렉션 되었으면 노멀상태는 쓰지 않는다..
			{
				if ( isSelected())							// 셀렉션에 대해 마우스오버를 켜지 않음..
				{
					m_CurShape = I3UI_BUTTON_SHAPE_PUSHED;		// 일반 버튼에 대해서도 탭기능 모사 기능을 넣었습니다. (11.07.08.수빈)
					nBtnTextColorType = I3UIBUTTON_CR_SELECTED;
				}
				else
				{
					if ( isState( I3UI_STATE_ONMOUSE ) )
					{
						m_CurShape = I3UI_BUTTON_SHAPE_ONMOUSED;
						nBtnTextColorType = I3UIBUTTON_CR_ONMOUSE;
					}
				}				
			}
		}
		else if ( m_bSkipUnselect == true )			// 셀렉션 유지기능이 선택되고 셀렉션 되었으면 노멀상태는 쓰지 않는다..
		{
			if ( isSelected())							// 셀렉션에 대해 마우스오버를 켜지 않음..
			{
				m_CurShape = I3UI_BUTTON_SHAPE_PUSHED;		// 일반 버튼에 대해서도 탭기능 모사 기능을 넣었습니다. (11.07.08.수빈)
				nBtnTextColorType = I3UIBUTTON_CR_SELECTED;
			}
			else if(!IsSelectedNormalState())
			{
				m_CurShape = I3UI_BUTTON_SHAPE_DISABLED;
				nBtnTextColorType = I3UIBUTTON_CR_DISABLE;
			}

		}
		else if(!IsSelectedNormalState())
		{
			m_CurShape = I3UI_BUTTON_SHAPE_DISABLED;
			nBtnTextColorType = I3UIBUTTON_CR_DISABLE;
		}
	}

	for( INT32 i = 0; i < I3UI_BUTTON_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, ( i == (INT32)m_CurShape) && (isValidShape( i)));	//C4389
	}

	i3UICaptionControl::SetTextColor( &m_ButtonTextColor[ nBtnTextColorType ] );
}

void i3UIButton::OnUpdate( REAL32 rDeltaSeconds)
{
	//Parent가 ScrollBar인 경우에 버튼이 눌려져 있으면 
	//ScrollBar에 일정 시간간격으로 Notify를 보낸다.
	if( isState( I3UI_STATE_SELECTED) && isState( I3UI_STATE_ONMOUSE))
	{
		i3GameNode* pParent = getParent();
		if( i3::same_of<i3UIScrollBar*>( pParent))
		{
			m_PassedTime += rDeltaSeconds;
			
			if( m_PassedTime > 0.5f)
			{
				ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);
				m_PassedTime -= 0.1f;
			}
		}
	}

	i3UICaptionControl::OnUpdate( rDeltaSeconds);
}

void i3UIButton::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if(isActivated() == false)
		return;

	i3UICaptionControl::OnLButtonDown( nFlag, point);

	if ( m_bSkipUnselect )		// 셀렉션 고정된 경우..중복눌림이라고 변수에 기록..
	{
		m_bSkipUnselect_PushedCurrSel = isState(I3UI_STATE_SELECTED);
	}

	addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);

	_UpdateShapeState();

	//ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);
	Notify( I3UI_EVT_BTN_PUSHED, 0, 0);
}

void i3UIButton::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if(isActivated() == false)
		return;

	bool bLBDrag = isState( I3UI_STATE_LBTN_DRAG);

	i3UICaptionControl::OnLButtonUp( nFlag, point);
	m_PassedTime = 0.0f;

	addState( I3UI_STATE_NEED_UPDATE);

	if (m_bSkipUnselect )
	{
		REAL32 sx = m_Size.x; REAL32 sy = m_Size.y;

		if( point.x <= m_AccPos.x || point.x >= m_AccPos.x + sx || point.y <= m_AccPos.y || point.y >= m_AccPos.y + sy)
		{
			if ( bLBDrag == true && !m_bSkipUnselect_PushedCurrSel)		// 이전상태에서 LBUTTON눌림이 있었는지를 확인하고 처리합니다.(11.10.10.수빈)
				removeState(I3UI_STATE_SELECTED);			// 마우스오버에서 벗어난 뗀 동작은 셀렉션 제거합니다.(11.07.06.수빈)
		}
		else
		{
			//  [3/5/2012 dosun.lee]
			// skip unselect 버튼 의 경우 포커스를 잃으면 계속 셀렉트 상태로 되버리는 현상이 있어 추가 함
			m_bSkipUnselect_PushedCurrSel = true;
		}
	}
	else
		removeState( I3UI_STATE_SELECTED);
}


void i3UIButton::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnRButtonDown( nFlag, point);

	addState( I3UI_STATE_NEED_UPDATE);
}

bool i3UIButton::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	bool bRet = i3UICaptionControl::OnKillFocus( pByCtrl);

	//Button의 State는 Focus와는 무관. 오직 Mouse에만 영향을 받는다.
	//removeState( I3UI_STATE_SELECTED | I3UI_STATE_ONMOUSE );

	if(isActivated() == false)
		return bRet;

	if( m_bSkipUnselect )	
	{
		//  [3/5/2012 dosun.lee]
		// skip unselect 버튼 의 경우 포커스를 잃으면 계속 셀렉트 상태로 되버리는 현상이 있어 추가 함
		if( false == m_bSkipUnselect_PushedCurrSel )
		{
			removeState( I3UI_STATE_SELECTED );
		}
	}

	removeState( I3UI_STATE_LBTN_DRAG);
	addState( I3UI_STATE_NEED_UPDATE);

	return bRet;
}

bool i3UIButton::OnSetFocus()
{
	i3UICaptionControl::OnSetFocus();

	addState( I3UI_STATE_NEED_UPDATE);

	return true;
}

void i3UIButton::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButton::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	m_PassedTime = 0.0f;

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButton::OnKeyDown( UINT32 nKey)
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

void i3UIButton::OnKeyUp( UINT32 nKey)
{
	i3UICaptionControl::OnKeyUp( nKey);
}

void i3UIButton::OnMove( REAL32 x, REAL32 y)
{
	i3UICaptionControl::OnMove( x, y);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButton::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

i3UIControl* i3UIButton::OnSelect( void)
{
	// Possible Cases: 
	// Child of i3UIListView_Header, ScrollBar, Spin, Combo
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) && (getStyle() & I3UI_STYLE_SLAVE))
	{
		i3GameNode* pParent = getParent();
		if( pParent != nullptr)
		{
			if(    i3::same_of<i3UIListView_Header* >(pParent) 
				|| i3::same_of<i3UIScrollBar* >(pParent) 
				|| i3::same_of<i3UISpin* >(pParent) 
				|| i3::kind_of<i3UIComboBase* >(pParent) )	/* add Here */ //)
			{

				return (i3UIControl*)pParent;
			}
		}
	}

	return this;
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UIBUTTON
	{
		INT8		m_ID[4] = { 'U', 'B', 'T', '2' };
		bool		m_SelectedNormalState = false;			// 4바이트패킹 구조체에서 여기에 1바이트를 썼기 때문에 3바이트가 패딩공간으로 남은 상태임..
													// 패딩공간은 재활용이 가능합니다. 
		UINT32		m_textColor_Normal = 0;
		UINT32		m_textColor_Selected = 0;
		UINT32		m_textColor_MouseOver = 0;
		UINT32		m_textColor_Disable = 0;
		UINT32		m_textColor_SelectedNormal = 0;
		UINT32		pad[2] = { 0 };
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



UINT32 i3UIButton::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UIBUTTON data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_SelectedNormalState = m_bSelectedNormalState;

	data.m_textColor_Normal		= GetButtonTextColor(I3UIBUTTON_CR_IDLE);
	data.m_textColor_MouseOver  = GetButtonTextColor(I3UIBUTTON_CR_ONMOUSE);
	data.m_textColor_Selected	= GetButtonTextColor(I3UIBUTTON_CR_SELECTED);
	data.m_textColor_Disable	= GetButtonTextColor(I3UIBUTTON_CR_DISABLE);
	data.m_textColor_SelectedNormal = GetButtonTextColor(I3UIBUTTON_CR_SELECTEDNORMAL);

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIButton::OnLoad( i3ResourceFile * pResFile)
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

	m_bSelectedNormalState = data.m_SelectedNormalState;

	UINT32 myID = FileIDToUINT(data.m_ID);

	if (myID == UBT2)
	{
		SetButtonTextColor(	I3UIBUTTON_CR_IDLE, data.m_textColor_Normal);
		SetButtonTextColor( I3UIBUTTON_CR_ONMOUSE, data.m_textColor_MouseOver);
		SetButtonTextColor( I3UIBUTTON_CR_SELECTED, data.m_textColor_Selected);
		SetButtonTextColor( I3UIBUTTON_CR_DISABLE, data.m_textColor_Disable);
		SetButtonTextColor( I3UIBUTTON_CR_SELECTEDNORMAL, data.m_textColor_SelectedNormal);
	}
	else
	{
		UINT32 uColor = getTextColor();

		for( int i = 0; i < MAX_I3UIBUTTON_COLOR; ++i )
		{
			SetButtonTextColor( (I3UIBUTTON_COLOR_TYPE) i, uColor );
		}
	}
	return Result;
}

bool i3UIButton::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->addAttr("SelectedNormal", m_bSelectedNormalState);

	I3COLOR clr = m_ButtonTextColor[ I3UIBUTTON_CR_IDLE];
	pXML->addAttr("ButtonText_Normal_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Normal_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Normal_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Normal_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTON_CR_ONMOUSE];
	pXML->addAttr("ButtonText_MouseOver_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_MouseOver_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTON_CR_SELECTED];
	pXML->addAttr("ButtonText_Selected_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Selected_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Selected_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Selected_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTON_CR_DISABLE];
	pXML->addAttr("ButtonText_Disable_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_Disable_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_Disable_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_Disable_ColorA", (UINT32) i3Color::GetA(&clr));

	clr = m_ButtonTextColor[ I3UIBUTTON_CR_SELECTEDNORMAL];
	pXML->addAttr("ButtonText_SelectedNormal_ColorR", (UINT32) i3Color::GetR(&clr));
	pXML->addAttr("ButtonText_SelectedNormal_ColorG", (UINT32) i3Color::GetG(&clr));
	pXML->addAttr("ButtonText_SelectedNormal_ColorB", (UINT32) i3Color::GetB(&clr));
	pXML->addAttr("ButtonText_SelectedNormal_ColorA", (UINT32) i3Color::GetA(&clr));

	return bResult;
}

bool i3UIButton::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->getAttr("SelectedNormal", (INT32*) &m_bSelectedNormalState);
	
	UINT32 r = 255, g = 255, b = 255, a = 255;

	if (pXML->getAttr("ButtonText_Normal_ColorR", &r) )
	{
		pXML->getAttr("ButtonText_Normal_ColorG", &g);
		pXML->getAttr("ButtonText_Normal_ColorB", &b);
		pXML->getAttr("ButtonText_Normal_ColorA", &a);
		I3COLOR color;
		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTON_CR_IDLE ] = color;

		pXML->getAttr("ButtonText_MouseOver_ColorR", &r);
		pXML->getAttr("ButtonText_MouseOver_ColorG", &g);
		pXML->getAttr("ButtonText_MouseOver_ColorB", &b);
		pXML->getAttr("ButtonText_MouseOver_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTON_CR_ONMOUSE ] = color;


		pXML->getAttr("ButtonText_Selected_ColorR", &r);
		pXML->getAttr("ButtonText_Selected_ColorG", &g);
		pXML->getAttr("ButtonText_Selected_ColorB", &b);
		pXML->getAttr("ButtonText_Selected_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTON_CR_SELECTED ] = color;

		pXML->getAttr("ButtonText_Disable_ColorR", &r);
		pXML->getAttr("ButtonText_Disable_ColorG", &g);
		pXML->getAttr("ButtonText_Disable_ColorB", &b);
		pXML->getAttr("ButtonText_Disable_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTON_CR_DISABLE ] = color;

		pXML->getAttr("ButtonText_SelectedNormal_ColorR", &r);
		pXML->getAttr("ButtonText_SelectedNormal_ColorG", &g);
		pXML->getAttr("ButtonText_SelectedNormal_ColorB", &b);
		pXML->getAttr("ButtonText_SelectedNormal_ColorA", &a);

		i3Color::Set( &color, (UINT8) r, (UINT8) g, (UINT8) b, (UINT8) a);
		m_ButtonTextColor[I3UIBUTTON_CR_SELECTEDNORMAL ] = color;

	}
	else
	{

		UINT32 uColor = getTextColor();

		for( int i = 0; i < MAX_I3UIBUTTON_COLOR; ++i )
		{
			SetButtonTextColor( (I3UIBUTTON_COLOR_TYPE) i, uColor );
		}
	}

	return bResult;
}

void i3UIButton::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of< i3UIButton* >(pDest));

	i3UIButton* pButton = (i3UIButton*)pDest;

	pButton->m_CurShape = m_CurShape;

	for (INT32 i = 0 ; i < MAX_I3UIBUTTON_COLOR; ++i)
	{
		pButton->m_ButtonTextColor[i] = this->m_ButtonTextColor[i];
	}

}

#if defined( I3_DEBUG)
void i3UIButton::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIButton");
	AddMFCProperty_Bool( List, &m_bSelectedNormalState, "SelectedNormal");

	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTON_CR_IDLE], "Text Color NORMAL");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTON_CR_SELECTED], "Text Color SELECTED");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTON_CR_ONMOUSE], "Text Color MOUSEOVER");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTON_CR_DISABLE], "Text Color DISABLE");
	AddMFCProperty_Color(List, &m_ButtonTextColor[I3UIBUTTON_CR_SELECTEDNORMAL], "Text Color SELECTEDNORMAL");

	AddMFCProperty_EndGroup( List);
}

void	i3UIButton::OnTDKPropertyChanged( const char* szName)
{
	if ( i3::generic_is_equal(szName, "Text Color NORMAL") ||
		i3::generic_is_equal(szName, "Text Color MOUSEOVER") ||
		i3::generic_is_equal(szName, "Text Color SELECTED") ||
		i3::generic_is_equal(szName, "Text Color DISABLE") ||
		i3::generic_is_equal(szName, "Text Color SELECTEDNORMAL") )
	{
		setModifyState();		// 이거 갖고 안되면 UpdateShapeState()를 호출해야됨..
	}	
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged(szName);
	}
}


#endif

void i3UIButton::Init( void)
{
	addState( I3UI_STATE_NEED_UPDATE);
}

INT32 i3UIButton::OnCommand(LuaState * pState)
{
	char szTemp[ 260];

	i3UIControl::OnCommand( pState);

	if( i3Lua::GetStringArg(pState, 2, szTemp, sizeof( szTemp)) != nullptr)
	{
		I3TRACE("DO %s\n", szTemp);
		return 1;
	}

	return 0;
}

extern "C"
{
	int uiButton_skipUnselect(lua_State* L)
	{
		
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			bool bEnabled = i3Lua::GetBooleanArg( L, 2, 0);
			pCtrl->SkipUnselect( bEnabled );
		}

		return 0;
	}

	int uiButton_isSkipUnselect(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			i3Lua::PushBoolean(L, pCtrl->IsSkipUnselect() );
			return 1;
		}
		return 0;
	}

	int uiButton_SetButttonTextColor(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);
		
		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			UINT8 r = (UINT8)i3Lua::GetNumberArg( L, 3, 0);
			UINT8 g = (UINT8)i3Lua::GetNumberArg( L, 4, 0);
			UINT8 b = (UINT8)i3Lua::GetNumberArg( L, 5, 0);
			UINT8 a = (UINT8)i3Lua::GetNumberArg( L, 6, 0);

			COLOR cr;
			i3Color::Set( &cr, r, g, b, a );

			pCtrl->SetButtonTextColor( (i3UIButton::I3UIBUTTON_COLOR_TYPE) nType, cr );
		}

		return 0;
	}

	int uiButton_SetButttonTextColor4Byte(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			INT32 rgba  = i3Lua::GetIntegerArg( L, 3 );
			
			pCtrl->SetButtonTextColor( (i3UIButton::I3UIBUTTON_COLOR_TYPE) nType, rgba );
		}

		return 0;
	}

	int uiButton_GetButttonTextColor(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			INT32 nType = i3Lua::GetIntegerArg( L, 2 );
			UINT32 uColor = pCtrl->GetButtonTextColor( (i3UIButton::I3UIBUTTON_COLOR_TYPE) nType );

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

	int uiButton_removeOnMouse(lua_State* L)
	{
		i3UIButton* pCtrl = (i3UIButton*)i3Lua::GetUserDataArg( L, 1);

		if( nullptr != pCtrl )
		{
			pCtrl->removeState(I3UI_STATE_ONMOUSE);
		}

		return 0;
	}
}


UINT32 i3UIButton::GetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type )
{
	COLOR cr = m_ButtonTextColor[ I3UIBUTTON_CR_IDLE ];

	switch( _type )
	{
	case	I3UIBUTTON_CR_ONMOUSE:		
	case	I3UIBUTTON_CR_SELECTED:
	case	I3UIBUTTON_CR_DISABLE:
	case	I3UIBUTTON_CR_SELECTEDNORMAL:
			cr = m_ButtonTextColor[ _type ];
			break;
	}

	return i3Color::ConvertRGBA32(  &cr );
}

void i3UIButton::SetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type, UINT32 uColor )
{
	COLOR cr;
	cr.SetRGBA(uColor );

	SetButtonTextColor( _type, cr );
}

void i3UIButton::SetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type, const COLOR& cr )
{
	switch( _type )
	{
	case	I3UIBUTTON_CR_ONMOUSE:			
	case	I3UIBUTTON_CR_SELECTED:
	case	I3UIBUTTON_CR_IDLE:
	case	I3UIBUTTON_CR_DISABLE:
	case	I3UIBUTTON_CR_SELECTEDNORMAL:
			m_ButtonTextColor[ _type ] = cr;
			break;
	}

	addState( I3UI_STATE_NEED_UPDATE);
}

void	i3UIButton::SetButtonTextColorAll(const COLOR& cr)
{
	for (INT32 i = 0 ; i < MAX_I3UIBUTTON_COLOR ; ++i)
	{
		m_ButtonTextColor[i] = cr;
	}
	addState( I3UI_STATE_NEED_UPDATE);
}

void	i3UIButton::SetButtonTextColorAll(UINT32 uColor)
{
	COLOR cr;
	cr.SetRGBA(uColor );
	i3UIButton::SetButtonTextColorAll(cr);
}


void	i3UIButton::SetTextColor( I3UIBUTTON_COLOR_TYPE _type, UINT32 uColor)
{
	i3UIButton::SetButtonTextColor(_type, uColor);
}

void	i3UIButton::SetTextColor( I3UIBUTTON_COLOR_TYPE _type, COLOR* pCol)
{
	i3UIButton::SetButtonTextColor(_type, *pCol );
}


/*
void i3UIButton::SetTextColor( UINT32 uColor )
{
	i3UICaptionControl::SetTextColor( uColor );

	for( int i = 0; i < MAX_I3UIBUTTON_COLOR; ++i )
	{
		SetButtonTextColor( (I3UIBUTTON_COLOR_TYPE)i, uColor );
	}
}

void i3UIButton::SetTextColor( COLOR* pCol )
{
	i3UICaptionControl::SetTextColor( pCol );

	for( int i = 0; i < MAX_I3UIBUTTON_COLOR; ++i )
	{
		if( pCol )
			SetButtonTextColor( (I3UIBUTTON_COLOR_TYPE)i, *pCol );
	}
}
*/
void i3UIButton::UnSelectedNormalState()
{
	SetPushFlag(!isPushFlag());
	_UpdateShapeState();
	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButton::set_disable_text_color(const COLOR & color)
{
	m_ButtonTextColor[I3UIBUTTON_CR_DISABLE] = color;
}

void i3UIButton::disable_btn()
{
	setInputDisable(true);
	addState( I3UI_STATE_DEACTIVATED|I3UI_STATE_NEED_UPDATE);
	i3UICaptionControl::SetTextColor( &m_ButtonTextColor[I3UIBUTTON_CR_DISABLE] );
}

void i3UIButton::enable_btn()
{
	setInputDisable(false);
	removeState(I3UI_STATE_DEACTIVATED);
	setModifyState();
}

void i3UIButton::RemoveOnMouse()
{
	removeState(I3UI_STATE_ONMOUSE);
}


LuaFuncReg i3UIButtonGlue[] = 
{
	{	"uiButton_skipUnselect",				uiButton_skipUnselect	},
	{	"uiButton_isSkipUnselect",				uiButton_isSkipUnselect	},
	{	"uiButton_SetButttonTextColor",			uiButton_SetButttonTextColor	},
	{	"uiButton_GetButttonTextColor",			uiButton_GetButttonTextColor	},
	{	"uiButton_SetButttonTextColor4Byte",	uiButton_SetButttonTextColor4Byte	},
	{	"uiButton_removeOnMouse",	uiButton_removeOnMouse	},
	{nullptr, nullptr}
};


void i3UIButton::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIButtonGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIButtonGlue[i].name, i3UIButtonGlue[i].func);
	}
}

bool i3UIButton::GetTextRect(i3::pack::RECT* pRect)
{
	bool res = i3UICaptionControl::GetTextRect(pRect);

	if (m_pTemplate && m_pText)
	{
		if(m_CurShape == I3UIBUTTON_CR_SELECTED)
		{
			pRect->left += m_pTemplate->getShape(I3UIBUTTON_CR_SELECTED)->getTextOffsetX();
			pRect->top +=  m_pTemplate->getShape(I3UIBUTTON_CR_SELECTED)->getTextOffsetY();
		}

		pRect->right  -= m_pTemplate->getShape(I3UIBUTTON_CR_IDLE)->getTextOffsetX();
		pRect->bottom -= m_pTemplate->getShape(I3UIBUTTON_CR_IDLE)->getTextOffsetY();
	}
	return res;

}