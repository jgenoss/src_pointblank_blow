#pragma once



class CMatchTeam;
 
#pragma pack(push, 1)


struct ENTER_CHANNEL
{
	INT64			m_i64UserID;
	INT32			m_i32ClanID;
	INT32			m_i32Channel;
	INT32			m_i32Session;
	INT32			m_i32KillCount;
	INT32			m_i32DeathCount;

	INT32			GetKillDeathWeight()		
	{
		if( m_i32KillCount + m_i32DeathCount <= 0 ) return 0;
		return m_i32KillCount / (m_i32KillCount + m_i32DeathCount);
	}
};
#pragma pack(pop)


// 용병 랭크별 전적 최대치
enum MERCENARY_RANKUP_MATCH
{
	MER_RANK_0		= 2,		// 0~1
	MER_RANK_1		= 6,		// 2~5
	MER_RANK_2		= 11,		// 6~10
	MER_RANK_3		= 51,		// 11~50
	MER_RANK_4		= 101,		// 51~100
	MER_RANK_5		= 501,		// 101~500
	MER_RANK_6		= 1001,		// 501~1000
	MER_RANK_7		= 5001,		// 1001~5000
	MER_RANK_8		= 10001,	// 5001~10000
	//MER_RANK_9		= 0xFFFF,	// 10001 이상
	
	MER_RANK_COUNT	= 9,	
};


//////////////////////////////////////////////////////////////////////////
// CMercenary
class CMercenary
{
public:
	T_RESULT			Invite(UINT32 ui32ServerIdx, CMatchTeam* pMatchTeam);

	T_MerID				GetMerID()										{ return m_stBaseInfo.m_TMercenaryID; }
	//INT32				GetKillDeathRate()								{ return m_i32KillDeathRate; }
	TTCHAR*				GetNickname()									{ return m_stBaseInfo.m_strNick; }
	T_UID				GetUserID()										{ return m_TUID; }
	INT32				GetSessionIdx()									{ return m_i32SessionIdx; }
	INT32				GetMerClass()									{ return m_stBaseInfo.m_ui8Class; }
	INT32				GetMerRank()									{ return m_stBaseInfo.m_ui8MerRank; }
	void				SetMerRank(UINT8 ui8MerRank)					{ m_stBaseInfo.m_ui8MerRank = ui8MerRank; }
	UINT8				GetNickColor()									{ return m_stBaseInfo.m_ui8NickColor; }
	MERCENARY_STATE		GetState()										{ return (MERCENARY_STATE)m_stBaseInfo.m_ui8State; }
	MERCENARY_INFO*		GetBaseInfo()									{ return &m_stBaseInfo; }
	T_TeamID			GetInviteTeamID()								{ return m_TInviteTeamID; }
	UINT8				CheckMerRank();
	void				GetRecordInfo(MERCENARY_RECORD& Record)			{ Record = m_stRecord ; }



	void				ChangeMemberInfo(CHANGE_INFO	eType, MATCH_MEMBER_INFO* pMemberInfo);		
 
	
	void				SetInviteTeamID(T_TeamID TInviteTeamID)			{ m_TInviteTeamID = TInviteTeamID; }
	void				SetState(MERCENARY_STATE eState )				{ m_stBaseInfo.m_ui8State = (UINT8)eState; }

	
	void				Remove();
	bool				SetMercenaryRecord(MERCENARY_RECORD* pMercenaryInfo);

	void				Resist(T_MerID TMerID, REGIST_MERCENARY* pMercenaryInfo, CLAN_USER_INFO *pUserInfo);

	void				MakePacketMercenary(i3NetworkPacket* pPacket);
	void				GetDetailInfo(MERCENARY_DETAIL_INFO& DetailInfo);

	void				AddRecord(CLAN_RESULT_USER* pResult);

	bool				GetSendMerRankUp(void)				{ return	m_bSendMerRankUp; }
	void				SetSendMerRankUp(bool bMerRankUp)	{ m_bSendMerRankUp = bMerRankUp; }

public:
	CMercenary();
	~CMercenary();


private:
	static const INT32	m_ai32MatchByRank[MER_RANK_COUNT];


private:

	T_UID				m_TUID;
	INT32				m_i32SessionIdx;		// Game서버 세션인덱스
	MERCENARY_INFO		m_stBaseInfo;
	bool				m_bSendMerRankUp;		// 용병 랭크 업되었는지.

	/////////////////////////////////////////////////////
	//////////////	용병전적
	MERCENARY_RECORD	m_stRecord;			// 용병전적
	T_TeamID			m_TInviteTeamID;
};



//////////////////////////////////////////////////////////////////////////
// CMercenaryMgr
class CMercenaryManager
{
public:

	T_RESULT			InviteMercenary(UINT32 ui32ServerIdx, T_MerID TMerID, CMatchTeam* pMatchTeam);
	BOOL				RemoveMercenary(T_MerID TMerID);

	CMercenary*			SetMercenaryRecord(T_MerID TMerID, MERCENARY_RECORD* pMercenaryInfo);
	CMercenary*			FindMercenary(T_UID TUID);
	CMercenary*			FindMercenary(T_MerID TMerID);

	void				SendMercenaryList(CLAN_USER_INFO* pUserInfo);

	T_RESULT			RegistMercenary(REGIST_MERCENARY* pMercenaryInfo, CLAN_USER_INFO* pUserInfo );

	void				ChangeMemberInfo(CHANGE_INFO	eType, CLAN_WAR_INFO *pUserInfo, MATCH_MEMBER_INFO* pMemberInfo);

	BOOL				GetDetailInfo(T_MerID TMerID, MERCENARY_DETAIL_INFO& DetailInfo);

	void				ReleaseAllMercenary();
private:
	T_MerID				GetEmptyBuffer();
	INT32				GetMercenaryCount(INT32 i32MultiState = 0xFF);

	bool				CheckValidMercenary(T_MerID TMerID)		
	{
		if( TMerID >= MAX_MERCENARY_COUNT ) return false;

		return true;
	}


public:
	CMercenaryManager();
	~CMercenaryManager();


private:
	CMercenary			m_Mercenary[MAX_MERCENARY_COUNT];
};

