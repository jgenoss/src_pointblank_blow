#include "MatchingManager.h"

#pragma once


#define CLAN_MATCH_BUFFER_SIZE		600	// 동접 5000명이 모두 4v4 팀을 만들었을때 기준

//class CMatching;
class CTeamManager;


class CMatchManager
{
public:
	CMatchManager();
	~CMatchManager();

	void				Create(void);
	void				SetRoom(T_MatchingID TMatchingID, INT32 i32ChannelIdx, INT32 i32MatchRoomIdx);

	bool				JoinRoomMatchMembers(T_MatchingID TMatchingID, i3NetworkPacket* pPacket );
	void				RemoveMatchingTeam(T_MatchingID TMatchingID);

	//void				ApplyMatchResult(CLAN_MATCH_RESULT___* pMatchResult, CLAN_MATCH_MEMBER_RECORD* pMemberRecords);

	bool				CheckBufferIdxValidation(T_MatchingID TMatchingID)					{ return TMatchingID < CLAN_MATCH_BUFFER_SIZE; }
	CMatching*			GetEmptyMatching(T_MatchingID& TMatchingID);

	T_RESULT			LeaveMatchRoom(CTeamManager* pTeamManager, CLAN_USER_INFO* pUserInfo, T_MatchingID TMatchingID, T_TeamID TTeamID, INT8 &i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave );
	T_TeamID			GetTeamID( T_MatchingID TMatchingID, UINT8 ui8TeamType );
	
	CMatching*			GetMatching(T_MatchingID  TMatchingID) 
	{ 
		if( !CheckBufferIdxValidation(TMatchingID) ) return NULL;
		return &m_Matchings[TMatchingID]; 
	}

	void				GetMatchResult(T_MatchingID  TMatchingID, TEAM_TYPE eWinTeam, double* pResultClanPoint );

private:
	CMatching			m_Matchings[CLAN_MATCH_BUFFER_SIZE];
};

