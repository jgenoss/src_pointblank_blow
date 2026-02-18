#pragma once


class CTeamMember;
class CMercenary;
struct MATCHING_REQUEST;

class CMatchTeam
{

public:
	void				CreateTeam(UINT8 ui8MaxMember, CLAN_DETAIL_INFO_SERVER* pOwnerClan, CLAN_MEMBER_DETAIL_INFO* pClanMember, UINT32 ui32ServerIdx );
	void				DestroyTeam( UINT32 ui32ClanDBIdx );

	T_RESULT			JoinMember(T_UID TUID, INT32 i32KillDeathRate, INT8& i8JoinSlot );
	T_RESULT			JoinMember(CMercenary* pMercenary, INT32 i32KillDeathRate, INT8& i8JoinSlot );
	T_RESULT			LeaveMember(UINT32 ui32ServerIdx, T_UID TUID, INT8 i8LeaveSlot, UINT32 ui32ClanDBIdx);
	
	T_RESULT			ChangeOperation(T_UID TUID, TTCHAR* pOperation);
	T_RESULT			ChangeMaxPerCount(T_UID TUID, UINT8 ui8MaxPerCount);

	T_RESULT			LeaveMatchRoom(CLAN_USER_INFO* pUserInfo, INT32 i32ChannelIdx, INT8& i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave );

	void				SetTeamID(T_TeamID TTeamID)					{ m_TTeamID = TTeamID; }
	T_TeamID			GetTeamID()									{ return m_TTeamID; }

	void				SetTeamState(TEAM_STATE eState)					{ m_eTeamState = eState; }

	void				SetMaxMember(UINT8 ui8MaxMember)				{ m_ui8MaxMember = ui8MaxMember; }
	UINT8				GetMaxMemberCount()								{ return m_ui8MaxMember; }

	INT32				GetCurMemberCount()								{ return m_pMembers.GetCount();; }
	

	void				SetOwnerClan(CLAN_DETAIL_INFO_SERVER* pOwnerClan)			{ m_pOwnerClan = pOwnerClan; }
	UINT32				GetClanID()										{ return m_pOwnerClan ? m_pOwnerClan->_ClanDetailInfo.m_ui32CID : INVALID_IDX; }
	TTCHAR*				GetClanName()									{ return m_pOwnerClan ? m_pOwnerClan->_ClanDetailInfo.m_name : NULL; }
	
	TEAM_STATE			GetTeamState()									{ return m_eTeamState; }
	T_UID				GetLeaderUID()									{ return m_aTSlotUID[m_i8LeaderSlot]; }
	INT32				GetMercenaryCount();

	bool				IsFilledAllMember()								{ return m_ui8MaxMember == m_pMembers.GetCount(); }

	CRating&			GetClanRating()									{ return m_pOwnerClan->_ClanDetailInfo.GetClanRating(); }

	CLAN_DETAIL_INFO_SERVER*	GetOwnerClan()									{ return m_pOwnerClan; }

	void				SetMatchedTeam(T_TeamID TTeamID)			
	{ 
		m_TMatchedTeamID = TTeamID; 
	}

	void				CalculateRatings(ROOM_ENDTYPE eResult, REAL64 r64C, REAL64 r64V, REAL64 r64W);
	void				ApplyMatchResult(CLAN_WAR_RESULT* pClanWarResult, T_UID* pPlayerUID);

	void				MakeMatchingRequst(MATCHING_REQUEST& request );

	void				MakePacketTeamInfo(i3NetworkPacket* pPacket);
	void				MakePacketTeamDetailInfo(i3NetworkPacket* pPacket);
	void				MakePacketMemberInfo(i3NetworkPacket* pPacket );
	void				MakePacketJoinRoom(i3NetworkPacket* pPacket );
	void				MakePacketCreateRoom(i3NetworkPacket* pPacket);
	void				MakePacketLeaveTeam(i3NetworkPacket* pPacket );
	void				MakePacketOperation(i3NetworkPacket* pPacket );
	void				MakePacketPerMaxCount(i3NetworkPacket* pPacket );
	void				MakePacketMemberUIDList(i3NetworkPacket* pPacket );
	void				MakePacketLeaveRoom(i3NetworkPacket* pPacket );
	void				MakePacketTeamChatting(i3NetworkPacket* pPacket );
	void				MakePacketClanMatchingInfo(i3NetworkPacket* pPacket);

	INT32				AddTeamMember(CTeamMember* pTeamMember)				{ return m_pMembers.Add(pTeamMember); }

	void				SetOperation(TTCHAR* pstrOperation )				{ i3String::Copy( m_strOperation, pstrOperation, sizeof(TTCHAR) * NET_CLAN_OPERATION_SIZE); }
	TTCHAR*				GetOperation(void)									{ return m_strOperation; }

	INT32				SetSlot( T_UID TUID, BOOL bReset = FALSE );	// 슬롯에 UID 설정.
	INT32				GetSlotIdx( T_UID TUID );		// UID로 슬롯인덱스 가져오기.
	T_UID				GetSlotUID( INT32 i32SlotIdx );	// 슬롯인덱스로 UID 가져오기.

	INT8				GetClanTamIdx(void)	{ return m_i8ClanTeamIdx; }	
	void				SetClanTamIdx(INT8 i8Idx)	{ m_i8ClanTeamIdx = i8Idx; }	

	INT32				CalculateMatchingPoint(void);

	
	INT32				GetMatchingPointByKDRate(INT32 i32KillDeathRate)	// 킬뎃 비율에 따른 매칭포인트 계산
	{	// 킬뎃 비율에 따라 최소 -53 ~ 최대 +59
		if( i32KillDeathRate < 55 )	return (i32KillDeathRate/2) - 53; 
		if( i32KillDeathRate < 56 ) return -25;
		if( i32KillDeathRate < 61 ) return 0;
		if( i32KillDeathRate < 66 ) return 10;
		return 25 + ( i32KillDeathRate - 66 ) ;
	}

	INT32				GetMatchingPointByWinRate(INT32 i32WinRate)	// 승률 비율에 따른 매칭포인트 계산
	{	// 승률 비율에 따라 최소 -50 ~ 최대 +59
		if( i32WinRate < 50 ) return (i32WinRate/2) - 50; 
		if( i32WinRate < 51 ) return -25;
		if( i32WinRate < 56 ) return 0;
		if( i32WinRate < 66 ) return 10;
		return 25 + ( i32WinRate - 66 ) ;
	}

	CTeamMember*		FindMember(INT64 i64userID);

	void				ChangeMemberInfo( CHANGE_INFO	eType, CLAN_WAR_INFO* pUserInfo, MATCH_MEMBER_INFO* pMemberInfo );

	void				SendMerRankUp(UINT32 ui32ServerIdx);
	void				SendPacketMemberInfoChange(UINT32 ui32ServerIdx, CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo);
	void				SendAllMercenaryLeave( UINT32 ui32ServerIdx, CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo );	// 모든 용병 팀 퇴장.( 팀에 용병만 남은경우 )

	void				ReleaseClanMatchTeam(UINT32 ui32ClanDBIdx);

	T_TeamID			GetMatchedTeamID(void)	{ return m_TMatchedTeamID; }
	UINT32				GetServerIdx(void)		{ return m_ui32ServerIdx; }
private:
	CTeamMember*		GetEmptyBuffer();
	
public:
	CMatchTeam();
	~CMatchTeam();


private:
	T_TeamID			m_TTeamID;
	TEAM_STATE			m_eTeamState;

	UINT8				m_ui8MaxMember;			// 한팀 기준.
	UINT32				m_ui32MatchRequestTime;

	CLAN_DETAIL_INFO_SERVER*			m_pOwnerClan;
	i3List				m_pMembers;

	T_TeamID			m_TMatchedTeamID;

	TTCHAR				m_strOperation[NET_CLAN_OPERATION_SIZE];	// 작전명
	T_UID				m_aTSlotUID[MAX_CLAN_MATCH_MEMBER];			// 팀 슬롯 관리용.

	INT8				m_i8LeaderSlot;		// 팀 리더 슬롯 인덱스
	INT8				m_i8ClanTeamIdx;	// 클랜 매치팀 인덱스. 0 ~ MAX_CLAN_MATCH_TEAM_COUNT-1.

	UINT32				m_ui32ServerIdx;	// Game 서버 Idx
};




//////////////////////////////////////////////////////////////////////////
// CMatchTeamMgr
class CTeamManager
{
//	I3_CLASS_DEFINE( CTeamManager );

public:
	CMatchTeam*			CreateMatchTeam(CLAN_DETAIL_INFO_SERVER* pOwnerClan, UINT8 ui8MaxMember, CLAN_MEMBER_DETAIL_INFO* pClanMember, INT32 i32KillDeathRate, UINT32 ui32ServerIdx);

	CMatchTeam*			FindMatchTeam(T_TeamID TTeamID);

	void				MakePacketMatchTeamList(UINT32 ui32ClanDBIdx, i3NetworkPacket* pPakcet);

	bool				CheckVaildTeam(T_TeamID TTeamID)
	{
		if( TTeamID >= MAX_MATCH_TEAM_COUNT ) return false;
		if( TEAM_STATE_NONE == m_MatchTeams[TTeamID].GetTeamState() ) return false;
		return true;
	}

	void				MakePacketTeam(T_TeamID TTeamID, i3NetworkPacket* pPakcet);
	void				MakePacketTeamDetail(T_TeamID TTeamID, i3NetworkPacket* pPakcet);
	void				MakePacketMemberInfo(T_TeamID TTeamID, i3NetworkPacket* pPakcet );
	void				MakePacketLeaveTeam(T_TeamID TTeamID, i3NetworkPacket* pPakcet );
	void				MakePacketOperation(T_TeamID TTeamID, i3NetworkPacket* pPakcet );
	void				MakePacketPerMaxCount(T_TeamID TTeamID, i3NetworkPacket* pPakcet ); 
	void				MakePacketTeamChatting(T_TeamID TTeamID, i3NetworkPacket* pPakcet );
	void				MakePacketTeamUIDList(T_TeamID TTeamID, i3NetworkPacket* pPakcet );

	void				GetMatchTeamCountPerState(INT32* i32TeamCount);	// TEAM_STATE별 매치팀 카운트 조회

	void				ReleaseAllMatchTeam(void);
private:
	CMatchTeam*			GetEmptyTeamBuffer();


public:
	CTeamManager();
	~CTeamManager();


private:
	CMatchTeam			m_MatchTeams[MAX_MATCH_TEAM_COUNT];
};
