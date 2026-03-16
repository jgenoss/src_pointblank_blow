#ifndef __COMMONDEF_EVENT_H__
#define __COMMONDEF_EVENT_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
#define MAX_INDONESIA_PCBANG_KEY	51										// 인도네시아 피씨방

#define MAX_TS_ITEM_STRING		30
#define MAX_TS_GOODS_LIST		7

#define ATTENDANCE_MAX_EACH_EVENT			5	// 각 타입당 설정할 수 있는 이벤트의 최대 개수
#define ATTENDANCE_MAX_EVENT_COUNT			ATTENDANCE_MAX_EACH_EVENT * ATTENDANCE_TYPE_MAX	// 전체 이벤트 개수

#define ATTENDANCE_MAX_CONSECUTIVE_DAY		31	// 최대 진행 가능한 연속 일수
#define ATTENDANCE_CHOICE_MAX_COUNT			2	// 최대 고를수 있는 아이템 목록 개수
#define	ATTENDANCE_GOODS_MAX_COUNT			ATTENDANCE_MAX_CONSECUTIVE_DAY * ATTENDANCE_CHOICE_MAX_COUNT	// 최대 설정 가능한 아이템 개수
#define ATTENDANCE_NAME_MAX_SIZE			50	// 최대 설정 가능한 이벤트명

#define MAX_RANKUP_ITEM_COUNT				4				// 랭크업아이템의 총 갯수
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
// TS 보상
enum DAILY_GIFT_TYPE
{
	DAILY_GIFT_FIRST		= 0,
	DAILY_GIFT_SECOND,
	DAILY_GIFT_THIRD,

	DAILY_GIFT_TYPE_MAX,
};

// 출석 체크 이벤트 상태
enum ATTENDANCE_STATE
{
	ATTENDANCE_STATE_NONE = 0,			// 이벤트 기간이 아님
	ATTENDANCE_STATE_RUNNING,			// 이벤트 기간
	ATTENDANCE_STATE_COMPLETE,			// 이벤트 보상 받음
};

enum ATTENDANCE_TYPE
{
	ATTENDANCE_TYPE_DORMANT = 0,		// 휴면 유저
	ATTENDANCE_TYPE_NORMAL,				// 모든 유저
	ATTENDANCE_TYPE_MAX
};

enum TS_EVENT_TYPE
{
	TS_EVENT_SEND_ITEM_LIST = 0,
	TS_EVENT_REMAIN_TIME,

	TS_EVENT_INFO_MAX
};

enum EVENT_COUNTRY_BONUS
{
	EVENT_COUNTRY_BONUS_NOT_USE,
	EVENT_COUNTRY_BONUS_USE,
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

// 채널 보너스
struct TYPE_CHANNEL_BONUS
{	
	UINT16	_ui16Type = 0;			// CHANNEL_TYPE
	UINT32	_ui32Point = 0;
	UINT32	_ui32Exp = 0;
};

struct	ATTENDANCE_CHOICE_ITEM
{
	UINT8					m_ui8GoodsCount = 0;
	T_GoodsID				m_ui32GoodsID[ATTENDANCE_CHOICE_MAX_COUNT] = { 0 };
};

struct	ATTENDANCE_INFO
{
	TTCHAR					m_strEventName[ATTENDANCE_NAME_MAX_SIZE] = { 0 };
	BOOL					m_bRepeat = FALSE;												// 반복여부 ( 출책이 한번 완료 한 후 다시 시작 여부 )
	BOOL					m_bReset = FALSE;												// 중간결석 시 처음부터 시작 여부
	UINT8					m_ui8AttendanceType = 0;									// 이벤트 타입( 일반 출석 이벤트, 휴면 계정 출석 이벤트, .. )
	UINT8					m_ui8AttendanceDay = 0;										// 연속 일수
	UINT32					m_ui32Version = 0;											// 이벤트 버전. 각 이벤트마다 유니크한 값 이므로 겹칠수 없다.
	UINT32					m_ui32StartDate = 0;										// 이벤트 시작날
	UINT32					m_ui32EndDate = 0;											// 이벤트 마지막날
	UINT32					m_ui32NextStartDate = 0;									// 다음 이벤트 시작날
	UINT32					m_ui32NextEndDate = 0;										// 다음 이벤트 마지막날
	UINT32					m_ui32ConditionDate = 0;									// 특정 유저를 대상으로 할 경우, 판단의 기준이 되는 일자
	ATTENDANCE_CHOICE_ITEM	m_ui32GiveGoodsID[ATTENDANCE_MAX_CONSECUTIVE_DAY];		// 지급될 아이템 리스트 ( 최대 31일동안 다른 아이템 설정이 가능하므로 2개씩 선택한다고 가정 , 배열 62개 )
};

struct	ATTENDANCE_USER
{
	UINT32	m_ui32Version = 0;										// 마지막으로 진행중이던 이벤트 버전
	UINT8	m_ui8AttendanceDay = 0;									// 마지막으로 진행했던 연속 일수
	UINT8	m_ui8AttendanceItem = 0;								// 마지막으로 지급받은 아이템 수
	UINT8	m_ui8EventState = 0;									// 출석체크 이벤트 상태 //		= ATTENDANCE_STATE_NONE;
	UINT8	m_ui8ItemNum = 0;										// 보상 아이템 번호 ( 왼쪽이냐 오른쪽이냐 )
	UINT8	m_ui8AttendanceType = 0;								// 마지막으로 진행중이던 이벤트 타입
};

struct RANKUP_ITEM
{
	UINT8		m_ui8Rank = 0;			// Rank
	UINT32		m_ui32GoodsID[4] = { 0 };	// GoodsID
};


// TS 이벤트 리스트 구조체

struct TS_GOODS_LIST
{
	UINT8		_uiGoodsOrder = 0;
	UINT32		_ui32GoodsID[2] = { 0 };
};

struct TS_EVENT_INFO
{
	UINT32			_ui32UID = 0;			// 이벤트 아이디
	TTCHAR			_strTSName[MAX_TS_ITEM_STRING] = { 0 };
	UINT8			_ui8TSType = 0;
	UINT32			_ui32StartDate = 0;
	UINT32			_ui32EndDate = 0;
	TS_GOODS_LIST	stGoodsList[MAX_TS_GOODS_LIST];
};


// TS 이벤트 클라이언트와 통신
struct	TS_EVENT_ITEM_INFO
{
	UINT8		_ui8Type = 0;
	TTCHAR		_strTSName[MAX_TS_ITEM_STRING] = { 0 };
	UINT32		_i32GoodsID[2] = { 0 };
	UINT32		_i32PlayTime = 0;
	UINT32		_i32SelGoodsID = 0;
};

struct USER_INFO_DAILY
{
	UINT16	_match = 0;
	UINT16	_win = 0;
	UINT16	_lose = 0;
	UINT16	_draw = 0;
	UINT16	_killcount = 0;
	UINT16	_headshot = 0;
	UINT16	_death = 0;
	UINT32	_exp = 0;
	UINT32  _point = 0;
	INT32	_playtime = 0;
	UINT8	_ui8GetItem = 0;
	UINT32	_DGPlaytime = 0;
	UINT16  _Assist = 0;

	void Reset()
	{
		_match = _win = _lose = _draw = _killcount = _headshot = _death = _Assist = 0;
		_exp = _point = _DGPlaytime = 0;
		_playtime = 0;
		_ui8GetItem = 0;
	}
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
