#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
enum KILL_REASON
{
	KILL_REASON_NO					= 0,
	KILL_REASON_UNKNOWN_PACKET		= 1,		// 잘못된 패킷
	KILL_REASON_LOGTIMEOUT			= 2,		// Login 타임아웃
	KILL_REASON_GAME_GUARD			= 3,		// GameGuard 에러
	KILL_REASON_NOT_ENCRIPT_PACKET	= 4,		// 암호화되지 않는 패킷
	KILL_REASON_SENDPACKET_ERROR	= 5,		// SendPacket 에러
	KILL_REASON_NORMAL_TIMEOUT		= 6,		// 기본적인 타임아웃
	KILL_REASON_HACK_PACKET			= 7,		// 잘못된 패킷
	KILL_REASON_SERVER_MOVE			= 8,		// 서버이동
	KILL_REASON_DB_ERROR			= 9,		// DB 에러
	KILL_REASON_RECVPACKET_ERROR	= 10,		// 받은 패킷 에러
	KILL_REASON_CHANNELTIMEOUT		= 11,		// 체널선택화면에서 타임아웃
	KILL_REASON_USERKICK			= 12,		// Control, Trans 에서 받은 일반적인 유저 킥
	KILL_REASON_VER_ERROR			= 13,		// 버전 틀림
	KILL_REASON_USER_CHECK_TIME		= 14,		// 체크하는 패킷을 보내지 않음 
	KILL_REASON_MD5_ERROR			= 15,		// 체크하는 패킷을 보내지 않음
	/////////////////////////////////////////////////
	//피로도 시스템
	KILL_REASON_FCM_INFO_ERROR		= 16,		// 피로도 실명인증 처리 타임 아웃
	KILL_REASON_FCM_LOGIN_ERROR		= 17,		// 피로도 시스템 로그인 실패
	KILL_REASON_FCM_STATE_ERROR		= 18,		// 피로도 시스템 누적값 요청 타임 아웃
	/////////////////////////////////////////////////
	KILL_REASON_PACKET_COUNT		= 19,		//
	KILL_REASON_CLIENT_HACK_CHECK	= 20,		//
	KILL_REASON_LOGOUT				= 21,		// 정상적인 게임 종료
	KILL_REASON_WRONG_LOGOUT		= 22,		// 비정상적인 게임 종료(로그아웃 패킷에 의한 종료가 아님)
	KILL_REASON_HACK_CLIENT			= 23,		// 클라이언트에서 감지한 해킹
	KILL_REASON_HACK_SERVER_D		= 24,		// 데디에서 감지한 해킹
	KILL_REASON_HACK_SERVER_G		= 25,		// 게임서버에서 감지한 해킹
};

/////////////////////////////////////////////////
enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

//-----------------------------------------------------
//Common
#define NO_STR_TBL					//스트링 테이블 사용하지 않는다. 

#if defined(BUILD_QA_VERSION)
	//-----------------------------------------------------
	//QA용 빌드 입니다.
	#define	BUILD_VER				"QA"
	#define USE_TEST_PACKET			//테스트에서만 사용합니다. 엔씨 QA팀 요청 	
	//-----------------------------------------------------
#elif defined(BUILD_DIST_VERSION)
	//-----------------------------------------------------
	//배포판 빌드
	#define	BUILD_VER				"DIST"
	//-----------------------------------------------------
#elif defined(BUILD_MATCH_VERSION)	//대회빌드옵션 추가		2011.08.30 안국정
	//-----------------------------------------------------
	//대회용 빌드
	#define BUILD_VER				"MATCH"
	//-----------------------------------------------------
#else
	//-----------------------------------------------------
	//디버깅용 
	#define	BUILD_VER				"DEBUG"
	#define USE_TEST_PACKET			//테스트에서만 사용합니다. 엔씨 QA팀 요청 
	//#define USE_SOLO_PLAY			//혼자도 플레이가 가능하도록 만든다.
	#define CLAN_MATCH_SOLO_PLAY	//클랜서버에서 혼자서도 가능하게 만든다.
	//#define TRACE_GIP_PACKET		//GIP를 TRACE로 출력
	//-----------------------------------------------------
	#define NEW_EXPPOINT	
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY		// 주무기 장착을 검사하지 않는다.
	//-----------------------------------------------------
#endif

#if defined( LOCALE_KOREA) 	
	#define USE_NPROTECT
	
#elif defined( LOCALE_THAILAND)			// 태국버전	
	#define USE_AHNHS
	
#elif defined( LOCALE_INDONESIA)		// 인도네시아	
	#define USE_AHNHS
	
#elif defined( LOCALE_RUSSIA)			// 러시아
#elif defined( LOCALE_TURKEY)			// 터키 	
	#define USE_AHNHS
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY		// 주무기 장착을 검사하지 않는다.
	
#elif defined( LOCALE_BRAZIL)			// 브라질
	#define USE_AHNHS
	//#define USE_XTRAP
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY		// 주무기 장착을 검사하지 않는다.
	
#elif defined( LOCALE_LATIN_AMERICA)	// 중남미
	#define USE_XTRAP								// 중남미 핵실드 - > XTrap 으로 변경
	//#define USE_AHNHS
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY		// 주무기 장착을 검사하지 않는다.
	
#elif defined( LOCALE_NORTHAMERICA)		// 북미	
	#define USE_AHNHS
	
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY		// 주무기 장착을 검사하지 않는다.
#elif defined( LOCALE_ITALY)			// 이탈리아
	#define USE_AHNHS
	#define DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY
	
#elif defined( LOCALE_MIDDLE_EAST)		// 중동
	#define USE_AHNHS
	#define NEW_EXPPOINT
	
#elif defined( LOCALE_PHILIPPINES)		// 필리핀
	#define USE_AHNHS
	
#elif defined( LOCALE_TAIWAN)			// 대만
	#define USE_AHNHS
	#define NEW_EXPPOINT
	
#endif

//#define USE_NPROTECT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//개발 진행중 기능 TEST - 기능완료후 제거

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//서버에서 사용하는 값 Define 
#define SESSION_CHECK_COUNT								300			//
#define SESSION_CHECK_TIMEOUT_NORMAL					60 * 2		//
#define SESSION_CHECK_TIMEOUT_CHANNEL					60 * 60		//채널 선택화면에서 5분 동안 있을수 있다.
#ifdef LOGIN_DELAY
#define SESSION_CHECK_TIMEOUT_LOGIN						60 * 10		//로그인 타임 아웃 
#else
#define SESSION_CHECK_TIMEOUT_LOGIN						60			//로그인 타임 아웃 
#endif
#if defined( LOCALE_RUSSIA )
#define SESSION_CHECK_TIMEOUT_NOT_LOGIN					40			//접속후 로그인 안한 유저 타임 아웃 
#else
#define SESSION_CHECK_TIMEOUT_NOT_LOGIN					5			//접속후 로그인 안한 유저 타임 아웃 
#endif

#define SESSION_CHECK_TIMEOUT_GETINFO					10			//로그인 타임 아웃 
#define SESSION_DISCONNECT_TIME							2			//kill메시지를 받으면 5초뒤에 삭제 

#define SESSION_CHECK_TIMEOUT_GAMEGUARD					30			//Send를 보낸후 10초동안 Recv오지 않으면 타임아웃.
#define SESSION_CHECK_DATABASE_UPDATE_TIME				60 * 60		//30분 마다 데이터 베이스에 유저정보를 업데이트 합니다. 
#define SESSION_CHECK_DATABASE_UPDATE_TIME_ACCIDENTAL	60 * 10		//10분의 편차를 가지고 있다. 
#define SESSION_CHECK_LOGINWAIT_SENDTIME				10			// 10초에 한번씩 로그인 Wait Packet 를 전송

#define SERVER_TYPE_GATEWAY				0
#define SERVER_TYPE_GAME				1

#define KICK_LIST_IN_ROOM_MAX			10

/////////////////////////////////////////////////
//Time Out 
#define	LOADING_TIME_OUT				60	
#define	RELAY_TIME_OUT					10	
#define	PREGAME_TIME_OUT				20
#define	BATTLE_TIME_OUT					60 * 60	* 3		//3시간 동안 배틀이 끝나지 않으면.. 문제가 있는 방이에용..
#define BATTLE_SEND_PING_TIME			5	

//LOG UPDATE TIME
#define	LOG_UPDATE_TIME_STATE			9			//10초
#define	LOG_UPDATE_TIME_CHANNEL			0			//1초마다 한 채널씩 보내요.
#define	GAMEGUARD_UPDATE_TIME			60 * 5		//5분 권장

enum ROOM_ENDTYPE
{
	ROOM_ENDTYPE_NONE			= 0,
	ROOM_ENDTYPE_WIN,
	ROOM_ENDTYPE_LOSE,
	ROOM_ENDTYPE_DRAW,
	ROOM_ENDTYPE_DISCONNECT,
	ROOM_ENDTYPE_GIVEUP,
	ROOM_ENDTYPE_HACKING,
	ROOM_ENDTYPE_GM_KICK,
	ROOM_ENDTYPE_VOTE,
};

#define GAMEGUARD_NPROTECT					1
#define GAMEGUARD_HACKSHIELD				2
#define GAMEGUARD_XTRAP						3
#define GAMEGUARD_GPK						4

//					  변수,체크값
#define LENGTH_CHECK( val, check )		{ if(val>check)	{val=check;} }

// Trans 서버에 보내는 LoginKey 값입니다.
#define SET_LOGIN_KEY( thread, value )	((thread<<24)|value)
#define GET_LOGIN_KEY_VALUE( key )		(key&0x00FFFFFF)

//-----------------------------------------------------------------------------------------------------
// Training
extern INT32 g_ai32trainingKillPoint[TRAINING_DIFFICULTY_LEVEL_HIGH + 1];

//-----------------------------------------------------------------------------------------------------
//Server에서만 사용하는 Define 
#define SERVER_ERROR_NETWORK			0X80000001
#define SERVER_ERROR_CONFIG				0X80000002
#define SERVER_ERROR_LOGFILE			0X80000003
#define SERVER_ERROR_DATABASE			0X80000004
#define SERVER_ERROR_CONTEXT			0X80000005
#define SERVER_ERROR_HOLE				0X80000006
#define SERVER_ERROR_AUTH				0X80000007
#define SERVER_ERROR_PATH				0X80000008
#define SERVER_ERROR_SYSTEMLOG			0X80000009
#define SERVER_ERROR_LOG				0X8000000A
#define SERVER_ERROR_GAMEGUARD			0X8000000B
#define SERVER_ERROR_LOAD_QUEST			0X8000000C
#define SERVER_ERROR_GIP				0X8000000D
#define SERVER_ERROR_CLAN				0X8000000E
#define SERVER_ERROR_CONTROL			0X8000000F
#define SERVER_ERROR_MESSENGER			0X80000010
#define SERVER_ERROR_DSERVER			0x80000011 // yjha
#define SERVER_ERROR_RELAY_SERVER		0x80000012
#define SERVER_ERROR_SHOP_CONTEXT		0x80000013
#define SERVER_ERROR_MODULE_DATABASE	0x80000014
#define SERVER_ERROR_DIFFERENT_BUILD_VERSION	0x80000015

#define SERVER_UPDATE_PLAYER			0x00000001
#define SERVER_UPDATE_ROOM				0x00000002

#define MAIN_THREAD_IDX					-100	
//#define MODULE_ZP_THREAD_IDX			-200		//zepetto에서 만든 모듈 Auth, Clan, Message, Relay, Control
#define MODULE_CONTROL_THREAD_IDX		-201		//Control
#define MODULE_TRANS_THREAD_IDX			-202		//Trans
#define MODULE_CLAN_THREAD_IDX			-203		//Clan
#define MODULE_MESSENGER_THREAD_IDX		-204		//Message
#define MODULE_NC_THREAD_IDX			-300		//NC에서 만든 모듈 GIP, LOG_D
#define MODULE_DATABASE_THREAD_IDX		-400		//독립 DataBase 쓰레드 Update부분을 저장합니다.
#define MODULE_USER_THREAD_IDX			-500		// DB 
#define MODULE_GAMEGUARD_THREAD_IDX		-600		// GameGuard 

#define	ROOM_CHECK_COUNT				1000
#define	ROOM_CHECK_MAINCLIENT_TIMEOUT	2

#define	PATH_STRING_COUNT				512 
#define SERVER_STRING_COUNT				512		//줄이면 않됨...줄이기전에 모든 변수를 확인해야함.
#define SQL_STRING_COUNT				4096	//DataBase

#define DESTROY_ROOM					1
#define INVENTORY_TABLE_COUNT			10
#define IP_NAME_COUNT					32

#define DEF_SIZE_HACK_NOTICE			256			// Hack 으로 자를 경우 상세 이유

#define GET_USERTITLE( title, Id )		(0x1&(title>>(UINT64)Id))
#define SET_USERTITLE( title, Id )		(title | ((UINT64)0x1<<(UINT64)Id))

//////////////////////////////////////////////////////////////////////////
// Data Base
//
//////////////////////////////////////////////////////////////////////////

// 진급 Point
extern const INT32 g_ai32RankUpBonus[DEF_RANK_COUNT];
#define RANK_UP_FLAG_NONE			0x00	// 없음
#define RANK_UP_FLAG_RANK_05		0x01	// 하사
#define RANK_UP_FLAG_RANK_45		0x02	// 대령
#define RANK_UP_FLAG_RANK_46		0x04	// 준장
#define RANK_UP_FLAG_RANK_47		0x08	// 소장
#define RANK_UP_FLAG_RANK_48		0x10	// 중장
#define RANK_UP_FLAG_RANK_49		0x20	// 대장
#define RANK_UP_FLAG_RANK_50		0x40	// 원수
#define RANK_UP_FLAG_RANK_RESERVED	0x80	// 예약



// 
#define GENDER_MALE					1
#define GENDER_FEMALE				2

// QUEST
#define	MAX_USED_WEAPON_LIST		3		// 사용한 무기 리스트 누적 수

// 상대방 정보(무기,캐릭터,아이템) 보기
#define	VIEW_USER_ITEM_INFO_TYPE_LOBBY		1		// 로비
#define	VIEW_USER_ITEM_INFO_TYPE_ROOM		2		// 룸 - 상대방 정보보기 아이템 사용
#define	VIEW_USER_ITEM_INFO_TYPE_BATTLE		3		// 배틀 - 추후구현??

enum USED_ITEM_TYPE
{
	USED_ITEM_TYPE_NONE,
	USED_ITEM_TYPE_ONE,
	USED_ITEM_TYPE_ENDURANCE,
};

//Game Task = Stage
enum GAME_TASK
{
	GAME_TASK_NONE = 0,
	GAME_TASK_LOGIN,			
	GAME_TASK_CHANNEL_LIST,	
	GAME_TASK_LOBBY,
	GAME_TASK_READY_ROOM,
	GAME_TASK_BATTLE,
	GAME_TASK_COUNT,
};

//Sub Task 
enum GAME_TASK_SUB
{
	GAME_TASK_SUB_NONE = 0,
	GAME_TASK_SUB_INVENTORY,	//인벤토리
	GAME_TASK_SUB_SHOP,			//숍
	GAME_TASK_SUB_INFO,			//개인정보
	GAME_TASK_SUB_CLAN,			//개인정보
	GAME_TASK_SUB_RS,			//뽑기상점
};


//=====================================================================================================================================//

// Event Bouns Setting
enum EVENT_BONUS_TYPE
{
	EVENT_BONUS_NORMAL					= 1,	// Normal
	
	EVENT_BONUS_PC_CAFE					= 10,	// PC List

	EVENT_BONUS_CHANNEL					= 30,	// Channel List

	EVENT_BONUS_MODE					= 70,	// + STAGE_TYPE_NA (...STAGE_TYPE_MAX)

	EVENT_BONUS_STAGE_MASK				= 90,	// + STAGE_MASK_TYPE (...STAGE_MASK_TYPE_MAX)

	EVENT_BONUS_STAGE					= 100,	// + STAGE_ORD_NA (...STAGE_ORD_MAX)

	EVENT_BONUS_NATION					= 500,	// 국가 코드

	EVENT_BONUS_SERVER					= 1000,	// 서버 개수
};

enum EVENT_ROOM_BONUS_TYPE
{
	EVENT_ROOM_BONUS_NORMAL				= 0,
	EVENT_ROOM_BONUS_SERVER,

	EVENT_ROOM_BONUS_CAFE,																// + PC_MAX_COUNT
	EVENT_ROOM_BONUS_CAFE_END			= EVENT_ROOM_BONUS_CAFE + PC_MAX_COUNT,

	EVENT_ROOM_BONUS_CHANNEL			= EVENT_ROOM_BONUS_CAFE_END,					// + CHANNEL_TYPE_MAX
	EVENT_ROOM_BONUS_CHANNEL_END		= EVENT_ROOM_BONUS_CHANNEL + CHANNEL_TYPE_MAX,

	EVENT_ROOM_BONUS_MODE				= EVENT_ROOM_BONUS_CHANNEL_END,					// + STAGE_TYPE_NA (...STAGE_TYPE_MAX)
	EVENT_ROOM_BONUS_MODE_END			= EVENT_ROOM_BONUS_MODE + STAGE_TYPE_MAX,

	EVENT_ROOM_BONUS_STAGE_MASK			= EVENT_ROOM_BONUS_MODE_END,					// + STAGE_MASK_TYPE (...STAGE_MASK_TYPE_MAX)
	EVENT_ROOM_BONUS_STAGE_MASK_END		= EVENT_ROOM_BONUS_STAGE_MASK + STAGE_MASK_TYPE_MAX,

	EVENT_ROOM_BONUS_STAGE				= EVENT_ROOM_BONUS_STAGE_MASK_END,				// + STAGE_ORD_NA (...STAGE_ORD_MAX)
	EVENT_ROOM_BONUS_STAGE_END			= EVENT_ROOM_BONUS_STAGE + STAGE_ORD_MAX,

	EVENT_ROOM_BONUS_NATION				= EVENT_ROOM_BONUS_STAGE_END,					// + STAGE_ORD_NA (...STAGE_ORD_MAX)
	EVENT_ROOM_BONUS_NATION_END			= EVENT_ROOM_BONUS_NATION + GEOIP2_MAX_COUNTRY_COUNT,

	EVENT_ROOM_BONUS_MAX,
};

struct LOG_KILL_DEATH_INFO
{
	UINT32		ui32WeaponNum;
	UINT32		m_ui32DeathType; 

	REAL32		r32Kill_X; 
	REAL32		r32Kill_Y; 
	REAL32		r32Kill_Z; 
	REAL32		r32Death_X; 
	REAL32		r32Death_Y; 
	REAL32		r32Death_Z; 
}; 

struct LOG_GET_TITLE
{
	INT32			_i32Miniature;
	INT32			_i32Insignia;
	INT32			_i32Order;
	INT32			_i32Master;
	void	Reset()
	{
		_i32Miniature = _i32Insignia = _i32Order = _i32Master = 0;
	}
}; 

struct LOG_MISSION
{
	UINT8			_ui8CardType;
	UINT8			_ui8CardSet;
	UINT32			_ui32Exp;
	UINT32			_ui32Point;
	UINT32			_ui32LastExp;
	UINT32			_ui32LastPoint;
	INT32			_i32Miniature;
	INT32			_i32Insignia;
	INT32			_i32Order;
	INT32			_i32Master;
	INT32			_i32LastMiniature;
	INT32			_i32LastInsignia;
	INT32			_i32LastOrder;
	INT32			_i32LastMaster;
	BOOL			_bFinish;
	UINT32			_aui32GetItem[ MAX_QUEST_ITEM_CARD ];
	UINT32			_aui32LastGetItem[ MAX_QUEST_ITEM_CARD ];
}; 

//UserSession에서만 사용
struct FRIEND_INFO_STRUCT
{
	UINT8			_ui8NickSize;
	char			_strNickName[NET_NICK_NAME_SIZE]; 
	FRIEND_INFO		_BasicInfo; 
}; 

struct BLOCK_INFO_STRUCT
{
	UINT8			_ui8NickSize;
	UINT8			_ui8CommentSize;
	char			_strNickName[NET_NICK_NAME_SIZE]; 
	char			_strComment[BLOCK_COMMENT_MAX];	// 차단 사유
	BLOCK_INFO		_BasicInfo; 
}; 

struct CLANMEMBER_INFO_STRUCT
{
	UINT8			_ui8NickSize;
	char			_strNickName[NET_NICK_NAME_SIZE]; 
	CLANMEMBER_INFO	_BasicInfo; 
}; 

struct TYPE_RING_IN
{
	TUID			_i64UID;
	INT32			_i32SessionIdx;
	
	UINT32			_ui32ChangeFlag;
	UINT8			_ui8DefaultKey;
	NET_ENV_SET		_NetEnvSet;
	GAME_KEY_STRUCT	_aKeyMap[GAME_KEY_MAP_COUNT];
	char			_astrMacro[MACRO_MAX_COUNT][MACRO_MAX_LENGHT];

	UINT8			_ui8Type;
};

struct TYPE_RING_OUT
{
	TUID			_i64UID;
	INT32			_i32SessionIdx;

	UINT8			_ui8DefaultKey;
	NET_ENV_SET		_NetEnvSet;
	GAME_KEY_STRUCT	_aKeyMap[GAME_KEY_MAP_COUNT];
	char			_astrMacro[MACRO_MAX_COUNT][MACRO_MAX_LENGHT];

	UINT8			_ui8Type;

	UINT8			_ui8Event;
};

struct TYPE_IPLIST
{
	UINT32		_ui32Ip;
	UINT8		_ui8Rank;
};

struct ROOM_USER_INFO
{
	UINT8		_ui8Ping;							// 핑정보 
	BOOL		_bAlreadyMain;						// 방장을 잡았는지 유무
	BOOL		_bChangeMain;						// 방장 교체 버튼을 눌렀는지
	
	void	Reset()
	{
		_ui8Ping		= 0;
		_bAlreadyMain	= FALSE;
		_bChangeMain	= FALSE;
	};
};

struct USED_WEAPON
{
	INT8		_i8Idx;									// 인덱스
	INT8		_ai8WeaponClass[MAX_USED_WEAPON_LIST];	// 사용한 무기 누적 (인덱스를 기준으로 순환)

	void	Reset()
	{
		for( _i8Idx = 0; _i8Idx < MAX_USED_WEAPON_LIST; _i8Idx++ )
			_ai8WeaponClass[_i8Idx] = 0;
		_i8Idx = 0;
	}

	void	IncreaseIdx()
	{
		_i8Idx += 1;
		if(_i8Idx >= MAX_USED_WEAPON_LIST)
			_i8Idx = 0;
	}
};

#define RING_TYPE_OPTION_SET	0
#define RING_TYPE_OPTION_GET	1

//글로벌 변수 
extern char				g_strCCurrentPath[PATH_STRING_COUNT];
extern SERVER_STATE		g_eServerState;

extern INT8				g_ai8SlotSequenceforOdd[SLOT_MAX_COUNT];
extern INT8				g_ai8SlotSequenceforEven[SLOT_MAX_COUNT];

extern UINT32			g_ui32StartTime;
extern UINT8			g_ui8Active;



//글로벌 함수 
void					g_SetCurrentPath( char* pstrPath );
INT32					g_InitServer(void); 
INT32					g_StartServer(void);
void					g_UpdateServer(void); 
void					g_DestroyServer();

// 미션카드 체크 보조 함수
UINT32					g_GetWeaponClass_ForMission(UINT32 ui32WeaponClass);			// 원래 웨폰 클래스와 비교하는 클래스가 다른 경우가 있어 함수에서 확인
STAGE_ORDINAL_ID		g_GetStageID_ForMission(STAGE_ORDINAL_ID ui32StageID);					// 원래 웨폰 클래스와 비교하는 클래스가 다른 경우가 있어 함수에서 확인

extern UINT8			g_ui8ConBuildVerMonth;		// 컨트롤 서버 빌드 버전 Month
extern UINT8			g_ui8ConBuildVerDay;		// 컨트롤 서버 빌드 버전 Day

////////////////////////////////////////////////
//// GameGuard
// AhnHack
#ifdef USE_AHNHS
#include "AntiCpXSvr.h"
extern AHNHS_SERVER_HANDLE	g_hAhnhsServer;
#endif
// Xtrap 
#ifdef USE_XTRAP
#include "XTrapModule.h"
extern CXTrapModule*		g_pXTrapModule;
#endif
#ifdef USE_GPK
#include "GPK\\Include\\GPKitSvr.h"
//using namespace SGPK
extern SGPK::IGPKSvrDynCode*		g_pSvrDynCode;
INT32	LoadGPK();
#endif

////////////////////////////////////////////////

// 어뷰징 체크시 계산하는 배수의 인덱스
enum ABUSING_CHECK_COUNT
{
	ABUSING_CHECK_NORMAL_1 = 0,	// 일반제재 1단계
	ABUSING_CHECK_NORMAL_2,		// 일반제재 2단계
	ABUSING_CHECK_NORMAL_3,		// 일반제재 3단계	
	ABUSING_CHECK_NORMAL_4,		// 일반제재 4단계	
	ABUSING_CHECK_STRONG_1,		// 강화제재 1단계	 
	ABUSING_CHECK_STRONG_2,		// 강화제재 2단계	 
	ABUSING_CHECK_STRONG_3,		// 강화제재 3단계	 
	ABUSING_CHECK_STRONG_4,		// 강화제재 4단계	

	ABUSING_CHECK_COUNT_MAX,	// 총 배수의 갯수
};


extern UINT32 g_ui32AbusingCalValue[ABUSING_CHECK_COUNT_MAX];

#define ABUSING_CHECK_MIN_PLAY_TIME			21		// 어뷰징 체크시 최소 플레이 시간 

#define CHECK_NORMAL_ABUSING_EXP			250		// 일반 어뷰저 체크 기준 EXP ( 초당 )
#define CHECK_STRONG_ABUSING_EXP			610		// 강화 어뷰저 체크 기준 EXP ( 초당 )
#define NORMAL_ABUSING_MIN_POPUP_TIME		30		// 일반 어뷰저 최소 팝업 시간( 초 )
#define NORMAL_ABUSING_MAX_POPUP_TIME		600		// 일반 어뷰저 최대 팝업 시간( 초 )
#define STRONG_ABUSING_MAX_POPUP_TIME		900		// 강화 어뷰저 최대 팝업 시간( 초 )


#define ABUSING_CHECK_COMPARE_TIME_1		32		// 어뷰징 체크 기준 배틀 시간 1
#define ABUSING_CHECK_COMPARE_TIME_2		182		// 어뷰징 체크 기준 배틀 시간 2
#define ABUSING_CHECK_COMPARE_TIME_3		302		// 어뷰징 체크 기준 배틀 시간 3


// 리스폰 단축권 사용 여부
#define USE_SHORT_RESPAWN_20				0x01
#define USE_SHORT_RESPAWN_30				0x02
#define USE_SHORT_RESPAWN_50				0x04
#define USE_SHORT_RESPAWN_100				0x08


// 리스폰 시간
#define RESPAWN_TIME_NO				6	// 기본 리스폰 시간	+ 리스폰 게이지 뜰때까지 시간 - 여유시간 = 5 + 3 - 2
#define RESPAWN_TIME_SHORT_20		5	// 20% 단축권 리스폰 시간	+ 리스폰 게이지 뜰때까지 시간 - 여유시간 = 4 + 3 - 2
#define RESPAWN_TIME_SHORT_30		4	// 30% 단축권 리스폰 시간	+ 리스폰 게이지 뜰때까지 시간 - 여유시간 = 3 + 3 - 2
#define RESPAWN_TIME_SHORT_50		3	// 50% 단축권 리스폰 시간	+ 리스폰 게이지 뜰때까지 시간 - 여유시간 = 2 + 3 - 2
#define RESPAWN_TIME_SHORT_100		1	// 100% 단축권 리스폰 시간	+ 리스폰 게이지 뜰때까지 시간 - 여유시간 = 0 + 3 - 2

#define VOTE_POSSIBLE_TIME			20		// 투표 제한 시간
#define VOTE_RE_SUGGEST_TIME		60		// 투표종료(부결) 후 동일한 신고인, 피신고인으로 재투표가 불가능한 시간
#define	MAX_SIZE_VOTE_RECORD		(VOTE_RE_SUGGEST_TIME / VOTE_POSSIBLE_TIME)	// 투표 결과 최대 저장 수
#define VOTE_FINISH_TIME			30		// 게임 종료 몇 초 전부터 투표를 불가능하도록 할 것인가

enum	GET_RANKING_TYPE
{
	GET_RANKING_KILLDEATH,
	GET_RANKING_RAWEXP,
};

#define ANNOUNCE_MESSAGE_BUFFER_SIZE	1040
#define ANNOUNCE_MESSAGE_BUFFER_COUNT	20
#define SEND_COUNT_ANNOUNCE_MESSAGE		16 

#define ANNOUNCE_MESSAGE_NULL_SET		0xFFFF

enum ANNOUNCE_MESSAGE_TARGET
{
	ANNOUNCE_MESSAGE_NONE = 0,
	ANNOUNCE_MESSAGE_ALL,
	ANNOUNCE_MESSAGE_CHANNEL,
	ANNOUNCE_MESSAGE_ROOM,
	ANNOUNCE_MESSAGE_USER,

	ANNOUNCE_MESSAGE_TYPE_COUNT
};

enum GET_INFO_TO_SERVER
{
	GET_INFO_TO_SERVER_BASIC,
	GET_INFO_TO_SERVER_CLAN,
//	GET_INFO_TO_SERVER_ITEM,
//	GET_INFO_TO_SERVER_MESSENGER,

	GET_INFO_TO_SERVER_MAX,
};

struct ANNOUNCE_MESSAGE
{
	UINT32	ui32Protocol;
	UINT8	ui8Target;
	UINT32	ui32ChannelNum;
	UINT32	ui32RoomIdx;
	UINT32	ui32SessionIdx;
	UINT32	ui32BufferSize;
	char	strBuffer[ANNOUNCE_MESSAGE_BUFFER_SIZE];
};

struct SERVER_NET_BUFFER
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_WritePos;
	INT32				_ReadPos;
};

struct DEFAULT_ITEM_SET
{
	UINT8					m_ui8Count;
	T_ItemID				m_TItemID[ DEFAULT_ITEM_COUNT ];
};

// 얼라이브 패킷 전달
#define SEND_ALIVEPACKET_PERIOD					10		// 전송 주기(Frame)
#define SEND_ALIVEPACKET_PACK					4		// 한번에 몇명의 유저를 보낼지

#endif