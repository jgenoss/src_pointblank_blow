#ifndef __COMMONSERVERDEF_H__
#define __COMMONSERVERDEF_H__

#define CLAN_MATCH_MAKING

extern INT32	g_holdrand;
#define RAND_DK(Seed)	((g_holdrand = ((( g_holdrand * 214013L + 2531011L) >> 16) & 0x7fff )) % (Seed) )

#define MByte 1048576
//#define ENABLE_GACHA_SHOP 

#define	INIT_UINT8		255
#define	INIT_0XFFF		4095
#define	INIT_UINT16		65535
#define	INIT_UINT32		4294967295
#define	INIT_UINT64		-1

#define USE_FINISH_PACKET				// Trans->Game 서버 종결자 패킷 추가
#define FINISH_PACKET				0x4504F046	// EOF
//DataBase							//0x8000F000
#define NET_ERROR_DB_QUERY			0x80001000		// 
#define NET_ERROR_WRONG_RESULT		0x80002000		// 

#define DEF_LINK_LIMIT_TIME			500		// 웹, 빌링단의 느려짐 감지
#define DEF_DB_LIMIT_TIME			500		// DB의 느려짐 감지

#define SERVICE_AUTH_GUEST		0
#define SERVICE_AUTH_ADMIN		1

#define EVENTITEM_LOGIN_SLOT_COUNT	4				// 이벤트 아이템 슬롯 개수
#define EVENT_ITEM_GOODS_COUNT		4

#define TS_EVENT_COUNT			10

#define MAX_SHOP_DATA_BUF		2	// 상점 버퍼 최대 갯수.

#define DEFAULT_ITEM_COUNT			80						// 유저에게 지급 되는 기본 장비

#define GEOIP2_MAX_COUNTRY_COUNT			256

#define IPBLOCK_NATION_OPEN_COUNT			255
#define IPBLOCK_IP_OPEN_COUNT				100

#ifdef _SIA_SERVER
template < class enumtype, class flagtype  >
#else
// UINT64는 두 번째 탬플릿 클래스명을 반드시 적어주시기 바랍니다.
template < class enumtype, class flagtype = UINT32 >
#endif
flagtype g_addFlagByEnum(flagtype flag, enumtype addFlag)
{
	flagtype _mask = 1;
	_mask <<= addFlag;
	flag |= _mask;
	return flag;
}

#ifdef _SIA_SERVER
template < class enumtype, class flagtype  >
#else
// UINT64는 두 번째 탬플릿 클래스명을 반드시 적어주시기 바랍니다.
template < class enumtype, class flagtype = UINT32 >
#endif
flagtype g_removeFlagByEnum(flagtype flag, enumtype rmFlag)
{
	flagtype _mask = 1;
	_mask = ~(_mask << rmFlag);
	return (flag & _mask);
}

#ifdef _SIA_SERVER
template < class enumtype, class flagtype  >
#else
// UINT64는 두 번째 탬플릿 클래스명을 반드시 적어주시기 바랍니다.
template < class enumtype, class flagtype = UINT32 >
#endif
bool g_isIncludedEnumFlag(flagtype flag, enumtype checkFlag)
{
	flagtype _mask = 1;
	_mask <<= checkFlag;
	return 0 != (flag & _mask);
}
typedef char	DIRPATH[MAX_PATH];
typedef TTCHAR	DIRPATH_TT[MAX_PATH];

// HackShield H/W 인증 기능에서 사용하는 값 - 서동권
enum SHOP_VERSION_BUF_TYPE		// Shop 버전 버퍼 타입.
{
	SHOP_VERSION_BUF_TYPE_CUR	=	0,  // 현재 사용중인 버전의 버퍼
	SHOP_VERSION_BUF_TYPE_BACK	,		// 백업 버퍼
};

#define MATCH_TEAM_COUNT_UPDATE_TIME			2	// 매치팀 수 업데이트 시간. (s)

#define MATCHING_ID_INVALID_IDX					0xFFFF

// 당일전적 초기화 시간
#define DAILY_RECORD_INIT_TIME_HOUR				4	
#define DAILY_RECORD_INIT_TIME_MIN				1

// 이벤트
#define USER_EVENT_COUNT						LOCALE_CODE_MAX

#define BOOST_EVENT_COUNT						100										// 서버 이벤트 보너스 카운트
#define BOOST_EVENT_ALL_LOCALE_COUNT			BOOST_EVENT_COUNT*USER_EVENT_COUNT		// 서버 이벤트 보너스 카운트

#define BOOST_EVENT_RANK_ALL_LOCALE_COUNT		RANK_MAX*USER_EVENT_COUNT				// 랭크 Boost 이벤트 카운트

#define NEW_USER_EVENT_COUNT					50
#define NEW_USER_EVENT_ALL_LOCALE_COUNT			NEW_USER_EVENT_COUNT*USER_EVENT_COUNT	// 서버 이벤트 보너스 카운트

#define LOGIN_USER_EVENT_COUNT					50
#define LOGIN_USER_EVENT_ALL_LOCALE_COUNT		LOGIN_USER_EVENT_COUNT*USER_EVENT_COUNT	// 서버 이벤트 보너스 카운트

// GeoIP2
extern char g_astrGeoIP2_country_code[GEOIP2_MAX_COUNTRY_COUNT][3];

#ifdef _DEBUG
#define	USE_RESPAWN_SKIP
#endif

#define WARNING_TYPE_MAX 2 // i3엔진으로 올라가면서 컨트롤 툴에서 사용하기위해 필요합니다. i3엔진에서 서버타입개수가 변경될경우 수정이필요합니다.

#define	SERVER_VALUE_UNKNOWN				-1
#define	MATCH_TEAM_ID_NONE					-1

// 채팅 제재 단계별 제한 시간(sec). 실제로 채팅이 막히는 시간 
extern INT32 g_ai32ChatLimitTime[CHAT_LIMIT_STATE_MAX];

// 채팅 제재 단계별 경고 시간(sec). 채팅은 가능하나, 중복채팅을 할 경우 더 높은 제재가 걸리는 시간 
extern INT32 g_ai32ChatWarningTime[CHAT_LIMIT_STATE_MAX];
 

// Round Start , End 처리 데디에서 진행 여부
#define ROUND_CONTROL_BY_DEDI

#define MAX_DAILY_GIFT_COUNT				3
#define ADD_DAILY_GIFT_RATE					100
#define MAX_DAILY_GIFT_RATE					10000

#define MAX_DAILY_GIFT_GOODS_LIST			20
#define MAX_DAILY_GIFT_GOODS_COUNT			DAILY_GIFT_TYPE_MAX*MAX_DAILY_GIFT_GOODS_LIST

// QA 자동화 커맨드 시스템
#define MAX_COUNT_QA_AUTO_ARG				8

enum QA_AUTO_COMMAND_ID
{
	QA_AUTO_COMMAND_ID_KILL_COUNT,
	QA_AUTO_COMMAND_ID_CHARACTER_MOVE,
	QA_AUTO_COMMAND_ID_DAMAGE_OBJECT,

	QA_AUTO_COMAMND_ID_MAX_COUNT
};


enum SPEND_TYPE	// 포인트, 캐쉬 차감 유형
{
	SPEND_TYPE_BUY_GOODS = 1,	// 구매
	SPEND_TYPE_BUY_MISSIONCARD,	// 임무카드 구매
	SPEND_TYPE_CREATE_CLAN,		// 클랜 생성
	SPEND_TYPE_REPAIR,			// 수리
	SPEND_TYPE_PENALTY,			// 배틀중 이탈
	SPEND_TYPE_BUY_GIFT,		// 선물 구매
	SPEND_TYPE_BUY_WEBSHOP,		// 웹상점 구매
};

// 판수보상 지급 아이템 리스트
struct DAILY_GIFT_GOODS_LIST
{
	UINT32					m_ui32GoodsID;
	INT32					m_i32Rate;
};

struct DAILY_GIFT
{
	bool					m_bActive;								// 컨텐츠 사용 유무( 1 : Active, etc : NotUse )
	//UINT32					m_ui32ResetTime;						// 이벤트 갱신 시간
	
	UINT32					m_ui32PlayTime[ DAILY_GIFT_TYPE_MAX ];	// 보상적용시간

	DAILY_GIFT_GOODS_LIST	m_DailyGiftGoodsList[DAILY_GIFT_TYPE_MAX][MAX_DAILY_GIFT_GOODS_LIST];
};

enum DB_SORT_TYPE		// Database select 시 정렬 방식
{
	DB_SORT_TYPE_ASC	=	0,  // 오름차순
	DB_SORT_TYPE_DESC,			// 내림차순
};


// 순서 변경하지 마세요. SIA_GOODS_BUY랑 맞춰놓음.
enum CHARA_RING_TYPE
{
	CHARA_RING_TYPE_BUY_POINT = 1,
	CHARA_RING_TYPE_BUY_CASH,	// 캐쉬 캐릭터 구매 성공( LINK 결제 성공 )
	CHARA_RING_TYPE_CREATE,
	CHARA_RING_TYPE_DELETE,		
	CHARA_RING_TYPE_USE,		// 캐릭터 사용시작.
};

enum CALCULATE_POINT_VER
{
	CALCULATE_POINT_VER_1,		// 포인트 보상 1
	CALCULATE_POINT_VER_2,		// 포인트 보상 2

	CALCULATE_POINT_VER_MAX,
};

enum REPARE_DURABILITY_VER
{
	REPARE_DURABILITY_VER_1,		// 게임당 1씩 깎이는 구 내구도 시스템
	REPARE_DURABILITY_VER_2,		// 플레이 시간으로 깎는 내구도 시스템

	REPARE_DURABILITY_VER_MAX,
};


// 장성 계급 상대 평가
enum GENERAL_RANK
{
	GENERAL_RANK_47		= 0,		// 소장
	GENERAL_RANK_48,				// 중장
	GENERAL_RANK_49,				// 대장
	GENERAL_RANK_50,				// 원수

	GENERAL_RANK_COUNT,
};

enum ASC_CONNECT_TYPE
{
	ASC_CONNECT_TYPE_UNKNOWN	= 0,
	ASC_CONNECT_TYPE_SERVER		= 1,
	ASC_CONNECT_TYPE_ADMIN		= 2,
	ASC_CONNECT_TYPE_WEB		= 3,

	ASC_CONNECT_TYPE_COUNT,
};


enum ASC_SERVER_TYPE
{
	ASC_SERVER_NONE			= -1,
	ASC_SERVER_TRANS		= 0,
	ASC_SERVER_CLAN			= 1,
	ASC_SERVER_MESSENGER	= 2,
	ASC_SERVER_BATTLE		= 3,
	//ASC_SERVER_DSERVER		= 4,		// Dedication Server로 변경
	ASC_SERVER_UPDATER		= 5,
	ASC_SERVER_CONTROL		= 6,
	ASC_SERVER_ZLOG			= 7,
	ASC_SERVER_GATEWAY		= 8,
	ASC_SERVER_SIA			= 9,
	ASC_SERVER_GAME			= 10,
	//ASC_SERVER_BILLING		= 11,		// 더 이상 사용하지 않습니다.
	ASC_SERVER_CAST			= 12,

	ASC_SERVER_COUNT,
};

#define WEAPON_AUTH_COUNT			1			// 게임 종료시 무기는 한개씩만 내구도가 깎입니다.

//
#define MEDAL_MAX_COUNT				10000			// 메달 카운트 약장, 휘장, 훈장 최대값입니다. 10000 입니다.

#define BASIC_MEMBER_COUNT			50			// 최초 클랜원 수

#define	SLOT_CLAN_MAX_COUNT			10				// 클랜전일때 기본 세팅되는 슬롯 갯수입니다.

// 중독방지 시스템 - 방식
#define AAS_METHOD_DAILY_USAGE			0x01	// 하루이용량 제한 방식
#define AAS_METHOD_APPOINTED			0x02	// 지정시간 금지 방식
#define AAS_METHOD_ACCOUNT_APPOINTED	0x04	// 계정별 지정시간 금지 방식

// 중독방지 시스템 - 금지옵션
#define AAS_BLOCK_READY_START			0x0001	// 레디/시작 버튼 클릭 불가
#define AAS_BLOCK_CREATE_ROOM			0x0002	// 방 생성 불가
#define AAS_BLOCK_JOIN_ROOM				0x0004	// 방 조인 불가
#define AAS_BLOCK_ENTER_SHOP			0x0008	// 상점 진입 불가
#define AAS_BLOCK_ENTER_INVEN			0x0010	// 인벤토리 진입 불가
#define AAS_BLOCK_LOGIN					0x0020	// 로그인 불가
#define AAS_BLOCK_FORCE_LOGOUT			0x0040	// 로그인되어 채널리스트, 로비, 레디룸에 있는 상태라면 강제 로그아웃

#define DAY_COUNT_IN_WEEK				7
#define HOUR_COUNT_IN_DAY				24

////////////////////////////////////////////////////////////////////////////////
//중계, 메신저 서버에서 사용합니다. 
#ifdef I3_DEBUG
#define MAX_CONNECT_USER_COUNT				5000		// 
#define MAX_CONNECT_USER_DIVISOR			31		//20 이상 소수
#else
#define MAX_CONNECT_USER_COUNT				50000		// 
#define MAX_CONNECT_USER_DIVISOR			3001		//20 이상 소수
#endif
////////////////////////////////////////////////////////////////////////////////

enum	SAVE_DATABASE
{	
	SAVE_DATABASE_BASEINFO				= 0x00000001,					// 기본
	SAVE_DATABASE_RECORD				= 0x00000002,					// 전적
	//SAVE_DATABASE_RECORD_RESET			= 0x00000004,					// 전적 초기화
	SAVE_DATABASE_CHARA_COMMON			= 0x00000008,					// 캐릭터 공통정보 변경
	//SAVE_DATABASE_CHARA_MAIN_CHANGE		= 0x00000010,					// 캐릭터 메인슬롯 교체	
	//SAVE_DATABASE_CHARA_INFO_1			= 0x00000020,					// 캐릭터 정보 변경 1슬롯
	//SAVE_DATABASE_CHARA_INFO_2			= 0x00000040,					// 캐릭터 정보 변경 2슬롯
	//SAVE_DATABASE_CHARA_INFO_3			= 0x00000080,					// 캐릭터 정보 변경 3슬롯
	//SAVE_DATABASE_CHARA_INFO_4			= 0x00000100,					// 캐릭터 정보 변경 4슬롯
	//SAVE_DATABASE_CHARA_INFO_5			= 0x00000200,					// 캐릭터 정보 변경 5슬롯
	//SAVE_DATABASE_CHARA_INFO_6			= 0x00000400,					// 캐릭터 정보 변경 6슬롯
	//SAVE_DATABASE_CHARA_INFO_7			= 0x00000800,					// 캐릭터 정보 변경 7슬롯
	//SAVE_DATABASE_CHARA_INFO_8			= 0x00001000,					// 캐릭터 정보 변경 8슬롯
	//SAVE_DATABASE_CHARA_INFO_9			= 0x00002000,					// 캐릭터 정보 변경 9슬롯
	SAVE_DATABASE_CLAN					= 0x00004000,					// 클랜변경
	SAVE_DATABASE_ATTENDANCE			= 0x00008000,					// 출석체크
	SAVE_DATABASE_MEDAL_BASIC			= 0x00010000,					// 메달 기본 정보. 알리미 메달.
	SAVE_DATABASE_MEDAL					= 0x00020000,					// 메달 ★ 주의 : 일반 메달 저장 플래그 킬떄는 MEDAL_GT 에 m_ui8IsSave도 반드시 켜줘야 합니다 ★
	SAVE_DATABASE_CUR_MEDAL_SET			= 0x00040000,					// 진행중인 메달 셋트.
	SAVE_DATABASE_DOMI_SCORE			= 0x00080000,					// 제압모드 하이스코어
	SAVE_DATABASE_GACHA_RECORD			= 0x00100000,					// Gacha 구매 이력 저장
	SAVE_DATABASE_ITEM					= 0x00200000,					// 계정아이템( 위장 계급, 위장닉네임 등 )사용정보 저장
	SAVE_DATABASE_QUEST					= 0x00400000,					// 퀘스트( 구 SAVE_DATABASE_MEDAL )
	SAVE_DATABASE_QUICK_JOIN			= 0x00800000,					// 빠른 입장.
	SAVE_DATABASE_AAS					= 0x01000000,					// 중독방지시스템
	SAVE_DATABASE_LOGIN_EVENT			= 0x02000000,					// 로그인 이벤트

	SAVE_DATABASE_ALL					= 0x7FFFFFFF,
};

//#define SAVE_DATABASE_CHARA_INFO_ALL	(SAVE_DATABASE_CHARA_INFO_1|SAVE_DATABASE_CHARA_INFO_2|SAVE_DATABASE_CHARA_INFO_3|SAVE_DATABASE_CHARA_INFO_4|SAVE_DATABASE_CHARA_INFO_5|SAVE_DATABASE_CHARA_INFO_6|SAVE_DATABASE_CHARA_INFO_7|SAVE_DATABASE_CHARA_INFO_8|SAVE_DATABASE_CHARA_INFO_9)
#define DEF_TRANS_LINK_COUNT_MAX		100					// Trans Link Thread Max

//////////////////////////////////////////////////////////////////////////
// 인게임 관련
//////////////////////////////////////////////////////////////////////////

//enum ENUM_LOAD_START_TYPE
//{
//	LOAD_START_NONE = 0,
//	LOAD_START_CREATE,
//	LOAD_START_ENTER,
//	LOAD_START_GIVEUP,
//	LOAD_START_END,
//	LOAD_START_SUCCESS,
//
//	MAX_LOAD_START_TYPE
//};

////////////////////////////////////////////////////////////////////////////////
// 여기서부터는 
// 서버 Debug Test시 사용합니다.

// 기획 미정의 사항.
// 기획 정의 및 기능 필요시 사용 예정
#define USE_MESS_FIND
//#define USE_TRANS_FIND	--사용안함

#pragma pack( push, 1 )

enum SERVER_ACTIVE_STATE
{
	SERVER_ACTIVE_STATE_DISCONNECT = 0,			// 검은색
	SERVER_ACTIVE_STATE_CONNECT,				// 녹색
	SERVER_ACTIVE_STATE_ERROR,					// 빨간색
	SERVER_ACTIVE_STATE_NOT_ALLOW_CREATE_ROOM,	// 하늘색
	SERVER_ACTIVE_STATE_LOADING,				// 노란색

	SERVER_ACTIVE_STATE_COUNT,
};

struct TYPE_ROOM_BONUS
{
	BOOL	_bActive;
	REAL32	_r32Exp;
	REAL32	_r32Point;	
};

struct TYPE_STAGE_BONUS : TYPE_ROOM_BONUS
{	
	UINT32	_ui32StageID;
};

#ifndef _CONTROL_TOOL_

// Event Bouns Setting
// DB에 세팅 된 값이며 Fix 된 값입니다. 값이 변경 되면 절!!대!! 안됩니다.
enum EVENT_BONUS_TYPE
{
	EVENT_BONUS_NORMAL = 1,					// Normal	
	EVENT_BONUS_PC_CAFE = 2,				// PC List
	EVENT_BONUS_CHANNEL = 3,				// Channel List	
	EVENT_BONUS_NATION = 4,					// 국가 코드
	EVENT_BONUS_RANK = 5,					// RANK_MAX;
	EVENT_BONUS_RULE = 6,					// STAGE_MODE_MAX
	EVENT_BONUS_STAGE = 7,					// STAGE_UID_MAX
	EVENT_BONUS_MATCHING = 8,				// MapRuleStageMatching 테이블 Row수
	EVENT_BONUS_RANDOM_MAP = 9,				// RANDOM MAP 
	EVENT_BONUS_MAX,
};

struct BOOST_EVENT
{	
	UINT8					m_ui8LocaleCode;
	UINT16					m_ui16Type;
	DATE32					m_dt32StartDate;
	DATE32					m_dt32EndDate;
	UINT32					m_ui32Point;
	UINT32					m_ui32Exp;
};

struct NEW_USER_EVENT
{
	UINT8					m_ui8LocaleCode;
	DATE32					m_dt32StartDate;
	DATE32					m_dt32EndDate;
	T_GoodsID				m_ui32GoodsID;//[ EVENT_ITEM_GOODS_COUNT ];
};

struct LOGIN_USER_EVENT
{
	UINT8					m_ui8LocaleCode;
	UINT8					m_ui8Slot;
	DATE32					m_dt32StartDate;
	DATE32					m_dt32EndDate;
	UINT8					m_ui8GoodsCount;
	T_GoodsID				m_ui32GoodsID[ EVENT_ITEM_GOODS_COUNT ];
};

#endif

struct DSERVER_ROOM_INFO // duplicated! 
{
	UINT8	_cBattle;				// relay server index
	UINT8	_cGame;					// game server index
	UINT8	_cChannel;				// channel index
	UINT16	_sRoom;					// room index
	void	Reset()
	{
		_cBattle	= 0;
		_cGame		= 0;
		_cChannel	= 0;
		_sRoom		= 0;
	}
	void	Copy( DSERVER_ROOM_INFO* pDes )
	{
		_cBattle	= pDes->_cBattle;
		_cGame		= pDes->_cGame;
		_cChannel	= pDes->_cChannel;
		_sRoom		= pDes->_sRoom;
	}
};

struct P_BATTLE_HACK_INFO
{
	T_UID				_i64UID;
	UINT32				_uSlotIdx;
	UINT32				_uReason;
} ;

struct P_SERVER_STATE_REQ
{
	UINT32				_ui32Active;
	INT32				_i32Cpu;
	INT32				_i32Handle;
	INT32				_i32Arg0;
	INT32				_i32Arg1;
	INT32				_i32Arg2;
	INT32				_i32Arg3;
	INT32				_i32Arg4;
	UINT32				_ui32WarningInfo[ WARNING_TYPE_MAX ];
};

struct SERVER_INFO_COMMON
{
	SS_VERSION			_ServerVersion;
	INT32				_i32IsActive;
	UINT32				_ui32Ip;			//공용	
	UINT16				_ui16Port;			//공용	
	UINT8				_ui8Type;			//공용
	INT32				_i32Cpu;			//공용
	INT32				_i32Handle;			//공용
	INT32				_i32Arg0;			//
	INT32				_i32Arg1;			//
	INT32				_i32Arg2;			//
	INT32				_i32Arg3;			//
	INT32				_i32Arg4;			//

	UINT32				_ui32WarningInfo[ WARNING_TYPE_MAX ];
};

struct PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ
{
	EXTEND_BASKET		m_ExtendItemData;		// 연장할 기간에 대한 상세 데이터(상품을 던지면 구매와 동일하게 처리)
};

struct PACKET_H_AUTH_NEW_SHOP_ITEM_EXTEND_ACK
{
	UINT32				m_ui32InvenTime;

	UINT32				m_ui32Point;
	UINT32				m_ui32Cash;	

	TItemCnt			m_ui16CharaCount;
	TItemCnt			m_ui16WeaponCount;
	TItemCnt			m_ui16ItemCount;
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_START_REQ		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_START_REQ		// 빠른 입장 시작 요청. Client -> Game
{
	UINT8			m_ui8QuickJoinIdx;		// m_aQuickJoinInfo 중 실제로 검색할 조건. 0~MAX_QUICK_JOIN_INFO_COUNT-1 까지 유효. 
	QUICKJOIN_INFO	m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];	// 빠른입장 설정 정보. 전체가 DB에 저장됨.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_START_ACK	 
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_START_ACK		// 빠른 입장 시작 응답. Game -> Client
{
	INT32			m_i32Rv;			// 결과값. EVENT_ERROR_SUCCESS : 성공, EVENT_ERROR_QUICK_JOIN_2ND_BEST : 차선책 권유, 그외 : 실패
	QUICKJOIN_INFO	m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];	// 빠른입장 설정 정보.

	// m_i32Rv이 EVENT_ERROR_SUCCESS 또는 EVENT_ERROR_QUICK_JOIN_2ND_BEST 일때만 유효.
	UINT8			m_ui8ChannelIdx;	// 입장 성공/권유 채널번호.	
	UINT32			m_ui32RoomIdx;		// 입장 성공/권유 방 번호.	

	UINT8			m_ui8ChannelType;	// 채널 타입.ChannelType
	UINT8			m_ui8RoomState;		// 방상태. ROOM_STATE

	// m_i32Rv이 EVENT_ERROR_QUICK_JOIN_2ND_BEST 일때만 유효.
	ROOM_INFO_2ND_BEST 	m_QuickJoin;			// 차선책 추천방 정보.
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ		// 차선책 방 입장 요청. Client -> Game
{
	UINT32			m_ui32ChannelIdx;	// 채널번호.	
	UINT32			m_ui32RoomIdx;		// 방 번호.		
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK		// 차선책 방 입장 요청.  Game -> Client
{
	INT32			m_i32Rv;			// 결과값. EVENT_ERROR_SUCCESS : 성공,  그외 : 실패
	UINT32			m_ui32ChannelIdx;	// 채널번호.	
	UINT32			m_ui32RoomIdx;		// 방 번호.		
};

struct PACKET_AUTH_NEW_EQUIPMENT_REPAIR_REQ
{
	UINT8			m_ui8CharaSlotIdx;		// 캐릭터 슬롯 번호(배열번호와 동일)
};

struct PACKET_GT_D_AUTH_NEW_SHOP_ITEM_REPAIR
{	
	T_ItemDBIdx		m_TItemWareIdx;
	UINT32			m_ui32Point;	
	UINT32			m_ui32Cash;
};

typedef struct __tag_ClanResult
{
	UINT32		_ClanIdx;							// Clan 인덱스
	UINT32		_Exp;								// 경험치	
	UINT8		_Win;								// 승수
	UINT8		_Lose;								// 패수

	UINT8		_RoundWinCount;						// 라운드 승수
}CLAN_RESULT;

struct CLAN_DETAIL_RESULT	
{
	UINT8			m_ui8StageMode;				// 스테이지 모드 ( STAGE_MODE )	
	UINT32			m_ui32StageID;				// STAGE_ID
	T_MatchingID	m_TMatchingID;				// 매칭 ID
	UINT8			m_ui8CurPlayer;				// 현재인원수
	UINT8			m_ui8MaxPlayer;				// 최대인원수
	UINT8			m_ui8IsMatchMaking;			// 매치메이킹여부
};

// Vote
struct VoteLimit
{
	UINT8	m_ui8MaxVoteCount;		//일일 최대 신고 횟수 제한
	UINT8	m_ui8MinRank;			//신고하기 최소 Rank 제한
	UINT32	m_ui32ReportGuardTime;	//피신고 보호 시간
};

// 채팅 도배 방지
struct ChatHackLimit
{
	UINT8					m_ui8Size;			// 핵으로 판단하는 채팅 사이즈
	UINT8					m_ui8Count;			// 핵으로 판단하는 기준시간.( sec )
	UINT8					m_ui8Time;			// 핵으로 판단하는 기준 채팅수( 패킷수 )
	UINT32					m_ui32LimitTime;	// 채팅금지시간(sec)
};			

// To change Hack check rate (Control tool -> control Server-> DediServer)
struct ChangedHackCheckRatio
{
	UINT8			m_ui8ServerType;
	UINT8			m_ui8HackCheckType;
	UINT8			m_ui8ServerIdx;
	UINT8			m_ui8ChangedCheckRatio;
};

struct LOGIN_INFO
{
	T_UID					m_TUID;
	UINT8					m_ui8LinkThreadIdx;
	UINT32					m_ui32LoginDelayTime;
	char					m_strID[ NET_ID_SIZE ];
	char					m_strcustom[ NORTH_CUSTOM_MSG_SIZE ];
	char					m_strBillingID[ BILLING_ID_SIZE ];
	UINT32					m_ui32BlockDate;
	UINT8					m_ui8LocaleCode;
	TTCHAR					m_strUserNoticeComment[BLOCK_COMMENT_SIZE]; //추가
	INT32					m_i32PenaltyAmount;	//추가

	char					m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
	char					m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)

	LOGIN_INFO::LOGIN_INFO()
	{
		m_strID[0]			= '\0';
		m_strcustom[0]		= '\0';
		m_strBillingID[0]	= '\0';
		m_strUserNoticeComment[0] = '\0';

		m_strExtUID[0] = '\0';
		m_strWebLoginKey[0] = '\0';
	}
};

// New Durability System 
struct USED_ITEM
{
	T_ItemDBIdx					m_TUsedItemDBIdx;			// Used Item in a game match
	UINT16						m_ui16UsedDurablity;	// The durability consumption quantity after a game match
};

struct USED_ITEM_LIST
{
	T_ItemDBIdx					m_TUsedItemDBIdx;		// Used Item in a game match
	UINT32						m_ui32ItemID;
	UINT8						m_ui8Type;
	UINT32						m_ui32UsedDurablity;	// The durability consumption quantity after a game match
	UINT8						m_ui8CheckSlotInfo;
};

#ifndef _CONTROL_TOOL_
// Trans - Game 에서 사용되는 유저 정보
struct USER_INFO_SERVER
{
	UINT32					m_ui32TotalBattleTime;
	UINT32					m_ui32LoginBattleTime;
	UINT32					m_ui32LastBuyCash;
	UINT8					m_ui8ReportCount;
	UINT32					m_ui32ChatBlockDate;
	UINT8					m_ui8VoteCount;
	DATE32					m_dt32HackChatBlockDate;
};
#endif

// New 유저 찾기용 구조체 서버용
struct	NEW_FIND_USER_INFO_SEV
{
	INT32							m_i32Result;			// 찾기 결과값.
	INT32							m_i32RUserSessionIdx;	// 찾기 요청한 유저 Game서버 내 세션 Idx.
	T_UID							m_i64RUserUID;			// 찾기 요청한 유저 UID.
	INT32							m_i32FUserSessionIdx;	// 찾고자 하는 유저 Game서버 내 세션 Idx.
	T_UID							m_i64FUserUID;			// 찾고자 하는 유저 UID.
	NEW_FIND_USER_INFO				m_NewFindUserInfo;	// 찾고자 하는 유저 상세 정보.
};

struct PACKET_GT_H_AUTH_NEW_SHOP_ITEM_EXTEND_ACK
{
	T_UID			m_i64UID;
	INT32			m_iRv;
	T_ItemDBIdx		m_TWareDBIdx;	
	UINT32			m_ui32GoodsID;
	UINT8			m_ui8BuyType;
};

struct PACKET_SHOP_GOODS_BUY_ACK
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	INT32	m_i32Rv;
	UINT32	m_ui32GoodsID;
	UINT8	m_ui8BuyType;
};

struct	PACKET_GM_BASE_FIND_NICK_USER_POSITION_REQ				// 닉네임으로 유저 간략 정보 요청
{
	INT32				m_i32SessionIdx;
	T_UID				m_i64UID;
	char				m_strNick[NET_NICK_NAME_SIZE];
};

struct	PACKET_GM_BASE_USER_POSITION_ACK
{
	INT32				m_i32Rv;								// 성공이면 진행 실패면 무시
	INT32				m_i32SessionIdx;
	T_UID				m_i64UID;								// 찾기를 요청한 유저 UID
	T_UID				m_i64FindUID;							// 찾기 요청당한 유저 UID
	//UINT32				m_ui32UserConnectState;				// 유저 현재 위치(MAKE_FRIEND_STATE)

	INT32				m_State;
	INT32				m_ServerIdx;
	INT32				m_ChannelNum;
	INT32				m_RoomIdx;
};

struct PACKET_GC_NEW_CLAN_USER_INFO_ACK
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	T_UID	m_i64FindUID;
	UINT32	m_ui32ClanDBIdx;
	UINT32	m_clanMark;
	TTCHAR	m_strClanName[NET_CLAN_NAME_SIZE];
};

struct PACKET_GC_NEW_CLAN_USER_INFO_REQ
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	T_UID	m_i64FindUID;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_NEW_CLAN_MATCH_RESULT_CONTEXT_REQ
// 클랜전 매치 정보 보기 목록 정보 요청
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_CS_CLAN_MATCH_RESULT_CONTEXT_REQ
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	UINT32	m_ui32CUID;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_NEW_CLAN_MATCH_RESULT_LIST_REQ		클랜전 매치 정보 보기 요청
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_CS_CLAN_MATCH_RESULT_LIST_REQ
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	UINT32	m_ui32CUID;
	UINT8	m_ui8iSection;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_NEW_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ		클랜전 매치 정보 감정표현(좋아요) 요청
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	UINT32	m_ui32CUID;								// Clan No

	UINT8	m_ui8MatchCount;						// 클랜 결과 중 선택한 기록의 Index
	UINT8	m_ui8Emotion;							// 클랜 매치 결과에 대한 감정표현(좋아요(1),맙소사(2))
	TTCHAR	m_strUserNick[NET_NICK_NAME_SIZE];		// 닉네임
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_NEW_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ		클랜 가입신청서 (자동,수동) 승인 설정 요청(클랜마스터만 가능)
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	UINT32	m_ui32CUID;								// Clan No

	UINT32	m_ui32AutoAcceptOption;					// 자동, 수동 승인 설정(클랜마스터만 가능)
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_MATCH_NEW_CLAN_INFO_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_D_CS_MATCH_NEW_CLAN_INFO_REQ
{
	UINT32	m_ui32CUID[TEAM_COUNT];					// Team Clan No(Red(0),Blue(1))	
	UINT32	m_ui32ChannelNum;
	UINT32	m_ui32RoomIdx;
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_MATCH_NEW_OPERATION_REQ
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_D_CS_MATCH_NEW_OPERATION
{
	TTCHAR	m_strOperation[NET_CLAN_OPERATION_SIZE];
};

//////////////////////////////////////////////////////////////////////////
// ACK 공통 부분 처리
//////////////////////////////////////////////////////////////////////////
struct PACKET_GC_H_CLAN_ACK
{
	INT32	m_i32SessionIdx;
	T_UID	m_i64UID;
	INT32	m_i32Result;		// 성공이면 해당 데이터 받음.
};

struct	PACKET_GT_BASE_FIND_UID_USER_BASIC_INFO_REQ				// UID로 유저 간략 정보 요청
{
	INT32				m_i32SessionIdx;						// 자신의 세션번호
	T_UID				m_i64UID;								// 자신의 UID
	T_UID				m_i64FindUID;							// 찾을 유저
};

struct	PACKET_GT_BASE_USER_BASIC_INFO_ACK
{
	INT32				m_i32Rv;								// 성공이면 진행 실패면 무시

	INT32				m_i32SessionIdx;
	T_UID				m_i64UID;								// 자신의 UID
	T_UID				m_i64FindUID;							// 찾을 유저

	UINT32				m_rank;
	UINT32				m_exp;
	char				m_nickName[NET_NICK_NAME_SIZE];
	UINT32				m_killCount;
	UINT32				m_deathCount;
};

struct FIND_USER_INFO
{
	INT32						m_FUState;									// 찾는 유저 상태
	INT32						m_FUServerIdx;								// 찾는 유저 서버 위치
	INT32						m_FUChannelNum;								// 찾는 유저 채널 위치
	INT32						m_FURoomIdx;								// 찾는 유저 룸   위치

	UINT32						m_FUClanMark;								// 찾는 유저의 클랜 마크
	TTCHAR						m_FUClanName[NET_CLAN_NAME_SIZE];			// 찾는 유저의 클랜 이름
	
	//////////////////////////////////////////////////////////////////////////
	void	Reset()
	{
		m_FUClanMark		= CLAN_MARK_DEFAULT;
		m_FUState			= FRIEND_STATE_OFFLINE;
		m_FUServerIdx		= INIT_UINT8;
		m_FUChannelNum		= INIT_UINT8;
		m_FURoomIdx			= INIT_0XFFF;
		m_FUClanName[0]		= 0;
	}
};

// 쫌 복잡하긴 한데.. 메모리 관리 차원에서 이렇게 갑니다.. 너무 복잡함?? 20131206 김현우
enum DEFAULT_ITEM_TYPE
{
	DEFAULT_ITEM_TYPE_NORMAL,
	DEFAULT_ITEM_TYPE_PCCAFE,					// 이후의 값은 (PC_CAFE_TYPE) 의 값을 참조 합니다.
	DEFAULT_ITEM_TYPE_PCCAFE_END	= (PC_MAX_COUNT-1) + DEFAULT_ITEM_TYPE_PCCAFE,	// -1을 하는 이유는 PCCafe 가 PC_NOT_CAFE 부터 시작하기 때문입니다.
									//= DEFAULT_ITEM_TYPE_PCCAFE_END,
	DEFAULT_ITEM_TYPE_MAX			= DEFAULT_ITEM_TYPE_PCCAFE_END,
};

struct DEFAULT_ITEM
{
	UINT8					m_ui8Type;						// Default Item Type
	T_ItemID				m_TItemID;						// Default Item
};

struct INVEN_BUFFER_SET
{
	UINT8			ui8ChangeType;
	UINT16			m_ui16InvenCount;
	INVEN_BUFFER	m_Inventory[MAX_INVEN_COUNT];

	INVEN_BUFFER_SET::INVEN_BUFFER_SET()
	{
		m_ui16InvenCount = 0;
	}
};

struct IPBLOCK_IP
{
	UINT32					m_ui32StartIP;
	UINT32					m_ui32EndIP;
};

struct AAS_OPTION_DATA
{
	UINT8			m_ui8AASEnableFlag;				// 기능사용 비트조합	
	UINT16			m_ui16AASProhibitionFlag;		// 금지내용 비트조합
	UINT16			m_ui16AASDailyUsageMaxMin;		// 하루이용량제한방식 : 하루이용가능한 시간(분)
	UINT16			m_ui16AASDailyUsageResetTime;	// 하루이용량제한방식 : 리셋시간
	UINT64			m_ui64AASAppointedTimeTable;	// 지정시간금지방식시간대 비트조합
};

struct	AUTH_GIFT_INVEN	: public AUTH_GIFT
{
	T_GoodsID				m_TGoodsID;				// 선물 GoodsID. 로그용
	UINT8					m_ui8ItemCount;
	INVEN_BUFFER			m_Inven[MAX_SHOP_GOODS_ITEM_COUNT];
};

struct	REWARD_ITEM
{
	UINT8					m_ui8RequestType;	//	ITEM_INSERT_REQUEST_TYPE
	UINT8					m_ui8GoodsCount;	// 선물갯수
	T_GoodsID				m_aTGoodsID[MAX_SHOP_GOODS_BUY_COUNT];
};

struct MapPriorityData
{
	UINT16		m_ui16MapOrd;		//MapOrd
	UINT16		m_ui16Priority;		//WebTool에서 설정된 우선순위
};

#ifndef _CONTROL_TOOL_
struct	ATTENDANCE_USER_SVR : public ATTENDANCE_USER
{
	DATE32					m_dt32AttendanceTime;	// 마지막으로 출석체크한 시간
	bool					m_bAttendanceTarget;	// 유저가 이벤트 대상인지( 대상 == true, 대상아님 == false )
};
#endif

struct	TEAM_MEMBER_IDX
{
	T_UID		m_TUID;				// UID
	INT32		m_i32SessionIdx;	// 세션 Idx
	INT8		m_i8Slot;			// 매치팀 슬롯 Idx.
};

struct GENERAL_RANK_RESULT
{
	T_UID	m_TUID;
	UINT32	m_ui32ChangeRank;
};

struct WORK_PROFILE
{
	UINT64				m_ui64Packet;	// 처리한 패킷 갯수
	UINT64				m_ui64Work;		// 처리한 일 갯수
	REAL64				m_r64Time;		// 처리시간.

	WORK_PROFILE::WORK_PROFILE()
	{
		m_ui64Packet = 0;
		m_ui64Work = 0;
		m_r64Time = 0.f;
	}

	void		Add(WORK_PROFILE * pDest)
	{
		m_ui64Packet += pDest->m_ui64Packet;
		m_ui64Work += pDest->m_ui64Work;
		m_r64Time += pDest->m_r64Time;
	}


	void		CopyTo(WORK_PROFILE * pDest)
	{
		pDest->m_ui64Packet = m_ui64Packet;
		pDest->m_ui64Work = m_ui64Work;
		pDest->m_r64Time = m_r64Time;
	}

	void		UpdateLogProfile(WORK_PROFILE * pCurrent, WORK_PROFILE * pLog)
	{
		pLog->m_ui64Packet = pCurrent->m_ui64Packet - m_ui64Packet;
		pLog->m_ui64Work = pCurrent->m_ui64Work - m_ui64Work;
		pLog->m_r64Time = pCurrent->m_r64Time - m_r64Time;

		m_ui64Packet = pCurrent->m_ui64Packet;
		m_ui64Work = pCurrent->m_ui64Work;
		m_r64Time = pCurrent->m_r64Time;
	}
};

#define MAX_GENERAL_ITEM_COUNT		8

#pragma pack( pop )

#ifdef I3_DEBUG
extern		char*				g_preMacroDateToInt();
#endif
extern		INT32				g_GetBuildCount();

// QuickJoin
extern		UINT32				g_GetStageOption_WithUINT8(UINT8 ui8StageOption );

extern		void				g_MakeBasicItemDBIdx( T_ItemDBIdx& TItemDBIdx, T_ItemID TItemID );

extern TTCHAR g_TCharaName[ TEAM_COUNT ][ NET_NICK_NAME_SIZE ];

extern T_ItemID g_TCharEquipBasic[ TEAM_COUNT ];

#ifdef _DEBUG
#ifndef _CONTROL_TOOL_
extern void g_PrintProtocolName(PROTOCOL protocol, const char *pSenderName = "????" );
#endif
#endif

#endif
