#ifndef __COMMONDEF_H
#define	__COMMONDEF_H

#define GAME_VER_MONTH	02
#define GAME_VER_DAY	24

#define VER_GAME			1
#define VER_BUILD			13
#define VER_SC_PROTOCOL		7
#define VER_SS_PROTOCOL		1

#ifndef BUILD_VERSION_TOOL		// 빌드버전툴은 여기까지만 읽음

#include <map>
#include <vector>
#include <string>

#define USE_SERVER_BONUS // 서버 보너스
//#define __MODE_DOMINATION__
#define USE_ATTENDANCE
#define USE_SALE_COUPON

//#define USE_QUICK_JOIN	// 빠른입장 설정 사용
#define USE_BASKET_COUNT	8 // 장바구니 최대 아이템 개수

#define USE_INDONESIA_PCBANG // - 서동권
#define MAX_INDONESIA_PCBANG_KEY 51

#define USE_DAILY_RECORD	// 당일 전적 사용, TS 보상 플레이시간
#define USE_RANKUP_ITEM		// 진급축하창 사용

//#define USE_BLOCK_LIST		// 차단리스트 사용

#define MAX_REPORT_SCREENSHOT_COUNT	3

// 최대 쿠폰 개수는 SIA 서버에서도 사용하니깐 변경되면 거기도 바꿔야됨. 최대 개수 이상으로 선물받을경우 첫번째꺼 지우고 밀어넣기 때문
#define MAX_COUPON_INVEN_COUNT	50	//	최대 쿠폰함에 보유할 수 있는 쿠폰 개수 
#define MAX_USER_ITEM_COUPON_INVEN_COUNT (MAX_SHOP_USER_ITEM_COUNT+MAX_COUPON_INVEN_COUNT) // 유저가 최대 가질수 있는 인벤 크기 ( 아이템 + 할인쿠폰 )

#pragma pack( push, 1)
//=================================================================================
//	New Version Format
//=================================================================================
struct SC_VERSION	// 서버-클라
{
	UINT8	_ui8VerGame;		// 게임 대분류
	UINT16	_ui16VerBuild;		// 원빌드 버전
	UINT16	_ui16VerSC;			// 서버-클라 패킷 버전

	SC_VERSION::SC_VERSION()
	{
		_ui8VerGame			= VER_GAME;
		_ui16VerBuild		= VER_BUILD;
		_ui16VerSC			= VER_SC_PROTOCOL;
	}
};
//=================================================================================
struct SS_VERSION	//서버-서버
{
	UINT8	_ui8VerGame;		// 게임 대분류
	UINT16	_ui16VerBuild;		// 원빌드 버전
	UINT16	_ui16VerSS;			// 서버-서버 패킷 버전

	SS_VERSION::SS_VERSION()
	{
		_ui8VerGame			= VER_GAME;
		_ui16VerBuild		= VER_BUILD;
		_ui16VerSS			= VER_SS_PROTOCOL;
	}
};
//=================================================================================
#pragma pack( pop )

#ifndef GAMEPORTAL_BUILD	// ZLog,SIA를 빌드 중이라면 하단의 내용은 전부 무시		2011.10.06

#define MAX_TS_ITEM_STRING		30
#define MAX_TS_GOODS_LIST		7
// 이 부분은 SIA 서버에서도 사용하는데 SIA 에서는 Commondef.h 를 참조하지 않아 따로 정의해놓았습니다.
// SALE COUPON 부분이 수정될 경우 반드시 SIA 에서도 수정해주시기 바랍니다. - 서동권
enum SALE_TYPE
{
	SALE_TYPE_NONE = 0,
	SALE_TYPE_PER,	// 퍼센트로 할인
	SALE_TYPE_CASH	// 지정된 금액 할인
};
enum DAY_TYPE
{
	DAY_TYPE_NONE = 0,
	DAY_TYPE_DATE,	// 정해진 기간 ( 절대적 ) 20111031 ~ 20111105
	DAY_TYPE_ARG	// 인증시 정해진 값만큼 기간 설정 ( 상대적 )	86400
};
enum LIMIT_TYPE
{
	LIMIT_TYPE_NONE	= 0,
	LIMIT_TYPE_UP,	// 얼마 이상 시에만 사용가능
	LIMIT_TYPE_DOWN // 얼마 이하 시에만 사용가능
};

// 선물 지급 
enum ENUM_TYPE_GIFT_DELIVER
{	
	TYPE_GIFT_DELIVER_BEFORE		= 0,	// 선물 지급전
	TYPE_GIFT_DELIVER_COMPLETE		= 1,	// 선물 지급 완료.

	TYPE_GIFT_DELIVER_MAX,
};

// ControlTool Icon
enum ASC_ICON
{
	ASC_ICON_DISCONNECTION,
	ASC_ICON_CONNECTION,
	ASC_ICON_WARNING,
	ASC_ICON_ROOMNOTALLOW,
	ASC_ICON_LOADING,

	ASC_ICON_COUNT
};


#pragma pack( push, 1)
struct SALE_COUPON
{
	char	m_strCouponName[32]; // 쿠폰 이름
	UINT32	m_ui32CouponID;		// 쿠폰 ID
	UINT8	m_ui8CouponType;	// 쿠폰의 기간 타입 ( 절대적이냐 상대적이냐 )
	UINT32	m_ui32PeriodArg;	// 상대적 기간
	UINT32	m_ui32StartDate;	// 언제부터
	UINT32	m_ui32EndDate;		// 언제까지 - 쿠폰 유효기간
	UINT8	m_ui8SaleType;		// 할인 방법 - % 할인이냐 아니면 정가 할인이냐 ( 퍼센트일 경우 1 ~ 50% )
	UINT32	m_ui32DisCount;		// 할인값
	UINT8	m_ui8ItemClass;		// 쿠폰 적용 가능한 아이템 클래스. 해당 아이템 클래스를 구입할때만 쿠폰 사용 가능함.
	UINT8	m_ui8LimitType;		// 금액 제한 타입 - 얼마 이상짜리를 살때만 , 얼마 이하짜리를 살때만
	UINT32	m_ui32Limit;		// 금액 제한값
};

struct BUY_BASKET
{
	UINT32	m_ui32GoodsID;
	UINT8	m_ui8BuyType;
	UINT32	m_ui32CouponID;
	INT64	m_i64CouponIdx;
	BUY_BASKET()
	{
		m_ui32GoodsID = 0;
		m_ui8BuyType = 0;
		m_ui32CouponID = 0;
		m_i64CouponIdx = 0;
	}
};
#pragma pack( pop )
#if defined( USE_QUICK_JOIN )

#define MAX_QUICK_INFO	3

#pragma pack( push, 1)
typedef struct __quickjoininfo
{
	UINT8	_ui8Mode[ MAX_QUICK_INFO ];
	UINT8	_ui8Map[ MAX_QUICK_INFO ];
}QUICK_JOIN_INFO;
#pragma pack( pop )

#endif

#pragma pack( push, 4)

enum	Attendance_Type
{
	ATTENDANCE_TYPE_NONE = 0,
	ATTENDANCE_TYPE_ONECE,	// 특정기간 1회 이벤트
	ATTENDANCE_TYPE_DAILY,	// 매일 이벤트
	ATTENDANCE_TYPE_CONSECUTIVELY	// 연속 이벤트
};

#define ATTENDANCE_MAX_EVENT_COUNT			5	// 최대 설정 가능한 이벤트 개수 ( 중복은 불가능 )
#define ATTENDANCE_MAX_CONSECUTIVE_DAY		7	// 최대 진행 가능한 연속 일수
#define ATTENDANCE_CHOICE_MAX_COUNT			2	// 최대 고를수 있는 아이템 목록 개수
#define	ATTENDANCE_GOODS_MAX_COUNT			ATTENDANCE_MAX_CONSECUTIVE_DAY * ATTENDANCE_CHOICE_MAX_COUNT	// 최대 설정 가능한 아이템 개수
#define ATTENDANCE_NAME_MAX_SIZE			50	// 최대 설정 가능한 이벤트명

struct	ATTENDANCE_CHOICE_ITEM
{
	UINT32	m_ui32GoodsID[ATTENDANCE_CHOICE_MAX_COUNT];

	ATTENDANCE_CHOICE_ITEM()
	{
		for( UINT8 i = 0; i < ATTENDANCE_CHOICE_MAX_COUNT; ++i )
		{
			m_ui32GoodsID[i] = 0;
		}
	}
};

struct	ATTENDANCE_INFO
{
	char	m_strEventName[ATTENDANCE_NAME_MAX_SIZE];
	BOOL	m_bRepeat;	// 반복여부
	UINT8	m_ui8EventType;	// 이벤트 타입. ( 특정기간, 매일, 연속 )
	UINT8	m_ui8AttendanceDay; // 연속 일수
	UINT32	m_ui32Version; // 이벤트 버전. 각 이벤트마다 유니크한 값 이므로 겹칠수 없다.
	UINT32	m_ui32StartDate; // 이벤트 시작날
	UINT32	m_ui32EndDate; // 이벤트 마지막날
	ATTENDANCE_CHOICE_ITEM	m_ui32GiveGoodsID[ATTENDANCE_MAX_CONSECUTIVE_DAY]; // 지급될 아이템 리스트 ( 최대 7일동안 다른 아이템 설정이 가능하므로 2개씩 선택한다고 가정 , 배열 14개 )

	ATTENDANCE_INFO()
	{
		memset( m_strEventName, 0, ATTENDANCE_NAME_MAX_SIZE );
		m_bRepeat = FALSE;
		m_ui8EventType = ATTENDANCE_TYPE_NONE;
		m_ui32Version = 0;
		m_ui32StartDate = 0;
		m_ui32EndDate = 0;
		memset( m_ui32GiveGoodsID, 0, ATTENDANCE_GOODS_MAX_COUNT );
	}
};

struct	ATTENDANCE_USER
{
	UINT32	m_ui32Version; // 마지막으로 진행중이던 이벤트 버전
	UINT8	m_ui8AttendanceDay; // 마지막으로 진행했던 연속 일수
	UINT8	m_ui8AttendanceItem; // 마지막으로 지급받은 아이템 수
	UINT32	m_ui32AttendanceTime; // 마지막으로 출석체크한 시간

	ATTENDANCE_USER()
	{
		m_ui32Version = 0;
		m_ui8AttendanceDay = 0;
		m_ui8AttendanceItem = 0;
		m_ui32AttendanceTime = 0;
	}
};
#pragma pack( pop )

#pragma pack( push, 4)
typedef struct __tag_ChannelBonus
{	
	UINT16	_ui16Type;			// CHANNEL_TYPE
	UINT32	_ui32Point;
	UINT32	_ui32Exp;
}TYPE_CHANNEL_BONUS;
#pragma pack( pop )

/****************************************************************************************/
//					국가 설정
//
//	외국일 경우 국가에 맞게 Build Symbol을 Preprocessor Definitions에 추가 바람
//	LOCALE_CODE 에 해당 나라 값을 넣어주세요.
//
//	한국:		LOCALE_KOREA
//	태국:		LOCALE_THAILAND_ORG
//	인도네시아:	LOCALE_INDONESIA
//	러시아:		LOCALE_RUSSIA	
//	터키:		LOCALE_TURKEY
//	중국:		LOCALE_CHINA
//	대만:		LOCALE_TAIWAN
//	일본:		LOCALE_JAPAN
//	브라질:		LOCALE_BRAZIL
//	중남미:		LOCALE_LATIN_AMERICA
//	미국:		LOCALE_NORTHAMERICA
//	이탈리아 :  LOCALE_ITALY
//	English	:   LOCALE_ENGLISH
//	중동	:	LOCALE_MIDDLE_EAST
//  필리핀 :	LOCALE_PHILIPPINES 
//	태국Garena:	LOCALE_THAILAND
/****************************************************************************************/
enum LOCALE_CODE
{
	LOCALE_CODE_NONE			= 0,
	LOCALE_CODE_KOREA			= 1,
	LOCALE_CODE_THAILAND		= 2,
	LOCALE_CODE_INDONESIA		= 3,
	LOCALE_CODE_RUSSIA			= 4,
	LOCALE_CODE_TURKEY			= 5,
	LOCALE_CODE_CHINA			= 6,
	LOCALE_CODE_TAIWAN			= 7,
	LOCALE_CODE_JAPAN			= 8,
	LOCALE_CODE_BRAZIL			= 9,
	LOCALE_CODE_LATIN_AMERICA	= 10,
	LOCALE_CODE_NORTHAMERICA	= 11,
	LOCALE_CODE_ITALY			= 12,
	LOCALE_CODE_ENGLISH			= 13,
	LOCALE_CODE_MIDDLE_EAST		= 14,
	LOCALE_CODE_PHILIPPINES		= 15,
	LOCALE_CODE_MATCH			= 16,
	LOCALE_CODE_MAX,
};

#if defined( LOCALE_KOREA)
	#if defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY) || defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_THAILAND_ORG)
	#if defined( LOCALE_THAILAND) || defined( LOCALE_KOREA) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_THAILAND)
	#if defined( LOCALE_THAILAND_ORG) || defined( LOCALE_KOREA) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_INDONESIA)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_RUSSIA)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_TURKEY)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_CHINA)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_BRAZIL)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_LATIN_AMERICA)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_NORTHAMERICA)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_ITALY)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_ITALY)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA)|| defined(LOCALE_ENGLISH) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_ENGLISH)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY) || defined(LOCALE_MIDDLE_EAST) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_MIDDLE_EAST)
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY) || defined(LOCALE_ENGLISH) || defined(LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_PHILIPPINES )
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY) || defined(LOCALE_ENGLISH)|| defined(LOCALE_MIDDLE_EAST) || defined( LOCALE_TAIWAN )
		#error "You must select locale by defining one"
	#endif
#elif defined( LOCALE_TAIWAN )
	#if defined( LOCALE_KOREA) || defined( LOCALE_THAILAND_ORG) || defined( LOCALE_THAILAND) || defined( LOCALE_INDONESIA) || defined( LOCALE_RUSSIA) || defined( LOCALE_TURKEY) || defined( LOCALE_CHINA) || defined( LOCALE_BRAZIL) || defined( LOCALE_LATIN_AMERICA) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_ITALY) || defined(LOCALE_ENGLISH)|| defined(LOCALE_MIDDLE_EAST) || defined( LOCALE_PHILIPPINES )
		#error "You must select locale by defining one"
	#endif
#endif

/****************************************************************************************/
//	게임가드 사용여부
/****************************************************************************************/
#if defined(DEV_BUILD) || defined(I3_RELEASE_PROFILE)

	// DEV_BUILD는 보안 솔루션을 사용하지 않습니다.

#else
	#if !defined( I3_DEBUG)
		#if defined( LOCALE_KOREA)
			#define _NPROTECT_

		#elif defined( LOCALE_THAILAND_ORG)
			#if !defined( BUILD_RELEASE_QA_VERSION)// 게임가드 문제로 인해 QA빌드는 적용하지 않으며 라이브 빌드만 게임가드 작동
				#define _NPROTECT_
				// 웹세션 로그인만 허용
				#define USE_ONLY_WEB_LOGIN
			#endif
		#elif defined( LOCALE_THAILAND)
			#define _HACK_SHIELD_

		#elif defined ( LOCALE_INDONESIA)
			#define _HACK_SHIELD_						//	V3 핵쉴드 사용
			#define USING_HSMS

		#elif defined ( LOCALE_RUSSIA)
			#if !defined(BUILD_RELEASE_QA_VERSION)
			#define	_FROST_
			#endif
		#elif defined ( LOCALE_TURKEY)
			#define _HACK_SHIELD_

		#elif defined ( LOCALE_ITALY)
			#define _HACK_SHIELD_
			#define USING_HSMS

		#elif defined ( LOCALE_CHINA)
			#define _GPK_
		#elif defined ( LOCALE_BRAZIL)
			#define _XTRAP_
			//#define _XTRAP_DUMP
		#elif defined( LOCALE_LATIN_AMERICA)
			#define _HACK_SHIELD_
		
		#elif defined( LOCALE_NORTHAMERICA)
			#define _HACK_SHIELD_
			#define USING_HSMS
		
		#elif defined ( LOCALE_ENGLISH)

		#elif defined ( LOCALE_MIDDLE_EAST )
			#define _HACK_SHIELD_
			#define USING_HSMS

		#elif defined ( LOCALE_PHILIPPINES  )
			#define _HACK_SHIELD_
			#define USING_HSMS

		#elif defined ( LOCALE_TAIWAN )
			// 웹로그인을 끄면 핵쉴드를 사용 안함
			#ifndef LOCALE_TAIWAN_NO_WEBLOGIN
				#define _HACK_SHIELD_
				#define USING_HSMS
			#endif
		#else
		#endif

	#if defined( BUILD_RELEASE_QA_VERSION)
		#undef _HACK_SHIELD_						//	V3 핵쉴드 사용 안함.
	#endif

	#endif
#endif

/****************************************************************************************/
//					Server & Client 모두 동일하게 적용되는 디파인
/****************************************************************************************/
#if defined(DEV_BUILD)

	#define	LOCALE_NUMBER	LOCALE_CODE_NONE	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT					// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500					// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD					// 모드 전적 적용

#elif defined (LOCALE_KOREA)

	#define	LOCALE_NUMBER	LOCALE_CODE_KOREA	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT					// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500					// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD					// 모드 전적 적용

#elif defined (LOCALE_THAILAND_ORG)

	#define	LOCALE_NUMBER	LOCALE_CODE_THAILAND	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용		
	
#elif defined (LOCALE_THAILAND)

	#define	LOCALE_NUMBER	LOCALE_CODE_THAILAND	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용		
	
#elif defined (LOCALE_INDONESIA)
	
	#define	LOCALE_NUMBER	LOCALE_CODE_INDONESIA		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용

#elif defined (LOCALE_MIDDLE_EAST)

	#define	LOCALE_NUMBER	LOCALE_CODE_MIDDLE_EAST		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_RUSSIA)

	#define	LOCALE_NUMBER	LOCALE_CODE_RUSSIA		// 나라 번호입니다.
	//#define	USE_PACKET_ENCRIPT					// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용		

#elif defined (LOCALE_TURKEY)

	#define	LOCALE_NUMBER	LOCALE_CODE_TURKEY		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용		

#elif defined (LOCALE_CHINA)

	#define	LOCALE_NUMBER	LOCALE_CODE_CHINA		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용			
	#define LINK_SDOA_LOGIN
	#define LINK_SDOA_FCM_INFO
	#define	LINK_SDOA_FCM
	#define LINK_SDOA_BILLING

	#define	USE_SDOA_LOGIN				// 중국 인증 모듈을 사용하는 경우

#elif defined (LOCALE_BRAZIL)

	#define	LOCALE_NUMBER	LOCALE_CODE_BRAZIL		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD						// 모드 전적 적용		
	
#elif defined (LOCALE_LATIN_AMERICA)

	#define	LOCALE_NUMBER	LOCALE_CODE_LATIN_AMERICA	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_NORTHAMERICA)

	#define	LOCALE_NUMBER	LOCALE_CODE_NORTHAMERICA	// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		
	 
#elif defined (LOCALE_ITALY)

	#define	LOCALE_NUMBER	LOCALE_CODE_ITALY			// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_ENGLISH)

	#define	LOCALE_NUMBER	LOCALE_CODE_ENGLISH			// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_MIDDLE_EAST)

	#define	LOCALE_NUMBER	LOCALE_CODE_MIDDLE_EAST		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_PHILIPPINES )

	#define	LOCALE_NUMBER	LOCALE_CODE_PHILIPPINES		// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#elif defined (LOCALE_TAIWAN)

	#define	LOCALE_NUMBER	LOCALE_CODE_TAIWAN			// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		


#elif defined (LOCALE_MATCH)
	
	#define	LOCALE_NUMBER	LOCALE_CODE_MATCH			// 나라 번호입니다.
	#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
	#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD							// 모드 전적 적용		

#else

	#define	LOCALE_NUMBER	LOCALE_CODE_NONE	// 나라 번호입니다.
	// 공통 빌드
	// 전적 시스템
	#define SUBTRACTE_POINT	500					// 유저 전적 상세보기시 차감되는 포인트
	//#define USE_MODE_RECORD					// 모드 전적 적용	
 
#endif

#define USE_CC_MODE
//#define USE_SHOP_GIFT_SEND				// (상점) 선물하기 [주석처리시 상점 내 선물하기 버튼이 Disable 됩니다.]
//#define USE_SHOP_GIFT_CHECK_RANK		// (상점) 선물하기시 계급 제한

//#define		USE_LOG
#ifdef LOCALE_CHINA
#define LINK_SDOA_LOGIN
#define LINK_SDOA_FCM_INFO
#define	LINK_SDOA_FCM
#define LINK_SDOA_BILLING

//#define USE_FCM_LOG

#define	USE_SDOA_LOGIN				// 중국 인증 모듈을 사용하는 경우

#define USE_XMAS_POPUP				// 크리스마스 팝업 사용 유무

#define USE_LOG_KILLREASON			// 서버에서 클라 종료시키는 코드 의심부분
#endif

#ifdef I3_DEBUG
	#define USE_LOG_KILLREASON
#endif

#define		USE_BIGINT_UID				// uid를 int64로 사용
//#define DEF_OBSERVER_MODE				// 옵저버 모드
#define		USE_MESSENGER_SHILD			// 메신저 서버의 부하 처리를 위해 사용

// 릴레이 서버나, 랑데뷰 서버 둘중에 하나만 켜져 있어야 합니다.
//#define	USE_TEST_NETSEND
#define USE_COLOR_NICK
//#define USE_COLOR_NICK2

// #define		USE_RESPAWN_HACKING_CHECK	// 리스폰 해킹 체크 사용.

// TypeCast
typedef		INT64		TUID;
typedef		INT32		CTUID;
typedef		INT64		TWareDxIdx;
typedef		INT32		TGiftDxIdx;

// New 버전입니다. Define 새로 갑니다. 이유는.. 추후에 클라이언트 까지 바뀌면 위에꺼 삭제.
#define		MAX_QUEST_PER_CARD_NEW				4		//4 한개의 카드가 가질 수 있는 퀘스트
#define		MAX_CARD_PER_CARDSET				10		//각각 P,O,I,N,T,B,L,A,N2,K 총 10개의 카드가 한개의 카드셋안에 들어있다.
#define		MAX_CARDSET_PER_USER				4		//4가지 종류의 카드를 저장한다.
#define		TUTORIAL_CARDSET_SLOT				0
#define		EVENT_CARDSET_SLOT					(MAX_CARDSET_PER_USER - 1)
#define		MAX_QUEST_PER_CARDSET_NEW			(MAX_QUEST_PER_CARD_NEW * MAX_CARD_PER_CARDSET)							//4 * 10	총 40개입니다.
#define		MAX_QUEST_ALL_CARD					(MAX_CARD_PER_CARDSET * MAX_CARDSET_PER_USER)							//10 * 4 
#define		MAX_QUEST_ALL_USE_NEW				(MAX_QUEST_PER_CARD_NEW * MAX_CARD_PER_CARDSET * MAX_CARDSET_PER_USER)	//4 * 10 * 4 총 160개입니다. 

/////////////////////////////////////////////////
// 피로도 시스템
enum FCM_STATE_TYPE
{
	FCM_LOGOUT				= 0,
	FCM_NOMAL_STATE1		= 60,		// 정상 상태(정상 보상, 누적 1시간 경고 메시지)	
	FCM_NOMAL_STATE2		= 120,		// 정상 상태(정상 보상, 누적 2시간 경고 메시지)		
	FCM_NOMAL_STATE3		= 180,		// 피로한 상태(정상 보상, 누적 3시간 경고 메시지)		
	FCM_TIRED_STATE			= 210,		// 피로한 상태(누적 3.5시간이상, 게임 수익(경험치,포인트) 50% 감소, 30분마다 경고메시지) - 게임시간 4시간이상 누적	
	FCM_UNHEALTHY_STATE		= 300,		// 비건강 상태(누적 5시간, 게임 수익 없음, 15분마다 경고메시지) - 게임시간 5시간이상 누적

};

/////////////////////////////////////////////////

// 임무시스템 카드셋
enum ENUM_QUEST_CARDSET_TYPE
{
	QUEST_CARDSET_TYPE_NONE				=	0,
	QUEST_CARDSET_TYPE_TUTORIAL			=	1,		
	QUEST_CARDSET_TYPE_DINO_TUTORIAL	=	2,		//공룡미션 공룡 튜토리얼, QUEST_CARDSET_TYPE_NORMAL는 삭제 됨
	QUEST_CARDSET_TYPE_HUMAN_TUTORIAL	=	3,		//공룡미션 인간 튜토리얼, QUEST_CARDSET_TYPE_SILVER는 삭제 됨
	QUEST_CARDSET_TYPE_GOLD				=	4,		//삭제 됨
	QUEST_CARDSET_TYPE_ASSAULT			=	5,		//돌격
	QUEST_CARDSET_TYPE_BACKUP			=	6,		//지원
	QUEST_CARDSET_TYPE_INFILTRATION		=	7,		//침투
	QUEST_CARDSET_TYPE_SPECIAL			=	8,		//특공
	QUEST_CARDSET_TYPE_DEFCON			=	9,		//데프콘
	QUEST_CARDSET_TYPE_COMMISSIONED_O	=	10,		//사관 
	QUEST_CARDSET_TYPE_COMPANY_O		=	11,		//위관
	QUEST_CARDSET_TYPE_FIELD_O			=	12,		//영관
	QUEST_CARDSET_TYPE_EVENT			=	13,		//이벤트	// 이벤트 카드 13번으로 고정 (20110310) 추가되는것은 이 뒤로 추가하세요.
	QUEST_CARDSET_TYPE_DINO_2ND			=	14,		//공룡미션 공룡 세컨드
	QUEST_CARDSET_TYPE_HUMAN_2ND		=	15,		//공룡미션 인간 세컨드
	QUEST_CARDSET_TYPE_DINO_3RD			=	16,		//공룡미션 공룡 3rd
	QUEST_CARDSET_TYPE_HUMAN_3RD		=	17,		//공룡미션 인간 3rd

	QUEST_CARDSET_TYPE_COUNT,
};

#define QUEST_CARDSET_TYPE_BUY_START		(QUEST_CARDSET_TYPE_DINO_TUTORIAL)		// 구입가능 카드셋 시작
#define QUEST_CARDSET_TYPE_BUY_END			(QUEST_CARDSET_TYPE_HUMAN_3RD)			// 구입가능 카드셋 끝

//#ifdef LOCALE_THAILAND
//#define USE_QUEST_RENEWAL
//#define USE_MEDAL_RENEWAL
//#endif
// 

#define GPK_SETINFO_PACKETBUFF_SIZE				128
#define GPK_SETINFO_PACKETBUFF_SIZE_NP			127

//기존버전에 있는것 모두 삭제할 수는 없다. 아직 파일이 변경되지 않음.. 
#define		MAX_QUEST_PER_CARD				16		//16 한개의 카드가 가질 수 있는 퀘스트
#define		MAX_QUEST_PER_CARDSET			MAX_QUEST_PER_CARD*MAX_CARD_PER_CARDSET							//16 * 10	총 160개입니다.
#define		MAX_QUEST_ALL_USE				MAX_QUEST_PER_CARD*MAX_CARD_PER_CARDSET*MAX_CARDSET_PER_USER	//16 * 10 * 4 총 640개입니다. 

#define		MAX_QUEST_ROW_PER_CARD			4 //카드 한장의 최대 열개수
#define		MAX_QUEST_COL_PER_CARD			4 //카드 한장의 최대 행개수
#define		MAX_QUEST_ITEM_CARD			5 //퀘스트 완료시 지급 가능한 아이템 종류

// 임무카드셋 가격
extern const UINT32 g_QuestCardSetBuyPoint[QUEST_CARDSET_TYPE_COUNT]; 
extern const UINT32 g_QuestCardBuyPoint[QUEST_CARDSET_TYPE_COUNT][MAX_CARD_PER_CARDSET];
/////////////////////////////////////////////////
//
// QUEST_ID
//
// UINT16으로 표현되는 0-based Index이며, 0~65534(0x0000 ~ 0xFFFD)까지 사용 가능하다. 
//65534(0xFFFE) 값은 NONE의 의미로 사용된다. 
//65535(0xFFFF) 값은 Random의 의미로 사용된다. 
typedef UINT16			QUEST_ID;

#define		MAX_QUEST_ARG_COMPLETE	5			// QUEST_CTX 배열중 임무 개수를 담고있는 배열
#define		MAX_QUEST_ARG			6			// QUEST_CTX 배열의 개수입니다.

/////////////////////////////////////////////////////////////////
// 
// 서버에서 저장용, 네트워크 전달용으로 사용합니다. 
//
// Quest의 진행 상태를 저장하는데 사용되는 자료형
//
typedef struct ALIGN1 _QuestGetItemInfo
{
	//col
	UINT8 m_ColPoint[MAX_QUEST_COL_PER_CARD]; 
	UINT8 m_ColExp[MAX_QUEST_COL_PER_CARD]; 
	UINT8 m_ColPrize[MAX_QUEST_COL_PER_CARD]; 	

	//Row
	UINT8 m_RowPoint[MAX_QUEST_ROW_PER_CARD]; 
	UINT8 m_RowExp[MAX_QUEST_ROW_PER_CARD]; 
	UINT8 m_RowPrize[MAX_QUEST_ROW_PER_CARD]; 	

	//Last
	UINT8 m_Point; 
	UINT8 m_Exp; 
	UINT8 m_Prize; 	
	UINT8 m_AlineDum;
}QUEST_ITEM_INFO; 

#pragma pack( push, 4)	// 서버, 클라이언트 동일 파싱 규칙을 적용하기 위해 사용

typedef struct ALIGN1 _QuestGetItemInfoNew
{
	// 2011. 4. 7 수정 - 서동권

	UINT32 m_ui32Point;
	UINT32 m_ui32Exp; 
	UINT32 m_ui32Prize;

	struct ItemInfo // 지급할 아이템 구조체
	{
		INT32		m_i32Editor_index;			// ChallengeEditor 에서 사용하기 위한 Index , 툴을 제외한 다른 곳에서는 필요없다.
		INT8		m_i8AuthType;				// 지급할 아이템의 지급 타입 ( 개수제 , 기간제 )
		UINT32		m_ui32ItemID;				// 지급할 아이템의 코드
		UINT32		m_ui32Arg;					// 이 아이템을 몇개를 줄것인가
	};

	ItemInfo m_Item[MAX_QUEST_ITEM_CARD]; // 최대 MAX_QUEST_ITEM_CARD 종류의 아이템까지 지급할 수 있도록

}QUEST_ITEM_INFO_NEW;

// 2011. 4. 12 - 서동권
// 카드별 보상과 최종 보상을 하나로 합쳐 편리하게 관리하도록 하였습니다.

typedef struct ALIGN1 _QuestGetItemInfoNew2
{
	QUEST_ITEM_INFO_NEW m_CardReward[MAX_CARD_PER_CARDSET]; // 각 카드별 보상 정보
	QUEST_ITEM_INFO_NEW m_CompleteReward;					// 카드셋 완료 최종 보상 정보

}QUEST_ITEM_INFO_NEW2;

#pragma pack( pop )

#define GET_EXP_POINT_MULTI_VALUE		10		//보상을 받는 Exp, Point  UINT8을 사용합니다.. 화면에 출력시에는 * 10을 해서 보여 줘야하고 보상도 * 10을 해줘야 합니다.

#pragma pack( push, 1)

typedef struct ALIGN1 _QuestIngInfo
{
	//ActiveSetInfo
	UINT8			m_ActiveIndexUse;													//진행중인 카드셋 인덱스  0 1 2 3  
	UINT8			m_pActiveIndexCardSet[MAX_CARDSET_PER_USER];						//각 카드셋에서 진행중인 카드번호 0 1 2 3 4 5 6 7 8 9 
	UINT16			m_pActiveIndexCard[MAX_CARDSET_PER_USER][MAX_CARD_PER_CARDSET];		//각 카드의 진행 상태 미성공 0 성공 F

	//카드 사용여부
	UINT8			m_pCardSetType[MAX_CARDSET_PER_USER];								//카드 셋타입	0이면 없음 
}QUESTING_INFO;
#pragma pack( pop )

#ifdef I3_DEBUG
char *g_preMacroDateToInt();
#endif


#pragma pack( push, 1 )
/////////////////////////////////////////////////////////////////////
//For P2P
//----------------------------------------------------------------------------------------------------------------
// InGame [P2P] Default UDP Packet Header
// N_GAME_HEADER
//	기본 Packet header
struct N_GAME_HEADER
{
	UINT8	_ProtocolID;	// 데이타 프로토콜 타입
	UINT8	_Index;			// 유저 슬롯 인덱스
	REAL32	_GameTime;		// Game Time
	UINT8	_RoundNum;		// round number : 일치하지 않으면 패킷 버림
	UINT16	_Size;			// 패킷 사이즈
	UINT8	_RespawnCount;	// 리스폰 카운트( 리스폰 싱크를 맞추기 위한 값으로 데디 서버와 값이 다를경우 무시됩니다. - 일부 패킷 제외 )
	UINT8	_ui8UID;		// user id(64bit)를 % 256한 값(user 확인용)

	void	Reset()
	{
		_ProtocolID = _Index = 0;
		_GameTime = 0.f;
		_RoundNum = 0;
		_Size = 0;
		_RespawnCount = 0;
		_ui8UID = 0;
	}

	void	CopyTo( N_GAME_HEADER * pDest )
	{
		pDest->_ProtocolID		= _ProtocolID;
		pDest->_Index			= _Index;
		pDest->_GameTime		= _GameTime;
		pDest->_RoundNum		= _RoundNum;
		pDest->_Size			= _Size;
		pDest->_RespawnCount 	= _RespawnCount;
		pDest->_ui8UID		 	= _ui8UID;
	}

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (N_GAME_HEADER*) pPacket);
		return (UINT16) sizeof(N_GAME_HEADER);
	}
} ;		// 8bytes

#pragma pack( pop )

INT16 g_Encryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize);
INT16 g_Decryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize);


/////////////////////////////////////////////////////////////////////
#define RENDEZVOUS_SEND_PORT_COUNT			2
#define RENDEZVOUS_RECV_PORT_COUNT			4

//버퍼생성 카운트 
#define FRIEND_MAX_COUNT					50			//친구 생성 맥스 카운트 
#define BLOCK_MAX_COUNT						50			// 차단 생성 맥스 카운트

#define BLOCK_COMMENT_MAX					101			// 차단 사유 최대 값 ( 영문 기준 )

// Event Receive Code
// 각 Scope 이하에서 Error/Fail이 유일한 경우에만 EVENT_ERROR_FAIL을 사용한다.
// 다양한 Error/Fail이 존재할 경우 각각을 define하여 사용해야한다.

// ERC - SNDA Result Define
#define EVENT_ERROR_SNDA_SPEND								0x70000000	// 캐쉬 인증 재확인 요청

// ERC - Common define
#define EVENT_ERROR_SUCCESS									0x00000000	// 기본 성공
#define EVENT_ERROR_FAIL									0x80000000	// 기본 실패
#define EVENT_ERROR_MASK_FAIL								0x80FFFFFF	// ERC Fail Bit-mask

// ERC - Macro Function
#define EV_SUCCESSED(arg)									((arg) >= 0)
#define EV_FAILED(arg)										((arg) < 0)
#define EV_IS_ERROR(arg,err)								(((arg) & (EVENT_ERROR_MASK_FAIL)) == (err))

#define EVENT_ERROR_SUCCESS_1								0x00000001	// 기본 성공 1

// ERC - Game scope
#define EVENT_ERROR_GAME_WRONG_POSITION						0x80000001	// 명령을 실행할 수 없는 포지션에 있습니다. 이메시를 받으면 다시 시작위치로 이동합니다. 서버에서도 리셋합니다.	-2147483647
#define EVENT_ERROR_GAME_WRONG_SAME_REQ						0x80000002	// 채널이 선택되있지않음, CServerStage구현 후 redefine 예정 - praptor
#define EVENT_ERROR_GAME_WRONG_REQUEST_LOGIN				0x80000003	// 로그인 하지 않은 유저가 로그인 이후의 동작을 요구한 경우 : 해킹 입니다.
#define EVENT_ERROR_GAME_WRONG_REQUEST_CHANNEL				0x80000004	// 채널선택을 하지않은 유저가 채널선택 이후의 동작을 요구한 경우 : 해킹 입니다.									-2147483644
#define EVENT_ERROR_GAME_WRONG_REQUEST_ROOM					0x80000005	// 방에 없는 유저가 방안에 있는 동작을 요구한 경우 : 해킹입니다. 
#define EVENT_ERROR_GAME_WRONG_DATA							0x80000006	// 보낼수 없는 값을 요청한 경우 : 해킹이나..깨진 데이터 입니다. 

// ERC - Stage scope Login
#define EVENT_ERROR_LOGIN									0x80000100
#define EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN				0x80000101	// 동일 아이디로 접속된 사용자가 있음
#define EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT			0x80000102	// 아이디나 패스워드가 틀림
#define EVENT_ERROR_LOGIN_BREAK_SESSION						0x80000103	// Session이 깨졌음
#define EVENT_ERROR_EVENT_LOGOUTING							0x80000104	// 아직 로그아웃 중입니다.
#define EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1					0x80000105	// 로그인-서버에 문제가 있습니다.
#define EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2					0x80000106	// 로그인-서버에 문제가 있습니다.
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT				0x80000107	// 계정제재
#define EVENT_ERROR_EVENT_LOG_IN_UNKNOWN					0x80000108	// 처리 외 이유로 로그인 실패
//인도 네시아 로그인으로 추가됨 
#define EVENT_ERROR_EVENT_LOG_IN_PACKET						0x80000109	// 패킷 깨짐 
#define EVENT_ERROR_EVENT_LOG_IN_MD5						0x8000010A	// MD5 
#define EVENT_ERROR_EVENT_LOG_IN_HTTP						0x8000010B	// HTTP ERROR
#define EVENT_ERROR_EVENT_LOG_IN_SYS						0x8000010C	// HTTP ERROR
#define EVENT_ERROR_EVENT_LOG_IN_NOTAVAILABLE				0x8000010D	// HTTP ERROR
#define EVENT_ERROR_EVENT_LOG_IN_MAXUSER					0x8000010E	// 
#define EVENT_ERROR_EVENT_SIA_USERITEM_SUCCESS				0x8000010F	// SIA 를 사용하지 않음
#define EVENT_ERROR_EVENT_BUY_GOODS_REPAIR					0x80000110	// Repair 실패
#define EVENT_ERROR_EVENT_BUFFER_FULL						0x80000111	// 버퍼가 가득 찼다.
#define EVENT_ERROR_DATABASE								0x80000112	// DATABASE ERROR
#define EVENT_ERROR_EVENT_DUPLICATE_NICK					0x80000113	// 닉네임 중복
#define EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL				0x80000114	// DB Buffer 가득 참
// 중남미 로그인으로 추가됨
#define EVENT_ERROR_EVENT_LOG_IN_SERVER_INSPECT_TIME		0x80000115	// 서버 점검중 입니다.
#define EVENT_ERROR_EVENT_LOG_IN_NO_SERVICE_GAME			0x80000116	// 서비스 되는 게임이 아닙니다.
// 터키 로그인으로 추가됨
#define EVENT_ERROR_EVENT_LOG_IN_ID_FAIL							0x80000117	// ID 오류
#define EVENT_ERROR_EVENT_LOG_IN_PW_FAIL							0x80000118	// PW 오류
#define EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT			0x80000119	// 삭제된 계정
#define EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR		0x80000120	// email 인증 실패
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP						0x80000121	// ip 블럭

// 이탈리아 로그인으로 추가됨
#define EVENT_ERROR_EVENT_LOG_IN_DB							0x80000122	// Login DB 오류

// 북미 로그인으로 추가됨
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_COUNTRY				0x80000123	// Block 된 국가
#define EVENT_ERROR_EVENT_LOG_IN_CUSTOM_MESSAGE				0x80000124	// 북미에서 보내준 에러 메세지를 클라이언트에 전송하여 띄우게 합니다.

// 대만 로그인으로 추가됨 
#define EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB				0x80000125	// DB 서버 접속 실패.
#define EVENT_ERROR_EVENT_LOG_IN_BUSY_SERVER				0x80000126	// 서버가 바쁘다?.
#define EVENT_ERROR_EVENT_LOG_IN_WRONG_INPUT_DATA			0x80000127	// Input Data ( TCP 패킷) 포맷이 잘못된다.
#define EVENT_ERROR_EVENT_LOG_IN_SEND_PACKET				0x80000128	// 로그인 패킷 전송 실패.
#define EVENT_ERROR_EVENT_LOG_IN_TIME_OUT_TAIWAN			0x80000129	// 지정된 시간안에 대만 로그인 서버로부터 ACK 패킷을 받지 못했다.

// 인도네시아 PC방 연동으로 추가됨 - 인니 PC방 웹서버가 보내는 값으로 Block PC방만 접속 실패처리고 나머지는 오류코드는 사용하되 접속은 성공시킴 ( 음수가 아니라 양수 )
#define EVENT_ERROR_EVENT_BLOCK_PCBANG						0x80000125	// 접속이 차단되있는 PC방
#define EVENT_ERROR_EVENT_UNKNOWN_KEY						0x00000126	// 인증키 정보가 정확하지 않음 - 음수가 아닙니다.
#define EVENT_ERROR_EVENT_EXPIRATION_KEY					0x00000127	// 인증키 정보가 만료됨 - 음수가 아닙니다.
#define EVENT_ERROR_EVENT_UNKOWN_SYSTEM_ERROR				0x00000128	// 알수없는 시스템 오류 - 음수가 아닙니다.

// ERC - Stage scope Server
#define EVENT_ERROR_SERVER									0x80000200
#define EVENT_ERROR_SERVER_USER_FULL						0x80000201	// 채널에 사람이 꽉 찼음, CServerStage구현 후 redefine 예정 - praptor
#define EVENT_ERROR_CHANNEL_ENTER_CONDITION					0x80000202	// 채널 입장 실패
#define EVENT_ERROR_CHANNEL_ENTER_CONDITION_BATTLETIME		0x80000203	// 채널 입장 실패(배틀시간이 10시간 미만이라 초보2채널외엔 들어갈수 없음)
#define EVENT_ERROR_CHANNEL_ENTER_CONDITION_GWARNET			0x80000204	// 채널 입장 실패(와루넷 전용 채널인데 와루넷 유저가 아닌 경우)

// ERC - Stage scope Lobby
#define EVENT_ERROR_LOBBY									0x80000300

// ERC - Stage scope ReadyRoom
#define EVENT_ERROR_ROOM									0x80000400
#define EVENT_ERROR_ROOM_NO_AUTHORITY						0x80000401	// 권한없는 작업

// ERC - Event scope
// 로그는 연관되어있는 것들로 차차 묶어나갈 예정입니다. -- 20100106 김현우...
#define EVENT_ERROR_EVENT									0x80001000
#define EVENT_ERROR_EVENT_TRANS								0x80001001	// Trans 버퍼가 가득차 더이상 Send 할수 없음
#define EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL				0x80001003	// 방안에 유저가 다 차있음
#define EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM			0x80001004	// 방이 이미 사라졌음
#define EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD			0x80001005	// 방 비밀번호가 다름
#define EVENT_ERROR_EVENT_BATTLE_TIMEOUT_CN					0x80001006	// 타임아웃 에러이다.. 로비로 강제 이동합니다.
#define EVENT_ERROR_EVENT_BATTLE_TIMEOUT_CS					0x80001007	// 타임아웃 에러
#define EVENT_ERROR_NO_REAL_IP								0x80001008	// RealIP가 없다
#define EVENT_ERROR_NO_READY_TEAM							0x80001009	// 2팀이 되지 않았다
#define EVENT_ERROR_FIRST_MAINLOAD							0x8000100A	// 로딩시간으로 아웃(방장)
#define EVENT_ERROR_FIRST_HOLE								0x8000100B	// 홀펀칭으로 아웃
#define EVENT_ERROR_ROOM_JOIN_VOTE							0x8000100C	// 방안에 입장할 수 없습니다.
#define EVENT_ERROR_WAIT_BATTLE_CLIMAX						0x8000100F	// ! praptor - 사용하지 않습니다.

#define EVENT_ERROR_NO_START_FOR_UNDER_NAT					0x80001012	// 게임을 시작할 수 없는 NAT 아래에 있음
#define EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER			0x80001013	// 난입이 불가능한 방 
#define EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL				0x80001014	// 난입이 불가능한 방 
#define EVENT_ERROR_PRESTART_BATTLE_ALEADY_END				0x80001015	// 난입 시 이미 게임이 끝났다
#define EVENT_ERROR_START_BATTLE_ALEADY_END					0x80001016  // 난입 시 이미 게임이 끝났다

#define EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS			0x80001017	// 목록에 없는 상품 - idx overflow
#define EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_USER			0x80001018	// 유저를 찾을 수 없음
#define EVENT_ERROR_EVENT_BUY_GOODS_NETWORK					0x80001019	// IBGame error
#define EVENT_ERROR_EVENT_BUY_GOODS_OVERLOAD				0x8000101A	// 빌링요청 폭주로 당분간 요청을 받을 수없음
#define EVENT_ERROR_EVENT_GET_USER_ITEM_PARAMETER_USER		0x8000101B	// 유저를 찾을 수 없음
#define EVENT_ERROR_EVENT_GET_USER_ITEM_NETWORK				0x8000101C	// IBGame error
#define EVENT_ERROR_EVENT_GET_USER_ITEM_OVERLOAD			0x8000101D	// 빌링요청 폭주로 당분간 요청을 받을 수없음
#define EVENT_ERROR_EVENT_GET_USER_ITEM_FAIL				0x8000101E	// Auth fail - 반환 목록 만들기 실패
#define EVENT_ERROR_EVENT_INSERT_PERMANENT_PARAMETER_USER	0x8000101F	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_INSERT_PERMANENT_NETWORK			0x80001020	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_INSERT_PERMANENT_OVERLOAD			0x80001021	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_INSERT_PERMANENT_FAIL				0x80001022	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_AUTH_START_PARAMETER_USER			0x80001023	// 유저를 찾을 수 없음
#define EVENT_ERROR_EVENT_AUTH_START_NETWORK				0x80001024	// IBGame error
#define EVENT_ERROR_EVENT_AUTH_START_OVERLOAD				0x80001025	// 빌링요청 폭주로 당분간 요청을 받을 수없음
#define EVENT_ERROR_EVENT_AUTH_START_FAIL					0x80001026	// IBGame fail
#define EVENT_ERROR_EVENT_AUTH_START_PRICE					0x80001027	// 돈이없음 
#define EVENT_ERROR_EVENT_AUTH_START_ENDTIME_FAIL			0x80001028	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL			0x80001029  // 아이템을 넣을수 있는 인벤토리가 없습니다. 	
#define EVENT_ERROR_EVENT_BUY_GOODS_ENDTIME_FAIL			0x80001030	// ! 사용하지 않는다.
#define EVENT_ERROR_EVENT_AUTH_START_ALREADY_AUTH			0x80001031	// 이미 인증되었음.
#define EVENT_ERROR_EVENT_AUTH_START_NONSEC					0x80001032	// 기간인증 아이템 아님
#define EVENT_ERROR_EVENT_AUTH_FAIL							0x80001033	// 인증실패
#define EVENT_ERROR_EVENT_LOG_IN_USER_ITEM_FAIL				0x80001034	// 로그인 중 창고 받아오기 실패
#define ERROR_EVENT_FRIEND_INFO_PARAMETER_USER				0x80001035	// 유저정보 찾지 못함
#define ERROR_EVENT_FRIEND_INFO_QUERY						0x80001036	// DB 작업 실패
#define EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER		0x80001037	// 유저정보 찾지 못함
#define EVENT_ERROR_EVENT_FRIEND_INSERT_FULL				0x80001038	// 허용 친구수 다참
#define EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY				0x80001039	// DB 작업 실패
#define EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_USER		0x8000103A	// 유저정보 찾지 못함
#define EVENT_ERROR_EVENT_FRIEND_ACCEPT_PARAMETER_IDX		0x8000103B	// 친구 idx 범위 초과
#define EVENT_ERROR_EVENT_FRIEND_ACCEPT_QUERY				0x8000103C	// DB 작업 실패
#define EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_USER		0x8000103D	// 유저정보 찾지 못함
#define EVENT_ERROR_EVENT_FRIEND_INVITED_PARAMETER_IDX		0x8000103E	// 친구 idx 범위 초과
#define EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_CONNECTED		0x8000103F	// 접속중인 친구 정보 찾지 못함
#define EVENT_ERROR_EVENT_FRIEND_INVITED_NOT_MATCHED		0x80001040	// 상대방에 친구등록정보가 없다
#define EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND		0x80001041	// 이미 등록된 친구
#define EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK		0x80001042	// 닉네임을 찾을 수 없음
#define EVENT_ERROR_EVENT_INSERT_ITEM_VALID_ITEM_ID			0x80001043	// 잘못된 아이템ID
#define EVENT_ERROR_EVENT_INSERT_ITEM_OVERLOAD				0x80001044	// 요청 폭주로 당분간 요청을 받을 수 없음
#define EVENT_ERROR_EVENT_INSERT_ITEM_NETWORK				0x80001045	// IBGame error
#define EVENT_ERROR_EVENT_INSERT_ITEM_PARAMETER_USER		0x80001046	// 유저를 찾을 수 없음
#define EVENT_ERROR_EVENT_INSERT_ITEM_FAIL					0x80001047	// IBGame fail
#define EVENT_ERROR_CLAN_CREATE_FAIL						0x80001048	// DB에 쓰기 실패
#define EVENT_ERROR_CLAN_CREATE_DUPLICATE					0x80001049	// 중복된 클랜(클랜명||azitURL)
#define EVENT_ERROR_CLAN_CREATE_ALEADY_MEMBER				0x8000104A	// 클랜에 가입된 유저
#define EVENT_ERROR_EVENT_INSERT_INFO_FAIL					0x8000104B	// 추가성공하였으나 종료일을 받아내지 못했음
#define EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL			0x8000104C	// 구입할 카드를 찾을 수 없다
#define EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL				0x8000104D	// 돈이 부족하다
#define EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL				0x8000104E	// 그외 이유로 실패
#define EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL		0x8000104F	// 삭제할 카드를 찾을 수 없다
#define EVENT_ERROR_EVENT_DELETE_CARD_UNKOWN_FAIL			0x80001050	// 그외 이유로 실패
#define EVENT_ERROR_EVENT_AUTH_COUNT_PARAMETER_USER			0x80001051	// 유저정보 찾지 못함
#define EVENT_ERROR_EVENT_AUTH_COUNT_NETWORK				0x80001052	// IBGame error
#define EVENT_ERROR_EVENT_AUTH_COUNT_OVERLOAD				0x80001053	// 요청 폭주로 당분간 요청을 받을 수 없음
#define EVENT_ERROR_EVENT_AUTH_COUNT_FAIL					0x80001054	// 개수인증 실패
#define EVENT_ERROR_CLAN_MAXCLAN							0x80001055	// 클랜의 맴버임
#define EVENT_ERROR_CLAN_MAXMEMBER							0x80001056	// 클랜의 멤버가 아님
#define EVENT_ERROR_CLAN_MAXREQUEST							0x80001057	// 클랜에 가입되 있지 않음
#define EVENT_ERROR_CLAN_MEMBER								0x80001058	// 클랜의 맴버임
#define EVENT_ERROR_CLAN_NOMEMBER							0x80001059	// 클랜의 멤버가 아님
#define EVENT_ERROR_CLAN_SEARCH_CLANIDX						0x8000105A	// 클랜에 가입되 있지 않음
#define EVENT_ERROR_CLAN_NOSEARCH_CLANIDX					0x8000105B	// 클랜을 찾을수 없다.
#define EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX				0x8000105C	// 클랜원이 존재한다.
#define EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX				0x8000105D	// 클랜원을 찾을수 없다.
#define EVENT_ERROR_CLAN_LEVEL_MASTER						0x8000105E	// 클랜 레벨이 클랜장이다.
#define EVENT_ERROR_CLAN_LEVEL_NOMASTER						0x8000105F	// 클랜 레벨이 클랜장이 아니다.
#define EVENT_ERROR_CLAN_LEVEL_STAFF						0x80001060	// 클랜 레벨이 스탭이다.
#define EVENT_ERROR_CLAN_LEVEL_NOSTAFF						0x80001061	// 클랜 레벨이 스탭이 아니다.
#define EVENT_ERROR_CLAN_LEVEL_REGULAR						0x80001062	// 클랜 레벨이 일반 회원이다.
#define EVENT_ERROR_CLAN_LEVEL_NOREGULAR					0x80001063	// 클랜 레벨이 일반 회원이 아니다.
#define EVENT_ERROR_CLAN_SEARCH_SECTION						0x80001064	// 색션을 찾았음
#define EVENT_ERROR_CLAN_NOSEARCH_SECTION					0x80001065	// 색션을 찾을수 없음
#define EVENT_ERROR_CLAN_SEARCH_REQUESTIDX					0x80001066	// 가입신청서에 있다
#define EVENT_ERROR_CLAN_NOSEARCH_REQUESTIDX				0x80001067	// 가입신청서에 없다
#define EVENT_ERROR_CLAN_REQUEST_FAIL						0x80001068	// 가입신청 실패
#define EVENT_ERROR_CLAN_SECESSION_FAIL						0x80001069	// 클랜탈퇴 실패
#define EVENT_ERROR_CLAN_CLOSECLAN_FAIL						0x8000106A	// 클랜 해체 실패
#define EVENT_ERROR_CLAN_CLOSECLAN_MEMBER_FAIL				0x8000106B	// 클랜에 가입인원이 남아있어 해체 실패
#define EVENT_ERROR_CLAN_COMMISSION_FAIL					0x8000106C	// 권한변경 실패
#define EVENT_ERROR_CLAN_CANCEL_REQUEST_FAIL				0x8000106D	// 클랜신청서 취소에 실패
#define EVENT_ERROR_CLAN_DEPORTATION_FAIL					0x8000106E	// 클랜원 제명 실패
#define EVENT_ERROR_FIND_USER_DB							0x8000106F	// 유저찾기 DB 실패
#define EVENT_ERROR_FIND_USER_NOT_FOUND						0x80001070	// 유저찾기 없는 유저
#define EVENT_ERROR_NO_CLAN_TEAM							0x80001071	// 팀이 이상하다
#define EVENT_ERROR_NO_CLAN_TEAM_NOTFULL					0x80001072	// 한팀에 4명 이상이 되지 않았다
#define EVENT_ERROR_CLAN_NOTICE_FAIL						0x80001073	// 공지사항 변경 실패
#define EVENT_ERROR_CLAN_INTRO_FAIL							0x80001074	// 클랜 소개 변경 실패
#define EVENT_ERROR_CLAN_MARKCHANGE_FAIL					0x80001075	// 클랜 마크 변경 실패
#define EVENT_ERROR_CLAN_NAMECHANGE_FAIL					0x80001076	// 클랜 이름 변경 실패
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FAIL			0x80001077	// 배틀 클랜팀 만들기 실패
#define EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_FAIL				0x80001078	// 배틀 클랜팀 입장 실패
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CLOSE_FAIL				0x80001079	// 배틀 클랜팀 해체
#define EVENT_ERROR_CLAN_JOIN_AUTH_AGE_FAIL					0x8000107A	// 가입 권한(나이)이 맞지 않는다.
#define EVENT_ERROR_CLAN_JOIN_AUTH_RANK_FAIL				0x8000107B	// 가입 권한(계급)이 맞지 않는다.
#define EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING				0x8000107C	// 사격장은 난입할 수 없다.
#define EVENT_ERROR_EVENT_CREATE_TRAINING_WRONG_CHANNEL		0x8000107D	// 사격장을 만들 수 있는 채널이 아니다.
#define EVENT_ERROR_NOTE_SEND_UKNOWN_NICK					0x8000107E	// 해당 닉네임의 사용자 없다.
#define EVENT_ERROR_NOTE_SEND_BOX_FULL						0x8000107F	// 쪽지함이 꽉 찼다.
#define EVENT_ERROR_NOTE_SEND_SELF							0x80001080	// 스스로에게 쪽지를 보낼 수 없다.
#define EVENT_ERROR_NOTE_SEND_DAILY_LIMIT					0x80001081	// 하루 쪽지 발송 제한에 걸려 보낼 수 없다.
#define EVENT_ERROR_CLAN_RECORDRESET_FAIL					0x80001082	// 클랜 전적을 초기화 할수 없다.
#define EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL				0x80001083	// 호칭 교환 조건을 만족하지 않는다.
#define EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL			0x80001084	// 이미 호칭을 보유중이다.
#define EVENT_ERROR_EVENT_BUY_GOODS_USER_TITLE_NOT_ALLOWED	0x80001085	// 허용된 호칭이 없어 구입할 수 없다.
#define EVENT_ERROR_EVENT_EQUIP_USER_TITLE_NOT_ALLOWED		0x80001086	// 허용된 호칭이 없어 장비할 수 없다.
#define EVENT_ERROR_CLAN_BATTLE_TEAM_HAVE_FAIL				0x80001087	// 배틀 클랜팀 실패_이미 팀을 가지고 있다.
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_FULL_FAIL		0x80001088	// 배틀 클랜팀 만들기 실패_모든 팀이 가득차서 더이상 생성불가
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_PER_FAIL		0x80001089	// 배틀 클랜팀 만들기 실패_제한인원수가 이상하다
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CREATE_NOTEMPTY_FAIL	0x8000108A	// 배틀 클랜팀 만들기 실패_비어있는 클랜이 아니다.
#define EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_NOSEARCH_FAIL		0x8000108B	// 배틀 클랜팀 입장 실패_찾을수 없는 팀
#define EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_EMPTY_FAIL		0x8000108C	// 배틀 클랜팀 입장 실패_없는 방
#define EVENT_ERROR_CLAN_BATTLE_TEAM_JOIN_PERMAX_FAIL		0x8000108D	// 배틀 클랜팀 입장 실패_인원이 가득차 입장할 수 없음
#define EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL		0x8000108E	// 배틀 클랜팀 퇴장 실패_찾을수 없는 팀
#define EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL		0x8000108F	// 배틀 클랜팀 퇴장 실패_없는 유저
#define EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_READY_FAIL		0x80001090	// 배틀 클랜전 신청 실패_이미 시작된 팀
#define EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_SELF_FAIL		0x80001091	// 배틀 클랜전 신청 실패_자기자신에게 신청
#define EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_NOPER_FAIL		0x80001092	// 배틀 클랜전 신청 실패_인원이 맞지 않습니다.
#define EVENT_ERROR_CLAN_BATTLE_TEAM_REQUEST_REFUSAL_FAIL	0x80001093	// 배틀 클랜전 신청 거절
#define EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL			0x80001094	// 존재하지 않는 팀
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_FAIL			0x80001095	// 배틀 클랜전 인원변경 실패_변경할 인원이 이상함
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_PERMAX_FAIL		0x80001096	// 배틀 클랜전 인원변경 실패_인원이 많아서 실패
#define EVENT_ERROR_CLAN_BATTLE_TEAM_CHANGE_NOMASTER_FAIL	0x80001097	// 배틀 클랜전 인원변경 실패_방장이 아닌데 전달 되었음
#define EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY				0x80001098	// 클랜전 실패, 모두 레뒤가 아니다.
#define EVENT_ERROR_CLAN_INVITED_REFUSAL					0x80001099	// 클랜 가입 신청 거절
#define EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_GOODS		0x8000109A	// 선물하기 실패 - 잘못된 상품아이디
#define EVENT_ERROR_CLAN_INCREASE_MAXPERSON_FAIL			0x8000109B	// 클랜 최대인원 증가 실패
#define EVENT_ERROR_EVENT_GIFT_GOODS_PARAMETER_USER			0x8000109C	// 선물하기 실패 - 잘못된 유저
#define EVENT_ERROR_EVENT_GIFT_GOODS_NOT_ENOUGH_POINT		0x8000109D	// 선물하기 실패 - 포인트 부족
#define EVENT_ERROR_EVENT_GIFT_GOODS_WRONG_RECEIVE_USER		0x8000109E	// 선물하기 실패 - 잘못된 상대 유저
#define EVENT_ERROR_EVENT_CHEKC_GIFT_PARAMETER_GOODS		0x8000109F	// 선물받기 실패 - 없는 상품
#define EVENT_ERROR_EVENT_CHECK_GIFT_OVERLOAD				0x800010A0	// 선물받기 실패 - 서버과부화
#define EVENT_ERROR_EVENT_CHECK_GIFT_NETWORK				0x800010A1	// 선물받기 실패 - 네트워크 문제 - IBGameError
#define EVENT_ERROR_EVENT_DELETE_INFO_FAIL					0x800010A2	// 아이템 삭제 실패
#define EVENT_ERROR_EVENT_DELETE_ITEM_VALID_ITEM_ID			0x800010A3	// 잘못된 아이템ID
#define EVENT_ERROR_EVENT_DELETE_ITEM_OVERLOAD				0x800010A4	// 요청 폭주로 당분간 요청을 받을 수 없음
#define EVENT_ERROR_EVENT_DELETE_ITEM_NETWORK				0x800010A5	// IBGame error
#define EVENT_ERROR_EVENT_DELETE_ITEM_PARAMETER_USER		0x800010A6	// 유저를 찾을 수 없음
#define EVENT_ERROR_EVENT_DELETE_ITEM_FAIL					0x800010A7	// IBGame fail
#define EVENT_ERROR_CLAN_USE_ITEM_FAIL						0x800010A8	// 클랜 아이템이 있어서 실패
#define EVENT_ERROR_EVENT_BLOCK_INSERT_ALREADY_BLOCK		0x800010A9	// 이미 차단된 목록
#define EVENT_ERROR_EVENT_BLOCK_INSERT_ALREADY_CLAN			0x800010AA	// 같은 클랜
#define EVENT_ERROR_EVENT_READY_WEAPON_EQUIP				0x800010AB	// 
#define EVENT_ERROR_CLAN_MEMORY_FULL						0x800010AC	// 메모리가 부족하여 더이상 클랜 생성에 실패
#define EVENT_ERROR_HACKING_EXIT_USER						0x800010AD	// 유저가 해킹을 하여 GM이 팅겨냅니다.
#define EVENT_ERROR_GAMEGUARD_ERROR							0x800010AE	// 게임가드에 문제가 있습니다.
#define EVENT_ERROR_LOGIN_ERROR								0x800010AF	// 로그인에 문제가 있습니다.
#define EVENT_ERROR_CHANNEL_TIMEOUT							0x800010B0	// 로그인에 문제가 있습니다.
#define EVENT_ERROR_ABUSING_CHECK_ERROR						0x800010B1	// 어뷰징 체크 에러
#define EVENT_ERROR_EVENT_FRIEND_INSERT_BLOCK_USER			0x800010B2	// 차단되어 친구 추가 요청 실패.
#define EVENT_ERROR_NOTE_SEND_BLOCK_USER					0x800010B3	// 차단되어 쪽지 전송 실패.
#define EVENT_ERROR_GRAVITY_X_CHECK							0x800010B4	// 중력핵 X범위 벗어남.
#define EVENT_ERROR_GRAVITY_Y_CHECK							0x800010B5  // 중력핵 Y범위 벗어남.
#define EVENT_ERROR_GRAVITY_Z_CHECK							0x800010B6  // 중력핵 Z범위 벗어남.
#define EVENT_ERROR_GIVEUPBATTLE_WITHOUT_STARTBATTLE		0x800010B7	// STARTBATTLE_REQ 없이 GIVEUPBATTLE_REQ가 왔습니다. 무적버그 사용하려는 시도입니다. 2012.07.12

#define EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED			0x800010B7  // 장바구니 구입 실패. - 아이템이 여러가지라 이유도 여러개가 될 수 있겠지만 다 알려줄수 없으니 그냥 실패만

#define EVENT_ERROR_FSM_INFO_TIMEOUT						0x800010C1	// 실명인증 처리에 문제가 있습니다.
#define EVENT_ERROR_FSM_LOGIN_TIMEOUT						0x800010C2	// 피로도 로그인에 문제가 있습니다.
#define EVENT_ERROR_FSM_STATE_TIMEOUT						0x800010C3	// 피로도 누적값 요청에 문제가 있습니다.

#define EVENT_ERROR_EVENT_BUY_CARD_NO_GOODS					0x800010D0	// 임무 카드가 상품으로 판매되지 않고 있다.
#define EVENT_ERROR_EVENT_BUY_CARD_TRANS_ERROR				0x800010D1	// 트랜스 서버 연결 오류로 임무 카드 구입 실패.
#define EVENT_ERROR_EVENT_BUY_CARD_NETWORK					0x800010D2	// 네트워크 오류로 임무 카드 구입 실패.
#define EVENT_ERROR_EVENT_BUY_CARD_SLOT_ERROR				0x800010D3	// 임무카드 슬롯 오류로 임무 카드 구입 실패.

#define EVENT_ERROR_EVENT_PACKET_REPEATING					0x800010D4	// 패킷을 복사해서 다시 보냈습니다.

#define	EVENT_ERROR_EVENT_BUY_CARD_DISQUALIFIED_FAIL		0x800010D5	// 임무카드를 구입할 자격이 없다 (튜토리얼 임무를 완료 해야 자격 부여)

// 강제 탈퇴 투표 거절 이유
#define EVENT_ERROR_KICKVOTE_ALREADY_EXIST_OTHER_VOTE		0x800010E0	// 이미 다른 투표가 진행 중입니다.
#define EVENT_ERROR_KICKVOTE_SAME_SUGGEST_IN_ONE_MINUTE		0x800010E1	// 1분내에 같은 신고인, 같은 피신고인의 투표가 부결된적이 있습니다.
#define EVENT_ERROR_KICKVOTE_SMALL_USER_IN_ROOM				0x800010E2	// Room 내에 투표 인원이 2인 이하라 진행할 수 없습니다.
#define EVENT_ERROR_KICKVOTE_NOT_ENOUGH_VOTE_TIME			0x800010E3	// 게임 잔여 시간이 부족합니다.

// 강제 탈퇴 투표의 표 불인정 이유
#define EVENT_ERROR_KICKVOTE_NOTAPPLY_NONE_VOTE				0x800010F0	// 진행중인 투표가 없습니다. ( 투표가 종료되면, 서버에서 투표종료 패킷을 보내서 클라이언트가 투표 패킷을 보낼수 없게 막기 때문에 발생 가능성 낮습니다. )
#define EVENT_ERROR_KICKVOTE_NOTAPPLY_ALREADY_VOTED			0x800010F1	// 이미 투표를 했습니다. ( 투표를 처음하면, 클라이언트 자체내에서 더 이상 투표 패킷을 보낼수 없게 되기 때문에 발생 가능성 극히 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )
#define EVENT_ERROR_KICKVOTE_NOTAPPLY_NO_RIGHT_TO_VOTE		0x800010F2	// 투표권이 없습니다. ( 투표권이 없는 신고자, 피신고자, 난입자는 클라이언트 자체내에서 투표 패킷을 보낼수 없기 때문에 발생 가능성 극리 낮습니다. 이 에러가 자주 발생한다면, 버그나 해킹 의심 )

// 강제 탈퇴 투표 부결 이유
#define EVENT_ERROR_KICKVOTE_REJECTED_SHORTAGES				0x80001100	// 과반수에 도달하지 못했습니다.
#define EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ALLY		0x80001101	// 아군에서 찬성표가 한 표도 없습니다.
#define EVENT_ERROR_KICKVOTE_REJECTED_NO_USER_IN_ENEMY		0x80001102	// 적군에서 찬성표가 한 표도 없습니다.

// 제압미션 HP회복, 탄창증가, 공격레벨업, 방어레벨업 실패 이유
#define	EVENT_ERROR_DOMINATION_NOT_ENOUGH_SP				0x80001200	// SP가 부족합니다.
#define	EVENT_ERROR_DOMINATION_ALREADY_MAX					0x80001201	// 최고레벨, 풀HP, 풀탄창인 상태이기 때문에 더 증가될수 없습니다.


// 차단 리스트 - 에러 코드
#define EVENT_ERROR_EVENT_BLOCK_PARAMETER_USER				0x80001300	// 유저정보 찾지 못함
#define EVENT_ERROR_EVENT_BLOCK_FULL						0x80001301	// 허용 차단유저수 다참
#define EVENT_ERROR_EVENT_BLOCK_QUERY						0x80001302	// DB 작업 실패
#define EVENT_ERROR_EVENT_BLOCK_ALREADY_BLOCK				0x80001303	// 이미 등록된 차단유저
#define EVENT_ERROR_EVENT_BLOCK_NOT_FIND_NICK				0x80001304	// 닉네임을 찾을 수 없음
#define EVENT_ERROR_EVENT_BLOCK_MY_NICKNAME					0x80001305	// 자신의 닉네임으로 차단 추가/삭제 요청.
#define EVENT_ERROR_EVENT_BLOCK_PARAMETER_IDX				0x80001306	// 차단 유저 목록 범위 초과.
#define EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_CLAN			0x80001307	// 동일 클랜원이라서 차단 추가 실패.
#define EVENT_ERROR_EVENT_BLOCK_INSERT_FAIL_FRIEND			0x80001308	// 친구사이라서 차단 유저 추가 실패.
#define EVENT_ERROR_EVENT_BLOCK_BUFFER_OVERFLOW				0x80001309	// 버퍼 오버 플로우.
#define EVENT_ERROR_EVENT_BLOCK_UNKOWN						0x80001310	// 알수없는 이유.
 

// Coupon Award Item
#define EVENT_FAIL_COUPON_AWARD_ITEM_OF						0X84000100	// 
#define EVENT_FAIL_COUPON_AWARD_ITEM_OF_NOT_COUPON_NUMBER	0X84000101	// 쿠폰 번호가 유효하지 않음
#define EVENT_FAIL_COUPON_AWARD_ITEM_OF_IN_BUFFER_FULL		0X84000102	// 쿠폰 보상 쿠폰 요청 버퍼가 꽉 찼습니다. (메시지 : 네트워크 오류 발생.)
#define EVENT_FAIL_COUPON_AWARD_ITEM_OF_DB_BUFFER_FULL		0X84000103	// 디비 버퍼가 꽉 찼습니다.(메시지 : 네트워크 오류 발생.)
#define EVENT_FAIL_COUPON_AWARD_ITEM_OF_SNDA_PROBLUM		0X84000104	// 쿠폰 인증에 문제가 있습니다.(샨다 문제)
#define EVENT_ERROR_COUPON_AWARD_ITEM_COUPON_SIZE			0x84000105	// 쿠폰번호를 이용한 보상시 쿠폰번호의 사이즈가 잘못된 경우
#define EVENT_ERROR_COUPON_AWARD_ITEM_COUPON_NULL			0x84000106	// 쿠폰번호를 이용한 보상시 쿠폰번호의 파라미터가 NULL인 경우
#define EVENT_ERROR_COUPON_AWARD_ITEM_PARAMETER_USER		0x84000107	// 유저를 찾을 수 없음


// 출석체크 이벤트 - 서동권 2011.07.07
#define EVENT_ERROR_ATTENDANCE_PARAMETER_USER				0x80001500	// 유저를 찾을 수 없습니다.
#define EVENT_ERROR_ATTENDANCE_NOT_ENOUGH					0x80001501	// 현재 상태가 완료 조건보다 부족합니다.
#define EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED			0x80001502	// 이미 보상 처리를 완료 하였습니다.
#define EVENT_ERROR_ATTENDANCE_WRONG_VERSION				0x80001503	// 출석체크 이벤트 버전이 다릅니다.
#define EVENT_ERROR_ATTENDANCE_SUCCESS						0x80001504	// 보상 지급, 출석이 성공 하였습니다.
#define EVENT_ERROR_ATTENDANCE_UNKWON						0x80001505	// 알수없는 이유로 실패하였습니다.

// 할인쿠폰 - 서동권 2011.11.11
#define EVENT_ERROR_INVALID_COUPON							0x80001600	// 존재하지 않는 쿠폰 입니다.
#define EVENT_ERROR_EXPIRY_DATE_COUPON						0x80001601	// 유효기간이 지난 쿠폰 입니다.
#define EVENT_ERROR_COUPON_DATA								0x80001602	// 잘못된 쿠폰 데이터이거나 쿠폰 사용 조건에 맞지 않습니다.

// 뽑기 상점 - 클라 - 서버 공통 에러코드
#define EVENT_ERROR_RS_LOAD_FAIL_RSINFO						0x80001700	// 뽑기상점 정보를 가져오지 못했습니다.
#define EVENT_ERROR_RS_START_FAIL_CASH_BALANCE				0x80001701	// 캐쉬 잔액이 부족하여 룰렛 시작이 실패 했습니다.
#define EVENT_ERROR_RS_START_FAIL_WRONG_ROULETTE			0x80001702	// 룰렛구성이 올바르지 못해 룰렛 시작이 실패 했습니다.
#define EVENT_ERROR_RS_FAIL_NETWORK							0x80001703	// 네트워크 오류 발생.
#define EVENT_ERROR_RS_START_FAIL_NOT_ENOUGH_ITEM			0x80001704	// 선택한 아이템의 갯수가 부족하여 룰렛 시작이 실패 했습니다.
#define EVENT_ERROR_RS_START_FAIL_UNKNOWN					0x80001705	// 알수없는 이유로 룰렛 시작이 실패 했습니다.
#define EVENT_ERROR_RS_START_FAIL_ALREADY_PLAY				0x80001706	// 이전 뽑기가 진행중입니다.

// 뽑기 상점 - 서버 테스트 용 에러코드
#define EVENT_ERROR_RS_START_FAIL_RS_ST_NULL				0x80001707	// RS_ROULETTE_INFO가 NULL 이다.
#define EVENT_ERROR_RS_START_FAIL_RS_INSERT_FALSE			0x80001708  // Insert() 결과값이 FALSE이다.
#define EVENT_ERROR_RS_START_FAIL_WRONG_PRIZE_GOODSID		0x80001709  // 뽑힌 GoodsID 가 잘못 지정될경우


// NEW 유저 찾기  에러코드
#define EVENT_ERROR_NEW_USER_FIND							0x80001800	
#define EVENT_ERROR_NEW_USER_FIND_MS_BUFFER_FULL			0x80001801	// Messenger 서버 버퍼가 풀이다.
#define EVENT_ERROR_NEW_USER_FIND_RUSER_WRONG				0x80001802	// 요청한 유저가 없다.
#define EVENT_ERROR_NEW_USER_FIND_FUSER_NOT_FIND_DB			0x80001803	// 찾고자 하는 유저를 DB에서 찾지못했다.
#define EVENT_ERROR_NEW_USER_FIND_FUSER_OFFLINE				0x80001804	// 찾고자 하는 유저가 오프라인이다.



// Block Mask										0x80FFFFFF
//	하위 20 bits는 bitflag 형식으로 전달됩니다. - praptor
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_MASK					0x000FFFFF	// 하위 20 bits Mask
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK			0x80F00000
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_INNER				0x80100000	// InnerBlock in GameBlock 
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_OUTER				0x80200000	// OuterBlock in GameBlock 
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_RESERVED				0x80400000	// 예약.
#define EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME					0x80800000	// GameBlock in PortalBlock

inline	bool	IsLogInBlockError(UINT32 Error)
{
	return 0 != ((Error & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK) & (~0x80000000 & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK));
}

//
#define EVENT_ERROR_EVENT_UNEXPECTED_VALUE					0x80002001


// [zion] 추가 메시지
#define EVENT_RECEIVE_CLAN_NOTE								0x80003001	// 클랜 전체 쪽지 도착
#define EVENT_ERROR_EVENT_FAIL_INVITED_USER					0x80003002	// 배틀룸에 초대된 유저가 초대 가능한 상태가 아님
#define EVENT_ERROR_EVENT_FAIL_INVITED_USER_IN_CLAN_MATCH	0x80003003	// 배틀룸에 초대된 유저가 클랜 서버에서 매치 매이킹 중이면 초대 가능한 상태가 아님

////////////////////////////////////////////////////////////////////////////////
#define INVENTORY_FLAG_CHARACTER		0x00000001
#define INVENTORY_FLAG_WEAPON			0x00000002
#define INVENTORY_FLAG_ITEM				0x00000004

#define CONVERT_STRING_COUNT			1024
////////////////////////////////////////////////////////////////////////////////

#define GAME_CHANNEL_COUNT			10
#define INCREASE_MEMBER_COUNT		50			// 추가 클랜원 수
#define MAX_MEMBER_COUNT			250			// 최대 클랜원 수
#define MAX_REQUEST_COUNT			100			// 최대 가입신청서 수

#define MAX_BATTLE_CLAN_ROOM_COUNT	250			// 각 클랜에서 만들수 있는 최대 전투방 개수

#define NET_CLAN_NAME_SIZE			17			// 클랜명
#define NET_CLAN_NOTICE_SIZE		255			// 클랜 공지
#define NET_CLAN_INTRO_SIZE			255			// 클랜 소개
#define NET_CLAN_REQUEST_SIZE		255			// 신청서 내용
#define NET_CLAN_AZIT_URL_SIZE		21			// 아지트 URL
#define NET_MD5_KEY_SIZE			32			// MD5 KEY SIZE

#define CLAN_TEAM_MEMBER_COUNT		8			// 클랜 매치메이킹 최대인원수
#define CLAN_BASIC_POINT			1000.f		// 최초 클랜 포인트
#define CLAN_MARK_DEFAULT			0xFFFFFFFF	// 클랜마크 default
// ERC End

// ACCOUNT_KICK_TYPE
#define ACCOUNT_KICK_LOGIN			0x01			// 중복로그인으로 인한 킥
#define ACCOUNT_KICK_GM_TOOL		0x02			// 운영툴에 의한 킥
#define ACCOUNT_KICK_OTHER			0xFF			// 그 외 다른 이유로 인한 킥

//Size 
//////////////////////////////////////////////////////////////////////////
// SDOA
#define NET_SNDA_ID					16
#define NET_SNDA_UNIQUE_ID			33
#define NET_CLIENT_IP				16
#define NET_CALL_TIME				20

#define MAX_ITEM_INFO				128
#define MAX_COST_INFO				128

#define MAX_SEQUENCEID				33
#define MAX_ORDERID					33
#define MAX_REDIRECTURL				2049			// 중국 빌링 인증 재문의 URL

#define MAX_BATCHID		             10

//////////////////////////////////////////////////////////////////////////
#define MAX_URL_SIZE				1024			// 중국 실명인증 URL
#define NET_ID_SIZE					65
#define NET_PW_SIZE					32
#define NET_SERVER_PW				12
#define NET_ROOM_PW					4				// null 종료 문자열 아님
#define NET_NICK_NAME_SIZE			33				// 한글/영문 16자 (note. 다른 언어일 경우 16자 아닐 수 있음.)
#define NET_ROOM_NAME_SIZE			23
#define NET_NCGUSID_SIZE			16
#define NET_SESSION_KEY_SIZE		257
#define NET_ENCRYPTED_TOKEN_SIZE	344				// Token 최대크기
#define INVEN_ITEMDATA_SIZE			33
#define TURKEY_UID_SIZE				21				//NULL 포함 
#define NORTH_UID_SIZE				21				//NULL 포함 
#define NORTH_CUSTOM_MSG_SIZE		128				//북미 커스텀 메세지 크기 (2011.06.10 양승천)

#define MACRO_MAX_COUNT				5
#define MACRO_MAX_LENGHT			255
#define	VIEW_ROOM_COUNT				300				//채널당 명수랑 같아야 합니다. 
#define	SEND_MAX_ROOM_COUNT			15				//몇개의 카운터는 정확해야한다.
#define	SEND_MAX_LOBBY_USER_COUNT	10				//게임서버에서 보내는 로비 유저 카운트
#define VIEW_CHANNEL_COUNT			10				//GAME_CHANNEL_COUNT와 뭐가 다른가요? (영권)
#define	SLOT_MAX_COUNT				16				//플레이어 슬롯입니다.
#define	AI_MAX_COUNT				32				// AI 최대 인원수
#define AI_GROUP_MAX_COUNT			AI_MAX_COUNT	// AI 그룹 최대 갯수
#define AI_GROUP_NAME_MAX_LENGTH	32				// AI 그룹명 최대 길이
#define AI_SCRIPT_MAX_COUNT			64				// AI Lua 스크립트 파일 배틀룸당 최대 로딩 가능 갯수
#define AI_MAX_HAVE_WEAPON_COUNT	6				// AI가 소유할수 있는 무기 최대 갯수
#define SLOT_DEDICATION_IDX			0xFF			//데디케이션 인덱스 
#define	SLOT4VS4_MAX_COUNT			8				// 4vs4플레이어 슬롯입니다.
#define	SLOT_MAX_OBSERVER_COUNT		4				// 옵저버 플레이어 슬롯입니다.
#define	SLOT_MAX_HIDDEN_OBSERVER_COUNT	1			// 히든 옵저버 플레이어 슬롯입니다.(GM만 사용가능)
#define	SLOT_ROOM_VIEW_COUNT		SLOT_MAX_COUNT + SLOT_MAX_OBSERVER_COUNT									// 히든슬롯을 뺀 슬롯의 전체 개수
#define	SLOT_ALL_COUNT				SLOT_MAX_COUNT + SLOT_MAX_OBSERVER_COUNT + SLOT_MAX_HIDDEN_OBSERVER_COUNT	// 슬롯의 전체 개수
#define	CHARA_ALL_COUNT				USER_SLOT_COUNT + AI_MAX_COUNT												// 슬롯의 전체 개수 + AI
#define BEGIN3_CHANNEL_CONDITION_TIME	36000		// 배틀시간으로 제한하는 초보채널3의 입장가능기준(이하) 플레이 초

#define MISSION_POINT_MAX_COUNT		6				// 미션 보너스 포인트가 지급되는 최대 회수의 기준 단위 개수 (5, 10, 15, 20, 25, 30)

#define MAX_NAVIMESH_COUNT			8192			// 메시데이터가 가장 많은 스테이지의 메시데이터 갯수
#define	MAX_WAYPATH_COUNT			512				// 길찾기를 통해 얻은 메시이동경로의 최대갯수

#define MAX_AI_CHARA_COUNT			8
#define MAX_KILLUSER_COUNT			16				// 최대 킬 카운터

#define NEW_MEDAL_FLAG_SIZE			32				// NEW표시를 위한 저장소

#define GM_COMMENT_SIZE				255				// GM 명령시 첨부하는 Comment 길이

#define MAX_GIFT_MESSAGE_SIZE		126					// 선물 메시지 길이
#define MAX_USER_NICK				33					// 유저 닉네임 길이
#define MAX_SHOP_USER_GIFT_COUNT	100					// 선물받은 아이템 카운트

// 방어 미션 오브젝트 개수
enum MISSION_OBJECT_COUNT
{
	MISSION_FIRST_DEFENCE_OBJECT = 0,	// 첫번째 오브젝트
	MISSION_SECOND_DEFENCE_OBJECT,		// 두번째 오브젝트

	MAX_OBJECT_COUNT					// 최대 오브젝트의 수
};

////////////////////////////////////////////////////////////////////////////////////////////
// 옵션 설정값
#define ENV_SETTING_DEFAULT			1				// 기본 옵션입니다.
#define ENV_SETTING_CHANGE			0				// 옵션이 변경 되었습니다.


//미션 라운드 엔드 타입 
enum MISSION_END_TYPE
{
	MISSION_END_TYPE_NONE			= 0,
	MISSION_END_TIMEOUT				= 1,
	MISSION_END_BOMBFIRE			= 2,
	MISSION_END_UNINSTALL			= 3,
	MISSION_END_ALLDEATH			= 4,	
	MISSION_END_GENERATOR_DESTROY	= 5,			// 파괴 미션 오브젝트 파괴
	MISSION_END_DEFENCE_DESTROY		= 6,			// 방어 미션 오브젝트 파괴
	MISSION_END_TUTORIAL_COMPLETE	= 7,			// 튜토리얼 임무 완료
	MISSION_END_MAX_KILL			= 8,			// 데스매치에서 목표 킬카운트 달성

	MISSION_END_TYPE_MAX
};


// 제압미션관련	2011.07.18	안국정
// 2011.07.20 추가

enum DOMINATION_USE_SP_KIND
{
	DOMINATION_USE_SP_FOR_RECOVERY_HP		= 7,
	DOMINATION_USE_SP_FOR_AMMO_UP			= 8,
	DOMINATION_USE_SP_FOR_ATTACK_LEVEL_UP	= 9,
	DOMINATION_USE_SP_FOR_DEFENSE_LEVEL_UP	= 0,
};

// 공격력 레벨업시 소비하는 SP
enum REQUITE_SP_FOR_ATTACK_LEVEL_UP
{
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_0_TO_1	=	200,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_1_TO_2	=	400,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_2_TO_3	=	700,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_3_TO_4	=	1000,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_4_TO_5	=	1400,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_5_TO_6	=	2000,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_6_TO_7	=	2500,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_7_TO_8	=	3500,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_8_TO_9	=	5000,
	REQUIRE_SP_FOR_ATTACK_LEVEL_UP_9_TO_MAX	=	7000,
};
// 방어력 레벨업시 소비하는 SP
enum REQUITE_SP_FOR_DEFENSE_LEVEL_UP
{
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_0_TO_1	=	200,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_1_TO_2	=	400,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_2_TO_3	=	700,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_3_TO_4	=	1000,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_4_TO_5	=	1400,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_5_TO_6	=	2000,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_6_TO_7	=	2500,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_7_TO_8	=	3500,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_8_TO_9	=	5000,
	REQUIRE_SP_FOR_DEFENSE_LEVEL_UP_9_TO_MAX=	7000,
};

/*********//*********//*********//*********//*********/
#define V_STAGEOPTION				UINT16			// 옵션이 더 늘어나게 되면 혹시나 몰라서..

enum STAGE_OPTION_TYPE
{
	STAGE_OPTION_NONE			= 0,
	STAGE_OPTION_AI				= 0x0001,
	STAGE_OPTION_RPG			= 0x0002,
	STAGE_OPTION_SNIPER			= 0x0004,
	STAGE_OPTION_ANNIHILATION	= 0x0008,
	STAGE_OPTION_SUDDENDEATH	= 0x0010,
	STAGE_OPTION_HEADHUNTER		= 0x0020,
	STAGE_OPTION_HEADKILLER		= 0x0040,
	STAGE_OPTION_SHOTGUN		= 0x0080,
	STAGE_OPTION_KNUCKLE		= 0x0100,
	STAGE_OPTION_TUTORIAL		= 0x0200,
};
#define MAX_OPTION_COUNT	10	//(STAGE_OPTION_NONE 제외 수)
//MAX_OPTION_COUNT 도 같이 적용해야 한다.
/*********//*********//*********//*********//*********/



// Max String Count
#define		MAX_STRING_COUNT	256
#define		MAX_CHATTING_COUNT	256

//변경하지 마세요.. 로그에서 사용합니다. 
#define		CHATTING_TYPE_RADIO					0		//InGame에서만 사용
#define		CHATTING_TYPE_ALL					1		//Room에서만 사용
#define		CHATTING_TYPE_WHISPER				2
#define		CHATTING_TYPE_REPLY					3	
#define		CHATTING_TYPE_TEAM					4		//Room에서만 사용
#define		CHATTING_TYPE_CLAN					5
#define		CHATTING_TYPE_LOBBY					6
#define		CHATTING_TYPE_MATCH					7
#define		CHATTING_TYPE_CLAN_MEMBER_PAGE		8
#define		CHATTING_TYPE_COMMENT				100		// Client에서 Comment를 쓰는경우 사용

//Mission 
#define MISSION_PREBATTLE_TIME		5			//5초 시작전 대기타임
#define MISSION_FREE_TIME			5			//5초 프리타임
#define MISSION_RESULT_TIME			3			//3초 결과타임

// 게임 설정 - 주: 팀, 데스메치
#define BATTLE_KILL_TYPE_60			0x00
#define BATTLE_KILL_TYPE_80			0x01
#define BATTLE_KILL_TYPE_100		0x02
#define BATTLE_KILL_TYPE_120		0x03
#define BATTLE_KILL_TYPE_140		0x04
#define BATTLE_KILL_TYPE_160		0x05

// 게임 설정 - 주: 미션모드에서 사용합니다.
enum BATTLE_ROUND_TYPE
{
	BATTLE_ROUND_TYPE_1	= 0,
	BATTLE_ROUND_TYPE_2,
	BATTLE_ROUND_TYPE_3,
	BATTLE_ROUND_TYPE_5,
	BATTLE_ROUND_TYPE_7,
	BATTLE_ROUND_TYPE_9,

	BATTLE_ROUND_TYPE_ENDLESS,

	BATTLE_ROUND_TYPE_MAX,
};

// 게임 설정 - 부
#define BATTLE_TIME_MISSION_TYPE_3	0x00
#define BATTLE_TIME_MISSION_TYPE_5	0x10
#define BATTLE_TIME_MISSION_TYPE_7	0x20

#define BATTLE_TIME_TYPE_3			0x00
#define BATTLE_TIME_TYPE_5			0x30
#define BATTLE_TIME_TYPE_10			0x40
#define BATTLE_TIME_TYPE_15			0x50
#define BATTLE_TIME_TYPE_20			0x60
#define BATTLE_TIME_TYPE_25			0x70
#define BATTLE_TIME_TYPE_30			0x80
#define BATTLE_TIME_TYPE_20s		0xA0

#define MAX_NPC_COUNT				16

#define GET_QUEST_EXP_POINT			0x00000001
#define GET_QUEST_WEAPON			0x00000010
#define GET_QUEST_FINISH_LAST_CARD	0x00000100

#define RESULT_ICON_PC				0x0001
#define RESULT_ICON_PC_PLUS			0x0002
#define RESULT_ICON_ITEM			0x0004
#define RESULT_ICON_EVENT			0x0008
#define RESULT_ICON_PC_ID			0x0010
#define RESULT_ICON_GWARNET			0x0020	//인도네시아 GWarnet 마크

// 훈련전 난이도
#define TRAINING_DIFFICULTY_LEVEL_NONE		0
#define TRAINING_DIFFICULTY_LEVEL_HIGH		10
#define TRAINING_DIFFICULTY_LEVEL_LOW		1

#define GET_POINTITEM(ItemID)			((GET_ITEM_FLAG_USAGE(ItemID)*1000000)+(GET_ITEM_FLAG_NUMBER(ItemID)*1000)+(GET_ITEM_FLAG_TYPE(ItemID)*100))

#define GET_KILLTYPE_HIGH(t)			(t&0xF0)
#define SET_KILLTYPE_HIGH(t,bit)		(t|(bit&0xF0))

#define GET_KILLTYPE_LOW(t)				(t&0x0F)
#define SET_KILLTYPE_LOW(t,Idx)			((t&0xF0)|(Idx&0x0F))


/*	서버 추가
	1.	SChannelType 요기에 추가한다.
	2.	LPCTSTR TabList[S_CHANNEL_TYPE_MAX] 에 ConfigEx.pef 파일에서 읽을 스트링을 적는다.
	3.	GetServerTabTypeText( SChannelType TabType ), GetServerTypeText( SChannelType ServerType )
		Excel 스트링을 등록한다.
*/
enum SChannelType
{
	S_CHANNEL_TYPE_UNDEFINED	= -1,

	S_CHANNEL_TYPE_ALL		= 0,	//전체
	S_CHANNEL_TYPE_NORMAL	= 1,	//일반 
	S_CHANNEL_TYPE_NORMAL2,			//일반 2
	S_CHANNEL_TYPE_BEGIN,			//초보 
	S_CHANNEL_TYPE_CLAN,			//클랜
	S_CHANNEL_TYPE_EXPERT,			//고수 
	S_CHANNEL_TYPE_CONBINATION,		//조합	
	S_CHANNEL_TYPE_CHAMPIONSHIP,	//대회
	S_CHANNEL_TYPE_NATIONAL,		//중남미 국가
	S_CHANNEL_TYPE_PCCAFE_SILVER,	//인니 피씨방 일반
	S_CHANNEL_TYPE_PCCAFE_GOLD,		//인니 피씨방 골드 
	S_CHANNEL_TYPE_PCCAFE_CLAN,		//인니 피씨방 클랜 서버
	
	S_CHANNEL_TYPE_MAX,
};


/*	채널 추가
	1.	ChannelType 요기에 추가한다.
	2.	SERVERSELECT_MAINTYPE 요기에 추가한다.
	3.	SCript/String/korea/ServerSelect.txt 파일에 채널 스트링을 등록한다.
	4.2번에서 등록 된 내용을 _LoadServerTextFromFile() 읽을 수 있도록 수정
	5. GetCurrentSeverInfo	:	로비방 채널 제목
		_SetServerEnv	:	채널 안 슬롯
		GetCategoryText	:	채널 제목
*/
enum ChannelType
{
	CHANNEL_TYPE_NONE = 0,
	CHANNEL_TYPE_NORMAL,			//일반 채널
	CHANNEL_TYPE_BEGIN1,			//초보 채널 Kill/Death로 제한
	CHANNEL_TYPE_BEGIN2,			//초보 채널 계급으로 제한
	CHANNEL_TYPE_CLAN,				//클랜 채널
	CHANNEL_TYPE_EXPERT,			//고수 채널 병장 이상 
	CHANNEL_TYPE_EXPERT2,			//고수 채널 상사 이상
	CHANNEL_TYPE_CHAMPIONSHIP,		//대회 채널
	CHANNEL_TYPE_NORMAL2,			//일반 채널 2
	CHANNEL_TYPE_NATIONAL,			//중남미 국가 채널
	CHANNEL_TYPE_MIDDLE,			//중간 채널
	CHANNEL_TYPE_PCCAFE_SILVER,		//인니 피씨방 일반
	CHANNEL_TYPE_PCCAFE_GOLD,		//인니 피씨방 골드 
	CHANNEL_TYPE_PCCAFE_CLAN,		//인니 피씨방 클랜 채널
	
	CHANNEL_TYPE_MAX,
};

enum TEAM_TYPE
{
	TEAM_RED = 0, 
	TEAM_BLUE,
	TEAM_COUNT,
};

enum TEAM_RESULT_TYPE
{
	TEAM_RED_WIN = 0, 
	TEAM_BLUE_WIN,
	TEAM_DRAW
};

TEAM_TYPE	SlotIndexToTeamType(INT32 Index);

//KILLING MESSAGE
#define KILLING_MESSAGE_PIERCINGSHOT		0x0100
#define KILLING_MESSAGE_MASSKILL			0x0200
#define KILLING_MESSAGE_CHAINSTOPPER		0x0400
#define KILLING_MESSAGE_HEADSHOT			0x0800
#define KILLING_MESSAGE_CHAINHEADSHOT		0x1000
#define KILLING_MESSAGE_CHAINSLUGGER		0x2000
#define KILLING_MESSAGE_ASSISTKILL			0x4000				
#define KILLING_MESSAGE_OBJECTDEFENCE		0x8000
#define KILLING_MESSAGE_INTERCEPT			0x10000


// 초대하기 목록
#define MAX_INVITE_COUNT			8			// 5명

//공지 
#define NOTICE_TYPE_NORMAL			1
#define NOTICE_TYPE_EMERGENCY		2
#define NOTICE_STRING_MAX_COUNT		1024

// PCCafe
enum PC_CAFE_TYPE
{
	PC_NOT_CAFE = 0,				// PC방 아님
	PC_NORMAL_CAFE,					// 일반 PC방
	PC_PREMIUM_CAFE,				// 고급 PC방	
	PC_VACATION_EVENT_ITEM,			// 방학 이벤트 아이템 착용
	PC_INDO_TELCOM_ITEM,			// 인니 Telcom 이벤트
	PC_INDO_SILVER,					// 인니 실버 피시방( 인니의 경우 별도의 PC 방 연동을 진행하여 따로 구분하였습니다. )
	PC_INDO_GOLD,					// 인니 골드 피시방( 인니의 경우 별도의 PC 방 연동을 진행하여 따로 구분하였습니다. )

	PC_MAX_COUNT					
};

enum CHAT_PASING_TYPE
{
	CHAT_PASING_TYPE_NONE = -1,

	CHAT_PASING_TYPE_TEAM1 = 0,
	CHAT_PASING_TYPE_TEAM2,
	CHAT_PASING_TYPE_TEAM3,
	CHAT_PASING_TYPE_TEAM4,

	STR_TBL_CHAT_HOTKEY_ERASE1,
	STR_TBL_CHAT_HOTKEY_ERASE2,
	STR_TBL_CHAT_HOTKEY_ERASE3,
	STR_TBL_CHAT_HOTKEY_ERASE4,

	STR_TBL_CHAT_HOTKEY_HELP,
	STR_TBL_CHAT_HOTKEY_HELP2,

	STR_TBL_CHAT_HOTKEY_WHISPER1,
	STR_TBL_CHAT_HOTKEY_WHISPER2,
	STR_TBL_CHAT_HOTKEY_WHISPER3,
	STR_TBL_CHAT_HOTKEY_WHISPER4,

	STR_TBL_CHAT_HOTKEY_REPLY1,
	STR_TBL_CHAT_HOTKEY_REPLY2,
	STR_TBL_CHAT_HOTKEY_REPLY3,
	STR_TBL_CHAT_HOTKEY_REPLY4,

	CHAT_PASING_TYPE_COUNT,
};

enum BATTLE_SERVER_TYPE
{
	BATTLE_SERVER_TYPE_NONE			= 0,
	BATTLE_SERVER_TYPE_P2P			= 1,
	BATTLE_SERVER_TYPE_RELAY		= 2,
	BATTLE_SERVER_TYPE_DEDICATION	= 3,

	BATTLE_SERVER_TYPE_COUNT,	
};

// ESportID
enum ESPORT_ID_TYPE
{
	ESPORT_NOT_ID = 0,				// ESport ID 아님
	ESPORT_ID,						// ESport ID
};

enum ESPORT_ID_LEVEL
{
	ESPORT_ID_LEVEL_ZERO = 0,				// ESport ID 아님
	ESPORT_ID_LEVEL_BEGINNER1,				// ESport ID
	ESPORT_ID_LEVEL_BEGINNER2,
	ESPORT_ID_LEVEL_BEGINNER3,
	ESPORT_ID_LEVEL_SUBSTITUTE1,
	ESPORT_ID_LEVEL_SUBSTITUTE2,
	ESPORT_ID_LEVEL_STARTER1,
	ESPORT_ID_LEVEL_STARTER2,
	ESPORT_ID_LEVEL_STAR1,
	ESPORT_ID_LEVEL_STAR2,
	ESPORT_ID_LEVEL_STAR3,
	ESPORT_ID_LEVEL_SUPERSTAR1,
	ESPORT_ID_LEVEL_SUPERSTAR2,
	ESPORT_ID_LEVEL_SUPERSTAR3,
	ESPORT_ID_LEVEL_SUPERSTAR4,
	ESPORT_ID_LEVEL_WORLDCLASS1,
	ESPORT_ID_LEVEL_WORLDCLASS2,
	ESPORT_ID_LEVEL_WORLDCLASS3,
	ESPORT_ID_LEVEL_LEGEND1,
	ESPORT_ID_LEVEL_LEGEND2,
	ESPORT_ID_LEVEL_HALLOFFAME,

};

enum VACATION_EVENT_ITEM_LEVEL
{
	EVENT_ITEM_LEVEL_ZERO,
	EVENT_ITEM_LEVEL_ONE,

	EVENT_ITEM_LEVEL_MAX
};


// 쿠폰정보
typedef struct CouponInfo
{
	INT32 _count;
	INT32 _point;
} COUPON_INFO;

//////////////////////////////////////////////////////////////////////////////////////////
//											ENUM										//
//////////////////////////////////////////////////////////////////////////////////////////
// 게임안에서 사용되는 Key Mapping에 사용됩니다.
enum GAME_KEY_MAP
{
	GAME_KEY_MAP_VERSION = 0,

	// 이동
	GAME_KEY_MAP_LEFT,
	GAME_KEY_MAP_RIGHT,
	GAME_KEY_MAP_FORWARD,
	GAME_KEY_MAP_BACKWARD,
	GAME_KEY_MAP_JUMP,									// 점프 키
	GAME_KEY_MAP_FOOTSTEP,								// 걷기
	GAME_KEY_MAP_CROUCH,								// 앉기
	GAME_KEY_MAP_TURNBACK,								// Extension 활성화 버튼

	// 무기사용
	GAME_KEY_MAP_ATTACK,								// 무기 공격
	GAME_KEY_MAP_SUBFUNC,								// 특수기능
	GAME_KEY_MAP_RELOAD,								// 탄환 재장전

	// 무기교체
	GAME_KEY_MAP_CHANGEWEAPON,							// 무기교체창 활성화
	GAME_KEY_MAP_CHANGEWEAPON_1,						// 주무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_2,						// 보조무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_3,						// 근접무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_4,						// 투척무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_5,						// 특수무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_6,						// 폭발물로 교체
	GAME_KEY_MAP_CHANGEWEAPON_QUICK,					// 지난무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_PREV,						// 이전무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_NEXT,						// 다음무기로 교체
	GAME_KEY_MAP_DROP,									// 무기 드랍 버튼	
	
	// 기타0
	GAME_KEY_MAP_SHOWSCORE,								//	현재 게임전적 보기
	GAME_KEY_MAP_TOGGLE_MAP,							// 전체맵 보기
	GAME_KEY_MAP_MINIMAP_ENLARGE,						// 미니맵 확대
	GAME_KEY_MAP_MINIMAP_REDUCTION,						// 미니맵 축소
	GAME_KEY_MAP_CHAT,									// 채팅
	GAME_KEY_MAP_CHAT_ALL,								// 전체채팅모드
	GAME_KEY_MAP_CHAT_TEAM,								// 팀 채팅모드
	GAME_KEY_MAP_CHAT_HISTORY,							// 지난 대화보기(게임안에서만 사용됨)
	GAME_KEY_MAP_VOICECHAT,								// 음성채팅
	GAME_KEY_MAP_RADIOMESSAGE1,							// 라디오 메시지 1
	GAME_KEY_MAP_RADIOMESSAGE2,							
	GAME_KEY_MAP_RADIOMESSAGE3,
	GAME_KEY_MAP_SPECIALACTION,							// 특수 행동키(폭발물 해체등)
	GAME_KEY_MAP_MOUSE_SENS_UP,							// 마우스 감도 증가
	GAME_KEY_MAP_MOUSE_SENS_DOWN,						// 마우스 감도 감소
	GAME_KEY_MAP_SCREENSHOT,							// 화면저장	

	GAME_KEY_MAP_CHAT_CLAN,								// 클랜 채팅모드

	// 08년 0901 추가
	GAME_KEY_MAP_WEAPON_EXTENSION,								// 레이져 도트
	
	GAME_KEY_MAP_VIDEOCAPTURE,							// 비디오 캡쳐
	GAME_KEY_MAP_INGAME_SHOP,							// 야전 상점
	GAME_KEY_MAP_INGAME_REPORT,							// 신고

	GAME_KEY_MAP_COUNT,
};

//순서 중요
enum SLOT_STATE
{
	SLOT_STATE_EMPTY = 0,			// 
	SLOT_STATE_CLOSE,				// 	
	SLOT_STATE_SHOP,				// 상점
	SLOT_STATE_INFO,				// 유저정보
	SLOT_STATE_CLAN,				// 클랜
	SLOT_STATE_INVENTORY,			// 인벤토리
	SLOT_STATE_RS,				// 뽑기상점.
	SLOT_STATE_NORMAL,				// 레디룸에서 상태
	SLOT_STATE_READY,				// 레디룸에서 상태
	SLOT_STATE_LOAD,				// (게임중) 로딩중입니다.			Time Out ( 2분)     처음시작 방장시작후 (10초)
	SLOT_STATE_RENDEZVOUS,			// (게임중) 랑데뷰서버 홀펀칭		Time Out ( )
	SLOT_STATE_PRESTART,			// (게임중) 홀펀칭					Time Out ( )
	SLOT_STATE_BATTLE_READY,		// (게임중) 홀펀칭 완료후 게임 시작을 기다림 
	SLOT_STATE_BATTLE,				// (게임중) 전투중입니다.
	
};

//순서 중요
enum ROOM_STATE
{	
	ROOM_STATE_READY = 0,			//ReadyRoom 상태 
	ROOM_STATE_COUNTDOWN,			//start countdown
	ROOM_STATE_LOADING,				//로딩 상태
	ROOM_STATE_RENDEZVOUS,			//랑데뷰 서버에 접속준비 중인 상태
	ROOM_STATE_PRE_BATTLE,			//로딩 이 끝나고, 홀펀칭 상태 
	ROOM_STATE_BATTLE,				//홀펀칭이 끝나고 게임을 시작하는 상태 
	ROOM_STATE_BATTLE_RESULT,		//배틀 종료, 결과 창 출력 
	ROOM_STATE_BATTLE_END,			//배틀  종료  바로 Ready로 변경됨 
	ROOM_STATE_EMPTY,				//깨진방: Lobby의 Room 목록에서 AutoRefresh 시, 이 빠진 방 구분 용. Lobby에서만 사용.	
};

	//ROOM_STATE_ROUND_PRE,				//홀펀칭이 끝나고 게임을 시작하는 상태
	//ROOM_STATE_ROUND_BATTLE,				//홀펀칭이 끝나고 게임을 시작하는 상태
	//ROOM_STATE_ROUND_FREE,				//홀펀칭이 끝나고 게임을 시작하는 상태
	//ROOM_STATE_ROUND_RESULT,				//홀펀칭이 끝나고 게임을 시작하는 상태

enum MISSION_STATE
{
	MISSION_STATE_UNKNOWN = 0,		//
	MISSION_STATE_PRE_BATTLE,		//보스전으로 들어갔습니다. 모든 미션전에서 사용합니다. 
	MISSION_STATE_BATTLE,			//
	MISSION_STATE_FREE,				//
	MISSION_STATE_RESULT,			//
};

///////////////////////////////////////////////////////////////////////////////////
//
enum STAGE_MASK
{
	STAGE_MASK_NORMAL			= 0,
	STAGE_MASK_TRAINING,

	STAGE_MASK_MAX
};

///////////////////////////////////////////////////////////////////////////////////

// Stage Type (Bitpattern)
 enum STAGE_TYPE
{
	STAGE_TYPE_NA				= 0,
	STAGE_TYPE_DEATHMATCH,
	STAGE_TYPE_BOMB,
	STAGE_TYPE_DESTROY,	
	STAGE_TYPE_ANNIHILATION,
	STAGE_TYPE_DEFENCE,
	STAGE_TYPE_SUDDEN_DEATH,
	STAGE_TYPE_ESCAPE,
	STAGE_TYPE_HEAD_HUNTER,
	STAGE_TYPE_HEAD_KILLER,
	STAGE_TYPE_TUTORIAL,
	STAGE_TYPE_DOMINATION,
	STAGE_TYPE_CROSSCOUNT,

	STAGE_TYPE_MAX
};

 enum MODE_RECORD
 {
	 MODE_DEATHMATCH = 0,
	 MODE_ANNIHILATION,
	 MODE_BOMB,
	 MODE_ETC,
	 MODE_RECORD_MAX
 };

enum STAGE_MODE
{
	STAGE_MODE_NORMAL			= 0,
	STAGE_MODE_DEATHMATCH,
	STAGE_MODE_BOMB,
	STAGE_MODE_DESTROY,
	STAGE_MODE_DEFENCE,
	STAGE_MODE_DINO,
	STAGE_MODE_TUTORIAL,
	STAGE_MODE_ANNIHILATION,
	STAGE_MODE_SUDDENDEATH,
	STAGE_MODE_HEADHUNTER,
	STAGE_MODE_HEADKILLER,
	STAGE_MODE_AI,
	STAGE_MODE_KNUCKLE,
	STAGE_MODE_SHOTGUN,
	STAGE_MODE_DOMINATION,
	STAGE_MODE_CLANMATCH,
	STAGE_MODE_ALL,

	STAGE_MODE_MAX
};

enum STAGE_SLOT_MODE
{
	STAGE_SLOT_MODE_NORMAL		= 0,
	STAGE_SLOT_MODE_4VS4,

	STAGE_SLOT_MODE_MAX
};

#define		STAGE_NAME_COUNT		64
#define		STAGE_CODE_NAME_COUNT	32

//KEYWORD : STAGE_ADD
//== 스테이지 추가시 필히 추가항목 ==라자== 각모드의 마지막에추가

//Stage 추가, 맵 추가
//설명 :	스테이지 Stgae Id를 추가한다. (순서 중요 - 서버도 사용)
//				스테이지에 대한 기본 인덱스
//				새로운 스테이지 추가 ->  맨 밑에 추가해야 됨.
enum STAGE_ORDINAL_ID
{
	STAGE_ORD_NA				= 0,

	STAGE_ORD_PORT_AKABA,				
	STAGE_ORD_RED_ROCK,
	STAGE_ORD_LIBRARY,
	STAGE_ORD_MSTATION,
	STAGE_ORD_MIDNIGHT_ZONE,
	STAGE_ORD_UPTOWN,
	STAGE_ORD_BURNINGHALL,
	STAGE_ORD_DSQUAD,
	STAGE_ORD_CRACKDOWN,
	STAGE_ORD_SAINTMANSION, 
	STAGE_ORD_EASTERNROAD,	
	STAGE_ORD_D_DOWNTOWN,
	STAGE_ORD_D_LUXVILLE,
	STAGE_ORD_D_BLOWCITY,
	STAGE_ORD_D_STORMTUBE,
	STAGE_ORD_D_GIRAN2,	
	STAGE_ORD_D_BREAKDOWN,
	STAGE_ORD_TRAININGCAMP,
	STAGE_ORD_D_SENTRYBASE,
	STAGE_ORD_DESERTCAMP,
	STAGE_ORD_KICKPOINT,
	STAGE_ORD_FACEROCK,
	STAGE_ORD_SUPPLYBASE,
	STAGE_ORD_D_SANDSTORM,
	STAGE_ORD_DOWNTOWN,					
	STAGE_ORD_LUXVILLE,
	STAGE_ORD_OUTPOST,
	STAGE_ORD_BLOWCITY,
	STAGE_ORD_STORMTUBE,
	STAGE_ORD_SENTRYBASE,
	STAGE_ORD_HOSPITAL,
	STAGE_ORD_DOWNTOWN2,
	STAGE_ORD_SHOPPINGCENTER,
	STAGE_ORD_SANDSTORM,
	STAGE_ORD_BREAKDOWN,				
	STAGE_ORD_GIRAN,
	STAGE_ORD_GIRAN2,
	STAGE_ORD_HELISPOT,
	STAGE_ORD_BLACKPANTHER,				
	STAGE_ORD_BREEDING_NEST,			
	STAGE_ORD_D_UPTOWN,
	STAGE_ORD_DINO_BREAKDOWN,
	STAGE_ORD_DEADTOWN,					
	STAGE_ORD_TUTORIAL,					
	STAGE_ORD_D_SHOPPINGCENTER,			// 데스 매치 - 쇼핑 센터 
	STAGE_ORD_D_SAFARI,
	STAGE_ORD_DRAGONALLEY,
	STAGE_ORD_MACHUPICHU,
	STAGE_ORD_CARGOSHIP,
	STAGE_ORD_TWOTOWERS,
	STAGE_ORD_C_CRACKDOWN,	
	STAGE_ORD_C_SAINTMANSION,
	STAGE_ORD_C_DSQUAD,
	STAGE_ORD_C_PORT_AKABA,
	STAGE_ORD_C_MSTATION,
	STAGE_ORD_C_TRAININGCAMP,
	STAGE_ORD_ANGKOR,
	STAGE_ORD_C_RED_ROCK,
	STAGE_ORD_C_SAFARI,
	STAGE_ORD_GHOSTTOWN,
	STAGE_ORD_GRANDBAZAAR,
	
	//새로운 맵이 추가될 경우 맨 아래에 추가 해야됨
	STAGE_ORD_MAX
};

// 호칭 적용 계열
enum DESIGNATION_APPLY_TYPE
{
	DESIGNATION_APPLY_TYPE_ALL = 0,		// 전체 계열
	DESIGNATION_APPLY_TYPE_ASSULT,		// 어설트라이플 계열
	DESIGNATION_APPLY_TYPE_RIFLE,		// 저격소총 계열
	DESIGNATION_APPLY_TYPE_SMG,			// SMG(기관단총)계열
	DESIGNATION_APPLY_TYPE_MELLE,		// 근접무기 계열
	DESIGNATION_APPLY_TYPE_HANDGUN,		// 핸드건 계열
	DESIGNATION_APPLY_TYPE_SHOTGUN,		// 샷건 계열
	DESIGNATION_APPLY_TYPE_BOMB,		// 폭탄 계열

	DESIGNATION_APPLY_TYPE_COUNT,
};

// 호칭 타입
enum DESIGNATION_TYPE
{
	DESIGNATION_TYPE_NONE = 0,
	DESIGNATION_TYPE_1,			// 최 하위 단계 공통 호칭 A
	DESIGNATION_TYPE_2,			// 최 하위 단계 공통 호칭 B
	DESIGNATION_TYPE_3,			// 최 하위 단계 공통 호칭 C
	DESIGNATION_TYPE_4,			// 최 하위 단계 공통 호칭 D

	DESIGNATION_TYPE_5,			// 1차 선택 단계 호칭 A
	DESIGNATION_TYPE_6,			// 1차 선택 단계 호칭 B
	DESIGNATION_TYPE_7,			// 1차 선택 단계 호칭 C

	DESIGNATION_TYPE_8,			// 2차 선택 단계 호칭 A (Assault Rifle)
	DESIGNATION_TYPE_9,			// 2차 선택 단계 호칭 B (Assault Rifle)
	DESIGNATION_TYPE_10,		// 2차 선택 단계 호칭 C (Assault Rifle)
	DESIGNATION_TYPE_11,		// 2차 선택 단계 호칭 D (Assault Rifle)
	DESIGNATION_TYPE_12,		// 마스터 (Assault Rifle)
	DESIGNATION_TYPE_13,		// 히어로 (Assault Rifle)

	DESIGNATION_TYPE_14,		// 2차 선택 단계 호칭 A (Rifle)
	DESIGNATION_TYPE_15,		// 2차 선택 단계 호칭 B (Rifle)
	DESIGNATION_TYPE_16,		// 2차 선택 단계 호칭 C (Rifle)
	DESIGNATION_TYPE_17,		// 2차 선택 단계 호칭 D (Rifle)
	DESIGNATION_TYPE_18,		// 마스터 (Rifle)
	DESIGNATION_TYPE_19,		// 히어로 (Rifle)

	DESIGNATION_TYPE_20,		// 2차 선택 단계 호칭 A (SMG)
	DESIGNATION_TYPE_21,		// 2차 선택 단계 호칭 B (SMG)
	DESIGNATION_TYPE_22,		// 2차 선택 단계 호칭 C (SMG)
	DESIGNATION_TYPE_23,		// 2차 선택 단계 호칭 D (SMG)
	DESIGNATION_TYPE_24,		// 마스터 (SMG)
	DESIGNATION_TYPE_25,		// 히어로 (SMG)

	DESIGNATION_TYPE_26,		// 2차 선택 단계 호칭 A (Knife)
	DESIGNATION_TYPE_27,		// 2차 선택 단계 호칭 B (Knife)
	DESIGNATION_TYPE_28,		// 2차 선택 단계 호칭 C (Knife)
	DESIGNATION_TYPE_29,		// 마스터 (Knife)

	DESIGNATION_TYPE_30,		// 2차 선택 단계 호칭 A (Hand Gun)
	DESIGNATION_TYPE_31,		// 2차 선택 단계 호칭 B (Hand Gun)
	DESIGNATION_TYPE_32,		// 2차 선택 단계 호칭 C (Hand Gun)
	DESIGNATION_TYPE_33,		// 마스터 (Hand Gun)
	DESIGNATION_TYPE_34,		// 히어로 (Hand Gun)

	DESIGNATION_TYPE_35,		// 2차 선택 단계 호칭 A (Shot Gun)
	DESIGNATION_TYPE_36,		// 2차 선택 단계 호칭 B (Shot Gun)
	DESIGNATION_TYPE_37,		// 2차 선택 단계 호칭 C (Shot Gun)
	DESIGNATION_TYPE_38,		// 마스터 (Shot Gun)
	DESIGNATION_TYPE_39,		// 히어로 (Shot Gun)

	DESIGNATION_TYPE_40,		// 2차 선택 단계 호칭 A (Composition)
	DESIGNATION_TYPE_41,		// 2차 선택 단계 호칭 B (Composition)
	DESIGNATION_TYPE_42,		// 2차 선택 단계 호칭 C (Composition)
	DESIGNATION_TYPE_43,		// 2차 선택 단계 호칭 D (Composition)
	DESIGNATION_TYPE_44,		// 마스터 (Composition)

	DESIGNATION_TYPE_MAX
};


enum SALE_ITEM_TYPE
{
	SALE_ITEM_DISGUISE_RANK		=		0x01,
	SALE_ITEM_DISGUISE_NAME		=		0x02,
	SALE_ITEM_COLOR_CROSSHAIR	=		0x04,
	SALE_ITEM_COLOR_CHATTING	=		0x08,
	SALE_ITEM_COLOR_NICK		=		0x10,
		 
	SALE_ITEM_MAX_COUNT			=		5,
};

//#define CASHITEM_USAGE_DATA_ENABLE				1
//#define CASHITEM_USAGE_DATA_DISABLE				0

enum STAGE_MASK_TYPE
{
	STAGE_MASK_TYPE_NONE		= 0,
	STAGE_MASK_TYPE_KNUCKLE,
	STAGE_MASK_TYPE_ESCAPE,
	STAGE_MASK_TYPE_SHOTGUN,
	STAGE_MASK_TYPE_SNIPER,
	STAGE_MASK_TYPE_CLANMATCH,
	STAGE_MASK_TYPE_TRAINING,
	STAGE_MASK_TYPE_HEADHUNTER,
};

// 무기제한 
#define	LOCK_KNUCKLE				0x80
#define LOCK_SHOTGUN				0x40
#define LOCK_SNIPER					0x20
#define LOCK_RPG7					0x10
#define LOCK_PRIMARY				0x08
#define LOCK_SECONDARY				0x04
#define LOCK_MELEE					0x02
#define LOCK_THROW					0x01
#define LOCK_BASIC					0x0F
/* StageId bit-field

   3                   2                   1                   0
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|P| MissionType |    SlotType   |   MatchType   |   MapIndex    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	PriviteFlag - private
		0     일반방
		1     비밀방

    MissionType - type
	    00    사용안함
		01    단체전
		02    폭파미션
		03    파괴미션
		04    섬멸전

	SlotType - slot mode
	    00    8대8
		01    4대4 

	MatchType - mode
	    00    일반 전투
		01    클랜전
		02    예약됨 (훈련소)

	MapIndex - ord
	    00    사용안함
	    01-FF 맵 인덱스

*/

// 20081226 - StageID 에 Mode 가 제거 되었습니다. - 김현우
//#define MAKE_STAGEID(type,slot,mode,ord)	((type & 0x7F) << 24 | (slot & 0xFF) << 16 | (mode & 0xFF) << 8 | (ord & 0xFF))
#define MAKE_STAGEID(type,slot,ord)				((type & 0x7F) << 24 | (slot & 0xFF) << 16 | (ord & 0xFF))

#define GET_STAGETYPE(stageId)					((STAGE_TYPE)(stageId >> 24 & 0x7F))
#define GET_STAGESLOT(stageId)					(stageId >> 16 & 0xFF)
#define GET_STAGEORD(stageId)					((STAGE_ORDINAL_ID)(stageId & 0xFF))

#define SET_STAGETYPE(stageId,type)				((stageId & 0x80FFFFFF) | (type & 0x7F) << 24)
#define SET_STAGESLOT(stageId,slot)				((stageId & 0xFF00FFFF) | (slot & 0xFF) << 16)
#define SET_STAGEORD(stageId,ord)				((stageId & 0xFFFFFF00) | (ord & 0xFF))

//KEYWORD : STAGE_ADD
//== 스테이지 추가시 필히 추가항목 ==라자== 각모드의 마지막에추가

#define		DEF_RANK_COUNT		52

enum RANK_COUNT
{
	RANK_00	=	0,	//	"훈련병"
	RANK_01	,		//	"이병"
	RANK_02,		//	"일병"
	RANK_03,		//	"상병"
	RANK_04,		//	"병장"
	RANK_05,		//	"하사 1호봉"
	RANK_06,		//	"하사 2호봉"
	RANK_07,		//	"하사 3호봉"
	RANK_08,		//	"중사 1호봉"
	RANK_09,		//	"중사 2호봉"
	RANK_10,		//	"중사 3호봉"
	RANK_11,		//	"중사 4호봉"
	RANK_12,		//	"상사 1호봉"
	RANK_13,		//	"상사 2호봉"
	RANK_14,		//	"상사 3호봉"
	RANK_15,		//	"상사 4호봉"
	RANK_16,		//	"상사 5호봉"
	RANK_17,		//	"소위 1호봉"
	RANK_18,		//	"소위 2호봉"
	RANK_19,		//	"소위 3호봉"
	RANK_20,		//	"소위 4호봉"
	RANK_21,		//	"중위 1호봉"
	RANK_22,		//	"중위 2호봉"
	RANK_23,		//	"중위 3호봉"
	RANK_24,		//	"중위 4호봉"
	RANK_25,		//	"중위 5호봉"
	RANK_26,		//	"대위 1호봉"
	RANK_27,		//	"대위 2호봉"
	RANK_28,		//	"대위 3호봉"
	RANK_29,		//	"대위 4호봉"
	RANK_30,		//	"대위 5호봉"
	RANK_31,		//	"소령 1호봉"
	RANK_32,		//	"소령 2호봉"
	RANK_33,		//	"소령 3호봉"
	RANK_34,		//	"소령 4호봉"
	RANK_35,		//	"소령 5호봉"
	RANK_36,		//	"중령 1호봉"
	RANK_37,		//	"중령 2호봉"
	RANK_38,		//	"중령 3호봉"
	RANK_39,		//	"중령 4호봉"
	RANK_40,		//	"중령 5호봉"
	RANK_41,		//	"대령 1호봉"
	RANK_42,		//	"대령 2호봉"
	RANK_43,		//	"대령 3호봉"
	RANK_44,		//	"대령 4호봉"
	RANK_45,		//	"대령 5호봉"
	RANK_46,		//	"준장"
	RANK_47,		//	"소장"
	RANK_48,		//	"중장"
	RANK_49,		//	"대장"
	RANK_50,		//	"원수"
	RANK_51,		//	"영웅"	
	RANK_NEWBIE		,	// 클라만 사용합니다.
	RANK_GAMEMASTER	,
	RANK_MOD,		// MOD권한 (터키 한정)
	RANK_MAX,
};

//KEYWORD : STAGE_ADD
//스테이지 추가시 필히 추가항목 ==라자== 각모드의 마지막에 추가

//Stage 추가, 맵 추가
//설명 :	스테이지 Stgae Id를 추가한다. (순서 안 중요)
//				스테이지에 대한 정보를 담고 있다.
enum STAGE_ID
{
	STAGE_ID_NA							= 0,

	/////////////////////////	 팀 데스
	STAGE_ID_PORT_AKABA					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_PORT_AKABA),
	STAGE_ID_RED_ROCK					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_RED_ROCK),
	STAGE_ID_LIBRARY					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_LIBRARY),
	STAGE_ID_MSTATION					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_MSTATION),
	STAGE_ID_MIDNIGHT_ZONE				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_MIDNIGHT_ZONE),
	STAGE_ID_UPTOWN						= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_UPTOWN),
	STAGE_ID_BURNINGHALL				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_BURNINGHALL),
	STAGE_ID_DSQUAD						= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DSQUAD),
	STAGE_ID_EASTERNROAD				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_EASTERNROAD),
	STAGE_ID_D_DOWNTOWN					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_DOWNTOWN),
	STAGE_ID_D_LUXVILLE					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_LUXVILLE),	
	STAGE_ID_D_BLOWCITY					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_BLOWCITY),
	STAGE_ID_D_STORMTUBE				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_STORMTUBE),
	STAGE_ID_D_GIRAN2					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_GIRAN2),
	STAGE_ID_D_BREAKDOWN				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_BREAKDOWN),
	//STAGE_ID_OUTPOST					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_OUTPOST),	
	STAGE_ID_TRAININGCAMP				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_TRAININGCAMP),
	STAGE_ID_D_SENTRYBASE				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_SENTRYBASE),
	STAGE_ID_DESERTCAMP					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DESERTCAMP),
	STAGE_ID_KICKPOINT					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_KICKPOINT),
	STAGE_ID_FACEROCK					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_FACEROCK),
	STAGE_ID_SUPPLYBASE					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_SUPPLYBASE),
	STAGE_ID_D_SANDSTORM				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_SANDSTORM),
    STAGE_ID_D_SHOPPINGCENTER			= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_SHOPPINGCENTER),
	STAGE_ID_D_SAFARI					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_SAFARI),
	STAGE_ID_DRAGONALLEY				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DRAGONALLEY),	
	STAGE_ID_MACHUPICHU					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_MACHUPICHU),	
	STAGE_ID_TWOTOWERS					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_TWOTOWERS),	
	STAGE_ID_ANGKOR						= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_ANGKOR),	
	STAGE_ID_GHOSTTOWN					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_GHOSTTOWN),	
	STAGE_ID_GRANDBAZAAR				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_GRANDBAZAAR),

	///////////////////////// 폭파미션
	STAGE_ID_DOWNTOWN					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DOWNTOWN),
	STAGE_ID_LUXVILLE					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_LUXVILLE),
	STAGE_ID_OUTPOST					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_OUTPOST),
	STAGE_ID_BLOWCITY					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_BLOWCITY),
	STAGE_ID_STORMTUBE					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_STORMTUBE),
	STAGE_ID_SENTRYBASE					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL, STAGE_ORD_SENTRYBASE),
	STAGE_ID_HOSPITAL					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL, STAGE_ORD_HOSPITAL),
	STAGE_ID_DOWNTOWN2					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DOWNTOWN2),
	STAGE_ID_SHOPPINGCENTER				= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_SHOPPINGCENTER),
	STAGE_ID_SANDSTORM					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_SANDSTORM),
	STAGE_ID_CARGOSHIP					= MAKE_STAGEID( STAGE_TYPE_BOMB,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_CARGOSHIP),

	///////////////////////// 파괴미션
	STAGE_ID_BREAKDOWN					= MAKE_STAGEID( STAGE_TYPE_DESTROY,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_BREAKDOWN),
	STAGE_ID_GIRAN						= MAKE_STAGEID( STAGE_TYPE_DESTROY,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_GIRAN),
	STAGE_ID_GIRAN2						= MAKE_STAGEID( STAGE_TYPE_DESTROY,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_GIRAN2),
	STAGE_ID_HELISPOT					= MAKE_STAGEID( STAGE_TYPE_DESTROY,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_HELISPOT),

	/////////////////////////
	STAGE_ID_CRACKDOWN					= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_4VS4,	STAGE_ORD_CRACKDOWN),
	STAGE_ID_SAINTMANSION				= MAKE_STAGEID( STAGE_TYPE_DEATHMATCH,	STAGE_SLOT_MODE_4VS4,	STAGE_ORD_SAINTMANSION),

	///////////////////////// 방어미션
	STAGE_ID_BLACKPANTHER				= MAKE_STAGEID( STAGE_TYPE_DEFENCE,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_BLACKPANTHER),	

	///////////////////////// 보스미션
	STAGE_ID_BREEDING_NEST				= MAKE_STAGEID( STAGE_TYPE_ESCAPE,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_BREEDING_NEST),	
	STAGE_ID_D_UPTOWN					= MAKE_STAGEID( STAGE_TYPE_ESCAPE,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_D_UPTOWN),	
	STAGE_ID_DINO_BREAKDOWN				= MAKE_STAGEID( STAGE_TYPE_ESCAPE,		STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DINO_BREAKDOWN),

	///////////////////////// 제압 미션
	STAGE_ID_DEADTOWN					= MAKE_STAGEID( STAGE_TYPE_DOMINATION,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_DEADTOWN),

	///////////////////////// 튜토리얼 모드
	STAGE_ID_TUTORIAL					= MAKE_STAGEID( STAGE_TYPE_TUTORIAL,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_TUTORIAL),	

	///////////////////////// CC 모드
#ifdef USE_CC_MODE
	STAGE_ID_C_CRACKDOWN				= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_4VS4,	STAGE_ORD_C_CRACKDOWN),	
	STAGE_ID_C_SAINTMANSION				= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_4VS4,	STAGE_ORD_C_SAINTMANSION),
	STAGE_ID_C_DSQUAD					= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_DSQUAD),
	STAGE_ID_C_PORT_AKABA				= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_PORT_AKABA),
	STAGE_ID_C_MSTATION					= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_MSTATION),
	STAGE_ID_C_TRAININGCAMP				= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_TRAININGCAMP),
	STAGE_ID_C_RED_ROCK					= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_RED_ROCK),
	STAGE_ID_C_SAFARI					= MAKE_STAGEID( STAGE_TYPE_CROSSCOUNT,	STAGE_SLOT_MODE_NORMAL,	STAGE_ORD_C_SAFARI),
#endif
};


class STAGEID_INFO
{
private:
	STAGE_ID	_ID;
	UINT32		m_uiEncryptKey;

	char		_DisplayName[ STAGE_NAME_COUNT];
	char		_CodeName[STAGE_CODE_NAME_COUNT];
	char		_StageName[STAGE_CODE_NAME_COUNT];

public:
	STAGEID_INFO();
	STAGE_ID	GetStageID();
	void		GetStageDisplayName(char* szOut);
	void		GetStageCodeName(char* szOut);
	void		GetStageName(char* szOut);

	void		SetStageInfo(STAGE_ID stageID, char* szDiplay, char* szCode, char* szName);
	void		SetStageID(STAGE_ID stageID);
	void		SetStageDisplayName(char* szTemp);
	void		SetStageCodeName(char* szTemp);
	void		SetStageName(char* szTemp);
};


//스테이지 관리를 편리하게 하는 클래스
//STAGEID_INFO, Stage에 대한 이미지를 빠르게 검색하도록 도와준다.
class STAGEID_INFO_LIST
{
public:
	/*****************//*****************//*****************/
	/*	Stage 정보를 나타낸다.
		unsigned int : STAGE_ORDINAL_ID 정보 (	STAGE_ORD_PORT_AKABA, STAGE_ORD_RED_ROCK, STAGE_ORD_LIBRARY 등..)
		STAGEID_INFO : Stage 정보
	*/
	typedef std::map< unsigned int, STAGEID_INFO > StageIdInfoList;

	/*	StageOrg ID 를 나타낸다.
		unsigned int : STAGE_ORDINAL_ID 정보 (	STAGE_ORD_PORT_AKABA, STAGE_ORD_RED_ROCK, STAGE_ORD_LIBRARY 등..)
		std::string : 스테이지 코드명(NPort, Construction, Library 등..)
	*/
	typedef std::map< unsigned int, std::string > StageOrdList;


	/*****************//*****************//*****************/
	/*	서버에서 제공된 Stage 를 나타낸다.
		STAGE_ORDINAL_ID : 인자는 StageOrg ID 를 나타낸다.
		unsigned int : 해당 Stage에 대한 모드(헤드샷, 헤드헌터, AI, 스나이퍼, 샷건)정보를 나타낸다.
	*/
	typedef std::map< STAGE_ORDINAL_ID, unsigned int > ActiveStageList;

	/*	서버에서 제공되는 Stage 순서를 나타낸다.
	*/
	typedef std::vector< STAGE_ORDINAL_ID > ActiveStage;

	/*	서버에서 제공되는 Stage Type(Mode)에 첫번째 Stage ID를 나타낸다.
	*/
	typedef std::vector< STAGE_ID > ActiveStageId;

	/*	서버에서 제공되는 Stage 에 대한 Rotation 정보
	std::pair 로 first[좌], second[우] 를 나타낸다.
	*/
	typedef std::vector< STAGE_TYPE > StageTypeList;
	typedef std::pair< STAGE_TYPE, STAGE_TYPE > StageOrderType;
	typedef std::map< STAGE_TYPE, StageOrderType > ActiveStageRotate;

	/*	서버에서 제공되는 Stage New Mark 표시를 나타낸다.
	*/
	typedef std::vector< UINT8 > ActiveStageNewMark;

	/*****************//*****************//*****************/
	/*	Stage 모드에 따른 StageList
		std::vector 인덱스 인자가 Stage Type(Mode)
		ActiveStage : 사용 가능한 Stage Ordianl Id List
	*/
	typedef std::vector< ActiveStage> ActiveStageTypeList;

	/*	Stage 옵션에 따른 StageList
		unsigned int : Stage Option(Mode)
		ActiveStage : 사용 가능한 Stage Ordianl Id List
	*/
	typedef std::map< unsigned int, ActiveStage > ActiveStageOptionList;


	/*****************//*****************//*****************/
	/*	사용 가능한 Stage 이름 목록
	*/
	typedef std::pair< std::string, unsigned int >  EnableStage;
	typedef std::vector< EnableStage  > EnableStageList;
	
	/*	Stage 모드, Stage 옵션을 나타낸다.
	*/
	typedef std::pair< STAGE_TYPE, unsigned int > EnableStageTypeOpt;

	/*	사용 가능한 Stage 모드 / 옵션 리스트
	*/
	typedef std::map< EnableStageTypeOpt, EnableStageList >	EnableStageTypeOptList;

	/*	사용 가능한 Stage 모드 리소스
	*/
	typedef std::map< unsigned int, EnableStageList >	EnableStageTypeList;

private:
	StageIdInfoList m_StageIdInfoList;
	StageOrdList m_StageOrdList;

	/*****************//*****************//*****************/
	/*	서버에서 제공된 Stage 를 나타낸다.
		STAGE_ORDINAL_ID : 인자는 StageOrg ID 를 나타낸다.
		unsigned int : 해당 Stage에 대한 모드(헤드샷, 헤드헌터, AI, 스나이퍼, 샷건)정보를 나타낸다.
		STAGE_ID	:	STAGE_TYPE + STAGE_SLOT_MODE + STAGE_ORDINAL_ID 가 결합된 번호
	*/
	ActiveStageList m_ActiveStageList;
	ActiveStage m_ActiveStageOrderList;
	ActiveStageId m_ActiveFirstStageId;
	StageTypeList m_StageRotateOrder;
	ActiveStageRotate m_ActiveStageRotate;
	ActiveStageNewMark m_ActiveStageNewMarkList;

	V_STAGEOPTION	m_StageTypeOption[ STAGE_TYPE_MAX ];		// 스테이지 타입별 옵션 마스크

	STAGE_TYPE		m_DefaultStageType;


	/*****************//*****************//*****************/
	/*	Stage 모드에 따른 StageList
		STAGE_TYPE : Stage Type(Mode)
		ActiveStage : 사용 가능한 Stage Ordianl Id List
	*/
	ActiveStageTypeList m_ActiveStageTypeList;

	/*	Stage 옵션에 따른 StageList
		unsigned int : Stage Option(Mode)
		ActiveStage : 사용 가능한 Stage Ordianl Id List
	*/
	ActiveStageOptionList m_ActiveStageOptionList;

	/*****************//*****************//*****************/
	/*	사용 가능한 Stage 모드 / 옵션 리스트
		해당 리스트는 Stage 이름을 가지고 있다.
	*/
	EnableStageTypeOptList m_EnableStageTypeOptList;

	/*	사용 가능한 Stage 모드 리소스
		해당 리스트는 Stage 이름을 가지고 있다.
	*/
	EnableStageTypeList m_EnableStageTypeList;

	/*****************//*****************//*****************/
	/*	특수전 활성화/비활성화를 나타낸다.
	*/
	bool m_ActiveSpecialWar;

protected:
	/*	내부 사용 함수	*/
	void _InsertActiveStageList(STAGE_ORDINAL_ID StageOrderId, unsigned int OptValue = 0);
	void _InsertActiveStageOrderList(STAGE_ORDINAL_ID StageOrderId);
	
	void _InsertActiveStageTypeList(STAGE_TYPE StageType, STAGE_ORDINAL_ID StageOrderId);
	void _InsertActiveStageOptionList(unsigned int OptValue, STAGE_ORDINAL_ID StageOrderId);

	void _ExceptionMode();

protected:
	/*	서버에서 내려주는 활성화 된 맵리스트에 해당 옵션을 얻어 온다.
	StageOrderId	:	알고 싶은 Stage Oridinal Id
	return	:	Stage Option
	*/
	unsigned int _getActiveStageOption(STAGE_ORDINAL_ID StageOrderId) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 해당 옵션을 얻어 온다.
	Index	:	알고 싶은 순서 Index (서버에서 내려준 순서)
	return	:	Stage Option
	*/
	unsigned int _getActiveStageOption(unsigned int Index) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 해당 Stage Oridinal Id에 해당하는 Index를 얻어온다.
	StageOrderId	:	순서 Index (서버에서 내려준 순서) 를 알고 싶은 Stage Oridinal Id
	return	:	Stage Index
	*/
	unsigned int _getActiveStageIndex(STAGE_ORDINAL_ID StageOrderId) const;


public:
	/*****************//*****************//*****************/
	/*	서버에서 내려주는 활성화 된 맵 Stage Oridinal 를 저장한다.
	pActiveStage	:	활성화 된 맵 Stage Id 
	*/
	void SetActiveStageID(UINT32* pActiveStage);

	/*	서버에서 내려주는 활성화 된 맵 Stage Option 를 저장한다.
	pActiveStageOption	:	활성화 된 맵 Stage Option 
	*/
	void SetActiveStageOption(V_STAGEOPTION* pActiveStageOption);

	// 스테이지 활성화 타입과 옵션 설정
	void SetActiveStageTypeOption(void);

	/*	서버에서 내려주는 활성화 각 Stage Type(Mode)에 첫번째 Stage Type(Mode) 를 저장한다.
	pActiveStageTypeFirstId	:	Stage Type(Mode)에 첫번째 Stage Type(Mode)
	*/
	void SetActiveStageTypeFirstId(STAGE_ORDINAL_ID* pActiveStageTypeFirstId );

	/*	서버에서 내려주는 활성화 각 Stage New Mark 를 저장한다.
	pActiveStageNewMark	:	Stage New Mark
	*/
	void SetActiveStageNewMark( UINT8* pActiveStageNewMark );

public:
	/*	활성화된 Stage 인지 판단한다.
	StageId	:	확인 할 Stage Id
	return	:	활성화 유무
	*/
	bool IsActiveStage(STAGE_ID StageId) const;

	/*	활성화된 Stage 인지 판단한다.
	StageOrderId	:	확인 할 Stage Oridinal Id
	return	:	활성화 유무
	*/
	bool IsActiveStage(STAGE_ORDINAL_ID StageOrderId) const;

public:
	/*	Stage Type(Mode) 가 활성화 유무를 알려준다.
	StageType	:	확인 할 Stage Type(Mode)
	return	:	활성화 유무
	*/
	bool IsActiveStageType(STAGE_TYPE StageType) const;

	/*	해당 Stage 가 Stage Type(Mode) 로 활성화 유무를 알려준다.
	StageId	:	활성화 유무를 알고 싶은 Stage Id
	StageType	:	확인 할 Stage Type(Mode)
	return	:	활성화 유무
	*/
	bool IsActiveStageType(STAGE_ID StageId, STAGE_TYPE StageType) const;

	/*	해당 Stage 가 Stage Type(Mode) 로 활성화 유무를 알려준다.
	StageOrderId	:	활성화 유무를 알고 싶은 Stage Oridinal Id
	StageType	:	확인 할 Stage Type(Mode)
	return	:	활성화 유무
	*/
	bool IsActiveStageType(STAGE_ORDINAL_ID StageOrderId, STAGE_TYPE StageType) const;

	/*	Stage Type(Mode) 로 활성화된 맵 리스트를 얻어 온다.
	StageList	:	반환값으로 사용 될 맵 리스트
	StageType	:	Stage Type(Mode)
	*/
	void GetActiveStageType(ActiveStage & StageList, STAGE_TYPE StageType) const;

	/*	Stage Type(Mode) 로 활성화 첫번째 맵을 얻어 온다.
	StageType	:	Stage Type(Mode)
	return	:	첫번째 맵 Stage Id
	*/
	STAGE_ID GetFirstStageType(STAGE_TYPE StageType) const;

public:
	/*	Stage Option이 활성화 유무를 알려준다.
	OptValue	:	확인 할 Stage Option
	return	:	활성화 유무
	*/
	bool IsActiveStageOption(V_STAGEOPTION OptValue) const;
	
	/*	해당 Stage 가 Stage Option으로 활성화 유무를 알려준다.
	StageId	:	활성화 유무를 알고 싶은 Stage Id
	OptValue	:	확인 할 Stage Option
	return	:	활성화 유무
	*/
	bool IsActiveStageOption(STAGE_ID StageId, V_STAGEOPTION OptValue) const;

	/*	해당 Stage 가 Stage Option으로 활성화 유무를 알려준다.
	StageOrderId	:	활성화 유무를 알고 싶은 Stage Oridinal Id
	OptValue	:	확인 할 Stage Option
	return	:	활성화 유무
	*/
	bool IsActiveStageOption(STAGE_ORDINAL_ID StageOrderId, V_STAGEOPTION OptValue) const;

	// 타입과 옵션이 함께 적용된 활성 스테이지 유무
	bool	IsActiveStageTypeOption( STAGE_TYPE StageType, V_STAGEOPTION Stage_Option_Type ) const;

	/*	Stage Option 로 활성화된 맵 리스트를 얻어 온다.
	StageList	:	반환값으로 사용 될 맵 리스트
	OptValue	:	Stage Option
	return	:	활성화 유무
	*/
	void GetActiveStageOption(ActiveStage & StageList, V_STAGEOPTION OptValue) const;

	/*	Stage Option 로 활성화 첫번째 맵을 얻어 온다.
	OptValue	:	Stage Option
	return	:	첫번째 맵 Stage Id
	*/
	STAGE_ID GetFirstStageOption(V_STAGEOPTION OptValue) const;

protected:
	/*	활성화 된 Stage 이름 목록을 얻어온다.
	List	:	반환값으로 사용 될 맵 이름 리스트
	StageType	:	Stage Type(Mode)
	*/
	void _GetEnableActiveStage(EnableStageList & List, STAGE_TYPE StageType);

public:
	/*	특수전 활성화/비활성화를 얻어온다.
	return	:	특수전 활성화/비활성화 유무
	*/
	bool IsActiveSpecialWar() const	{	return m_ActiveSpecialWar;	}

	/*	활성화 된 Stage 이름 목록을 얻어온다.
	List	:	반환값으로 사용 될 맵 이름 리스트
	StageType	:	Stage Type(Mode)
	OptValue	:	Stage Option
	*/
	void GetEnableActiveStage(EnableStageList & List, 
		STAGE_TYPE StageType, V_STAGEOPTION OptValue);


public:
	/*	서버에서 내려주는 활성화 된 맵리스트에 Index(내려 준 순서)에 해당하는 Stage Id를 얻어 온다.
	Index	:	서버에서 내려 준 순서
	return	:	해당 Stage Id
	*/
	STAGE_ID GetActiveStageID(INT32 Index) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 Stage Id로 해당 Stage Oridinal Id를 얻어 온다.
	StageId	:	Stage Id
	return	:	Stage Oridinal Id
	*/
	STAGE_ORDINAL_ID GetActiveStageOrderId(STAGE_ID StageId) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 Stage Id 에 해당하는 Index(내려 준 순서)를 얻어온다.
	StageId	:	Stage Id
	return	:	Index( 내려준 순서 )
	*/
	size_t GetActiveStageIndex(STAGE_ID StageId) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 Stage Oridinal Id 에 해당하는 Index(내려 준 순서)를 얻어온다.
	StageOrderId	:	Stage Oridinal Id
	return	:	Index( 내려준 순서 )
	*/
	size_t GetActiveStageIndex(STAGE_ORDINAL_ID StageOrderId) const;

public:
	/*	서버에서 내려주는 활성화 된 맵리스트에 해당하는 StageId 에 대한 NewMark 표시를 알려준다.
	StageOrderId	:	Stage Oridinal Id
	return	:	GOODS_MARK_NO, GOODS_MARK_NEW, GOODS_MARK_HOT, GOODS_MARK_EVENT, GOODS_MARK_PCROOM	
	*/
	UINT8 GetActiveStageNewMark(STAGE_ORDINAL_ID StageOrderId) const;

	/*	서버에서 내려주는 활성화 된 맵리스트에 해당하는 StageId 에 대한 NewMark 표시를 알려준다.
	StageId	:	Stage Id
	return	:	GOODS_MARK_NO, GOODS_MARK_NEW, GOODS_MARK_HOT, GOODS_MARK_EVENT, GOODS_MARK_PCROOM	
	*/
	UINT8 GetActiveStageNewMark(STAGE_ID StageId) const;

protected:
	STAGE_ID _SetStageId(STAGE_ID StageId, STAGE_TYPE StageType);
	STAGE_ID _GetNextStageType(STAGE_TYPE StageType, V_STAGEOPTION OptValue, bool forward);


public:
	/*	서버에서 내려주는 활성화 된 맵리스트에 해당하는 StageId 에 대한 NewMark 표시를 알려준다.
	StageOrderId	:	Stage Oridinal Id
	return	:	GOODS_MARK_NO, GOODS_MARK_NEW, GOODS_MARK_HOT, GOODS_MARK_EVENT, GOODS_MARK_PCROOM	
	*/
	STAGE_ID GetFirstStageId(STAGE_TYPE StageType, V_STAGEOPTION OptValue);
	
	STAGE_ID GetNextStageType(STAGE_ID StageId, V_STAGEOPTION OptValue, bool forward);
	STAGE_ID GetNextStageType(STAGE_TYPE StageType, V_STAGEOPTION OptValue, bool forward);


public:
	void Init();

	/*	Stage Type(Mode) 순서
	*/
	void Init_StageTypeOrder();
	//STAGEID_INFO, StageOrg 에 대한 자료를 추가한다.
	//stageID : StageID 인덱스
	//szDiplay : 화면에 보이는 이름
	//szCode : pef 등록 이름
	//szName : 스테이지 이름
	void	 InsertStageInfo(STAGE_ID stageID, char* szDiplay, char* szCode, char* szName);
	const std::string& GetStageName(INT32 OrgId) const;

	STAGE_ID	GetStageID(int StageOrgId);

	void		GetStageDisplayName(int StageOrgId, char* szOut);
	void		GetStageCodeName(int StageOrgId, char* szOut);
	void		GetStageName(int StageOrgId, char* szOut);

	void		SetStageInfo(int StageOrgId, STAGE_ID stageID, char* szDiplay, char* szCode, char* szName);
	void		SetStageID(int StageOrgId, STAGE_ID stageID);
	void		SetStageDisplayName(int StageOrgId, char* szTemp);
	void		SetStageCodeName(int StageOrgId, char* szTemp);
	void		SetStageName(int StageOrgId, char* szTemp);

	const STAGEID_INFO& operator[](const size_t index) const;
	STAGEID_INFO& operator[](const size_t index);

	const StageIdInfoList&	GetStageIdInfoList(void)	{ return m_StageIdInfoList;		}
	STAGE_TYPE	GetStageTypeDefault(void) const			{ return m_DefaultStageType;	}
};

//Initialization
extern STAGEID_INFO_LIST s_StageInfoTableList;
void InitStageInfo();

void				getStageCodeName(char* szOut, STAGE_ID id);			// MapJump를 위한 Stage명
void				getStageCodeName(char* szOut, INT32 ord);

void				getStageDisplayName(char* szOut, STAGE_ID id);				// 화면 표시를 위한 Stage명
void				getStageDisplayName(char* szOut,INT32 ord);

void				getStageName(char* szOut, INT32 ord);
void				getStageName(char* szOut, STAGE_ID id);


/*****************//*****************//*****************/
STAGE_ID getStageId(UINT32 ord);
STAGE_ID getStageId(STAGE_ORDINAL_ID ord);

STAGE_ORDINAL_ID getStageOrd(UINT32 stageId);
STAGE_ORDINAL_ID getStageOrd(STAGE_ID stageId);

STAGE_SLOT_MODE getStageSlot(UINT32 stageId);
STAGE_SLOT_MODE getStageSlot(STAGE_ID stageId);
STAGE_ID setStageSlot(STAGE_ID stageId, STAGE_SLOT_MODE slotMode);

STAGE_TYPE getStageType(UINT32 stageId);
STAGE_TYPE getStageType(STAGE_ID stageId);

STAGE_ID setStageType(UINT32 stageId, UINT32 stageType);
STAGE_ID setStageType(STAGE_ID stageId, STAGE_TYPE stageType);


/*	서버에서 내려주는 활성화 Stage Oridinal 리스트	*/
void	 _SetActiveStage_( UINT32* StageIdx );

/*	서버에서 내려주는 활성화 Stage Option 리스트	*/
void	 _SetStageOption_( V_STAGEOPTION* pStageOption );

//	활성화 Stage Option과 Oridianl을 이용해 TypeOption 값을 설정
void	_SetStageTypeOption(void);

/*	서버에서 내려주는 활성화 각 Stage Type(Mode)에 첫번째 Stage Type(Mode) 리스트	*/
void _SetFirstStageByType_( STAGE_ORDINAL_ID* pStageType );

/*	서버에서 내려주는 활성화 각 Stage New Mark 리스트	*/
void _SetStageNewMark_( UINT8* pStageNewMark );


#define	TEAMBATTLE_TIME_COUNT			11			//클라이언트에서 이값을 사용하면 -1을해줘야 함	 
#define TEAMBATTLE_KILL_COUNT			7			//클라이언트에서 이값을 사용하면 -1을해줘야 함	
#define MISSION_ROUND_TIME_COUNT		30			//

/////////////////////////////////////////////////////////////////////////////////////////
//


// GAME_STAGE_NAME과 index를 맞춰야한다. CommonDef.cpp

extern const UINT32	g_ExpValue[DEF_RANK_COUNT]; 

// 진급 Point

extern const INT32 g_InitialRankUpPoint;


//Battle Info Table 
extern const UINT32	g_TeamTimeTable[TEAMBATTLE_TIME_COUNT];
extern const UINT32	g_TeamKillCountTable[TEAMBATTLE_KILL_COUNT]; 
extern const UINT32	g_MissionRoundCountTable[BATTLE_ROUND_TYPE_MAX];
extern const bool	g_bHaveNaviMesh[STAGE_ORD_MAX];

// 죽인 방법 - 0xF(16개) 이상을 사용하면 않됨.
enum CHARA_DEATH_TYPE
{
	CHARA_DEATH_UN = 0,
	CHARA_DEATH_A,					// default
	CHARA_DEATH_B,					// Bome! = Granade
	CHARA_DEATH_H,					// Head Shot!!
	CHARA_DEATH_F,					// Falling death
	CHARA_DEATH_O,					// by Object Explosion
	CHARA_DEATH_FAST_OBJECT,		// by Fast Object, Damage Object
	CHARA_DEATH_POISON,				// 독에 의한 사망
	CHARA_DEATH_TRAMPLED,			// 짓밟힘
	CHARA_DEATH_HOWL,				// HOWL에 놀라 죽음;;;
//	CHARA_DEATH_ASSIST,				// Assistkill;;;
	CHARA_DEATH_BUFF,				// 엘리트 버프
	CHARA_DEATH_TEMP5,
	CHARA_DEATH_TEMP4,
	CHARA_DEATH_TEMP3,
	CHARA_DEATH_TEMP2,
	CHARA_DEATH_TEMP1,
	
	CHARA_DEATH_MAXCOUNT
};

// 죽였을때 적의 상태 - 0xF(16개) 이상 사용하면 안됨
enum CHARA_DEATH_ENEMY_STATE_TYPE
{
	CHARA_DEATH_ENEMY_STATE_NONE	= 0,
	CHARA_DEATH_ENEMY_STATE_DEFENCE,		// 방어미션에서 Defence
	CHARA_DEATH_ENEMY_STATE_ESCAPE,			// 탈출미션에서 탈출 직전
	CHARA_DEATH_ENEMY_STATE_SAVER,			// 제압미션에서 Saver
	CHARA_DEATH_ENEMY_STATE_TEMP12,
	CHARA_DEATH_ENEMY_STATE_TEMP11,
	CHARA_DEATH_ENEMY_STATE_TEMP10,
	CHARA_DEATH_ENEMY_STATE_TEMP9,
	CHARA_DEATH_ENEMY_STATE_TEMP8,
	CHARA_DEATH_ENEMY_STATE_TEMP7,
	CHARA_DEATH_ENEMY_STATE_TEMP6,
	CHARA_DEATH_ENEMY_STATE_TEMP5,
	CHARA_DEATH_ENEMY_STATE_TEMP4,
	CHARA_DEATH_ENEMY_STATE_TEMP3,
	CHARA_DEATH_ENEMY_STATE_TEMP2,
	CHARA_DEATH_ENEMY_STATE_TEMP1,

	CHARA_DEATH_ENEMY_STATE_MAXCOUNT,
};

// 죽였을때 나의 상태 - 0xF(16개) 이상 사용하면 안됨
enum CHARA_DEATH_MY_STATE_TYPE
{
	CHARA_DEATH_MY_STATE_NONE	= 0,
	CHARA_DEATH_MY_STATE_ASSIST,			// 탈출미션에서 Assist
	CHARA_DEATH_MY_STATE_TEMP14,			
	CHARA_DEATH_MY_STATE_TEMP13,
	CHARA_DEATH_MY_STATE_TEMP12,
	CHARA_DEATH_MY_STATE_TEMP11,
	CHARA_DEATH_MY_STATE_TEMP10,
	CHARA_DEATH_MY_STATE_TEMP9,
	CHARA_DEATH_MY_STATE_TEMP8,
	CHARA_DEATH_MY_STATE_TEMP7,
	CHARA_DEATH_MY_STATE_TEMP6,
	CHARA_DEATH_MY_STATE_TEMP5,
	CHARA_DEATH_MY_STATE_TEMP4,
	CHARA_DEATH_MY_STATE_TEMP3,
	CHARA_DEATH_MY_STATE_TEMP2,
	CHARA_DEATH_MY_STATE_TEMP1,

	CHARA_DEATH_MY_STATE_MAXCOUNT,
};
// 16가지 종류밖에 사용할수 없습니다. 20110624 김현우
enum CHARA_KILL_TYPE
{
	CHARA_KILL_DEFAULT = 0,
	CHARA_KILL_PIERCING,
	CHARA_KILL_MASS,
};
// 4가지 종류밖에 사용할수 없습니다. 20110624 김현우
enum CHARA_KILL_TYPE_HIGH
{
	CHARA_KILL_HIGH_BOMBING	= 0x10,
	CHARA_KILL_HIGH_TEMP2	= 0x20,
	CHARA_KILL_HIGH_TEMP1	= 0x40,
	CHARA_KILL_HIGH_TEMP	= 0x80,
};
// Game Key Mode
enum GAMEKEY_MODE_STATE
{
	GAMEKEY_MODE_MENU = 0,		// 메뉴에 사용되는 게임키 모드
	GAMEKEY_MODE_GAME,			// 게임에 사용되는 게임키 모드

	GAMEKEY_MODE_COUNT,
};


//폰트 타입
enum FONT_TYPE
{
	FONT_ENGLISH_COMMON = 0,
	FONT_KOREAN_COMMON,

	FONT_TYPE_COUNT,
};

// 공용 메시지 박스에서 사용되는 타입(버튼이 나오는..)
enum MESSAGE_BOX_TYPE
{
	MESSAGE_BOX_TYPE_CONFIRM = 0,
	MESSAGE_BOX_TYPE_CONFIRM_CANCEL,
	MESSAGE_BOX_TYPE_NONE,

	MESSAGE_BOX_TYPE_COUNT,
};

enum ITEM_CHANGE_TYPE
{
	ITEM_CHANGE_TYPE_INSERT,
	ITEM_CHANGE_TYPE_AUTH,
	ITEM_CHANGE_TYPE_USE,
};

//	무기 분류 방식
//
// 아이템 아이디 세번째에 들어갑니다. ( 3 )
//
// 아이템 아이디에서 저장되는 자릿수 : 1000000-1 ~ 1000
// 유효 값 범위 : 967
enum WEAPON_CLASS_TYPE
{
	WEAPON_CLASS_UNKNOWN	= 0,

	WEAPON_CLASS_KNIFE,		//	나이프
	WEAPON_CLASS_HANDGUN,	//	권총
	WEAPON_CLASS_ASSAULT,	//	소총
	WEAPON_CLASS_SMG,		//	서브머신건
	WEAPON_CLASS_SNIPER,	//	저격총
	WEAPON_CLASS_SHOTGUN,	//	산탄총
	WEAPON_CLASS_THROWING,	//	투척 무기
	WEAPON_CLASS_ITEM,		//	아이템
	WEAPON_CLASS_MISSION,	//	미션 아이템
	WEAPON_CLASS_MG,		//	머신건

	// 게임에서만 사용하는 클래스 ( 서버에는 없습니다.)
	WEAPON_CLASS_GRENADESHELL,		// 유탄
	WEAPON_CLASS_BOMBTRIGGER,		// 폭탄 스위치

	// 새로 추가되는 무기 2008.07.09
	WEAPON_CLASS_CIC,
	WEAPON_CLASS_DUALHANDGUN,
	WEAPON_CLASS_DUALKNIFE,

	WEAPON_CLASS_ROCKET_LAUNCHER,
	WEAPON_CLASS_OBJECT_GUN,		// 특정 오브젝트에 붙어 있는 총 ( M197....)

	WEAPON_CLASS_DUALSMG,

	WEAPON_CLASS_DINO,		
	WEAPON_CLASS_DINO_LEFT_SCRATCH,	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	WEAPON_CLASS_TREX,				// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
	WEAPON_CLASS_STING,
	WEAPON_CLASS_KNUCKLE,			// 너클
	WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
	WEAPON_CLASS_ELITE_LEFT_SCRATCH,
	WEAPON_CLASS_BOW,
	WEAPON_CLASS_COUNT,
};


#define WEAPON_CLASS_NPC_ARROW		100

enum EQMIPEMENT_SLOT
{
	EQMIPEMENT_SLOT_PRIM,
	EQMIPEMENT_SLOT_SUB,
	EQMIPEMENT_SLOT_MELEE,
	EQMIPEMENT_SLOT_THROWING,
	EQMIPEMENT_SLOT_ITEM,
	EQMIPEMENT_SLOT_CHAR_RED,
	EQMIPEMENT_SLOT_CHAR_BLUE,
	EQMIPEMENT_SLOT_CHAR_HEAD,
	EQMIPEMENT_SLOT_CHAR_ITEM,
	EQMIPEMENT_SLOT_CHAR_DINO,
	EQMIPEMENT_SLOT_COUNT,
};

//	class
//
// 굿즈 아이디 첫번째 
enum GOODS_CLASS_TYPE
{
	GOODS_CLASS_UNKNOWN			= 0,
	GOODS_CLASS_PRIMARY			= 1,
	GOODS_CLASS_SECONDARY		= 2,
	GOODS_CLASS_MELEE			= 3,
	GOODS_CLASS_GRENADE			= 4,
	GOODS_CLASS_THROWING		= 5,
	GOODS_CLASS_CHARA			= 6,
	GOODS_CLASS_HEAD			= 7,
	GOODS_CLASS_QUESTCARD		= 8,
	GOODS_CLASS_POINT			= 9,	// 포인트. 판매용 아님.
	GOODS_CLASS_EXPENDABLE		= 10,
	GOODS_CLASS_MIX				= 11,
	GOODS_CLASS_SALECOUPON		= 12,	// 할인쿠폰
	GOODS_CLASS_RSPRICE			= 13,	// 뽑기상점 탭 가격
		 

	GOODS_CLASS_COUNT
};

//	class
//
// 아이템 아이디 첫번째에 들어갑니다. ( 1 )
//
// 아이템 아이디에서 저장되는 자릿수 : 0xFFFFFFFF(약42억) ~ 100000000
// 유효 값 범위 : 0~42
enum ITEM_CLASS_TYPE
{
	ITEM_CLASS_UNKNOWN	= 0,
	ITEM_CLASS_ASSAULT	= 1,
	ITEM_CLASS_SMG,
	ITEM_CLASS_SNIPER,
	ITEM_CLASS_SHOTGUN,
	ITEM_CLASS_MACHINE,
	ITEM_CLASS_HANDGUN,
	ITEM_CLASS_KNIFE,
	ITEM_CLASS_GRENADE,
	ITEM_CLASS_THROWING,
	ITEM_CLASS_CHARA,
	ITEM_CLASS_HEAD,
	ITEM_CLASS_MAINTENANCE,
	ITEM_CLASS_EXPENDABLE,
	ITEM_CLASS_COUPON,
	ITEM_CLASS_POINT,
	ITEM_CLASS_TEMP, 
	ITEM_CLASS_QUESTCARD,	// 임무카드
	ITEM_CLASS_SALECOUPON,	// 할인쿠폰
	ITEM_CLASS_RSCOUPON,	// 뽑기이용권

	ITEM_CLASS_COUNT		// MAX값일 뿐
};

//
// 아이템 아이디 두번째에 들어갑니다. ( 2 )
//
// 아이템 아이디에서 저장되는 자릿수 : 100000000-1 ~ 1000000
// 유효 값 범위 : 0~94
enum WEAPON_USAGE_TYPE
{
	WEAPON_USAGE_UNKNOWN	= -1,
	WEAPON_USAGE_ALL		= WEAPON_USAGE_UNKNOWN,
	WEAPON_USAGE_PRIMARY	= 0,	//	주무기
	WEAPON_USAGE_SECONDARY	= 1,	//	보조무기
	WEAPON_USAGE_MELEE		= 2,	//	근접무기
	WEAPON_USAGE_THROWING	= 3,	//	투척무기	
	WEAPON_USAGE_ITEM		= 4,	//	투척 아이템
	WEAPON_USAGE_MISSION	= 5,	//	미션 아이템
	WEAPON_USAGE_COUNT
};



//	유료 아이템 분류 방식
//
// 아이템 아이디 두번째에 들어갑니다. ( 2 )
//
enum CASHITEM_USAGE_TYPE
{
	CASHITEM_USAGE_UNKNOWN		= -1,
	CASHITEM_USAGE_ALL			= CASHITEM_USAGE_UNKNOWN,
	CASHITEM_USAGE_MAINTENANCE	= 0,	//	지속성 아이템
	CASHITEM_USAGE_EXPENDABLES	= 1,	//	소모성 아이템

	CASHITEM_USAGE_COUNT
};

// Hit 부위에 대한 정의
enum HIT_CHARA_PART
{
	HIT_CHARA_PART_NA = 0,
	HIT_CHARA_PART_HEAD,
	HIT_CHARA_PART_BODY,
	HIT_CHARA_PART_ARM,
	HIT_CHARA_PART_LEG,
	HIT_CHARA_PART_WEAPON,
	HIT_CHARA_PART_HELMET,
	HIT_CHARA_PART_NECK,
	HIT_CHARA_PART_VITAL,


	HIT_CHARA_PART_COUNT,
};

// 행동 ID
enum ACTION_ID
{
	ACTION_ID_NA				= 0,			// 정의되지 않음.
	ACTION_ID_C4_PLANT,							// C4 설치하기
	ACTION_ID_C4_DEFUSE,						// C4 해체하기
	ACTION_ID_HIT_HEAD_20,						// 머리 명중률 20% 이상
	ACTION_ID_HIT_HEAD_30,						// 머리 명중률 30% 이상
	ACTION_ID_HIT_CHEST_20,						// 가슴 명중률 20% 이상
	ACTION_ID_HIT_CHEST_30,						// 가슴 명중률 30% 이상
	ACTION_ID_HIT_ARM_20,						// 팔 명중률 20% 이상
	ACTION_ID_HIT_ARM_30,						// 팔 명중률 30% 이상
	ACTION_ID_HIT_LEG_20,						// 다리 명중률 20% 이상
	ACTION_ID_HIT_LEG_30,						// 다리 명중률 30% 이상
	ACTION_ID_STAGE_ENTER,						// 입장
	ACTION_ID_STAGE_INTERCEPT,					// 난입
	ACTION_ID_WIN,								// 승리
	ACTION_ID_DEFEAT,							// 패배
	ACTION_ID_KILL,								// Kill
	ACTION_ID_DEATH,							// Death
	ACTION_ID_SUICIDE,							// 자살
	ACTION_ID_ENFORCE_EXIT,						// 강제퇴장
	ACTION_ID_RANK_1,							// 최종 결과 순위 1
	ACTION_ID_RANK_2,							// 최종 결과 순위 2
	ACTION_ID_RANK_3,							// 최종 결과 순위 3
	ACTION_ID_RANK_4,							// 최종 결과 순위 4
	ACTION_ID_RANK_5,							// 최종 결과 순위 5
	ACTION_ID_RANK_6,							// 최종 결과 순위 6
	ACTION_ID_RANK_7,							// 최종 결과 순위 7
	ACTION_ID_RANK_8,							// 최종 결과 순위 8
	ACTION_ID_HEADSHOT,							// Head shot 달성
	ACTION_ID_DOUBLE_KILL,						// Double Kill 달성
	ACTION_ID_PIERCING,							// Piercing Kill 달성
	ACTION_ID_TRIPLE_KILL,						// Triple Kill 달성
	ACTION_ID_MASS_KILL,						// Mass Kill
	ACTION_ID_HIT_RATE_40,						// 명중률 40%
	ACTION_ID_HIT_RATE_50,						// 명중률 50%
	ACTION_ID_HIT_RATE_60,						// 명중률 60%
	ACTION_ID_KILL_RATE_40,						// 킬 비율 40%
	ACTION_ID_KILL_RATE_50,						// 킬 비율 50%
	ACTION_ID_KILL_RATE_60,						// 킬 비율 60%
	ACTION_ID_HELMET_PROTECTION,				// 헬맷 방어
	ACTION_ID_DESTROY,							// 목표물 파괴
	ACTION_ID_KILL_WEAPONCLASS,					// 특정클래스 무기 사용중인 유저 킬
	ACTION_ID_DOUBLE_KILL_WEAPONCLASS,			// 특정클래스 무기 사용중인 유저 더블킬
	ACTION_ID_TRIPLE_KILL_WEAPONCLASS,			// 특정클래스 무기 사용중인 유저 트리플킬
	ACTION_ID_USE_WEAPON,						// 특정 무기 사용
	ACTION_ID_CHAINKILLER,						// 체인킬
	ACTION_ID_CHAINSLUGGER,						// 체인 슬러거
	ACTION_ID_CHAINSTOPPER,						// 체인 스타퍼
	ACTION_ID_CHAINHEADSHOT,					// 체인 헤드샷
	ACTION_ID_PROTECT,							// 오브젝트 보호
	ACTION_ID_DEATHBLOW,						// 데스블로우
	ACTION_ID_TOUCHDOWN,						// 터치다운
	ACTION_ID_TOUCHDOWN_ACE_ATTACKER,			// 에이스 어태커
	ACTION_ID_TOUCHDOWN_HATTRICK,				// 해트트릭
	ACTION_ID_TOUCHDOWN_GAME_MAKER,				// 게임메이커
	ACTION_ID_KILL_ASSIST,						// 킬 어시스트
	ACTION_ID_KILL_MAN,							// 인간을 죽여라

	ACTION_ID_COUNT
};

const char *		getActionName( ACTION_ID id);

// 캐릭터 팀 분류 방식
//
// 아이템 아이디 세번째에 들어갑니다. ( 3 )
//

enum CHARACTER_TEAM_TYPE
{
	CHARACTER_TEAM_NONE = 0x00,
	CHARACTER_TEAM_RED = 0x01,
	CHARACTER_TEAM_BLUE = 0x02,
	NPC_TEAM_RED = 0x04,
	NPC_TEAM_BLUE = 0x08,
	CHARACTER_TEAM_RB = (CHARACTER_TEAM_RED | CHARACTER_TEAM_BLUE)
};

// 캐릭터 class 분류 방식
//
// 아이템 아이디 두번째에 들어갑니다. ( 2 )
//
enum CHARACTER_CLASS_TYPE
{
	CHARACTER_CLASS_UNKNOWN = 0,
	CHARACTER_CLASS_ALL = CHARACTER_CLASS_UNKNOWN,

	CHARACTER_CLASS_CHARA_TYPE,		//	캐릭터
	CHARACTER_CLASS_HEAD,			//	헬멧
	CHARACTER_CLASS_ITEM,			//	호칭 베레모
	CHARACTER_CLASS_MASK,
	CHARACTER_CLASS_EVENT,			//	이벤트 장비들
	CHARACTER_CLASS_CHARA_DINO,		//	공룡
	CHARACTER_CLASS_RESERVED,		//	7번은 예약되어 있습니다.

	CHARACTER_CLASS_COUNT
};

enum CHARACTER_RES_ID
{
	CHARACTER_RES_ID_UNKNOWN = 0,

	// 아래 두개는 안씀..
	//CHARACTER_RES_ID_MALEDEFAULT		= 1,	//남자 1셋
	//CHARACTER_RES_ID_FEMALEDEFAULT	= 2,	//여자 1셋

	CHARACTER_RES_ID_FEMALE_RED			= 3,	// Tarantula
	CHARACTER_RES_ID_FEMALE_BLUE		= 4,	// Keen Eyes

	CHARACTER_RES_ID_MALE_RED			= 5,	// Redbulls
	CHARACTER_RES_ID_MALE_BLUE			= 6,	// Acid Pol

	CHARACTER_RES_ID_D_FOX				= 7,
	CHARACTER_RES_ID_LEOPARD			= 8,

	CHARACTER_RES_ID_FEMALE_HIDE		= 9,
	CHARACTER_RES_ID_FEMALE_VIPERRED	= 10,

	CHARACTER_RES_ID_REINFORCED_D_FOX			= 11,
	CHARACTER_RES_ID_REINFORCED_LEOPARD			= 12,
	CHARACTER_RES_ID_REINFORCED_VIPERRED		= 13,
	CHARACTER_RES_ID_REINFORCED_HIDE			= 14,

	CHARACTER_RES_ID_REINFORCED_COMBO_D_FOX		= 15,
	CHARACTER_RES_ID_REINFORCED_COMBO_LEOPARD	= 16,
	CHARACTER_RES_ID_REINFORCED_COMBO_VIPERRED	= 17,
	CHARACTER_RES_ID_REINFORCED_COMBO_HIDE		= 18,

	CHARACTER_RES_ID_REINFORCED_ACID_POL		= 19,
	CHARACTER_RES_ID_REINFORCED_KEEN_EYES		= 20,
	CHARACTER_RES_ID_REINFORCED_REDBULLS		= 21,
	CHARACTER_RES_ID_REINFORCED_TARANTULA		= 22,
	CHARACTER_RES_ID_DINO_TRex					= 23,
	CHARACTER_RES_ID_DINO_Raptor				= 24,

	CHARACTER_RES_ID_NORMAL_D_FOX				= 25,		// 옵션 제거된 일반 캐릭터
	CHARACTER_RES_ID_NORMAL_LEOPARD				= 26,		// 옵션 제거된 일반 캐릭터
	CHARACTER_RES_ID_NORMAL_FEMALE_HIDE			= 27,		// 옵션 제거된 일반 캐릭터
	CHARACTER_RES_ID_NORMAL_FEMALE_VIPERRED		= 28,		// 옵션 제거된 일반 캐릭터
	CHARACTER_RES_ID_DINO_STING					= 29,
	CHARACTER_RES_ID_DINO_TANK					= 30,
	CHARACTER_RES_ID_DINO_ACID					= 31,
	CHARACTER_RES_ID_DINO_ELITE					= 32,
	CHARACTER_RES_ID_CHOU						= 33,
	CHARACTER_RES_ID_BELLA						= 34,

	CHARACTER_RES_ID_REINFORCED_CHOU			= 35,
	CHARACTER_RES_ID_REINFORCED_BELLA			= 36,

	CHARACTER_RES_ID_DINO_RAPTOR_BLUE			= 37,
	CHARACTER_RES_ID_DINO_RAPTOR_GREEN			= 38,
	CHARACTER_RES_ID_DINO_MUTANTREX				= 39,
	CHARACTER_RES_ID_DINO_ACID_NOT_BOMB			= 40,
	
	CHARACTER_RES_ID_DINO_CC_Raptor				= 41,
	CHARACTER_RES_ID_DINO_CC_STING				= 42,
	CHARACTER_RES_ID_DINO_CC_ACID				= 43,

	MAX_CHARACTER_RES_ID
};

enum HEAD_RES_ID
{
	HEAD_RES_ID_UNKNOWN = 0,

	HEAD_RES_ID_DEFAULT			= 1,	//	기본
	HEAD_RES_ID_NORMAL			= 2,	//	일반	
	HEAD_RES_ID_REINFORCE		= 3,	//	강화
	HEAD_RES_ID_HEAT			= 4,
	HEAD_RES_ID_RADAR			= 5,
	HEAD_RES_ID_TARGET			= 6,
	HEAD_RES_ID_STRENGTHENPLUS	= 7,
	HEAD_RES_ID_SUPER			= 8,
	HEAD_RES_ID_ANGELRING		= 9,

	MAX_HEAD_RES_ID,

	//HEAD_RES_ID_REINFORCE_EVENT = 7,	//  강화 헬멧 이벤트
};

//베레모 추가
enum ITEM_RES_ID
{
	ITEM_RES_ID_UNKNOWN = 0,

	ITEM_RES_ID_BERET_ASSAULT	= 1,	// 광폭한 맹수 베레모 (호칭 마스터)
	ITEM_RES_ID_BERET_SMG		= 2,	// 교활한 침입자 베레모 (호칭 마스터)
	ITEM_RES_ID_BERET_RIFLE		= 3,	// 은밀한 암살자 베레모 (호칭 마스터)
	ITEM_RES_ID_BERET_SHOTGUN	= 4,	// 난폭한 파괴자 베레모 (호칭 마스터)
	ITEM_RES_ID_BERET_HANDGUN	= 5,	// 예리한 명사수 베레모 (호칭 마스터)
	
	ITEM_RES_ID_BERET_BLACK		= 6,			// 블랙 베레모 / 이벤트용

	ITEM_RES_ID_BERET_CHE_RED_STAR		= 7,	// 체게바라 붉은별 베레모
	ITEM_RES_ID_BERET_CHE_YELLOW_STAR	= 8,	// 체게바라 노란별 베레모
	ITEM_RES_ID_BERET_CHE_CROSS_SWORD	= 9,	// 체게바라 칼교차 베레모

	ITEM_RES_ID_BERET_THAI_PBTN	= 10,	// 태국 PBTN 베레모
	ITEM_RES_ID_BERET_BLACKPB	= 11,	// BLACK PB 베레모

	ITEM_RES_ID_BERET_COUNT
};

//마스크 추가
enum MASK_ID
{
	MASK_ID_UNKNOWN = 0,

	MASK_ID_WHITE			= 1,
	MASK_ID_BLACK			= 2,
	MASK_ID_RUSSIAN_BLUE	= 3,
	MASK_ID_ROK_MARINE		= 4,
	MASK_ID_FRENCH_DESERT	= 5,

	MASK_ID_FLAME			= 6,
	MASK_ID_TWO_TONE_BR		= 7,
	MASK_ID_IRON			= 8,
	MASK_ID_TARGET_MARK		= 9,
	MASK_ID_PUMPKIN			= 10,

	MASK_ID_JEWEL_PINK		= 11,
	MASK_ID_SMILE_GOLD		= 12,
	MASK_ID_SKULL			= 13,
	MASK_ID_PIERROT			= 14,
	MASK_ID_ALIEN_BLUE_EYES	= 15,

	MASK_ID_ALIEN_RED_EYES	= 16,
	MASK_ID_CROSS_RED		= 17,
	MASK_ID_JASON			= 18,
	MASK_ID_PANDA			= 19,
	MASK_ID_EGG				= 20,
	MASK_ID_DEATH			= 21,

	MASK_ID_WORLDCUP_ARGENTINA	= 22,
	MASK_ID_WORLDCUP_BRAZIL		= 23,
	MASK_ID_WORLDCUP_ENGLAND	= 24,
	MASK_ID_WORLDCUP_FRANCE		= 25,
	MASK_ID_WORLDCUP_GERMANY	= 26,
	MASK_ID_WORLDCUP_ITALIA		= 27,
	MASK_ID_WORLDCUP_JAPAN		= 28,
	MASK_ID_WORLDCUP_S_AFRICA	= 29,
	MASK_ID_WORLDCUP_S_KOREA	= 30,
	MASK_ID_WORLDCUP_SPAIN		= 31,

	MASK_ID_TIGER		= 32,
	MASK_ID_JESTER_BW		= 33,
	MASK_ID_WRESTLING		= 34,

	MASK_ID_RUSSIA          = 35,
	MASK_ID_UKRAINE			= 36,
	MASK_ID_BELORUS			= 37,
	MASK_ID_KAZAHSTAN		= 38,	

	MASK_ID_RED_DEVIL		= 39,		// 레드 데빌

	MASK_ID_TURKEY_FC_BESIKTAS = 40,		// 터키 축구 클럽 베식타스
	MASK_ID_TURKEY_FC_BURSAPOR = 41,		// 터키 축구 클럽 부르사스포르
	MASK_ID_TURKEY_FC_FENERBAHCE = 42,		// 터키 축구 클럽 페네르바체
	MASK_ID_TURKEY_FC_GALATASARAY = 43,		// 터키 축구 클럽 갈라타사라이
	MASK_ID_TURKEY_FC_TRABZONSPOR = 44,		// 터키 축구 클럽 트라브존스포르

	MASK_ID_FRAIL_SKULL		= 45,

	MASK_ID_LATIN_AMERICA_BAHAMAS	= 46,	// 중남미 마스크 (바하마)
	MASK_ID_LATIN_AMERICA_BOLIVIA	= 47,	// 중남미 마스크 (볼리비아)
	MASK_ID_LATIN_AMERICA_CAYMAN	= 48,	// 중남미 마스크 (케이먼 제도)
	MASK_ID_LATIN_AMERICA_CHILE		= 49,	// 중남미 마스크 (칠레)
	MASK_ID_LATIN_AMERICA_COLOMBIA	= 50,	// 중남미 마스크 (콜롬비아)
	MASK_ID_LATIN_AMERICA_COSTARICA	= 51,	// 중남미 마스크 (코스타리카)
	MASK_ID_LATIN_AMERICA_HONDURAS	= 52,	// 중남미 마스크 (온두라스)
	MASK_ID_LATIN_AMERICA_JAMAICA	= 53,	// 중남미 마스크 (자메이카)
	MASK_ID_LATIN_AMERICA_MEXICO	= 54,	// 중남미 마스크 (멕시코)
	MASK_ID_LATIN_AMERICA_NICARAGUA	= 55,	// 중남미 마스크 (니카라과)
	MASK_ID_LATIN_AMERICA_PANAMA	= 56,	// 중남미 마스크 (파나마)
	MASK_ID_LATIN_AMERICA_PARAGUAY	= 57,	// 중남미 마스크 (파라과이)
	MASK_ID_LATIN_AMERICA_DOMINICAN	= 58,	// 중남미 마스크 (도미니카 공화국)
	MASK_ID_LATIN_AMERICA_ECUADOR	= 59,	// 중남미 마스크 (에콰도르)
	MASK_ID_LATIN_AMERICA_FRENCH_GUYANA	= 60,	// 중남미 마스크 (가이아나 프랑스령)
	MASK_ID_LATIN_AMERICA_GUATEMALA	= 61,	// 중남미 마스크 (과테말라)
	MASK_ID_LATIN_AMERICA_GUYANA	= 62,	// 중남미 마스크 (가이아나)
	MASK_ID_LATIN_AMERICA_HAITI		= 63,	// 중남미 마스크 (아이티)
	MASK_ID_LATIN_AMERICA_PERU		= 64,	// 중남미 마스크 (페루)
	MASK_ID_LATIN_AMERICA_PUERTORICO	= 65,	// 중남미 마스크 (푸에르토리코)
	MASK_ID_LATIN_AMERICA_SURINAME	= 66,	// 중남미 마스크 (수리남)
	MASK_ID_LATIN_AMERICA_TRINIDAD	= 67,	// 중남미 마스크 (트리니다드토바고)
	MASK_ID_LATIN_AMERICA_URUGUAY	= 68,	// 중남미 마스크 (우루과이)
	MASK_ID_LATIN_AMERICA_VENEZUELA	= 69,	// 중남미 마스크 (베네수엘라)
	MASK_ID_LATIN_AMERICA_ARGENTINA	= 70,	// 중남미 마스크 (아르헨티나)

	MASK_ID_DINOSKULL		= 71,		// 공룡 해골 마스크

	MASK_ID_FLAG_NA_RAPTR	= 72,		//북미 Raptr 마스크
	MASK_ID_FLAG_CANADA		= 73,		//카나다 국기 마스크
	MASK_ID_FLAG_UK			= 74,		//영국(United Kingdom) 국기 마스크
	MASK_ID_FLAG_USA		= 75,		//미국(United States America) 국기 마스크
	MASK_ID_FLAG_INDONESIA	= 76,		//인도네시아(Indonesia) 국기 마스크

	MASK_ID_THAI_PBTN		= 77,		//태국 대회용 마스크
	MASK_ID_CHINA_RED		= 78,		//중국 변겸 마스크
	MASK_ID_REINFORCE_BLACK = 79,		//터키 강화 블랙 마스크

	MASK_ID_ITALY_AZZURRO_E_BIANCO		= 80,	// 이태리 프로축구
	MASK_ID_ITALY_AZZURRO_E_ROSSA		= 81,	// 이태리 프로축구
	MASK_ID_ITALY_BIANCA_E_NERA			= 82,	// 이태리 프로축구
	MASK_ID_ITALY_BIANCO_E_AZZURRO		= 83,	// 이태리 프로축구
	MASK_ID_ITALY_BLU_E_NERA			= 84,	// 이태리 프로축구
	MASK_ID_ITALY_BLU_E_ROSSA			= 85,	// 이태리 프로축구
	MASK_ID_ITALY_CROCE_NERA			= 86,	// 이태리 프로축구
	MASK_ID_ITALY_GIALLO_E_AMARANTO		= 87,	// 이태리 프로축구
	MASK_ID_ITALY_NERA_BIANCA_E_ROSSA	= 88,	// 이태리 프로축구
	MASK_ID_ITALY_NERA_E_BIANCA			= 89,	// 이태리 프로축구
	MASK_ID_ITALY_NERA_E_BLU			= 90,	// 이태리 프로축구
	MASK_ID_ITALY_NERA_E_ROSA			= 91,	// 이태리 프로축구
	MASK_ID_ITALY_NERA_GIALLA_E_BIANCA	= 92,	// 이태리 프로축구
	MASK_ID_ITALY_ROSSO_E_GIALLA		= 93,	// 이태리 프로축구
	MASK_ID_ITALY_STELLA				= 94,	// 이태리 프로축구
	MASK_ID_ITALY_VIOLA					= 95,	// 이태리 프로축구

	MASK_ID_BRAZIL_TROLL				= 96,	// 브라질 troll
	MASK_ID_BRAZIL_DETERMINADO			= 97,	// 브라질 determanido
	MASK_ID_BRAZIL_FUUU					= 98,

	MASK_ID_DINO_TREX					= 99,	//인도네시아 공룡 마스크

	MASK_ID_TROJAN_MASK					= 100,	//북미 Trojan 마스크
	MASK_ID_FRAIL_SKULL_GOLD			= 101,	//인도네시아 Fail Skull Gold 마스크

	MASK_ID_TURKEY_FC_BESIKTAS_V2		= 102,		// 터키 강화 축구 클럽 베식타스
	MASK_ID_TURKEY_FC_BURSAPOR_V2		= 103,		// 터키 강화 축구 클럽 부르사스포르
	MASK_ID_TURKEY_FC_FENERBAHCE_V2		= 104,		// 터키 강화 축구 클럽 페네르바체
	MASK_ID_TURKEY_FC_GALATASARAY_V2	= 105,		// 터키 강화 축구 클럽 갈라타사라이
	MASK_ID_TURKEY_FC_TRABZONSPOR_V2	= 106,		// 터키 강화 축구 클럽 트라브존스포르

	MASK_ID_PBIC12						= 107,		//PBIC'12 마스크

	// 대만 이벤트용 마스크
	MASK_ID_RUSSIAN_BLUE_TIGER_EV		= 108,
	MASK_ID_KOREA_MARINE_EV				= 109,
	MASK_ID_FRANCE_DESERT_EV			= 110,
	MASK_ID_BLACK_EV					= 111,
	MASK_ID_WHITE_EV					= 112,
		
	MASK_ID_GATOTKACA					= 113,

	MASK_ID_RUSSIAN_BLUE_100M			= 114,		//인도네시아 새로운 아이템 ID
	MASK_ID_ROK_MARINE_100M				= 115,
	MASK_ID_FRENCH_DESERT_100M			= 116,

	MASK_ID_GATOTKACA_GOLD				= 117,		//gatotkaca gold
	MASK_ID_PBSC						= 118,		// PBSC 마스크
	MASK_ID_PBTN_2012					= 119,

	MAX_MASK_ID,
};

enum EVENT_EQUIP_ID
{
	EVENT_EQUIP_UNKNOWN	= 0,

	EVENT_EQUIP_SANTA_HAT		= 1,
	EVENT_EQUIP_CHINESE_HAT		= 2,
	EVENT_EQUIP_HAIRBAND		= 3,
	EVENT_EQUIP_COWBOY_HAT		= 4,
	EVENT_EQUIP_HAIRBAND_GW		= 5,

	MAX_EVENT_EQUIP_ID,
};

enum WEAPON_INDICATOR_TYPE
{
	WEAPON_INDICATOR_UNKNOWN = 0,

	WEAPON_INDICATOR_GRENADE,
	WEAPON_INDICATOR_C5,
};

// 임무카드 아이템 아이디 2번째 값
enum QUESTCARD_USAGE_TYPE
{
	QUESTCARD_USAGE_UNKNOWN	= 0,		// 기본 
};


// 임무카드 아이템 아이디 3번째 값(카드종류)은 ENUM_QUEST_CARDSET_TYPE 의 enum 값을 동일하게 사용합니다.

// 임무카드 아이템 아이디 4번째 값
enum QUESTCARD_NUMBER
{
	QUESTCARD_NUMBER_UNKNOWN	= 0,	// 기본 
};

#pragma pack( push, 1 )
typedef struct
{
	INT8	_nDiviceType;
	UINT32	_nState;
}GAME_KEY_STRUCT;
#pragma pack( pop )



//
// Title System
////////////////////////////////////////////////////////////////////////////////

// 호칭 설정
#define MAX_USER_TITLE					256
#define MAX_EQUIP_USER_TITLE			3
#define MAX_DEFAULT_USER_TITLE_COUNT	44	// 기본 호칭 갯수

// 호칭 계열
enum USER_TITLE_CATEGORY
{
	UTC_NOTHING = 0,
	UTC_ALL,
	UTC_ASSAULT,
	UTC_RIFLE,
	UTC_SMG,
	UTC_KNIFE,
	UTC_HANDGUN,
	UTC_SHOTGUN,
	UTC_COMPOSITION
};

// 호칭 조건 테이블용 구조체 그룹
enum USER_TITLE_RIBBON_TYPE
{
	UTR_NOTHING = 0,
	UTR_RIBBON,
	UTR_ENSIGN,
	UTR_MEDAL,
	UTR_MASTER
};

typedef struct __tag_UserTitleContext
{
	INT32 _category;
	INT32 _weapon;
	INT32 _character;	
} USER_TITLE_CONTEXT;

typedef struct __tag_UserTitleRibbon
{
	UINT8	_type;
	//UINT8	_id;
	UINT8	_count;
} USER_TITLE_RIBBON;

#define	USER_TITLE_REQUIREMENT_COUNT		3
#define USER_TITLE_PRIORITY_COUNT			2

typedef struct __tag_UserTitleRequirement
{
	USER_TITLE_RIBBON _ribbon[USER_TITLE_REQUIREMENT_COUNT];
	UINT8	_userTitleId[USER_TITLE_PRIORITY_COUNT];
	UINT8	_rank;
} USER_TITLE_REQUIREMENT;

typedef struct __tag_UserTitleEffect
{	
	UINT8	_Id;
	char	_Text[128];
} USER_TITLE_EFFECT;

#define RAPTOR_TOTAL_CNT	9
#define DINO_TOTAL_CNT		(RAPTOR_TOTAL_CNT + 1)
#define UNION_TOTAL_CNT		(SLOT_MAX_COUNT - DINO_TOTAL_CNT)

typedef struct __tag_Dino_Room_Info
{
	UINT8	_uiTRexSlotIdx; 
	UINT8	_uiRaptorSlotIdx[RAPTOR_TOTAL_CNT]; 

	bool	IsDinoIdx(UINT8 idx) 
	{
		if(_uiTRexSlotIdx == idx) {
			return true;
		}
		
		INT32 i = 0;
		for(i=0; i<RAPTOR_TOTAL_CNT; i++) {
			if(_uiRaptorSlotIdx[i] == idx) {
				return true;
			}
		}

		return false;
	}
}DINO_ROOM_INFO; 

extern const USER_TITLE_CONTEXT g_UserTitleContext[MAX_USER_TITLE];
extern const USER_TITLE_REQUIREMENT g_UserTitleRequirement[MAX_USER_TITLE];


// 어뷰징 제재 레벨 
enum ABUSING_LEVEL
{
	ABUSING_LEVEL_NONE = 0,		// 어뷰징 X
	ABUSING_LEVEL_NORMAL,		// 노말 제재
	ABUSING_LEVEL_STRONG,		// 강화 제재
	ABUSING_LEVEL_STRONGEST,	// 초강화 제재
};


////////////////////////////////////////////////////////////////////////////////
// 두 종류의 #define를 같이 고쳐야 함. - 박명진 2008. 11. 14
#define ROOM_INFO_FLAG_INTER_ENTER			0x80
#define ROOM_INFO_FLAG_OBSERVER				0x40
#define ROOM_INFO_FLAG_TEAM_BALANCE			0x20
#define ROOM_INFO_FLAG_REAL_IP				0x10
#define ROOM_INFO_FLAG_OBSERVER_MODE		0x08
#define ROOM_INFO_FLAG_PRIVATE_MODE			0x04
#define ROOM_INFO_FLAG_RANDOM_MODE			0x02

#define GET_ROOM_INFO_INTERENTER( state )		((state>>7) & 0x00000001)
#define GET_ROOM_INFO_OBSERVER( state )			((state>>6) & 0x00000001)
#define GET_ROOM_INFO_TEAMBALANCE( state )		((state>>5) & 0x00000001)
#define GET_ROOM_INFO_REAL_IP( state )			((state>>4) & 0x00000001)
#define GET_ROOM_INFO_OBSERVER_MODE( state )	((state>>3) & 0x00000001)
#define GET_ROOM_INFO_PRIVATE_MODE( state )		((state>>2) & 0x00000001)
#define GET_ROOM_INFO_RANDOM_MODE( state )		((state>>1) & 0x00000001)

#define SET_ROOM_INFO_INTERENTER( state, flag )		((state & 0x7F) | ((flag & 0x00000001) << 7))
#define SET_ROOM_INFO_OBSERVER( state, flag )			((state & 0xBF) | ((flag & 0x00000001) << 6))
#define SET_ROOM_INFO_TEAMBALANCE( state, flag )		((state & 0xDF) | ((flag & 0x00000001) << 5))
#define SET_ROOM_INFO_REAL_IP( state, flag )			((state & 0xEF) | ((flag & 0x00000001) << 4))
#define SET_ROOM_INFO_OBSERVER_MODE( state, flag )	((state & 0xF7) | ((flag & 0x00000001) << 3))
#define SET_ROOM_INFO_PRIVATE_MODE( state, flag )		((state & 0xFB) | ((flag & 0x00000001) << 2))
#define SET_ROOM_INFO_RANDOM_MODE( state, flag )		((state & 0xFD) | ((flag & 0x00000001) << 1))

#define GET_ROOM_INFO_WEAPON_KNUCKLE( state )	((state>>7)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_SHOTGUN( state )	((state>>6)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_SNIPER( state )	((state>>5)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_RPG7( state )		((state>>4)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_00( state )		((state>>3) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_01( state )		((state>>2) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_02( state )		((state>>1) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_03( state )		((state   ) & 0x00000001)

// 여기까지 (두 종류의 #define를 같이 고쳐야 함. - 박명진 2008. 11. 14)

//////////////////////////////////////////////////////////////////////////////////////////////
//										Structure											//
//////////////////////////////////////////////////////////////////////////////////////////////
#define RELAY_ENCRYPTKEY_DEFAULT		0xFFFFFFFF
#define DSERVER_ENCRYPTKEY_DEFAULT		0xFFFFFFFF // duplicated!

#pragma pack( push, 1 )

typedef struct _GameServerInfo
{
	INT32	_IsActive;
	UINT32	_OutIP;
	UINT16	_OutPort;
	UINT8	_AllType; 
	UINT16	_ui16MaxUserCount;
	INT32	_Arg0;
}GAME_SERVER_INFO;

//방 정보 
typedef struct __tagLobbyUserInfoList
{
	INT32	_Session;							// DB ID
	UINT32	_ClanMark;							// Clan Mark
	char	_ClanName[ NET_CLAN_NAME_SIZE ];	// 클랜 이름
	UINT8	_Rank;								// 랭킹
	UINT8	_eSportID;							// E-Sport ID
	char	_Nick[ NET_NICK_NAME_SIZE ];		// 닉네임
	UINT8	_NickColor;


	void	Reset() {
		//_Idx		= 0;
		_Session	= 0;
		_ClanMark	= 0xFFFFFFFF;
		_ClanName[0] = '\0';
		_Rank		= 0;
		_eSportID	= 0;
		_Nick[ 0 ]	= '\0';
		_NickColor	= 0;
	}
}LOBBY_USER_INFO_LIST;

typedef struct __tag_RelayRoomInfo
{
	UINT8	_cRelay;
	UINT8	_cGame;
	UINT8	_cChannel;
	UINT16	_sRoom;
	void	Reset()
	{
		_cRelay		= 0;
		_cGame		= 0;
		_cChannel	= 0;
		_sRoom		= 0;
	}
	void	Copy( __tag_RelayRoomInfo* pDes )
	{
		_cRelay		= pDes->_cRelay;
		_cGame		= pDes->_cGame;
		_cChannel	= pDes->_cChannel;
		_sRoom		= pDes->_sRoom;
	}

} RELAY_ROOM_INFO;

typedef struct __tag_BattleRoomInfo
{
	UINT8	_cBattle;
	UINT8	_cGame;
	UINT8	_cChannel;
	UINT16	_sRoom;
	void	Reset()
	{
		_cBattle	= 0;
		_cGame		= 0;
		_cChannel	= 0;
		_sRoom		= 0;
	}
	void	Copy( __tag_BattleRoomInfo * pDes )
	{
		_cBattle	= pDes->_cBattle;
		_cGame		= pDes->_cGame;
		_cChannel	= pDes->_cChannel;
		_sRoom		= pDes->_sRoom;
	}

} BATTLE_ROOM_INFO;

typedef struct __tag_DSERVERRoomInfo // duplicated! 
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
	void	Copy( __tag_DSERVERRoomInfo* pDes )
	{
		_cBattle	= pDes->_cBattle;
		_cGame		= pDes->_cGame;
		_cChannel	= pDes->_cChannel;
		_sRoom		= pDes->_sRoom;
	}
} DSERVER_ROOM_INFO;

#define		INVALID_WAREDB_INDEX			0xFFFFFFFF

#define ITEM_ATTR_TYPE_BUY			1			//구입만한 아이템(기간제)
#define ITEM_ATTR_TYPE_AUTH			2			//인증한 아이템
#define ITEM_ATTR_TYPE_BASIC		3			//기본 지급 아이템
#define ITEM_ATTR_TYPE_PC_1			4			//PC방 아이템 1
#define ITEM_ATTR_TYPE_PC_2			5			//PC방 아이템 2
#define ITEM_ATTR_TYPE_SPECIAL		6			//특별 이벤트

#pragma pack( pop )

#pragma pack( push, 1 )

// 삭제요망
enum NAT_STATE
{
	NAT_STATE_WITHOUT						= 0,
	NAT_STATE_FULL_CONE_NAT,
	NAT_STATE_RESTRICTED_CONE_NAT,
	NAT_STATE_PORT_RESTRICTED_CONE_NAT,
	NAT_STATE_SYMMETRIC_NAT,
};
// 삭제요망

enum SERVER_UDP_STATE
{
	SERVER_UDP_STATE_NONE				= 0,
	SERVER_UDP_STATE_DSERVER, //SERVER_UDP_STATE_RENDEZVOUS,
	SERVER_UDP_STATE_CLIENT,
	SERVER_UDP_STATE_RELAY,
	SERVER_UDP_STATE_RELAYCLIENT,
};

typedef struct __NetUdpInfo
{
	UINT32	m_nRealIP;			// 공인 IP
	UINT16	m_nRealPort;
	UINT32	m_nLocalIP;			// 사설 IP
	UINT16	m_nLocalPort;
	UINT8	m_nNATState;

	void Reset(void)
	{
		m_nRealIP		= 0;
		m_nRealPort		= 0;
		m_nLocalIP		= 0;
		m_nLocalPort	= 0;
		m_nNATState		= 0;
	}

} NET_UDP_INFO;


//////////////////////////////////////////////////////////////////
//						Main Server to client					//
//////////////////////////////////////////////////////////////////
typedef struct __ChatBasic
{
protected:
	char	_String[MAX_CHATTING_COUNT];

public:
	UINT16 _Type;

	void	Reset() { _Type = 0; _String[0] = 0; }

	char *	getString(void)					{ return _String; }
	void	setString( const char * pszMsg)
	{
		I3ASSERT( pszMsg != NULL);
		I3ASSERT( strlen( pszMsg) < MAX_CHATTING_COUNT);

		//i3String::Copy( _String, pszMsg, MAX_CHATTING_COUNT);
		strcpy_s( _String, MAX_CHATTING_COUNT, pszMsg );
	}
}CHAT_INFO;



//방 정보 
typedef struct __RoomInfoBasic
{
	INT32	_Idx;
	char	_Title[NET_ROOM_NAME_SIZE];		// 타이틀명
	UINT32	_StageID;						//
	UINT8	_State;							// 상태	
	UINT8	_PersonNow;						// 들어와 있는 사람수
#ifdef DEF_OBSERVER_MODE
	UINT8	_PersonObserver;				// 옵저버로 들어와 있는 사람수
	UINT8	_PersonHidden;					// 히든으로 들어와 있는 사람수	
	UINT8	_PersonPlayerMax;				// 플레이어인 전체 사람수 	
	UINT8	_PersonObserverMax;				// 옵저버 모드인 전체 사람수 	
	UINT8	_PersonHiddenMax;				// 히든인 전체 사람수 	
#else
	UINT8	_PersonPlayerMax;				// 플레이어인 전체 사람수 	
#endif
	UINT8	_Ping;							// 핑	
	UINT8	_WeaponFlag;					// 0x01111111, 0, 샷건, 저격, RPG7, 주, 보조, 칼, 투척
	UINT8	_InfoFlag;						// 0x11111000, 난입제한, 관전제한, 팀벨런스, RealIP,옵저버모드
	UINT8	_StageMask;						// 스테이지 모드 ( STAGE_MASK_TYPE )
	void	Reset() {
		_Title[0]	= 0;
		_Idx		= 0;
		_StageID	= 0;
#ifdef DEF_OBSERVER_MODE
		_StageMask = _State = _PersonNow = _PersonObserver = _PersonHidden = _PersonPlayerMax = _PersonObserverMax = _PersonHiddenMax = _Ping = _InfoFlag = 0;
#else
		_StageMask = _State = _PersonNow = _PersonPlayerMax = _Ping = _InfoFlag = 0;
#endif
		_WeaponFlag	= 0x0F;
	}
}ROOM_INFO_BASIC;	

typedef struct __RoomInfoADD
{
//protected:
#ifdef USE_COLOR_NICK2
	UINT8	_NicklColor;					// 방장 Nick Color
#endif
	char	_Nick[NET_NICK_NAME_SIZE];		// 방장 Nick	

public:
	UINT8	_SubType;						// Time, kill	
	UINT8	_NowRoundCount;					// 라운드 카운트 
	UINT16	_NowIngTime;					// 타임 카운트(진행시간)	
	UINT8	_LockInterEnter;				// 1 Lock, 0 UnLock	
	UINT8	_LockObserver;					// 0x00001111,	(상대진영HP,상대진영,자유관전,3인칭자유)
	UINT8	_LockTeamBalance;				// 1 Lock, 0 UnLock (default unlock)
	UINT8	_AIState;						// 1 DieHard 0 Original (default Original)

	char *		getNick(void)				{ return _Nick; }
	void		setNick( const char * pszName)
	{
		I3ASSERT( pszName != NULL);
		I3ASSERT( strlen( pszName) < NET_NICK_NAME_SIZE);

		//i3String::Copy( _Nick, pszName, NET_NICK_NAME_SIZE );
		strcpy_s( _Nick, NET_NICK_NAME_SIZE, pszName );
	}

	void		Reset(void)
	{
#ifdef USE_COLOR_NICK2
		_NicklColor			= 0;
#endif
		_Nick[0]			= '\0';
		_SubType			= 0x00;
		_NowRoundCount		= 0x00;
		_NowIngTime			= 0;
		_LockInterEnter		= 0;
		_LockObserver		= 0;
		_LockTeamBalance	= 0;		
		_AIState			= 0;
	}
}ROOM_INFO_ADD;	

typedef struct __RoomInfoAll
{
	ROOM_INFO_BASIC _Basic;  
	ROOM_INFO_ADD	_Add;
}ROOM_INFO_ALL;	

//방 생성 인자
typedef struct __RoomInfoCreate
{
	ROOM_INFO_BASIC		RoomInfoBasic;
	ROOM_INFO_ADD		RoomInfoAdd;
	char				szRoomPw[NET_ROOM_PW];
	INT32				nChannelNum;
	UINT32				wMatchTeamIdx;
	UINT32				wMatchReqTeamIdx;
	INT8				difficultyLevel;
	INT8				enemyAiCount;
	void	Reset() {
		RoomInfoBasic.Reset();
		RoomInfoAdd.Reset();
		szRoomPw[0]			= 0;
		nChannelNum			= 0;
		wMatchTeamIdx		= 0;
		wMatchReqTeamIdx	= 0;
		difficultyLevel		= TRAINING_DIFFICULTY_LEVEL_NONE;
		enemyAiCount = 0;
	}
}ROOM_INFO_CREATE;	

typedef struct __RoomInfoJoin
{
	char	szRoomPw[ NET_ROOM_PW ];
	UINT32	wMatchTeam;	
	UINT8	cForce;
	BOOL	bMatch;

	void	Reset() {
		szRoomPw[0]	= 0;
		wMatchTeam	= 0;
		cForce		= 0;
		bMatch		= FALSE;
	}
}ROOM_INFO_JOIN;	

struct USER_INFO_BASIC
{
	// User Nick
	char	_nick[NET_NICK_NAME_SIZE];

	// 경험치
	UINT32	_exp; 

	// 랭크??
	UINT32	_rank;
	UINT32	_keep_rank;
 

	// Point
	UINT32	_point; 

	// Cash
	UINT32	_cash; 

	// Clan Index
	UINT32	_clanidx; 

	// ??
	UINT32  _clanstate;
	
	UINT32	_NCCoin;			//한국빌드용 해피코인 및 엔시코인 
	UINT32	_HappyCoin;			//
	UINT8	_pcCafe;
	UINT8	_eSportID;

	// Nick Color
	UINT8	_nickcolor;

	// clan
	char	_clanName[NET_CLAN_NAME_SIZE];
	UINT8	_clanExpRank; 
	UINT8	_clanPerRank; 
	UINT32	_clanMark; 
	UINT8	_clanNamecolor;
	UINT32	_ui32AbusingCount;
	UINT8	_ui8TutorialIng;	// 튜토리얼 진행여부 - 0x00 : 튜토리얼 미완료 0x01: 튜토리얼 완료
	UINT32	_ui32ActualBattleTime;
	UINT32	_ui32LastBuyCash;
	
	void	Reset() 
	{
		_nick[0]		= '\0';
		_clanName[0]	= '\0';
		_pcCafe			= PC_NOT_CAFE;
		_eSportID		= ESPORT_NOT_ID;
		_clanMark		= CLAN_MARK_DEFAULT;			// 클랜 마크 초기화는 0xFFFFFFF이기 때문에 세팅해줍니다.
		_exp = _rank = _point = _cash = _clanidx= _NCCoin=_HappyCoin	= _clanstate = 0;
		_nickcolor = _clanExpRank = _clanPerRank = _clanNamecolor = 0;
		_keep_rank = 0;		
		_ui32AbusingCount = 0;
		_ui8TutorialIng = 0;
		_ui32ActualBattleTime = 0;
		_ui32LastBuyCash = 0;
	}
};

struct USER_INFO_BASIC2
{
	char			_strNick[NET_NICK_NAME_SIZE];
	UINT32			_ui32Rank; 
	UINT32			_ui32Point;	
	UINT32			_ui32Exp; 
	UINT32			_keep_rank;	
	UINT32			_ui32AbusingCount;
	UINT8			_ui8NickColor;
	UINT8			_ui8TutorialIng;
	UINT32			_ui32TotalBattleTime;
	UINT32			_ui32LastBuyCash;
};


typedef struct __UserInfoClan
{
	UINT32			_ui32ClanIdx; 
	UINT32			_ui32ClanState;
	UINT8			_ui8ExpRank;
	UINT8			_ui8PerRank;
	UINT32			_ui32Mark;
	char			_strClanName[NET_CLAN_NAME_SIZE] ;
}USER_INFO_CLAN;

#ifdef USE_MODE_RECORD
typedef struct __UserInfoBasic3
{
	char			_strNick[NET_NICK_NAME_SIZE];
	char			_clanName[NET_CLAN_NAME_SIZE];
	UINT8			_EquipUserTitle[MAX_EQUIP_USER_TITLE];	
	UINT32			_ui32ClanIdx; 
	UINT32			_ui32Rank; 
	UINT32			_ui32Point;	
	UINT32			_ui32cash; 		
}USER_INFO_BASIC3;

typedef struct __UserInfoModeRecord
{
	UINT32	_match;
	UINT32	_win;
	UINT32	_lose;
	UINT32	_draw;
	UINT32	_killcount;
	UINT32	_headshot;
	UINT32	_death;
	UINT32	_dmatch;
	UINT32	_dkillcount;
	UINT32	_shoot_count;
	UINT32	_hit_count;

	void	Reset() 
	{
		_match = _win = _lose = _draw = _killcount = _headshot = _death = _dmatch = _dkillcount = _shoot_count = _hit_count = 0;			
	}
}USER_INFO_MODE_RECORD;

typedef struct __UserInfoBasicRecord
{
	UINT32	_match; 
	UINT32	_win; 
	UINT32	_lose; 
	UINT32	_draw;	
	UINT32	_killcount;
	UINT32	_headshot;
	UINT32	_death;		
	UINT32	_dmatch;
	UINT32	_dkillcount;
	UINT32	_dis; 

	UINT32	_user_ranking;
	UINT32	_shoot_count;
	UINT32	_hit_count;

	void	Reset() 
	{
		_match = _win = _lose = _draw = _killcount = _headshot = _death = _dmatch = _dkillcount = _dis = 0;		
		_user_ranking = _shoot_count = _hit_count = 0;
	}
}USER_INFO_BASIC_RECORD;

typedef struct __UserInfoRecord
{
	USER_INFO_BASIC_RECORD		_total_record;
	USER_INFO_BASIC_RECORD		_season_record;

	void	Reset() 
	{
		_total_record.Reset();
		_season_record.Reset();
	}
}USER_INFO_RECORD;

typedef struct __UserInfoDetailRecord
{	
	USER_INFO_MODE_RECORD	_season_mode_record[MODE_MAX];
	USER_INFO_MODE_RECORD	_total_mode_record[MODE_MAX];

	void	Reset() 
	{
		int Loop = 0;
		for(Loop = 0; Loop <MODE_MAX; Loop++)
		{
			_season_mode_record[Loop].Reset();
			_total_mode_record[Loop].Reset();
		}
	}
}USER_INFO_DETAIL_RECORD;

typedef struct __UserInfoAllRecord
{
	USER_INFO_RECORD			_basic_record;
	USER_INFO_DETAIL_RECORD		_detail_record;		

	void	Reset() 
	{
		_basic_record.Reset();
		_detail_record.Reset();
	}
}USER_INFO_ALL_RECORD;
#else
typedef struct __UserInfoRecord
{
	UINT32	_match; 
	UINT32	_win; 
	UINT32	_lose; 
	UINT32	_draw;	
	UINT32	_killcount;
	UINT32	_headshot;
	UINT32	_death;		
	UINT32	_dmatch;
	UINT32	_dkillcount;
	UINT32	_dis; 

	UINT32	_smatch;
	UINT32	_swin;	
	UINT32	_slose;	
	UINT32	_sdraw;	
	UINT32	_skillcount;
	UINT32	_sheadshot;
	UINT32	_sdeath;
	UINT32  _sdmatch;
	UINT32	_sdkillcount;
	UINT32	_sdis;

	void	Reset() {
		_match = _win = _lose = _draw = _killcount = _headshot = _death = _smatch = _swin = _slose = _sdraw = _skillcount = _sheadshot = _sdeath = 0;
		_dmatch = _dkillcount = _sdmatch = _sdkillcount = _dis = _sdis = 0;
	}
}USER_INFO_RECORD;
#endif


struct USER_INFO_DAILY
{
	UINT32	_match;
	UINT32	_win;
	UINT32	_lose;
	UINT32	_draw;
	UINT32	_killcount;
	UINT32	_headshot;
	UINT32	_death;
	UINT32	_exp;
	UINT32  _point;
	INT32	_playtime;
	BOOL	_bGetItem;
	UINT32	_EventDate;
};

typedef struct __UserInfoQuest
{
	UINT32			_iQuestEventDate;
	UINT8			_pQuestCtxActive[MAX_CARDSET_PER_USER][MAX_QUEST_PER_CARDSET_NEW];
	QUESTING_INFO	_QuestIngInfo;
	UINT32			_pMiniature;
	UINT32			_pInsignia;
	UINT32			_pOrder;		
	UINT32			_pMaster;
	UINT64			_ui64GainUserTitle;
	UINT8			_EquipUserTitle[MAX_EQUIP_USER_TITLE];	
}USER_INFO_QUEST;

typedef struct __UserInfoEquipmentWeapon
{
	UINT32	_noprim;						// 주무기
	UINT32	_nosub;							// 보조무기
	UINT32	_nomelee;						// 근접무기
	UINT32	_nothrow;						// 수류탄류	( 수류탄 )
	UINT32	_noitem;						// 던지는 무기 ( 플래쉬뱅 )

	void	Reset() { _noprim = _nosub = _nomelee = _nothrow = _noitem = 0; }
}USER_INFO_EQUIPMENT_WEAPON;

#define	CHECK_CHARA_EQUIP	4		// 인벤토리 아이템 체크시 확인할 개수 
class USER_INFO_EQUIPMENT_CHARA
{
private:
	UINT32	_noRed;							// 레드팀
	UINT32	_noBlue;						// 블루팀
	UINT32	_nohair;						// 헬멧
	UINT32	_noitem;						// 헬멧과 동시 장착 가능 아이템 ( 베레모 )
	UINT32	_noDino;						// 공룡

public:
	USER_INFO_EQUIPMENT_CHARA()
	{
		Reset();
	}

	UINT32	GetRedNum()			const	{ return _noRed; }	
	void	SetRedNum(UINT32 nRed)		{ _noRed = nRed; }	

	UINT32	GetBlueNum()		const	{ return _noBlue; }	
	void	SetBlueNum(UINT32 nBlue)	{ _noBlue = nBlue; }

	UINT32	GetHairNum()		const	{ return _nohair; }	
	void	SetHairNum(UINT32 nHair)	{ _nohair = nHair; }	

	UINT32	GetItemNum()		const	{ return _noitem; }	
	void	SetItemNum(UINT32 nItem)	{ _noitem = nItem; }

	UINT32	GetDinoNum()		const	{ return _noDino; }	
	void	SetDinoNum(UINT32 nDino)	{ _noDino = nDino; }

	void	Reset()	{ _noRed = _noBlue = _nohair = _noitem = _noDino = 0; }
	void	CopyTo( USER_INFO_EQUIPMENT_CHARA * pDest ) const
	{
		pDest->_noRed	= _noRed;
		pDest->_noBlue	= _noBlue;
		pDest->_nohair	= _nohair;
		pDest->_noitem	= _noitem;
		pDest->_noDino	= _noDino;
	}
	UINT32	GetCharaID( TEAM_TYPE Type ) const
	{
		return ( Type == TEAM_RED ) ? _noRed : _noBlue;
	}

	bool IsSame(USER_INFO_EQUIPMENT_CHARA& temp)
	{
		if(GetRedNum() == temp.GetRedNum() && GetBlueNum() == temp.GetBlueNum() &&
			GetHairNum() == temp.GetHairNum() && GetItemNum() == temp.GetItemNum()  && GetDinoNum() == temp.GetDinoNum())
		{
			return true;
		}

		return false;
	}

};

typedef struct __UserInfoInvItemData
{
	// 아이템
	UINT8	_ui8Item;									// 00011111	-- ...컬러닉,채팅,크로스,닉네임,위장

	// 아이템 리스트
	UINT32	_wRank;										// 위장 계급
	char	_cNick[NET_NICK_NAME_SIZE];					// 임시 닉네임	
	UINT8	_cColorCrossHair;							// 칼라 크로스헤어
	UINT8	_cColorChatting;							// 칼라 채팅
	
	void	Reset() 
	{
		_ui8Item			= 0;
		_wRank				= 0;
		_cNick[0]			= 0;
		_cColorCrossHair	= 0;
		_cColorChatting		= 0;
		//i3mem::FillZero( _wCustomCrossHair, sizeof( UINT32 ) * 32 * 32 );
	}
}USER_INFO_INVITEM_DATA;
/*
typedef struct __UserInfoBasicOriginal
{
	UINT32	_arg;
	char	_nick[NET_NICK_NAME_SIZE];
	UINT32	_rank;
	
	void	Reset() 
	{
		_arg		= 0;
		_nick[0]	= 0;
		_rank		= 0;
	}
}USER_INFO_ORIGINAL;
*/
struct ROOM_SLOT_CHANGE_INFO
{
	UINT8	ui8OldSlot;
	UINT8	ui8NewSlot;
	UINT8	ui8OldSlotState;
	UINT8	ui8NewSlotState;
};

typedef struct
{
	UINT32			m_GamePlayState;
	UINT32			m_InterfaceState;
	UINT32			m_GameSoundState;
	UINT32			m_GameSoundVol;
	UINT32			m_GameControlState;	
	UINT32			m_GameCommunityState;
	UINT32			m_GameMacroState;

	void Reset(void)
	{
		m_GamePlayState = 0;
		m_InterfaceState = 0;
		m_GameSoundState = 0;
		m_GameSoundVol = 0;
		m_GameControlState = 0;
		m_GameCommunityState = 0;
		m_GameMacroState = 0;
	}

}NET_ENV_SET;

// 캐쉬 아이템 슬롯에 담긴 정보
#define SLOT_ITEM_SHORT_RESPAWN_100				0x400000
#define SLOT_ITEM_SHORT_RESPAWN_50				0x200000
#define SLOT_ITEM_SHORT_RESPAWN_30				0x100000
#define SLOT_ITEM_SHORT_RESPAWN_20				0x80000
#define SLOT_ITEM_AMMO_UP						0x40000				// Extended Magazines
#define SLOT_ITEM_PICKUP_WEAPON					0x20000			 
#define SLOT_ITEM_VALENTINE_CHOCOLATE			0x10000				// 발렌타인 초코렛 (HP+10)
#define SLOT_ITEM_QUICK_CHANGE_WEAPON			0x8000
#define SLOT_ITEM_QUICK_CHANGE_MAGAZINE			0x4000
#define SLOT_ITEM_MEGA_HP10						0x2000
#define SLOT_ITEM_INCREASE_INVINCIBLE_TIME		0x1000
#define SLOT_ITEM_BULLET_PROOF_VEST				0x800
#define SLOT_ITEM_FULLMETALJACKETAMMO			0x400
#define SLOT_ITEM_HOLLOW_POINT_AMMO				0x200
#define SLOT_ITEM_C4_SPEED_KIT					0x100
#define SLOT_ITEM_INCREASE_GRENADE_SLOT			0x80
#define SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO	0x40
#define SLOT_ITEM_MEGA_HP5						0x20
#define SLOT_ITEM_BULLET_PROOF_VEST_PLUS		0x10
#define SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS		0x08
#define SLOT_ITEM_BULLET_PROOF_VEST_METAL		0x04
#define SLOT_ITEM_KETUPAT						0x02	
#define SLOT_ITEM_BULLET_PROOF_VEST_GM			0x01



// Score
typedef struct ALIGN4 __ScoreInfo
{
	UINT16		m_KillCount;
	UINT16		m_DeathCount; 

	void	Reset() { m_KillCount = m_DeathCount = 0; }
}SCORE_INFO; 

//슬롯 정보 
typedef struct ALIGN4 __SlotInfo
{	
	UINT8		_State;				//현재 상태	
	UINT8		_Rank;				//랭크 51단계.	( Level ); 
	UINT32		_clanidx;			//
	UINT32		_clanstate;			//
	UINT8		_clanExpRank;		//
	UINT32		_clanMark;			//
	UINT8		_pcCafe;			//
	UINT8		_eSportID;			//
	UINT32		_Item;				// 1111111111110000
									// SLOT_ITEM_QUICK_CHANGE_WEAPON
									// SLOT_ITEM_QUICK_CHANGE_MAGAZINE
									// SLOT_ITEM_MEGA_HP10
									// SLOT_ITEM_INCREASE_INVINCIBLE_TIME
									// SLOT_ITEM_BULLET_PROOF_VEST
									// SLOT_ITEM_FULLMETALJACKETAMMO
									// SLOT_ITEM_HOLLOW_POINT_AMMO
									// SLOT_ITEM_C4_SPEED_KIT
									// SLOT_ITEM_INCREASE_GRENADE_SLOT
									// SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO
									// SLOT_ITEM_MEGA_HP5
									// SLOT_ITEM_BULLET_PROOF_VEST_PLUS
	char		_ClanName[ NET_CLAN_NAME_SIZE ];	// 클랜 이름

	BOOL _bIsGmObserverMode;

	void Reset()
	{
		_State = _Rank = _clanExpRank = _pcCafe = _eSportID = 0;
		_clanidx = _clanstate = 0;
		_clanMark = 0xFFFFFFFF;
		_Item = 0;
		_ClanName[0] = '\0';
		_bIsGmObserverMode = FALSE;
	}
}SLOT_INFO;

// 훈련전 NPC 정보
typedef struct ALIGN4 __AiEntry
{
	INT32		_level;
	INT32		_number;
	INT8		_hp;
	REAL32		_respawnTime;

	void		Reset()
	{
		_level = 0;
		_number = 0;
		_hp = 0;
		_respawnTime = 0.0f;
	}
}AI_ENTRY;



typedef struct _CharaCommonInfo
{
	INT32		_Number;
	char		_Name[NET_NICK_NAME_SIZE];
	REAL32		_BackMoveSpeed; 
	REAL32		_DiagonalMoveSpeed; 
	REAL32		_FrontMoveSpeed; 
	INT16		_Hp; 
	REAL32		_JumpDelay; 
	REAL32		_JumpHeight; 
	REAL32		_JumpTime; 
	REAL32		_ReloadSpeed; 
	REAL32		_ScopeWaveRange; 
	REAL32		_ScopeWaveVelocity; 
	REAL32		_ShoulderSpeed; 
	REAL32		_SideMoveSpeed; 
	REAL32		_WeaponSwap;

	void	Reset() {
		_Number = 0;
		_Name[0] = 0;
		_BackMoveSpeed = _DiagonalMoveSpeed = _FrontMoveSpeed = 0.f;
		_Hp = 0;
		_JumpDelay = _JumpHeight = _JumpTime = _ReloadSpeed = _ScopeWaveRange = _ScopeWaveVelocity = _ShoulderSpeed = _SideMoveSpeed = _WeaponSwap = 0.f;
	}
}CHARA_COMMON_INFO; 

// 초대하기 목록
typedef struct _UserInfoInvite
{
	UINT32	_idxSession;					// User SessionIdx
	UINT32	_rank;							// 추후 UINT8로 수정
	char	_nick[NET_NICK_NAME_SIZE];	
#ifdef USE_COLOR_NICK2
	UINT8	_nickColor;
#endif

	void Reset(void)
	{
		_idxSession = UINT_MAX;
		_rank		= 0;
		_nick[0]	= '\0';
#ifdef USE_COLOR_NICK2
		_nickColor	= 0;
#endif
	}

} USER_INFO_INVITE;

typedef struct _InviteInfo
{
	char	_nick[NET_NICK_NAME_SIZE];
	INT32	_idxRoom;
	TUID	_iUID;
	UINT8	_pw[NET_ROOM_PW];	
} INVITE_INFO;

typedef struct _DeathInfo_Header
{										// 0xF (High-BitOR)			F(Low-KillIdx)
	UINT8	_nKillType;					// 0x1(자폭) 0x2(미정)		0(Default), 1(Piercing), 2(Mass)
	UINT8	_nCount;					// deafult이면 1
	UINT8	_nUserIdx;					// killer user SlotIdx
	UINT32	_WeaponInfo;				// aabbcccddd - aa:Class, bb:USAGE, ccc:TYPE(Weapon Class), ddd:ItemNumber
	REAL32	_KillPosition[3];			// 2Byte position( x, y, z )
	UINT8	_nByObjectIdx;				// 오브젝트에 의한 사망시 오브젝트 인덱스 ( _nKillType == CHARA_KILL_BYOBJECT)
} DEATH_INFO_HEADER;

#define GET_DEATHINFO_IDX_TYPE( Type )				(Type & 0x0000000F)
#define GET_DEATHINFO_DEATH_TYPE( Type )			((Type & 0x000000F0) >> 4)
#define GET_DEATHINFO_MESSAGE_TYPE( Type )			((Type & 0x00FFFF00) >> 8)
#define GET_DEATHINFO_ENEMY_TYPE( Type )			((Type & 0x0F000000) >> 24) 
#define GET_DEATHINFO_MY_TYPE( Type )				((Type & 0xF0000000) >> 28)

#define SET_DEATHINFO_IDX_TYPE( Type, State )		( Type | (State & 0x0000000F) )
#define SET_DEATHINFO_DEATH_TYPE( Type, State )		( Type | ((State << 4)&0x000000F0) )
#define SET_DEATHINFO_MESSAGE_TYPE( Type, State )	( Type | ((State << 8)&0x00FFFF00) )
#define SET_DEATHINFO_ENEMY_TYPE( Type, State )		( Type | ((State << 24)&0x0F000000) )
#define SET_DEATHINFO_MY_TYPE( Type, State )		( Type | ((State << 28)&0xF0000000) )

#define __MISSION_CARD_SERVER

typedef struct _DeathInfo_CHARA
{
	UINT8	_ui8Weapon;					// Weapon
	UINT32	_DeathInfo;					// MyState(0xF0000000), EnemyState(0x0F000000), Message(0x00FFFF00), DeathType(0x000000F0), Index(0x0000000F)
	REAL32	_DeathPosition[3];			// 2Byte position( x, y, z )
} DEATH_INFO_CHARA;

// 초대자 정보 Auth->GS, GS->Client 
typedef struct _ServerInviterInfo
{
	TUID _uid;
	INT32 _sesseionIdx;
	INT32 _serverIdx;
	INT32 _channelIdx;
	INT32 _roomIdx;
	char _nick[NET_NICK_NAME_SIZE];
} SERVER_INVITER_INFO;

#pragma pack( pop )

// 커뮤니티
#define MAKE_FRIEND_STATE(state,server,channel,room)	((((state) & 0x0000000F) << 28) | (((server) & 0x000000FF) << 20) | (((channel) & 0x000000FF) << 12) | ((room) & 0x00000FFF))
#define MAKE_FRIEND_MATCH_STATE(match,state,server,channel,room)	(((UINT64)(match) & 0x00000000FF) << 32) | ((((state) & 0x000000000F) << 28) | (((server) & 0x00000000FF) << 20) | (((channel) & 0x00000000FF) << 12) | ((room) & 0x0000000FFF))

#define GET_FRIEND_MATCH(state)							(((state) >> 32) & 0x00000000000000FF)
#define GET_FRIEND_STATE(state)							(((state) >> 28) & 0x0000000F)
#define GET_FRIEND_SERVER(state)						(((state) >> 20) & 0x000000FF)
#define GET_FRIEND_CHANNEL(state)						(((state) >> 12) & 0x000000FF)
#define GET_FRIEND_ROOMIDX(state)						((state) & 0x00000FFF)

#define SET_FRIEND_MATCH(state, match)					(state | (((UINT64)match & 0x00000000000000FF ) << 32))
#define CLEAR_FRIEND_MATCH(state)						(state & 0xFFFFFF00FFFFFFFF)


inline	UINT32	FRIEND_STATE_64_TO_32(UINT64 State)
{
	return static_cast<UINT32>(MAKE_FRIEND_STATE(GET_FRIEND_STATE(State),GET_FRIEND_SERVER(State),
		GET_FRIEND_CHANNEL(State),GET_FRIEND_ROOMIDX(State)));
}

enum FRIEND_STATE
{
	FRIEND_STATE_REQUEST = 1,
	FRIEND_STATE_HOLD,
	FRIEND_STATE_OFFLINE,		//이하단은 친구 
	FRIEND_STATE_ONLINE,
	FRIEND_STATE_ROOM,	

	FRIEND_STATE_MAX
};

enum FRIEND_CHANGE_STATE
{
	FRIEND_CHANGE_STATE_INSERT = 1,
	FRIEND_CHANGE_STATE_ACCEPT, 
	FRIEND_CHANGE_STATE_DELETE, 
	FRIEND_CHANGE_STATE_CHANGE,
};

#pragma pack( push, 4 )

typedef struct _FriendInfo
{
	TUID	_fuid;		// 친구 uid
	UINT32	_state;		// 상태 0xF 00 F F FFF - bit 28-31:loginState, 20-27:reserved, 16-19:serverIdx, 12-15:channelIdx, 0-11:roomIdx
	UINT8	_rank;		// 계급 (0,51]
} FRIEND_INFO;

typedef struct _BlockInfo
{
	INT32	_Idx;		// DB Index : 차단 유저 삭제 요청시에 사용
	UINT8	_rank;		// 계급 (0,51]
} BLOCK_INFO;
 

#pragma pack( pop )

#pragma pack( push, 1 )

typedef struct _ClanMemberInfo
{
	TUID	_fuid;		// 친구 uid
	UINT64	_state;		// 상태 0xF 00 F F FFF - bit 28-31:loginState, 20-27:reserved, 16-19:serverIdx, 12-15:channelIdx, 0-11:roomIdx	
	UINT8	_rank;		// 계급 (0,51]
} CLANMEMBER_INFO;

#pragma pack( pop )
////////////////////////////////////////////////////////////////////////////////
// Shop 관련
////////////////////////////////////////////////////////////////////////////////

#define GET_ITEM_FLAG_CLASS(Itemflag)					((Itemflag) / 100000000)
#define GET_ITEM_FLAG_USAGE(Itemflag)					(((Itemflag) % 100000000) / 1000000)
#define GET_ITEM_FLAG_TYPE(Itemflag)					(((Itemflag) % 1000000) / 1000)
#define GET_ITEM_FLAG_NUMBER(Itemflag)					((Itemflag) % 1000)
#define MAKE_ITEM_FLAG(class,usage,classtype,number)	(((class)*100000000)+((usage)*1000000)+((classtype)*1000)+(number))

#define GET_GOODS_FLAG_CLASS(Goodsflag)					( (Goodsflag) / 10000000 )
#define GET_GOODS_FLAG_NUMBER(Goodsflag)				(((Goodsflag) % 10000000) / 100)
#define MAKE_GOODS_FLAG(class,number)					(((class)*10000000)+((number)*100))

#define GET_GOODS_FLAG_RS_TAB_IDX(Goodsflag)			(((Goodsflag) % 10000000) / 100)	// 굿즈 아이디로 뽑기 상점 탭번호 추출.
//INT32 MAKE_ITEM_FLAG(INT32 itemClass, INT32 usage, INT32 classtype, INT32 number)
//{
//	return ((itemClass*100000000)+(usage*1000000)+(classtype*1000)+(number));
//}

#define DEFAULT_WEAPON_ASSAULT							MAKE_ITEM_FLAG(ITEM_CLASS_ASSAULT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_K2"))
#define DEFAULT_WEAPON_SNIPER							MAKE_ITEM_FLAG(ITEM_CLASS_SNIPER, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_SSG69"))
#define DEFAULT_WEAPON_SMG								MAKE_ITEM_FLAG(ITEM_CLASS_SMG, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_MP5K_SILENCER"))					
#define DEFAULT_WEAPON_SHOTGUN							MAKE_ITEM_FLAG(ITEM_CLASS_SHOTGUN, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SHOTGUN, GetItemIndex("SHOTGUN_870MCS"))
#define DEFAULT_WEAPON_HANDGUN							MAKE_ITEM_FLAG(ITEM_CLASS_HANDGUN, WEAPON_USAGE_SECONDARY, WEAPON_CLASS_HANDGUN, GetItemIndex("HANDGUN_K5"))
#define DEFAULT_WEAPON_KNIFE							MAKE_ITEM_FLAG(ITEM_CLASS_KNIFE, WEAPON_USAGE_MELEE, WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_M7"))						
#define DEFAULT_WEAPON_THROWING							MAKE_ITEM_FLAG(ITEM_CLASS_THROWING, WEAPON_USAGE_THROWING, WEAPON_CLASS_THROWING, GetItemIndex("THROWING_K400"))
#define DEFAULT_WEAPON_ITEM								MAKE_ITEM_FLAG(ITEM_CLASS_THROWING, WEAPON_USAGE_ITEM, WEAPON_CLASS_THROWING, GetItemIndex("THROWING_SMOKE"))						

#define DEFAULT_CHARA_RED_TEAM							MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_MALE_RED)
#define DEFAULT_CHARA_BLUE_TEAM							MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_MALE_BLUE)
#define DEFAULT_CHARA_DINO								MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_DINO, CHARACTER_TEAM_RB,	CHARACTER_RES_ID_DINO_Raptor)

#define DEFAULT_EQUIP_HEAD								MAKE_ITEM_FLAG(ITEM_CLASS_HEAD,	CHARACTER_CLASS_HEAD, CHARACTER_TEAM_RB, HEAD_RES_ID_DEFAULT)

//////////////////////////////////////////////////////////////////////////////////////
//
//	유료 아이템중에서 같은 계열 상품을 하나로 묶기 위한 그룹 idx (인덱스 바뀌면 안됨)
//
// 아이템 아이디 세번째에 들어갑니다. ( 3 )
//
enum CASHITEM_GROUP_TYPE
{
	CASHITEM_GROUP_UNKNOWN = 0,

	CASHITEM_GROUP_XP_PLUS_110					= 1,
	CASHITEM_GROUP_XP_PLUS_130					= 2,
	CASHITEM_GROUP_XP_PLUS_150					= 3,
	CASHITEM_GROUP_PT_PLUS_130					= 4,
	CASHITEM_GROUP_COLOR_CLAN					= 5,

	CASHITEM_GROUP_COLOR_NICK					= 6,
	CASHITEM_GROUP_SHORT_RESPAWN				= 7,
	CASHITEM_GROUP_AMMO_UP						= 8,
	CASHITEM_GROUP_DISGUISE_RANK				= 9,
	CASHITEM_GROUP_DISGUISE_NICK				= 10,

	CASHITEM_GROUP_FREE_MOVE					= 11,
	CASHITEM_GROUP_CLAN_XP_PLUS_150				= 12,
	CASHITEM_GROUP_CUSTOM_CROSSHAIR				= 13,
	CASHITEM_GROUP_COLOR_CROSSHAIR				= 14,
	CASHITEM_GROUP_COLOR_CHATTING				= 15,

	CASHITEM_GROUP_BATTLEFIELD_MOVE				= 16,
	CASHITEM_GROUP_GET_DROPPED_WEAPON			= 17,
	CASHITEM_GROUP_XP_PLUS_X2_12PM				= 18,
	CASHITEM_GROUP_XP_PLUS_X2_3PM				= 19,
	CASHITEM_GROUP_XP_PLUS_X2_6PM				= 20,

	CASHITEM_GROUP_XP_PLUS_X2_9PM				= 21,
	CASHITEM_GROUP_PT_PLUS_X2_12PM				= 22,
	CASHITEM_GROUP_PT_PLUS_X2_3PM				= 23,
	CASHITEM_GROUP_PT_PLUS_X2_6PM				= 24,
	CASHITEM_GROUP_PT_PLUS_X2_9PM				= 25,

	CASHITEM_GROUP_QUICK_CHANGE_WEAPON			= 26,
	CASHITEM_GROUP_QUICK_CHANGE_MAGAZINE		= 27,
	CASHITEM_GROUP_MEGA_HP10					= 28,
	CASHITEM_GROUP_INCREASE_INVINCIBLE_TIME		= 29,
	CASHITEM_GROUP_BULLET_PROOF_VEST			= 30,

	CASHITEM_GROUP_FULLMETALJACKETAMMO			= 31,
	CASHITEM_GROUP_HOLLOW_POINT_AMMO			= 32,
	CASHITEM_GROUP_ANTI_FLASHBANG_MASK			= 33,
	CASHITEM_GROUP_C4_SPEED_KIT					= 34,
	CASHITEM_GROUP_INCREASE_GRENADE_SLOT		= 35,

	CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO	= 36,	
	CASHITEM_GROUP_XP_PLUS_200					= 37,
	CASHITEM_GROUP_PT_PLUS_200					= 38,	
	CASHITEM_GROUP_GIGA_BOMB					= 39,	
	CASHITEM_GROUP_MEGA_HP5						= 40,

	CASHITEM_GROUP_DEFCON_1						= 41,
	CASHITEM_GROUP_DEFCON_2						= 42,
	CASHITEM_GROUP_DEFCON_3						= 43,
	CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS		= 44,
	CASHITEM_GROUP_WEAPON_CRATE_2				= 45,

	CASHITEM_GROUP_MINIBOMB						= 46,
	CASHITEM_GROUP_CHANGE_NICK					= 47,
	CASHITEM_GROUP_CLR_HISTORY					= 48,
	CASHITEM_GROUP_CLR_SCORE					= 49,
	CASHITEM_GROUP_CLR_ESCAPE					= 50,

	CASHITEM_GROUP_CHANGE_CLAN_NAME				= 51,
	CASHITEM_GROUP_CHANGE_CLAN_MARK				= 52,
	CASHITEM_GROUP_CLR_CLAN_HISTORY				= 53,
	CASHITEM_GROUP_RECORDING_GAME				= 54,
	CASHITEM_GROUP_CLAN_PERSON_PLUS_50			= 55,

	CASHITEM_GROUP_CLAN_POINT_RESET				= 56,
	CASHITEM_GROUP_CUSTOM_CROSSHAIR_MAKE		= 57,
	CASHITEM_GROUP_MEGA_BOMB					= 58,
	CASHITEM_GROUP_WEAPON_CRATE					= 59,
	CASHITEM_GROUP_SURVIVAL_KIT					= 60,

	CASHITEM_GROUP_MEGABOMB_INDO				= 61,
	CASHITEM_GROUP_XP_PLUS_200_0AM_6AM			= 62,
	CASHITEM_GROUP_XP_PLUS_200_6AM_12PM			= 63,
	CASHITEM_GROUP_SHORT_RESPAWN_50				= 64,
	CASHITEM_GROUP_BULLET_PROOF_VEST_GM			= 65,

	CASHITEM_GROUP_POINT_GACHAPON_2500			= 66,
	CASHITEM_GROUP_POINT_GACHAPON_6000			= 67,
	CASHITEM_GROUP_GIGA_BOX						= 68,
	CASHITEM_GROUP_SWAT_KIT_ASSULT_7DAY			= 69,
	CASHITEM_GROUP_SWAT_KIT_ASSULT_30DAY		= 70,

	CASHITEM_GROUP_SWAT_KIT_SUPPORT_7DAY		= 71,
	CASHITEM_GROUP_SWAT_KIT_SUPPORT_30DAY		= 72,
	CASHITEM_GROUP_SWAT_KIT_INFILTRATION_7DAY	= 73,
	CASHITEM_GROUP_SWAT_KIT_INFILTRATION_30DAY	= 74,
	CASHITEM_GROUP_SWAT_KIT_COMBO_7DAY			= 75,

	CASHITEM_GROUP_SWAT_KIT_COMBO_30DAY			= 76,
	CASHITEM_GROUP_SHORT_RESPAWN_20				= 77,
	CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS		= 78,
	CASHITEM_GROUP_BULLET_PROOF_VEST_METAL		= 79,
	CASHITEM_GROUP_SHORT_RESPAWN_100			= 80,

	CASHITEM_GROUP_MEGABOMB_RUSSIA				= 81,
	CASHITEM_GROUP_MASK_ITEM_BOX_1				= 82,
	CASHITEM_GROUP_MASK_ITEM_BOX_2				= 83,
	CASHITEM_GROUP_ANGPAO						= 84,
	CASHITEM_GROUP_VIEW_OTHER_USER_ITEM_INFO	= 85,
	CASHITEM_GROUP_NEW_M4_SUPPLY_KIT			= 86,
	CASHITEM_GROUP_GIFT_BOX						= 87,
	CASHITEM_GROUP_RANGEMASTER_SUPPLY_KIT_1D	= 88,
	CASHITEM_GROUP_RAGINGBULL_454_SUPPLY_KIT	= 89,
	CASHITEM_GROUP_DUAL_UZI_SUPPLY_KIT			= 90,

	CASHITEM_GROUP_KETUPAT						= 91,
	CASHITEM_GROUP_RANDOM_MASK_BOX				= 92,
	CASHITEM_GROUP_RANGEMASTER_SUPPLY_KIT_3D	= 93,
	CASHITEM_GROUP_RANGEMASTER_SUPPLY_KIT_7D	= 94,
	CASHITEM_GROUP_RAGINGBULL_454_SUPPLY_KIT_3D	= 95,
	CASHITEM_GROUP_RAGINGBULL_454_SUPPLY_KIT_7D	= 96,
	CASHITEM_GROUP_FAMAS_G2_SUPPLY_KIT			= 97,
	CASHITEM_GROUP_DUAL_UZI_SUPPLY_KIT_3D		= 98,
	CASHITEM_GROUP_DUAL_UZI_SUPPLY_KIT_7D		= 99,
	CASHITEM_GROUP_DUAL_UZI_SUPPLY_KIT_30D		= 100,

	//CASHITEM_GROUP_SPEEDUP_BOOTS				= xx,	// 속도 향상 신발 (미적용)
	//CASHITEM_GROUP_SLIENT_FOOTSTEPS			= xx,	// 발소리 줄이는 신발 (미적용)

	CASHITEM_GROUP_VALENTINE_CHOCOLATE			= 101,
	CASHITEM_GROUP_FAMAS_G2_SUPPLY_KIT_3D		= 102,
	CASHITEM_GROUP_FAMAS_G2_SUPPLY_KIT_7D		= 103,
	CASHITEM_GROUP_FAMAS_G2_SUPPLY_KIT_30D		= 104,

	CASHITEM_GROUP_NEW_M4_SUPPLY_KIT_3D		= 105,
	CASHITEM_GROUP_NEW_M4_SUPPLY_KIT_7D		= 106,
	CASHITEM_GROUP_NEW_M4_SUPPLY_KIT_30D	= 107,

	CASHITEM_GROUP_CHEGUEVARA_SUPPLY_KIT	= 108,	//체게바라 뽑기 아이템

	CASHITEM_GROUP_LICENSE_SILVER_30D		= 109,	//라이센스(실버)
	CASHITEM_GROUP_LICENSE_GOLD_30D			= 110,	//라이센스(골드)
	
	//중남미용
	CASHITEM_GROUP_READYBOX_SILVER_SET_LATIN		= 111,	//보급 상자 뽑기 아이템(실버)
	CASHITEM_GROUP_READYBOX_GOLD_SET_LATIN	= 112,	//보급 상자 뽑기 아이템(골드)
	CASHITEM_GROUP_READYBOX_NORMAL_SET_LATIN		= 113,	//보급 상자 뽑기 아이템(일반)

	CASHITEM_GROUP_PRIZE_MONEY_500			= 114,	//500 달러 보상 아이템
	CASHITEM_GROUP_PRIZE_MONEY_1000			= 115,	//1000 달러 보상 아이템
	CASHITEM_GROUP_PRIZE_MONEY_5000			= 116,	//5000 달러 보상 아이템
	CASHITEM_GROUP_PRIZE_MONEY_10000		= 117,	//10000 달러 보상 아이템
	CASHITEM_GROUP_PRIZE_MONEY_30000		= 118,	//30000 달러 보상 아이템

	CASHITEM_GROUP_PT_PLUS_150				= 119,	// Point 150%	

	CASHITEM_GROUP_POINT_BOMB				= 120,	// Point Bomb	
	CASHITEM_GROUP_POINT_BOMB_PREMIUM		= 121,	// Point Bomb Premium	

	//중남미용
	CASHITEM_GROUP_READYBOX_SILVER_PRIZE_SET_LATIN		= 122,	//보급 상금 상자 뽑기 아이템(실버)
	CASHITEM_GROUP_READYBOX_GOLD_PRIZE_SET_LATIN		= 123,	//보급 상금 상자 뽑기 아이템(골드)
	CASHITEM_GROUP_READYBOX_NORMAL_PRIZE_SET_LATIN		= 124,	//보급 상금 상자 뽑기 아이템(일반)

	//일반 국가
	CASHITEM_GROUP_READYBOX_SILVER_SET		= 125,	//보급 상자 뽑기 아이템(실버)
	CASHITEM_GROUP_READYBOX_GOLD_SET		= 126,	//보급 상자 뽑기 아이템(골드)
	CASHITEM_GROUP_READYBOX_NORMAL_SET		= 127,	//보급 상자 뽑기 아이템(일반)

	CASHITEM_GROUP_PRIZE_MONEY_100		= 128,	//100 달러 보상 아이템

	CASHITEM_GROUP_CHRISTMAS_GIFTBOX		= 129,	//북미 크리스마스 기프트박스 아이템
	CASHITEM_GROUP_GIFTBOX_NORTH			= 130,	//북미 1주년기념 기프트박스 아이템

	CASHITEM_GROUP_FACEBOOKBOX				= 131,	// 북미 페이스북 박스 아이템
	CASHITEM_GROUP_RAPTRBOX					= 132,	// 북미 랩터 박스 아이템
	CASHITEM_GROUP_X_FILEBOX				= 133,	// 북미 X-File 박스 아이템

	//
	CASHITEM_GROUP_INCREASE_LIFE			= 134,	// 제압 라이프증가.
	CASHITEM_GROUP_BOOST_SP					= 135,	// 제압 SP부스트
	CASHITEM_GROUP_INCREASE_SP				= 136,	// 제압 SP증가
	CASHITEM_GROUP_RESURRECT_MINE			= 137,	// 제압 자신 부활
	CASHITEM_GROUP_RESURRECT_MEMBER			= 138,	// 제압 멤버 부활
	CASHITEM_GROUP_RESURRECT_TEAM			= 139,	// 제압 팀 부활
	CASHITEM_GROUP_ENHANCE_RECOVERY_ITEM	= 140,	// 제압 드랍된 회복아이템 강화
	//
	CASHITEM_GROUP_DOMI_RESERVED00			= 141,	// 모드전용 야전상점 아이템의 예비 아이템..
	CASHITEM_GROUP_DOMI_RESERVED01			= 142,
	CASHITEM_GROUP_DOMI_RESERVED02			= 143,
	CASHITEM_GROUP_DOMI_RESERVED03			= 144,
	CASHITEM_GROUP_DOMI_RESERVED04			= 145,
	CASHITEM_GROUP_DOMI_RESERVED05			= 146,
	CASHITEM_GROUP_DOMI_RESERVED06			= 147,
	CASHITEM_GROUP_DOMI_RESERVED07			= 148,
	CASHITEM_GROUP_DOMI_RESERVED08			= 149,
	CASHITEM_GROUP_DOMI_RESERVED09			= 150,
	CASHITEM_GROUP_DOMI_RESERVED10			= 151,
	CASHITEM_GROUP_DOMI_RESERVED11			= 152,
	CASHITEM_GROUP_DOMI_RESERVED12			= 153,
	CASHITEM_GROUP_DOMI_RESERVED13			= 154,
	CASHITEM_GROUP_DOMI_RESERVED14			= 155,
	CASHITEM_GROUP_DOMI_RESERVED15			= 156,
	CASHITEM_GROUP_DOMI_RESERVED16			= 157,
	CASHITEM_GROUP_DOMI_RESERVED17			= 158,

	// 대만 이벤트 아이템.
	CASHITEM_GROUP_PT_PLUS_130_EV				= 159,
	CASHITEM_GROUP_XP_PLUS_130_EV				= 160,
	CASHITEM_GROUP_CLAN_XP_PLUS_150_EV			= 161,
	CASHITEM_GROUP_FREE_MOVE_EV					= 162,
	CASHITEM_GROUP_SHORT_RESPAWN_30_EV			= 163,
	CASHITEM_GROUP_SHORT_RESPAWN_20_EV			= 164,
	CASHITEM_GROUP_QUICK_CHANGE_WEAPON_EV		= 165,
	CASHITEM_GROUP_QUICK_CHANGE_MAGAZINE_EV		= 166,
	CASHITEM_GROUP_INCREASE_INVINCIBLE_TIME_EV	= 167,
	CASHITEM_GROUP_GET_DROPPED_WEAPON_EV		= 168,
	CASHITEM_GROUP_HOLLOW_POINT_AMMO_EV			= 169,
	CASHITEM_GROUP_FULLMETALJACKETAMMO_EV		= 170,

	CASHITEM_GROUP_RANDOMBOX_GOLD			= 500,	// 2012.06.05 이후부터 등록되는 뽑기아이템(=캡슐아이템,=Capsule Item)은 500번부터 시작합니다. 안국정
	CASHITEM_GROUP_RANDOMBOX_BLACK			= 501,
	CASHITEM_GROUP_RANDOMBOX_GATOTKACA_BOX	= 502,	//
};

// 혹시나 인게임 장바구니가 생길가봐 비트 연산으로 갑니다.
enum EQUIPMENT_IN_BATTLE
{
	EQUIPMENT_IN_BATTLE_PRIMARY		= 0x1,
	EQUIPMENT_IN_BATTLE_SECONDARY	= 0x2,
	EQUIPMENT_IN_BATTLE_MELEE		= 0x4,
	EQUIPMENT_IN_BATTLE_GRENADE		= 0x8,
	EQUIPMENT_IN_BATTLE_THROWING	= 0x10,
	EQUIPMENT_IN_BATTLE_CHAR		= 0x20,
	EQUIPMENT_IN_BATTLE_HEAD		= 0x40,
	EQUIPMENT_IN_BATTLE_ITEM		= 0x80,
};
//
// 아이템 아이디 네번째에 들어갑니다. ( 4 )
//
#define		CASHITEM_DAY_1		1
#define		CASHITEM_DAY_3		3
#define		CASHITEM_DAY_7		7
#define		CASHITEM_DAY_10		10
#define		CASHITEM_DAY_30		30
#define		CASHITEM_DAY_90		90
#define		CASHITEM_DAY_NO		0



enum AUTH_TYPE
{
	AUTH_UNKNWON	= 0,
	AUTH_COUNT		= 1,	// 개수제 인증
	AUTH_SEC				// 기간제 인증
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InBattle

// 파괴미션 정보 오브젝트
typedef struct __tag_DestructionInfo
{
	UINT16		_nRedObjectHP;
	UINT16		_nBlueObjectHP;
	UINT16		_nUserDamage[ SLOT_MAX_COUNT];		//변수 이름 변경해야함 퍼센트 아님

	void		Reset( void)
	{
		_nRedObjectHP		= 0;
		_nBlueObjectHP		= 0;
		for( INT32 i=0; i<SLOT_MAX_COUNT;i++)
		{
			_nUserDamage[ i] = 0;
		}
	}

	void		CopyTo( __tag_DestructionInfo * pDest)
	{
		pDest->_nRedObjectHP = _nRedObjectHP;
		pDest->_nBlueObjectHP = _nBlueObjectHP;
		memcpy( pDest->_nUserDamage, _nUserDamage, sizeof(UINT16)*SLOT_MAX_COUNT );
	}
	void		AddUserDamage(INT32 idx, INT32 iDamage)
	{
		INT32 iMax = (INT32)ldexp( 1.f, sizeof(_nUserDamage[0])*8 ) - 1;
		INT32 iTemp = _nUserDamage[ idx] + iDamage;	

		if(iTemp > iMax) {
			iTemp = iMax;
		}

		_nUserDamage[ idx] = (UINT16)iTemp;
	}
} DESTRUCTION_INFO;

// 디펜스 미션 오브젝트 정보
typedef struct __tag_DefenceInfo
{	
	UINT16		_nObjectHP[MAX_OBJECT_COUNT];
	UINT16		_nUserDamage[MAX_OBJECT_COUNT][ SLOT_MAX_COUNT];		// [Red : 두개의object 각각에 대한 유저당 피해량],[Blue : 각각의 오브젝트에 대한 방어 성공시 보너스]

	void		Reset( void)
	{
		for(INT32 Loop = 0; Loop < MAX_OBJECT_COUNT; Loop++)
		{
			_nObjectHP[Loop]		= 0;

			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				_nUserDamage[Loop][i] = 0;
			}
		}
	}

	void		CopyTo( __tag_DefenceInfo * pDest)
	{
		for(INT32 Loop = 0; Loop < MAX_OBJECT_COUNT; Loop++)
		{
			pDest->_nObjectHP[Loop] = _nObjectHP[Loop];
		}

		for(INT32 Loop2 = 0; Loop2 < MAX_OBJECT_COUNT; Loop2++)
		{
			for( INT32 i=0; i<SLOT_MAX_COUNT; i++)
			{
				pDest->_nUserDamage[Loop2][i] = _nUserDamage[Loop2][i];
			}
		}
	}

	void		CopyToBlueDefenceInfo( __tag_DefenceInfo * pDest)
	{
		for(INT32 Loop2 = 0; Loop2 < MAX_OBJECT_COUNT; Loop2++)
		{
			for( INT32 i=0; i<SLOT_MAX_COUNT; i++)
			{
				if((i % TEAM_COUNT)  == TEAM_BLUE)	pDest->_nUserDamage[Loop2][i] = _nUserDamage[Loop2][i];
			}
		}
	}

	UINT16		getObjectHP( INT32 idx)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);
		return _nObjectHP[ idx];
	}

	void		setObjectHP( INT32 idx, UINT16 hp)
	{
		I3_BOUNDCHK( idx, MAX_OBJECT_COUNT);
		_nObjectHP[ idx] = hp;
	}

	void		AddUserDamage(INT32 iObjectIdx, INT32 idx, INT32 iDamage)
	{
		INT32 iMax = (INT32)ldexp( 1.f, sizeof(_nUserDamage[iObjectIdx][0])*8 ) - 1;
		INT32 iTemp = _nUserDamage[iObjectIdx][idx] + iDamage;	

		if(iTemp > iMax) {
			iTemp = iMax;
		}

		_nUserDamage[iObjectIdx][idx] = (UINT16)iTemp;
	}

	UINT16		getUserDamage( INT32 iObjectIdx, INT32 idx)
	{
		I3_BOUNDCHK( iObjectIdx, MAX_OBJECT_COUNT);
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

		return _nUserDamage[ iObjectIdx][ idx];
	}

	void		setUserDamage( INT32 iObjectIdx, INT32 idx, UINT16 damage)
	{
		I3_BOUNDCHK( iObjectIdx, MAX_OBJECT_COUNT);
		I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

		_nUserDamage[ iObjectIdx][ idx] = damage;
	}

} DEFENCE_INFO;

#define BOUNDCHK( a, b)				I3ASSERT( ((a) >= 0) && ((a) < (b)))

// Clan
// Write, Invited, Deportation, Accept
#define CLAN_MENAGE_AUTHORITY(wr,in,de,ac)	((wr & 0x0000000F) << 3) | ((in & 0x0000000F) << 2) | ((de & 0x0000000F) << 1) | ((ac & 0x0000000F))
#define GET_CLAN_MENAGE_WRITE(state)		((state>>3) & 0x00000001)
#define GET_CLAN_MENAGE_INVITED(state)		((state>>2) & 0x00000001)
#define GET_CLAN_MENAGE_DEPORTATION(state)	((state>>1) & 0x00000001)
#define GET_CLAN_MENAGE_ACCEPT(state)		((state)	& 0x00000001)

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
	CLAN_MEMBER_LEVEL_UNKNOWN = 0,

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
	CLAN_ROOM_STAY,				// 대기중인 팀 ( 전투 신청 받을 수 있는 상태 )
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

#define DF_CLAN_UNIT_SQUARD			1
#define DF_CLAN_UNIT_PLATOON		10
#define DF_CLAN_UNIT_COMPANY		30
#define DF_CLAN_UNIT_BATTALION		50
#define DF_CLAN_UNIT_REGIMENT		100
#define DF_CLAN_UNIT_BRIGADE		150
#define DF_CLAN_UNIT_DIVISION		200
#define DF_CLAN_UNIT_CORPS			250

#pragma pack( push, 1 )

// 클랜 목록 정보
typedef struct _tag_ClanContext
{
	UINT32	_totalCount;						// 전체수
	UINT8	_sectionSize;						// 페이지 크기
	UINT16	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} CLAN_CONTEXT;

// 클랜 기본 정보
typedef struct _tag_ClanBasicInfo
{
	UINT32	_idx;								// 클랜 인덱스
	char	_name[NET_CLAN_NAME_SIZE];			// 클랜명
	UINT8	_rank;								// 클랜 등급
	UINT8	_person;							// 현재 클랜원 수
	UINT8	_maxperson;							// 최대 클랜원 수
	UINT32	_date;								// 클랜 개설일
	UINT32	_mark;								// 마크
	UINT8	_color;								// 클랜컬러
} CLAN_BASIC_INFO;

// 클랜 상세 정보
typedef struct _tag_ClanDetailInfo
{
	// Basic
	UINT32	_idx;								// 클랜 인덱스
	char	_name[NET_CLAN_NAME_SIZE];			// 클랜명
	UINT8	_expRank;							// 경험치 등급
	UINT8	_currentPerson;						// 현재 클랜원 수
	UINT8	_maxPerson;							// 최대 클랜원 수
	UINT32	_date;								// 클랜 개설일
	UINT32	_mark;								// 마크
	UINT8	_color;								// 클랜컬러

	// Detail
	UINT8	_personRank;						// 클랜원 등급
	UINT32	_exp;								// 경험치	
	UINT16	_clanRank;							// 클랜 순위
	UINT16	_prevRank;							// 이전 클랜 순위	
	TUID	_i64masterUID;						// 마스터 dbIdx(UID)
	char	_masterNick[NET_NICK_NAME_SIZE];	// 마스터 닉네임
#ifdef USE_COLOR_NICK2
	UINT8	_masterNickColor;					// 마스터 닉네임 칼라
#endif
	UINT8	_masterRank;						// 마스터 등급	
	char	_intro[NET_CLAN_INTRO_SIZE];		// 클랜 소개	
	char	_azitURL[NET_CLAN_AZIT_URL_SIZE];	// 아지트 URL
	UINT8	_ranklock;							// 계급 제한
	UINT8	_lagelock;							// 연령 제한 하한
	UINT8	_hagelock;							// 연령 제한 상한

	// Member
	UINT8	_authstaff;							// 스탭 권한
	char	_announce[NET_CLAN_INTRO_SIZE];		// 공지사항

	UINT32	_match;								// 통산전적
	UINT32	_win;								// 승수
	UINT32	_lose;								// 패수	
	UINT32	_smatch;							// 시즌전적
	UINT32	_swin;								// 시즌승수
	UINT32	_slose;								// 시즌패수

	TUID	_topexp;							// 통산 경험치 공로자
	TUID	_stopexp;							// 시즌 경험치 공로자
	TUID	_topwin;							// 통산 승수 공로자
	TUID	_stopwin;							// 시즌 승수 공로자
	TUID	_topkill;							// 통산 킬 공로자
	TUID	_stopkill;							// 시즌 킬 공로자
	TUID	_tophead;							// 통산 헤드샷 공로자
	TUID	_stophead;							// 시즌 헤드샷 공로자
	TUID	_topconnect;						// 통산 접속률 공로자
	TUID	_stopconnect;						// 시즌 접속률 공로자
	REAL32	_clanpoint;							// 클랜 포인트

} CLAN_DETAIL_INFO;

// 멤버 목록 정보
typedef struct _tag_ClanMemberContext
{
	UINT8	_totalCount;						// 전체 클랜원 수
	UINT8	_sectionSize;						// 페이지 크기
	UINT8	_sectionCount;						// 전체 페이지
	UINT32	_version;							// 목록 버전
} CLAN_MEMBER_CONTEXT;

// 멤버 기본 정보
typedef struct _tag_ClanMemberBasicInfo
{
//	UINT8	_idx;								// 클랜원 인덱스
	TUID	_i64UID;							// 클랜원 dbIdx(UID)
	char	_nick[NET_NICK_NAME_SIZE];			// 클랜원 닉네임
	UINT8	_rank;								// 클랜원 등급
	UINT8	_level;								// 클랜원 스텝 직급
	UINT64	_state;								// 클랜원 현재 접속 위치 및 상태
	UINT32	_date;								// 클랜원 신청일
	UINT8	_color;								// 클랜원 닉네임 칼라
} CLAN_MEMBER_BASIC_INFO;

// 멤버 기본 정보
typedef struct _tag_ClanMemberDetailInfo
{
	// Basic
//	UINT8	_idx;								// 클랜원 인덱스
	TUID	_i64UID;							// 클랜원 dbIdx(UID)
	char	_nick[NET_NICK_NAME_SIZE];			// 클랜원 닉네임
	UINT8	_rank;								// 클랜원 등급
	UINT8	_level;								// 클랜원 스텝 직급
	UINT64	_state;								// 클랜원 현재 접속 위치 및 상태
	UINT32	_date;								// 클랜원 신청일
	UINT8	_color;								// 클랜원 닉네임 칼라

	// Server
	INT32	_session;							// Session
	UINT32	_battleteam;						// BattleTeamIdx
	UINT8	_pagein;							// 클랜 페이지인

	_tag_ClanMemberDetailInfo::_tag_ClanMemberDetailInfo()
	{
		Reset();
	};

	void	Reset()
	{
		_nick[0]	= '\0';
		_level		= CLAN_MEMBER_LEVEL_REGULAR;
		_state		= MAKE_FRIEND_MATCH_STATE(0, FRIEND_STATE_OFFLINE, 0, 0, 0 );
		_session	= -1;
		_battleteam	= 65535; //CLAN_BATTLE_EMPTY_INDEX
		_pagein		= 0;
		_color		= 0;
	};

} CLAN_MEMBER_DETAIL_INFO;

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
	TUID	_i64UID;							// 신청서 dbIdx(UID)
	char	_nick[NET_NICK_NAME_SIZE];			// 신청인 닉네임
	UINT8	_rank;								// 신청인 등급
	UINT32	_date;								// 신청일
} CLAN_REQUEST_BASIC_INFO;

// 신청서 상세 정보
typedef struct _tag_ClanRequestDetailInfo
{
	TUID	_i64UID;							// 신청인 dbIdx(UID)
	char	_nick[NET_NICK_NAME_SIZE];			// 신청인 닉네임
	UINT8	_rank;								// 신청인 등급
	UINT32	_kill;								// 신청인 킬수
	UINT32	_death;								// 신청인 데스수
	UINT32	_match;								// 신청인 전수
	UINT32	_win;								// 신청인 승수
	UINT32	_lose;								// 신청인 패수
	char	_contents[NET_CLAN_REQUEST_SIZE];	// 신청서 내용

	void Reset(void)
	{
		//_idx = 0;
		_i64UID = 0;
		_nick[0] = '\0';
		_rank = 0;
		_kill = 0;
		_death = 0;
		_match = 0;
		_win = 0;
		_lose = 0;
		_contents[0] = '\0';
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
	char	_name[NET_CLAN_NAME_SIZE];
	char	_intro[NET_CLAN_INTRO_SIZE];
	char	_URL[NET_CLAN_AZIT_URL_SIZE];
	UINT32	_mark;
} CREATE_CLAN_INFO;

typedef struct _tagclanjoin
{
	UINT32	_clanidx;
	char	_message[ NET_CLAN_REQUEST_SIZE ];
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
} CLAN_BATTLE_TEAM;

typedef struct __tag_ClanBattleClan
{
	UINT32	_ClanIdx;									// 클랜 Idx ( DBIdx 가 아닌 클랜 서버 인덱스 )
	UINT8	_ClanRank;									// 클랜 랭킹
	UINT32	_ClanMark;									// 클랜 마크
	char	_ClanName[ NET_CLAN_NAME_SIZE ];			// 클랜 이름
	REAL32	_ClanPoint;
	UINT8	_ClanColor;									// 클랜 칼라
} CLAN_BATTLE_CLAN;

typedef struct __tag_ClanBattleMember
{
	UINT8	_Rank;
	char	_Name[ NET_NICK_NAME_SIZE ];
	TUID	_i64UID;
	UINT8	_State;
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

#pragma pack( pop)

////////////////////////////////////////////////////////////////////////////////
// End of Clan



// 임무카드 
//이 카운트가 변경되면 서버도 다시 시작되어야 합니다. 
// 카운터를 + 1을 해서 사용하겠습니다. 0은 사용하지 않는 값입니다. 
#define MAX_MINIATURE_COUNT		19
#define MAX_INSIGNIA_COUNT		21
#define MAX_MEDAL_COUNT			21



//
// Note - Messenger
////////////////////////////////////////////////////////////////////////////////

// 쪽지 설정
#define MAX_NORMAL_NOTE_COUNT			100						// 최대 일반 쪽지 - 100개를 기준으로 1개당 1%씩 취급한다.
#define MAX_NOTE_LENGTH					240						// 한글 120자
#define MAX_NOTE_COUNT_PER_PAGE			25						// 전송 페이지당 최대 쪽지 갯수
#define NET_NOTE_SIZE					(MAX_NOTE_LENGTH + 1)	// 널문자 포함
#define MAX_SEND_NOTE_LIMIT				100						// 하루 전송 제한
#define REMAIN_DAY_FOR_READED_NOTE		7						// 읽은 편지 삭제 기간 7일
#define REMAIN_DAY_FOR_UNREAD_NOTE		15						// 안읽은 편지 삭제 기간 15일

// 쪽지 종류
enum NoteType
{
	NT_NORMAL = 0,		// 일반 쪽지
//	NT_NORMAL_ASK,		// 답장
	NT_NOTICE,			// 알림 쪽지
	NT_GIFT,			// 선물 쪽지
	NT_WEB,				// 웹구매 쪽지
	NT_CLAN,			// 클랜 쪽지
	NT_CLAN_ASK			// 클랜 권유 쪽지
};

// 쪽지 상태
enum NoteState
{
	NS_READED = 0,		// 읽은 쪽지
	NS_UNREAD,			// 안읽은 쪽지
	NS_NEW,				// 새 쪽지 - 로그인 후 전달된 쪽지
};

#define NT_GIFT_FLAG		0x10000000		//	note id 에 추가되며 선물 쪽지에 사용


#pragma pack( push, 1)

#define USE_MESS_TEST
// 쪽지 머리 구조체
typedef struct __tag_NoteHeader
{
	INT32 _id;								// 고유번호
#ifdef USE_MESS_TEST
	TUID _suid;							// Sender Idx
#endif
	UINT8 _type;							// NoteType
	UINT8 _state;							// NoteState
	UINT8 _remainDay;						// Day
	INT32 _appendingData;					// 첨부 정보
} NOTE_HEADER;

// 쪽지 몸체 구조체
typedef struct __tag_NoteBody
{
	UINT8 _senderSize;						// 보낸사람 문자열의 널포함 길이
	UINT8 _messageSize;						// 내용 문자열의 널포함 길이
	char _sender[NET_NICK_NAME_SIZE];		// 보낸사람	
	char _message[NET_NOTE_SIZE];			// 내용
	void * _user;							// 유저 데이터 (현재 Gift에서 사용함)
} NOTE_BODY;

// 쪽지 몸체 구조체(UID 이용시)
typedef struct __tag_NoteBodyByUID
{
	TUID	_receiverUid;					// 받는 사람의 UID
	UINT8	_messageSize;					// 내용 문자열의 널포함 길이
	char	_sender[NET_NICK_NAME_SIZE];	// 보낸사람	
	char	_message[NET_NOTE_SIZE];		// 내용
	void	*_user;							// 유저 데이터 (현재 Gift에서 사용함)
} NOTE_BODY_BY_UID;


#pragma pack (pop)


// 쪽지 구조체
typedef struct __tag_NoteNode
{
	NOTE_HEADER _head;						// 쪽지 머리
	NOTE_BODY _body;						// 쪽지 몸체
} NOTE_NODE;



// 클랜 쪽지를 위한 구조체
typedef struct __tag_ClanNote
{
	TUID	_DBIdx;
	UINT8	_IsAsk;
	UINT8	_MessageSize;
} CLAN_NOTE;

// 연합 미션 전용
#define EXPERIENCE_RATIO	0.02f
#define POINT_RATIO			0.018f
#define BOSS_MIDDLE_RATIO	0.2f


// End of Note

#define MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN NET_PW_SIZE

// 대만 로그인시 ID, OTP 최대길이. 널문자 포함.
#define MAX_ID_FOR_TAIWAN 13
#define MAX_OTP_FOR_TAIWAN 13


#define FIREDELAY_COST		///< 발사 간격에 대한 오차를 줄입니다.
//#define CHECK_LOADEDBULLET	///< 무한 탄약 치트 검사

//--------------------------------------
//	CHARA_ACTION_BODYLOWER
//--------------------------------------
enum CHARA_ACTION_BODYLOWER
{
	CHARA_BODYLOWER_STAND = 0,
	CHARA_BODYLOWER_CROUCH,

	CHARA_BODYLOWER_MAXCOUNT,
};

//--------------------------------------
//	CHARA_ACTION_BODYUPPER
//--------------------------------------
enum CHARA_ACTION_BODYUPPER
{
	CHARA_BODYUPPER_NONE = 0,		//	상체가 Death 상태일때는 상태가 무의미하므로 None이 된다.
	CHARA_BODYUPPER_IDLE,			//	휴식 상태 (현재 사용안함)
	CHARA_BODYUPPER_ATTACKIDLE,		//	기본 상태
	CHARA_BODYUPPER_ZOOM,			//	정조준 상태
	CHARA_BODYUPPER_GRENADE,		// 유탄 발사기 조준 상태
	CHARA_BODYUPPER_SHOTGUN,		// LSS (보조 샷건) 상태 
	CHARA_BODYUPPER_KNIFE,			// 총검 상태 
	CHARA_BODYUPPER_CROSSFIRE,		// 교차사격 모드

	CHARA_BODYUPPER_MAXCOUNT,	
};

enum DINO_ACTION_BODYUPPER
{
	DINO_BODYUPPER_NONE = -1,		//	상체가 Death 상태일때는 상태가 무의미하므로 None이 된다.
	DINO_BODYUPPER_IDLE,			//	기본 상태
	DINO_BODYUPPER_MOVE,			//	달리기 상태
	DINO_BODYUPPER_MAXCOUNT,	
};

//--------------------------------------
//	CHARA_ACTION_LOWER
// 8개를 초과하는 경우 문제가 생깁니다. 순구에게 문의
//--------------------------------------
enum CHARA_ACTION_LOWER
{
	CHARA_LOWER_NONE = 0,		
	CHARA_LOWER_IDLE,			//	기본 상태
	CHARA_LOWER_RUN,			//	뛰기 상태
	CHARA_LOWER_JUMP,			//	점프 상태
	CHARA_LOWER_DEATH,			//	죽음 상태
	CHARA_LOWER_WALK,			//  걷기 상태

	CHARA_LOWER_MAXCOUNT,
};

//--------------------------------------
//	CHARA_ACTION_UPPER
// 16개를 초과하면 문제가 생깁니다.
//--------------------------------------
enum CHARA_ACTION_UPPER
{
	CHARA_UPPER_NONE = 0,		//	상체가 Death 상태일때는 상태가 무의미하므로 None이 된다.
	CHARA_UPPER_IDLE,			//	기본 상태
	CHARA_UPPER_MOVE,			//	이동 상태
	CHARA_UPPER_FIRE,			//	무기 발사
	CHARA_UPPER_FIRE_READY,		//	무기 발사 준비 상태
	CHARA_UPPER_SECONDARY_FIRE,	//	보조 무기 발사
	CHARA_UPPER_LOADMAGAZINE,	//	총알 및 탄창 갈아끼우기
	CHARA_UPPER_LOADBULLET,		//	노리쇠 당기기
	CHARA_UPPER_DAMAGE,			//	데미지 상태
	CHARA_UPPER_SWAPWEAPON,		//	무기 교환 상태
	CHARA_UPPER_JUMP,			//	점프 상태	
	CHARA_UPPER_EXTENSION_ACTIVATE,		//	해당 무기의 Extension 기능을 활성화
	CHARA_UPPER_EXTENSION_DEACTIVATE,	//	해당 무기의 Extension 기능을 비활성화

	CHARA_UPPER_NONEBATTLEIDLE,			// UI의 Idle
	CHARA_UPPER_NONEBATTLEWEAPONSHOW,	// UI의 WeaponViewAction
	CHARA_UPPER_REMOVESHELL,

	CHARA_UPPER_LOADMAG_READY,
	CHARA_UPPER_FIREBULLET,		// 무기 발사후 노리쇠 당기기 (일부무기중에 LoadBullet과 모션이 달라 FireBullet을 사용하는 무기가 있다. M70)

	CHARA_UPPER_MAXCOUNT,
};

enum CHARA_ACTION_COMMON
{
	CHARA_ACTION_COMMON_NONE = 0,
	CHARA_ACTION_COMMON_CEREMONY,

	CHARA_ACTION_COMMON_MAXCOUNT,
};


enum NPC_ACTION_STATE
{
	NPC_ACTION_NONE = 0,
	NPC_ACTION_IDLE,
	NPC_ACTION_ATTACK,
	NPC_ACTION_DAMAGE,
	NPC_ACTION_DEATH,

	NPC_ACTION_MAXCOUNT
};

//--------------------------------------
//	CHARA_HIT_PART
//--------------------------------------
#define TEMP_UDP

enum CHARA_HIT_PART
{
	CHARA_HIT_UNKNOWN = 0,
	CHARA_HIT_ROOT,					// B_Root
	
	CHARA_HIT_BODY3,				// B_Spine1
	CHARA_HIT_BODY2,				// B_Spine2
	CHARA_HIT_BODY1,				// B_Spine3
	
	CHARA_HIT_NECK,					// B_Neck0
	CHARA_HIT_NECK1,				// B_Neck1
	CHARA_HIT_NECK2,				// B_Neck2

	CHARA_HIT_NECK_LOWER,			// B_Neck0_Lower
#ifdef TEMP_UDP
	CHARA_HIT_HEAD_OLD,				// H_Head
#else
	CHARA_HIT_HEAD,
#endif
	CHARA_HIT_JAW,					// H_Head
	
	CHARA_HIT_LCLAVICLE,			// B_L Clavicle
	CHARA_HIT_RCLAVICLE,			// B_R Clavicle
	
	CHARA_HIT_LUARM,				// AU_L UpperArm
	CHARA_HIT_LDARM,				// AD_L ForeArm
	CHARA_HIT_LHAND,				// HN_L Hand
	
	CHARA_HIT_RUARM,				// AU_R UpperArm
	CHARA_HIT_RDARM,				// AD_R ForeArm
	CHARA_HIT_RHAND,				// HN_R Hand
	
	CHARA_HIT_LULEG,				// LU_L Thigh
	CHARA_HIT_RULEG,				// LU_R Thigh

	CHARA_HIT_LDLEG,				// LD_L Calf
	CHARA_HIT_LHHORSE_LINK,			// LH_L HorseLink
	CHARA_HIT_LFOOT,				// F_L Foot

	CHARA_HIT_RDLEG,				// LD_R Calf
	CHARA_HIT_RHHORSE_LINK,			// LH_R HorseLink
	CHARA_HIT_RFOOT,				// F_R Foot

	CHARA_HIT_TAIL1,				// T_Tail1
	CHARA_HIT_TAIL2,				// T_Tail2
	CHARA_HIT_TAIL3,				// T_Tail3
	CHARA_HIT_TAIL4,				// T_Tail4

	CHARA_HIT_VITAL_EYE,			//

    CHARA_HIT_DEATH_TRAMPLE,		// NET용 DEATH확인
	CHARA_HIT_DEATH_HOWL,

#ifdef TEMP_UDP
	CHARA_HIT_HEAD,					// H_Head
#else
	CHARA_HIT_HEAD_OLD,
#endif

	CHARA_HIT_MAXCOUNT
};

// -------------------------------------------------
// Shffle hit pars
// -------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ShuffleHitPart(): 게임서버와 Dedi에서 사용
//		Desc. Hit Part를 섞어 주는 함수이며 결과값은 PROTOCOL_BATTLE_PRESTARTBATTLE_ACK로 
//			클라이언트에 전달됩니다.
// ------------------------------------------------------------------------------------------------
#define SHUFFLE_VALUE_MAX	20
extern const INT32 g_shufflevalue[SHUFFLE_VALUE_MAX][2]; 
void ShuffleHitPart ( UINT8* pOut, UINT32 ui32Key );

// -------------------------------------------------------------------------------------------------
// CalcHeadShotDamage : 
//		Desc. calc head shot damage
//		pShuffledPartsIdx	: the array of shuffled hit parts indices
//		ui32SizeOfPartsIdx	: size of the array
//		ui32RespawnIdx		: chara respawn index in the respawn packet
// -------------------------------------------------------------------------------------------------
UINT32	CalcHeadShotDamage( PBYTE pShuffledPartsArray, UINT32 ui32SizeOfShuffledParts,  UINT32 ui32MyRespawnCnt );


#define _SHUFFLE_HIT_PART
//--------------------------------------
//	UI_CHARA_VIEW
//--------------------------------------
enum UI_CHARA_ROTATE
{
	CHARA_LEFT = 1,
	CHARA_RIGHT,
};

#pragma pack( push, 1)
//공룡임무카드 데스블로우를 위해 추가된 구조체
typedef struct _DeathBlow_Info
{
	UINT8	_nTrexIdx;					// Target user SlotIdx
	UINT32	_WeaponInfo;				// 0x00 FF(SlotIdx ) FF(ClassType) FF(Number)
} DEATHBLOW_INFO;

typedef struct __tag_BattleRoomCreate_Req 
{
	BATTLE_ROOM_INFO	_RoomInfo; 
	UINT32				_iStage; 
	UINT32				_iStartUser;					// 시작 유저들 (비트 번호 = slot Index)
	UINT8				_StageMask; 
	UINT8				_ui8AvailableWeapon;			// 해당룸에서 사용가능한 무기
} P_BATTLE_ROOM_CREATE_REQ; 

typedef struct __tag_BattleRoomCreate_Ack 
{
	UINT8				_BattleType; 
	BATTLE_ROOM_INFO	_RoomInfo; 
	UINT32				_GroupIdx; 
	UINT32				_Key; 
	UINT32				_Ip;
	UINT16				_Port; 
}P_BATTLE_ROOM_CREATE_ACK; 

typedef struct __tag_P_BattleUserEnter_Req
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_wGroupIdx;
	UINT8				_cSlotIdx;
	UINT32				_iItem;
	UINT32				_ui32Character;			// 장착 캐릭터
	UINT32				_ui32Helmet;			// 헬맷
	UINT32				_ui32Beret;				// 헬맷과 동기 장착 가능 아이템 (베레모)
	UINT8				_aui8Designation[MAX_EQUIP_USER_TITLE];		// 호칭
	TUID				_i64UID;
}P_BATTLE_USER_ENTER_REQ;

typedef struct __tag_P_BattleUserLeave_Req
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_wGroupIdx;
	UINT8				_cSlotIdx;
}P_BATTLE_USER_LEAVE_REQ;

typedef struct __tag_P_BattleRound_Req 
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_GroupIdx; 
	UINT8				_iRound; 
	UINT32				_iDinoInfo[SLOT_MAX_COUNT];	
	UINT32				_aui32Item[SLOT_MAX_COUNT];
	UINT32				_aui32Helmet[SLOT_MAX_COUNT];
	UINT32				_aui32Beret[SLOT_MAX_COUNT];
	UINT8				_aui8Designation[SLOT_MAX_COUNT][MAX_EQUIP_USER_TITLE];
}P_BATTLE_ROUND_REQ; 

typedef struct __tag_P_BattleRespawn_Req
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_GroupIdx; 
	UINT8				_cSlotIdx;
	UINT32				_pUseWeaponList[WEAPON_USAGE_COUNT];
	INT32				_i32TotalRespawnIdx;
	INT32				_i32RespawnCount;
}P_BATTLE_RESPAWN_REQ;

typedef struct __tag_P_BattleMDinoInfo_Req
{
	BATTLE_ROOM_INFO	_RoomInfo;	
	UINT32				_GroupIdx;
	UINT16				_iSlotIdx; 
	UINT32				_iDinoInfo; 
	UINT32				_ui32Item;
	UINT32				_ui32Helmet;
	UINT32				_ui32Beret;
	UINT8				_aui8Designation[MAX_EQUIP_USER_TITLE];
}P_BATTLE_M_DINO_INFO_REQ;

typedef struct __tag_P_BattleRoomDestroy_Req 
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_GroupIdx; 
}P_BATTLE_ROOM_DESTROY_REQ; 

typedef struct __tag_P_BattleRoomKill_Ack
{
	BATTLE_ROOM_INFO	_RoomInfo;
	DEATH_INFO_HEADER	_killHeader; 
}P_BATTLE_ROOM_KILL_ACK;

typedef struct __tag_P_BattleMBombInstall_Ack 
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_iSlotIdx; 
	UINT8				_BombPosition; 
	REAL32				_BombVecPos[3];
}P_BATTLE_M_BOMB_INSTALL_ACK; 

typedef struct __tag_P_BattleMBombUnInstall_Ack
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_iSlotIdx; 
}P_BATTLE_M_BOMB_UNINSTALL_ACK;

typedef struct __tag_P_BattleMDestructionHp_Ack
{
	BATTLE_ROOM_INFO	_RoomInfo;
	DESTRUCTION_INFO	_DestructionInfo; 
}P_BATTLE_M_DESTRUCTION_HP_ACK;

typedef struct __tag_P_BattleMDefence_Ack
{
	BATTLE_ROOM_INFO	_RoomInfo;
	DEFENCE_INFO		_DefenceInfo;
}P_BATTLE_M_DEFENCE_ACK;


typedef struct __tag_P_BattleHackInfo
{
	INT64	_i64UID;
	UINT32	_uSlotIdx;
	UINT32	_uReason;
} P_BATTLE_HACK_INFO;

typedef struct __tag_P_BattleHackUser_Ack
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_uCount; 
}P_BATTLE_HACK_USER_ACK; 

typedef struct __tag_P_ServerState_Req
{
	UINT32	_ui32Active;
	INT32	_iCpu; 
	INT32	_iHandle; 
	INT32	_iArg0; 
	INT32	_iArg1; 
	INT32	_iArg2; 
	INT32	_iArg3; 
	INT32	_iArg4; 
	UINT32	_ui32WarningInfo; 
}P_SERVER_STATE_REQ;

typedef struct __tag_P_BattleMTouchdown_Req
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_GroupIdx; 
	UINT8				_cSlotIdx;
}P_BATTLE_M_TOUCHDOWN_REQ;

#define		MODE_DATA_MAX	100
#define		MODE_TIMER_MAX	5

typedef struct __tag_P_BattleModeHeader
{
	BATTLE_ROOM_INFO	_RoomInfo;
	UINT32				_ui32SendTarget;	// 패킷 받을 유저 (비트 번호 = slot Index)
	UINT32				_ui32SyncPosition;	// 싱크 데이터 위치
}P_BATTLE_MODE_HEADER;

typedef struct __tag_P_BattleServerStartAck
{
	INT32						_i32SlotIdx;
	USER_INFO_EQUIPMENT_CHARA	_UserInfoEquipmentChara;
	UINT8						_ui8EquipUserTitle[MAX_EQUIP_USER_TITLE];
	UINT8						_ui8InteruptEnter;
	UINT16				_TeamScore[TEAM_COUNT];				// 팀 스코어
	SCORE_INFO			BattleUserKillDeath[SLOT_MAX_COUNT];// 킬데스카운트
}P_BATTLE_SERVER_START_ACK;

typedef struct __tag_P_BattleServerStartReq
{
	P_BATTLE_MODE_HEADER		_Header;
	P_BATTLE_SERVER_START_ACK	_Body;
}P_BATTLE_SERVER_START_REQ;

typedef struct __tag_P_BattleServerMissionRoundPreStartAck
{
	UINT16		_ui16PlayUser;			// 게임중인 유저
	UINT8		_ui8InteruptEnter;
}P_BATTLE_SERVER_MISSION_ROUND_PRE_START_ACK;

typedef struct __tag_P_BattleServerMissionRoundStartAck
{
	UINT8				_ui8CurrentRound;					// 현재 라운드
	UINT32				_ui32timeleft;						// 남은 시간
	UINT16				_ui16PlayUser;						// 게임중인 유저
	UINT8				_ui8InteruptEnter;
}P_BATTLE_SERVER_MISSION_ROUND_START_ACK;

typedef struct __tag_P_BattleServerMissionRoundEndAck
{
	UINT8		_ui8WinTeam;
	UINT8		_ui8EndType;
	UINT16		_TeamScore[TEAM_COUNT];
}P_BATTLE_SERVER_MISSION_ROUND_END_ACK;

typedef struct __tag_P_BattleServerEndAck
{
	UINT8		_ui8WinTeam;
	UINT16		_ui16EndUser;
	UINT16		_ui16MissionCompleteUser;
	UINT16		_ui16Exp[SLOT_MAX_COUNT];
	UINT16		_ui16Point[SLOT_MAX_COUNT];
	UINT16		_ui16RawExp[SLOT_MAX_COUNT];
	UINT16		_ui16RawPoint[SLOT_MAX_COUNT];
}P_BATTLE_SERVER_END_ACK;

typedef struct __tag_P_BattleServerTimerEventAck
{
	UINT8		_ui8TimerIdx;
	UINT16		_ui16TimerTime;
}P_BATTLE_SERVER_TIMER_EVENT_ACK;

typedef struct __tag_P_BattleServerSyncScoreAck
{
	UINT32		_ui32TeamScore[TEAM_COUNT];
}P_BATTLE_SERVER_SYNC_SCORE_ACK;

typedef struct __tag_P_BattleServerSyncModedataAck
{
	UINT8		_ui8SyncDataidx;
	INT32		_i32SyncData;
}P_BATTLE_SERVER_SYNC_MODEDATA_ACK;

typedef struct __tag_P_BattleServerInstallUninstallAck
{
	INT32		_i32SlotIdx;
	REAL32		_r32VecPos[3];
	UINT32		_ui32ItemID;
}P_BATTLE_SERVER_INSTALL_UNINSTALL_ACK;

typedef struct __tag_P_BattleRespawnAck
{
	INT32		_i32SlotIdx;
	INT32		_i32RespawnIdx;
	INT32		_i32RespawnObjIdx;			// i3RespawnObj를 로딩한 순서대로 인덱스를 부여해 사용.
}P_BATTLE_SERVER_RESPAWN_ACK;

typedef struct __tag_P_BattleServerEventAlarmAck
{
	INT32		_i32Event;
}P_BATTLE_SERVER_EVENT_ALARM_ACK;

typedef struct __tag_P_BattleServerKeyProcReq
{
	INT32			_i32KeyProcIdx;			// 키 입력 처리 요청시마다 순차적으로 증가시켜서 전달.
	GAME_KEY_MAP	_eKey;					// 입력받은 키
}P_BATTLE_SERVER_KEY_PROC_REQ;

typedef struct __tag_P_BattleServerKeyProcAck
{
	INT32			_i32KeyProcIdx;			// 키 입력 처리 요청시 받은 인덱스값
	INT32			_i32Ret;				// 처리 결과 - 성공시: EVENT_ERROR_SUCCESS
}P_BATTLE_SERVER_KEY_PROC_ACK;

#pragma pack ( pop )

#pragma pack(push, 2)
struct KILLDATA
{
	DEATH_INFO_HEADER	head;
	DEATH_INFO_CHARA	info[ MAX_KILLUSER_COUNT ];
} ;
#pragma pack(pop)

/// RadioChat Type
// RadioChat은 Type * 10 + Number(0~9) 조합으로 UDP통신 패킷을 보냅니다.
enum RADIOCHAT_TYPE
{
	RADIOCHAT_TYPE_COMMAND	= 0,
	RADIOCHAT_TYPE_REQUEST,
	RADIOCHAT_TYPE_STATUS,
	RADIOCHAT_TYPE_AUTO,

	RADIOCHAT_TYPE_COUNT
};

#define _SUPPLY_BOX_

// 보급상자
#pragma pack(push, 1)
typedef struct __tag_SupplyBoxAnnounceSetting
{
	UINT16	ui16NormalStock;			//일반 일일 남은 수량
	UINT16	ui16SilverStock;			//실버 일일 남은 수량
	UINT16	ui16GoldStock;				//골드 일일 남은 수량
	UINT16	ui16ResetTime;				//리셋 시간(HHMM)
	UINT16	ui16NormalStockMax;			//일반 일일 지급 수량
	UINT16	ui16SilverStockMax;			//일반 일일 지급 수량
	UINT16	ui16GoldStockMax;			//일반 일일 지급 수량
} SUPPLY_BOX_ANNOUNCE_SETTING;

typedef struct __tag_SupplyBoxSetting
{
	UINT16	ui16BasicTime;				//기준시간
	UINT16	ui16NextTime;				//상자 재지급 가능 시간
	UINT16	ui16BasicRate;				//지급 확률(100% = 1000으로 표기, 계산후 /1000으로 적용)
	UINT16	ui16RankingBonusRate;		//지급 확률(100% = 1000으로 표기, 계산후 /1000으로 적용)
	UINT16	ui16ResetTime;				//리셋 시간(HHMM)
	UINT32	ui32StartDate;				//시작 시간(YYMMDDHHMM)
	UINT32	ui32EndDate;				//종료 시간(YYMMDDHHMM)
	UINT8	ui8MaxPresentaionCount;		//중복 획득 개수
	UINT16	ui16AnnouncePeriod;			//지급 수량 알림 반복 주기
} SUPPLY_BOX_SETTING;

typedef struct __tag_SupplyBoxPackage
{
	UINT16	ui16NormalStock;			//일반 일일 지급 수량
	UINT16	ui16NormalPrizeStock;		//일반 일일 지급 수량
	UINT8	ui8NormalPresentMessage;	//상자 획득 알림 메시지 설정
	UINT16	ui16SilverStock;			//실버 일일 지급 수량
	UINT16	ui16SilverPrizeStock;		//실버 일일 지급 수량
	UINT8	ui8SilverPresentMessage;	//상자 획득 알림 메시지 설정
	UINT16	ui16GoldStock;				//골드 일일 지급 수량
	UINT16	ui16GoldPrizeStock;			//골드 일일 지급 수량
	UINT8	ui8GoldPresentMessage;		//상자 획득 알림 메시지 설정
} SUPPLY_BOX_PACKAGE;

typedef struct __tag_SupplyBoxStatus
{
	UINT16	ui16NormalPresentaion;		//일반 지급량
	UINT16	ui16NormalPrizePresentaion;	//일반 상금 지급량
	UINT16	ui16SilverPresentaion;		//실버 지급량
	UINT16	ui16SilverPrizePresentaion;	//실버 상금 지급량
	UINT16	ui16GoldPresentaion;		//골드 지급량
	UINT16	ui16GoldPrizePresentaion;	//골드 상금 지급량

	void reset(void)
	{
		ui16NormalPresentaion		= 0;
		ui16NormalPrizePresentaion	= 0;
		ui16SilverPresentaion		= 0;
		ui16SilverPrizePresentaion	= 0;
		ui16GoldPresentaion			= 0;
		ui16GoldPrizePresentaion	= 0;
	}
} SUPPLY_BOX_STATUS;
#pragma pack(pop)

typedef struct __tag_SupplyBoxInfo
{
	SUPPLY_BOX_SETTING	stSupplyBoxSetting;
	SUPPLY_BOX_PACKAGE	stSupplyBoxPackage;
	SUPPLY_BOX_STATUS	stSupplyBoxStatus;
} SUPPLY_BOX_INFO;

enum SUPPLY_BOX_TYPE
{
	SUPPLY_BOX_NONE	= 0,
	SUPPLY_BOX_NORMAL,
	SUPPLY_BOX_SILVER,
	SUPPLY_BOX_GOLD,

	SUPPLY_BOX_TYPE_COUNT
};

enum SUPPLY_BOX_MESSAGE_TYPE
{
	SUPPLY_BOX_MESSAGE_NONE = 0,
	SUPPLY_BOX_MESSAGE_ALL,
	SUPPLY_BOX_MESSAGE_SERVER,
	SUPPLY_BOX_MESSAGE_CHANNEL,
	SUPPLY_BOX_MESSAGE_ROOM,

	SUPPLY_BOX_MESSAGE_TYPE_COUNT
};

#pragma pack(push, 1)
typedef struct __tag_UserRankInfo
{
	INT8	i8SlotIdx;
	INT64	i64UID;
	INT8	i8Ranking;
} USER_RANK_INFO;

typedef struct __tag_UserPresentedInfo
{
	INT8	i8SlotIdx;
	INT8	i8Presented;
	UINT32	ui32ItemID;
} USER_PRESENTED_INFO;
#pragma pack(pop)

// 신고하기 사유
enum REPORTCAUSE
{
	REPORTCAUSE_NONE = -1,
	REPORTCAUSE_CURSE = 0,		// 욕설, 비매너
	REPORTCAUSE_ILLEGALPROGRAM,	// 불법 프로그램 사용
	REPORTCAUSE_USEBUG,			// 버그 악용
	REPORTCAUSE_ABUSE,			// 어뷰즈
	REPORTCAUSE_ETC,			// 기타

	REPORTCAUSE_MAX
};

// 강퇴하기 사유
enum FORCEREMOVE_CAUSE
{
	FORCEREMOVE_NONE = -1,
	FORCEREMOVE_CURSE = 0,			// 욕설, 노매너
	FORCEREMOVE_ILLEGALPROGRAM,		// 불법 프로그램 사용
	FORCEREMOVE_ABUSE,				// 어뷰즈
	FORCEREMOVE_ETC,				// 기타

	FORCEREMOVE_MAX
};

// 신고사유 내용 길이
#define MAX_REPORTCAUSE_SIZE		600		/// 300자

// 튜토리얼 플레이타입
enum TUTORIAL_PLAY_TYPE
{
	TUTORIAL_TRAINING = 0,
	TUTORIAL_TIMEATTACK,
};


// 치팅 유저를 서버에게 알려주기 위한 항목
enum CHEAT_CONTEXT
{

	//CHEAT_CLIENT와 1:1로 대응합니다.

	// 1000 이하는 서버에서 로그만 남기고 짜르지 않음
	CC_NON_CHEATING = 1,
	CC_MANY_GRENADE_COUNT = 410,		//수류탄, RPG7 여러번 던진다.
	// 1000 이상은 로그를 남기고, 서버에서 짜름
	CC_FILE_FALSIFY = 1000,	//파일 변조 치트
	CC_LOAD_BULLET_COUNT_MG,
	CC_LOAD_BULLET_COUNT_OTHERS,
	CC_FIRE_BULLET_COUNT,
	CC_DAUL_KRISS_SUPPER_V,
	CC_LOAD_BULLET_COUNT_ROCKET,
	CC_LOAD_BULLET_COUNT_K400,
	CC_NEED_TO_LOAD_BULLET_SNIPER,
	CC_PHYSIX_MASK,
	CC_MAX_SPEED,
	CC_INVALID_WEAPON,
	CC_INVALID_CHARA,
	CC_INVALID_DESIGN,
	CC_INVALID_EQUIP_CHARA,
	CC_FIRE_DELEY,
	CC_MAGNET,
	CC_KNIFE_SPEED,
	CC_UNKNOWN_POS,
	CC_GRAVITY_HACK,
	CC_AI_HACK,
	CC_WALLSHOT,
	CC_OBSERVER,
	CC_BULLET_COUNT,
	CC_MAX_SPEED_POS,
	CC_QUICK_CHANGE,
	CC_WEAPON_CHANGE,
	CC_AUTOAIM,
	CC_EQUIP_CHANGE,
	CC_GRENADE_POS,
	CC_UNKNOWN_POS2,
	CC_MAX_COUNT,



};

// 치팅 유저를 서버에게 알려주기 위한 항목
enum CHEAT_CLIENT
{
	C_NON_CHEATING = 0,
	C_MANY_GRENADE_COUNT =1,		//수류탄, RPG7 여러번 던진다.
	C_FILE_FALSIFY =2			,	//파일 변조 치트
	C_LOAD_BULLET_COUNT_MG=3,
	C_LOAD_BULLET_COUNT_OTHERS=4,
	C_FIRE_BULLET_COUNT=5,
	C_DAUL_KRISS_SUPPER_V=6,
	C_LOAD_BULLET_COUNT_ROCKET=7,
	C_LOAD_BULLET_COUNT_K400=8,
	C_NEED_TO_LOAD_BULLET_SNIPER=9,
	C_PHYSIX_MASK=10,
	C_MAX_SPEED=11,
	C_INVALID_WEAPON=12,
	C_INVALID_CHARA=13,
	C_INVALID_DESIGN=14,
	C_INVALID_EQUIP_CHARA=15,
	C_FIRE_DELEY=16,
	C_MAGNET=17,
	C_KNIFE_SPEED=18,
	C_UNKNOWN_POS=19,
	C_GRAVITY_HACK=20,
	C_AI_HACK=21,
	C_WALLSHOT=22,
	C_OBSERVER=23,
	C_BULLET_COUNT=24,
	C_MAX_SPEED_POS=25,
	C_QUICK_CHANGE=26,
	C_WEAPON_CHANGE =27,
	C_AUTOAIM       =28,
	C_EQUIP_CHANGE  =29,
	C_GRENADE_POS	=30,
	C_UNKNOWN_POS2  = 31,

	C_MAX_COUNT,


	
};




enum TOUCHDOWN_BLOCK_STATE
{
	TOUCHDOWN_BLOCK_STATE_INIT,
	TOUCHDOWN_BLOCK_STATE_FULL,
};

enum BOUNDRY_NAME
{
	X_START_BOUNDRY	= 0,
	Y_START_BOUNDRY,
	Z_START_BOUNDRY,
	X_END_BOUNDRY,
	Y_END_BOUNDRY,
	Z_END_BOUNDRY,

	MAX_BOUNDRY_NAME
};

// max hacktype:32
// 30개가 넘어가면 문제가 생깁니다. 해킹유형(class)으로 구분해야 합니다.
enum DS_HACK_TYPE
{
	HACK_TYPE_NO = 0,
	// ------------------------
	HACK_TYPE_GRAVITY=1,			// position 
	HACK_TYPE_DAMAGE=2,				// one shot/one kill 
	HACK_TYPE_RANGE=3,				// weapon range
	HACK_TYPE_SPEED=4,				// speed hack
	HACK_TYPE_WALLSHOT=5,			// wall shot
	HACK_TYPE_GRENADE=6,			// max throwing weapon
	HACK_TYPE_MAGAZINE=7,			// infinite bullet (=no magazine change)
	HACK_TYPE_GHOST=8,				// hide my position (구현 안됨)
	HACK_TYPE_RECALL=9,				// recalls other users (=magnet)
	HACK_TYPE_WEAPON=10,			// weapon change
	HACK_TYPE_TRAINING=11,			// 삭제
	HACK_TYPE_FIRE_SPEED=12,		// shotgun/sniper delay hack
	HACK_TYPE_INVINCIBILITY=13,		// ignore invincibility time 
	HACK_TYPE_RESPAWN_TIME=14,		// ignore respawn delay
	HACK_TYPE_RESPAWN_POSITION=15,	// respawn position hack
	HACK_TYPE_INSTALL_C4=16,		// install C4 by hack user 
	HACK_TYPE_CLIENT_TIME=17,		// client time check
	HACK_TYPE_HEADSHOT=18,			// 삭제
	HACK_TYPE_PACKET=19,			// udp 패킷 에러 
	HACK_TYPE_WEAPON_PARAM=20,		// weapon parameter hacking
	HACK_TYPE_WEAPON_NOT_FOUND=21,	// weapon not found (끄는 기능 없음)
	// ------------------------
	HACK_TYPE_MAX	
};

// Hack Check ratio information 
struct ServerHackCheckRatio
{
	UINT8	m_aui8HackCheckRatio[HACK_TYPE_MAX];	

	// Constructor
	ServerHackCheckRatio() 
	{
		SetDefaultValue();
	}

	void SetDefaultValue()
	{
#ifdef _DEBUG
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY]  			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_DAMAGE]   			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_RANGE]	 			= 100;			
		m_aui8HackCheckRatio[HACK_TYPE_SPEED]	 			= 100;			
		m_aui8HackCheckRatio[HACK_TYPE_WALLSHOT] 			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_GRENADE]  			= 100;		
		m_aui8HackCheckRatio[HACK_TYPE_MAGAZINE] 			= 100;		
		m_aui8HackCheckRatio[HACK_TYPE_GHOST]	 			= 0;				
		m_aui8HackCheckRatio[HACK_TYPE_RECALL]	 			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON]	 			= 100;			
		m_aui8HackCheckRatio[HACK_TYPE_TRAINING] 			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_FIRE_SPEED]			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_INVINCIBILITY]		= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_TIME]		= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_POSITION]	= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_INSTALL_C4]			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_CLIENT_TIME]			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_HEADSHOT]			= 100;
		m_aui8HackCheckRatio[HACK_TYPE_PACKET]				= 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_PARAM]		= 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_NOT_FOUND]	= 100;
		
#else
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY]  			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_DAMAGE]   			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_RANGE]	 			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_SPEED]	 			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_WALLSHOT] 			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_GRENADE]  			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_MAGAZINE] 			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_GHOST]	 			= 0;		
		m_aui8HackCheckRatio[HACK_TYPE_RECALL]	 			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON]	 			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_TRAINING] 			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_FIRE_SPEED]			= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_INVINCIBILITY]		= 20;	
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_TIME]		= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_POSITION]	= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_INSTALL_C4]			= 100;	
		m_aui8HackCheckRatio[HACK_TYPE_CLIENT_TIME]			= 100;
		m_aui8HackCheckRatio[HACK_TYPE_HEADSHOT]			= 100;
		m_aui8HackCheckRatio[HACK_TYPE_PACKET]				= 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_PARAM]		= 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_NOT_FOUND]	= 100;
#endif
	}

	UINT8 CopyToPacket( UINT8* pOut, UINT32 ui32BufLen )
	{
		I3ASSERT( pOut != NULL );
		I3ASSERT( ui32BufLen >= HACK_TYPE_MAX );
		if( ui32BufLen < HACK_TYPE_MAX ) return 0;
		
		memcpy_s( pOut, ui32BufLen, m_aui8HackCheckRatio, sizeof(m_aui8HackCheckRatio) );
		return HACK_TYPE_MAX;
	}
};

// Field Shop
#define MAX_FIELDSHOP_CATEGORYNAMESIZE 		44
#define MAX_FIELDSHOP_CATEGORYCOUNT 		6
#define MAX_FIELDSHOP_GOODSLISTCOUNT		9
#define MAX_FIELDSHOP_GOODSPERIODCOUNT		4

////////////////////////////////////////////////////////////////
// 레벨별 아이템 지급 관련
#define MAX_RANK_COUNT						51  // 랭크업아이템을 지급할 레벨의 총 갯수
#define MAX_RANKUP_ITEM_COUNT				4  // 랭크업아이템의 총 갯수

////////////////////////////////////////////////////////////////
// RS. 뽑기 상점 관련

#define MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT 	6   		// 클라이언트에서 보여지는 탭당 등급별 아이템 갯수.
#define MAX_RS_TAB_SHOW_ALL_ITEM_COUNT		18			// 클라이언트에서 보여지는 탭당 총 아이템 갯수.
#define MIN_RS_TAB_ITEM_COUNT 				10			// 웹툴에서 설정하는 한 탭의 등급별 아이템 최소갯수
#define MAX_RS_TAB_ITEM_COUNT 				25			// 웹툴에서 설정하는 한 탭의 등급별 아이템 최대 갯수. 
#define MAX_RS_TAB_COUNT 					3			// 최대 탭 갯수
#define MAX_RS_ROULETTE_ITEM_COUNT			12			// 한 룰렛판이 가지는 최대 아이템 수. 대박 미포함.
#define MIN_RS_TAB_GRADE_COUNT				3			// 한 탭에서 구성할수 있는 별 갯수 합의 최소값.
#define MAX_RS_TAB_GRADE_COUNT				10			// 한 탭에서 구성할수 있는 별 갯수 합의 최대값.
#define MAX_RS_AD_MESSAGE_SIZE				101			// 광고 메시지 길이.
#define MAX_RS_AD_MESSAGE_COUNT				3			// 광고 메시지 갯수.
#define MAX_RS_HIDDEN_ITEM_COUNT			10			// 하나의 히든 세트가 가지는 아이템 개수.
#define	MAX_RS_BONUS_COUNT					5			// 보너스 판수.(기준)
#define RS_ITEM_INFO_UPDATE_TIME			2			// Game 서버에 뽑기상점 상품 구성을 업데이트 해주는 시간

#define MAKE_RS_TAB_GOODSID(tabidx,playcount)	(((GOODS_CLASS_RSPRICE)*10000000 )+((tabidx)*100)+(playcount))	// TabGoodsID 생성. 탭 번호, 이용 횟수

enum RS_ITEM_GRADE			// 아이템 등급
{
	RS_ITEM_GRADE_NOT_SETTING =-1,
	RS_ITEM_GRADE_LOSE		= 0,		// 꽝 상품
	RS_ITEM_GRADE_1			= 1,		// 별 1개
	RS_ITEM_GRADE_2			= 2,		// 별 2개 	
	RS_ITEM_GRADE_3			= 3,		// 별 3개 
	RS_ITEM_GRADE_JACKPOT	= 100,		// 대박 
};

enum RS_TAB			// 탭 번호.
{
	RS_TAB_1				= 0,		// 1번 탭
	RS_TAB_2				= 1,		// 2번 탭
	RS_TAB_3				= 2,		// 3번 탭
	RS_TAB_MAX				= RS_TAB_3,		// 3번 탭
	RS_TAB_COUNT	
};

#pragma pack(push, 1)

struct 	FieldShopInfo 
{
	// Type of a category
	struct Category
	{
		CHAR	m_strCategoryName[MAX_FIELDSHOP_CATEGORYNAMESIZE];

		UINT32	m_ui32GoodsIDList[MAX_FIELDSHOP_GOODSLISTCOUNT][MAX_FIELDSHOP_GOODSPERIODCOUNT];
	};
	
	// Field_Shop version information
	UINT32	m_ui32Version;

	// Category List
 	Category  m_CategoryList[MAX_FIELDSHOP_CATEGORYCOUNT];  

	// Constructor
 	FieldShopInfo::FieldShopInfo() : m_ui32Version(0)
	{
		memset(m_CategoryList, 0, sizeof(Category) * MAX_FIELDSHOP_CATEGORYCOUNT);
	} 
};

struct RS_ITEM_INFO	// 뽑기 상점 아이템 정보. 클라이언트 용.
{       
	UINT8			_ui8RsIndex;		// 해당 탭의 등급에서 고유 인덱스. 0~24
	RS_ITEM_GRADE	_eItemGrade;		// 등급. 별 갯수. 0 : 꽝, 1~3 : 각 등급
	UINT32			_ui32GoodsID;		// GoodsID

	UINT32 			_ui32MaxUnit;		// 아이템 최대 수량.
	UINT32 			_ui32CurUnit;		// 아이템 현재 수량 .

	void Reset()
	{
		 _ui8RsIndex		= 0;
		 _eItemGrade 		= RS_ITEM_GRADE_LOSE;
		 _ui32GoodsID		= 0;
		 _ui32MaxUnit		= 0;
		 _ui32CurUnit		= 0;
	}
};

struct  RS_HIDDEN	// 히든 구성
{
	RS_ITEM_INFO	_aRSItemInfo[MAX_RS_HIDDEN_ITEM_COUNT]; // 아이템 정보
};

struct  RS_TAB_INFO	 // 뽑기 상점 탭 아이템(당첨&꽝) 정보. Game서버 - 클라이언트 간 사용.
{
	UINT8			_ui8TotalItemGrade;	// 구성할수 있는 최대 별 갯수. 3 ~ 10.
	UINT32			_ui32Price;			// 탭 가격( cash )
	RS_ITEM_INFO	_aRSItemInfoG1[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별1개 등급 아이템 정보.
	RS_ITEM_INFO	_aRSItemInfoG2[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별2개 등급 아이템 정보.
	RS_ITEM_INFO	_aRSItemInfoG3[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별3개 등급 아이템 정보.	
	RS_HIDDEN 		_aRSHidden; // 히든 세트
};

#define MAX_RAND_COUNT			10000 // 뽑기상점 확률 최대값

struct	RS_RAND_ITEM_INFO
{
	UINT32			m_ui32Roulette[ MAX_RAND_COUNT ];
	UINT8			m_ui8Idx[ MAX_RAND_COUNT ];
	RS_ITEM_GRADE	m_eGrade[ MAX_RAND_COUNT ];
};

struct  RS_TAB_INFO_TRANS	// 뽑기 상점 탭 정보 - 트랜스서버가 가지고 있는것으로 꽝 목록까지 들고 있습니다.
{
	UINT8			_ui8TotalItemGrade;	// 구성할수 있는 최대 별 갯수. 3 ~ 10.
	UINT32			_ui32Price;			// 탭 가격( cash )
	RS_ITEM_INFO	_aRSItemInfoG1[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별1개 등급 아이템 정보.
	RS_ITEM_INFO	_aRSItemInfoG2[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별2개 등급 아이템 정보.
	RS_ITEM_INFO	_aRSItemInfoG3[MAX_RS_TAB_SHOW_GRADE_ITEM_COUNT]; // 별3개 등급 아이템 정보.
	RS_ITEM_INFO	_aRSItemInfoGLot[ 10 ]; // 꽝 등급 아이템 정보.
	RS_HIDDEN 		_aRSHidden; // 히든 세트
};

struct  RS_R_ITEM_INFO	// 룰렛판을 구성하는 아이템 정보.
{
	RS_ITEM_GRADE	_eItemGrade;		// 등급. 별 갯수. 0 : 꽝, 1~3 : 각 등급
	UINT8			_ui8RsIndex;		// 해당 탭의 등급에서 고유 인덱스. 0~24

	void Reset()
	{
		 _ui8RsIndex		= 0xFF;
		 _eItemGrade 		= RS_ITEM_GRADE_LOSE;
	}
};


struct  RS_ROULETTE_INFO	// 룰렛판 정보
{
	RS_TAB			_eTab;				// 탭 번호. 1~3.
	UINT8			_ui8ItemCount;		// 룰렛판에 들어가는 아이템(꽝템제외) 갯수. 
	RS_R_ITEM_INFO	_RSRitemInfo[MAX_RS_ROULETTE_ITEM_COUNT];	

	void Reset()
	{
		_eTab			= RS_TAB_1;
		_ui8ItemCount 	= 0;
		for(INT32 i=0; i<MAX_RS_ROULETTE_ITEM_COUNT ; i++)	
		{
			_RSRitemInfo[i]._eItemGrade = RS_ITEM_GRADE_LOSE;
			_RSRitemInfo[i]._ui8RsIndex	= 0;
		}

	}
};

struct RS_AD_MESSAGE	// 광고 메시지
{
	char		_strMessage[ MAX_RS_AD_MESSAGE_SIZE ];
};

struct RS_BASE_INFO	// 뽑기 상점 기본 정보. 
{
	UINT32			_ui32JackPotGoodsID;	// 잭팟 아이템 GoodsID.
	char			_strJackPotNick[NET_NICK_NAME_SIZE]; // 가장최근 대박 당첨 유저 닉네임
	RS_AD_MESSAGE	_aRSAdMessage[MAX_RS_AD_MESSAGE_COUNT]; //광고 메시지.
};

struct RS_ALL_ITEM_INFO	// 뽑기 상점 전체 아이템 정보.
{
	RS_TAB_INFO		_aRSTabInfo[MAX_RS_TAB_COUNT];	// 3개 고정.
};

struct RS_JACKPOT_INFO	// 뽑기 상점 잭팟 당첨 정보
{
	BOOL	bIsJackPot;						// 잭팟 당첨 여부. 패킷 전송 실패시 FALSE.
	char	strNick[NET_NICK_NAME_SIZE];	// 잭팟 당첨 유저 닉네임 
	UINT32	ui32GoodsID;					// 잭팟 GoodsID
};

struct STRUCT_BATTLE_SLOT_EQUIPMENT_ACK
{
	BATTLE_ROOM_INFO			m_RoomInfo;
	UINT32						m_ui32GroupIdx;
	UINT8						m_ui8SlotIdx;
	
	// 하단의 두 멤버는 GC_PROTOCOL_SLOT_EQUIPMENT 구조체의 멤버입니다.
	// 데디케이션서버까지 전송된 STRUCT_BATTLE_SLOT_EQUIPMENT_REQ 정보는 다시 전부 되돌아올필요없이 게임서버에서
	// 다시 복구할수 없는 정보인 하단의 두 멤버만 돌려줍니다. 52 Bytes -> 6 Bytes 용량절약
	UINT16						m_ui16ChangeFlag;
	UINT32						m_ui32EquipItem;
};

struct RANKUP_ITEM
{
	UINT8		m_ui8Rank;			// Rank
	UINT32		m_ui32GoodsID[4];	// GoodsID
};


// TS 이벤트 리스트 구조체

struct TS_GOODS_LIST
{
	UINT8		_uiGoodsOrder;
	UINT32		_ui32GoodsID[2];
};

struct TS_EVENT_INFO
{
	UINT32			_ui32UID;			// 이벤트 아이디
	char			_strTSName[MAX_TS_ITEM_STRING];
	UINT8			_ui8TSType;
	UINT32			_ui32StartDate;
	UINT32			_ui32EndDate;
	TS_GOODS_LIST	stGoodsList[MAX_TS_GOODS_LIST];
};

enum	TS_EVENT_TYPE
{
	TS_EVENT_SEND_ITEM_LIST = 0,
	TS_EVENT_REMAIN_TIME,

	TS_EVENT_INFO_MAX
};
// TS 이벤트 클라이언트와 통신
struct	TS_EVENT_ITEM_INFO
{
	UINT8		_ui8Type;
	char		_strTSName[MAX_TS_ITEM_STRING];
	UINT32		_i32GoodsID[2];
	UINT32		_i32PlayTime;
	UINT32		_i32SelGoodsID;
};


typedef struct _tagGiftBuffer
{
	TGiftDxIdx				_i32GiftWareDBIdx;						// DB IDX (삭제할때 사용되어짐)
	UINT32					_ui32GoodsID;							// 선물 Goods ID
	ENUM_TYPE_GIFT_DELIVER	_eDeliver;								// 지급 여부
	UINT32					_ui32Expire;							// 선물 유효기간. ( ex : 1110281115 YYMMDDHHmm)
	char					_strSenderNick[ MAX_USER_NICK ];		// 선물한 유저 닉네임
 	char					_strMessage[ MAX_GIFT_MESSAGE_SIZE ];	// 메시지
}GIFT_BUFFER;

#pragma pack(pop)

// New 유저 찾기용 구조체
#pragma pack(push, 4)
struct	NEW_FIND_USER_INFO
{
	UINT8							m_ui8Rank;			// 랭크
	INT32							m_i32State;			// 유저 상태. MAKE_FRIEND_STATE 이용해서 정보 추출.
	char							m_strClanName[NET_CLAN_NAME_SIZE];	// 클랜 명
	
	USER_INFO_RECORD				m_Record;	// 전적정보
	USER_INFO_EQUIPMENT_WEAPON		m_Weapon;	// 무기정보
	USER_INFO_EQUIPMENT_CHARA		m_Chara;	// 캐릭터정보
};



#pragma pack(pop)

//Hack Type Value
enum HACK_CHECK_TYPE
{
	HACK_CHECK_TYPE_NONE		= 0,
	HACK_CHECK_TYPE_CLIENT		= 1,
	HACK_CHECK_TYPE_SERVER_G	= 2,
	HACK_CHECK_TYPE_SERVER_D	= 3,
};


//--------------------------------------
//	CHARARES_TYPE
//--------------------------------------
enum CHARARES_TYPE
{
	CHARARES_UNKNOWN = 0,
	// 인간
	CHARARES_REBEL_MALE		= 1,	// 1번 캐릭터는 기본적 캐릭터, RED
	CHARARES_SWAT_MALE		= 2,	// 1번 캐릭터는 기본적 캐릭터, Blue
	CHARARES_REBEL_FEMALE	= 3,
	CHARARES_SWAT_FEMALE	= 4,
	CHARARES_D_FOX			= 5,
	CHARARES_LEOPARD		= 6,
	CHARARES_SWAT_HIDE		= 7,

	// 공룡
	CHARARES_DINO_RAPTOR,
	CHARARES_DINO_TREX,
	CHARARES_REBEL_VIPERRED,
	CHARARES_DINO_STING,
	CHARARES_DINO_TANK,
	CHARARES_DINO_ACID,
	CHARARES_DINO_ELITE,

	//인간캐릭터 추가
	CHARARES_CHOU,
	CHARARES_BELLA,

	// 제압모드 공룡
	CHARARES_DINO_RAPTOR_BLUE_STAIN,
	CHARARES_DINO_RAPTOR_GREEN_STRIPES,
	CHARARES_DINO_MUTANT_TREX,
	CHARARES_DINO_ACID_NOT_BOMB,

	// CC 모드 공룡
	CHARARES_DINO_CC_RAPTOR,
	CHARARES_DINO_CC_STING,
	CHARARES_DINO_CC_ACID,
	CHARARES_DINO_RAPTOR_MERCURY,
	CHARARES_DINO_STING_MARS,
	CHARARES_DINO_ACID_VULCAN,

	CHARARES_TYPE_COUNT // 캐릭터 추가시 아래 함수도 수정해야 합니다
};

inline
BOOL IsDinoResType( CHARARES_TYPE type )
{
	switch ( type )
	{
	case CHARARES_DINO_RAPTOR:
	case CHARARES_DINO_TREX:
	case CHARARES_REBEL_VIPERRED:
	case CHARARES_DINO_STING:
	case CHARARES_DINO_TANK:
	case CHARARES_DINO_ACID:
	case CHARARES_DINO_ELITE:
	case CHARARES_DINO_RAPTOR_BLUE_STAIN:
	case CHARARES_DINO_RAPTOR_GREEN_STRIPES:
	case CHARARES_DINO_MUTANT_TREX:
	case CHARARES_DINO_ACID_NOT_BOMB:
	case CHARARES_DINO_CC_RAPTOR:
	case CHARARES_DINO_CC_STING:
	case CHARARES_DINO_CC_ACID:
	case CHARARES_DINO_RAPTOR_MERCURY:
	case CHARARES_DINO_STING_MARS:
	case CHARARES_DINO_ACID_VULCAN:
		return TRUE;
	}
	
	return FALSE;
}



// --------------------------------------------------------------------------------------------------
// 클라에서 임의로 사용하는 weapon (예: Grenade Shell, Dummy, Extension...)을 원래의 weapon으로 변환.
// : weapon change 검사시 같은 무기인지 확인하기 위한 용도로 사용됩니다.
// --------------------------------------------------------------------------------------------------

void ChangeDummyWeapon( const UINT8 ServerClass, const UINT8 ServerNumber, UINT8& WeaponClass, UINT8& WeaponNumber, UINT8& WeaponUsage );
void ChangeDummyWeapon( const UINT8 ServerClass, const UINT8 ServerNumber, WEAPON_CLASS_TYPE& WeaponClass, INT32& WeaponNumber, WEAPON_USAGE_TYPE& WeaponUsage );

// --------------------------------------------------------------------------------------------------
// Client Weapon ID, Item ID 사이의 상호 변환 함수
// : UDP 패킷에 포함된 weapon을 ChangeDummyWeapon(), GetItemIDFromWeaponID() 순서로 호출하면
//   반드시 인벤에 포함된 weapon의 ItemID가 되어야 합니다. 
// --------------------------------------------------------------------------------------------------

ITEM_CLASS_TYPE	GetItemClass_TempFunc(WEAPON_USAGE_TYPE Usage, WEAPON_CLASS_TYPE ItemType);

inline
UINT32	GetItemIDFromWeaponID ( const UINT32 ui8Class, const UINT8 ui8Number, const UINT8 ui8Usage )
{
	UINT32 ItemClass    = GetItemClass_TempFunc( (WEAPON_USAGE_TYPE)ui8Usage, (WEAPON_CLASS_TYPE)ui8Class  );
	
	I3ASSERT( ItemClass ==   ITEM_CLASS_TEMP || (ItemClass >=  ITEM_CLASS_ASSAULT && ItemClass <=  ITEM_CLASS_THROWING) );
	
	return  MAKE_ITEM_FLAG( ItemClass, ui8Usage, ui8Class, ui8Number );
}

inline
void	GetWeaponIDFromItemID( const UINT32 ui32ItemID, UINT8& ui8Class, UINT8& ui8Number, UINT8& ui8Usage )
{
	I3ASSERT(	GET_ITEM_FLAG_CLASS( ui32ItemID ) >=  ITEM_CLASS_ASSAULT &&
				GET_ITEM_FLAG_CLASS( ui32ItemID ) <=  ITEM_CLASS_THROWING );
	
	ui8Class	= (UINT8) GET_ITEM_FLAG_TYPE	( ui32ItemID ); 
	ui8Number	= (UINT8) GET_ITEM_FLAG_NUMBER	( ui32ItemID );
	ui8Usage	= (UINT8) GET_ITEM_FLAG_USAGE	( ui32ItemID );
}

inline
UINT32	GetItemIDFromEquipID ( UINT8 ui8Class,const UINT8 ui8Number)
{
	return MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, ui8Class, CHARACTER_TEAM_RB, ui8Number);
}


#endif // #ifndef GAMEPORTAL_BUILD

#endif // #ifndef BUILD_VERSION_TOOL

#endif // #ifndef __COMMONDEF_H