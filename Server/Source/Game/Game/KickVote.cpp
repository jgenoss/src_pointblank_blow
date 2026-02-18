#include "pch.h"
#include "Room.h"
#include "RoomManager.h"
#include "ModuleContextNc.h"

BOOL CRoom::ChkBattleUser3Over()
{
	INT32 i32User = 0;
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if ( SLOT_STATE_BATTLE == m_Info._aSlotInfo[ i ]._State )
		{
			i32User++;

			switch( m_i8SuggestLocaleCode )
			{
			case LOCALE_CODE_INDONESIA:
			case LOCALE_CODE_ITALY:
				{
					if ( i32User == 5 ) return TRUE;
				}
				break;
			default:
				{
					if ( i32User == 3 ) return TRUE;
				}
				break;
			}
		}
	}
	return FALSE;
}

// 유권자유저의 투표 && 마지막 유권자 유저의 투표 후 강제탈퇴 표 집계 2011.05.25 안국정
void CRoom::GetVote( INT32 i32Slot, UINT8 ui8Vote )
{
	INT32	i32Result;

	// 현재 투표 중인지 검사
	if ( !m_bVoting )
	{
		// 투표중이 아닌데 투표 패킷을 받았음.
		// 투표가 끝나기 직전의 찰나에 투표를 하거나 해킹 패킷일 가능성.
		i32Result = EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE;

		i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK );
		
		SendPacket.WriteData( &i32Result, sizeof(INT32) );
		m_Info._pSession[i32Slot]->SendPacketMessage( &SendPacket );
	}
	// 투표한 유저의 슬롯에 해당하는 투표상태값이 1인경우(투표권 있는 경우)
	else if ( 1 == m_aui8VoteRight[i32Slot] )
	{
		if ( ui8Vote == 1 )
		{
			m_aui8VoteData[i32Slot] = KICKVOTE_THROW_DISAGREE;
		}
		else
		{
			m_aui8VoteData[i32Slot] = KICKVOTE_THROW_AGREE;

			// 찬성표를 던진 유저의 팀과 피신고자의 팀이 같은 경우는 마킹
			if ( i32Slot % TEAM_COUNT == m_i32TargetTeam )
			{
				m_i8GoodFromAlly++;
			}
			else
			{
				m_i8GoodFromEnemy++;
			}
		}
		// 투표를 했으므로 투표권을 없앤다.(해당 슬롯번호의 비트플래그를 0으로 셋팅합니다.
		m_aui8VoteRight[i32Slot] = 0;
		
		i32Result = EVENT_ERROR_SUCCESS;

		UINT8	ui8Good, ui8Bad;

		ui8Bad	= GetVoteState( &ui8Good );

		i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK );

		SendPacket.WriteData( &ui8Good, sizeof(UINT8) );
		SendPacket.WriteData( &ui8Bad, sizeof(UINT8) );

		INT32	i;
		// 변경된 현황을 전 클라이언트에게 통보
		for( i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			// 피신고인 제외 (피신고인 모르게 진행되야 하므로)
			// 투표인 제외 (투표인은 집계결과를 알려주는 PROTOCOL_BATTLE_NOTIFY_CURRENT_KICKVOTE_ACK 대신
			// 표 반영여부를 알려주는 PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK 로 대신할수 있기 때문)
			if ( i != i32Slot && i != m_i32TargetSlot && m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
			{
				m_Info._pSession[i]->SendPacketMessage( &SendPacket );
			}
			// 투표인에게는 { PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK, EVENT_ERROR_SUCCESS } 패킷을 보내
			// 투표성공을 알립니다. 클라이언트에서는 직전의 투표 현황(예: 찬성3 반대4)에서 
			// 자신의 표(예:찬성 1표)를 직접 더해 현황을 출력합니다.(예: 찬성4 반대4)
			else if ( i == i32Slot )
			{
				i3NetworkPacket	SendPacket2( PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK );
				
				SendPacket2.WriteData( &i32Result, sizeof(INT32) );
				m_Info._pSession[i]->SendPacketMessage( &SendPacket2 );
			}
		}
	}
	else
	{
		// 투표권 없는 경우(투표를 했거나, 난입자)인데 투표표가 왔다. 해킹 의심
		i32Result = EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE;

		i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK );
		
		SendPacket.WriteData( &i32Result, sizeof(INT32) );
		m_Info._pSession[i32Slot]->SendPacketMessage( &SendPacket );
	}


	// 마지막 투표자가 투표해 -> 표 집계 완료 후 가결 결정 ( 강퇴 결정 )
	// 마지막 투표자는 다른 투표자와는 달리 PROTOCOL_BATTLE_VOTE_KICKVOTE_ACK 대신
	// PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK 로 통보받습니다.
}

UINT8 CRoom::GetVoteState( UINT8* pGood )
{
	UINT8	ui8Agree	= 1;	// 투표 제안자의 자동 찬성표
	UINT8	ui8Disagree	= 1;	// 투표 당하는 유저의 자동 반대표

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		switch( m_aui8VoteData[i] )
		{
		case KICKVOTE_THROW_AGREE:		ui8Agree++;		break;
		case KICKVOTE_THROW_DISAGREE:	ui8Disagree++;	break;
		}
	}

	*pGood	= ui8Agree;
	
	return ui8Disagree;
}

// 인자설명:
// 1. 투표제안을 한 유저 세션 클래스 포인터
// 2. 강제탈퇴시키고 싶은 유저의 슬롯 번호
// 3. 강제탈퇴 시키는 사유
INT32 CRoom::DecideVote( CUserSession* pUser, UINT8 ui8TargetSlot, UINT8 ui8KickReason )
{
	CUserSession* pTargetUser = this->GetUserSession(ui8TargetSlot);

	m_i8SuggestLocaleCode	= pUser->m_ui8LocaleCode;
	// 아래경우일때 투표를 실행하지 못합니다.
	if ( pTargetUser == NULL )																		return EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME;
	if ( m_bVoting )																				return EVENT_ERROR_KICKVOTE_ALREADY_EXIST_OTHER_VOTE;
	if ( FALSE == ChkBattleUser3Over() )															return EVENT_ERROR_KICKVOTE_SMALL_USER_IN_ROOM;
	if ( SLOT_STATE_BATTLE != m_Info._aSlotInfo[ui8TargetSlot]._State )								return EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME;
	if ( pUser->m_ui32ReVoteTime > GetRoomBattleTime() )											return EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_ONE_MINUTE;
	if ( FALSE == m_pRoomMode->CheckVoteTime( GetRoomBattleTime() ) )								return EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME;
	if ( pUser->m_UserInfoServer.m_ui8VoteCount >= g_pContextMain->m_vlVoteLimit.m_ui8MaxVoteCount)	return EVENT_ERROR_KICKVOTE_MAX_VOTE_COUNT_OVER;
	if ( pUser->m_UserInfoBasic.m_ui32Rank < g_pContextMain->m_vlVoteLimit.m_ui8MinRank)			return EVENT_ERROR_KICKVOTE_UNDER_RANK_LIMIT;
	if( pTargetUser->m_ui32GuardTime > GetRoomBattleTime() )										return EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_GUARD_TIME;

		// 투표 시작
	m_bVoting			= TRUE;
	pUser->m_ui32ReVoteTime = GetRoomBattleTime() + VOTE_RE_SUGGEST_TIME;

	pTargetUser->m_ui32GuardTime = GetRoomBattleTime() + g_pContextMain->m_vlVoteLimit.m_ui32ReportGuardTime * 60;

	pUser->m_UserInfoServer.m_ui8VoteCount++;	// 유저가 투표제한이 성공했음으로 신고 제한 카운터수를 증가시킵니다.

	m_i8GoodFromAlly	= 0;			// 피신고인의 아군으로부터의 찬성표수
	m_i8GoodFromEnemy	= 0;			// 피신고인의 적군으로부터의 찬성표수

	m_i32CallSlot		= pUser->m_i32SlotIdx;
	m_i32TargetSlot		= ui8TargetSlot;
	m_i32CallTeam		= m_i32CallSlot % TEAM_COUNT;
	m_i32TargetTeam		= m_i32TargetSlot % TEAM_COUNT;

	m_ui8Reason			= ui8KickReason;

	m_i64TargetUID		= pTargetUser->m_i64UID;

	// 신고자와 피신고자가 같은편인지 다른편인지 구분합니다.
	if ( m_i32CallTeam == m_i32TargetTeam )	m_i8GoodFromAlly	= 1;
	else									m_i8GoodFromEnemy	= 1;

	// 반수를 구하기 위해 투표 인원 집계를 시작합니다.
	UINT8	ui8VoterCount	 = 2;		// 신고인과 피신고인 2명은 집계되지 않으므로 미리 2로 설정합니다.

	// 투표권이 있는 슬롯의 비트에만 1표시를 합니다.
	i3mem::FillZero( m_aui8VoteData, sizeof( m_aui8VoteData ) );
	i3mem::FillZero( m_aui8VoteRight, sizeof( m_aui8VoteRight ) );

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if ( i == m_i32CallSlot )	continue;
		if ( i == m_i32TargetSlot )	continue;

		if ( m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			m_aui8VoteRight[i] = 1;
			ui8VoterCount++;
		}
	}

	// '투표인원'으로 '과반수'를 미리 계산해놓습니다. 과반수 공식 수정 2011.06.14
	BOOL bClan = FALSE;
	switch( g_pContextMain->m_pChannelType[m_Info._ui32ChannelNum])
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 
		{
			bClan = TRUE;
		}
		break;
	}

	if( IsMatchMaking() ) 
	{
		bClan = TRUE;
	}

	if ( bClan )
	{
		m_ui8HalfCount = ui8VoterCount - 1;		// 클랜전이면 강퇴성립하기 위해서는 피신고자를 제외한 모든 인원이 찬성을 해야합니다.
	}
	else
	{
		switch( m_i8SuggestLocaleCode )
		{
		case LOCALE_CODE_INDONESIA:
		case LOCALE_CODE_ITALY:
			{
				m_ui8HalfCount	= (INT8)((REAL32)ui8VoterCount * 0.6f) + 1;
			}
			break;
		default:
			{
				m_ui8HalfCount	= ( ui8VoterCount / 2 ) + 1;
			}
			break;
		}
	}

	// 투표 시작 시간을 기록합니다.
	m_ui32RTVoteBegin		= GetRoomBattleTime();

	// 신고인에게는 제기한 투표가 성사되었다는 알림 패킷을 보냅니다.
	INT32	i32Reason	= EVENT_ERROR_SUCCESS;
	i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK );		
	SendPacket.WriteData( &i32Reason, sizeof(INT32) );
	m_Info._pSession[m_i32CallSlot]->SendPacketMessage( &SendPacket );

	UINT8 ui8CallSlot = (UINT8)m_i32CallSlot;
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		// 신고인과 피신고인을 제외한 게임 중인 유저에게 투표 개시 알림 패킷을 보냅니다.
		if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State )	continue;
		if( i == m_i32CallSlot )								continue;
		if( i == ui8TargetSlot )								continue;

		i3NetworkPacket	SendPacket2( PROTOCOL_BATTLE_START_KICKVOTE_ACK );
		SendPacket2.WriteData( &ui8CallSlot,		sizeof(UINT8) );
		SendPacket2.WriteData( &ui8TargetSlot,		sizeof(UINT8) );
		SendPacket2.WriteData( &m_ui8Reason,		sizeof(UINT8) );

		m_Info._pSession[i]->SendPacketMessage( &SendPacket2 );
	}

	pUser->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
	pTargetUser->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
	return EVENT_ERROR_SUCCESS;
}

// 투표시간경과 또는 마지막유권자유저의 Room 나감으로 인한 강제탈퇴 집계 2011.05.25 안국정
void CRoom::CloseVote()
{
	UINT8	ui8Good;
	UINT8	ui8Bad;
	UINT8	ui8TargetSlot;
	INT32	i32Result;

	// 가결이든 부결이든 투표는 종료
	m_bVoting	= FALSE;

	ui8Bad	= GetVoteState( &ui8Good );

	// 과반수 조건 ( 반수보다 찬성표가 1표라도 더 많은 경우 과반수 성립 )
	if ( m_ui8HalfCount <= ui8Good )	
	{
		INT8 i8AllyAtLeast;
		switch( m_i8SuggestLocaleCode )
		{
		case LOCALE_CODE_INDONESIA:
			{
				i8AllyAtLeast = 2;
			}
			break;
		default:
			{
				i8AllyAtLeast = 1;
			}
			break;
		}
		// 양팀 모두 최소 한명 이상의 찬성자가 나와야 함
		if ( m_i8GoodFromAlly >= i8AllyAtLeast && m_i8GoodFromEnemy >= 1 )
		{
			// 가결 ( 강제퇴장 결정 )
			i32Result = EVENT_ERROR_SUCCESS;
		}
		else if ( 0 == m_i8GoodFromAlly )
		{
			// 부결 ( 강제퇴장 안됨 ) : 아군 중에 찬성표가 1표도 없습니다.
			i32Result = EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ALLY;
		}
		else
		{
			// 부결 ( 강제퇴장 안됨 ) : 적군 중에 찬성표가 1표도 없습니다.
			i32Result = EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ENEMY;
		}
	}
	// 부결 ( 강제퇴장 안됨 ) : 찬성표가 과반수에 미치지 못했습니다.
	else
	{
		i32Result = EVENT_ERROR_KICKVOTE_REJECTED_SHORTAGES;
	}

	i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_KICKVOTE_RESULT_ACK );

	ui8TargetSlot = (UINT8)m_i32TargetSlot;

	SendPacket.WriteData( &ui8TargetSlot,	sizeof(UINT8) );
	SendPacket.WriteData( &ui8Good,			sizeof(UINT8) );
	SendPacket.WriteData( &ui8Bad,			sizeof(UINT8) );
	SendPacket.WriteData( &i32Result,		sizeof(INT32) );

	INT32	i;
	for( i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		// 피신고인 제외 ( 피신고인 모르게 진행되야 하므로 )
		if ( i != ui8TargetSlot && m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			m_Info._pSession[i]->SendPacketMessage( &SendPacket );
		}
	}

	// 실제로 유저를 쫓아냅니다. 
	if ( i32Result == EVENT_ERROR_SUCCESS )
	{
		// 강제퇴장 당한 유저 Insert
		m_aKickUserList[ m_i8KickUserInsertPoint ] = m_i64TargetUID;
		m_i8KickUserInsertPoint++;
		if( m_i8KickUserInsertPoint >= KICK_LIST_IN_ROOM_MAX ) m_i8KickUserInsertPoint = 0;

		// 1. '투표결과 너는 강퇴다' 패킷을 보내고,
		i3NetworkPacket SendPacket2( PROTOCOL_BATTLE_NOTIFY_BE_KICKED_BY_KICKVOTE_ACK );

		CUserSession* pKickedUser = m_Info._pSession[ui8TargetSlot];
		pKickedUser->SendPacketMessage( &SendPacket2 );

		// 2. 투표결과를 로그로 남기고,
		//pKickedUser->m_strId		// 강퇴당한 유저의 ID
		//pKickedUser->m_i64UID		// 강퇴당한 유저의 UID
		//m_pKickVote->GetReason()	// 투표 제기 사유번호 ( CommonDef.h 의 enum FORCEREMOVE_CAUSE 값 )
		//ui8Good					// 찬성표수
		//ui8Bad					// 반대표수

		// 로그 기록 - 서동권
		LOG_KICK	stLog;
		stLog.m_eType		= USER_KICK_TYPE_USER_VOTE;
		stLog.m_eReason		= (FORCEREMOVE_CAUSE)m_ui8Reason;
		stLog.m_TCallUID	= m_Info._pSession[m_i32CallSlot]->GetUID();
		stLog.m_ui8Good		= ui8Good;
		stLog.m_ui8Bad		= ui8Bad;
		g_pModuleContextNC->LogSendMessage(MAIN_THREAD_IDX, LOG_MESSAGE_U_R_KICK_LEAVE_ROOM, pKickedUser, &stLog);

		// 3. 실제로 로비로 내쫓습니다.
		g_pRoomManager->OnLeaveRoom( pKickedUser, pKickedUser->m_i32ChannelNum, MAIN_THREAD_IDX, ROOM_ENDTYPE_VOTE );
	}
}