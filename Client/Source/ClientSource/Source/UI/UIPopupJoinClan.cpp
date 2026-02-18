#include "pch.h"
#include "UIPopupJoinClan.h"
#include "UIMainFrame.h"
#include "GlobalVariables.h"

#include "UIPhaseNonClanMember.h"

#include "UIUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

#include "ClanGameContext.h"

I3_CLASS_INSTANCE( UIPopupJoinClan);//, UIPopupBase);

extern "C" 
{
	INT32 UIPopupJoinClan_JoinSend(LuaState* L)
	{
		UIPopupJoinClan* p = static_cast<UIPopupJoinClan*>(GameUI::GetPopup(UPW_CLAN_JOIN));
		if( p != nullptr)
			p->JoinSend();
		return 0;
	}

	INT32 UIPopupJoinClan_Cancel(LuaState* L)
	{
		UIPopupJoinClan* p = static_cast<UIPopupJoinClan*>(GameUI::GetPopup(UPW_CLAN_JOIN));
		if( p != nullptr)
			p->Close();
		return 0;
	}
}

LuaFuncReg UIPopupJoinClan_Glue[] =
{
	{ "JoinSend",	UIPopupJoinClan_JoinSend },
	{ "Cancel",		UIPopupJoinClan_Cancel },
	{nullptr,			nullptr}
};

UIPopupJoinClan::UIPopupJoinClan(void)
{
	m_pClanMark = nullptr;
	m_pClanName = nullptr;

}

UIPopupJoinClan::~UIPopupJoinClan(void)
{
	m_pClanMark = nullptr;
	m_pClanName = nullptr; 
}

/*virtual*/ void UIPopupJoinClan::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);
	
	// Fillter 적용
	if( m_StringFilter == nullptr)
	{
		m_StringFilter.reset(i3::make_pscoped<CStrFilter>()); 
		m_StringFilter->CreateFilter(STR_FILTER_MATCH_NICK);//CreateNickName();
	}
	// Fillter 적용
	if( m_StringFilter2 == nullptr)
	{
		m_StringFilter2.reset(i3::make_pscoped<CStrFilter>()); 
		m_StringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);//CreateFIlterNick();
	}

	m_pClanMark = (i3UIImageBoxEx *)pScene->FindChildByName("ImgBox_ClanMark");
	I3ASSERT(m_pClanMark);

	m_pClanName = (i3UIStaticText *)pScene->FindChildByName("StaticText_ClanName");
	I3ASSERT(m_pClanName);

	m_FlowEditBox.SetFlowEditBox((i3UIEditBox *)pScene->FindChildByName("Edit_ClanIntro"));

	_InitClanData();
}

void UIPopupJoinClan::_InitClanData( void)
{
	CLAN_DETAIL_INFO * pInfo = ClanGameContext::i()->getClanDetailInfo();

	GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_MEDIUM, pInfo->m_mark);
	// 여기서 사용되는 클랜마크 사이즈는 64 x 64
	m_pClanMark->SetShapeSize(64, 64);

	i3::rc_wstring wstrClanName = pInfo->m_name;

	m_pClanName->SetText(wstrClanName);

	i3::rc_wstring wstrIntro = pInfo->m_intro;

	m_FlowEditBox.SetFlowTextData(wstrIntro);
	m_FlowEditBox.PlayEditTextFlow(TFD_HORIZONTAL, true, true);
}

/*virtual*/ void UIPopupJoinClan::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	m_pClanMark = nullptr;
	m_pClanName = nullptr; 

	m_FlowEditBox.Reset();
}

void UIPopupJoinClan::_SubscriptionSendJoin(void)
{
	g_pFramework->GetClanContext()->SendJoinToClanMember( ClanGameContext::i()->getClanDetailInfo()->m_ui32CID,  m_wstrText);
}

/* virtual */ void UIPopupJoinClan::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);
	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_ClanJoin.i3UIs", UIPopupJoinClan_Glue);
}

/* virtual */ void UIPopupJoinClan::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);
	
	m_FlowEditBox.OnUpdate(rDeltaSeconds);
}

bool UIPopupJoinClan::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	bool bRv = UIPopupBase::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_RETURN:
			{
				JoinSend();
			}
			break;
		}
	}

	return bRv;
}

/*virtual*/ void UIPopupJoinClan::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	if( evt == EVENT_CLAN_JOIN_REQUEST)
		ReceiveGameEvent_Join_Request( arg, UserData);
}

void UIPopupJoinClan::JoinSend( void)
{
	{
		LuaState * L = _CallLuaFunction("GetRequestText");

		if(L != nullptr)
		{
			const char * pszTemp = _ReturnLuaString(L , 0);	
			
			i3::utf8_to_utf16(pszTemp, m_wstrText);			//  mb 대신 UTF8로 교체 해야할 것이다...(2014.07.14.수빈)
//			i3::safe_string_copy(m_szText, pszTemp , FONT_COUNT_512);
		}

		if( false == m_StringFilter2->IsFilterOK( m_wstrText ) || 
			false == m_StringFilter->IsFilterMatchOK( m_wstrText ) || 
			false == g_pFramework->IsFilterOK( m_wstrText ) ) 
		{
			//	"입력 할 수 없는 단어가 포함되어 있습니다."
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_BAD_WORD2"));
			
			return;
		}
	}

	//	"가입을 신청하시겠습니까?"
	//	"클랜 가입 신청"
	GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_REQUEST_SUBSCRIPTION_Q"), 
		&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_REQUEST_SUBSCRIPTION"), MAKE_CALLBACK(cbQuaryJoinClan), this);
}

void UIPopupJoinClan::Close( void)
{
	GetMainFrame()->ClosePopup(UPW_CLAN_JOIN);
}

void UIPopupJoinClan::cbQuaryJoinClan(void* pThis, INT32 nParam)
{
	UIPopupJoinClan * pPopup = (UIPopupJoinClan*) pThis;

	if( nParam == MB_OK)
	{
		pPopup->_SubscriptionSendJoin();
	}
}


void UIPopupJoinClan::ReceiveGameEvent_Join_Request( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED( arg ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_COMPLETE_SUBSCRIPTION"));	//	"가입신청이 완료되었습니다."
		Close();
	}
	else
	{
		switch( arg ) {
			case EVENT_ERROR_CLAN_NOSEARCH_CLANIDX:
				{
					//	"\n가입 신청 실패하였습니다.\n클랜을 찾을수 없습니다."
					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_NO_FINDING"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_MAXREQUEST:
				{
					//	"\n클랜 가입 처리가 실패했습니다.\n더이상 가입 할 수 없습니다."
					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_OVER_MEMBER"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;			
			case EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX:
				{
					//	"\n가입 신청 실패하였습니다.\n이미 다른 클랜에 가입되어 있거나 가입 요청한 상태입니다."
					//	클랜 가입 승인 실패	
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_ALREADY_SUBSCRIPTION"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_REQUEST_FAIL:
				{
					//	"\n가입 신청 실패하였습니다"
					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_SUBSCRIPTION_FAIL"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL:
				{
					//	해당 클랜에 가입할 수 있는 연령과 맞지 않아 클랜 가입 신청이 취소 되었습니다."
					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_AGE"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			case EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL:
				{
					//	"해당 클랜에 가입할 수 있는 계급과 맞지 않아 클랜 가입 신청이 취소 되었습니다."
					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION_RANK"), 
						&GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
			default:
				{
					i3::rc_wstring wstrTemp;
					//	"\n가입 신청에 실패하였습니다.\n 죄송합니다. 관리자에게 문의하여 주십시오."
					i3::sprintf( wstrTemp, L"%s\n [Errcode : 0x%08x]", 
						GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANNOT_SUBSCRIPTION"), arg );

					//	클랜 가입 승인 실패
					GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrTemp, &GAME_RCSTRING("STBL_IDX_CLAN_CAPTION_SUBSCRIPTION_FAIL"));
				}
				break;
		}
	}
}