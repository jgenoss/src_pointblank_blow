#include "pch.h"
#include "UISubTabClanMgt_Info.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UITabClanManagement.h"
#include "ClanDef.h"
#include "../CommunityContext.h"
#include "ClanContext.h"

#include "UIPopupReceiveMail.h"
#include "UI_V11/UIPopupSendMail_V11.h"

#include "UIUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


I3_CLASS_INSTANCE( UISubTabClanMgt_Info);//, UISubTabClan_Mgt);


static UISubTabClanMgt_Info * GetThis()
{
	UIPhaseClanMember* pPhase = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		UITabClanManagement* pClan = static_cast<UITabClanManagement*>(pPhase->GetTab( UTC_MGT));
		if( pClan != nullptr)
		{
			return static_cast<UISubTabClanMgt_Info*>(pClan->GetTab(USTC_MGT_INFO));
		}
	}

	return nullptr;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UISubTabClanMgt_Info_SendMessage(LuaState * L)
	{
		UISubTabClanMgt_Info * pThis = GetThis();
		if( pThis != nullptr)
			pThis->OnClanSendMessage();
		return 0;
	}

	int UISubTabClanMgt_Info_ModifyInfo(LuaState * L)
	{
		UISubTabClanMgt_Info * pThis = GetThis();
		if( pThis != nullptr)
			pThis->OnModifyClanInfo();
		return 0;
	}

	int UISubTabClanMgt_Info_CancelModify(LuaState * L)
	{
		UISubTabClanMgt_Info * pThis = GetThis();
		if( pThis != nullptr)
			pThis->OnInitClanInfo();
		return 0;
	}

	int UISubTabClanMgt_Info_NoticeKillFocus(LuaState * L)
	{
		UISubTabClanMgt_Info * pThis = GetThis();
		if ( pThis != nullptr)
			pThis->OnNoticeKillFocus();
		return 0;
	}
}

LuaFuncReg UISubTabClanMgt_Info_Glue[] = {
	{"SendMessage",				UISubTabClanMgt_Info_SendMessage	},
	{"ModifyInfo",				UISubTabClanMgt_Info_ModifyInfo	},
	{"CancelModify",			UISubTabClanMgt_Info_CancelModify	},
	{"NoticeKillFocus",			UISubTabClanMgt_Info_NoticeKillFocus },
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UISubTabClanMgt_Info::UISubTabClanMgt_Info()
{
	m_pClanContext = g_pFramework->GetClanContext();

	for(INT32 i = 0 ; i < 2 ; i++)
	{
		m_pEdInputCheck[i] = nullptr;
	}

}

UISubTabClanMgt_Info::~UISubTabClanMgt_Info()
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		m_pEdInputCheck[i] = nullptr;
	}
}

void UISubTabClanMgt_Info::OnClanSendMessage( void)
{
	if(m_pClanContext->IsEnter() == false)
		return;

	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"마스터 권한이 필요합니다."
		return;
	}
	if ( CanClanMemberSendNoteAll() )
	{
		if(g_pEnvSet->m_nUIVersion != UFV_1_1)
		{
			UIPopupReceiveMail::OPEN_DATA openData;

			openData.mailType = UIPopupReceiveMail::CLAN_SEND;
			openData.wstrNickName = GAME_RCSTRING("STBL_IDX_CLAN_ALL_MEMBER");
			GetMainFrame()->OpenPopup( UPW_MAILREV, &openData);
		}
		else 
		{
			GetMainFrame()->OpenPopup( UPW_SEND_CLANMAIL);
		}
	}
	else
	{
		//차후에 지원될 예정입니다.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_PREPARE_PLAN"));
	}
}

void UISubTabClanMgt_Info::OnInitClanInfo( void)
{
	_InitUI();
	_CheckIMELength();
}

void UISubTabClanMgt_Info::OnNoticeKillFocus(void)
{
	i3::stack_wstring wstrNotice; _GetNoticeMsg(wstrNotice);

	LuaState * L = _CallLuaFunction("EnableNoticeText");
	if (L != nullptr)
	{
		if (wstrNotice.empty())
		{
			i3Lua::PushBoolean(L, true);
		}
		else
		{
			i3Lua::PushBoolean(L, false);
		}
	}

	_EndLuaFunction(L, 1);
}


void UISubTabClanMgt_Info::OnModifyClanInfo( void)
{
	if(m_pClanContext->IsEnter() == false)
		return;

	i3::stack_wstring wstrNotice;    _GetNoticeMsg(wstrNotice);

	i3::stack_wstring wstrIntro;	_GetIntroMsg(wstrIntro);

	UINT32 i32Radio = _GetAcceptOption();

	if(g_pFramework->IsFilterOK(wstrNotice) == false || g_pFramework->IsFilterOK(wstrIntro) == false)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
		return;
	}
	else
	{
		CLAN_DETAIL_INFO * pInfo = m_pClanContext->GetMyClanDetailInfo();
		// 클랜 공지 수정 완료
		if( !i3::generic_is_equal( wstrNotice, pInfo->m_announce ) )	
			m_pClanContext->BoardNoticeModify( wstrNotice );			
		//	클랜소개 수정 완료

		if( !i3::generic_is_equal( wstrIntro, pInfo->m_intro ) )					
			m_pClanContext->BoardGuideModify(wstrIntro);				

		// 마스터 && 상태 변경시에만 수정
		if(m_pClanContext->IsMaster() && i32Radio != pInfo->m_reqoption)
			m_pClanContext->JoinOptionModify((UINT32)i32Radio);
	}
}

/*virtual*/ void UISubTabClanMgt_Info::OnCreate(i3GameNode * pParent)
{
	UISubTabClan_Mgt::OnCreate(pParent);

	AddScene("Scene/Clan/PointBlankRe_Clan_Management_Mgt.i3UIs", UISubTabClanMgt_Info_Glue);
}

/*virtual*/ bool UISubTabClanMgt_Info::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (UISubTabClan_Mgt::OnEntranceStart(pArg1, pArg2) == false ) return false;

	OnInitClanInfo();
	return true;
}


/*virtual*/ void UISubTabClanMgt_Info::_InitializeAtLoad( i3UIScene * pScene)
{
	UISubTabClan_Mgt::_InitializeAtLoad( pScene);

	m_pEdInputCheck[0] = (i3UIEditBox*) pScene->FindChildByName( "Edit_Notice");
	I3ASSERT( m_pEdInputCheck[0]);

	m_pEdInputCheck[1] = (i3UIEditBox*) pScene->FindChildByName( "Edit_Intro");
	I3ASSERT( m_pEdInputCheck[1]);
}

/*virtual*/ void UISubTabClanMgt_Info::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UISubTabClan_Mgt::ProcessGameEvent( event, arg, UserData);

	switch(event)
	{
	case EVENT_CLAN_NOTICE_MODIFY:
		{
			if( EV_SUCCESSED(arg))
			{
				i3::stack_wstring wstrNotice; _GetNoticeMsg(wstrNotice);
				i3::safe_string_copy( m_pClanContext->GetMyClanDetailInfo()->m_announce, wstrNotice, NET_CLAN_INTRO_SIZE );
			}
		}
		break;
	case EVENT_CLAN_GUIDE_MODIFY:
		{
			if( EV_SUCCESSED(arg))
			{
				i3::stack_wstring wstrIntro; _GetIntroMsg(wstrIntro); 
				
				i3::safe_string_copy(m_pClanContext->GetMyClanDetailInfo()->m_intro, wstrIntro, NET_CLAN_INTRO_SIZE);
			}
		}
		break;
	case EVENT_CLAN_ACCEPT_OPTION_MODIFY:
		{
			if( EV_SUCCESSED(arg))
			{
				m_pClanContext->GetMyClanDetailInfo()->m_reqoption = _GetAcceptOption();
			}
		}
		break;
	case EVENT_CLAN_NOTE:
		{
			if(EV_SUCCESSED(arg))
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_SUCCESS_SEND_MESSAGE"));/*쪽지 전달에 성공하였습니다.*/
				if(g_pEnvSet->m_nUIVersion == UFV_1_1)
					GetMainFrame()->ClosePopup(UPW_SEND_CLANMAIL);
			}			
			else
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_FAILED_SEND_NOTE"));/*쪽지 보내기에 실패했습니다.\n 다시 시도해 주세요.*/
			}
		}
		break;
	}
}

/*virtual*/ void UISubTabClanMgt_Info::_ClearAtUnload( i3UIScene * pScene)
{
	UISubTabClan_Mgt::_ClearAtUnload( pScene);

	for(INT32 i = 0 ; i < 2 ; i++)
	{
		m_pEdInputCheck[i] = nullptr;
	}
}

void UISubTabClanMgt_Info::_InitUI( void)
{
	CLAN_DETAIL_INFO * pMyClanInfo = m_pClanContext->GetMyClanDetailInfo();

	LuaState * L = _CallLuaFunction("InitUI");

	if(L != nullptr)
	{
		if (i3::generic_compare(pMyClanInfo->m_announce, "") != 0)
		{
			i3Lua::PushStringUTF16To8(L, pMyClanInfo->m_announce);
		}
		else
		{
			i3::rc_wstring wstrDefault;
			wstrDefault = GAME_RCSTRING("STBL_IDX_CLAN_NOTICE_TEXTBOX");
			i3Lua::PushStringUTF16To8(L, wstrDefault);
		}

		i3Lua::PushStringUTF16To8(L, pMyClanInfo->m_intro);
		i3Lua::PushInteger(L , pMyClanInfo->m_reqoption);
		i3Lua::PushBoolean(L , m_pClanContext->IsMaster());	// 마스터 여부 
		_EndLuaFunction(L, 4);
	}
}

void UISubTabClanMgt_Info::_CheckIMELength( void)
{
	for(INT32 i= 0; i < 2; i++)
	{
		if( m_pEdInputCheck[i] == nullptr)
			continue;

		if( m_pEdInputCheck[i]->GetIME() == nullptr )
			continue;			

		const i3::wstring& wstrIMETextBuffer = m_pEdInputCheck[i]->getIMETextBufString();

		int writtenSize = (int)wstrIMETextBuffer.size();

		if( writtenSize >= NET_CLAN_INTRO_SIZE )
		{
			m_pEdInputCheck[i]->GetIME()->SetInputFinish();
			m_pEdInputCheck[i]->SetIMEText( m_wstrInputheck[i].c_str() );
		}
		else
		{
			m_wstrInputheck[i] = wstrIMETextBuffer;
		}
	}
}

void UISubTabClanMgt_Info::_GetNoticeMsg( i3::stack_wstring& out)
{
	LuaState * L = _CallLuaFunction("GetNoticeMsg");					

	if(L != nullptr)
	{
		const char* szRes = _ReturnLuaString(L , 0);
		if (szRes)
		{
			i3::utf8_to_utf16(szRes, out);
		}
		else
			out.clear();
	}
	else
		out.clear();
}

void UISubTabClanMgt_Info::_GetIntroMsg( i3::stack_wstring& out)
{
	LuaState * L = _CallLuaFunction("GetIntroMsg");						

	if(L != nullptr)
	{
		const char* szRes = _ReturnLuaString(L , 0);
			
		if (szRes)
		{
			i3::utf8_to_utf16(szRes, out);
		}
		else
			out.clear();
	}
	else
		out.clear();
}

UINT32 UISubTabClanMgt_Info::_GetAcceptOption( void)
{
	LuaState * L = _CallLuaFunction("GetAcceptOption");

	if(L != nullptr)
		return _ReturnLuaInteger(L , 0);
	else
		return 0;
}
