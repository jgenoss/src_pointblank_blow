#include "pch.h"

#if legacy_gui_scene
#include "UIPopup_Clan_ChangeName.h"

#include "UIMainFrame.h"
#include "GlobalVariables.h"
#include "UIUtil.h"

I3_CLASS_INSTANCE( UIPopup_Clan_ChangeName);//, UIPopupClanBase);

extern "C" {
	int UIPopup_Clan_ChangeName_Confirm(LuaState * L)
	{
		UIPopup_Clan_ChangeName * pThis = static_cast<UIPopup_Clan_ChangeName*>(GameUI::GetPopup(UPW_CHANGE_CLAN_NAME));
		if( pThis != nullptr)
			pThis->ChangeName_Confirm();
		return 0;
	}

	int UIPopup_Clan_ChangeName_Cancel(LuaState * L)
	{
		UIPopup_Clan_ChangeName * pThis = static_cast<UIPopup_Clan_ChangeName*>(GameUI::GetPopup(UPW_CHANGE_CLAN_NAME));
		if( pThis != nullptr)
			pThis->ChangeName_Cancel();
		return 0;
	}

	int UIPopup_Clan_ChangeName_Duplicate(LuaState * L)
	{
		UIPopup_Clan_ChangeName * pThis = static_cast<UIPopup_Clan_ChangeName*>(GameUI::GetPopup(UPW_CHANGE_CLAN_NAME));
		if( pThis != nullptr)
		{
			i3Lua::GetStringArg(L,1, pThis->m_szChangeName,MAX_STRING_COUNT);
			pThis->ChangeName_Duplicate();
		}
		return 0;
	}
}

LuaFuncReg UIPBRe_PopUp_Clan_ChangeName_Glue[] = {
	{"UIPopup_Clan_ChangeName_Confirm",		UIPopup_Clan_ChangeName_Confirm		},
	{"UIPopup_Clan_ChangeName_Cancel",		UIPopup_Clan_ChangeName_Cancel		},
	{"UIPopup_Clan_ChangeName_Duplicate",	UIPopup_Clan_ChangeName_Duplicate	},
	{nullptr,						nullptr}
};

UIPopup_Clan_ChangeName::UIPopup_Clan_ChangeName()
{
	CheckFlag = false;
	m_EditBox = nullptr;
	m_AuthWareDBIndex = -1;
	ItemID = -1;
	//m_EditBox->SetEventHandler()
}

UIPopup_Clan_ChangeName::~UIPopup_Clan_ChangeName()
{
}

void UIPopup_Clan_ChangeName::ChangeName_Confirm()
{
	LuaState* L = UILua::CallLuaFunction(GetScene(0), "GetClanNewName");
	const char* new_name = UILua::ReturnLuaString(L, 0);
	i3::safe_string_copy( m_szChangeName, new_name, MAX_STRING_COUNT );
	
	if(CheckFlag == false)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));
		return;
	}
	
	g_pFramework->GetClanContext()->ProcessChangeClanName(m_AuthWareDBIndex, m_szChangeName);
}

void UIPopup_Clan_ChangeName::ChangeName_Cancel()
{
	GetMainFrame()->TogglePopup( UPW_CHANGE_CLAN_NAME);
}

void UIPopup_Clan_ChangeName::ChangeName_Duplicate()
{
	_CheckNickNameOverLap_Popup();

}


bool UIPopup_Clan_ChangeName::_CheckNickNameOverLap_Popup()
{
	if( !UIPopupClanBase::_CheckNickNameOverLap(m_szChangeName))
		return false;

	g_pFramework->GetClanContext()->SendOverlappingClanName(m_szChangeName);

	return true;
}


/*virtual*/ void UIPopup_Clan_ChangeName::ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupClanBase::ProcessGameEvent( evt, Arg, UserData);
	//	이벤트 결과를 받아 처리한다.
	switch( evt )
	{
	case EVENT_CLAN_DUPLICATE_NAME:				//	클랜 이름 중복 체크
		{
			_OnEventDuplicateName( Arg);
			
		}
		break;
	case EVENT_ITEM_AUTH:
		{
			if(EV_SUCCESSED(Arg))
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_CLAN_NAMECHANGE_SUCCESS"));
				GetMainFrame()->TogglePopup(UPW_CHANGE_CLAN_NAME);
			}
			else
			{
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_CLAN_NAMECHANGE_FAIL"));
				GetMainFrame()->TogglePopup(UPW_CHANGE_CLAN_NAME);
			}
			
		}
		break;
	}
}


	// 클랜 중복확인
void UIPopup_Clan_ChangeName::_OnEventDuplicateName( INT32 Arg)
{
	char szTemp[MAX_PATH] = ""; 

	if( Arg == 0)	//	클랜명이 중복되지 않는다
	{	
		_SetDupliButtonEnable( true );

		i3::snprintf( szTemp, sizeof( szTemp), "\n {col:255, 255, 255, 255} %s {/col} \n \n %s", m_szChangeName, GAME_RCSTRING("STBL_IDX_CLAN_CAN_USE"));

		GameUI::MsgBox( MSG_TYPE_GAME_OK, szTemp);
	}
	else			//	클랜명이 중복된다.
	{
		
		_SetDupliButtonEnable( false );

		UIPopupClanBase::_OnPopupCheckOverlapping(szTemp, MAX_PATH, m_szChangeName, true);

		GameUI::MsgBox( MSG_TYPE_GAME_OK, szTemp);
	}

}


/*virtual*/ void UIPopup_Clan_ChangeName::OnCreate( i3GameNode * pParent)
{
	UIPopupClanBase::OnCreate( pParent);
	AddScene( "Scene/Popup/PBRe_PopUp_ClanRename.i3UIs", UIPBRe_PopUp_Clan_ChangeName_Glue);
}


/*virtual*/ void  UIPopup_Clan_ChangeName::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupClanBase::_InitializeAtLoad( pScene);

	m_EditBox	= (i3UIEditBox*) pScene->FindChildByName( "EditBoxNewName");

	m_EditBox->SetLetterLimit( NET_CLAN_NAME_SIZE - 1 );
}


/*virtual*/ bool UIPopup_Clan_ChangeName::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupClanBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	ItemID = *static_cast<INT32 *>(pArg1);
	m_AuthWareDBIndex = CInvenList::i()->FindWareDBIdx_ByItemID( ItemID);
	return true;
}

/*virtual*/ void UIPopup_Clan_ChangeName::OnLoadAllScenes()
{
	char * pszName = ClanGameContext::i()->getMyClanDetailInfo()->m_name;
	_SetCurrentClanName(pszName);
	_SetDupliButtonEnable(false);
}

void UIPopup_Clan_ChangeName::_SetCurrentClanName(const char *szText)
{
	LuaState * L = _CallLuaFunction("SetCurrentClanName");
	i3Lua::PushA2UTF8String(L, szText);						// 서버 협업 후 교체 (2014.07.23.수빈)
	_EndLuaFunction(L, 1);									// 현재 Legacy코드로 사용되지 않으므로 투두 제거..
}

void UIPopup_Clan_ChangeName::_SetDupliButtonEnable(bool Sig)
{
	LuaState * L = _CallLuaFunction("_SetDupliButtonEnable");
	CheckFlag = Sig;
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L, 1);
}

#endif