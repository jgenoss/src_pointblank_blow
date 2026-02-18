#include "pch.h"
#include "UIPhaseBase.h"

#include "UIFramework.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UISideMenu.h"

#include	"UIPhaseProfile.h"
#include	"UIPhaseClanLobby.h"
#include	"UITabProfileInfo.h"
#include	"UserInfoContext.h"
#include	"UIFloatMercenaryList.h"

#include "../CommunityContext.h"

#include "i3Base/string/ext/snprintf.h"

#include "ReadyBoxContext.h"
#include "ViewOtherUserItemContext.h"
#include "ShopContext.h"
#include "UIPopupQuickMedalReward.h"

#include "../ChannelContext.h"
#include "../ClanSeasonContext.h"
#include "../UserInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "MailBoxContext.h"
#include "ClanGameContext.h"

#include "UITopMenu.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "../USG_Miscs.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIPhaseBase, UIBase);
I3_CLASS_INSTANCE( UIPhaseBase);

UIPhaseBase::UIPhaseBase()
{
	m_pScene = nullptr;
	m_bMyPhase = false;

	RegisterGameEventFunc( EVENT_INVITED_MESSAGE,				&UIPhaseBase::ReceiveGameEvent_InvitedMessage);
	RegisterGameEventFunc( EVENT_INVITED_CLAN_MESSAGE,			&UIPhaseBase::ReceiveGameEvent_InvitedMessageClan);
	RegisterGameEventFunc( EVENT_INVITED_NORMAL_USER_MESSAGE,	&UIPhaseBase::ReceiveGameEvent_InvitedMessageNormalUser);
	RegisterGameEventFunc( EVENT_CHANGE_CHARA_NICK,				&UIPhaseBase::ReceiveGameEvent_Change_Chara_Nick);
	RegisterGameEventFunc( EVENT_M_DRIVE_AWAY,					&UIPhaseBase::ReceiveGameEvent_DriveAway);
	RegisterGameEventFunc( EVENT_M_ERROR,						&UIPhaseBase::ReceiveGameEvent_Error);
	RegisterGameEventFunc( EVENT_M_DISCONNECTION,				&UIPhaseBase::ReceiveGameEvent_Disconnection);
	RegisterGameEventFunc( EVENT_M_GAMEGUARD_ERROR,				&UIPhaseBase::ReceiveGameEvent_GameGuard_Error);
	RegisterGameEventFunc( EVENT_ENTER_INVENTORY,				&UIPhaseBase::ReceiveGameEvent_Enter_Inventory);
	RegisterGameEventFunc( EVENT_LEAVE_INVENTORY,				&UIPhaseBase::ReceiveGameEvent_Leave_Inventory);
	RegisterGameEventFunc( EVENT_ITEM_AUTH,						&UIPhaseBase::ReceiveGameEvent_Auth_Item);
	RegisterGameEventFunc( EVENT_USE_ITEM_CHECK_NICK,			&UIPhaseBase::ReceiveGameEvent_Auth_Item);

	RegisterGameEventFunc( EVENT_ENTER_PROFILE,					&UIPhaseBase::ReceiveGameEvent_Enter_Profile);
	RegisterGameEventFunc( EVENT_LEAVE_PROFILE,					&UIPhaseBase::ReceiveGameEvent_Leave_Profile);

	RegisterGameEventFunc( EVENT_ENTER_INFO,					&UIPhaseBase::ReceiveGameEvent_Enter_Info);
	RegisterGameEventFunc( EVENT_LEAVE_INFO,					&UIPhaseBase::ReceiveGameEvent_Leave_Info);
	RegisterGameEventFunc( EVENT_ENTER_SHOP,					&UIPhaseBase::ReceiveGameEvent_Enter_Shop);
	RegisterGameEventFunc( EVENT_LEAVE_SHOP,					&UIPhaseBase::ReceiveGameEvent_Leave_Shop);
	RegisterGameEventFunc( EVENT_DELETE_NOTE_ASK,				&UIPhaseBase::ReceiveGameEvent_Delete_NoteAsk);
	RegisterGameEventFunc( EVENT_RECEIVE_NOTE,					&UIPhaseBase::ReceiveGameEvent_Note);
	RegisterGameEventFunc( EVENT_RECEIVE_GIFT,					&UIPhaseBase::ReceiveGameEvent_Note);
	RegisterGameEventFunc( EVENT_INSERT_BLOCK,					&UIPhaseBase::ReceiveGameEvent_Insert_Block);
	RegisterGameEventFunc( EVENT_ACCEPT_FRIEND,					&UIPhaseBase::ReceiveGameEvent_Accept_Friend);
	RegisterGameEventFunc( EVENT_DELETE_FRIEND,					&UIPhaseBase::ReceiveGameEvent_Delete_Friend);
	RegisterGameEventFunc( EVENT_CLAN_INVITE,					&UIPhaseBase::ReceiveGameEvent_Invite_Clan);
	RegisterGameEventFunc( EVENT_ENTER_GACHA,					&UIPhaseBase::ReceiveGameEvent_Enter_Gacha);
	RegisterGameEventFunc( EVENT_LEAVE_GACHA,					&UIPhaseBase::ReceiveGameEvent_Leave_Gacha);
	RegisterGameEventFunc( EVENT_CLAN_ENTER,					&UIPhaseBase::ReceiveGameEvent_Enter_Clan);
	RegisterGameEventFunc( EVENT_CLAN_LEAVE,					&UIPhaseBase::ReceiveGameEvent_Leave_Clan);
	RegisterGameEventFunc( EVENT_VIEW_OTHER_USER_ITEM_INFO,		&UIPhaseBase::ReceiveGameEvent_View_Other_User_Item_Info);
	RegisterGameEventFunc( EVENT_M_SHUTDOWNTIME,				&UIPhaseBase::ReceiveGameEvent_ShutDown_Time);
	RegisterGameEventFunc( EVENT_ENTER_GIFTSHOP,				&UIPhaseBase::ReceiveGameEvent_Enter_GiftShop);
	RegisterGameEventFunc( EVENT_LEAVE_GIFTSHOP,				&UIPhaseBase::ReceiveGameEvent_Leave_GiftShop);
	RegisterGameEventFunc( EVENT_FAIL_ENTER_GIFTSHOP,			&UIPhaseBase::ReceiveGameEvent_Fail_Enter_GiftShop);
	RegisterGameEventFunc( EVENT_GM_EXIT_USER,					&UIPhaseBase::ReceiveGameEvent_GM_User_Exit);
	RegisterGameEventFunc( EVENT_ENTER_TEMP,					&UIPhaseBase::ReceiveGameEvent_Enter_Temp);
	RegisterGameEventFunc( EVENT_LEAVE_TEMP,					&UIPhaseBase::ReceiveGameEvent_Leave_Temp);

	RegisterGameEventFunc( EVENT_M_CLAN_DEPROTATION,			&UIPhaseBase::ReceiveGameEvent_Clan_Deprotation);

	RegisterGameEventFunc( EVENT_HACKTOOL_DETECTED,				&UIPhaseBase::ReceiveGameEvent_HackToolDetected);
}

UIPhaseBase::~UIPhaseBase()
{
}

/*virtual*/ void UIPhaseBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	if( m_pScene == nullptr)
	{
		m_pScene = pScene;
	}
}

/*virtual*/ void UIPhaseBase::_ClearAtUnload( i3UIScene * pScene)
{
	UIBase::_ClearAtUnload( pScene);
	m_pScene = nullptr;
}

#include "UIShopFunction.h"
namespace inner
{
	void GetItemNameList(i3::wstring & NameList, const i3::vector<T_ItemID>& ItemList)
	{
		const size_t Max = ItemList.size();
		i3::rc_wstring wstrItemName;

		for (size_t i = 0 ; i < Max ; ++i )
		{
			if ( ItemList[i] == 0 ) continue;

			usf::GetShopItemName(wstrItemName, ItemList[i]);

			NameList.append(wstrItemName.begin(), wstrItemName.end());

			if ( i != Max-1)
				NameList += L", ";
		}
	}
}

void UIPhaseBase::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

/*virtual*/ void UIPhaseBase::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	CShopContext::i()->OnGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}


/*virtual*/ bool UIPhaseBase::OnKey_Escape( void)
{
	bool bEscape = true;
	if( i3::same_of<UIMainFrame*>(GetMainFrame()))
	{
		if( UISideMenu::instance()->clicked_escape_key() == true ) return true; //왼쪽 메뉴가 활성화 상태면 esc 키로 비활성화

		bEscape = GetMainFrame()->EscapeKey();
	}

	if( bEscape)
	{
		OnEscapeKey();
		return true;
	}

	return true;
}


void	UIPhaseBase::OnSlideInOnce()
{
	UIBase::OnSlideInOnce();

	// 진입 시점에서 사운드 출력
	if (g_pEnvSet->IsBaseOnUIVersion_1_0() )
		PlaySound(UISND_SCROLLIN);
	else if (g_pEnvSet->GetUIVersion() == UFV_1_1)
		PlaySound(UISND_POPUP_FADE_IN);
}

void	UIPhaseBase::OnSlideOutOnce()
{
	UIBase::OnSlideOutOnce();

	// 퇴장 시점에서 사운드 출력
	if ( g_pEnvSet->IsBaseOnUIVersion_1_0() )
		PlaySound(UISND_SCROLLOUT);
	else if (g_pEnvSet->GetUIVersion() == UFV_1_1)
		PlaySound(UISND_PAGE_SHIFT);
}



/*virtual*/ bool UIPhaseBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
i3_prof_func();

i3_prof_start("UIBase::OnEntranceStart call");
	if( UIBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
i3_prof_end();

	// 진입시 입력을 막는다.
i3_prof_start("SetInputDisable call");
	SetInputDisable(true);
i3_prof_end();


i3_prof_start("g_pFramework->PreventMessageBoxWhenPhaseChange call");
	// 페이즈및 서브페이즈의 진입시작시점부터 메세지 박스 처리를 홀드시킨다..
	g_pFramework->PreventMessageBoxWhenPhaseChange();
	//
i3_prof_end();

i3_prof_start("GetMainFrame()->TogglePopup call");
	GetMainFrame()->TogglePopup( UPW_NULL);
i3_prof_end();

	m_bMyPhase = true;
	
	return true;
}

/*virtual*/ void UIPhaseBase::OnEntranceEnd( void)
{
	UIBase::OnEntranceEnd();

	// Phase Move가 발생시 Clan Popup에 입장 했을 경우 나가는 처리를 해야 합니다.jinsik.kim(2015.1.22)
	CClanContext* pClanContext = g_pFramework->GetClanContext();
	if(pClanContext != nullptr)
	{
		if(pClanContext->IsEnter())
		{
			pClanContext->LeaveServer();
		}
	}

	// 입장 완료시 팝업이 없으면 입력 가능
	if( g_pFramework->IsMessageBoxEnabled() == false)
	{
		SetInputDisable( false);
	}
}

/*virtual*/ bool UIPhaseBase::OnExitStart( void)
{
	if( UIBase::OnExitStart() == false)
		return false;

	SetInputDisable( true);
	
	// 페이즈 전환 시작시점에서는 일반 팝업은 그냥 닫아버린다..
	i3Stage * pStage = g_pFramework->GetCurrentStage();
	if(i3::same_of<UIFramework*>(pStage))
		GetMainFrame()->TogglePopup(UPW_NULL);
	//
	
	return true;
}

void	UIPhaseBase::OnUpdate(REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate(rDeltaSeconds);

	OpenInviteMessage();

	// Key : Clan Season End Notice
	if( ChannelContext::i()->GetBattleChannel() )
	{
		i3::rc_wstring wstr_notice; 

		if( ClanSeasonContext::i()->CheckToSeasonEndNotice(wstr_notice) )
		{
			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_notice);
		}
	}
	
	this->OnUpdate_NoneRsc(rDeltaSeconds);
}


void UIPhaseBase::ReceiveGameEvent_DriveAway( INT32 arg, const i3::user_data&)
{
	switch( arg)
	{
	case ACCOUNT_KICK_LOGIN:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_LOGIN_E"), nullptr, EXIT_CALLBACK(CbExitNormal, true), this);
		break;
	case ACCOUNT_KICK_GM_TOOL:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_TOOL_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	case ACCOUNT_KICK_OTHER:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	default:
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		break;
	}

	GameEventSender::i()->SetEvent(EVENT_DISCONNECT);
}

bool UIPhaseBase::IsRun(void)
{
	// 스탭 윈도우의 이동이 끝났으면 참, 이동중이면 거짓.
	// 스탭 윈동우가 등장완료, 퇴장완료 이면 이동이 끝난다.
	if (SS_IN_OK == m_SlideState || SS_OUT_OK == m_SlideState)
	{
		return true;
	}

	return false;
}

UIMainFrame * UIPhaseBase::GetMainFrame( void)
{
	return ((UIFramework*) m_pParent)->GetUIMainframe();
}


void UIPhaseBase::_ApplyMsgReadyBox()
{
	if( ReadyBoxContext::i()->EmptyReadyBoxMsg() )
		return;

	size_t size = ReadyBoxContext::i()->GetReadyBoxMsg_Size();
	i3::rc_wstring wstrNameBuffer;
	i3::rc_wstring wstrBuf;

	for(size_t i = 0; i<size; ++i)
	{
	
		T_ItemID ItemID = ReadyBoxContext::i()->GetReadyBoxMsg_ItemID( i );
		g_pShopItemInfoDataBase->GetItemName(ItemID, wstrNameBuffer);

		//채팅창에 자랑한다.
		//알림 : {col:240, 30, 40, 255}%d{/col} 번방 {col:240, 30, 40, 255}%s{/col} 님이 {col:240, 30, 40, 255}%s{/col}를 획득하셨습니다.
		// char strBuf[MAX_STRING_COUNT];
		// memset(strBuf, 0, sizeof(strBuf));
	
		i3::sprintf(wstrBuf, GAME_RCSTRING("STR_READYBOX_COMMENT"), 
			ReadyBoxContext::i()->GetReadyBoxMsg_RoomIdx( i ) + 1,
			ReadyBoxContext::i()->GetReadyBoxMsg_Nick( i ), wstrNameBuffer	);
		
		//채팅창에 먹은 사람 자랑한다.
		g_pFramework->getChatBox()->PutSystemChatting( wstrBuf );
	}

	ReadyBoxContext::i()->ClearReadyBoxMsg();
}

void UIPhaseBase::ReceiveGameEvent_InvitedMessage( INT32 arg, const i3::user_data& UserData)
{
	OpenInvitemessage_Friend();
}

void UIPhaseBase::ReceiveGameEvent_InvitedMessageClan( INT32, const i3::user_data&)
{
	OpenInviteMessage_Clan();
}

void UIPhaseBase::ReceiveGameEvent_InvitedMessageNormalUser( INT32, const i3::user_data&)
{
	OpenInviteMessage_NormalUser();		
}

bool UIPhaseBase::IsEnableInviteMsg(INT64 i64UID)
{
	// 옵션에 따라 초대하기 창을 활성/비활성 상태를 결정한다.
	switch( g_pEnvSet->m_nInviteType)
	{
	case COMMUNITY_PERMISSION_ALL		: return true;		// 전체 허용	
	case COMMUNITY_PERMISSION_NO		: return false;		// 전체 차단	
	case COMMUNITY_PERMISSION_FRIEND	:									// 친구/클랜 허용	
		{
			INT32 i = 0;
			INT32 nFriendCount = CCommunityContext::i()->GetFriendCount();
			for( i=0; i < nFriendCount; i++)
			{
				const FRIEND_NODE* pNode = CCommunityContext::i()->GetFriend(i);

				if( pNode->_uid == i64UID)
				{
					return true;
				}
			}

			INT32 nClanMemberCount = CCommunityContext::i()->GetClanMemberCount();

			for( i=0; i < nClanMemberCount; i++)
			{
				const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetClanMember(i);

				if( pNode->_uid == i64UID)
				{
					return true;
				}
			}
		}
		break;
	default								: return true;			
	}

	return false;
}

void UIPhaseBase::OpenInviteMessage(void)
{
	// 이미 처리중인 메세지가 있으면 건너띕니다.
	if( CCommunityContext::i()->IsReceivedInvite() )
		return;

	// 지연 초대 Flag 체크.
	if( false == CCommunityContext::i()->IsDleayedInvite() )
		return;
	
	// 이벤트로 처리해야 됩니다. UPDATE중에 강제로 팝업 띄우면 안되요.
	switch( CCommunityContext::i()->GetInviteType() )
	{
	case INVITE_TYPE_NORMAL : GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_NORMAL_USER_MESSAGE);	break;
	case INVITE_TYPE_FRIEND : GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_MESSAGE);				break;
	case INVITE_TYPE_CLAN	: GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVITED_CLAN_MESSAGE);		break;
	default					: break;
	}
}

void UIPhaseBase::OpenInviteMessage_NormalUser(void)
{
	// 이미 처리중인 메세지가 있으면 건너띕니다.
	if( CCommunityContext::i()->IsReceivedInvite() )
		return;

	// 서브페이즈가 활성화 중인 경우, 초대 메세지를 딜레이 시킴.
	if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE )
	{
		CCommunityContext::i()->SetDelayedInvite(true, INVITE_TYPE_NORMAL);
		return;
	}

	// 일반유저 초대의 경우 전체 허용만 초대메세지를 받습니다.
	if(g_pEnvSet->m_nInviteType != COMMUNITY_PERMISSION_ALL)
		return;

	const INVITED_USER_NODE * pNode = CCommunityContext::i()->GetInvitedNormalUserNode();

	if(pNode->_UserInfo.m_ConnectState == 0)
		return;

	i3::rc_wstring wstrNick = pNode->_UserInfo.m_strNick;
	
	PopupInviteMessage(pNode->_UserInfo.m_ConnectState, wstrNick, CbInviteMove);
}

void UIPhaseBase::OpenInvitemessage_Friend(void)
{
	// 이미 처리중인 메세지가 있으면 건너띕니다.
	if( CCommunityContext::i()->IsReceivedInvite() )
		return;

	// 서브페이즈가 활성화 중인 경우, 초대 메세지를 딜레이 시킴.
	if( GetMainFrame()->IsSubPhaseNone() == false )
	{
		CCommunityContext::i()->SetDelayedInvite(true, INVITE_TYPE_FRIEND);
		return;
	}

	const FRIEND_NODE* node = CCommunityContext::i()->GetFriendNodeOfInviteUser();

	if( !node)
		return;

	if( IsEnableInviteMsg(node->_uid) == false)
		return;

	PopupInviteMessage(node->_state, node->_wstr_nick, CbInviteMove);
}

void UIPhaseBase::OpenInviteMessage_Clan(void)
{
	// 이미 처리중인 메세지가 있으면 건너띕니다.
	if( CCommunityContext::i()->IsReceivedInvite() )
		return;

	// 서브페이즈가 활성화 중인 경우, 초대 메세지를 딜레이 시킴.
	if( GetMainFrame()->IsSubPhaseNone() == false )
	{
		CCommunityContext::i()->SetDelayedInvite(true, INVITE_TYPE_CLAN);
		return;
	}

	const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetClanMemberNodeOfInviteUser();

	if( !node)
		return;

	if( IsEnableInviteMsg(node->_uid) == false)
		return;

	PopupInviteMessage(FRIEND_STATE_64_TO_32(node->_state), node->_wstr_nick, CbInviteMove);

}

void UIPhaseBase::PopupInviteMessage( const CUserLocationInfo & UserLocInfo, const i3::rc_wstring& wstrNickName, CbINVITEPOPUP pCallBackFunc)
{
	i3::stack_wstring wstrMessage;
	i3::stack_wstring wstrServer;
	i3::wstring wstrMatchName;
	
	UINT32 server = UserLocInfo.GetDisplayServerChannelNumber();
	UINT32 MatchTeamIndex = CCommunityContext::i()->GetMoveTargetMatch();
	
	i3::sprintf( wstrServer, L"%03d", server);
	i3::sprintf(wstrMatchName, L"%03d", MatchTeamIndex - 1);

	if( MatchTeamIndex != MATCH_TEAM_INVALID_IDX)
	{		
		// 수락 여부 확인		
		/*%03d번 서버의 %s 팀의 [%s]님으로부터 초대요청 메시지가 도착하였습니다. \n수락하시겠습니까?*/
		i3::sprintf(wstrMessage, GAME_RCSTRING("STR_TBL_GUI_INVITE_MESSAGEBOX_TO_TEAM"), wstrServer, wstrMatchName, wstrNickName);
	}
	else
	{
		UINT32 room = UserLocInfo.GetDisplayRoomNumber();
		// 수락 여부 확인
		/*%03d번 서버의 %d번 방의 [%s]님으로부터 초대요청 메시지가 도착하였습니다. \n수락하시겠습니까?*/
		i3::sprintf(wstrMessage, GAME_RCSTRING("STR_TBL_GUI_INVITE_MESSAGEBOX_TO_ROOM"), wstrServer, room, wstrNickName);
	}

	// 최신의 초대 메세지 팝업만 쓸것이기 때문에 존재하는 나머지 팝업은 조용이 닫어버린다...( 질의 메세지박스의 경우 Cancel동작이 되어야하므로 잘 체크할 필요가 있음).. 11.11.02.수빈.
	// GameUI::CloseAllMsgBox();	//친구초대 팝업 Maximum유지위해 막습니다.
	//
	UIMessageBox * msg = GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstrMessage, nullptr, MAKE_CALLBACK(pCallBackFunc), this);
	if( GetMainFrame()->IsOpenedPopup(UPW_MEDAL_QUICK_REWARD) == true)
	{
		msg->SetEnable(false);
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		pFrame->FocusPopup(msg, false, __FILE__, __LINE__);	
		UIPopupQuickMedaiReward * pQuick = (UIPopupQuickMedaiReward *)GetMainFrame()->GetPopup(UPW_MEDAL_QUICK_REWARD);
		pQuick->SetInviteMsg(msg);
	}

	// 초대 메세지 딜레이 리셋
	CCommunityContext::i()->SetDelayedInvite(false, INVITE_TYPE_NONE);
	CCommunityContext::i()->SetRecivedInvite(true);
}

void UIPhaseBase::CbInviteMove(void * pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		if( g_pFramework->GetClanContext()->InBattleTeam() == false &&
			CCommunityContext::i()->StartMovingToUser() &&
			g_pFramework->GetUIMainframe() != nullptr )
		{
			UIPhaseBase * pPhase = g_pFramework->GetUIMainframe()->GetCurrentPhase();
			if( pPhase != nullptr && ! pPhase->OnInviteMove() )
			{
				I3TRACE("[ERROR] CPhaseBase::cbInviteMove() - Wrong position\n");
				CCommunityContext::i()->CompleteMovingToUser();
			}
		}
	}	

	CCommunityContext::i()->SetRecivedInvite(false);
}

void UIPhaseBase::ReceiveGameEvent_Change_Chara_Nick( INT32 arg, const i3::user_data&)
{
	UIPhaseProfile * phase = static_cast<UIPhaseProfile*>(GameUI::GetPopup(UPW_PROFILE));

	if( phase != nullptr)
	{
		UITabProfileInfo* pInfo = static_cast<UITabProfileInfo*>(phase->GetTab( UIPhaseProfile::TAB_INFO ));

		if( pInfo )
			pInfo->SetChangeCharaNick( arg );
	}
}

void UIPhaseBase::ReceiveGameEvent_Error( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseBase::ReceiveGameEvent_Disconnection( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseBase::ReceiveGameEvent_GameGuard_Error( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_GAMEGUARD_ERROR"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseBase::ReceiveGameEvent_HackToolDetected(INT32 arg, const i3::user_data&)
{
	I3PRINTLOG(I3LOG_NOTICE, "[UlsanGuard] Hacking tool detected. Close Game.");

	GameUI::MsgBox(MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_EP_GAME_EXIT_HACKUSER"), &GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"), 
		MAKE_CALLBACK(&usg_misc::HackToolDetectionCtx::OnMsgboxOKButtonClicked), this);
}

void UIPhaseBase::ReceiveGameEvent_Enter_Inventory( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		GetMainFrame()->TogglePopup( UPW_NULL, false);
		GetMainFrame()->ChangeSubPhase( USP_INVENTORY);
	}
	else
	{
		PlaySound( UISND_ERROR);
	}
}

void UIPhaseBase::ReceiveGameEvent_Leave_Inventory( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		// Do nothing
	}
	else
	{
		if (EV_IS_ERROR(arg, EVENT_ERROR_EVENT_EQUIP_USER_TITLE_NOT_ALLOWED))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_NO_EQUIP_PRE_DESIGNATION"));/*호칭을 획득하기전엔 장비할 수 없습니다.*/
		}
		else// if (EV_IS_ERROR(arg, EVENT_ERROR_SUCCESS))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_EQUIP_FAIL_BY_NETWORK_PROBLEM"));/*네트워크 문제로 장비할 수 없습니다.*/
		}

		PlaySound( UISND_ERROR);
	}

	GetMainFrame()->EnterDelayedEventSubPhase();
}

void UIPhaseBase::ReceiveGameEvent_Auth_Item( INT32 arg, const i3::user_data&)
{
	i3UI::setDisableUIInput(false);

	//	아이템 인증 (무기는 장비)
	GetMainFrame()->AuthStartSuccess(arg);
}

void UIPhaseBase::ReceiveGameEvent_Enter_Info( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->TogglePopup( UPW_NULL, false );
	GetMainFrame()->TogglePopup( UPW_PROFILE);
	i3UI::setDisableUIInput( false );
}

void UIPhaseBase::ReceiveGameEvent_Enter_Profile( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->TogglePopup( UPW_NULL, false);
	GetMainFrame()->ChangeSubPhase( USP_PROFILE);
}

void UIPhaseBase::ReceiveGameEvent_Leave_Profile(INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}


void UIPhaseBase::ReceiveGameEvent_Leave_Info( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}

void UIPhaseBase::ReceiveGameEvent_Enter_Shop( INT32 arg, const i3::user_data& UserData)
{
	GetMainFrame()->TogglePopup( UPW_NULL, false);
	GetMainFrame()->ChangeSubPhase( USP_SHOP);
}

void UIPhaseBase::ReceiveGameEvent_Leave_Shop( INT32 arg, const i3::user_data& UserData)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}

void UIPhaseBase::ReceiveGameEvent_Delete_NoteAsk( INT32 arg, const i3::user_data&)
{
	if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_RECEIVE_CLAN_NOTE"));/*클랜 전체 쪽지가 도착했습니다. 쪽지함이 가득차 있으면 오래된 쪽지를 삭제합니다.*/ 				
	}
}

// 새쪽지 알람
void UIPhaseBase::ReceiveGameEvent_Note( INT32, const i3::user_data&)
{
	GetMainFrame()->StartAlarmNote();

	// Login Event Popup
	if(MailBoxContext::i()->IsRecvLoginEvent())
	{
		i3::rc_wstring wstrTemp;
		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
		{
			wstrTemp = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS");

			GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrTemp);
		}
		MailBoxContext::i()->setRecvLoginEvent(false);
	}
}

void UIPhaseBase::ReceiveGameEvent_Insert_Block( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
	}
	else
	{
		switch(arg)
		{
		case EVENT_ERROR_EVENT_BLOCK_FULL :
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_UI_BLOCK_MAXIMUM"));
			break;
		default:
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_UI_BLOCK_FAIL"));
			break;
		}
	}
}

void UIPhaseBase::ReceiveGameEvent_Accept_Friend( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		// 친구 수락이 성공하면 친구 정보창을 닫아 친구의 상태를 새로 볼 수 있도록 한다.
		CCommunityContext::i()->SetCommunityUserInfo(false);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_COMPLETE_NORMAL"));/*정상적으로 처리되었습니다.*/
	}
	else
	{
		// 상황에 따른 알맞은 메시지 필요함. 현재 메시지는 너무 포괄적임.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND"));/*친구요청수락에 실패하였습니다.*/
	}
}

void UIPhaseBase::ReceiveGameEvent_Delete_Friend( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetLastDeleteFriend();

		i3::rc_wstring wstr_message;
		i3::sprintf(wstr_message, GAME_RCSTRING("STR_TBL_GUI_BASE_NOTICE_DELETE_FRIEND_COMPLETE"), node->_wstr_nick);/*[%s]님이 친구 목록에서 삭제되었습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_message);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_DELETE_FRIEND"));/*친구삭제에 실패하였습니다.*/
	}
}

void UIPhaseBase::ReceiveGameEvent_Invite_Clan( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_COMPLETE_NORMAL"));/*정상적으로 처리되었습니다.*/
	}
	else if( EV_IS_ERROR( arg, EVENT_ERROR_CLAN_MEMBER ) ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_ALREADY_JOINED"));/*이미 클랜에 가입한 유저입니다.*/
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_SEND_NOTE"));/*쪽지 보내기에 실패하였습니다.*/
	}
}

void UIPhaseBase::ReceiveGameEvent_Enter_Gacha( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->TogglePopup( UPW_NULL, false);
	GetMainFrame()->ChangeSubPhase( USP_GACHA);
}

void UIPhaseBase::ReceiveGameEvent_Leave_Gacha( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}

void UIPhaseBase::ReceiveGameEvent_Enter_Clan( INT32 arg, const i3::user_data & userData)
{
	if( EV_SUCCESSED( arg))
	{
		i3UI::setDisableUIInput( false );

		UIPOPUPWIN popup = ( arg == 1) ? UPW_CLAN_MEMBER : UPW_CLAN_NON_MEMBER;
		GetMainFrame()->OpenPopup(popup);

		g_pFramework->GetClanContext()->OnGameEvent( EVENT_CLAN_ENTER, arg, userData);
	}
}

void UIPhaseBase::ReceiveGameEvent_Leave_Clan( INT32 arg, const i3::user_data& userData)
{
	// 모든 클랜 메시지는 ClanContext에서 처리해야하나,
	// 이 경우 CStageReady에 접근할 수 없어, ClanContext 전달 전에 처리한다.
	// ++ 클랜채널에서 강제로 이동중일때는 처리하지 않습니다.
	if(!ClanGameContext::i()->IsForceLeaveChannel())
		GetMainFrame()->EnterDelayedEventPopup();

	// Clan Event
	g_pFramework->GetClanContext()->OnGameEvent( EVENT_CLAN_LEAVE, arg, userData);
}



void UIPhaseBase::ReceiveGameEvent_View_Other_User_Item_Info( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->ClosePopup(UPW_USERPROFILE);

	OtherUserInfo *		data = ViewOtherUserItemContext::i()->GetData();

	const i3::rc_wstring& UserName = data->m_wstrOtherUserName;
	i3::wstring NameList;
	const i3::vector<T_ItemID>& OtherUserItemList = data->m_OtherUserItemIdList;

	inner::GetItemNameList(NameList, OtherUserItemList);

	i3::rc_wstring wstrMsg;
	//%s님의 소지품 : %s
	i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_GUI_VIEW_OTHER_USER_ITEM_INFO_MSG"), UserName, NameList);
	
	GameUI::PrintSystemChat( wstrMsg);
}

void UIPhaseBase::ReceiveGameEvent_ShutDown_Time( INT32 arg, const i3::user_data&)
{
	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STR_SHUTDOWN_POPUP_ACCESSPROHIBITION"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseBase::ReceiveGameEvent_Enter_GiftShop( INT32 arg, const i3::user_data&)
{
	CShopContext::i()->BuildGoodsList();

	//GetCurrentSubPhase()->Leave();

	GetMainFrame()->TogglePopup( UPW_NULL, false);
	GetMainFrame()->ChangeSubPhase( USP_GIFTSHOP);
}



void UIPhaseBase::ReceiveGameEvent_Leave_GiftShop( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}

void UIPhaseBase::ReceiveGameEvent_Fail_Enter_GiftShop(INT32 arg, const i3::user_data& rankData)
{
	i3::rc_wstring message;
	i3::rc_wstring rankName;
	char rankStringName[128];

	UINT8 rankValue = i3::value_cast<INT8>(rankData);

	_snprintf(rankStringName, 256, "STBL_IDX_RANK_%d", rankValue);

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UISUBPHASE currentSubPhaseType = pFrame->GetCurrentSubPhaseType();
	if(currentSubPhaseType != USP_TEMP)
		pFrame->CloseAllPopup();

	UITopMenu* topMenu = pFrame->GetTopMenu();
	topMenu->SyncSubPhaseWithMenuBtnState();

	pFrame->SetLockInput(false);
	i3UI::setDisableUIInput(false);

	i3::sprintf(rankName, GAME_STRING(rankStringName));
	i3::sprintf(message, GAME_RCSTRING("STR_POPUP_GIFTSHOP_ADMISSION_FAIL"), rankName);
	GameUI::MsgBox(MSG_TYPE_GAME_OK, message);
}


void UIPhaseBase::ReceiveGameEvent_GM_User_Exit( INT32 arg, const i3::user_data& )
{
	GameUI::MsgBox( MSG_TYPE_SYSTEM_OK, GAME_RCSTRING("STBL_IDX_GM_GAME_KICK_LOGOUT"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
}

void UIPhaseBase::ReceiveGameEvent_Enter_Temp( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->ChangeSubPhase( USP_TEMP);
}
void UIPhaseBase::ReceiveGameEvent_Leave_Temp( INT32 arg, const i3::user_data&)
{
	GetMainFrame()->EnterDelayedEventSubPhase();
}

//제명기능 콜백 함수.
void cbSuccessDeprotationClan(void* pThis, INT32 nParam)
{
	UIMainFrame * pMainFrame = g_pFramework->GetUIMainframe();;
	I3ASSERT(pMainFrame != nullptr);
	
	if (pMainFrame == nullptr) return;

	//패킷처리에서 해주는데 여기서 한번더 하더라고요. 혹시 몰라 남겨두어요.
	USER_INFO_BASIC		MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	MyUserInfoBasic.m_ui32Clanidx   = 0;
	MyUserInfoBasic.m_ui32ClanState = CLAN_MEMBER_LEVEL_UNKNOWN;
	MyUserInfoBasic.m_ui32ClanMark  = CLAN_MARK_DEFAULT;
	i3mem::FillZero(MyUserInfoBasic.m_strClanName, sizeof(MyUserInfoBasic.m_strClanName));
	UserInfoContext::i()->SetMyUserInfoBasic(MyUserInfoBasic);

	CCommunityContext::i()->ClearClanMember();

	pMainFrame->setDeprotationClanChannel(false, L"");

	ClanGameContext::i()->setClanBattleLobbyEnable(false);
	
	if (g_pEnvSet->m_nUIVersion != UFV_1_5)
	{
		if (pMainFrame->GetCurrentSubPhaseType() == USP_TEMP)
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_TEMP);
	}
}

void UIPhaseBase::ReceiveGameEvent_Clan_Deprotation(INT32 arg, const i3::user_data &)
{
	if (ClanGameContext::i()->IsForceLeaveChannel()) 
		return;

	UIMainFrame * pMainFrame = g_pFramework->GetUIMainframe();
	if (pMainFrame == nullptr) 
		return;

	if (!(ChannelContext::i()->GetBattleChannel() && (pMainFrame->GetCurrentPhaseType() >= UIPHASE_LOBBY))) 
		return;

	CLAN_DETAIL_INFO * pClanInfo = ClanGameContext::i()->getMyClanDetailInfo();
	i3::rc_wstring wstr_clanname;
	i3::sprintf(wstr_clanname, GAME_RCSTRING("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION"), pClanInfo->m_name);
	i3mem::FillZero(pClanInfo, sizeof(CLAN_DETAIL_INFO));

	ClanGameContext::i()->setForceLeaveChannel(true);

	UIFloatMercenaryList*  pList = static_cast<UIFloatMercenaryList*>(GameUI::GetFloating(UFW_MERCENARY));
	pList->SetLeaveClanChannel(true);
	((UIPhaseClanLobby*)GameUI::GetPhase(UIPHASE_CLANLOBBY))->SetLeaveTeamFlag();

	pMainFrame->setDeprotationClanChannel(true, wstr_clanname);
	
	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		pMainFrame->ChangeClanPopup();

		if (g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_CLAN_MEMBER) == true)		// 제명 되었음에도 불구하고 클랜멤버용 창이 떠있다면 Close처리
		{
			g_pFramework->GetUIMainframe()->ClosePopup(UPW_CLAN_MEMBER);
			g_pFramework->CloseGameMessageBox();
		}
		else if (g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_PROFILE) == true)		
		{
			g_pFramework->GetUIMainframe()->ClosePopup(UPW_PROFILE);
			g_pFramework->CloseGameMessageBox();
		}
	}

	//서브 페이지가 꺼져 있을경우에만 메시지 박스 띄우고 강제 서버/채널 선택 페이지 이동. (팝업은 제외 콜백함수에서 꺼줌)
	//UI_V10, UI_V11 의 클랜 페이지는 USP_TEMP 이므로 해당 페이지는 바로 꺼줘야한다!
	if (pMainFrame->GetCurrentSubPhaseType() == USP_NONE || pMainFrame->GetCurrentSubPhaseType() == USP_TEMP)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, wstr_clanname, nullptr, MAKE_CALLBACK(cbSuccessDeprotationClan), this); // 클랜 제명
		return;
	}
}
