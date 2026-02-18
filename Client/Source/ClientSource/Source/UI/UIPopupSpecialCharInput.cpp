#include "pch.h"
#if legacy_gui_scene

#include "UIPopupSpecialCharInput.h"

#include "../SpecialCharInput.h"

I3_CLASS_INSTANCE( UIPopupSpecialCharInput);//, UIPopupBase);

UIPopupSpecialCharInput::UIPopupSpecialCharInput()
{
	m_pSpCharInput = nullptr;
}

UIPopupSpecialCharInput::~UIPopupSpecialCharInput()
{
	if( m_pSpCharInput)
	{
		RemoveChild( m_pSpCharInput);
		I3_SAFE_RELEASE( m_pSpCharInput);
	}
}

/*virtual*/ void UIPopupSpecialCharInput::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	//	특수문자 입력기 GUI
#if !defined(USE_I3EXEC)
	m_pSpCharInput = CSpecialCharInput::new_object();
	m_pSpCharInput->Create(g_pGUIRoot);
	I3_GAMENODE_ADDCHILD(this, m_pSpCharInput);
#endif
}

void UIPopupSpecialCharInput::OnWheel( i3UIControl * pControl, INT32 zDelta)
{
	I3ASSERT( m_pSpCharInput != nullptr);

	// 현재 최우선 활성화 GuiWin에게 전달
#if !defined(USE_I3EXEC)
	if( m_pSpCharInput->IsActive())
	{
		m_pSpCharInput->OnWheelUpdate( zDelta);
	}
#endif
}

bool UIPopupSpecialCharInput::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
#if !defined(USE_I3EXEC)
	if( event == I3_EVT_INPUT)
	{
		I3ASSERT( m_pSpCharInput != nullptr);
		if( m_pSpCharInput->OnEvent( event, pObj, param2, code))
			return true;
	}
#endif
	return UIPopupBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIPopupSpecialCharInput::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	m_pSpCharInput->OnOpen();
#endif
	return true;
}

/*virtual*/ bool UIPopupSpecialCharInput::OnExitStart( void)
{
	if( UIPopupBase::OnExitStart() == false)
		return false;
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	m_pSpCharInput->OnClose();
#endif
	return true;
}

/*virtual*/ void UIPopupSpecialCharInput::SetInputDisable( bool bDisable)
{
	UIPopupBase::SetInputDisable( bDisable);
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	m_pSpCharInput->SetInputDisable( bDisable);
#endif
}

/*virtual*/ bool UIPopupSpecialCharInput::OnLinkControl( const i3::vector_set<STRCINFO*>* pList, i3GameNode * pControl)
{
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	return m_pSpCharInput->OnLinkControl( pList, pControl);
#else
	return true;
#endif
}

/*virtual*/ void UIPopupSpecialCharInput::OnInitControl( void)
{
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	m_pSpCharInput->OnInitControl();
#endif
}

/*virtual*/ void UIPopupSpecialCharInput::OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify)
{
#if !defined(USE_I3EXEC)
	I3ASSERT( m_pSpCharInput != nullptr);
	m_pSpCharInput->OnGuiNotify( pNotify);
#endif
}

/*virtual*/ bool UIPopupSpecialCharInput::OnKey_Enter()
{
	#if !defined(USE_I3EXEC)
	m_pSpCharInput->OnOk();
	#endif
	return false;		
//	return true;	// 이게 true가 되면 페이즈쪽으로 OnEvent가 전달되지 않습니다.(11.10.26.수빈)
	
}

/*virtual*/ bool UIPopupSpecialCharInput::OnKey_Escape()
{
	#if !defined(USE_I3EXEC)
	m_pSpCharInput->OnCancel();
	#endif
	return false;
//	return true;	// 이게 true가 되면 페이즈쪽으로 OnEvent가 전달되지 않습니다.(11.10.26.수빈)
	
}

#endif