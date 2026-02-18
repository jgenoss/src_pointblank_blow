#include "pch.h"
#include "UIPopupNickNameSetting.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UIUtil.h"
#include "UICharaInfoMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/generic_string_copy.h"
#include "i3Base/string/ext/generic_string_cat.h"

#if legacy_gui_function
	#include "../GuiNotifyReceiver.h"
#endif

#include "GameLoginContext.h"


I3_CLASS_INSTANCE( UIPopupNickNameSetting);//, UIPopupBase);

extern "C" {

	int UIPhaseCreateChara_CheckButton( LuaState * L)
	{
		UIPopupNickNameSetting * pThis = static_cast<UIPopupNickNameSetting*>(GameUI::GetPopup(UPW_CREATENICKNAME));
		if( pThis != nullptr)
			pThis->CheckButton();
		return 0;
	}
}
LuaFuncReg PopupNickNameSetting_Glue[] = 
{
	{"CheckButton",					UIPhaseCreateChara_CheckButton},
	{nullptr, nullptr}
};



UIPopupNickNameSetting::UIPopupNickNameSetting()
{
	m_pEd = nullptr;
	m_bCreate = false;
	m_bChangeNameChecked = false;
}

UIPopupNickNameSetting::~UIPopupNickNameSetting()
{
}

/*virtual*/ void UIPopupNickNameSetting::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pEd = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
	I3ASSERT( m_pEd != nullptr);

	if (m_pEd) m_pEd->setUseCopyNPaste(false);
}

/*virtual*/ void UIPopupNickNameSetting::_ClearAtUnload(i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
	m_pEd = nullptr;
}

/*virtual*/ void UIPopupNickNameSetting::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_IbnicknameWin.i3UIs", PopupNickNameSetting_Glue);
}

/*virtual*/ bool UIPopupNickNameSetting::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	/*if(i3UI::getGlobalIme()->isFocused())
		return true;

	return UIBase::OnKeyInput(pInputMng);*/
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

	if( pKeyboard->GetStrokeState( I3I_KEY_ENTER))
	{
		if( OnKey_Enter())
			return true;
	}

	return false;
}

/*virtual*/ bool UIPopupNickNameSetting::OnKey_Enter( void)
{
	if( m_bChangeNameChecked )
		return OnOKButton();
	else
	{
		CheckButton();
	}

	return false;
}

/*virtual*/ void UIPopupNickNameSetting::OnEntranceEnd( void)
{
	UIPopupBase::OnEntranceEnd();

	g_pFramework->SetFocus(m_pEd);

	m_pEd->SetLetterLimit( NICKNAME_CHAR_COUNT);

	GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);
}


/*virtual*/ bool UIPopupNickNameSetting::OnExitStart( void)
{
	if( m_bCreate)
		return UIPopupBase::OnExitStart();

	return false;
}

/*virtual*/ bool UIPopupNickNameSetting::OnOKButton()
{
	if( CreateNickName() == false)
		return false;
	m_bCreate = true;
	GetMainFrame()->ClosePopup(UPW_CREATENICKNAME);
	//CharaInfoMgr::instance()->list_up();
	return true;
	// Server 확인을 받으면 창을 닫는다.
}
/*virtual*/ void UIPopupNickNameSetting::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent( event, arg, UserData);

	if( event == EVENT_USE_ITEM_CHECK_NICK)
		ChangeTextMsg(arg);
}

/*vurtial*/ bool UIPopupNickNameSetting::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName() , "i3UIButtonComposed3") )
	{
		return PlaySound(UISND_ITEM_CLICK_OK);
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}

void UIPopupNickNameSetting::ChangeTextMsg( INT32 arg)
{
	i3::stack_wstring wstrBuf;

	if( EV_SUCCESSED(arg))
	{
		m_bChangeNameChecked = true;
		wstrBuf = L"{col:52,188,5,255}";
		i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STR_POPUP_CAN_USE_NICK_NAME") );
		i3::generic_string_cat( wstrBuf, L"{/col}");

		if( m_pEd->getText() != nullptr)
		{
		//	i3mem::FillZero(m_szChangeName, MAX_STRING_COUNT);
		//	INT32 len = i3::generic_string_size( m_pEd->getText());
		//	i3String::CopyWCHARtoASCII( m_szChangeName, sizeof(m_szChangeName), m_pEd->getText(), len);

			m_wstrChangeName = m_pEd->getText();
		}
	}
	else
	{
		m_bChangeNameChecked = false;

		wstrBuf = L"{col:255,0,0,255}";

		if( EV_IS_ERROR(arg,EVENT_ERROR_EVENT_DUPLICATE_NICK))
		{	
			i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STBL_IDX_EP_LOBBY_SAME_NICK") );
		}
		else if (arg == EVENT_ERROR_FAIL)
		{
			i3::generic_string_cat(wstrBuf, GAME_RCSTRING("STR_POPUP_MESSAGE_CANNOT_NICKNAME"));
		}
		else
		{
			i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STBL_IDX_EP_LOBBY_FAIL_NICK_E") );
		}

		i3::generic_string_cat( wstrBuf, L"{/col}");

		PlaySound( UISND_ERROR);

	}

	//SetMessageBox
	LuaState * L = _CallLuaFunction( "SetMessageBox");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrBuf.c_str(), wstrBuf.size());
		_EndLuaFunction(L, 1);
	}
}

bool UIPopupNickNameSetting::CreateNickName( void)
{
	if( m_pEd == nullptr)	return false;

	i3::wstring wstrBuf;
	static const bool bError = false;
	bool NickCheck = false;

	wstrBuf = m_pEd->getText();

	if(  i3::generic_is_iequal( m_wstrChangeName, wstrBuf) )
	{
		NickCheck = true;
	}

	if((m_bChangeNameChecked == false)|| (NickCheck == false))
	{
		/*중복검사를 해주십시오.*/
		// char szBuf[MAX_STRING_COUNT] = {0,};
		// i3::safe_string_copy( szBuf, GAME_RCSTRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK"), MAX_STRING_COUNT );

		LuaState * L = _CallLuaFunction( "SetMessageBox");
		if(L != nullptr)
		{
			const i3::rc_wstring& wstrGAMESTRING = GAME_RCSTRING("STR_POPUP_MESSAGE_OVERLAPPING_CHECK");
			i3Lua::PushStringUTF16To8( L, wstrGAMESTRING );
			_EndLuaFunction(L, 1);
		}
		return false;
	}

	
	if( bError == false)
	{
		GameLoginContext::i()->setUserNick(wstrBuf);

		GameEventSender::i()->SetEvent(EVENT_CREATE_NICK_NAME);
		m_bCreate = true;
	}
	else
		m_bCreate = false;

	SetEditFocus( false);
	return true;
	// 창을 닫는다.
	//GetMainFrame()->TogglePopup( UPW_CREATENICKNAME);
}

void UIPopupNickNameSetting::SetEditFocus(bool bFlag)
{
	// 팝업창이 종료되면 포커스를 Edit Box로 이동시킨다.
	if (!GetScene(0))		// 급조 코드..
		return;

	if(m_pEd == nullptr) 
		return;

	if(bFlag)
		g_pFramework->SetFocus(m_pEd);
	else
		g_pFramework->SetFocus( nullptr);

}

void UIPopupNickNameSetting::CheckButton()
{
//	char strBuf[MAX_STRING_COUNT] = {0,};
//	i3::string strBuf;
	INT32 len = i3::generic_string_size( m_pEd->getText());	

	i3::stack_wstring wstrBuf;

	if( len > 0)
	{
//		i3String::CopyWCHARtoASCII( strBuf, sizeof(strBuf), m_pEd->getText(), len);
		
		wstrBuf = m_pEd->getText();

		// NickName
		if (false == IsValidNicknameLetterSize(wstrBuf))
		{
			// ! praptor - 최소 글자수 포함되도록 내용 수정해야합니다.
			i3::generic_string_copy( wstrBuf, L"{col:255,0,0,255}");
			i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_LENGTH") ) ;
			i3::generic_string_cat( wstrBuf, L"{/col}");
		}
		else if ( ! g_pFramework->CheckValidNickname(wstrBuf) || g_pFramework->IsFilterOK(wstrBuf) == false)
		{
			i3::generic_string_copy( wstrBuf, L"{col:255,0,0,255}");
			i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STBL_IDX_EP_LOBBY_WRONG_NICK") );
			i3::generic_string_cat( wstrBuf, L"{/col}");
		}
		else
		{
			GameEventSender::i()->SetEvent(EVENT_USE_ITEM_CHECK_NICK, wstrBuf.c_str());
			return;
		}
	}
	else
	{
		i3::generic_string_copy( wstrBuf, L"{col:255,0,0,255}");
		i3::generic_string_cat( wstrBuf, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY") );
		i3::generic_string_cat( wstrBuf, L"{/col}");
	}

	LuaState * L = _CallLuaFunction( "SetMessageBox");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrBuf);
		_EndLuaFunction(L, 1);
	}
	
}