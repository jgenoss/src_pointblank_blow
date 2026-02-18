#ifndef __SERVERDEF_H
#define	__SERVERDEF_H


/*------------------------------------------------------------------------------------
Auth Server 
----------------------- Thread 
Main
Task
Module LogD
Module LogFile 
Module Control
AuthGateWay
Receiver * 10
------------------------------------------------------------------------------------*/
//User PDH
#include "../../CommonServerSource/SIA_Protocol.h"
#include "Shop.h"
#include "../../CommonServerSource/BillingDefine.h"
#include "../../CommonServerSource/DestroyReason.h"
#include "PCCafeInfo.h"
#include "Design/DesignBase.h"
#include "Design/DesignV1.h"
#include "Design/DesignV2.h"

#if defined(BUILDMODE_SEV)
	//-----------------------------------------------------
	//배포판 빌드
	#define	BUILD_VER				"DIST"
	//-----------------------------------------------------
#else
	//-----------------------------------------------------
	//디버깅용
	#define	BUILD_VER				"DEBUG"
	//-----------------------------------------------------
#endif

#define	TASK_WEB_REQ_MAX_PROC	10
#define	TASK_USER_KICK_MAX		10
#define	MAX_TAIWAN_LOGIN_RETURN_VALUE	128 // 대만 로그인 응답 패킷 최대 길이.

#define NO_STR_TBL										//스트링 테이블 사용하지 않는다. 

#define	ACCEPTOR_RECV_TIME					2			//소켓 접속 확인 시간	ms
#define	SERVER_STRING_COUNT					1024		//
#define GOODS_UPDATE_TIME_STATE				60 * 60 	// 1시간(3600초)
#define GOODS_UPDATE_RETRY_STATE			30			// 30초
#define BILLING_TIMEOUT						5			// 4초
#define MAXPACKETBUFFERSIZE					66560		// 65kb	- IBGameInterface에서 사용

#define INVALID_THREAD_INDEX						0
#define MAIN_THREAD_IDX								-10
#define MODULE_LOGD_THREAD_IDX						-20
#define MODULE_TASK_THREAD_IDX						-30
#define MODULE_CONTROL_THREAD_IDX					-40
#define MODULE_SIA_THREAD_IDX						-50
#define MODULE_DATABASE_THREAD_IDX					-60
#define MODULE_THAI_PCCAFE_DB_THREAD_IDX			-70		
#define MODULE_NICK_DATABASE_THREAD_IDX				-80		
#define MODULE_UID_DATA_THREAD_IDX					-90	
#define MODULE_THAI_IDPLUS_DB_THREAD_IDX			-100	
#define MODULE_GENERAL_RANKUP_THREAD_IDX			-200	
#define MODULE_STATS_DATABASE_THREAD_IDX			-300	
#define MODULE_DB_SHOP_MAIN_THREAD_IDX				-400	
#define MODULE_WEBGIFT_THREAD_IDX					-500	
#define MODULE_DB_USER_INFO_THREAD_IDX				-700	


#define THREAD_RECV_START_IDX						0			//0		~	99
#define THREAD_USER_DB_SAVE_START_IDX				100			//100	~	199
#define THREAD_USER_DB_LOAD_START_IDX				200			//100	~	199
#define THREAD_SHOP_DB_SHOP_IDX						300			//200	~	299

#define THREAD_S_DATABASE_IDX						100			//100	~	199


#define BUFFER_MAX_INT_COUNT				2000000000

#define AUTO_SAVE_DELAY_TIME					20

// 잠시 사용 합니다. 
#define EVENT_RETURN_OK				 0x00000001
#define EVENT_RETURN_ERROR_DB		 0x80000001
#define EVENT_RETURN_ERROR_DUPLICATE 0x80000002
#define EVENT_RETURN_ERROR_BUFFER	 0x80000003

//////////////////////////////////////////////////////////////////////////////////////////
// DB Buffer

#define		WEBGIFT_MAX						50
#define		USER_KICK_MAX					50
#define		WEB_GIFT_UPDATE_TIME			10
#ifdef I3_DEBUG
#define		DATABASE_BUFFER_COUNT			64
#else 
#define		DATABASE_BUFFER_COUNT			1024
#endif
#ifdef I3_DEBUG
#define		DATABASE_BUFFER_WEB_GIFT		64
#else 
#define		DATABASE_BUFFER_WEB_GIFT		2048
#endif
#define		TASK_WEB_REQ_MAX_PROC			10
#define		MAX_SALE_PERCENT				50	// 최대 할인률
#define		MAX_MAIN_STREAM_BUFFER_COUNT	500


#define		SHOP_VERSION_UPDATE_TIME		1	// 상점 버전 업데이트(DB 로드) 주기. ( 분 ) 

#define		AAS_APPOINTED_SHUTDOWN_AGE		16	// 셧다운(지정시간 금지) 대상자 제한 만나이(해당 나이 미만인 유저가 대상이 됩니다)

#define EVENT_FAIL_CREATE_TASK								0X90000001
#define EVENT_FAIL_CREATE_DATABASE							0X90000002
#define EVENT_FAIL_CREATE_INI								0X90000003
#define EVENT_FAIL_CREATE_LINK								0X90000004
#define EVENT_FAIL_CREATE_LOG								0X90000006
#define EVENT_FAIL_CREATE_SERVICE_LOG						0X90000007
#define EVENT_FAIL_CREATE_INI_VALUE							0X90000008
#define EVENT_FAIL_CREATE_NETWORK							0X90000009
#define EVENT_FAIL_CREATE_SHOP								0X9000000A
#define EVENT_FAIL_CREATE_CONTROL							0X9000000B
#define EVENT_FAIL_CREATE_SYSTEMLOG							0X9000000C
#define EVENT_FAIL_RUSSIA_INIT								0X9000000D
#define EVENT_FAIL_CREATE_FIELDSHOP							0X9000000E
#define EVENT_FAIL_CREATE_LOGIN								0X9000000F
#define EVENT_FAIL_CREATE_DATABASE_SHOP						0X90000010

#define EVENT_FAIL_RINGBUFFER_MAX							0X91000001
#define EVENT_FAIL_RINGBUFFER_OF							0X91000002
#define EVENT_FAIL_DATABASE_SQL								0X91000003
#define EVENT_FAIL_DATABASE_AUTH							0X91000004
#define EVENT_FAIL_DATABASE_CHECK							0X91000005
#define EVENT_FAIL_DATABASE_REPAIR							0X91000006
#define EVENT_FAIL_DATABASE_REPAIR_AUTHERROR				0X91000007

#define EVENT_FAIL_DIFFERENT_BUILD_VERSION					0X91000008

// #define OLD_VER

#define LINK_RESULT_TYPE_E001_NOT_ALLOW_METHOD				0X92000001	
#define LINK_RESULT_TYPE_E010_NOT_MATCH_AUTH_KEY			0X92000002	
#define LINK_RESULT_TYPE_E020_UNKNOWN_SYSTEM_ERROR			0X92000003
#define LINK_RESULT_TYPE_E050_SYSTEM_ERROR					0X92000004	
#define LINK_RESULT_TYPE_E051_DATABASE_ERROR				0X92000005	
#define LINK_RESULT_TYPE_E100_NOT_FIND_PARAMETER0			0X92000006	
#define LINK_RESULT_TYPE_E200_NOT_ALLOW_FORMAT				0X92000007	
#define LINK_RESULT_TYPE_E201_NOT_MATCH_PRICE				0X92000008	
#define LINK_RESULT_TYPE_E202_EVENT_CODE_NOT_EXIST			0X92000009	
#define LINK_RESULT_TYPE_E203_EVENT_NAME_NOT_EXIST			0X92000010	
#define LINK_RESULT_TYPE_E204_TOTAL_PRICE_TOO_HIGH			0X92000011	
#define LINK_RESULT_TYPE_E300_INVALID_SHOP_CODE				0X92000012	
#define LINK_RESULT_TYPE_E301_SHOP_DISABLED					0X92000013	
#define LINK_RESULT_TYPE_E302_NOT_ALLOW_SHOP_IP				0X92000014	
#define LINK_RESULT_TYPE_E303_NOT_USE_SSL					0X92000015	
#define LINK_RESULT_TYPE_E304_LOWER_THAN_MIN_PRICE			0X92000016	
#define LINK_RESULT_TYPE_E305_NOT_USE_PAYMENT_GATEWAY		0X92000017	 
#define LINK_RESULT_TYPE_E400_NOT_FIND_BILLING_ACCOUNT		0X92000018	
#define LINK_RESULT_TYPE_E401_BLOCK_ACCOUNT					0X92000019	
#define LINK_RESULT_TYPE_E402_ROW_CASH_BALANCE				0X92000020
#define LINK_RESULT_TYPE_E403_PAYMENT_LIMITOVER				0X92000021	
#define LINK_RESULT_TYPE_E404_PG_LIMITOVER					0X92000022	
#define LINK_RESULT_TYPE_E405_USER_IP_INFO_REQUIRE			0X92000023	
#define LINK_RESULT_TYPE_E406_BLOCK_USER_IP					0X92000024	
#define LINK_RESULT_TYPE_E407_NOT_ALLOW_USER				0X92000025	
#define LINK_RESULT_TYPE_UNKNOWN_ERROR						0X92000026							// 알려지지 않은 에러
#define LINK_RESULT_TYPE_PACKET_BREAK						0X92000027							// 패킷 깨짐

//Thread Priority 
#define THREAD_PRIORITY_TASK				THREAD_PRIORITY_ABOVE_NORMAL	//
#define THREAD_PRIORITY_NETWORK				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LOG					THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_SERVICE_LOG			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_DATABASE			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LINK				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_WEBSHOP				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_CONTROL				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_WEBGIFT				THREAD_PRIORITY_NORMAL			//

 
//LogIn Method
#define TYPE_LOGIN_WEB					1
#define TYPE_LOGIN_ID					2
#define TYPE_LOGIN_LOGOUT				3
#define TYPE_LOGIN_CASH_CHECK			4
#define PCCAFE_MAX_COUNT				20000
#define THAI_ESPORT_ID_MAX_COUNT		30000
#define THAI_IDPLUS_MAX_COUNT			50000

#define TASK_RING_BUFFER_COUNT			200

//DataBase Thread Buffer
#define DB_BUFFER_COUNT					200
#define DB_NICKNAME_COUNT				200
#define DB_LOADINFO_COUNT				300
#define DB_USER_KICK_COUNT				500
#define DB_SAVEINFO_COUNT				1000
#define DB_AUTOSAVE_COUNT				100
#define DB_NICK_CHANGE_BUFFER_COUNT		200
#define DB_UID_BUFFER_COUNT				500
#define DB_CHARA_COUNT					500
#define DB_USER_INFO_COUNT				200

#define DATABASE_THREAD_COUNT			10
#define SQL_STRING_COUNT				1024

#define INVALID_BUFFER_INDEX			0xFFFFFFFF

#define TIME_ATTENDANCE_CHECK			(I3DATETYPE_YEAR | I3DATETYPE_MONTH | I3DATETYPE_DAY)

enum CAPSULE_LOAD_TYPE
{
	CAPSULE_LOAD_INI = 0,
	CAPSULE_LOAD_SHOP,
	
	CAPSULE_LOAD_MAX,
};

enum DB_REPAIR_TYPE
{
	DB_REPAIR_TYPE_BUY = 1,	// Point는 UPDATE, Cash는 STAND BY 로우 INSERT
	DB_REPAIR_TYPE_LINK,	// STAND BY 로우 DELETE, 기존 로우 UPDATE
};

enum DB_COUPON_TYPE
{
	DB_COUPON_TYPE_USE = 1,
	DB_COUPON_TYPE_LINK,
};

//DataBase Thread
enum TYPE_USERINFO
{
	TYPE_USERINFO_LOAD = 1,
	TYPE_USERINFO_SAVE,
	TYPE_USERINFO_NICK_CHANGE_NEW,
	TYPE_USERINFO_NICK_CHANGE_ITEM,
};

// 전적 저장 방법
enum USER_RECORD_TYPE
{
	USER_RECORD_TYPE_NONE		= 0,
	USER_RECORD_TYPE_SEASON,
	USER_RECORD_TYPE_TOTAL,
};

enum	USER_STATE
{
	USER_STATE_INIT			=	0,
	USER_STATE_LOGIN,
	USER_STATE_INFO_LOAD,
	USER_STATE_PLAYING,
	USER_STATE_MOVING,
	USER_STATE_EXIT,
	USER_STATE_INFO_SAVE,
	USER_STATE_ACCOUNTING,
};

enum USER_INFO_FIND_STATE
{
	USER_INFO_FIND_STATE_UID,
	USER_INFO_FIND_STATE_NICK,
};

enum TASK_INVEN_TYPE
{
	TASK_INVEN_TYPE_BUY_GOODS,
	TASK_INVEN_TYPE_GET_CASH,
	TASK_INVEN_TYPE_USE_GIFTCOUPON,
	TASK_INVEN_TYPE_LOGIN,
	TASK_INVEN_TYPE_PCCAFE,
	TASK_INVEN_TYPE_MAX,
};


//Nick Thread
enum TYPE_NICK_T
{
	TYPE_NICK_T_FIND	 = 1,
	TYPE_NICK_T_CHANGE,
	TYPE_NICK_T_GIFT,
};

//UID Thread 
enum TYPE_UID
{
	TYPE_UID_LOAD		= 1,
	TYPE_UID_INSERT_ITEM,
	TYPE_UID_UPDATE_ITEM,
	TYPE_UID_DELETE_ITEM,
	TYPE_UID_BLOCK_USER,
	TYPE_UID_BIRTHDAY_SET,
	TYPE_UID_DIRECT_PENALTY
};

// 닉네임 관련 SP 호출 결과값.
enum	TYPE_NICK_SP
{
	TYPE_NICK_SP_NONE	=	1,
	TYPE_NICK_SP_USE,
	TYPE_NICK_SP_MINE,
	TYPE_NICK_SP_DATE,
	TYPE_NICK_SP_FAIL,
	TYPE_NICK_SP_NOT_EXIST,	
};

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

// Chara Thread(캐릭터구매,삭제)
#define TYPE_NEW_CHARA_CREATE				1
#define TYPE_CHARA_CREATE					2
#define TYPE_CHARA_DELETE					3
#define TYPE_CHARA_SPECIALITY_CLASS_SETTING	4	// 주특기 병과(클래스) 셋팅

enum NODE_SAVE_TYPE
{
	NODE_SAVE_TYPE_NICK_T_CREATE,
	NODE_SAVE_TYPE_CHAR_CREATE,
	NODE_SAVE_TYPE_EXIT,
	NODE_SAVE_TYPE_AUTO,
};


enum WEBGIFT_INSERT_TYPE		// 웹 Gift 지급 타입
{
	WEBGIFT_INSERT_TYPE_WEBTOOL	=	0,	// 웹툴 Send Gift. - GM
	WEBGIFT_INSERT_TYPE_WEBSHOP	,		// 웹상점. - 퍼블리셔
};

enum WEBGIFT_INSERT_STATE
{
	WEBGIFT_INSERT_STATE_BEFORE = 0,
	WEBGIFT_INSERT_STATE_COMPLETE,
	WEBGIFT_INSERT_STATE_FAIL,
	WEBGIFT_INSERT_STATE_ING,

};

enum KICKLIST_INSERT_STATE
{
	KICKLIST_INSERT_STATE_BEFORE = 0,
	KICKLIST_INSERT_STATE_COMPLETE,
	KICKLIST_INSERT_STATE_FAIL,
	KICKLIST_INSERT_STATE_ING,

};

enum USER_DB_LOAD_STATE
{
	USER_DB_LOAD_STATE_NONE,
	USER_DB_LOAD_STATE_LOAD,
};

//---------------------------------------------------------------------------------------------------------
//글로벌 변수 
extern UINT32				g_ui32ConnectServer;
extern char					g_pCCurrentPath[SERVER_STRING_COUNT];

extern SERVER_STATE			g_eServerState;

//글로벌 함수 

void						g_SetCurrentPath( char* strPath );
INT32						g_InitServer( void );
INT32						g_StartServer( void );
BOOL						g_UpdateServer( void );
void						g_DestroyServer(INT32 i32DestroyReason);

extern CShop*				g_pShop;

extern INT32				g_i32MainLogIdx;
extern i3LogFileW*			g_pLog;

#pragma pack( push, 1 )

// 계정 DB 상태
struct USER_NODE
{
	T_UID						_i64UID;					// User Unique ID

	UINT32						_idx;
	UINT8						m_ui8PBVer;					// PB_VER
	CDesignBase*				m_pDesign;					
	CDesignV1*					m_pDesignV1;
	CDesignV2*					m_pDesignV2;
	INT32						_i32Serveridx;				// 접속 서버 번호	
	INT32						_i32Sessionidx;				// 접속된 서버의 섹션 번호 
	UINT32						_ui32AuthKey;				// 로그인용으로 잠시 사용하는 변수 입니다. (다른 사람 로그인을 막기 위한 변수.. 여러나라를 지원하기위해서 필요함)
	UINT32						_ui32State;					// 접속 상태 
	bool						_bLogOut;					// 로그아웃 플래그 // 이 플래그가 켜져있으면 로그아웃하는 중입니다.
	UINT8						m_ui8DBLoadState;			// DB 의 로드 상태

	UINT32						_ui32ConnectState;			// 연결상태( 온/오프라인, 서버, 채널, 방번호 )

	//유저 업데이트에 사용하는 정보 
	UINT32						_LastRecvTimeST;			// Server Time

	UINT32						_ServerLeaveTimeST;			// Server Leave Time

	//----------------------------------------------------------------------
	//인도네시아 중남미 연동을 위해 필요
	char						_strID[NET_ID_SIZE];		// ID

	//----------------------------------------------------------------------
	//Nc, 터키 연동을 위해 필요
	//UINT8						_pLinkId[TURKEY_UID_SIZE];

	char						m_strBillingID[ BILLING_ID_SIZE ];	// Billing 에 사용되는 ID 값

	//----------------------------------------------------------------------
	//중남미 연동을 위해 필요
	UINT16						_nation;

	//----------------------------------------------------------------------
	//러시아 연동을 위해 필요 (Billing DB Idx in Russia)
	UINT64						_ui64UserBillingIdx;

	//----------------------------------------------------------------------
	// 연동을 위해 필요 (  )
	//char						_TurkeyUID[ TURKEY_UID_SIZE ]; 

	//----------------------------------------------------------------------
	//한국, 태국
	UINT8						_ui8RoleFlag;				// 0- 일반유저(PC_NOT_CAFE), 1- 일반PC방(PC_NORMAL_CAFE), 2 - 프리미엄 피씨방(PC_PREMIUM_CAFE)
	UINT8						_ui8ESportIDFlag;			// 0 - ESPOET_NOT_ID, 1 - ESPOET_ID

	//----------------------------------------------------------------------
	//한국, 태국, 러시아, 인도네시아,중국 
	UINT32						_ui32BirthDay;				// 생일
	UINT8						_ui8Gender; 				// 성별

	UINT32						_IpAddress;					// 접속 아이피

	UINT8						m_ui8LocaleCode;

	//----------------------------------------------------------------------
	//유저 접속 정보
	//UINT64						_ui64LogInTime;
	DATE32						m_dt32LoginDate;
	DATE32						m_dt32LastLogInDate;
	DATE32						m_dt32LastLogoutDate;
	UINT32						_ui32TotalConnectTime;
	UINT32						_ui32TotalConnectCount;
	UINT32						_ui32CreateDate; 
	UINT32						_ui32NowConnectTime; 
	UINT32						_ui32Cash;

	INT8						m_i8LogOutSendType;			// 로그아웃 시 퍼블리셔 통보 여부

	//----------------------------------------------------------------------//----------------------------------------------------------------------
	//이하단은  DataBase에서 저장할때 필요한 변수만 정의합니다. 

	//----------------------------------------------------------------------
	//DataBase 저장 유무
	UINT32						_ui32UpdateInfo;
	UINT64						m_ui64CharaUpdateInfo;

	//----------------------------------------------------------------------
	//유저 기본 정보 
	
	USER_INFO_BASIC_BASE		_sBasic;
	USER_INFO_SERVER			m_stUserInfoServer;
	USER_INFO_CLAN				_sClan;

	//유저 인벤	
	//CUserInven					m_Inven;			// 미구현.
	UINT16						_ui16InvenCount;
	INVEN_BUFFER				_Inven[MAX_INVEN_COUNT]; // 할인쿠폰을 사용하면 유저 인벤버퍼에 쿠폰 최대개수 + 인벤최대개수를 잡아줘야 합니다. 인벤버퍼로 할인쿠폰까지 다하고 클라에서 분류
	
	TItemCnt					_ui16DeleteCharaCount;
	T_ItemDBIdx					_DeleteCharaDBIdx[S2MO_MULTI_SLOT_COUNT];

	//----------------------------------------------------------------------
	//Record
	USER_INFO_RECORD			_sRecord;

	USER_INFO_LAST_GAME			m_LastGame;

	//----------------------------------------------------------------------
	//임무카드	
	USER_INFO_QUEST				_sQuest;

	//----------------------------------------------------------------------
	//특수아이템
	USER_INFO_INVITEM_DATA		_sInvenData;

	//----------------------------------------------------------------------
	//장착	
	//USER_INFO_EQUIPMENT_CHARA	_sChara;
	//USER_INFO_EQUIPMENT_WEAPON	_sWeapon;

	CMultiSlot					m_MultiSlot;

	//----------------------------------------------------------------------
	//유저 국가 코드
	UINT8						_ui8LocaleCode;

	// 이벤트 아이템 지급 날짜
	DATE32						_dt32EventItemDate[ EVENTITEM_LOGIN_SLOT_COUNT ];

	//----------------------------------------------------------------------
	//북미 로그인 연동 방식 변경에 따라서 필요합니다. 로그인 할때 받아서 계속 들고있다가 로그아웃때 웹서버로 전송 - 서동권
	INT64						_i64UniqueKey;

	INT32						_i32ReferenceCount;

	USER_INFO_DAILY				_sDaily;
	
	// 출석체크 - 서동권
	ATTENDANCE_USER_SVR			_Attendance[ ATTENDANCE_TYPE_MAX ];
	ATTENDANCE_INFO				m_stAttendanceInfo[ ATTENDANCE_TYPE_MAX ];

	INT32 AddReference()
	{
		return ++_i32ReferenceCount;
	};
	INT32 ReleaseReference()
	{
#ifdef I3_DEBUG
		I3ASSERT( (_i32ReferenceCount-1) >= 0 );
#endif
		return --_i32ReferenceCount;
	};

	UINT8						m_ui8Country;

	MEDAL_ALL_INFO_DB			m_MedalAllInfoDB;	

	QUICKJOIN_INFO				m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];		// 빠른입장 설정정보.
#ifdef ENABLE_GACHA_SHOP
	CGachaPurchase				m_GachaPurchase;
#endif
	// 중독방지 시스템
	bool						m_bAASDailyUsageWay;
	bool						m_bAASAppointedWay;
	UINT16						m_ui16AASAccMin;
	UINT32						m_aui32AASBanTime[DAY_COUNT_IN_WEEK];

	bool						m_bGiftAutoAuth;	// 선물함의 선물이 인벤토리로 자동 지급 됬는지 여부.

	CPCCafeInfo					m_PCCafeInfo;

	//북미-Steam연동	
	char						m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
	char						m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)

	//SIA 서버 멀티연동을 위한 유저별 LinkMethod
	UINT8						m_ui8LinkMethod;

	BOOL					Create()
	{
		_i32Sessionidx = -1;
		if( FALSE == m_MultiSlot.Create() )		return FALSE;

		m_pDesign = NULL;
		m_pDesignV1 = NULL;
		m_pDesignV2 = NULL;

		m_pDesignV1		= new CDesignV1;
		if( NULL == m_pDesignV1 ) return FALSE;
		if( FALSE == m_pDesignV1->OnCreate() ) return FALSE;

		m_pDesignV2		= new CDesignV2;
		if( NULL == m_pDesignV2 ) return FALSE;
		if( FALSE == m_pDesignV2->OnCreate() ) return FALSE;

		return TRUE;
	}

	INVEN_BUFFER* FindInvenItem(UINT32 ui32ItemID)
	{
		for( INT32 i = 0; i < _ui16InvenCount; ++i )
		{
			if( _Inven[i]._ui32ItemID == ui32ItemID )
				return &_Inven[i];
		}

		return NULL;
	}

	INVEN_BUFFER* FindInvenItem(ITEM_TYPE eItemType)
	{
		for( INT32 i = 0; i < _ui16InvenCount; ++i )
		{
			if( static_cast<ITEM_TYPE>(GET_ITEM_TYPE(_Inven[i]._ui32ItemID)) == eItemType )
				return &_Inven[i];
		}

		return NULL;
	}

#ifdef ENABLE_GACHA_SHOP
	INT32 GetGachaPurchaseRecordCnt(void)											{ return m_GachaPurchase.GetRecordCount();	}
	void SetGachaPurchaseRecordCnt(INT32 i32RecordCount)							{ m_GachaPurchase.SetRecordCount(i32RecordCount); }

	INT32 GetGachaPurchaseCnt(INT32 i32ItemKey)									{ return m_GachaPurchase.GetPurchaseCount(i32ItemKey);	}
	void SetGachaPurchaseCnt(INT32 i32ItemKey, INT32 i32ItemAmount)				{ m_GachaPurchase.IncPurchaseRecord(i32ItemKey, i32ItemAmount); }

	INT32 IncGachaPurchaseCnt(INT32 i32ItemKey, INT32 i32ItemAmount = 1)
	{
		_ui32UpdateInfo |= SAVE_DATABASE_GACHA_RECORD;
		return m_GachaPurchase.IncPurchaseRecord(i32ItemKey, i32ItemAmount);
	}

	void ResetGachaPurchaseCnt(INT32 i32ItemKey)
	{
		_ui32UpdateInfo |= SAVE_DATABASE_GACHA_RECORD;
		m_GachaPurchase.ResetPurchaseRecord(i32ItemKey);
	}
	
	void ResetDailyGachaPurchase()
	{
		_ui32UpdateInfo |= SAVE_DATABASE_GACHA_RECORD;
		m_GachaPurchase.ResetPurchaseRecord();
	}
#endif
};

struct TASK_INVEN_BUFFER
{
	UINT8					m_ui8Type;
	T_UID					m_i64UID;
	INT32					m_i32Result;
	UINT32					m_ui32Cash;
	UINT8					m_ui8Action;							// 작업 종류

	union
	{
		struct	// Login
		{
			UINT8			m_ui8PBVer;
			INT32			m_i32ServerIdx;
			INT32			m_i32SessionIdx;
			UINT32			m_ui32AuthKey;

			INT8			m_i8UIDCreateType;
			INT8			m_i8LogOutSendType;

			char			m_strID[ NET_ID_SIZE ];

			UINT64			m_ui64BillingIdx;	// 러시아에서 사용하는 BillingIDX
			char			m_strBillingID[ BILLING_ID_SIZE ];		// Billing 에 사용되는 ID 값

			UINT64			m_ui64UniqueKey;						// 인증키			

			UINT32			m_ui32IP;

			UINT8			m_ui8PCCafe;
			UINT32			m_ui32PCCafeGUID;
			UINT32			m_ui32PCCafeBillingUID;

			UINT32			m_ui32Birthday;	
			UINT32			m_ui32LoginTime;
			UINT8			m_ui8LinkThreadIdx;

			UINT8			m_ui8LocaleCode;

			UINT8			m_aui8ShutDownTime[SHUTDOWN_TIME_SIZE];	// 셧다운 타임 설정값
			UINT8			m_ui8Country;

			char			m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
			char			m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)

			UINT8			m_ui8LinkMethod;
		};
		struct	// Billing 
		{
			UINT32			m_ui32CurrentUserCash;					//캐쉬 잔액
			UINT32			m_ui32CurrentUserCashType;				//캐쉬 잔액 유효성(0: 사용안함, 1: 사용)
			UINT8			m_ui8GoodsCount;						// 구매 상품 갯수.
			INT32			m_i32Rv[MAX_SHOP_GOODS_BUY_COUNT];		// 구매 상품 별 결과. LINK 결제 결과
			char			m_strCouponCode[GIFTCOUPON_LENGTH + 1];	// 쿠폰번호(16자리)

			T_GoodsID		m_aGoodsID[MAX_SHOP_GOODS_BUY_COUNT];	// SIA 갖다 와서 복원용.

			union//구입관련 상세정보 버퍼
			{
				BUFFER_BUY_GACHA	m_stGachaBuffer;
				BUFFER_BUY_CHARA	m_stCharaBuffer;
				BUFFER_BUY_REPAIR	m_stRepairBuffer;
				BUFFER_BUY_GIFT		m_stGiftBuffer;
				BUFFER_BUY_SHOP		m_stShopBuffer;
			};
		};
		struct	// PCCafe
		{
			union
			{
				PACKET_PCCAFE_FIRSTLOGIN_ACK	m_FirstLogin;
				PACKET_PCCAFE_SUBTRACTION_ACK	m_Subtraction;
				PACKET_PCCAFE_LOGIN_ACK			m_Login;
				PACKET_PCCAFE_EMPTY_AMOUNT_ACK	m_EmptyAmount;
				PACKET_PCCAFE_CHECK_AMOUNT_ACK	m_CheckAmount;
			};
		};
	};
};

struct LOGIN_IN_DB
{
	T_UID					m_TUID;
	char					m_strId[ NET_ID_SIZE ];
	INT8					m_i8UIDCreateType;
	UINT32					m_ui32Birthday;
	UINT32					m_ui32BlockDate;
	TTCHAR					m_strUserNoticeComment[BLOCK_COMMENT_SIZE];  //팝업창에 출력할 내용
	INT32					m_i32PenaltyAmount; //GM권한 Block시 Block 종류

};

// 데이타가 필요한 특수아이템
struct INVEN_ITEM_DATA
{
	UINT32					_ui32ItemID;									// 아이템 ID		
	UINT8					_pui8Data[INVEN_ITEMDATA_SIZE];					// 아이템 데이타
}; 

// User DB Load IN Buffer
struct LOAD_USER_INFO_IN
{
	USER_NODE*				pNode;
};

// User DB Load OUT Buffer
struct LOAD_USER_INFO_OUT
{
	INT32					i32Rv;
	USER_NODE*				pNode;
};

// User DB Save IN Buffer
struct RING_EXIT_SAVE_IN
{
	INT8					i8Type;
	USER_NODE*				pNode;
};

// User DB Save OUT Buffer
struct RING_EXIT_SAVE_OUT
{
	INT32					m_i32Result;
	USER_NODE*				pNode;
};

struct RING_CREATE_NICK_IN
{
	T_UID					m_TUID;
	TTCHAR					m_strNick[ NET_NICK_NAME_SIZE ];
};

struct RING_CREATE_NICK_OUT
{
	INT32					m_i32Result;
	T_UID					m_TUID;
	TTCHAR					m_strNick[ NET_NICK_NAME_SIZE ];
	UINT32					m_ui32CreateDate;
	UINT32					m_ui32CreatePoint;
};

struct NICK_CHANGE_IN
{
	UINT8					ui8Type;
	T_UID					i64UID;

	T_ItemDBIdx				_TItemWareIdx;
	char					_strNick[ NET_NICK_NAME_SIZE ];
};

struct NICK_CHANGE_OUT
{
	UINT8					ui8Type; 
	INT32					i32Rv;
	T_UID					i64UID;

	T_ItemDBIdx				_TItemWareIdx;
	UINT32					m_ui32Birthday;
	char					_strNick[ NET_NICK_NAME_SIZE ];
};

struct RING_CREATE_CHAR
{
	INT32					m_i32Result;

	T_UID					m_TUID;
	CHARA_RING_TYPE			m_eType;
	UINT8					m_ui8CharSlot; 
	UINT8					m_ui8Class;
	T_GoodsID				m_TGoodsID;
	INVEN_BUFFER_DB			m_Inven;
	UINT8					m_ui8SlotState;
	bool					m_bAddCharaSlot;	// 캐릭터 슬롯 추가여부.
	
	S2MO_CHAR_BASE_INFO		m_CharBaseInfo;
};

struct DB_SHOP_GOODS	// IN or OUT
{
	INT32				m_i32Rv;				// 캐쉬 구매시. Link 결제전 DB부분실패 체크용.
	T_UID				m_TUID;					// UserDBIdx
	UINT8				m_ui8JobAction;			// 최초요청되었던 작업. ENUM_TYPE_SHOP.
	UINT8				m_ui8DBAction;			// DB 작업. ENUM_TYPE_SHOP.
	UINT8				m_ui8ItemInsertType;	// 아이템 지급 방식. ITEM_INSERT_REQUEST_TYPE
	//UINT32				m_ui32BufferIdx;

	UINT8				m_ui8GoodsCount;
	DB_GOODS			m_stDBGoods[MAX_SHOP_GOODS_BUY_COUNT];

	T_ItemDBIdx			m_TItemDBIdx;			// ItemExtend 용 or CharaBuy용.
	//T_GiftDBIdx			m_TGiftDBIdx[MAX_SHOP_GOODS_BUY_COUNT];			// Gift 용.

	UINT32				m_ui32CurrentUserCash;					//캐쉬 잔액
	UINT32				m_ui32CurrentUserCashType;				//캐쉬 잔액 유효성(0: 사용안함, 1: 사용)

	RING_CREATE_CHAR	m_stChara;
};



struct SEND_GIFT
{
	UINT8					m_ui8GiftCount;							// 선물갯수
	S2MO_RECV_GIFT			m_aGift[MAX_SHOP_GOODS_BUY_COUNT];		// 선물구입정보
	ITEM_INSERT_REQUEST_TYPE m_eRequestType;						// 요청종류.

	// 선물 보내기
	GIFT_TYPE				m_eGiftType;							// 선물 종류
	T_UID					m_TSenderUID;							// 선물 보낸 사람
	T_UID					m_TReceiverUID;							// 선물 받는 사람
	TTCHAR					m_strSenderNick[NET_NICK_NAME_SIZE];	// 선물보낸사람
	TTCHAR					m_strReceiverNick[NET_NICK_NAME_SIZE];	// 선물받는사람
	TTCHAR					m_strMessage[MAX_GIFT_MESSAGE_SIZE];	// 선물메시지
	UINT32					m_ui32CurrentUserCash;
	UINT32					m_ui32CurrentUserCashType;
	INVEN_BUFFER_DB			m_InvenItem[MAX_SHOP_GOODS_BUY_COUNT];	//인벤토리 지급시 사용
	// 선물 지급, 삭제
	// IN - UID, 선물갯수, 인증방식 , 선물갯수 * 선물 DB Idx
	// OUT - UID, 선물갯수, 인증방식,  선물갯수 * ( 선물 DB Idx, 결과, 전달상태, 만료날짜 )
};

struct NICK_IN
{
	UINT8					ui8Type; 

	USER_NODE*				pNode;
	T_UID					m_T_UID;							
	T_ItemDBIdx				TItemWareIdx;			
	TTCHAR					strOldNick[NET_NICK_NAME_SIZE];		
	TTCHAR					strNick[NET_NICK_NAME_SIZE];		// 선물구매시는 받는 사람 닉네임

	SEND_GIFT				m_SendGift;
};

struct NICK_OUT
{
	UINT8					ui8Type; 
	INT32					i32Rv;
 
	USER_NODE*				pNode;
	T_ItemDBIdx				TItemWareIdx;					
	TTCHAR					strOldNick[NET_NICK_NAME_SIZE];
	TTCHAR					strNick[NET_NICK_NAME_SIZE];	// 선물구매시 받는 사람 닉네임
	T_UID					m_TUID_Receiver;				// 선물구매시 받는 사람 UID

	SEND_GIFT				m_SendGift;	
};

struct RING_USER_INFO_IN
{
	INT32					m_i32ServerIdx;
	INT32					m_i32SessionIdx;
	T_UID					m_TUID;

	USER_INFO_FIND_STATE	m_eFindState;
	T_UID					m_TFindUID;
	TTCHAR					m_strNickName[ NET_NICK_NAME_SIZE ];
};

struct RING_USER_INFO_OUT
{
	INT32					m_i32ServerIdx;
	INT32					m_i32SessionIdx;
	T_UID					m_TUID;
	T_RESULT				m_TResult;
		
	S2_USER_DETAIL_INFO		m_DetailInfo;
};

struct UID_IN
{
	UINT8					_ui8Type; 
	UINT8					m_ui8PBVer; 
	UINT32					_ui32ServerIdx; 
	UINT32					_ui32SessionIdx; 
	T_UID					_i64UID; 
	UINT32					_i32AuthKey;
	INT32					_birthday;
	UINT8					_ui8Gender;
	UINT8					_pcCafe;
	UINT32					_ui32PCCafeGUID;
	UINT32					_ui32PCCafeBillingUID;
	//UINT8					_eSportID;
	INT32					_ncCoin;
	UINT32					m_ui32LoginTime;
	UINT8					m_ui8LinkThreadIdx;
	//NC,터키에서만 사용 
	//UINT8					_pLinkId[TURKEY_UID_SIZE]; 
	char					m_strBillingID[ BILLING_ID_SIZE ];	// Billing 에 사용되는 ID 값
	//러시아에서 사용. 
	UINT64					_ui64UserBillingIdx;
	char					_strID[NET_ID_SIZE];

	UINT8					m_ui8LocaleCode;
	// 중남미에서 사용.
	UINT16					_nation;

	// 북미
	INT64					_UniqueKey;
	char					_Custom[NORTH_CUSTOM_MSG_SIZE];

	INVEN_ITEM_DATA			InvenItem;
	TTCHAR					_strComment[GM_COMMENT_SIZE+1];

	UINT8					m_ui8Country;

	UINT32					_IpAddress;

	INT8					m_i8UIDCreateType;
	INT8					m_i8LogOutSendType;

	//유저 제재 처리할 때 사용
	INT8					m_i8PenaltyType;
	INT32					m_i32PenaltyAmount;
	INT32					m_i32SessionIdx;		//기 선언된 SessionIdx는 UINT32형이라 새로선언함

	// 셧다운 타임 설정값
	UINT8					m_aui8ShutDownTime[SHUTDOWN_TIME_SIZE];

	//북미-Steam연동	
	char					m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
	char					m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)

	//SIA 서버 멀티연동을 위한 유저별 LinkMethod
	UINT8					m_ui8LinkMethod;
};

struct UID_OUT
{
	UINT8					_ui8Type; 
	INT32					_i32Rv;
	UINT8					m_ui8PBVer; 
	UINT32					_ui32ServerIdx; 
	UINT32					_ui32SessionIdx; 
	T_UID					_i64UID; 
	UINT32					_i32AuthKey;
	INT32					_birthday;
	UINT8					_ui8Gender;
	UINT8					_pcCafe;
	UINT32					_ui32PCCafeGUID;
	UINT32					_ui32PCCafeBillingUID;
	//UINT8					_eSportID;
	INT32					_ui32Cash;
	UINT32					m_ui32LoginTime;
	UINT8					m_ui8LinkThreadIdx;
	//NC,터키에서만 사용 
	//UINT8					_pLinkId[TURKEY_UID_SIZE]; 
	char					m_strBillingID[ BILLING_ID_SIZE ];	// Billing 에 사용되는 ID 값

	//러시아에서 사용. 
	UINT64					_ui64UserBillingIdx;
	char					_strID[NET_ID_SIZE];
	char					_strPass[NET_PW_SIZE];
	// 중남미에서 사용.
	UINT16					_nation;
	//UINT64				_ui64GameMoney;
	UINT32					_IpAddress;	// 접속 아이피
	bool					_bFCMInfoValue;	// 2010년 8월 1일 이전 계정 판단( false : 미인증, true : 인증 )
	// 북미
	INT64					_UniqueKey;
	char					_Custom[NORTH_CUSTOM_MSG_SIZE];

	USER_INFO_INVITEM_DATA	_sInvenData;

	UINT8					m_ui8Country;

	INT8					m_i8LogOutSendType;

	//유저 제재 처리할 때 사용
	INT8					m_i8PenaltyType;
	INT32					m_i32PenaltyAmount;
	INT32					m_i32SessionIdx;		//기 선언된 SessionIdx는 UINT32형이라 새로선언함
	UINT32					m_ui32BlockDate;
	UINT8					m_ui8LocaleCode;

	// 셧다운 타임 설정값
	UINT8					m_aui8ShutDownTime[SHUTDOWN_TIME_SIZE];
	TTCHAR					m_strUserNoticeComment[BLOCK_COMMENT_SIZE];


	//북미-Steam연동	
	char					m_strExtUID[EXT_UID_SIZE];				// 외부연동 UID(암호화)
	char					m_strWebLoginKey[WEB_LOGINKEY_SIZE];	// 웹로그인 연동키(북미스팀만 사용중)

	//SIA 서버 멀티연동을 위한 유저별 LinkMethod
	UINT8					m_ui8LinkMethod;
};

struct TYPE_IPLIST
{
	UINT32					m_ui32IP;
	UINT8					m_ui8Rank;
	UINT32					m_ui32CIDR;
};

struct TYPE_IDLIST
{
	T_UID					_i64uid;
};

struct TYPE_IDPLUS_LIST
{
	T_UID					_i64uid;
	UINT8					_Level;
	UINT32					_ExpireDate;
};

struct STATS_PUSH
{
	INT8					i8Type;
	UINT32					ui32Date;
	UINT64					ui64Value;
};

struct DB_USER_KICK_IN
{
		T_UserKickIdx		i32UserKickIdx;		// KickList 테이블의 Idx
		T_UID				i64UID;				//Kick할 유저 UID
};

struct DB_USER_KICK_OUT : public DB_USER_KICK_IN
{
	INT32				i32Rv;		//성공 여부		
};

struct USER_SAVE_DATA
{
	UINT32					m_ui32UpdateInfo;
	USER_INFO_BASIC_BASE	m_stBasic;
	USER_INFO_SERVER		m_stUserInfoServer;
	USER_INFO_CLAN			m_stClan;
	ITEM_INFO				m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
	UINT8					m_ui8TeamSlot[TEAM_COUNT];

	/*UINT8					m_aui8MultiSlotState[ S2MO_MULTI_SLOT_COUNT ];
	S2MO_CHAR_BASE_INFO		m_CharBaseInfo[ S2MO_MULTI_SLOT_COUNT ];
	S2MO_CHAR_SKILL_INFO	m_CharSkillInfo[  S2MO_MULTI_SLOT_COUNT ];
	ITEM_INFO				m_aCharEquipment[  S2MO_MULTI_SLOT_COUNT  ][ CHAR_EQUIPMENT_COUNT ];
	*/

	USER_INFO_SEASON_RECORD	m_stRecord;
	USER_INFO_QUEST			m_stQuest;
	USER_INFO_DAILY			m_stDaily;
	MEDAL_ALL_INFO_DB		m_MedalAllInfoDB;
	QUICKJOIN_INFO			m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];		// 빠른입장 설정정보.
	UINT16					m_ui16AASAccMin;
	USER_INFO_INVITEM_DATA	m_stInvItem;

	UINT32					m_ui32CheckNum;
};

struct USER_SAVE_CHARA_DATA
{
	UINT64					m_ui64UpdateInfo;
	S2MO_CHAR_INFO			m_CharaInfo[S2MO_MULTI_SLOT_PACKET_COUNT];
	UINT32					m_ui32CheckNum;
};

enum STATS_TYPE
{
	STATS_TYPE_CCU_MIN,
};

struct STATS_INFO
{
	UINT32					m_ui32StMinDate;
	UINT32					m_ui32StHourDate;
	UINT32					m_ui32StMonthDate;
	UINT32					m_ui32StDayDate;
	UINT32					m_ui32StWeekDate;

	UINT32					m_ui32StCCU;
	UINT32					m_ui32StLocaleMCU_1Min[ LOCALE_CODE_MAX ];
	UINT32					m_ui32StMCU_1Min;
	UINT32					m_ui32StMCU_10Min;
};

// 이벤트 아이템 지급 타입
enum EVENT_ITEM_TYPE
{
	EVENT_ITEM_NEW_USER					= 1,	// 신규 유저 지급
	EVENT_ITEM_OLD_USER					= 2,	// 기존 유저 지급

	EVENT_ITEM_MAX	
};

enum ASC_WEB_REQ_TYPE
{
	ASC_WEB_NONE,
	ASC_WEB_GM_KICK,
	ASC_WEB_USER_INFO_LOCATION,
};

struct ASC_WEB_REQ_INFO
{
	ASC_WEB_REQ_TYPE		reqType;
	INT32					i32WebSessionIdx;
	T_UID					i64UID;
};


struct CHARA_SPECIALITY_IN	// SpecialityClassSetting In
{
	UINT8					ui8Type;			// 요청 타입.
	INT32					m_i32SessionIdx;	// Session Idx.
	T_UID					i64UID;				// UID
	UINT8					ui8SlotIdx;			// 캐릭터 슬롯 Idx.
	UINT8					ui8Class;			// 병과(클래스)
	UINT32					m_ui32SkillRelTime;
};

struct CHARA_SPECIALITY_OUT	// SpecialityClassSetting Out
{
	UINT8					ui8Type;			// 요청 타입.
	INT32					m_i32SessionIdx;	// Session Idx.
	INT32					m_i32Rv;			// 결과값
	T_UID					i64UID;				// UID
	UINT8					ui8SlotIdx;			// 캐릭터 슬롯 Idx.
	UINT8					ui8Class;			// 병과(클래스)
	UINT32					m_ui32SkillRelTime;
};

struct GACHA_PURCHASE
{
	GACHA_GROUP				m_eGachaGroup;
	INT32					m_i32ItemIdx;
	INT32					m_i32ItemKey;
	INT32					m_i32ItemAmount;
};

struct MAIN_STREAM_BUFFER
{
	UINT32					_ui32ServerST;			
	UINT8					_ui8Action;				//  값변동없음. ENUM_TYPE_SHOP.
	T_UID					_i64UID;				// 	 

	// TYPE_SHOP_CHARA_BUY	
	RING_CREATE_CHAR	m_stChara;

	union 
	{
		// TYPE_SHOP_REPAIR
		S2MO_REPAIR_INFO		m_stRepairInfo;	// 수리정보

		// TYPE_SHOP_GIFT_BUY
		SEND_GIFT				m_SendGift;		// 선물정보

		// TYPE_GACHA_PURCHASE, TYPE_SHOP_GOODS_BUY, TYPE_SHOP_ITEM_EXTEND, TYPE_SHOP_CHARA_BUY, TYPE_SHOP_GOODS_BUY_LINK
		DB_SHOP_GOODS			m_stDBShopBuy;	// 구매정보
	};

};

union DBSHOP_ARG1		// 몇 케이스에서만 사용하는 데이터일 경우 여기에 넣습니다.
{
	INT32		i32Arg1;
	TLoopCnt	TLoopCount;
};

struct DB_SHOP_OUT
{
	INT32					_i32Rv;					// 성공 여부
	UINT8					_ui8Type;				// DB 리턴 타임 // Buy, Insert, Auth....		
	INT8					_i8RequestType;			// ITEM_INSERT_REQUEST_TYPE	
	UINT32					_ui32TempItem;			// 인증 할때 아이템번호로 사용됨 
	T_UID					_i64UID;				// UserDBIdx

	T_ItemDBIdx				_TItemWareDBIdx;		// 유저 DB 에 저장된 ItemDBIdx
	T_GiftDBIdx				_i32GiftWareDBIdx;		// 유저 GiftDB 에 저장된 GiftDBIdx
	UINT32					_ui32ItemID;			// ItemIdx	
	UINT8					_ui8ItemState;			// 아이템 상태
	UINT32					_ui32AuthSec;			// 인증시간	
	UINT32					_ui32InsertAuthSec;		// 추가한 시간

	// 선물하기. 할인쿠폰
	UINT32					_ui32GoodsID;			// GoodsID

	// 페이레터쿠폰
	char					_strCoupon[GIFTCOUPON_LENGTH + 1];	// Coupon Serial Code
	INT8					_i8SlotIdx;		// 캐릭터 삭제시 사용.-1이면 일반 아이템 삭제.

	DBSHOP_ARG1				_32Arg1;		// 32bit Argument Union
};

struct DB_SHOP_IN
{
	UINT8					_ui8InsertType;		//_ui8Type
	INT8					_i8RequestType;		//	ITEM_INSERT_REQUEST_TYPE
	//UINT8					_ui8AuthType;		//	ITEM_INSERT_AUTH_TYPE
	UINT32					_ui32BufferIdx;		//
	T_UID					_i64UID;			//UID
																		//	Insert	Extend	Delete		Auth	Repair	GiftAuth	
	T_ItemDBIdx				_TWareDBIdx;		// DBIDX					X		0		0			0		0		X			
	UINT32					_ui32ItemID;		//							0		0		X			X		0		X			
	UINT32					_ui32ItemArg;		// Count or Sec				0		0		O			0		0		X			

	UINT32					_ui32GoodsID;		// GoodsID					X		X		X			X		X		X			

	// 페이레터쿠폰
	char					_strCoupon[GIFTCOUPON_LENGTH + 1];	// Coupon Serial Code

	T_GiftDBIdx				_i32GiftDBIdx;		// GiftDBIdx				X			X			X		X		O
	INT8					_i8SlotIdx;		// 캐릭터 사용시작/삭제. -1이면 일반 아이템 삭제.

	DBSHOP_ARG1				_32Arg1;		// 32bit Argument Union
};

struct DB_WEB_GIFT_IN
{
	WEBGIFT_INSERT_TYPE		_eInsertType;
	TWebGiftDxIdx			_i32BufferIdx;		// TGiftDeliver 테이블의 Idx
	T_GiftDBIdx				_i32GiftWareDBIdx;	// TUserGift 테이블의 IDx
	T_UID					_i64UID;			// 받을 유저 UID
	UINT32					_ui32GoodsID;		// 선물할 상품 ID							
	TTCHAR					_strMessage[ MAX_GIFT_MESSAGE_SIZE ];	// 메시지				
};

struct DB_WEB_GIFT_OUT
{
	INT32					_i32Rv;				// 결과 
	TWebGiftDxIdx			_i32BufferIdx;		// TGiftDeliver 테이블의 Idx
	T_GiftDBIdx				_i32GiftWareDBIdx;	// TUserGift 테이블의 IDx
	T_UID					_i64UID;			// 받을 유저 UID
	T_GoodsID				m_TGoodsID;			// 상품 ID
};

struct DB_INVENTORY_IN
{
	T_UID					_i64UID;
};

struct DB_INVENTORY_OUT
{
	INT32					_i32Rv;
	T_UID					_i64UID;
	TItemCnt				_ui16Count;

	INVEN_BUFFER			_Inven[MAX_INVEN_COUNT];

	bool					_bGiftAutoAuth;

	TItemCnt				_ui16DeleteCharaCount;
	T_ItemDBIdx				_DeleteCharaDBIdx[S2MO_MULTI_SLOT_COUNT];
};



struct DB_GIFT_IN	 
{	
	T_UID					m_TUID;				// UID
	UINT8					m_ui8Type;			// 작업 타입. ENUM_TYPE_SHOP
	UINT8					m_ui8GiftCount;		// 선물 갯수

	SEND_GIFT				m_SendGift;			// 선물 관련 데이터 저장용 버퍼

	union 
	{	// TYPE_SHOP_GIFT_BUY, TYPE_SHOP_GIFT_READ, TYPE_SHOP_GIFT_AUTH, TYPE_SHOP_GIFT_DELETE
		AUTH_GIFT			m_aAuthGift[MAX_SHOP_GOODS_BUY_COUNT];
	};

};

struct DB_GIFT_OUT	 
{
	INT32					_i32Rv;				// 결과
	T_UID					m_TUID;				// UID
	UINT8					m_ui8Type;			// 작업 타입. ENUM_TYPE_SHOP
	UINT8					m_ui8GiftCount; 	// 선물 갯수

	//	TYPE_SHOP_GIFT_BUY
	SEND_GIFT				m_SendGift;			// 선물 관련 데이터 저장용 버퍼

	union 
	{	// TYPE_SHOP_GIFT_LIST
		S2MO_GIFT			m_Gift[MAX_SHOP_USER_GIFT_COUNT];

		// TYPE_SHOP_GIFT_READ, TYPE_SHOP_GIFT_DELETE, TYPE_SHOP_GIFT_AUTH
		AUTH_GIFT_INVEN		m_aAuthGift[MAX_SHOP_GOODS_BUY_COUNT];
	};

};

struct DB_REPAIR
{
	T_UID					m_TUID;
	DB_REPAIR_TYPE			m_eType;
	S2MO_REPAIR_INFO		m_stRepairInfo;
};

struct DB_COUPON
{
	T_UID					m_TUID;
	DB_COUPON_TYPE			m_eType;
	char					m_strCoupon[GIFTCOUPON_LENGTH + 1];
	INT32					m_i32Ret;
	INT8					m_i8GoodsCount;
	INT32					m_ai32GoodsID[MAX_GIFTCOUPON_COUNT];
};

struct EVENT_ITEM_NEW
{
	UINT8					m_ui8ItemCount;
	ST_ITEM					m_stItem[ MAX_SHOP_DB_ITEM_COUNT ];
};

struct EVENT_ITEM_LOGIN
{
	UINT8					m_ui8ItemCount;
	DATE32					m_dt32SendDate;
	ST_ITEM					m_stItem_Item[ MAX_SHOP_DB_ITEM_COUNT ];
	S2MO_RECV_GIFT			m_stItem_Goods[ MAX_SHOP_GOODS_BUY_COUNT ];
};


#pragma pack( pop )



#endif
