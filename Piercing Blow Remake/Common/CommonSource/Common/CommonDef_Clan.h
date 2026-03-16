#ifndef __COMMONDEF_CLAN_H__
#define __COMMONDEF_CLAN_H__

#include "../ClanMatch//Rating.h"

// Clan
//////////////////////////////////////////////////////////////////////////
// Define
//////////////////////////////////////////////////////////////////////////
#define CLAN_MIN_POINT			0.9f		// 클랜 포인트 하한선
#define CLAN_MAX_POINT			9999.f		// 클랜 포인트 상한선

#define BASIC_MEMBER_COUNT			50			// 최초 클랜원 수
#define INCREASE_MEMBER_COUNT		50			// 추가 클랜원 수
#define MAX_MEMBER_COUNT			250			// 최대 클랜원 수
#define MAX_REQUEST_COUNT			100			// 최대 가입신청서 수
#define MAX_MATCH_RESULT_COUNT		15			// 최대 클랜 매치 결과 수
#define MATCH_RESULT_SECTION_SIZE	10			// 클랜 매치 결과 목록 섹션 크기
#define MAX_LMRC					3			// 최근 클랜 매치 결과 집계수

#define MAX_BATTLE_CLAN_ROOM_COUNT	250			// 각 클랜에서 만들수 있는 최대 전투방 개수

#define MAX_CLAN_MATCH_TEAM_COUNT	26			// 각 클랜당 클랜 매치팀 최대 생성 갯수.

#define NET_CLAN_NOTICE_SIZE		255			// 클랜 공지
#define NET_CLAN_INTRO_SIZE			255			// 클랜 소개
#define NET_CLAN_REQUEST_SIZE		255			// 신청서 내용
#define NET_CLAN_AZIT_URL_SIZE		21			// 아지트 URL

#define BATTLE_TEAM_SECTION_SIZE		13			// 신청서목록 섹션 크기 ( 64를 넘을수 없음 )

#define MAX_SEASON_RANKING_TEAM_COUNT	3			// 시즌 랭킹 팀 갯수. ( 이전 시즌 1~3위 팝업 출력용 )


#define CLAN_TEAM_MEMBER_COUNT				8			// 클랜 매치메이킹 최대인원수
#define MERCENARY_REREGISTRATION_TIME		60		// 테스트 60		// 용병 이탈시 재등록 제한시간(Min)

// 클랜 스탭 권한
#define CLAN_MASTER_AUTH			9			// 기본 스태프 권한 9 = 1001

#define MAX_MATCH_TEAM_COUNT		5000
#define MATCH_TEAM_INVALID_IDX		0xFFFF		// 매치팀 ID 초기값.
#define MERCENARY_INVALID_IDX		0xFFFF		// 용병 ID 초기값

#define MERCENARY_MIN_KILL_DEATH_RATE	0.50f // 용병 등록 최소 킬뎃률( 50% )

#define MAX_MERCENARY_COUNT			1000			// 용병 최대등록수. 1개 서버 기준.

#define MAX_MERCENARY_LIST_PER_PACKET	50			// 한 패킷당 용병 최대수( 용병 리스트 )

#define NET_CLAN_CHATING_SIZE		255			// 클랜 체팅 내용

// 클랜 스탭 권한
#define CLAN_MASTER_AUTH			9			// 기본 스태프 권한 9 = 1001
#define CLAN_STAFF_AUTH_MASTER		0xFF		// 클랜 마스터 권한
#define CLAN_STAFF_AUTH_NONE		0x00		// 스탭 권한 없음
#define CLAN_STAFF_AUTH_JOIN		0x01		// 가입신청서 관리 권한
#define CLAN_STAFF_AUTH_EJECT		0x02		// 멤버 제명 권한
#define CLAN_STAFF_AUTH_INVITE		0x04		// 클랜 초대 권한
#define CLAN_STAFF_AUTH_BOARD		0x08		// 게시판 관리 권한

//////////////////////////////////////////////////////////////////////////
#define MAX_CLAN_MATCH_MEMBER	8
#define MIN_CLAN_MATCH_MEMBER	4

#define BUFFER_CNT_PER_MEMBER	(MAX_CLAN_MATCH_MEMBER - MIN_CLAN_MATCH_MEMBER) + 1

#define MAX_MERCENARY_IN_TEAM		2

//////////////////////////////////////////////////////////////////////////

#define MATCHING_TEAM_BUF_SIZE		10

#define MATCHING_LIMIT_TIME			30	// 퀵 매치 제한시간.( sec )

// 용병 등록 상태
enum MERCENARY_STATE
{
	MER_STATE_NONE			= 0,	
	MER_STATE_REGISERING	,		// 등록중
	MER_STATE_REGISTERED	,		// 등록 완료
	MER_STATE_JOIN_TEAM		,		// 매치팀 입장.

};

// Clan 감정표현
enum ENUM_CLAN_EMOTION_TYPE
{
	CLAN_EMOTION_TYPE_NONE = 0,	// 감정 표현 없음.
	CLAN_EMOTION_TYPE_LIKE,		// 좋아요
	CLAN_EMOTION_TYPE_OMG,		// 맙소사

	MAX_CLAN_EMOTION_TYPE
};

// Clan 
enum ENUM_CLAN_AUTO_OPTION
{
	CLAN_AUTO_OPTION_MANUAL = 0,	// 수동 승인
	CLAN_AUTO_OPTION_AUTO,			// 자동 승인

	MAX_CLAN_AUTO_OPTION
};

enum ENUM_CLAN_MATCH_RESULT
{
	CLAN_MATCH_RESULT_NONE = 0,
	CLAN_MATCH_RESULT_DRAW,
	CLAN_MATCH_RESULT_LOSE,
	CLAN_MATCH_RESULT_WIN,

	MAX_CLAN_MATCH_RESULT
};

enum TEAM_STATE
{
	TEAM_STATE_NONE		= 0,	// 해체
	TEAM_STATE_WAIT		,		// 대기 상태.
	TEAM_STATE_MATCHING	,		// 퀵 매치 버튼 누른 상태.	
	TEAM_STATE_MATCHED	,		// 매칭되어 대기방으로 이동된 상태.
	TEAM_STATE_CANCEL_MATCHING, // 매칭 취소 버튼 누른 상태.
	TEAM_STATE_COUNT
};


//////////////////////////////////////////////////////////////////////////
#define MAX_LATELY_MATCH_RESULT		3			// 최근 클랜전  3경기 승패
#define MAX_MATCH_RESULT			30			// 최근 클랜전 30경기 결과

#define DF_CLAN_UNIT_SQUARD			1
#define DF_CLAN_UNIT_PLATOON		10
#define DF_CLAN_UNIT_COMPANY		30
#define DF_CLAN_UNIT_BATTALION		50
#define DF_CLAN_UNIT_REGIMENT		100
#define DF_CLAN_UNIT_BRIGADE		150
#define DF_CLAN_UNIT_DIVISION		200
#define DF_CLAN_UNIT_CORPS			250

// Clan
// Write, Invited, Deportation, Accept
#define CLAN_MENAGE_AUTHORITY(wr,in,de,ac)	((wr & 0x0000000F) << 3) | ((in & 0x0000000F) << 2) | ((de & 0x0000000F) << 1) | ((ac & 0x0000000F))
#define GET_CLAN_MENAGE_WRITE(state)		((state>>3) & 0x00000001)
#define GET_CLAN_MENAGE_INVITED(state)		((state>>2) & 0x00000001)
#define GET_CLAN_MENAGE_DEPORTATION(state)	((state>>1) & 0x00000001)
#define GET_CLAN_MENAGE_ACCEPT(state)		((state)	& 0x00000001)

#define DF_CLAN_RANK_NEW	0
#define DF_CLAN_RANK_A1		24000
#define DF_CLAN_RANK_A2		64000
#define DF_CLAN_RANK_B1		112000
#define DF_CLAN_RANK_B2		168000
#define DF_CLAN_RANK_C1		240000
#define DF_CLAN_RANK_C2		320000
#define DF_CLAN_RANK_C3		440000
#define DF_CLAN_RANK_D1		600000
#define DF_CLAN_RANK_D2		720000
#define DF_CLAN_RANK_D3		920000
#define DF_CLAN_RANK_D4		1160000
#define DF_CLAN_RANK_E1		1440000
#define DF_CLAN_RANK_E2		1760000
#define DF_CLAN_RANK_E3		2120000
#define DF_CLAN_RANK_E4		2480000
#define DF_CLAN_RANK_F1		2880000
#define DF_CLAN_RANK_F2		3360000
#define DF_CLAN_RANK_F3		3920000
#define DF_CLAN_RANK_F4		4560000
#define DF_CLAN_RANK_F5		5280000
#define DF_CLAN_RANK_G1		5840000
#define DF_CLAN_RANK_G2		6480000
#define DF_CLAN_RANK_G3		7200000
#define DF_CLAN_RANK_G4		8000000
#define DF_CLAN_RANK_G5		8880000
#define DF_CLAN_RANK_H1		9840000
#define DF_CLAN_RANK_H2		11680000
#define DF_CLAN_RANK_H3		12800000
#define DF_CLAN_RANK_H4		14000000
#define DF_CLAN_RANK_H5		15280000
#define DF_CLAN_RANK_I1		16880000
#define DF_CLAN_RANK_I2		18800000
#define DF_CLAN_RANK_I3		20240000
#define DF_CLAN_RANK_I4		22800000
#define DF_CLAN_RANK_I5		25680000
#define DF_CLAN_RANK_J1		28880000
#define DF_CLAN_RANK_J2		32400000
#define DF_CLAN_RANK_J3		36240000
#define DF_CLAN_RANK_J4		40400000
#define DF_CLAN_RANK_J5		44880000
#define DF_CLAN_RANK_K1		49680000
#define DF_CLAN_RANK_K2		54800000
#define DF_CLAN_RANK_K3		60240000
#define DF_CLAN_RANK_K4		66000000
#define DF_CLAN_RANK_K5		72080000
#define DF_CLAN_RANK_L1		78880000
#define DF_CLAN_RANK_L2		86080000
#define DF_CLAN_RANK_L3		93680000
#define DF_CLAN_RANK_L4		94480000
#define DF_CLAN_RANK_L5		103680000
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum

enum CLAN_UNIT
{
	CLAN_UNIT_UNKNOWN = -1,
	CLAN_UNIT_SQUARD = 0,	//	분대
	CLAN_UNIT_PLATOON,		//	소대
	CLAN_UNIT_COMPANY,		//	중대
	CLAN_UNIT_BATTALION,	//	대대
	CLAN_UNIT_REGIMENT,		//	연대
	CLAN_UNIT_BRIGADE,		//	여단
	CLAN_UNIT_DIVISION,		//	사단
	CLAN_UNIT_CORPS,		//	군단

	CLAN_UNIT_TOP = CLAN_UNIT_CORPS,
	CLAN_UNIT_MAX
};

enum CLAN_ROOM_STATE
{
	CLAN_ROOM_EMPTY = 0,		// 비어있는 팀
	CLAN_ROOM_WAIT,				// 대기중인 팀 ( 전투 신청 받을 수 없는 상태 )
	CLAN_ROOM_STAY = CLAN_ROOM_WAIT,	// 작업완료동안 임시로 적용(강지원)
	//CLAN_ROOM_STAY,				// 대기중인 팀 ( 전투 신청 받을 수 있는 상태 )
	CLAN_ROOM_READY,			// 매치 팀을 만나 게임 시작 준비중 ( 전투 신청 받을 수 없는 상태 )
	CLAN_ROOM_PLAY,				// 게임중인 팀
};

enum CLAN_NOTE_CLASS
{
	CLAN_NOTE_ALL = 0,			// 쪽지 전체 보내기
	CLAN_NOTE_STAFF,			// 쪽지 스텝에게 보내기
	CLAN_NOTE_REGULAR,			// 쪽지 일반원에게 보내기
};

enum NOTE_MESSAGE_BASIC
{
	NOTE_MESSAGE_CLAN_INVITE		= 1,	// 마스터가 보내는 클랜 초대 쪽지에 대한 코드
	NOTE_MESSAGE_CLAN_INVITE_ACCEPT,		// 클랜 가입신청서에 대한 승락을 마스터가 보내는 코드
	NOTE_MESSAGE_CLAN_INVITE_DENIAL,		// 클랜 가입신청서에 대한 거절을 마스터가 보내는 코드
	NOTE_MESSAGE_CLAN_JOIN_ACCEPT,			// 클랜 가입 수락을 마스터에게 보내는 코드
	NOTE_MESSAGE_CLAN_JOIN_DENIAL,			// 클랜 가입 거부를 마스터에게 보내는 코드
	NOTE_MESSAGE_CLAN_SECESSION,			// 클랜 탈퇴를 쪽지로 보내는 코드
	NOTE_MESSAGE_CLAN_DEPORTATION,			// 클랜 강제 제명 쪽지에 대한 코드
	NOTE_MESSAGE_CLAN_MASTER,				// 클랜 마스터를 위임하는 쪽지에 대한 코드
	NOTE_MESSAGE_CLAN_STAFF,				// 클랜 회원을 클랜 스텝으로 임명하는 쪽지에 대한 코드
	NOTE_MESSAGE_CLAN_REGULAR,				// 클랜 스텝을 일반 회원으로 강등하는 쪽지에 대한 코드
	
	NOTE_MESSAGE_MAX,
};

enum CLAN_MENAGE_AUTHORITY_TYPE
{
	CLAN_MENAGE_TYPE_WRITE = 0,
	CLAN_MENAGE_TYPE_INVITED,
	CLAN_MENAGE_TYPE_DEPORTATION,
	CLAN_MENAGE_TYPE_ACCEPT,

	CLAN_MENAGE_TYPE_MAX
};

// 클랜원 등급
enum CLAN_MEMBER_LEVEL_TYPE
{
	CLAN_MEMBER_LEVEL_UNKNOWN = 0,				// 클랜 방문자

	CLAN_MEMBER_LEVEL_MASTER = 1,				// 클랜 마스터
	CLAN_MEMBER_LEVEL_STAFF,					// 클랜 스탭
	CLAN_MEMBER_LEVEL_REGULAR,					// 클랜 일반회원

	CLAN_MEMBER_LEVEL_MAX
};


//	클랜 등급
enum CLAN_RANK
{
	CLAN_RANK_UNKNOWN = -1,
	CLAN_RANK_NEW = 0,	//	신생
	CLAN_RANK_A1,		//	훈련 1단계
	CLAN_RANK_A2,		//	훈련 2단계
	CLAN_RANK_B1,		//	견습 1단계
	CLAN_RANK_B2,		//	견습 2단계
	CLAN_RANK_C1,		//	지원 1단계
	CLAN_RANK_C2,		//	지원 2단계
	CLAN_RANK_C3,		//	지원 3단계
	CLAN_RANK_D1,		//	우수 1단계
	CLAN_RANK_D2,		//	우수 2단계
	CLAN_RANK_D3,		//	우수 3단계
	CLAN_RANK_D4,		//	우수 4단계
	CLAN_RANK_E1,		//	전투 1단계
	CLAN_RANK_E2,		//	전투 2단계
	CLAN_RANK_E3,		//	전투 3단계
	CLAN_RANK_E4,		//	전투 4단계
	CLAN_RANK_F1,		//	선봉 1단계
	CLAN_RANK_F2,		//	선봉 2단계
	CLAN_RANK_F3,		//	선봉 3단계
	CLAN_RANK_F4,		//	선봉 4단계
	CLAN_RANK_F5,		//	선봉 5단계
	CLAN_RANK_G1,		//	기동 1단계
	CLAN_RANK_G2,		//	기동 2단계
	CLAN_RANK_G3,		//	기동 3단계
	CLAN_RANK_G4,		//	기동 4단계
	CLAN_RANK_G5,		//	기동 5단계
	CLAN_RANK_H1,		//	수색 1단계
	CLAN_RANK_H2,		//	수색 2단계
	CLAN_RANK_H3,		//	수색 3단계
	CLAN_RANK_H4,		//	수색 4단계
	CLAN_RANK_H5,		//	수색 5단계
	CLAN_RANK_I1,		//	특전 1단계
	CLAN_RANK_I2,		//	특전 2단계
	CLAN_RANK_I3,		//	특전 3단계
	CLAN_RANK_I4,		//	특전 4단계
	CLAN_RANK_I5,		//	특전 5단계
	CLAN_RANK_J1,		//	특공 1단계
	CLAN_RANK_J2,		//	특공 2단계
	CLAN_RANK_J3,		//	특공 3단계
	CLAN_RANK_J4,		//	특공 4단계
	CLAN_RANK_J5,		//	특공 5단계
	CLAN_RANK_K1,		//	특수 1단계
	CLAN_RANK_K2,		//	특수 2단계
	CLAN_RANK_K3,		//	특수 3단계
	CLAN_RANK_K4,		//	특수 4단계
	CLAN_RANK_K5,		//	특수 5단계
	CLAN_RANK_L1,		//	특무 1단계
	CLAN_RANK_L2,		//	특무 2단계
	CLAN_RANK_L3,		//	특무 3단계
	CLAN_RANK_L4,		//	특무 4단계
	CLAN_RANK_L5,		//	특무 5단계

	CLAN_RANK_TOP = CLAN_RANK_L5,
	CLAN_RANK_MAX
};

// 명예전 신청 응답값
enum ENUM_CLAN_HONOR_MATCH_ACCEPT
{
	ENUM_CLAN_HONOR_MATCH_REFUSE	= 0,
	ENUM_CLAN_HONOR_MATCH_AGREE,

	ENUM_CLAN_HONOR_MATCH_ACCEPT_MAX
};

// 명예전 신청 상태값
enum ENUM_CLAN_HONOR_MATCH_TYPE
{
	ENUM_CLAN_HONOR_MATCH_NONE		= 0,
	ENUM_CLAN_HONOR_MATCH_WAIT,
	ENUM_CLAN_HONOR_MATCH_ENABLE,
	
	ENUM_CLAN_HONOR_MATCH_MAX
};

enum ENUM_MATCH_CLAN_INFO_TYPE
{
	ENUM_MATCH_CLAN_INFO_NONE		= 0,
	ENUM_MATCH_CLAN_INFO_WAIT,
	ENUM_MATCH_CLAN_INFO_ENABLE,

	ENUM_MATCH_CLAN_INFO_MAX
};

enum SEASON_TYPE
{
	SEASON_TYPE_NONE		= -1,	// None
	SEASON_TYPE_PRE			,		// 이전 시즌
	SEASON_TYPE_ONGOING		,		// 현재 시즌

	SEASON_TYPE_COUNT
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)



// 클랜 쪽지를 위한 구조체
struct CLAN_NOTE
{
	T_UID	_DBIdx = 0;
	UINT8	_IsAsk = 0;
	UINT8	_MessageSize = 0;
};

// 클랜 목록 정보
struct CLAN_CONTEXT
{
	UINT32	_totalCount = 0;						// 전체수
	UINT8	_sectionSize = 0;						// 페이지 크기
	UINT16	_sectionCount = 0;						// 전체 페이지
	UINT32	_version = 0;							// 목록 버전
} ;


// 클랜전 전적. 매치메이킹 입장시 전달용.
// 클랜원 : 누적 클랜 개인 전적( 일반 클랜 매치 + 매치 메이킹 ) 
// 용병 : 시즌 전적(매치메이킹)
struct MATCHMAKING_RECORD
{
	INT8	m_i8RoomSlotIdx = 0;					// 룸 슬롯 인덱스. 매치팀 슬롯 인덱스 아님. 음수이면 문제 없는 유저.
	UINT32	m_ui32Killcount = 0;					// 킬
	UINT32	m_ui32Death = 0;						// 데쓰
	UINT32	m_ui32Win = 0;							// 승
	UINT32	m_ui32Lose = 0;							// 패
	UINT32	m_ui32Assistcount = 0;					// 어시스트
};

struct MATCHMAKING_RECORD_SVR	: public MATCHMAKING_RECORD
{
	T_UID	m_TUID = 0;								// UID
	INT32	m_i32SessionIdx = 0;					// 세션인덱스
};

// 클랜 기본 정보
struct CLAN_BASIC_INFO
{
	UINT32	_idx = 0;								// 클랜 인덱스
	TTCHAR	_name[NET_CLAN_NAME_SIZE] = { 0 };			// 클랜명
	UINT8	_rank = 0;								// 클랜 등급
	UINT8	_person = 0;							// 현재 클랜원 수
	UINT8	_maxperson = 0;							// 최대 클랜원 수
	UINT32	_date = 0;								// 클랜 개설일
	UINT32	_mark = 0;								// 마크
	UINT8	_color = 0;								// 클랜컬러
} ;

struct CLAN_MATCHING_TEAM_INFO : public CLAN_BASIC_INFO
{
	T_TeamID m_TTeamID = 0;							// 매치팀 ID.
	//MATCHMAKING_RECORD m_Record[MAX_CLAN_MATCH_MEMBER];
};

struct CLAN_BASIC_RECORD
{
	// 승률
	UINT32	m_match = 0;							// 매치수
	UINT32	m_win = 0;								// 승수
	UINT32	m_lose = 0;								// 패수

	UINT32	m_totalWin = 0;							// 누적 승수
};

struct CLAN_BASIC_SORT_INFO
{
	// 클랜 정렬 조건
	REAL64	m_clanpoint = 0.0f;						// 현재시즌 클랜 포인트. 
	REAL64	m_Preclanpoint = 0.0f;						// 이전시즌 클랜 포인트. 
	UINT8	m_ui8ConsecutiveWinCnt = 0;				// 최근 클랜 매치 30개의 최대 연승 횟수

};

struct CLAN_MATCH_INFO
{
	CLAN_BASIC_INFO			m_ClanBasicInfo;			// 클랜 기본정보
	CLAN_BASIC_RECORD		m_ClanBasicRecord;			// 클랜 승률	
	UINT8					m_ui8WinLose[MAX_LMRC] = { 0 };		// 매치 전적(최근 3개)( 승패 win:3, lose:2, drow:1)	
};

struct TYPE_CLAN_ROOM_INFO
{
	UINT8					m_ui8ClanHonorType = ENUM_CLAN_HONOR_MATCH_NONE;				// 클랜명예전 매치상태
	UINT8					m_ui8ClanInfoState = ENUM_MATCH_CLAN_INFO_NONE;				// 클랜정보 저장 상태(ENUM_MATCH_CLAN_INFO_ENABLE : 정보설정 완료)
	CLAN_MATCH_INFO			m_ClanMatchInfo[TEAM_COUNT];	// 클랜전일 경우 방생성시 정보 셋팅

	void	Reset()
	{
		m_ui8ClanHonorType				= ENUM_CLAN_HONOR_MATCH_NONE;
		m_ui8ClanInfoState				= ENUM_MATCH_CLAN_INFO_NONE;
		ZeroMemory( m_ClanMatchInfo,	sizeof(CLAN_MATCH_INFO)*TEAM_COUNT );
	}

}; // 클랜매치시의 클랜정보

struct CLAN_LIST_BASIC_INFO
{
	CLAN_BASIC_INFO			m_ClanBasicInfo;					// 클랜 기본정보
	CLAN_BASIC_RECORD		m_ClanBasicRecord;					// 클랜 승률
	CLAN_BASIC_SORT_INFO	m_ClanBsicSortInfo;					// 클랜 정렬조건 데이터

	UINT16					m_ui16introsize = 0;
	TTCHAR					m_intro[NET_CLAN_INTRO_SIZE] = { 0 };		// 클랜 소개	
};

struct CLAN_RECORD
{
	INT32		m_i32MatchCount = 0;
	INT32		m_i32WinMatch = 0;
	INT32		m_i32LoseMatch = 0;
	INT32		m_i32Experience = 0;
	CRating		m_ClanRating;
};





// 클랜 상세 정보
struct CLAN_DETAIL_INFO
{
	// Basic
	UINT32	m_ui32CID;							// 클랜 인덱스
	TTCHAR	m_name[NET_CLAN_NAME_SIZE];			// 클랜명
	UINT8	m_expRank;							// 경험치 등급
	UINT8	m_currentPerson;					// 현재 클랜원 수
	UINT8	m_maxPerson;						// 최대 클랜원 수
	UINT32	m_date;								// 클랜 개설일
	UINT32	m_mark;								// 마크
	UINT8	m_color;							// 클랜컬러

	// Detail
	UINT8	m_personRank;						// 클랜원 등급
	UINT32	m_exp;								// 경험치	
	UINT16	m_clanRank;							// 클랜 순위
	UINT16	m_prevRank;							// 이전 클랜 순위	
	T_UID	m_i64masterUID;						// 마스터 dbIdx(UID)
	TTCHAR	m_masterNick[NET_NICK_NAME_SIZE];	// 마스터 닉네임
#ifdef USE_COLOR_NICK2
	UINT8	m_masterNickColor;					// 마스터 닉네임 칼라
#endif
	UINT8	m_masterRank;						// 마스터 등급	
	TTCHAR	m_intro[NET_CLAN_INTRO_SIZE];		// 클랜 소개	
	TTCHAR	m_azitURL[NET_CLAN_AZIT_URL_SIZE];	// 아지트 URL
	UINT8	m_ranklock;							// 계급 제한
	UINT8	m_lagelock;							// 연령 제한 하한
	UINT8	m_hagelock;							// 연령 제한 상한

	// Member
	UINT8	m_authstaff;						// 스탭 권한
	TTCHAR	m_announce[NET_CLAN_INTRO_SIZE];	// 공지사항

	// 누적 전적
	UINT32	m_match;							// 통산전적
	UINT32	m_win;								// 승수
	UINT32	m_lose;								// 패수	

	// 전체 전적
	UINT32	m_Totalmatch;							// 전체 전적
	UINT32	m_Totalwin;								// 전체 승수
	UINT32	m_Totallose;							// 전체 패수	

	//UINT32	m_smatch;
	//UINT32	m_swin;	
	//UINT32	m_slose;
	
	//REAL32	m_clanpoint;						// 클랜 포인트	
	UINT32	m_reqoption;						// 클랜 가입 승인 설정(자동승인, 수동승인)

	//UINT8	m_ui8ConsecutiveWinCnt;				// 최근 클랜 매치 30개의 최대 연승 횟수

	CLAN_RECORD	m_SeasonRecord[SEASON_TYPE_COUNT];	// 시즌 전적.( 포인트포함 )

//////////////////////////////////////////////////////////////////////////
	void	Reset()
	{
		m_ui32CID		= 0;
		m_expRank		= 0;
		m_currentPerson	= 1;
		m_maxPerson		= BASIC_MEMBER_COUNT;
		m_name[0]		= '\0';
		m_color			= 0;

		m_personRank	= 0;
		m_exp			= 0;
		m_clanRank		= 0;
		m_prevRank		= 0;
		m_i64masterUID	= 0;
		m_masterNick[0]	= '\0';
#ifdef USE_COLOR_NICK2
		m_masterNickColor	= 0;
#endif
		m_masterRank	= 0;
		m_intro[0]		= '\0';
		m_azitURL[0]	= '\0';
		m_ranklock		= 0;
		m_lagelock		= 0;
		m_hagelock		= 0;

		m_authstaff		= CLAN_MASTER_AUTH;
		m_announce[0]	= '\0';

		m_match			= 0;
		m_win			= 0;
		m_lose			= 0;

		m_Totalmatch	= 0;
		m_Totalwin		= 0;
		m_Totallose		= 0;

		m_reqoption		= CLAN_AUTO_OPTION_MANUAL;

		//m_ui8ConsecutiveWinCnt	= 0;

		memset( m_SeasonRecord, 0, sizeof(CLAN_RECORD)* SEASON_TYPE_COUNT );
	
		for( INT32 i = 0 ; i < SEASON_TYPE_COUNT; ++i )
		{
			m_SeasonRecord[(SEASON_TYPE)i].m_ClanRating.Initialize(); 
		}
	}

	double	GetClanPoint(SEASON_TYPE eSeasonType = SEASON_TYPE_ONGOING)
	{
		if( eSeasonType <= SEASON_TYPE_NONE || eSeasonType >= SEASON_TYPE_COUNT )
		return CLAN_BASIC_POINT;

		return m_SeasonRecord[eSeasonType].m_ClanRating.GetClanPoint();
	}

	void	IncClanPoint(REAL64 r64AddPoint)
	{
		m_SeasonRecord[SEASON_TYPE_ONGOING].m_ClanRating.IncClanPoint(r64AddPoint);
	}

	void	ResetClanPoint(void)
	{
		m_SeasonRecord[SEASON_TYPE_ONGOING].m_ClanRating.Initialize();
	} 

	void	ResetClanRecord(void)
	{
		m_SeasonRecord[SEASON_TYPE_ONGOING].m_i32MatchCount = 0;
		m_SeasonRecord[SEASON_TYPE_ONGOING].m_i32LoseMatch = 0;
		m_SeasonRecord[SEASON_TYPE_ONGOING].m_i32LoseMatch = 0;
	} 

	CRating&	GetClanRating(void)
	{
		return m_SeasonRecord[SEASON_TYPE_ONGOING].m_ClanRating;
	}

	void	SetClanRecord( SEASON_TYPE eSeasonType, INT32 i32Match, INT32 i32Win, INT32 i32Lose, CRating& Rating ) 
	{
		if( eSeasonType <= SEASON_TYPE_NONE || eSeasonType >= SEASON_TYPE_COUNT )
		return;

		i32Match	= i32Match>0?i32Match:0;
		i32Win		= i32Win>0?i32Win:0;
		i32Lose		= i32Lose>0?i32Lose:0;

		m_SeasonRecord[eSeasonType].m_i32MatchCount = i32Match;
		m_SeasonRecord[eSeasonType].m_i32WinMatch	= i32Win;
		m_SeasonRecord[eSeasonType].m_i32LoseMatch	= i32Lose;

		if( Rating.GetClanPoint() < CLAN_MIN_POINT || Rating.GetDeviation() < CLAN_MIN_POINT )
		{	// 시즌 전적이 한번도 없는 경우입니다.
			Rating.Initialize();
		}

		m_SeasonRecord[eSeasonType].m_ClanRating = Rating;
	}
};

struct CLAN_TOP_MEMBER
{
	T_UID	m_topexp;							// 통산 경험치 공로자
	T_UID	m_stopexp;							// 시즌 경험치 공로자
	T_UID	m_topwin;							// 통산 승수 공로자
	T_UID	m_stopwin;							// 시즌 승수 공로자
	T_UID	m_topkill;							// 통산 킬 공로자
	T_UID	m_stopkill;							// 시즌 킬 공로자
	T_UID	m_tophead;							// 통산 헤드샷 공로자
	T_UID	m_stophead;							// 시즌 헤드샷 공로자
	T_UID	m_topconnect;						// 통산 접속률 공로자
	T_UID	m_stopconnect;						// 시즌 접속률 공로자
	
	void	Reset()
	{
		m_topexp		= 0;
		m_stopexp		= 0;
		m_topwin		= 0;
		m_stopwin		= 0;
		m_topkill		= 0;
		m_stopkill		= 0;
		m_tophead		= 0;
		m_stophead		= 0;
		m_topconnect	= 0;
		m_stopconnect	= 0;
	}
};

struct CLAN_NEWS
{
	UINT8	m_ClanRequestCount;							// 가입승인 대기 인원
	
	// 최근 클랜전
	UINT8	m_ui8WinLose;								// ROOM_ENDTYPE
	T_UID	m_i64PlayerUID[CLAN_TEAM_MEMBER_COUNT];		// 매치 참가 클랜원(최대 8명)
};

//// 클랜 최근 전적 30개. 구버전. 사용 안함. 2015.10.28
struct CLAN_MATCH_RESULT
{
	UINT64	m_ui64MatchDate;								// 매치 결과 기록 시간
	UINT8	m_ui8MatchCount;								// 매치 결과 고유번호(0~29의 값을 하나씩 부여 받는다.)	
	UINT8	m_ui8StageMode;									// 스테이지 모드 ( STAGE_MASK_TYPE )
	//UINT8	m_ui8SubMask;									// 무기제한 모드
	UINT32	m_ui32StageID;									// 맵 아이디

	UINT32	m_ui32CID;										// 상대 클랜아이디
	UINT32	m_ui32Mark;										// 상대 클랜마크
	TTCHAR	m_ClanName[NET_CLAN_NAME_SIZE];					// 상대 클랜명

	UINT8	m_ui8WinLose;									// 승패 win:3, lose:2, drow:1 (ENUM_CLAN_MATCH_RESULT)
	UINT8	m_ui8Win;										// 라운드 승수
	UINT8	m_ui8Lose;										// 라운드 패수	

	T_UID	m_i64PlayerUID[CLAN_TEAM_MEMBER_COUNT];			// 매치 참가 클랜원(최대 8명)
	
	UINT32	m_ui32PositiveCnt;								// 좋아요 누적 선택 개수
	UINT32	m_ui32NegativeCnt;								// 싫어요 누적 선택 개수

	T_UID	m_i64PositiveUser;								// 좋아요 마지막 유저 UID
	T_UID	m_i64NegativeUser;								// 싫어요 마지막 유저 UID

	TTCHAR	m_strPositiveUserNick[NET_NICK_NAME_SIZE];		// 좋아요 마지막 유저 닉네임	
	TTCHAR	m_strNegativeUserNick[NET_NICK_NAME_SIZE];		// 싫어요 마지막 유저 닉네임	

	void	Reset()
	{
		m_ui8MatchCount				= 0;
		m_ui8StageMode				= 0;
		m_ui32StageID				= 0;
		m_ui32CID					= 0;
		m_ui32Mark					= CLAN_MARK_DEFAULT;
		m_ClanName[0]				= '\0';

		m_ui8WinLose				= CLAN_MATCH_RESULT_NONE;
		m_ui8Win					= 0;
		m_ui8Lose					= 0;

		ZeroMemory(m_i64PlayerUID, sizeof(T_UID)*CLAN_TEAM_MEMBER_COUNT);

		m_i64PositiveUser			= 0;
		m_i64NegativeUser			= 0;

		m_strPositiveUserNick[0]	= '\0';
		m_strNegativeUserNick[0]	= '\0';
	}
};

// 클랜 매치 결과 목록 정보
struct CLAN_MATCH_RESULT_CONTEXT
{
	UINT8	m_ui8TotalCount;					// 전체 클랜원 수
	UINT8	m_ui8SectionSize;					// 페이지 크기
	UINT8	m_ui8SectionCount;					// 전체 페이지
	UINT32	m_ui32Version;						// 목록 버전
};

// 멤버 목록 정보
typedef struct _tag_ClanMemberContext
{
	UINT8	_totalCount;						// 전체 클랜원 수
	UINT8	_sectionSize;						// 페이지 크기
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} CLAN_MEMBER_CONTEXT;

typedef struct _ClanMemberInfo
{
	T_UID	_fuid;		// 클랜맴버 uid
	UINT64	_state;		// 상태 0xF 00 F F FFF - bit 28-31:loginState, 20-27:reserved, 16-19:serverIdx, 12-15:channelIdx, 0-11:roomIdx	
	UINT8	_rank;		// 계급 (0,51]
	UINT8	m_ui8NickColor;		// 닉네임 컬러
} CLANMEMBER_INFO;

// 멤버 기본 정보
typedef struct _tag_ClanMemberBasicInfo
{
//	UINT8	_idx;								// 클랜원 인덱스
	T_UID	_i64UID;							// 클랜원 dbIdx(UID)
	TTCHAR	_nick[NET_NICK_NAME_SIZE];			// 클랜원 닉네임
	UINT8	_rank;								// 클랜원 등급
	UINT8	_level;								// 클랜원 스텝 직급
	UINT64	_state;								// 클랜원 현재 접속 위치 및 상태
	UINT32	_date;								// 클랜원 신청일
	UINT8	_color;								// 클랜원 닉네임 칼라

	UINT32	m_win;								// 승리 횟수
	UINT32	m_lose;								// 패배 횟수

} CLAN_MEMBER_BASIC_INFO;

typedef struct __tag_ClanResultUser
{
	T_MerID		m_TMerID;							// 용병 인덱스
	UINT8		_ui8TeamIdx;						// 팀 인덱스. T_TeamID 아님.
	UINT32		_ClanIdx;							// Clan 인덱스
	T_UID		_i64UID;							// User 인덱스
	UINT32		_Exp;								// 경험치
	UINT16		_Kill;								// 킬
	UINT16		_Death;								// 데스
	UINT16		_Head;								// 헤드샷률
	UINT8		_WinLose;							// ROOM_ENDTYPE
	UINT16		_Assist;							// 어시스트
}CLAN_RESULT_USER;

// 멤버 상세 정보
typedef struct _tag_ClanMemberDetailInfo
{
	// Basic
	//	UINT8			_idx;								// 클랜원 인덱스
	T_UID			_i64UID;							// 클랜원 dbIdx(UID)
	TTCHAR			_nick[NET_NICK_NAME_SIZE];			// 클랜원 닉네임
	UINT8			_rank;								// 클랜원 등급
	UINT8			_level;								// 클랜원 스텝 직급
	UINT64			_state;								// 클랜원 현재 접속 위치 및 상태
	UINT32			_date;								// 클랜원 신청일
	UINT8			_color;								// 클랜원 닉네임 칼라

	UINT32			m_win;								// 승리 횟수
	UINT32			m_lose;								// 패배 횟수

	UINT32			m_kill;
	UINT32			m_death;
	UINT32			m_Assist;

	// Server
	INT32			_session;							// Session
	//T_TeamID 		_battleteam;						// Match Team Id.
	UINT8			_pagein;							// 클랜 페이지인

	CRating			m_Rating;

	_tag_ClanMemberDetailInfo::_tag_ClanMemberDetailInfo()
	{
		Reset();
	};

	void	Reset()
	{
		_nick[0]	= '\0';
		_level		= CLAN_MEMBER_LEVEL_REGULAR;
		_state		= MAKE_FRIEND_MATCH_STATE( MATCH_TEAM_INVALID_IDX, FRIEND_STATE_OFFLINE, 0, 0, 0 );
		_session	= -1;
//		_battleteam	= MATCH_TEAM_INVALID_IDX; 
		_pagein		= 0;
		_color		= 0;

		m_win		= 0;
		m_lose		= 0;

		m_kill = 0;
		m_death = 0;
		m_Assist = 0;
	};

	void	SetRating(REAL64 r64Mean, REAL64 r64StdDeviation);

	void	AddRecord(CLAN_RESULT_USER* pResult)
	{
		switch( pResult->_WinLose )
			{
			case ROOM_ENDTYPE_WIN	: ++m_win;	break;
			case ROOM_ENDTYPE_LOSE	: ++m_lose;	break;
			default : break;
			}
	
		m_kill	+= pResult->_Kill;
		m_death	+= pResult->_Death;
		m_Assist += pResult->_Assist;
	};

} CLAN_MEMBER_DETAIL_INFO;

inline void CLAN_MEMBER_DETAIL_INFO::SetRating(REAL64 r64Mean, REAL64 r64StdDeviation)
{
	m_Rating.mean_ = r64Mean;
	m_Rating.standard_deviation_ = r64StdDeviation;
}

// 신청서 목록 정보
typedef struct _tag_ClanRequestContext
{
	UINT8	_totalCount;						// 전체 신청서 수
	UINT8	_sectionSize;						// 페이지 크기
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} CLAN_REQUEST_CONTEXT;

// 신청서 기본 정보
typedef struct _tag_ClanRequestBasicInfo
{
	T_UID	_i64UID;							// 신청서 dbIdx(UID)
	TTCHAR	_nick[NET_NICK_NAME_SIZE];			// 신청인 닉네임
	UINT8	_rank;								// 신청인 등급
	UINT8	_ui8NickColor;						// 신청인 닉네임컬러
	UINT32	_date;								// 신청일
	UINT32	m_ui32Kill;								// 신청인 킬수
	UINT32	m_ui32Death;							// 신청인 데스수
	UINT32	m_ui32Match;							// 신청인 전수
	UINT32	m_ui32Win;								// 신청인 승수
	UINT32	m_ui32Lose;								// 신청인 패수	
	TTCHAR	m_strContents[NET_CLAN_REQUEST_SIZE];	// 신청서 내용
	UINT32	m_ui32Assist;							// 신청인 어시스트수

} CLAN_REQUEST_BASIC_INFO;

// 신청서 상세 정보(1.5에서는 폐기 예정)
typedef struct _tag_ClanRequestDetailInfo
{
	T_UID	_i64UID;							// 신청인 dbIdx(UID)
	TTCHAR	_nick[NET_NICK_NAME_SIZE];			// 신청인 닉네임
	UINT8	_rank;								// 신청인 등급
	UINT8	_ui8NickColor;						// 신청인 닉네임컬러
	UINT32	_kill;								// 신청인 킬수
	UINT32	_death;								// 신청인 데스수
	UINT32	_match;								// 신청인 전수
	UINT32	_win;								// 신청인 승수
	UINT32	_lose;								// 신청인 패수
	TTCHAR	_contents[NET_CLAN_REQUEST_SIZE];	// 신청서 내용
	UINT32	_Assist;							// 신청인 어시스트수

	void Reset(void)
	{
		//_idx = 0;
		_i64UID = 0;
		_nick[0] = '\0';
		_rank = 0;
		_ui8NickColor = 0;
		_kill = 0;
		_death = 0;
		_match = 0;
		_win = 0;
		_lose = 0;
		_contents[0] = '\0';
		_Assist = 0;
	}

} CLAN_REQUEST_DETAIL_INFO;

typedef struct __tag_ClanManagement
{
	UINT8	_authority;							// Staff 권한
	UINT8	_limitRank;							// 계급 제한
	UINT8	_limitAgeLow;						// 나이 제한( 최저 )
	UINT8	_limitAgeHigh;						// 나이 제한( 최대 )

}CLAN_MANAGEMENT;

typedef struct _tagCreateClanInfo
{
	TTCHAR	_name[NET_CLAN_NAME_SIZE];
	TTCHAR	_intro[NET_CLAN_INTRO_SIZE];
	TTCHAR	_URL[NET_CLAN_AZIT_URL_SIZE];
	UINT32	_mark;
} CREATE_CLAN_INFO;

typedef struct _tagclanjoin
{
	UINT32	_clanidx;
	TTCHAR	_message[ NET_CLAN_REQUEST_SIZE ];
	UINT8	_age;
	UINT8	_rank;
}CLAN_JOIN_INFO;

typedef struct __tag_ClanBattleAllTeamContext
{
	UINT16	_totalcount;
	UINT8	_sectionSize;								// 페이지 크기
	UINT16	_sectionCount;								// 전체 페이지
}CLAN_BATTLE_ALL_TEAM_CONTEXT;

typedef struct __tag_ClanBattleClanContext
{
	UINT8	_totalcount;
	UINT8	_sectionSize;								// 페이지 크기
	UINT8	_sectionCount;								// 전체 페이지
}CLAN_BATTLE_TEAM_CONTEXT;

typedef struct __tag_ClanBattleTeam
{
	UINT16	_Channel;
	UINT8	_State;										// 현재 팀 상태 ( CLAN_ROOM_STATE )
	UINT8	_Naming;									// 네이밍 ( 클랜 팀 인덱스와 동일 )
	INT8	_PerMax;									// 전투 최대 인원수
	INT8	_PerCount;									// 현재 인원수
	UINT8	_MasterIdx;									// 마스터 인덱스(CLAN_BATTLE_TEAM_LIST에서 몇번째 인덱스인지 )
	UINT32	_SpIdx;										// 고유 인덱스(클랜에서 설정해주는 고유값)

	TTCHAR	m_strOperation[NET_CLAN_OPERATION_SIZE];	// 작전명

	__tag_ClanBattleTeam()
	: _Channel(0), _State(CLAN_ROOM_EMPTY), _Naming(0), _PerMax(0), _PerCount(0), _MasterIdx(0), _SpIdx(0)
	{
	}
} CLAN_BATTLE_TEAM;

typedef struct __tag_ClanBattleClan
{
	UINT32	_ClanIdx;									// 클랜 Idx ( DBIdx 가 아닌 클랜 서버 인덱스 )
	UINT8	_ClanRank;									// 클랜 랭킹
	UINT32	_ClanMark;									// 클랜 마크
	TTCHAR	_ClanName[ NET_CLAN_NAME_SIZE ];			// 클랜 이름
	REAL64	_ClanPoint;
	UINT8	_ClanColor;									// 클랜 칼라

} CLAN_BATTLE_CLAN;

enum MEMBER_TYPE
{
	MEMBER_CLAN			= 0,
	MEMBER_MERCENARY	,
	MEMBER_TYPE_MAX		,
};

typedef struct __tag_ClanBattleMember
{
	MEMBER_TYPE		m_eMemberType;
	UINT8			_Rank;
	TTCHAR			_Name[ NET_NICK_NAME_SIZE ];
	T_UID			_i64UID;
	UINT8			_State;
	CRating			m_Rating;
	INT32			m_i32KDWeight;
} CLAN_BATTLE_MEMBER;

typedef struct __tag_ClanBattleTeamList
{
	UINT32				_Idx;
	CLAN_BATTLE_TEAM	_Team;
	CLAN_BATTLE_MEMBER	_Member;
} CLAN_BATTLE_TEAM_LIST;

typedef struct __tag_ClanBattleTeamInfo
{
	UINT32				_Idx;							
	CLAN_BATTLE_TEAM	_Team;
	CLAN_BATTLE_CLAN	_Clan;
	CLAN_BATTLE_MEMBER	_Member;
} CLAN_BATTLE_TEAM_INFO;

struct USER_INFO_CLAN
{
	UINT32			_ui32ClanIdx; 
	UINT32			_ui32ClanState;
	UINT8			_ui8ExpRank;
	UINT8			_ui8PerRank;
	UINT32			_ui32Mark;
	TTCHAR			_strClanName[NET_CLAN_NAME_SIZE] ;
	UINT32			_ui32ClanRankingDate;
};

struct MATCH_TEAM_INFO	
{
	T_TeamID		m_TTeamID;			// 매치팀 ID. 서버내 매치팀 고유 ID. 0~MAX_MATCH_TEAM_COUNT.
	INT8			m_i8ClanTeamIdx;	// 클랜 팀 Idx. 자신의 클랜 매치팀들중 몇번인지. 0 ~ MAX_CLAN_MATCH_TEAM_COUNT-1.
	UINT8			m_ui8TeamState;		// 팀 상태.	TEAM_STATE
	UINT8			m_ui8MaxPer;		// 최대 인원수
	UINT8 			m_ui8CurPer;		// 현재 인원수	
};

struct MATCH_TEAM_DETAIL_INFO : public MATCH_TEAM_INFO
{
	INT8			m_i8LeaderSlot;	// 리더 슬롯 인덱스. 
	TTCHAR			m_strOperation[NET_CLAN_OPERATION_SIZE]; // 작전명
};

struct MATCH_MEMBER_INFO
{
	T_UID			m_TUID;				// UID
	UINT8			m_ui8MemberType;	// 멤버 종류. MEMBER_TYPE
	INT8			m_i8Slot;			// 슬롯인덱스
	UINT8			m_ui8Rank;			// 랭크. 용병일 경우 용병 랭크
	UINT8			m_ui8NickColor;		// 닉네임 컬러
	TTCHAR			m_strNick[NET_NICK_NAME_SIZE];			// 닉네임
	UINT8			m_ui8MerClass;		// 용병 주무기 병과. WEAPON_CLASS_TYPE
};

// 용병 정보
struct MERCENARY_INFO
{
	T_MerID			m_TMercenaryID;		// 용병고유번호.
	UINT8			m_ui8State;			// 상태. MERCENARY_STATE
	UINT8			m_ui8Class;			// 주무기 병과. WEAPON_CLASS_TYPE
	UINT8			m_ui8MerRank;		// 용병 랭크
	UINT8			m_ui8NickColor;		// 닉네임 컬러
	TTCHAR			m_strNick[NET_NICK_NAME_SIZE];			// 닉네임
};

// GameServer - Clan Server
struct REGIST_MERCENARY
{
	T_UID			m_TUID;
	UINT8			m_ui8Class;			// CHARA_CLASS
	UINT8			m_ui8NickColor;
	TTCHAR			m_strNick[NET_NICK_NAME_SIZE];
};

// 용병 전적
struct MERCENARY_RECORD
{
	INT32			m_i32MatchCount;
	INT32			m_i32WinMatchCnt;
	INT32			m_i32LoseMatchCnt;
	INT32			m_i32DisconMatchCnt;
	INT32			m_i32KillCount;
	INT32			m_i32DeathCount;
	INT32			m_i32Headshot;
	INT32			m_i32AssistCount;
};

// 용병 전적
struct MERCENARY_DETAIL_INFO
{
	MERCENARY_RECORD	m_Recod;

	// 확장 고려.
};

struct CLAN_MATCH_MAKING_RESULT
{
	TTCHAR		m_strClanName[NET_CLAN_NAME_SIZE];	// 상대 클랜명
	UINT8		m_Result;							// 승패 결과. ROOM_ENDTYPE
	UINT8		m_MyClanScore;						// 우리 클랜 라운드 스코어
	UINT8		m_OtherClanScore;					// 상대 클랜 라운드 스코어
};

// 클랜 매치메이킹 정보. 최근 15개
struct CLAN_WAR_RESULT : public CLAN_MATCH_MAKING_RESULT
{
	INT32				m_i32MatchIdx;				// 매치 결과 DB Idx. 클수록 최근 경기
	UINT32				m_ui32StageID;				// STAGE_ID

	void	Reset()
	{
		m_i32MatchIdx = 0;
		m_ui32StageID = StageID::MakeStageID( STAGE_MODE_NA, WEAPON_ALL, AI_USE_NO, false, false );
		m_strClanName[0] = '\0';
		m_Result = ROOM_ENDTYPE_NONE;
		m_MyClanScore = 0;
		m_OtherClanScore = 0;
	}
};
#pragma pack( pop)

////////////////////////////////////////////////////////////////////////////////
// End of Clan



#endif