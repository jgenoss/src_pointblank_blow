#include "pch.h"
#include "UIPopupTwitchLogin.h"

#include "UIMainFrame.h"
#include "UIOption.h"

#include "TwitchTVContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE( UIPopupTwitchLogin );

namespace
{
	UIPopupTwitchLogin* g_this = 0;
	enum { LOGIN, CANCEL, TWITCHBANER };
}

extern "C" 
{
	int login_twich( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg login_twich_glue[] =
{
	{ "Click",				login_twich	},
	{ nullptr,							nullptr}
};

void UIPopupTwitchLogin::clicked(INT32 idx)
{
	switch(idx)
	{
	case LOGIN:
		if(!CheckInput())
		{
			SetWaringTitle(false);
			break;
		}
		else
		{
			if(TwitchTVLoginProcess())
			{
				GetMainFrame()->OpenPopup(UPW_TWITCHOPTION);
				GetMainFrame()->ClosePopup(UPW_TWITCHLOGIN);
			}else
			{
				SetWaringTitle(false);
			}
		}
		break;
	case CANCEL:
		GetMainFrame()->ClosePopup(UPW_TWITCHLOGIN);
		break;
	case TWITCHBANER:
		//ClickBanner();
		break;
	}
}

UIPopupTwitchLogin::UIPopupTwitchLogin()
{
	g_this = this;
	
	m_pIDBox = nullptr;
	m_pPwBox = nullptr;
}

UIPopupTwitchLogin::~UIPopupTwitchLogin()
{
}

/**********************//**********************/
/*virtual*/ void UIPopupTwitchLogin::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Twitch_TV_Login.i3UIs", login_twich_glue);
}

/*virtual*/ void UIPopupTwitchLogin::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pIDBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_Twitch_ID");
	I3ASSERT( m_pIDBox != nullptr);
	m_pIDBox->setAlphaNumericOnly(true);
	m_pIDBox->SetLetterLimit( 64);

	m_pPwBox = (i3UIEditBox*) pScene->FindChildByName( "EditBox_Twitch_PW");
	I3ASSERT( m_pPwBox != nullptr);
	m_pPwBox->setAlphaNumericOnly(true);
	m_pPwBox->SetLetterLimit( 64);

	ClearEditBox();
}

/*virtual*/ void UIPopupTwitchLogin::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);
}

/*virtual*/ void UIPopupTwitchLogin::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();
	
	//GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_CART_OVERLAP_ERROR"), nullptr, MAKE_CALLBACK(cb_close_duplicate_popup), this) ;
}

/*virtual*/ void UIPopupTwitchLogin::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();
}

/*virtual*/ void UIPopupTwitchLogin::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();
}

/*virtual*/ void UIPopupTwitchLogin::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);
}

/*virtual*/ bool UIPopupTwitchLogin::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	return UIPopupBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIPopupTwitchLogin::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIPopupBase::OnKeyInput( pInputMng))
		return true;

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	I3ASSERT( pKeyboard != nullptr);

	if( pKeyboard->GetStrokeState(I3I_KEY_TAB))
	{
		if( m_pIDBox->isFocused())
			g_pFramework->SetFocus( m_pPwBox);
		else if( m_pPwBox->isFocused())
			g_pFramework->SetFocus( m_pIDBox);
		else
			g_pFramework->SetFocus( m_pIDBox);

		return true;
	}

	return false;
}

/*virtual*/ bool UIPopupTwitchLogin::OnKey_Enter( void)
{
	clicked(LOGIN);

	return false;
}
/**********************//**********************/

void UIPopupTwitchLogin::ClickBanner()
{
	SHELLEXECUTEINFO sh;
	memset(&sh, 0L, sizeof(SHELLEXECUTEINFO));

	//트위치 티비 주소 가 만약에 바뀐다면 여길 수정.
	char * url = "www.twitch.tv";

	sh.cbSize = sizeof(SHELLEXECUTEINFO);
	sh.lpVerb = "open";	
	sh.lpFile = url;
	sh.nShow = SW_NORMAL;

	if(!ShellExecuteEx(&sh))
	{
		DWORD dw = GetLastError();

		char errorbuffer[256];

		::FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,NULL,
			dw,0,
			(LPTSTR)errorbuffer,0,NULL);

		I3PRINTLOG(I3LOG_DEFALUT, errorbuffer);
	}
}

void UIPopupTwitchLogin::ClearEditBox()
{
	m_pIDBox->SetText(L"");
	m_pPwBox->SetText(L"");
}

void UIPopupTwitchLogin::SetWaringTitle(bool result)
{
	LuaState * L = 0;

	L = _CallLuaFunction( "Change_WaringTitle");
	if( L != nullptr)
	{
		i3Lua::PushBoolean( L, result);	
		UILua::EndLuaFunction(L , 1);
	}
	
	ClearEditBox();
}

bool UIPopupTwitchLogin::CheckInput()
{
	const i3::wstring& strID = m_pIDBox->getText();
	const i3::wstring& strPW = m_pPwBox->getText();

	if( (strID.length()) > 4 && strPW.length() > 4)
			return true;

	ClearEditBox();
	
	return false;
}

bool UIPopupTwitchLogin::TwitchTVLoginProcess()
{
	//추후 트위치 연동 작업 때 트위치 인증 및 로그인 해당 함수내에서 처리 할 예정
	//현재는 임시로 True로.

	const i3::wstring& strID = m_pIDBox->getText();
	const i3::wstring& strPW = m_pPwBox->getPassword();

	if(TwitchTVContext::i()->TTVLogin(strID,strPW))
	{
		//옵션 팝업 커뮤니티 창에 갱신하는 부분입니다.
		UIPopupBase * option_popup = GetMainFrame()->GetPopup(UPW_OPTION);
		UIOptionBase * current_option = ((UIPopupOption*)option_popup)->GetCurrentOption();
		if( i3::same_of<UIOptionCommunity*>(current_option) )
		{
			((UIOptionCommunity*)current_option)->SetTwitchButton();
		}
		return true;
	}
	else
	{
		return false;
	}
}