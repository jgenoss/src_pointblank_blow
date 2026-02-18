#ifndef __CLANDEFINE_H__
#define __CLANDEFINE_H__

#include "i3BinListEx.h"

class CMatchTeam;

//#include "MercenaryMgr.h"

#ifdef I3_DEBUG
#define CLAN_MATCH_SOLO_PLAY				//클랜서버에서 혼자서도 가능하게 만든다.
#endif

#define MAX_RING_DB_BUFFER_SIZE				4096

#define CLAN_SECTION_SIZE					15			// 클랜목록 섹션 크기
#define MEMBER_SECTION_SIZE					14			// 멤버목록 섹션 크기
#define REQUEST_SECTION_SIZE				13			// 신청서목록 섹션 크기

#define BATTLE_ALL_TEAM_SECTION_SIZE		13			// 신청서목록 섹션 크기 ( 64를 넘을수 없음 )


#define DB_THREAD_COUNT						10

#define MAX_CONNECT_USER_NODE_COUNT			10		// 

#define	CLAN_BATTLE_EMPTY_INDEX				65535

#define EV_FAILED_CLAN( pt, ui, su )	{	SendPacket( pt, ui, su ); return;	}

#define SET_TEAM_CLAN_IDX( channel, naming )			(((channel&0x0000FFFF)<<16) | (naming&0x0000FFFF))
#define GET_TEAM_CLAN_CHANNEL( idx )					(( idx >> 16 ) & 0x0000FFFF )
#define GET_TEAM_CLAN_NAMING( idx )						( idx & 0x0000FFFF )

#define MAKE_CLANMARK_FLAG(back_image_idx, shape_image_idx, back_color_idx, shape_color_idx)			\
																			(((UINT8)back_image_idx & 0xFF) << 24 | \
																			((UINT8)shape_image_idx & 0xFF) << 16 | \
																			((UINT8)back_color_idx & 0xFF) << 8 | \
																			((UINT8)shape_color_idx & 0xFF))

#define MEM_ALLOC( valu, type, size )				{valu = (type*)i3MemAlloc( sizeof(type)*size ); i3mem::FillZero( valu, sizeof(type)*size );}
#define MEM_ALLOC_LOOP( valu, type, size1, size2 )	for( UINT32 idx = 0 ; idx < size1 ; idx++ ) { valu[idx] = (type*)i3MemAlloc( sizeof(type)*size2 ); i3mem::FillZero( valu[idx], sizeof(type)*size2 ); }

enum ENUM_RING_IN_DB
{
	RING_DB_NONE	= 0,

	RING_DB_USER_LOGIN,

	RING_DB_CREATE,
	RING_DB_DEATROY,

	RING_DB_REQUEST_JOIN,
	RING_DB_REQUEST_ACCEPT,
	RING_DB_REQUEST_CANCEL,
	RING_DB_REQUEST_DENIAL,

	RING_DB_MEMBER_SECESSION,
	RING_DB_MEMBER_DEPORTATION,

	RING_DB_INVITE,
	RING_DB_INVITE_ACCEPT,

	RING_DB_COMMISSION_MASTER,
	RING_DB_COMMISSION_STAFF,
	RING_DB_COMMISSION_REGULAR,

	RING_DB_GET_MEMBER_INFO,
	RING_DB_GET_REQUEST_INFO,

	RING_DB_REPLACE_NOTICE,
	RING_DB_REPLACE_INTRO,
	RING_DB_REPLACE_CLANNAME,
	RING_DB_REPLACE_MARK,
	RING_DB_REPLACE_PERSONMAX,
	RING_DB_REPLACE_MANAGEMENT,

	RING_DB_REPLACE_MEMBER_NICK,
	RING_DB_REPLACE_MEMBER_NICKCOLOR,
	RING_DB_REPLACE_MEMBER_RANK,

	RING_DB_CHECK_NAME,
	RING_DB_CHECK_AZIT,

	RING_DB_ITEM_PERIOD,
	RING_DB_ITEM_RESETRECORD,
	RING_DB_ITEM_RESETPOINT,
	RING_DB_ITEM_DELETE,

	RING_DB_SET_ACTIVE_STATE,

	RING_DB_BATTLE_RESULT_CLAN,
	RING_DB_BATTLE_RESULT_USER,

	RING_DB_MEMBER_ORDER,

	RING_DB_CLAN_FIND_USER_INFO,

	RING_DB_MATCH_RESULT_SAVE,
	RING_DB_MATCH_RESULT_EMOTION_SAVE,
	RING_DB_REQUEST_AUTO_OPTION,

	RING_DB_UPDATE_CLAN_MATCH_RECORD,
	RING_DB_UPDATE_MERCENARY_MATCH_RECORD,
	RING_DB_UPDATE_MERCENARY_DISCONNECT,

	RING_DB_LOAD_MERCENARY,
	RING_DB_LOAD_MERCENARY_PENALTY_TIME,
};

#pragma pack( push, 1 )

typedef struct __tagLogSendUseData
{	
	UINT32	_wServerID;
	T_UID	_i64UID;
	char	_szNick[ NET_NICK_NAME_SIZE ];
}LOG_SEND_USERDATA;

typedef struct __tag_LogSendMatchData
{
	UINT32	_WinClanIdx;						//이긴 클랜 Idx
	UINT8	_Rank[TEAM_COUNT];					//Rank
	UINT32	_Exp[TEAM_COUNT];					//경험치
	UINT32	_Point[TEAM_COUNT];					//포인트
	INT32	_ExpInc[TEAM_COUNT];				//경험치증가값
	INT32	_PointInc[TEAM_COUNT];				//포인트증가값
}LOG_SEND_MATCHDATA;

// 멤버 목록 정보
typedef struct _tag_MemberContextServer
{
	UINT8	_totalCount;						// 전체 클랜원 수
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} MEMBER_CONTEXT_SERVER;

typedef struct _tag_RequestContextServer
{
	UINT8	_totalCount;						// 전체 신청서 수
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} REQUEST_CONTEXT_SERVER;

// 클랜 매치 결과 목록 정보
struct MATCH_RESULT_CONTEXT_SERVER
{
	UINT8	_totalCount;						// 전체 클랜 매치 결과 수
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전

	UINT8	_ui8LastIdx;						// 마지막 경기 결과 저장 위치
};

// 클랜 상세 정보
typedef struct _tag_ClanDetailInfoServer
{
	CLAN_DETAIL_INFO			_ClanDetailInfo;					// 클랜 상세 정보
	CLAN_TOP_MEMBER				_ClanTopMember;						// 클랜 우수 회원

																	// Server
	UINT32						_statedate;							// 상태변화날짜

	UINT32						_ItemExpUpDate;						// 클랜 경험치 증가 유효 날짜.
	UINT32						_ItemColorNameDate;					// 클랜 칼라 유효 날짜.	

	BOOL						_bClanOrder;						// 클랜 맴버 관련 데이타 로딩 완료했는지.

	MEMBER_CONTEXT_SERVER 		_stMemberContext;					// 클랜 맴버 목록 정보
	REQUEST_CONTEXT_SERVER		_stRequestContext;					// 클랜 가입 신정 목록 정보

																	//MATCH_RESULT_CONTEXT_SERVER	_stMatchResultContext;				// 클랜 매치 결과 목록 정보	

	i3BinListEx64*				_pMember;							// 클랜원 정보
	i3BinListEx64*				_pRequest;							// 가입자 정보
																	//i3BinListEx64*				_pMatchResult;						// 클랜 매치 결과 정보

	CLAN_WAR_RESULT				m_ClanWarResult[MAX_MATCH_RESULT_COUNT];		// 클랜 매치 메이킹 결과 정보. 최근 15개. 
	T_UID						m_aClanWarPlayerUID[CLAN_TEAM_MEMBER_COUNT];	// 용병을 제외한 클랜 매치 메이킹 참여인원.  최근 1개.

	UINT8						m_ui8MatchTeamCount;							// 매치팀 카운트.
	CMatchTeam*					m_pMatchTeamList[MAX_CLAN_MATCH_TEAM_COUNT];	// 매치팀 리스트.

	_tag_ClanDetailInfoServer::_tag_ClanDetailInfoServer() { Reset(); }

	void						Reset();
	void						SetClanListBaiscInfo(CLAN_LIST_BASIC_INFO* pClanListBasicInfo);

	INT8						RegisterMatchTeam(CMatchTeam* pMatchTeam);
	void						ReleaseMatchTeam(INT8 i8Idx);

	UINT8						GetMatchTeamCount(void) { return m_ui8MatchTeamCount; }
	CMatchTeam*					GetMatchTeam(INT32 i32Idx)
	{
		if (i32Idx < 0 || i32Idx >= MAX_CLAN_MATCH_TEAM_COUNT) return NULL;
		return m_pMatchTeamList[i32Idx];
	}

	BOOL						HaveMatchTeam(void) { return 0 == m_ui8MatchTeamCount ? FALSE : TRUE; }
	INT32						FindNewMatchArrIdx(void);
	void						InsertClanWarResult(CLAN_WAR_RESULT *pClanWarResult);


} CLAN_DETAIL_INFO_SERVER;

inline void CLAN_DETAIL_INFO_SERVER::Reset()
{
	_ClanDetailInfo.Reset();
	_ClanTopMember.Reset();

	_statedate = 0;

	_ItemExpUpDate = 0;
	_ItemColorNameDate = 0;

	_bClanOrder = FALSE;

	i3mem::FillZero(&_stMemberContext, sizeof(MEMBER_CONTEXT_SERVER));
	i3mem::FillZero(&_stRequestContext, sizeof(REQUEST_CONTEXT_SERVER));
	//	i3mem::FillZero( &_stMatchResultContext,	sizeof(MATCH_RESULT_CONTEXT_SERVER) );
	//	i3mem::FillZero( &_stMatchResultContext,	sizeof(MATCH_RESULT_CONTEXT_SERVER) );

	i3mem::Fill(m_pMatchTeamList, NULL, sizeof(CMatchTeam*) * MAX_CLAN_MATCH_TEAM_COUNT);

	i3mem::FillZero(m_ClanWarResult, sizeof(CLAN_WAR_RESULT) * MAX_MATCH_RESULT_COUNT);
	i3mem::FillZero(m_aClanWarPlayerUID, sizeof(T_UID) * CLAN_TEAM_MEMBER_COUNT);
}

inline void CLAN_DETAIL_INFO_SERVER::SetClanListBaiscInfo(CLAN_LIST_BASIC_INFO* pClanListBasicInfo)
{
	pClanListBasicInfo->m_ClanBasicInfo._idx		= _ClanDetailInfo.m_ui32CID;
	i3String::Copy( pClanListBasicInfo->m_ClanBasicInfo._name, _ClanDetailInfo.m_name, NET_CLAN_NAME_SIZE);
	pClanListBasicInfo->m_ClanBasicInfo._name[NET_CLAN_NAME_SIZE-1] = 0;
	pClanListBasicInfo->m_ClanBasicInfo._rank		= _ClanDetailInfo.m_expRank;
	pClanListBasicInfo->m_ClanBasicInfo._person		= _ClanDetailInfo.m_currentPerson;
	pClanListBasicInfo->m_ClanBasicInfo._maxperson	= _ClanDetailInfo.m_maxPerson;
	pClanListBasicInfo->m_ClanBasicInfo._date		= _ClanDetailInfo.m_date;
	pClanListBasicInfo->m_ClanBasicInfo._mark		= _ClanDetailInfo.m_mark;
	pClanListBasicInfo->m_ClanBasicInfo._color		= _ClanDetailInfo.m_color;

	pClanListBasicInfo->m_ClanBasicRecord.m_match		= _ClanDetailInfo.m_match;
	pClanListBasicInfo->m_ClanBasicRecord.m_win			= _ClanDetailInfo.m_win;
	pClanListBasicInfo->m_ClanBasicRecord.m_lose		= _ClanDetailInfo.m_lose;
	pClanListBasicInfo->m_ClanBasicRecord.m_totalWin	= _ClanDetailInfo.m_Totalwin;
	
	pClanListBasicInfo->m_ClanBsicSortInfo.m_clanpoint				= _ClanDetailInfo.GetClanPoint();
	pClanListBasicInfo->m_ClanBsicSortInfo.m_Preclanpoint			= _ClanDetailInfo.GetClanPoint(SEASON_TYPE_PRE);
	//pClanListBasicInfo->m_ClanBsicSortInfo.m_ui8ConsecutiveWinCnt	= _ClanDetailInfo.m_ui8ConsecutiveWinCnt;

	pClanListBasicInfo->m_ui16introsize				= (UINT16)i3String::Length(_ClanDetailInfo.m_intro);
	i3String::Copy( pClanListBasicInfo->m_intro, _ClanDetailInfo.m_intro, NET_CLAN_INTRO_SIZE);
	pClanListBasicInfo->m_intro[NET_CLAN_INTRO_SIZE-1] = 0;
}

inline INT8 CLAN_DETAIL_INFO_SERVER::RegisterMatchTeam( CMatchTeam* pMatchTeam )
{
	if( m_ui8MatchTeamCount >= MAX_CLAN_MATCH_TEAM_COUNT ) return -1;

	for( INT32 i = 0 ; i < MAX_CLAN_MATCH_TEAM_COUNT ; ++i )
	{
		if( m_pMatchTeamList[i] ) continue;

		m_pMatchTeamList[i] = pMatchTeam;
		++m_ui8MatchTeamCount;

		return (INT8)i;
	}
	return -1;
}


inline void CLAN_DETAIL_INFO_SERVER::ReleaseMatchTeam( INT8 i8Idx )  
{
	if( i8Idx < 0 ) return;
	if( i8Idx >= MAX_CLAN_MATCH_TEAM_COUNT ) return; 
	if( !m_pMatchTeamList[i8Idx] ) return;

	m_pMatchTeamList[i8Idx] = NULL;

	if( 0 < m_ui8MatchTeamCount ) 
	{
		--m_ui8MatchTeamCount;
	}

	return;
} 

inline void CLAN_DETAIL_INFO_SERVER::InsertClanWarResult(CLAN_WAR_RESULT *pClanWarResult)
{
	INT32 i32ArrIdx = 0;
	INT32 i32OldMatchIdx = 0x7FFFFFFF;	// 가장 오래된 매치 인덱스.
	for (INT32 i = 0; i < MAX_MATCH_RESULT_COUNT; ++i)
	{
		if (m_ClanWarResult[i].m_i32MatchIdx < i32OldMatchIdx)
		{
			i32OldMatchIdx = m_ClanWarResult[i].m_i32MatchIdx;
			i32ArrIdx = i;
		}
	}

	i3mem::Copy(&m_ClanWarResult[i32ArrIdx], pClanWarResult, sizeof(CLAN_WAR_RESULT));

	return;
}

inline INT32 CLAN_DETAIL_INFO_SERVER::FindNewMatchArrIdx(void)
{
	INT32 i32ArrIdx = 0;
	INT32 i32NewMatchIdx = 0;	// 가장 최신 매치 인덱스
	for (INT32 i = 0; i < MAX_MATCH_RESULT_COUNT; ++i)
	{
		if (m_ClanWarResult[i].m_i32MatchIdx > i32NewMatchIdx)
		{
			i32NewMatchIdx = m_ClanWarResult[i].m_i32MatchIdx;
			i32ArrIdx = i;
		}
	}
	return i32ArrIdx;
}

typedef struct _tag_ClanRequestServerInfo
{
	//UINT32		_idx;								// 신청서 인덱스
	T_UID		_i64UID;							// 신청서 dbIdx(UID)
	TTCHAR		_nick[NET_NICK_NAME_SIZE];			// 신청인 닉네임
	UINT8		_rank;								// 신청인 등급
	UINT8		_ui8NickColor;						// 신청인 닉네임컬러
	UINT32		_date;								// 신청일

	UINT32		_ui32Kill;								// 신청인 킬수
	UINT32		_ui32Death;							// 신청인 데스수
	UINT32		_ui32Match;							// 신청인 전수
	UINT32		_ui32Win;								// 신청인 승수
	UINT32		_ui32Lose;								// 신청인 패수
	TTCHAR		_strContents[NET_CLAN_REQUEST_SIZE];	// 신청서 내용

	UINT64		_state;								// 현재 위치
	INT32		_sessionIdx;						// Session
	UINT32		_ui32Assist;						// 신청인 어시스트수

	_tag_ClanRequestServerInfo::_tag_ClanRequestServerInfo()
	{
		Reset();
	};

	void	Reset(void)
	{
		_i64UID		= 0;
		_nick[0]	= '\0';
		_rank		= 0;
		_date		= 0;
		_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_OFFLINE, 0, 0, 0 );
		_sessionIdx = -1;

		_ui32Kill			= 0;			// 신청인 킬수
		_ui32Death			= 0;			// 신청인 데스수
		_ui32Match			= 0;			// 신청인 전수
		_ui32Win			= 0;			// 신청인 승수
		_ui32Lose			= 0;			// 신청인 패수
		_strContents[0]	= '\0';			// 신청서 내용
		_ui8NickColor		= 0;
		_ui32Assist = 0;			// 신청인 어시스트수
	}

}CLAN_REQUEST_SERVER_INFO;

typedef struct __tagClanCSCreateInfo
{
	UINT8	wChannelNum;
	INT32	nRoomIdx;
	UINT32	wMark;
	UINT32	ui32Rank;
	UINT8	m_ui8MemberCount;
	TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
	TTCHAR	szClanName[NET_CLAN_NAME_SIZE];
	TTCHAR	szClanIntro[NET_CLAN_INTRO_SIZE];
	TTCHAR	szAzitURL[NET_CLAN_AZIT_URL_SIZE];
	UINT8	m_ui8NickColor;
} CLAN_CS_CREATE_INFO;

typedef struct __tagClanCSJoinInfo
{
	UINT8	wChannelNum;
	INT32	nRoomIdx;
	TTCHAR	szContents[NET_CLAN_REQUEST_SIZE];
	UINT8	wAge;
	UINT8	wRank;
	UINT8	wViewRank;
	TTCHAR	strNick[ NET_NICK_NAME_SIZE ];
	UINT8	ui8NickColor;

	UINT32	_ui32Killcount;
	UINT32	_ui32Death;
	UINT32	_ui32Match;
	UINT32	_ui32Win;
	UINT32	_ui32Lose;
	UINT32	_ui32AssistCount;
} CLAN_CS_JOIN_INFO;

typedef struct __tagBattleTeamList
{	
} BATTLE_TEAM_LIST;							// 만들어진 팀 리스트

typedef struct __tagBattleTeamContext
{
	UINT8	_cChannel;						// Channel Idx

	UINT16	_sTotalCount;					// 총 갯수
	UINT16	_sSectionCount;					// Section 갯수

	UINT16	_sStartIdx;						// 리스트 시작 위치 인덱스
	UINT16	_sEndIdx;						// 마지막 저장된 리스트 위치 인덱스

} BATTLE_TEAM_CONTEXT;						// 만들어진 팀 Context

typedef struct __tagBattleTeamClanContext
{
	UINT8	_cTotalcount;					// 총 갯수
	UINT8	_cSectionCount;					// Section 갯수

	UINT32	_wStartIdx;						// 리스트 시작 위치 인덱스
	UINT32	_wEndIdx;						// 마지막 저장된 리스트 위치 인덱스

} BATTLE_TEAM_CLAN_CONTEXT;					// 클랜에 만들어진 팀 리스트

typedef struct __tagBattleClanContext
{
	UINT8	_ui8Totalcount;					// 총 갯수
	UINT8	_ui8SectionCount;				// Section 갯수
} BATTLE_CLAN_CONTEXT;						// 클랜에 만들어진 팀 리스트

typedef struct __tagBattleChannelContext
{
	UINT16	_ui16Totalcount;				// 총 갯수
	UINT16	_ui16SectionCount;				// Section 갯수
} BATTLE_CHANNEL_CONTEXT;

typedef struct __tagBattleTeamInfo
{
	CLAN_BATTLE_TEAM	_Team;
	CLAN_BATTLE_CLAN	_Clan;
	CLAN_BATTLE_MEMBER	_Member[ 8 ];

	UINT16				_sFront;			// 보통 리스트 팀 Front
	UINT16				_sNext;				// 보통 리스트 팀 Next

	UINT32				_wClanFront;		// 리스트 팀 Front
	UINT32				_wClanNext;			// 리스트 팀 Next

} BATTLE_TEAM_INFO;							// 실제 데이터


typedef struct __tagBattleInfo
{
	UINT32				_ui32Idx;
	CLAN_BATTLE_TEAM	_Team;
	CLAN_BATTLE_CLAN	_Clan;
	CLAN_BATTLE_MEMBER	_Member[ 8 ];

 	INT32	GetMemberCount()				{ return _Team._PerCount; }
 	INT32	GetMaxMemberCount()				{ return _Team._PerMax; }

	void	MakePacketMemberInfo(i3NetworkPacket* pPacket)
	{
		pPacket->WriteData(&_Team, sizeof(_Team));

		for( INT32 i = 0; i < GetMaxMemberCount(); ++i )
		{
			if( _Member[i]._State == CLAN_ROOM_EMPTY )
				continue;

			pPacket->WriteData(&_Member[i], sizeof(_Member[i]));
		}
	}
} BATTLE_INFO;

typedef struct __tagBattleClanInfo
{
	UINT32				_ui32CID;
	BATTLE_CLAN_CONTEXT	_stClanContext;
	i3BinList*			_pClanList;
} BATTLE_CLAN_INFO;

#define BATTLE_CHANNEL_NAMING		65535
typedef struct __tagBattleChannelInfo
{
	UINT32					_ui32Channel;
	BATTLE_CHANNEL_CONTEXT	_stChannelContext;
	UINT8					_ui8Naming[ BATTLE_CHANNEL_NAMING ];
	i3BinList*				_pChannelList;
} BATTLE_CHANNE_INFO;

typedef struct __tagClanrequestloginInfo
{
	UINT32		_state;
	INT32		_sessionIdx;

} CLAN_REQUEST_LOGIN_INFO;

typedef struct __tagClanuserinfo
{
	UINT32		ui32ServerIdx;
	INT32		i32SessionIdx;
	T_UID		i64UID;
	UINT32		ui32ClanDBIdx;
	//UINT8		ui8Rank;

} CLAN_USER_INFO;

struct CLAN_WAR_INFO : public CLAN_USER_INFO
{
	T_TeamID	m_TTeamID;
	T_MerID		m_TMerID;
};

typedef struct __tagRingInDB
{
	UINT8		_ui8Type;

	UINT32		_ui32ServerIdx;
	INT32		_i32SessionIdx;
	T_UID		_i64UID;
	UINT8		_ui8Rank;
	UINT32		_ui32ClanDBIdx;

	UINT16		_ui16BFSize;
	char		_bBuffer[ MAX_RING_DB_BUFFER_SIZE ];

	__tagRingInDB::__tagRingInDB()
	{
		_ui16BFSize	= 0;
	};
	void	Reset()
	{
		_ui16BFSize	= 0;
	};
	void	Write( void* pBuffer, UINT16 ui16Size )
	{
		if( _ui16BFSize + ui16Size > MAX_RING_DB_BUFFER_SIZE ) return;

		i3mem::Copy( &_bBuffer[ _ui16BFSize ], pBuffer, ui16Size );
		_ui16BFSize += ui16Size;
	};
	void	Read( void* pBuffer, UINT16 ui16Size )
	{
		if( _ui16BFSize + ui16Size > MAX_RING_DB_BUFFER_SIZE ) return;

		i3mem::Copy( pBuffer, &_bBuffer[ _ui16BFSize ], ui16Size );
		_ui16BFSize += ui16Size;
	};
}RING_IN_DB;

typedef struct __tagRingOutDB
{
	UINT8		_ui8Type;

	UINT32		_ui32ServerIdx;
	INT32		_i32SessionIdx;
	T_UID		_i64UID;
	UINT32		_ui32ClanDBIdx;
	INT32		_i32Result;

	UINT16		_ui16BFSize;
	char		_bBuffer[ MAX_RING_DB_BUFFER_SIZE ];	

	void	Copy( __tagRingInDB* pInDB )
	{
		_ui8Type		= pInDB->_ui8Type;
		_ui32ServerIdx	= pInDB->_ui32ServerIdx;
		_i32SessionIdx	= pInDB->_i32SessionIdx;
		_i64UID			= pInDB->_i64UID;
		_ui32ClanDBIdx	= pInDB->_ui32ClanDBIdx;
	};

	void	Reset()
	{
		_ui16BFSize	= 0;
	};
	void	Write( void* pBuffer, UINT16 ui16Size )
	{
		if( _ui16BFSize + ui16Size > MAX_RING_DB_BUFFER_SIZE ) return;

		i3mem::Copy( &_bBuffer[ _ui16BFSize ], pBuffer, ui16Size );
		_ui16BFSize += ui16Size;
	};
	void	Read( void* pBuffer, UINT16 ui16Size )
	{
		if( _ui16BFSize + ui16Size > MAX_RING_DB_BUFFER_SIZE ) return;

		i3mem::Copy( pBuffer, &_bBuffer[ _ui16BFSize ], ui16Size );
		_ui16BFSize += ui16Size;
	};

}RING_OUT_DB;

#pragma pack( pop)

#endif