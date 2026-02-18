#pragma once

class NSM_CRingBuffer;
class CMatchTeam;


#define MAX_CLAN_MATCH_OPPONENT		10

enum MATCH_STATE
{
	MATCH_NONE		= 0,
	MATCH_MATCHED	,	// 매칭된 상태
	MATCH_READY		,	// 레디룸 생성 대기 상태
};

struct MATCHING_REQUEST
{
	T_TeamID		m_TTeamID;
	UINT32			m_ui32ClanID;
	UINT8			m_ui8MaxMember;
	T_TeamID		m_TMatchedTeamID;
	INT32			m_i32MatchingPoint;
	DATE32			m_dt32MatchLimitTime; 
};

struct MATCHING_RESULT
{
	MATCHING_RESULT();

	T_RESULT		m_TResult;
	UINT8			m_ui8TeamCount;
	T_TeamID		m_TTeamID[TEAM_COUNT];
};

inline MATCHING_RESULT::MATCHING_RESULT()
{
	m_TResult		= EVENT_ERROR_SUCCESS;
	m_ui8TeamCount	= TEAM_COUNT;
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
		m_TTeamID[i] = MATCH_TEAM_INVALID_IDX;
}



//////////////////////////////////////////////////////////////////////////
// CMatching
class CMatching
{
public:
	bool			SetMatchTeam(TEAM_TYPE eTeamType, CMatchTeam* pMatchTeam);
	void			SetRoom(INT32 i32ChannelIndex, INT32 i32RoomIndex)				{ m_i32ChannelIndex = i32ChannelIndex; m_i32RoomIndex = i32RoomIndex; m_eMatchState = MATCH_READY; }
	void			SetMaxPlayer(UINT8 ui8MaxPlayer)								{ m_ui8MaxPlayer = ui8MaxPlayer; }

	void			SetMatchingID(T_MatchingID TMatchingID)							{ m_TMatchingID = TMatchingID; }
	T_MatchingID	GetMatchingID()													{ return m_TMatchingID; }

	T_RESULT		LeaveMatchRoom(T_TeamID TTeamID, CLAN_USER_INFO* pUserInfo, INT8& i8LeaveSlot, UINT8 ui8RoomState, bool bForceLeave  );

	bool			MakePacketJoinRoom(i3NetworkPacket* pPacket);
	void			ApplyMatchResult(CLAN_RESULT* aResult, UINT32 ui32ServerIdx, UINT32 ui32StageID, UINT8 ui8TotalPlayer, CLAN_RESULT_USER* pPlayer);

	void			ResetMatching();
	
	MATCH_STATE		GetState()														{ return m_eMatchState; }
	void			SetState(MATCH_STATE eState)									{ m_eMatchState = eState; }

	bool			SendCreateRoom(INT32 i32ServerIdx );	 

	INT32			GetChannaelIdx()												{ return m_i32ChannelIndex; }	// 매치 룸의 채널인덱스 얻기.
	T_TeamID		GetTeamID(TEAM_TYPE eTeamType);								
	
	void			SimulateRating(TEAM_TYPE eWinTeam, double *pResultClanPoint );

private:
	void			CalculateRating(CLAN_RESULT* aResult);
	


public:
	CMatching();
	~CMatching();


private:
	MATCH_STATE		m_eMatchState;

	T_MatchingID	m_TMatchingID;
	INT32			m_i32ChannelIndex;
	INT32			m_i32RoomIndex;
	UINT8			m_ui8MaxPlayer;		// 한팀기준 

	CMatchTeam*		m_pMatchTeams[TEAM_COUNT];
	CRating			m_Ratings[TEAM_COUNT][MAX_MEMBER_COUNT];
};



//////////////////////////////////////////////////////////////////////////
// CMatchingManager
class CMatchingManager : public i3Thread
{
	I3_CLASS_DEFINE(CMatchingManager);

public:
	bool				IsValidHeadOfTeam(T_MatchingID TMatchingID, INT64 i64HeadUID);

	bool				Create(INT32 i32ServerCount);
	void				Destroy();
	virtual UINT32		OnRunning(void* pData);

	T_RESULT			InsertMatchRequestBuffer(INT32 i32ServerIdx, MATCHING_REQUEST& pMatchTeam);

	INT32				GetMatchingCount(INT32 i32ServerIdx);
	MATCHING_RESULT*	GetMatchingResult(INT32 i32ServerIdx);
	void				PopMatchingIndex(INT32 I32ServerIdx);
	
private:
	void				_RemoveMatchRequest(UINT32 ui32ServerIdx);
	void				_InsertMatchRequest(UINT32 ui32ServerIdx);
	void				_Matchmaking(UINT32 ui32ServerIdx);
	void				_ChangeTeamState(UINT32 ui32ServerIdx, T_TeamID TTeamID, TEAM_STATE eStaate);
	CMatchTeam*			FindMatchTeam(UINT32 ui32ServerIdx, T_TeamID TTeamID);

public:
	CMatchingManager();
	virtual ~CMatchingManager();


private:
	bool				m_bRunning;
	INT32				m_i32ServerCount;
	
	UINT32				m_ui32UpdateTime;				// 매치메이킹 업데이트 주기. (ms)
	UINT32				m_ui32LastUpdateServerIdx;		// 마지막으로 매치메이킹 된 서버

	// MATCHING_REQUEST
	NSM_CRingBuffer*	m_pRequestBuffer[MAX_GAME_SERVER_COUNT];		// IN BUFFER - Task 쓰레드에서 입력받기 위한 버퍼
	i3BinList			m_MatchRequest[MAX_GAME_SERVER_COUNT][BUFFER_CNT_PER_MEMBER];	// FOR SEARCH - 이 쓰레드에서 인원수별 매칭을 위한 버퍼
	NSM_CRingBuffer*	m_pResultBuffer[MAX_GAME_SERVER_COUNT];			// OUT BUFFER - Task 쓰레드에서 Game서버로 매칭된 정보 보내기 위한 버퍼.
	INT32				m_i32LogIdx;
};

extern CMatchingManager* g_pMatchingManager;
