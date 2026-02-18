#include "pch.h"
#include "MatchTeam.h"

#include "Gateway.h"
#include "DataBase.h"
#include "TaskProcessor.h"
#include "Config.h"

#include "TeamMember.h"
#include "MatchingManager.h"
#include "../../CommonServerSource/TrueSkill/TrueSkill.h"


//////////////////////////////////////////////////////////////////////////
// CMatchTeam
CMatchTeam::CMatchTeam()
: m_TTeamID(MATCH_TEAM_INVALID_IDX)
, m_ui8MaxMember(0)
, m_pOwnerClan(NULL)
{
}

CMatchTeam::~CMatchTeam()
{
}

void CMatchTeam::CreateTeam(UINT8 ui8MaxMember, CLAN_DETAIL_INFO_SERVER* pOwnerClan, CLAN_MEMBER_DETAIL_INFO* pClanMember, UINT32 ui32ServerIdx)
{
	m_eTeamState = TEAM_STATE_WAIT;
	m_ui8MaxMember = ui8MaxMember;
	m_pOwnerClan = pOwnerClan;
	m_i8LeaderSlot = 0;
	i3mem::FillZero( m_aTSlotUID, sizeof( T_UID ) * MAX_CLAN_MATCH_MEMBER );
	m_aTSlotUID[0] = pClanMember->_i64UID;
	m_strOperation[0] = '\0';
	m_i8ClanTeamIdx = -1;
	m_ui32MatchRequestTime = 0;
	m_TMatchedTeamID = MATCH_TEAM_INVALID_IDX;
	m_ui32ServerIdx = ui32ServerIdx;
}

void CMatchTeam::DestroyTeam(UINT32 ui32ClanDBIdx )
{
	INT32 i32CurMember = m_pMembers.GetCount();

	for( INT32 i = i32CurMember-1; i >= 0 ; --i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pTeamMember ) continue;

		pTeamMember->LeaveTeam(m_pOwnerClan);
		m_pMembers.Remove(pTeamMember);
		I3_SAFE_DELETE(pTeamMember);
	}

	m_eTeamState = TEAM_STATE_NONE;

	ReleaseClanMatchTeam(ui32ClanDBIdx);

	return;
}

CTeamMember* CMatchTeam::FindMember(INT64 i64UserID)
{
	INT32 i32CurMember = m_pMembers.GetCount();

	for( INT32 i = 0; i < i32CurMember; ++i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pTeamMember )
			continue;

		if( pTeamMember->GetUserID() == i64UserID )
			return pTeamMember;
	}

	return NULL;
}

void CMatchTeam::CalculateRatings(ROOM_ENDTYPE eResult, REAL64 r64C, REAL64 r64V, REAL64 r64W)
{
	if( !m_pOwnerClan )
	{
		g_pLog->WriteLog( L"[ERROR][CMatchTeam::CalculateRatings] Not found clan.");

		return;
	}

	CTrueSkill::CalculateRating(m_pOwnerClan->_ClanDetailInfo.GetClanRating(), eResult, r64C, r64V, r64W);

	//g_pTaskProcessor->m_RankingManager.UpdateRanking(m_pOwnerClan, bRaiseRating);

}

void CMatchTeam::ApplyMatchResult(CLAN_WAR_RESULT* pClanWarResult, T_UID* pPlayerUID)
{
	UINT32 ui32ClanID = m_pOwnerClan->_ClanDetailInfo.m_ui32CID;

	// 클랜 매치메이킹 플레이어 저장.
	i3mem::Copy(m_pOwnerClan->m_aClanWarPlayerUID, pPlayerUID, sizeof(T_UID) * CLAN_TEAM_MEMBER_COUNT);

	// 시즌 전적 UPDATE
	CLAN_RECORD* pSeasonRecord = &m_pOwnerClan->_ClanDetailInfo.m_SeasonRecord[SEASON_TYPE_ONGOING];

	++pSeasonRecord->m_i32MatchCount;

	switch (pClanWarResult->m_Result)
	{
	case ROOM_ENDTYPE_WIN:		++pSeasonRecord->m_i32WinMatch; break;
	case ROOM_ENDTYPE_LOSE:	++pSeasonRecord->m_i32LoseMatch; break;
	default:	break;
	}

	RING_IN_DB stRingInDB;
	stRingInDB._ui8Type = RING_DB_UPDATE_CLAN_MATCH_RECORD;
	stRingInDB._ui32ClanDBIdx = ui32ClanID;
	stRingInDB.Write(pSeasonRecord, sizeof(CLAN_RECORD));
	stRingInDB.Write(pClanWarResult, sizeof(CLAN_WAR_RESULT));
	stRingInDB.Write(pPlayerUID, sizeof(T_UID) * CLAN_TEAM_MEMBER_COUNT);

	if (g_pDataBase->InsertWorking(stRingInDB._ui32ClanDBIdx, &stRingInDB) != RINGBUFFER_SUCCESS)
	{
		g_pLog->WriteLog(L"[ERROR][CMatching::ApplyMatchResult] Fail DB Insert TeamID : %d / ClanIdx : %d", m_TTeamID, ui32ClanID);
	}

	// 매치메이킹 결과를 클랜원들에게 전달합니다.
	g_pTaskProcessor->SendClanWarResult(m_pOwnerClan, pClanWarResult);
}

void CMatchTeam::MakeMatchingRequst(MATCHING_REQUEST& request )
{
	if( !m_pOwnerClan )
	{
		g_pLog->WriteLog( L"[ERROR][CMatching::MakeMatchingRequst] Not found clan / TeamId : %d / State : %d",  m_TMatchedTeamID, m_eTeamState);
		return;
	}

	request.m_TTeamID			= m_TTeamID;
	request.m_ui32ClanID		= m_pOwnerClan->_ClanDetailInfo.m_ui32CID;
	request.m_ui8MaxMember		= m_ui8MaxMember;
	request.m_TMatchedTeamID	= m_TMatchedTeamID;
	request.m_i32MatchingPoint	= CalculateMatchingPoint();
}

void CMatchTeam::MakePacketTeamInfo(i3NetworkPacket* pPacket)
{
	MATCH_TEAM_INFO TeamInfo;
	TeamInfo.m_TTeamID			= m_TTeamID;
	TeamInfo.m_i8ClanTeamIdx	= m_i8ClanTeamIdx;
	TeamInfo.m_ui8TeamState		= (UINT8)m_eTeamState;
	TeamInfo.m_ui8MaxPer		= m_ui8MaxMember;
	TeamInfo.m_ui8CurPer		= (UINT8)m_pMembers.GetCount();
	
	pPacket->WriteData(&TeamInfo,		sizeof(MATCH_TEAM_INFO));
}

void CMatchTeam::MakePacketTeamDetailInfo(i3NetworkPacket* pPacket)
{
	MakePacketTeamInfo(pPacket);

	UINT8 ui8OperationSize = (UINT8)i3String::Length( m_strOperation );

	pPacket->WriteData(&m_i8LeaderSlot,			sizeof(UINT8)); 
	pPacket->WriteData(&ui8OperationSize,		sizeof(UINT8)); 
	pPacket->WriteData(m_strOperation,			sizeof( TTCHAR ) * ui8OperationSize);
}

void CMatchTeam::MakePacketMemberInfo(i3NetworkPacket* pPacket )
{
	MATCH_MEMBER_INFO MemberInfo;
	i3mem::FillZero(&MemberInfo,sizeof(MATCH_MEMBER_INFO));

	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if (!pTeamMember)
		{	// 패킷 밀림 현상 방지
			pPacket->WriteData(&MemberInfo, sizeof(MATCH_MEMBER_INFO));
			continue;
		}

		pTeamMember->MakePacketMemeber(pPacket);
	}
}

void CMatchTeam::MakePacketJoinRoom(i3NetworkPacket* pPacket)
{
	T_UID	TUID = -1;
	INT32	i32SessionIdx = -1;
	UINT32	ui32Count = 0;	// 별의미없음.

	UINT8 ui8CurMember = (UINT8)m_pMembers.GetCount();
	pPacket->WriteData(&ui8CurMember, sizeof(UINT8));

	for( UINT8 i = 0; i < ui8CurMember; ++i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pTeamMember )
		{	// 패킷 밀림 현상 방지
			pPacket->WriteData(&TUID,			sizeof(T_UID));
			pPacket->WriteData(&i32SessionIdx,	sizeof(INT32));
			pPacket->WriteData(&ui32Count,		sizeof(UINT32));
			pPacket->WriteData(&ui32Count,		sizeof(UINT32));
			pPacket->WriteData(&ui32Count,		sizeof(UINT32));
			pPacket->WriteData(&ui32Count,		sizeof(UINT32));
			continue;
		}

		//pTeamMember->MakePacketMemberUID(pPacket, i32LogIdx); 
		pTeamMember->MakePacketMatchMakingRecord(pPacket);
	}
}

void CMatchTeam::MakePacketClanMatchingInfo(i3NetworkPacket* pPacket)
{
	CLAN_MATCHING_TEAM_INFO	stInfo;
	i3mem::FillZero( &stInfo, sizeof(CLAN_MATCHING_TEAM_INFO) );
	
	if( m_pOwnerClan ) 
	{
		stInfo._color	= m_pOwnerClan->_ClanDetailInfo.m_color;
		stInfo._date	= m_pOwnerClan->_ClanDetailInfo.m_date;
		stInfo._idx		= m_pOwnerClan->_ClanDetailInfo.m_ui32CID;
		stInfo._mark	= m_pOwnerClan->_ClanDetailInfo.m_mark; 
		stInfo._maxperson = m_pOwnerClan->_ClanDetailInfo.m_maxPerson;
		stInfo._person	= m_pOwnerClan->_ClanDetailInfo.m_currentPerson;
		stInfo._rank	= (UINT8)m_pOwnerClan->_ClanDetailInfo.m_clanRank; 
		i3String::Copy( stInfo._name, m_pOwnerClan->_ClanDetailInfo.m_name, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
		stInfo.m_TTeamID = m_TTeamID;
	}
 
	pPacket->WriteData(&stInfo,	sizeof(CLAN_MATCHING_TEAM_INFO));
}

void CMatchTeam::MakePacketCreateRoom(i3NetworkPacket* pPacket)
{
	INT32 i32ClanIdx = 0;
	if( m_pOwnerClan )
	{
		i32ClanIdx = m_pOwnerClan->_ClanDetailInfo.m_ui32CID;
	}
	pPacket->WriteData(&i32ClanIdx,		sizeof(INT32));
	pPacket->WriteData(&m_TTeamID,		sizeof(T_TeamID));
}

void CMatchTeam::MakePacketLeaveTeam(i3NetworkPacket* pPacket )
{
	pPacket->WriteData(&m_i8LeaderSlot,		sizeof(INT8));

	MakePacketMemberUIDList( pPacket);
}

void CMatchTeam::MakePacketOperation(i3NetworkPacket* pPacket )
{
	UINT8 ui8Length = (UINT8)i3String::Length( m_strOperation ); 
	pPacket->WriteData(&ui8Length,		sizeof(UINT8)); 
	pPacket->WriteData(m_strOperation,	sizeof(TTCHAR)*ui8Length);

	MakePacketMemberUIDList( pPacket );
}

void CMatchTeam::MakePacketPerMaxCount(i3NetworkPacket* pPacket )
{
	pPacket->WriteData(&m_ui8MaxMember,	sizeof(UINT8));

	MakePacketMemberUIDList( pPacket );
}

void CMatchTeam::MakePacketMemberUIDList(i3NetworkPacket* pPacket  )
{
	T_UID	TUID = -1;
	INT32	i32SessionIdx = -1;

	INT32 i32CurMember = m_pMembers.GetCount();
	pPacket->WriteData(&i32CurMember, sizeof(INT32));

	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if (!pTeamMember)
		{	// 패킷 밀림 현상 방지
			pPacket->WriteData(&TUID, sizeof(T_UID));
			pPacket->WriteData(&i32SessionIdx, sizeof(INT32));
			continue;
		}

		pTeamMember->MakePacketMemberUID(pPacket);
	}
}

void CMatchTeam::MakePacketLeaveRoom(i3NetworkPacket* pPacket )
{
	T_UID TLeaderUID = GetLeaderUID();

	pPacket->WriteData(&TLeaderUID,	sizeof(T_UID));

	MakePacketMemberUIDList( pPacket );
}

void CMatchTeam::MakePacketTeamChatting(i3NetworkPacket* pPacket)
{
	MakePacketMemberUIDList( pPacket );
}

INT32 CMatchTeam::GetMercenaryCount()
{
	INT32 i32MercenaryCnt = 0;
	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pTeamMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pTeamMember || !pTeamMember->IsMercenary() )
			continue;

		++i32MercenaryCnt;
	}

	return i32MercenaryCnt;
}

T_RESULT CMatchTeam::JoinMember(CMercenary* pMercenary, INT32 i32KillDeathRate, INT8& i8JoinSlot )
{
	if( m_eTeamState != TEAM_STATE_WAIT )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL;

	if( m_ui8MaxMember < m_pMembers.GetCount() + 1 )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL;

	if( MAX_MERCENARY_IN_TEAM < GetMercenaryCount() + 1 )
		return EVENT_ERROR_EXCEED_MERCENARY_LIMIT;

	CTeamMember* pMember = new CMercenaryMember(pMercenary);
	if( !pMember )
		return EVENT_ERROR_FAIL_JOIN_TEAM;

	if( m_pMembers.Add(pMember) < 0 )
	{
		I3_SAFE_DELETE(pMember);
		return EVENT_ERROR_FAIL_JOIN_TEAM;
	}

	INT32 i32Slot = SetSlot(pMercenary->GetUserID());
	if( i32Slot < 0 )
	{
		m_pMembers.Remove(pMember);
		I3_SAFE_DELETE(pMember);
		return EVENT_ERROR_CLAN_WAR_SET_SLOT;
	}

	pMember->SetSlot( i32Slot );
	i8JoinSlot = (INT8)i32Slot;

	pMember->SetKillDeathRate(i32KillDeathRate);

	pMember->JoinTeam( NULL, m_TTeamID ); 

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CMatchTeam::JoinMember(T_UID TUID, INT32 i32KillDeathRate, INT8& i8JoinSlot )
{
	if( m_eTeamState != TEAM_STATE_WAIT )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL;

	CLAN_MEMBER_DETAIL_INFO* pClanMember = static_cast<CLAN_MEMBER_DETAIL_INFO*>( m_pOwnerClan->_pMember->FindItemEx(TUID) );
	if( !pClanMember )
		return EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX;

	if( GET_FRIEND_MATCH(pClanMember->_state) != MATCH_TEAM_INVALID_IDX )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL;

	if( m_ui8MaxMember < m_pMembers.GetCount() + 1)
		return EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL;

	CTeamMember* pMember = new CClanMember(pClanMember);
	if( !pMember )
		return EVENT_ERROR_CLAN_WAR_ALLOC_TEAM_MEMBER;

	if( m_pMembers.Add(pMember) < 0 )
	{
		I3_SAFE_DELETE(pMember);
		return EVENT_ERROR_FAIL_JOIN_TEAM;
	}
	
	INT32 i32Slot = SetSlot(TUID);
	if( i32Slot < 0 )
	{
		m_pMembers.Remove(pMember);
		I3_SAFE_DELETE(pMember);
		return EVENT_ERROR_CLAN_WAR_SET_SLOT;
	}

	pMember->SetKillDeathRate(i32KillDeathRate);
	pMember->SetSlot( i32Slot ); 

	i8JoinSlot = (INT8)i32Slot;

	pMember->JoinTeam( m_pOwnerClan, m_TTeamID ); 

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CMatchTeam::ChangeOperation(T_UID TUID, TTCHAR* pOperation)
{
	if( m_eTeamState != TEAM_STATE_WAIT )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL;

	INT32 i32Slot = GetSlotIdx(TUID);

	if( m_i8LeaderSlot != i32Slot ) return EVENT_ERROR_CLAN_WAR_NOT_LEADER;
  
	i3String::Copy( m_strOperation, pOperation, NET_CLAN_OPERATION_SIZE );

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CMatchTeam::ChangeMaxPerCount(T_UID TUID, UINT8 ui8MaxPerCount)
{
	if( m_eTeamState != TEAM_STATE_WAIT )
		return EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL;

	INT32 i32Slot = GetSlotIdx(TUID);

	if( m_i8LeaderSlot != i32Slot ) return EVENT_ERROR_CLAN_WAR_NOT_LEADER;
  
	if( m_ui8MaxMember <= ui8MaxPerCount ) // 인원수 변경 없거나 커지면.
	{
		m_ui8MaxMember = ui8MaxPerCount;
		return EVENT_ERROR_SUCCESS;
	}
	
	// 변경되는 최대 인원수보다 큰 슬롯에 팀원이 있다면 실패입니다.
	for( INT32 i = 0 ; i < MAX_CLAN_MATCH_MEMBER ; ++i )
	{	
		if( ui8MaxPerCount > i ) continue;	// 대상에서 제외되는 슬롯.
		if( GetSlotUID(i) == 0 ) continue;	// 해당 슬롯에 유저가 없다.

		return EVENT_ERROR_USER_HACK_PACKET;	// 실패입니다.
	}

	m_ui8MaxMember = ui8MaxPerCount;

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CMatchTeam::LeaveMember(UINT32 ui32ServerIdx, T_UID TUID, INT8 i8LeaveSlot, UINT32 ui32ClanDBIdx )
{
	CTeamMember* pTeamMember = FindMember(TUID);
	if( !pTeamMember )	return EVENT_ERROR_NOT_FIND_USER;

	if( m_pMembers.GetCount() <= 1 )
	{	// 매치 팀 인원 모두가 나갔다면 팀해체. 
		DestroyTeam(ui32ClanDBIdx );
		return EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE ;
	}

	BOOL bExistClanMember = FALSE;
	UINT8	ui8MerCount = 0;
	TEAM_MEMBER_IDX	aMerIdx[CLAN_TEAM_MEMBER_COUNT];
	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pMember ) continue;

		// 나가는 유저 제외
		T_UID TMemberUID = pMember->GetUserID();
		if( TMemberUID == TUID ) continue;

		if( pMember->IsMercenary() )
		{
			aMerIdx[ui8MerCount].m_TUID				= TMemberUID;
			aMerIdx[ui8MerCount].m_i32SessionIdx	= pMember->GetSessionIdx();
			aMerIdx[ui8MerCount].m_i8Slot			= (INT8)pMember->GetSlot();
			++ui8MerCount;
			continue;
		}
		
		bExistClanMember = TRUE;
		break;
	}

	if( !bExistClanMember )
	{	// 남은 팀원이 전부 용병이면 팀 해체.  
		// 용병들에게 퇴장을 알립니다.
		i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_LEAVE_TEAM_MERCENARY_ACK);
		SendPacket.WriteData(&m_i8LeaderSlot,			sizeof(UINT8));
		SendPacket.WriteData(&ui8MerCount,				sizeof(UINT8));
		SendPacket.WriteData(aMerIdx,					sizeof(TEAM_MEMBER_IDX)*ui8MerCount); 
		g_pGateway->SendPacket(ui32ServerIdx, &SendPacket);

		DestroyTeam(ui32ClanDBIdx );
		return EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE;
	}

	pTeamMember->LeaveTeam(m_pOwnerClan);
	m_pMembers.Remove(pTeamMember);
	I3_SAFE_DELETE(pTeamMember);

	if( i8LeaveSlot == m_i8LeaderSlot ) 
	{	// 나간유저가 팀 리더라면 리더 인계.
		INT32 i32NewLeaderSlot = -1;
		for( INT32 i = 0 ; i < MAX_CLAN_MATCH_MEMBER ; ++i )
		{	// 남은 유저중 슬롯인덱스가 가장빠른 유저가 리더가 됩니다. 용병만 남았다면 위에서 팀 해체됨.
			if( i == m_i8LeaderSlot ) continue;	// 기존 리더는 pass.
			if( 0 == m_aTSlotUID[i] ) continue;	// 빈슬롯 pass.

			CTeamMember* pMember = FindMember(m_aTSlotUID[i]);
			if (NULL == pMember) continue;
			if( pMember->IsMercenary() ) continue;	// 용병이면 pass.

			i32NewLeaderSlot = i;
			break;
		}

		if( -1 == i32NewLeaderSlot )
		{	// 나오는지 봅시다.
	
			g_pLog->WriteLog( L"[ERROR][CMatchTeam::LeaveMember] New Leader Error / UID : %I64d / TeamID : %d / Old Leader Slot : %d / MemberCount : %d ",
				TUID, m_TTeamID, m_i8LeaderSlot, m_pMembers.GetCount());
			i32NewLeaderSlot = 0;
		}
		
		m_i8LeaderSlot = (UINT8)i32NewLeaderSlot;
	}

	// 해당 슬롯 비워줍니다.
	SetSlot(TUID, TRUE);
	
	return EVENT_ERROR_SUCCESS ;
}

T_RESULT CMatchTeam::LeaveMatchRoom(CLAN_USER_INFO* pUserInfo, INT32 i32ChannelIdx, INT8& i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave )
{
	CTeamMember* pTeamMeber = FindMember(pUserInfo->i64UID);
	if( !pTeamMeber ) return EVENT_ERROR_NOT_FIND_USER;

	// Leave team 
	i8LeaveSlot = (INT8)GetSlotIdx( pUserInfo->i64UID ); 
	if( i8LeaveSlot < 0 )	return EVENT_ERROR_WRONG_SLOT_IDX;

	if( ui8RoomState == ROOM_STATE_READY && i8LeaveSlot == m_i8LeaderSlot)
	{	// 레디룸 상태에서 리더가 나가면, 해당 팀원모두 퇴장시킵니다.( 매치팀에서 퇴장시키지는 않습니다. )

		i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_LEAVE_ROOM_ACK);
		SendPacket.WriteData(&i32ChannelIdx, sizeof(INT32));
		MakePacketLeaveRoom( &SendPacket );

 		g_pGateway->SendPacket(pUserInfo->ui32ServerIdx, &SendPacket);

		// 팀 상태 변경
		m_eTeamState = TEAM_STATE_WAIT;

		return EVENT_ERROR_CLAN_WAR_LEAVE_ALL_MATCH_ROOM_FOR_LEADER;
	} 

	if( bForceLeave ) 
	{	// 리더에 의해 레디룸에서 퇴장당함.
		return EVENT_ERROR_CLAN_WAR_LEAVE_ALL_MATCH_ROOM_FOR_LEADER;
	}

	return  EVENT_ERROR_SUCCESS;
}


INT32 CMatchTeam::SetSlot( T_UID TUID, BOOL bReset )
{	
	T_UID	TCompareUID = 0;
	T_UID	TApplyUID = TUID;

	if( bReset ) 
	{
		 TCompareUID = TUID;
		 TApplyUID = 0;
	}

	for( INT32 i = 0 ; i < MAX_CLAN_MATCH_MEMBER ; ++i )
	{ 
		if( m_aTSlotUID[i] != TCompareUID ) continue;

		m_aTSlotUID[i] = TApplyUID;
		return i;	 
	}
	return -1;
}

INT32 CMatchTeam::GetSlotIdx( T_UID TUID )
{	
	for( INT32 i = 0 ; i < MAX_CLAN_MATCH_MEMBER ; ++i )
	{ 
		if( m_aTSlotUID[i] != TUID ) continue;

		return i;
	}
	return -1;
}

T_UID CMatchTeam::GetSlotUID( INT32 i32SlotIdx )
{	
	if( i32SlotIdx < 0 ) return 0;
	if( i32SlotIdx >= MAX_CLAN_MATCH_MEMBER ) return 0;

	return m_aTSlotUID[i32SlotIdx];
}

INT32 CMatchTeam::CalculateMatchingPoint(void )
{
	if( !m_pOwnerClan )
	{
		g_pLog->WriteLog( L"[ERROR][CMatching::CalculateMatchingPoint] Not found clan / TeamId : %d / State : %d", 
			m_TMatchedTeamID, m_eTeamState );
		return (INT32)CLAN_BASIC_POINT;
	}

	INT32 i32MatchingPoint = (INT32)CLAN_BASIC_POINT;

	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pMember ) continue;

		// 개인의 시즌 킬뎃 비율을 이용한 포인트 합산.
		i32MatchingPoint += GetMatchingPointByKDRate(pMember->GetKillDeathRate());

		if( pMember->IsMercenary() ) continue;

		// 클랜원일 경우 개인의 클랜전 킬뎃 비율, 클랜전 승률을 이용한 포인트 합산.
		i32MatchingPoint += GetMatchingPointByKDRate(pMember->GetClanKillDeathRate());
		i32MatchingPoint += GetMatchingPointByWinRate(pMember->GetClanWinRate()); 		
	}

	return i32MatchingPoint;
}

void CMatchTeam::ChangeMemberInfo( CHANGE_INFO	eType, CLAN_WAR_INFO* pUserInfo, MATCH_MEMBER_INFO* pMemberInfo )
{
	CTeamMember* pMember  = FindMember( pUserInfo->i64UID);
	if( !pMember ) return;

	if( FALSE == pMember->ChangeMemberInfo( eType, pMemberInfo ) ) return;

	pMemberInfo->m_i8Slot			= (INT8)pMember->GetSlot();
	pMemberInfo->m_ui8MemberType	= (UINT8)(pMember->IsMercenary() ? MEMBER_MERCENARY : MEMBER_CLAN);

	SendPacketMemberInfoChange(pUserInfo->ui32ServerIdx, eType, pMemberInfo);

	return;
}

void CMatchTeam::SendMerRankUp(  UINT32 ui32ServerIdx )
{
	MATCH_MEMBER_INFO	stMemberInfo;
	stMemberInfo.m_ui8MemberType = MEMBER_MERCENARY;

	for( INT32 i = 0; i < m_pMembers.GetCount(); ++i )
	{
		CTeamMember* pMember = static_cast<CTeamMember*>( m_pMembers.GetItem(i) );
		if( !pMember ) continue;

		if( !pMember->IsMercenary() ) continue;

		if( !pMember->GetSendMerRankUp() ) continue;

		// 용병 랭크가 변경되었으므로 매치팀원들에게 알려줍니다.  
		stMemberInfo.m_i8Slot	= (INT8)pMember->GetSlot();
		stMemberInfo.m_TUID		= pMember->GetUserID();
		stMemberInfo.m_ui8Rank	= pMember->GetMerRank();
		
		SendPacketMemberInfoChange( ui32ServerIdx, CHANGE_INFO_RANK, &stMemberInfo);

		pMember->SetSendMerRankUp(false);

	}

}


void CMatchTeam::SendPacketMemberInfoChange( UINT32 ui32ServerIdx, CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_CHANGE_MEMBER_INFO_ACK);
	SendPacket.WriteData(&eType,		sizeof(UINT8));
	SendPacket.WriteData(pMemberInfo,	sizeof(MATCH_MEMBER_INFO));
	MakePacketMemberUIDList( &SendPacket  );
	g_pGateway->SendPacket(ui32ServerIdx, &SendPacket);

	return;
}
 
void CMatchTeam::SendAllMercenaryLeave( UINT32 ui32ServerIdx, CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_LEAVE_TEAM_MERCENARY_ACK);
	SendPacket.WriteData(&eType,		sizeof(UINT8));
	SendPacket.WriteData(pMemberInfo,	sizeof(MATCH_MEMBER_INFO));
	MakePacketMemberUIDList( &SendPacket );
	g_pGateway->SendPacket(ui32ServerIdx, &SendPacket);

	return;
}

void CMatchTeam::ReleaseClanMatchTeam( UINT32 ui32ClanDBIdx )
{
	CLAN_DETAIL_INFO_SERVER* pClanDetail = g_pTaskProcessor->FindClan2(ui32ClanDBIdx);
	if (!pClanDetail) return;
	
	// 해당 클랜의 매치팀 리스트에서 제거합니다.
	pClanDetail->ReleaseMatchTeam(GetClanTamIdx());

}

//////////////////////////////////////////////////////////////////////////
// CTeamManager
CTeamManager::CTeamManager()
{
	for( INT32 i = 0; i < MAX_MATCH_TEAM_COUNT; ++i )
	{
		m_MatchTeams[i].SetTeamID( (T_TeamID)i );
		m_MatchTeams[i].SetTeamState(TEAM_STATE_NONE);
	}
}

CTeamManager::~CTeamManager()
{
}

CMatchTeam* CTeamManager::GetEmptyTeamBuffer()
{
	for( INT32 i = 0; i < MAX_MATCH_TEAM_COUNT; ++i )
	{
		if( m_MatchTeams[i].GetTeamState() == TEAM_STATE_NONE )
			return &m_MatchTeams[i];
	}

	return NULL;
}

CMatchTeam* CTeamManager::CreateMatchTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, UINT8 ui8MaxMember,  CLAN_MEMBER_DETAIL_INFO* pClanMember, INT32 i32KillDeathRate, UINT32 ui32ServerIdx)
{
	CMatchTeam* pMatchTeam = GetEmptyTeamBuffer();
	if( !pMatchTeam )
		return NULL;

	CTeamMember* pTeamMemeber = new CClanMember(pClanMember);
	if( !pTeamMemeber )
		return NULL;

	pTeamMemeber->SetKillDeathRate(i32KillDeathRate);
	pTeamMemeber->SetSlot(0);

	if( pMatchTeam->AddTeamMember(pTeamMemeber) < 0 )
	{
		I3_SAFE_DELETE(pTeamMemeber);
		return NULL;
	}

	pMatchTeam->CreateTeam( ui8MaxMember, pOwnerClan, pClanMember, ui32ServerIdx );
	
	return pMatchTeam;
}

CMatchTeam* CTeamManager::FindMatchTeam(T_TeamID TTeamID)
{
	if( !CheckVaildTeam(TTeamID) ) return NULL;
	
	return &m_MatchTeams[TTeamID];
}

void CTeamManager::MakePacketTeam(T_TeamID TTeamID, i3NetworkPacket* pPakcet)
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketTeamInfo(pPakcet);
}

void CTeamManager::MakePacketTeamDetail(T_TeamID TTeamID, i3NetworkPacket* pPakcet)
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketTeamDetailInfo(pPakcet);
}

void CTeamManager::MakePacketMemberInfo(T_TeamID TTeamID, i3NetworkPacket* pPakcet )
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketMemberInfo(pPakcet );
}

void CTeamManager::MakePacketLeaveTeam(T_TeamID TTeamID, i3NetworkPacket* pPakcet )
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketLeaveTeam(pPakcet );
}

void CTeamManager::MakePacketOperation(T_TeamID TTeamID, i3NetworkPacket* pPakcet)
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketOperation(pPakcet);
}

void CTeamManager::MakePacketPerMaxCount(T_TeamID TTeamID, i3NetworkPacket* pPakcet)
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketPerMaxCount(pPakcet );
}

void CTeamManager::MakePacketTeamChatting(T_TeamID TTeamID, i3NetworkPacket* pPakcet )
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketTeamChatting(pPakcet );
}

void CTeamManager::MakePacketTeamUIDList(T_TeamID TTeamID, i3NetworkPacket* pPakcet )
{
	if( !CheckVaildTeam(TTeamID) ) return;
	m_MatchTeams[TTeamID].MakePacketMemberUIDList(pPakcet );
}

void CTeamManager::MakePacketMatchTeamList(UINT32 ui32ClanDBIdx, i3NetworkPacket* pPakcet)
{
	UINT8 ui8TeamCount = 0;

	CLAN_DETAIL_INFO_SERVER* pClanDetail = g_pTaskProcessor->FindClan2(ui32ClanDBIdx);
	if( !pClanDetail )
	{	// 해당 클랜검색 실패.
		pPakcet->WriteData( &ui8TeamCount,	sizeof( UINT8 ) );
		return;
	}

	MATCH_TEAM_INFO		aTeamInfo[MAX_CLAN_MATCH_TEAM_COUNT];

	for( INT32 i = 0 ; i < MAX_CLAN_MATCH_TEAM_COUNT; ++i )
	{
		CMatchTeam* pMatchTeam = pClanDetail->GetMatchTeam(i);

		if( !pMatchTeam ) continue; 
		if( ((UINT32)pMatchTeam->GetClanID()) != ui32ClanDBIdx ) continue;

		if(TEAM_STATE_NONE == pMatchTeam->GetTeamState() ) continue;

		aTeamInfo[ui8TeamCount].m_TTeamID		= pMatchTeam->GetTeamID();
		aTeamInfo[ui8TeamCount].m_i8ClanTeamIdx = pMatchTeam->GetClanTamIdx(); // i 랑 같아야함.
		aTeamInfo[ui8TeamCount].m_ui8MaxPer		= pMatchTeam->GetMaxMemberCount();
		aTeamInfo[ui8TeamCount].m_ui8CurPer		= (UINT8)pMatchTeam->GetCurMemberCount();
		aTeamInfo[ui8TeamCount].m_ui8TeamState	= (UINT8)pMatchTeam->GetTeamState();
			
		++ui8TeamCount;
	}

	pPakcet->WriteData( &ui8TeamCount,	sizeof( UINT8 ) );
	pPakcet->WriteData( aTeamInfo,		sizeof( MATCH_TEAM_INFO ) * ui8TeamCount );
}



void CTeamManager::GetMatchTeamCountPerState(INT32* i32TeamCount)
{
	for( INT32 i = 0; i < MAX_MATCH_TEAM_COUNT; ++i )
	{
		CMatchTeam* pTeam = &m_MatchTeams[i];

		if( pTeam->GetTeamState() < TEAM_STATE_NONE || pTeam->GetTeamState() >= TEAM_STATE_COUNT ) continue;

		++i32TeamCount[pTeam->GetTeamState()];		 
	}
}

void CTeamManager::ReleaseAllMatchTeam(void)
{
	// 매치팀 제거
	for (INT32 i = 0; i < MAX_MATCH_TEAM_COUNT; ++i)
	{
		CMatchTeam* pTeam = &m_MatchTeams[i];
		if (!pTeam) continue;
		if (pTeam->GetTeamState() <= TEAM_STATE_NONE) continue;

		UINT32 ui32ClanDBIdx = 0;
		CLAN_DETAIL_INFO_SERVER*	pOwnerClan = pTeam->GetOwnerClan();
		if (pOwnerClan)
		{
			ui32ClanDBIdx = pOwnerClan->_ClanDetailInfo.m_ui32CID;
		}

		pTeam->DestroyTeam(ui32ClanDBIdx);
	}

}