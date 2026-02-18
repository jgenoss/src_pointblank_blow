#ifndef _CLAN_STRUCT_DEF_H
#define _CLAN_STRUCT_DEF_H

// 클랜 팀 정보
typedef struct PACKET_CLAN_WAR_MATCH_TEAM_LIST_ACK		CLAN_MATCH_TEAM_LIST_DATA;	// 내 클랜 매치 팀 리스트 정보
struct CLAN_MATCH_TEAM_INFO
{
	T_TeamID		_TMatchTeamID;		// 매치팀 ID
	INT8			_i8ClanTeamIdx;		// 클랜 팀 Idx
	TEAM_STATE		_TeamState;			// 팀 상태
	UINT8			_ui8MaxPerson;		// 최대 인원수
	UINT8 			_ui8CurPerson;		// 현재 인원수	

	CLAN_MATCH_TEAM_INFO(void)
	{
		Reset();
	}

	CLAN_MATCH_TEAM_INFO(const MATCH_TEAM_INFO & Info )
	{
		ParseData(Info);
	}

	void				ParseData(const MATCH_TEAM_INFO & Info);
	void				Reset(void);

	void				setMaxPerson(const UINT8 value)	{ _ui8MaxPerson = value; }

	const T_TeamID		getMatchTeamID(void)	const	{ return _TMatchTeamID; }
	const INT8			getClanTeamIdx(void)	const	{ return _i8ClanTeamIdx; }
	const TEAM_STATE	getTeamState(void)		const	{ return _TeamState; }
	const UINT8			getMaxPerson(void)		const	{ return _ui8MaxPerson; }
	const UINT8			getCurPerson(void)		const	{ return _ui8CurPerson; }

	const bool			isSameState(const TEAM_STATE state)		const	{ return _TeamState == state; }
	const bool			isMaxPerson(void)						const	{ return _ui8CurPerson == _ui8MaxPerson; }
};

typedef i3::vector<CLAN_MATCH_TEAM_INFO>	ClanMatchTeamList;

// 클랜 매칭 정보
typedef struct PACKET_CLAN_WAR_MATCH_TEAM_COUNT_ACK		CLAN_ALL_MATCHING_DATA;	// 전 서버 클랜 매치 팀 카운트

struct CLAN_ALL_MATCHING_INFO
{
	INT32	_i32MatchCompleteTeamCount;		// 매칭 된 모든 팀 개수 (대기방 & 배틀중)
	INT32	_i32MatchWaitngTeamCount;		// 매칭 대기중인 모든 팀 개수 (매칭 버튼 누른 팀)

	CLAN_ALL_MATCHING_INFO(void)
	{
		Reset();
	}

	void			ParseData(const CLAN_ALL_MATCHING_DATA & Data);
	void			Reset(void);

	const INT32		getMatchCompleteTeamCount(void)					const	{ return _i32MatchCompleteTeamCount; }
	const INT32		getMatchWaitTeamCount(void)						const	{ return _i32MatchWaitngTeamCount; }
};

// 클랜 매치 방 정보
struct CLAN_MATCH_ROOM_INFO
{
	i3::rc_wstring	_wstrRoomName;		// 팀 이름
	UINT8			_ui8RoomIdx;		// 팀 인덱스
	bool			_ActivateSlot;		// 슬롯 활성 상태

	CLAN_MATCH_ROOM_INFO(void)
	{	
		Reset();
	}

	CLAN_MATCH_ROOM_INFO( const CLAN_MATCH_TEAM_INFO & Info)
	{
		ParseData(Info);
	}

	void					ParseData(const CLAN_MATCH_TEAM_INFO & Info);
	void					Reset(void);
	void					setActivateSlot(const bool bFlag)	{ _ActivateSlot = bFlag; }

	const i3::rc_wstring &	getRoomName(void)			const	{ return _wstrRoomName; }
	const UINT8				getRoomIdx(void)			const	{ return _ui8RoomIdx; }
	const bool				isActivateSlot(void)		const	{ return _ActivateSlot; }
};

typedef i3::vector<CLAN_MATCH_ROOM_INFO>	ClanMatchRoomList;

// 클랜전 분대 멤버 정보
struct CLAN_MATCH_SQUAD_MEMBER
{
	T_UID			_ui16UID;
	MEMBER_TYPE		_MemberType;
	INT8			_i8SlotIdx;
	UINT8			_ui8Rank;
	UINT8			_ui8NickColor;
	i3::rc_wstring	_wstrNickName;
	UINT8			_ui8MerClass;		// 용병 주무기 병과. WEAPON_CLASS_TYPE

	CLAN_MATCH_SQUAD_MEMBER(void)
	{
		Reset();
	}

	void					ParseData(const MATCH_MEMBER_INFO & Info);
	void					Reset(void);

	const i3::rc_wstring &	getNickName(void)			const { return _wstrNickName; }
	const T_UID				getUID(void)				const { return _ui16UID; }
	const MEMBER_TYPE		getMemberType(void)			const { return _MemberType; }
	const INT8				getSlotIdx(void)			const { return _i8SlotIdx; }
	const UINT8				getRank(void)				const { return _ui8Rank; }
	const UINT8				getNickColor(void)			const { return _ui8NickColor; }
	const UINT8				getMerClass(void)			const { return _ui8MerClass; }

	const bool				isEmptySlot(void)			const	{ return getUID() == 0; }
};

// 클랜전 분대 정보.
typedef struct PACKET_CLAN_WAR_CREATE_TEAM_ACK			CLAN_WAR_CREATE_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_JOIN_TEAM_ACK			CLAN_WAR_JOIN_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_LEAVE_TEAM_ACK			CLAN_WAR_LEAVE_TEAM_DATA;
typedef struct PACKET_CLAN_WAR_CHANGE_OPERATION_ACK		CLAN_WAR_CHANGE_OPERATION_DATA;
typedef struct PACKET_CLAN_WAR_CHANGE_MAX_PER_ACK		CLAN_WAR_CHANGE_MAX_PERSON_DATA;

struct CLAN_MATCH_SQUAD_INFO : CLAN_MATCH_TEAM_INFO
{
	INT8					_i8LeaderSlot;
	INT8					_i8MySlot;
	i3::rc_wstring			_wstrSquadName;
	i3::rc_wstring			_wstrOperation;
	CLAN_MATCH_SQUAD_MEMBER	_Member[MAX_CLAN_MATCH_MEMBER];

	CLAN_MATCH_SQUAD_INFO(void)
	{
		Reset();
	}

	void							ParseData(const CLAN_WAR_CREATE_TEAM_DATA & Data, const MATCH_MEMBER_INFO & Member);
	void							ParseData(const CLAN_WAR_JOIN_TEAM_DATA & Data);
	void							Reset(void);
	
	void							setLeaderSlot(const INT8 idx)			{ _i8LeaderSlot = idx; }
	void							setOperation(const i3::rc_wstring wstr)	{ _wstrOperation = wstr; }
	void							resetMember(const INT8 idx)				{ _Member[idx].Reset(); }

	const INT8						getLeaderSlot(void)				const	{ return _i8LeaderSlot; }
	const INT8						getMySlot(void)					const	{ return _i8MySlot; }
	const i3::rc_wstring &			getSquadName(void)				const	{ return _wstrSquadName; }
	const i3::rc_wstring &			getOperation(void)				const	{ return _wstrOperation; }
	const CLAN_MATCH_SQUAD_MEMBER &	getMember(const UINT8 idx)		const	{ return _Member[idx]; }

	const bool						isJoinTeam(void)				const	{ return getTeamState() != TEAM_STATE_NONE; }
	const bool						isTeamLeader(const INT8 idx)	const	{ return (isJoinTeam() && getLeaderSlot() == idx); }	
	const bool						isMySlot(const INT8 slot)		const	{ return getMySlot() == slot; }
};

typedef struct PACKET_D_CS_NEW_DETAIL_INFO					CLAN_DETAIL_INFO_ALL;		// 클랜 상세 정보

struct CLAN_MATCH_BATTLE_TEAM
{
	i3::rc_wstring		_wstrClanName;
	UINT32				_ui32ClanMark;
	T_TeamID			_TMatchTeamID;		// 매치팀 ID

	CLAN_MATCH_BATTLE_TEAM(void)
	{
		Reset();
	}

	void							ParseData(const CLAN_MATCHING_TEAM_INFO & Info);
	void							Reset(void);

	const i3::rc_wstring &			getClanName(void)				const	{ return _wstrClanName; }
	const UINT32					getClanMark(void)				const	{ return _ui32ClanMark; }
	const T_TeamID					getMatchTeamID(void)			const	{ return _TMatchTeamID; }
};

typedef struct PACKET_CLAN_WAR_MATCH_TEAM_INFO_ACK			CLAN_BATTLE_TEAM_DATA;		// 클랜 배틀 팀 정보

/************************************************************************/
/*								  용병 정보								*/
/************************************************************************/
// 클랜 용병 정보
typedef struct PACKET_CLAN_WAR_MERCENARY_LIST_ACK		CLAN_WAR_MERCENARY_LIST_DATA;
struct CLAN_MERCENARY_INFO
{
	T_MerID				_TMercenaryID;		// 용병 ID
	MERCENARY_STATE		_State;				// 용병 상태
	WEAPON_CLASS_TYPE	_ClassType;			// 주무기 병과
	UINT8				_ui8MerRank;		// 용병 계급
	UINT8				_ui8NickColor;		// 닉네임 컬러
	i3::rc_wstring		_wstrNickName;		// 닉네임

	CLAN_MERCENARY_INFO(void)
	{
		Reset();
	}

	CLAN_MERCENARY_INFO(const MERCENARY_INFO & Info )
	{
		ParseData(Info);
	}

	void						Reset(void);
	void						ParseData(const MERCENARY_INFO & Info);

	const T_MerID				getMecenaryID(void)	const	{ return _TMercenaryID; }
	const MERCENARY_STATE		getState(void)		const	{ return _State; } 
	const WEAPON_CLASS_TYPE		getClassType(void)	const	{ return _ClassType; }
	const UINT8					getRank(void)		const	{ return _ui8MerRank; }
	const UINT8					getNickColor(void)	const	{ return _ui8NickColor; }
	const i3::rc_wstring &		getNickName(void)	const	{ return _wstrNickName; }
};

typedef i3::vector<CLAN_MERCENARY_INFO>	ClanMercenaryList;

// 클랜 용병 전적
// CLAN_MERCENARY_RECOED 에서 CLAN_MERCENARY_RECORD 으로 오타 수정
typedef struct PACKET_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK		CLAN_WAR_MERCENARY_RECORD_DATA;
struct CLAN_MERCENARY_RECORD
{
	INT32			_i32MatchCount;
	INT32			_i32WinMatchCnt;
	INT32			_i32LoseMatchCnt;
	INT32			_i32DisconMatchCnt;
	INT32			_i32KillCount;
	INT32			_i32DeathCount;
	INT32			_i32Headshot;
	INT32			_i32AssistCount;

	CLAN_MERCENARY_RECORD(void)
	{
			Reset();
	}

	CLAN_MERCENARY_RECORD(const MERCENARY_RECORD & Info)
	{
		ParseData(Info);
	}

	void			Reset();
	void			ParseData(const MERCENARY_RECORD & Info);

	INT32			getMatchCount(void)		const	{	return _i32MatchCount;	}
	INT32			getWinMatchCnt(void)	const	{	return _i32WinMatchCnt;	}
	INT32			getLoseMatchCnt(void)	const	{	return _i32LoseMatchCnt;	}
	INT32			getDisconMatchCnt(void)	const	{	return _i32DisconMatchCnt;	}
	INT32			getKillCount(void)		const	{	return _i32KillCount;	}
	INT32			getDeathCount(void)		const	{	return _i32DeathCount;	}
	INT32			getHeadshotCount(void)	const	{	return _i32Headshot; }
	INT32			getAssistCount(void)	const	{	return _i32AssistCount; }
};

struct CLAN_PERSONAL_MATCH_RECORD
{
	INT8	_i8RoomSlotIdx;					// 룸 슬롯 인덱스. 매치팀 슬롯 인덱스 아님. 음수이면 문제 없는 유저.
	UINT32	_ui32Killcount;					// 킬
	UINT32	_ui32Death;						// 데쓰
	UINT32	_ui32Win;						// 승
	UINT32	_ui32Lose;						// 패

	CLAN_PERSONAL_MATCH_RECORD(void)
	{
		Reset();
	}

	CLAN_PERSONAL_MATCH_RECORD(const MATCHMAKING_RECORD & info)
	{
		ParseData(info);
	}

	void			Reset();
	void			ParseData(const MATCHMAKING_RECORD & info);

	INT8			getRoomSlotIdx(void)	const	{	return _i8RoomSlotIdx;  }
	UINT32			getKillCount(void)		const	{	return _ui32Killcount;	}
	UINT32			getDeathCount(void)		const	{	return _ui32Death;		}
	UINT32			getWinCount(void)		const	{	return _ui32Win;		}
	UINT32			getLoseCount(void)		const	{	return _ui32Lose;		}

};

#endif // _CLAN_STRUCT_DEF_H