#include "pch.h"
#include "MatchManager.h"

#include "TaskProcessor.h"
#include "MatchingManager.h"


CMatchManager::CMatchManager()
{
}

CMatchManager::~CMatchManager()
{
}

void CMatchManager::Create()
{
	for( INT32 i = 0 ; i < CLAN_MATCH_BUFFER_SIZE; ++i )
	{
		m_Matchings[i].SetState(MATCH_NONE);
	}
}

void CMatchManager::SetRoom(T_MatchingID TMatchingID, INT32 i32ChannelIdx, INT32 i32MatchRoomIdx)
{
	if( !CheckBufferIdxValidation(TMatchingID) )
		return;

	m_Matchings[TMatchingID].SetRoom(i32ChannelIdx,i32MatchRoomIdx);
}

bool CMatchManager::JoinRoomMatchMembers(T_MatchingID TMatchingID, i3NetworkPacket* pPacket )
{
	if( !CheckBufferIdxValidation(TMatchingID) )
		return false;

	return m_Matchings[TMatchingID].MakePacketJoinRoom(pPacket );
}

void CMatchManager::RemoveMatchingTeam(T_MatchingID TMatchingID)
{
	if( !CheckBufferIdxValidation(TMatchingID) )
		return;

	m_Matchings[TMatchingID].ResetMatching();
} 

CMatching* CMatchManager::GetEmptyMatching(T_MatchingID& TMatchingID)
{
	for( INT32 i = 0; i < CLAN_MATCH_BUFFER_SIZE; ++i )
	{
		if( m_Matchings[i].GetState() != MATCH_NONE ) continue;

		TMatchingID = (T_MatchingID)i;
		return &m_Matchings[i];
	}

	TMatchingID = MATCHING_ID_INVALID_IDX;

	return NULL;
}
 
T_RESULT CMatchManager::LeaveMatchRoom(CTeamManager* pTeamManager, CLAN_USER_INFO* pUserInfo, T_MatchingID TMatchingID, T_TeamID TTeamID, INT8& i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave )
{
	CLAN_DETAIL_INFO_SERVER* pClanDetail = g_pTaskProcessor->FindClan2(pUserInfo->ui32ClanDBIdx);
	if( !pClanDetail ) return EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;

	CLAN_MEMBER_DETAIL_INFO* pMember = static_cast<CLAN_MEMBER_DETAIL_INFO*>( pClanDetail->_pMember->FindItemEx(pUserInfo->i64UID) );
	if( !pMember ) return EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX;

	if( !CheckBufferIdxValidation(TMatchingID) ) return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	if( MATCH_NONE == m_Matchings[TMatchingID].GetState() ) return EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL;

	//CMatching* pMatching = m_Matchings[TMatchingID];
	return m_Matchings[TMatchingID].LeaveMatchRoom(TTeamID, pUserInfo, i8LeaveSlot, ui8RoomState, bForceLeave );

}

T_TeamID CMatchManager::GetTeamID( T_MatchingID TMatchingID, UINT8 ui8TeamType )
{
	if( !CheckBufferIdxValidation(TMatchingID) ) return MATCH_TEAM_INVALID_IDX; 
	if( ui8TeamType <= TEAM_NONE || ui8TeamType >= TEAM_COUNT ) return MATCH_TEAM_INVALID_IDX;

	//CMatching* pMatching = m_Matchings[TMatchingID];
	return m_Matchings[TMatchingID].GetTeamID((TEAM_TYPE)ui8TeamType );

}

void CMatchManager::GetMatchResult(T_MatchingID  TMatchingID, TEAM_TYPE eWinTeam, double* pResultClanPoint )
{
	if( !CheckBufferIdxValidation(TMatchingID) ) return;

	if( MATCH_NONE == m_Matchings[TMatchingID].GetState() ) return;

	//CMatching* pMatching = m_Matchings[TMatchingID];
	m_Matchings[TMatchingID].SimulateRating(  eWinTeam, pResultClanPoint );

	return;
}