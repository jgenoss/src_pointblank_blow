#include "pch.h"
#include "PhaseBase.h"
#include "GlobalVariables.h"

#include "PhaseChannelList.h"
#include "PhaseLobby.h"
#include "PhaseReadyRoom.h"
#include "CommunityContext.h"
#include "UserLocationInfo.h"


I3_ABSTRACT_CLASS_INSTANCE(CPhaseBase, CGuiNotifyReceiver);

CPhaseBase::CPhaseBase()
{
	m_pChatBody = NULL;
}

CPhaseBase::~CPhaseBase()
{
	// null

}

void CPhaseBase::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();

}

void CPhaseBase::OnGameEvent(INT32 event,INT32 arg)
{
	switch(event)
	{
	case EVENT_M_DRIVE_AWAY:
		CPhaseBase::DriveAway(arg);
		break;
	case EVENT_M_ERROR:
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case EVENT_M_DISCONNECTION:
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_EXIT_ASSERT_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case EVENT_M_GAMEGUARD_ERROR:
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAMEGUARD_ERROR"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	}
}

void CPhaseBase::OnEntranceStart(void)
{	
	CGuiNotifyReceiver::SlideIn();
	PlaySound(GUI_SND_SCROLLIN);
}

BOOL CPhaseBase::OnEntrancing(RT_REAL32 rDeltaSeconds)
{
	OnSliding(rDeltaSeconds);

	if (SS_IN == CPhaseBase::GetSlideState())
	{
		return TRUE;
	}
	
	return FALSE;
}

void CPhaseBase::OnEntranceEnd(void)
{	
	// Do nothing
}

void CPhaseBase::OnExitStart(void)
{
	CGuiNotifyReceiver::SlideOut();

	PlaySound(GUI_SND_SCROLLOUT);
}

BOOL CPhaseBase::OnExiting(RT_REAL32 rDeltaSeconds)
{
	OnSliding(rDeltaSeconds);

	if (SS_OUT == CPhaseBase::GetSlideState())
	{
		return TRUE;
	}

	return FALSE;
}

void CPhaseBase::OnExitEnd(void)
{
}

void CPhaseBase::OnSlideOut(BOOL bRight)
{
	CGuiNotifyReceiver::SlideOut();

	PlaySound(GUI_SND_SCROLLOUT);
}

void CPhaseBase::OnSlideIn(BOOL bRight)
{
	CGuiNotifyReceiver::SlideIn();

	PlaySound(GUI_SND_SCROLLIN);
}

BOOL CPhaseBase::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				OnExitKey();
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CPhaseBase::OnChangeVideo( i3VideoInfo * pInfo)
{
	/* Do nothing */
}

void CPhaseBase::DriveAway(INT32 nReason)
{
	switch(nReason)
	{
	case ACCOUNT_KICK_LOGIN:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_LOGIN_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case ACCOUNT_KICK_GM_TOOL:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_TOOL_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case ACCOUNT_KICK_OTHER:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	default:
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DRIVE_AWAY_OTHER_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	}

	g_pGameContext->SetEventOnce(EVENT_DISCONNECT);
}

void CPhaseBase::OnExitKey(void)
{
	CGuiNotifyReceiver::OnExitKey();
}

void CPhaseBase::OnInvitedMessage(void)
{
	const FRIEND_NODE* node = g_pCommunity->GetInviteViewFriend();

	if (! node)
	{
		g_pCommunity->ResetInviteFriend();
		return;
	}

	BOOL bPass = FALSE;

	// 옵션에 따라 초대하기 창을 활성/비활성 상태를 결정한다.
	switch( g_pEnvSet->m_nInviteType)
	{
	case COMMUNITY_PERMISSION_ALL		: bPass = TRUE;		break;		// 전체 허용	
	case COMMUNITY_PERMISSION_NO		: bPass = FALSE;		break;		// 전체 차단	
	case COMMUNITY_PERMISSION_FRIEND	: bPass = FALSE;		break;		// 친구/클랜 허용	
	default:bPass = TRUE;		break;			
	}

	// 친구/클랜 허용상태라면, 조건을 검색합니다.
	if( g_pEnvSet->m_nInviteType == COMMUNITY_PERMISSION_FRIEND)
	{
		INT32 i = 0;
		INT32 nFriendCount = g_pCommunity->GetFriendCount();
		for( i=0; i<nFriendCount; i++)
		{
			const FRIEND_NODE* pNode = g_pCommunity->GetFriend(i);
			if( pNode->_uid == node->_uid)
			{
				bPass = TRUE;
				break;
			}
		}

		if( !bPass)
		{
			INT32 nClanMemberCount = g_pCommunity->GetClanMemberCount();
			for( i=0; i<nClanMemberCount; i++)
			{
				const CLAN_MEMBER_NODE* pNode = g_pCommunity->GetClanMember(i);
				if( pNode->_uid == node->_uid)
				{
					bPass = TRUE;
					break;
				}
			}
		}
	}
	
	if( bPass)
	{
		PopupInviteMessage(node->_state, node->_nick, CbInviteFriend);
	}
	else
	{
		g_pCommunity->ResetInviteFriend();
	}

}

void CPhaseBase::CbInviteFriend(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
		{
			if (g_pCommunity->StartInviteMove())
			{
				CStageReady * pStage = (CStageReady*)g_pFramework->GetCurrentStage();
				CPhaseBase * pPhase = pStage->GetCurrentPhase();

				if ( ! pPhase->OnInviteMove() )
				{
					I3TRACE("[ERROR] CPhaseBase::CbInviteFriend() - Wrong position\n");
					g_pCommunity->ComplateInviteMove();
					
				}
			}			
		}
	}

	g_pCommunity->ResetInviteFriend();
}

void CPhaseBase::OnInvitedMessageClan(void)
{
	const CLAN_MEMBER_NODE* node = g_pCommunity->GetInviteViewClan();

	if ( ! node )
	{
		g_pCommunity->ResetInviteFriend();
		return;
	}


	BOOL bPass = FALSE;

	// 옵션에 따라 초대하기 창을 활성/비활성 상태를 결정한다.
	switch( g_pEnvSet->m_nInviteType)
	{
	case COMMUNITY_PERMISSION_ALL		: bPass = TRUE;			break;		// 전체 허용	
	case COMMUNITY_PERMISSION_NO		: bPass = FALSE;		break;		// 전체 차단	
	case COMMUNITY_PERMISSION_FRIEND	: bPass = FALSE;		break;		// 친구/클랜 허용	
	default:bPass = TRUE;		break;			
	}

	// 친구/클랜 허용상태라면, 조건을 검색합니다.
	if( g_pEnvSet->m_nInviteType == COMMUNITY_PERMISSION_FRIEND)
	{
		INT32 i = 0;
		INT32 nFriendCount = g_pCommunity->GetFriendCount();
		for( i=0; i<nFriendCount; i++)
		{
			const FRIEND_NODE* pNode = g_pCommunity->GetFriend(i);
			if( pNode->_uid == node->_uid)
			{
				bPass = TRUE;
				break;
			}
		}

		if( !bPass)
		{
			INT32 nClanMemberCount = g_pCommunity->GetClanMemberCount();
			for( i=0; i<nClanMemberCount; i++)
			{
				const CLAN_MEMBER_NODE* pNode = g_pCommunity->GetClanMember(i);
				if( pNode->_uid == node->_uid)
				{
					bPass = TRUE;
					break;
				}
			}
		}
	}

	if( bPass)
	{
		CUserLocationInfo	UserLocInfo(node->_state);
		PopupInviteMessage(node->_state, node->_nick, CbInviteClan);
	}
	else
	{
		g_pCommunity->ResetInviteFriend();
	}

}

void CPhaseBase::CbInviteClan(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
		{
			if (g_pCommunity->StartInviteMoveClan())
			{
				CStageReady * pStage = (CStageReady*)g_pFramework->GetCurrentStage();
				CPhaseBase * pPhase = pStage->GetCurrentPhase();

				if ( ! pPhase->OnInviteMove() )
				{
					I3TRACE("[ERROR] CPhaseBase::CbInviteClan() - Wrong position\n");
					g_pCommunity->ComplateInviteMove();
					
				}
			}
		}
	}

	g_pCommunity->ResetInviteFriend();
}

void CPhaseBase::PopupInviteMessage( const CUserLocationInfo & UserLocInfo, const char * Nickname, InvitePopupCallback pCallBackFunc)
{
	char message[MAX_STRING_COUNT];
	
	char szServer[64] = "";
	
	UINT32 server = UserLocInfo.GetDisplayServerChannelNumber();
	UINT32 MatchTeamIndex = UserLocInfo.GetMatch();

	i3String::Format( szServer, 64, "%03d", server);

	if( MatchTeamIndex > 0 )
	{		
		char szMatchName[ 64 ] = "";
		GetBattleTeamName( szMatchName, 64, MatchTeamIndex-1, FALSE );
		
		// 수락 여부 확인		
		const char * str = GAME_STRING("STR_TBL_GUI_INVITE_MESSAGEBOX_TO_TEAM");		/*%03d번 서버의 %s 팀의 [%s]님으로부터 초대요청 메시지가 도착하였습니다. \n수락하시겠습니까?*/
		std::string fmt =  BOOST_FORMAT3( str, szServer, szMatchName, Nickname);
		i3String::Copy( message, fmt.c_str(), MAX_STRING_COUNT );
	}
	else
	{
		UINT32 room = UserLocInfo.GetDisplayRoomNumber();

		// 수락 여부 확인
		const char * str = GAME_STRING("STR_TBL_GUI_INVITE_MESSAGEBOX_TO_ROOM");		/*%03d번 서버의 %d번 방의 [%s]님으로부터 초대요청 메시지가 도착하였습니다. \n수락하시겠습니까?*/
		std::string fmt =  BOOST_FORMAT3( str, szServer, room, Nickname);
		i3String::Copy( message, fmt.c_str(), MAX_STRING_COUNT );
	}

	STRING_POPUP_Q(message, MAKE_CALLBACK(pCallBackFunc), this);
}