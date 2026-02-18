#pragma once

#define  MERCENARY_RANK_STARTIDX 61		// 클랜매치 용병마크 시작점
//////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------//
//								Clan Information									//
//----------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////

class ClanGameContext : public i3::shared_ginst<ClanGameContext>
{

public:
	ClanGameContext();
	~ClanGameContext();

	//클랜 미 가입자 
	CLAN_LIST_BASIC_INFO *			CreateClanList(INT32 Count);
	void							DeleteClanList(void)				{ I3MEM_SAFE_FREE(m_pClanList); }

	void							CreateMyClanMemberList(INT32 Count);
	void							DeleteMyClanMemberList(void)		{ I3MEM_SAFE_FREE(m_pMyClanMemberList); }

	CLAN_REQUEST_BASIC_INFO *		CreateMyClanRequestList(INT32 Count);
	void							DeleteMyClanRequestList(void)		{ I3MEM_SAFE_FREE(m_pMyClanRequestList); }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CLAN_CONTEXT	*				getClanContext(void)				{ return &m_ClanContext; }
	CLAN_LIST_BASIC_INFO  *			getClanList(void)					{ return m_pClanList; }
	CLAN_DETAIL_INFO * 				getClanDetailInfo(void)				{ return &m_ClanDetailInfoAll.m_ClanDetailInfo; }
	CLAN_TOP_MEMBER		*			getClanTopMember(void)				{ return &m_ClanDetailInfoAll.m_ClanTopMember; }

	CLAN_NEWS			*			getClanNews(void)					{ return &m_ClanDetailInfoAll.m_ClanNews; }
	const CLAN_DETAIL_INFO_ALL* 	getClanDetailInfoAll(void)			{ return &m_ClanDetailInfoAll; }
	CLAN_DETAIL_INFO_ALL* 			setClanDetailInfoAll(void)			{ return &m_ClanDetailInfoAll; }

	//클랜 가입자
	CLAN_BASIC_INFO		* 			getMyClanInfo(void)					{ return &m_MyClanInfo;}
	CLAN_DETAIL_INFO	* 			getMyClanDetailInfo(void)			{ return &m_MyClanDetailInfoAll.m_ClanDetailInfo; }
	const CLAN_DETAIL_INFO_ALL* 	getMyClanDetailInfoAll(void)		{ return &m_MyClanDetailInfoAll; }	
	CLAN_DETAIL_INFO_ALL* 			setMyClanDetailInfoAll(void)		{ return &m_MyClanDetailInfoAll; }	

	CLAN_TOP_MEMBER		*			getMyClanTopMember(void)			{ return &m_MyClanDetailInfoAll.m_ClanTopMember; }
	CLAN_NEWS			*			getMyClanNews(void)					{ return &m_MyClanDetailInfoAll.m_ClanNews; }
	CLAN_MEMBER_CONTEXT * 			getMyClanMemberContext(void)		{ return &m_MyClanMemberContext; }


	const CLAN_MEMBER_BASIC_INFO *	getMyClanMember(INT32 Index);
	void							setMyClanMember(INT32 Index, const CLAN_MEMBER_BASIC_INFO & MemberInfo);

	INT32							GetMyClanMemberMaxCount()			{ return m_MyClanMemberMaxCount; }

	//클랜 스탭만 사용합니다. 
	CLAN_REQUEST_CONTEXT*			getMyClanRequestContext(void)		{ return &m_MyClanRequestContext; }
	CLAN_REQUEST_BASIC_INFO *		getMyClanRequest(void)				{ return m_pMyClanRequestList; }
	CLAN_REQUEST_DETAIL_INFO *		getClanRequestDetailInfo(void)		{return &m_RequestDetailInfo; }



	// Clan Match
	void							setMyClanMatchTeamList(const CLAN_MATCH_TEAM_LIST_DATA & Data );
	const ClanMatchTeamList	*		getMyClanMatchTeamList(void) const					{ return &m_MyClanMatchTeamList; }
	const CLAN_MATCH_TEAM_INFO *	getMyClanMatchTeam(const UINT8 TeamIdx) const;

	void							setClanAllMatingInfo( const CLAN_ALL_MATCHING_DATA & Data );
	const CLAN_ALL_MATCHING_INFO *	getClanAllMatingInfo( void) const	{ return &m_ClanAllMatingInfo; }

	void							setMyMatchSquadInfo(const CLAN_WAR_CREATE_TEAM_DATA & Data);
	void							setMyMatchSquadInfo(const CLAN_WAR_JOIN_TEAM_DATA & Data);

	void							modifyMyMatchSquadInfo(const CLAN_WAR_LEAVE_TEAM_DATA & Data);
	void							modifyMyMatchSquadOperation(const CLAN_WAR_CHANGE_OPERATION_DATA & Data);
	void							modifyMyMatchSquadMaxPerson(const CLAN_WAR_CHANGE_MAX_PERSON_DATA & Data);

	void							resetMyMatchSquadInfo(void)							{ m_MyMatchSquadInfo.Reset(); }
	const CLAN_MATCH_SQUAD_INFO *	getMyMatchSquadInfo(void) const						{ return &m_MyMatchSquadInfo; }

	const bool						isMatchTeamLeader(void) const;	

	void							setClanBattleTeamInfo(const CLAN_BATTLE_TEAM_DATA & Data);
	const CLAN_MATCH_BATTLE_TEAM *	getClanBattleTeamInfo(const TEAM_TYPE type)			{ return &m_ClanBattleTeamInfo[type]; }	
	
	void							setClanNameToTeamID(INT32 teamid, const i3::rc_wstring str)		{ m_ClanBattleTeamInfo[TEAM_RED].getMatchTeamID() == teamid ?
																									  m_ClanBattleTeamInfo[TEAM_RED]._wstrClanName = str:m_ClanBattleTeamInfo[TEAM_BLUE]._wstrClanName = str; }
	void							setClanMarkToTeamID(INT32 teamid, UINT32 mark)					{ m_ClanBattleTeamInfo[TEAM_RED].getMatchTeamID() == teamid ?
																									  m_ClanBattleTeamInfo[TEAM_RED]._ui32ClanMark = mark : m_ClanBattleTeamInfo[TEAM_BLUE]._ui32ClanMark = mark;
																									}
	void							setClanColorNameToTeamID(INT32 teamid, UINT8 color)				{ return; /*아직 클랜 칼라 정보는 받아 오지 않는듯 하다*/ }

	void							resetClanBattleTeamInfo(void);

	//

	bool							getClanBattleLobbyEnable(void)						{ return m_bClanBattleLobbyEnable; }
	void							setClanBattleLobbyEnable(bool isenable)				{ m_bClanBattleLobbyEnable = isenable; }

	bool							getClanChannel(void)								{ return m_bClanChannel; }
	void							setClanChannel(bool Flag)							{ m_bClanChannel = Flag; }

	const UINT8						getMyClanMatchTeamIdx()								{ return m_TeamIndex; }
	void							setMyClanMatchTeamIdx(UINT8 idx)					{ m_TeamIndex = idx; }

	bool							ChangeMercRank(UINT8 iOld, UINT8 iNew);

	bool							IsForceLeaveChannel()								{ return m_bForceLeaveChannel; }
	void							setForceLeaveChannel(bool enable)					{ m_bForceLeaveChannel = enable; }

	/************************************************************************/
	/*								Clan Mercenary							*/
	/************************************************************************/
	void							setClanMercenaryList( const CLAN_WAR_MERCENARY_LIST_DATA & Data );
	void							resetClanMercenaryList() { m_ClanMercenaryList.clear(); }
	const CLAN_MERCENARY_INFO *		getClanMercenaryInfo( const INT32 slot_idx ) const;
	const CLAN_MERCENARY_INFO *		getClanMercenaryInfo( const T_MerID MerID ) const;
	const CLAN_MERCENARY_INFO *		getClanMercenaryInfoByName( const i3::rc_wstring str) const; 

	void							setClanMercenaryRecord( const CLAN_WAR_MERCENARY_RECORD_DATA & Data );
	const CLAN_MERCENARY_RECORD *	getClanMercenaryRecord() const;

	const INT32						getClanMercenaryCount( void) const					{ return m_ClanMercenaryList.size(); }

	bool							getMyMercenaryState(void)							{ return m_bIsMercenary; }
	void							setMyMercenaryState(bool Flag)						{ m_bIsMercenary = Flag; }

	void							SetSelectedMerID(int idx)							{ m_SelectedMerID = m_ClanMercenaryList[idx].getMecenaryID();}
	T_MerID							GetSelectedMerID()									{ return m_SelectedMerID; 	}

	// 용병 상세정보가 존재하는지 여부. EVENT_CLAN_WAR_MERCENARY_DETAIL_INFO result 성공일 때만 true로 세팅하기 때문에
	// 현재 용병 상세정보 요청으로 UserProfile을 연 것인지 확인하는 용도로 쓰입니다.
	// UserProfile 닫을 때 reset. 
	void							SetMercDetailInfo()									{ m_bMerceInfo = true;  }
	void							ResetMercDetailInfo()								{ m_bMerceInfo = false; }
	bool							IsExistMercDetailInfo()								{ return m_bMerceInfo; }
	
	bool							IsMaxMerceMyTeam();
	INT32							GetMercIconIdx(const CLAN_MERCENARY_INFO *pInfo);
	
	void							SetMercRank(INT32 slotidx, UINT8 ClassType, UINT8 rank);
	INT32							GetMercRank(INT32 slotidx);
	void							ResetMercRank();

	bool							IsMatchEnd()										{ return m_bMatchEnd; }
	void							SetMatchEnd(bool enable)							{ m_bMatchEnd = enable; }

	// 용병 이탈 패널티 적용 여부
	void							SetMyMercDepartured(bool enable)						{ m_bMyMercDepartured = enable; }
	bool							IsMyMercDepartured()									{ return m_bMyMercDepartured; }

	CLAN_PERSONAL_MATCH_RECORD*		getMathRecord(INT32 slotidx);

	void							ProcessMatchResult(const CLAN_MATCH_MAKING_RESULT & Data);

private:
	//미 가입자 7
	CLAN_CONTEXT					m_ClanContext; 
	CLAN_LIST_BASIC_INFO  *			m_pClanList; 
	CLAN_DETAIL_INFO_ALL			m_ClanDetailInfoAll;

	//클랜원
	CLAN_BASIC_INFO					m_MyClanInfo; 
	CLAN_DETAIL_INFO_ALL			m_MyClanDetailInfoAll; 
	CLAN_MEMBER_CONTEXT				m_MyClanMemberContext; 
	CLAN_MEMBER_BASIC_INFO	*		m_pMyClanMemberList; 
	INT32							m_MyClanMemberMaxCount;

	bool							m_bForceLeaveChannel;				// 클랜탈퇴/제명/해체 등의 이유로 강제로 클랜채널에서 leave 시켜야 할 때 true로 설정

	// Match Making
	ClanMatchTeamList				m_MyClanMatchTeamList;				// 내 클랜 팀 리스트
	CLAN_ALL_MATCHING_INFO			m_ClanAllMatingInfo;				// 모든 매칭 팀 정보 (카운트)
	CLAN_MATCH_SQUAD_INFO			m_MyMatchSquadInfo;					// 내 매치팀 정보.
	
	CLAN_MATCH_BATTLE_TEAM			m_ClanBattleTeamInfo[TEAM_COUNT];	// 클랜전 배틀 팀 정보 (레디룸)

	bool							m_bClanBattleLobbyEnable;
	bool							m_bClanChannel; // 왜 쓰이는건지 모르겠다...

	//클랜 스탭만 사용합니다. 
	CLAN_REQUEST_CONTEXT			m_MyClanRequestContext; 
	CLAN_REQUEST_BASIC_INFO *		m_pMyClanRequestList;
	CLAN_REQUEST_DETAIL_INFO		m_RequestDetailInfo; 

	UINT8							m_TeamIndex;
	CLAN_PERSONAL_MATCH_RECORD 		m_pMatchRecord[SLOT_MAX_COUNT];

	/************************************************************************/
	/*								Clan Mercenary							*/
	/************************************************************************/
	ClanMercenaryList				m_ClanMercenaryList;
	bool							m_bIsMercenary;			// 현재 자신이 용병인지 아닌지..
	CLAN_MERCENARY_RECORD			m_ClanMercenaryRecord;
	T_MerID							m_SelectedMerID;		// 현재 리스트에서 클릭된 용병의 MerID
	bool							m_bMerceInfo;			// 용병 리스트에서 용병 정보 팝업을 열게 되면 true. 평소에는 false
	INT32							m_MercRank[SLOT_MAX_COUNT];		// 용병 전용 마크
	bool							m_bMatchEnd;			// 매칭하여 배틀이 종료
	bool							m_bMyMercDepartured;			// 자기 자신이 이탈한 용병인지
};