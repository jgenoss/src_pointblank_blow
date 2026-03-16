#ifndef __COMMONDEF_H
#define	__COMMONDEF_H

#include <map>
#include <vector>
#include <string>

#include "BuildVerInfo.h"

#define GAME_VER_MONTH		2
#define GAME_VER_DAY		19

#define VER_SS_PROTOCOL		3
#define VER_UDP_PROTOCOL_MAIN	1508	// v.1.5, 2015.1.5
#define VER_UDP_PROTOCOL_SUB	7		// seq no.

/****************************************************************************************/
//					국가 설정
//
//	외국일 경우 국가에 맞게 Build Symbol을 Preprocessor Definitions에 추가 바람
//	LOCALE_CODE 에 해당 나라 값을 넣어주세요.
//
//	한국:		LOCALE_KOREA
//	태국:		LOCALE_THAILAND
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
//  통합빌드:	LOCALE_CODE_TOTAL
//  싱말 :		LOCALE_SINGMAL
//	베트남 :	LOCALE_VIETNAM
/****************************************************************************************/
enum LOCALE_CODE
{
	LOCALE_CODE_NONE = 0,
	LOCALE_CODE_KOREA = 1,
	LOCALE_CODE_THAILAND = 2,
	LOCALE_CODE_INDONESIA = 3,
	LOCALE_CODE_RUSSIA = 4,
	LOCALE_CODE_TURKEY = 5,
	LOCALE_CODE_CHINA = 6,
	LOCALE_CODE_TAIWAN = 7,
	LOCALE_CODE_JAPAN = 8,
	LOCALE_CODE_BRAZIL = 9,
	LOCALE_CODE_LATIN_AMERICA = 10,
	LOCALE_CODE_NORTHAMERICA = 11,
	LOCALE_CODE_ITALY = 12,
	LOCALE_CODE_MIDDLE_EAST = 13,
	LOCALE_CODE_PHILIPPINES = 14,
	LOCALE_CODE_MATCH = 15,
	LOCALE_CODE_SINGMAL = 16,
	LOCALE_CODE_VIETNAM = 17,
	LOCALE_CODE_KOREA_DAUM = 18,
	LOCALE_CODE_KOREA_PICAON = 19,
	LOCALE_CODE_STEAM = 20,
	LOCALE_CODE_KOREA_GREEN_VOLT = 21,
	LOCALE_CODE_MAX,
};

/******************************************************/
//	연동 Method enum
//	
//	국가별 외부연동 처리를 구분하기 위한 enum값 입니다.
//	기본 1국가 1연동이므로 LINK_METHOD_국가명이
//	기본 네이밍 규칙입니다.
//	북미-스팀연동 처럼 1국가 n연동인 경우 국가명+연동방식을
//	적절히 혼합하여 네이밍 하도록합니다.
/******************************************************/
enum LINK_METHOD
{
	LINK_METHOD_NONE = 0,
	LINK_METHOD_ZPT = 1,
	LINK_METHOD_INDONESIA = 2,
	LINK_METHOD_RUSSIA = 3,
	LINK_METHOD_THAI = 4,
	LINK_METHOD_KOREA = 5,
	LINK_METHOD_TURKY = 6,
	LINK_METHOD_BRAZIL = 7,
	LINK_METHOD_LATINAMERICA = 8,
	LINK_METHOD_NORTHAMERICA = 9,
	LINK_METHOD_CHINA = 10,
	LINK_METHOD_ITALY = 11,
	LINK_METHOD_PHILIPPINES = 12,	// 필리핀 빌링 연동 추가	2011.06.02 안국정
	LINK_METHOD_MIDDLEEAST = 13,	// MENA 빌링 연동 추가 2011.11.23 최연주
	LINK_METHOD_TAIWAN = 14,	// 대만. 빌링 연동 DB.
	LINK_METHOD_SM = 15,	// 싱가폴말레이시아
	LINK_METHOD_VIETNAM = 16,	// 베트남
	LINK_METHOD_NORTHA_STEAM = 17,	// 북미 스팀 연동
	LINK_METHOD_MAX
};

#if defined( LOCALE_KOREA)
#if defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || defined( LOCALE_CHINA ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 
#elif defined( LOCALE_THAILAND)
#if defined( LOCALE_KOREA ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || defined( LOCALE_CHINA ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_INDONESIA)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || defined( LOCALE_CHINA ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_RUSSIA)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_TURKEY ) || defined( LOCALE_CHINA ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_TURKEY)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_CHINA ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_CHINA)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_BRAZIL)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_LATIN_AMERICA)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_NORTHAMERICA)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_ITALY ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_ITALY)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ENGLISH ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_ENGLISH)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_MIDDLE_EAST)
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_PHILIPPINES )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif 	
#elif defined( LOCALE_TAIWAN )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TOTAL ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif
#elif defined( LOCALE_TOTAL )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_SINGMAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif
#elif defined( LOCALE_SINGMAL )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || \
		defined( LOCALE_VIETNAM ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif
#elif defined( LOCALE_VIETNAM )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || \
		defined( LOCALE_SINGMAL ) || defined( LOCALE_JAPAN )
#error "You must select locale by defining one"
#endif
#elif defined( LOCALE_JAPAN )
#if defined( LOCALE_KOREA ) || defined( LOCALE_THAILAND ) || defined( LOCALE_INDONESIA ) || defined( LOCALE_RUSSIA ) || defined( LOCALE_TURKEY ) || \
		defined( LOCALE_CHINA ) || defined( LOCALE_BRAZIL ) || defined( LOCALE_LATIN_AMERICA ) || defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_ITALY ) || \
		defined( LOCALE_ENGLISH ) || defined( LOCALE_MIDDLE_EAST ) || defined( LOCALE_PHILIPPINES ) || defined( LOCALE_TAIWAN ) || defined( LOCALE_TOTAL ) || \
		defined( LOCALE_SINGMAL ) || defined( LOCALE_VIETNAM )
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
#define _HACK_SHIELD_
#define  USING_HSMS

#elif defined( LOCALE_THAILAND)
#define _HACK_SHIELD_
#define USING_HSMS

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
#define _XIGNCODE_

#elif defined( LOCALE_LATIN_AMERICA)
//	#define _XTRAP_

#elif defined( LOCALE_NORTHAMERICA)
//			#define _XIGNCODE_

#elif defined ( LOCALE_ENGLISH)

#elif defined ( LOCALE_MIDDLE_EAST )
#define _HACK_SHIELD_
#define USING_HSMS

#elif defined ( LOCALE_PHILIPPINES  )
#define _XIGNCODE_

#elif defined ( LOCALE_TAIWAN )
	// 웹로그인을 끄면 핵쉴드를 사용 안함
#ifndef LOCALE_TAIWAN_NO_WEBLOGIN
#define _HACK_SHIELD_
#define USING_HSMS
#endif

#elif defined ( LOCALE_SINGMAL )
#define _XIGNCODE_


#elif defined ( LOCALE_VIETNAM )


#elif defined ( LOCALE_JAPAN )
#define _XIGNCODE_
//#define _HACK_SHIELD_
//#define USING_HSMS
#else
#endif

#if defined( BUILD_RELEASE_QA_VERSION)
#undef _XIGNCODE_							//	XIGNCODE 사용 안함 (QA)
#undef _HACK_SHIELD_						//	V3 핵쉴드 사용 안함.
#undef _XTRAP_								//	XTrap 사용 안함. (QA)
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
#define	NEW_WEAPON_STATE					// 신규 무기 디파인
#define USE_GAMMAOPTION_BY_SHADER
#elif defined (LOCALE_KOREA)

#define	LOCALE_NUMBER	LOCALE_CODE_KOREA	// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT					// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500					// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_THAILAND)

#define	LOCALE_NUMBER	LOCALE_CODE_THAILAND	// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_INDONESIA)

#define	LOCALE_NUMBER	LOCALE_CODE_INDONESIA		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_MIDDLE_EAST)

#define	LOCALE_NUMBER	LOCALE_CODE_MIDDLE_EAST		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_RUSSIA)

#define	LOCALE_NUMBER	LOCALE_CODE_RUSSIA		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_TURKEY)

#define	LOCALE_NUMBER	LOCALE_CODE_TURKEY		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_CHINA)

#define	LOCALE_NUMBER	LOCALE_CODE_CHINA		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트
#define LINK_SDOA_LOGIN
#define LINK_SDOA_FCM_INFO
#define	LINK_SDOA_FCM
#define LINK_SDOA_BILLING

#define	USE_SDOA_LOGIN				// 중국 인증 모듈을 사용하는 경우

#define USE_XMAS_POPUP				// 크리스마스 팝업 사용 유무

#elif defined (LOCALE_BRAZIL)

#define	LOCALE_NUMBER	LOCALE_CODE_BRAZIL		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT						// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500						// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_LATIN_AMERICA)

#define	LOCALE_NUMBER	LOCALE_CODE_LATIN_AMERICA	// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_NORTHAMERICA)

#define	LOCALE_NUMBER	LOCALE_CODE_NORTHAMERICA	// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_ITALY)

#define	LOCALE_NUMBER	LOCALE_CODE_ITALY			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_ENGLISH)

#define	LOCALE_NUMBER	LOCALE_CODE_MATCH			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_MIDDLE_EAST)

#define	LOCALE_NUMBER	LOCALE_CODE_MIDDLE_EAST		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_PHILIPPINES )

#define	LOCALE_NUMBER	LOCALE_CODE_PHILIPPINES		// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_TAIWAN)

#define	LOCALE_NUMBER	LOCALE_CODE_TAIWAN			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_SINGMAL)

#define	LOCALE_NUMBER	LOCALE_CODE_SINGMAL			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_VIETNAM)

#define	LOCALE_NUMBER	LOCALE_CODE_VIETNAM			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined (LOCALE_MATCH)

#define	LOCALE_NUMBER	LOCALE_CODE_MATCH			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#elif defined(LOCALE_JAPAN)

#define	LOCALE_NUMBER	LOCALE_CODE_JAPAN			// 나라 번호입니다.
#define	USE_PACKET_ENCRIPT							// 패킷 암호화를 합니다.
#define SUBTRACTE_POINT	500							// 유저 전적 상세보기시 차감되는 포인트

#else

#define	LOCALE_NUMBER	LOCALE_CODE_NONE	// 나라 번호입니다.
// 공통 빌드
// 전적 시스템
#define SUBTRACTE_POINT	500					// 유저 전적 상세보기시 차감되는 포인트

#endif

// USE  Unicode_Multibyte String 
// TypeCast
typedef		INT64		T_UID;			// 유저 고유 번호
typedef		INT32		CTUID;			// 
typedef		INT32		T_ItemDBIdx;	// 인벤토리 Item Idx
typedef		UINT32		T_GoodsID;		// 상품 ID
typedef		UINT32		T_ItemID;		// 아이템 ID
typedef		INT32		T_GiftDBIdx;	// 선물하기 Item Idx
typedef		INT16		TItemCnt;		// 인벤 카운터
typedef		INT32		TWebGiftDxIdx;
typedef		UINT32		T_ItemArg;		// 아이템 Arg
typedef		UINT8		T_ItemState;	// 아이템 State
typedef		UINT16		T_OBJ;			// Dedi 에서 사용되는 Object 컬럼
typedef		UINT16		T_MerID;		// 용병 고유 번호.(1개 서버기준)
typedef		UINT16		T_TeamID;		// 클랜 매치팀 고유 번호.(1개 서버기준)
typedef		UINT16		T_MatchingID;	// 클랜 매치메이킹 고유 번호.(1개 서버기준)
typedef		INT32		T_UserKickIdx;
typedef		wchar_t		TTCHAR;
typedef		UINT16		TLoopCnt;		// 프로토콜로 전송하는 Loop Count 수

#define		_TT(x)		L ## x

#define INVALID_ITEM_ID					0 // T_ItemID 의 Invalid ItemID를 찾을수없어서 소스내용을 추측해서 넣음, 
// 틀렸거나 다른데있으면 삭제및 수정요망

// UINT16으로 표현되는 0-based Index이며, 0~65534(0x0000 ~ 0xFFFD)까지 사용 가능하다. 
//65534(0xFFFE) 값은 NONE의 의미로 사용된다. 
//65535(0xFFFF) 값은 Random의 의미로 사용된다. 
typedef UINT16			QUEST_ID;

#ifndef BUILD_VERSION_TOOL		// 빌드버전툴은 여기까지만 읽음

#define CHARA_MEMORY_BLOCK		// 캐릭터 정보 메모리 블럭

#define CC_DINO

#define	DOMINATION

#define USE_SALE_COUPON			// 할인쿠폰 사용

#define USE_DAILY_RECORD		// 당일 전적 사용, TS 보상 플레이시간

#define CHANGE_USURPATION
//#define USE_BLOCK_LIST		// 차단리스트 사용

#define NET_CLAN_OPERATION_SIZE		64			// 클랜 작전명 내용
#define MAX_REPORT_SCREENSHOT_COUNT	3
#define CHECK_TIMEOUT				0.5f	// Sec. 500ms

#define PACKET_BUFFERSIZE_MTU		1400		// MTU 기준에 맞춘 패킷 하나의 최대 사이즈.

#define NET_CHARA_NICK_NAME_SIZE	33			// 한글/영문 16자 (note. 다른 언어일 경우 16자 아닐 수 있음.)

#define NET_CLAN_NAME_SIZE			17			// 클랜명
#define CLAN_MARK_DEFAULT			0xFFFFFFFF	// 클랜마크 default

// 썸머타임 ( 사용, 미사용, 시스템설정에 따름 )
#define SUMMER_TIME_USE				1			// A value greater than 0 to indicate that daylight saving time is in effect.
#define SUMMER_TIME_NOT_USE			0			// Zero (0) to indicate that standard time is in effect.

#define	MAX_AT_PACKET				2000

#define BILLING_ID_SIZE				128
#define EXT_UID_SIZE				256
#define WEB_LOGINKEY_SIZE			256

#define		XIGNCODE_PACKET_SIZE	512

//#define __USE_PACKET_OPTIMIZE__
//#define __USE_TOTAL_OBJECT__

enum PB_VER
{
	// 클라이언트, Trans, Dedi, Game 서버 사용
	PB_VER_V10 = 0,	// V3 - v1.0,
	PB_VER_V15,		// V3 - v1.5 

	// Game 서버만 사용.
	PB_VER_V3,		// V3  내부 서버용. 클라이언트의 버전을 사용
	PB_VER_PASS,	// 사용안함. V2. 내부 서버용. V2 태그용. 모든 장비 체크 PASS 시킴.

	PB_VER_COUNT
};

#pragma pack( push, 1)
//=================================================================================
//	New Version Format
//=================================================================================
struct SC_VERSION	// 서버-클라
{
	UINT8	_ui8VerGame = VER_GAME;		// 게임 대분류
	UINT16	_ui16VerSC = VER_SC_PROTOCOL;			// 서버-클라 패킷 버전

	BOOL	VersionCompare(SC_VERSION* pVersion)
	{
		if (_ui8VerGame != pVersion->_ui8VerGame)		return FALSE;
		if (_ui16VerSC != pVersion->_ui16VerSC)		return FALSE;

		return TRUE;
	}
};

//=================================================================================
struct SS_VERSION	//서버-서버
{
	UINT8	_ui8VerGame = VER_GAME;		// 게임 대분류
	UINT16	_ui16VerSS = VER_SS_PROTOCOL;			// 서버-서버 패킷 버전

	BOOL	VersionCompare(SS_VERSION* pVersion)
	{
		if (_ui8VerGame != pVersion->_ui8VerGame)		return FALSE;
		if (_ui16VerSS != pVersion->_ui16VerSS)		return FALSE;

		return TRUE;
	}
};
//=================================================================================
#pragma pack( pop )

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

enum CHAR_EQUIPMENT
{
	CHAR_EQUIPMENT_NONE = -1,

	// Weapon Slots
	CHAR_EQUIPMENT_PRIMARY = 0,
	CHAR_EQUIPMENT_SECONDARY,
	CHAR_EQUIPMENT_MELEE,
	CHAR_EQUIPMENT_THROWING1,
	CHAR_EQUIPMENT_THROWING2,

	// Parts Slots
	CHAR_EQUIPMENT_CHARA,
	CHAR_EQUIPMENT_HEAD,
	CHAR_EQUIPMENT_FACE,
	CHAR_EQUIPMENT_UPPER,
	CHAR_EQUIPMENT_LOWER,
	CHAR_EQUIPMENT_GLOVE,
	CHAR_EQUIPMENT_BELT,
	CHAR_EQUIPMENT_HOLSTER,
	CHAR_EQUIPMENT_SKIN,
	CHAR_EQUIPMENT_BERET,
	CHAR_EQUIPMENT_COUNT,
};


enum CHAR_EQUIPMENT_COMMON
{
	CHAR_EQUIPMENT_COMMON_NONE = -1,

	CHAR_EQUIPMENT_COMMON_DINO = 0,
	CHAR_EQUIPMENT_COMMON_COUNT,
};

enum CHAR_EQUIPMENT_WEAPON
{
	CHAR_EQUIPMENT_WEAPON_NONE = -1,

	// Weapon Slots
	CHAR_EQUIPMENT_WEAPON_PRIMARY = 0,
	CHAR_EQUIPMENT_WEAPON_SECONDARY,
	CHAR_EQUIPMENT_WEAPON_MELEE,
	CHAR_EQUIPMENT_WEAPON_THROWING1,
	CHAR_EQUIPMENT_WEAPON_THROWING2,

	CHAR_EQUIPMENT_WEAPON_COUNT,
};

// ** 주의 : 중간에 끼워넣거나 순서 바꾸지 마세요!!
enum CHAR_EQUIPMENT_PARTS
{
	CHAR_EQUIPMENT_PARTS_NONE = -1,

	CHAR_EQUIPMENT_PARTS_CHARA = 0,
	CHAR_EQUIPMENT_PARTS_HEAD,
	CHAR_EQUIPMENT_PARTS_FACE,
	CHAR_EQUIPMENT_PARTS_UPPER,
	CHAR_EQUIPMENT_PARTS_LOWER,
	CHAR_EQUIPMENT_PARTS_GLOVE,
	CHAR_EQUIPMENT_PARTS_BELT,
	CHAR_EQUIPMENT_PARTS_HOLSTER,
	CHAR_EQUIPMENT_PARTS_SKIN,
	CHAR_EQUIPMENT_PARTS_BERET,

	CHAR_EQUIPMENT_PARTS_COUNT,
};


////// CHAR_EQUIPMENT_PARTS 로 대체
//enum BATTLE_EQUIPMENT_PARTS
//{
//	BATTLE_EQUIPMENT_PARTS_NONE			= -1,
//
//	// Parts Slots
//	BATTLE_EQUIPMENT_PARTS_HEAD,
//	BATTLE_EQUIPMENT_PARTS_FACE,
//	BATTLE_EQUIPMENT_PARTS_UPPER,
//	BATTLE_EQUIPMENT_PARTS_LOWER,
//	BATTLE_EQUIPMENT_PARTS_GLOVE,
//	BATTLE_EQUIPMENT_PARTS_BELT,
//	BATTLE_EQUIPMENT_PARTS_HOLSTER,
//	BATTLE_EQUIPMENT_PARTS_SKIN,
//	BATTLE_EQUIPMENT_PARTS_DINO,
//	BATTLE_EQUIPMENT_PARTS_COUNT,
//};

//Sub Task 
enum GAME_TASK_SUB
{
	GAME_TASK_SUB_NONE = 0,
	GAME_TASK_SUB_INVENTORY,	//인벤토리
	GAME_TASK_SUB_SHOP,			//숍
	GAME_TASK_SUB_INFO,			//개인정보
	GAME_TASK_SUB_CLAN,			//개인정보
	GAME_TASK_SUB_RS,			//뽑기상점
	GAME_TASK_SUB_GACHA,
	GAME_TASK_SUB_GIFTSHOP,		//선물상점
};

//////////////////////////////////////////////////////////////////////////
// 데이터 초기값
#define	INIT_UINT8		255
#define	INIT_0XFFF		4095
#define	INIT_UINT16		65535
#define	INIT_UINT32		4294967295
#define	INIT_UINT64		-1

/////////////////////////////////////////////////
// GPK
/////////////////////////////////////////////////
#define GPK_SETINFO_PACKETBUFF_SIZE				2048
#define GPK_SETINFO_PACKETBUFF_SIZE_NP			2047


#define INVALID_BUFFER_INDEX	0xFFFFFFFF
#define INVALID_GOODS_ID		0xFFFFFFFF


enum GACHA_SHOP_INITIALIZE
{
	INVALID_PRICE = 0x7FFFFFFF,
	INVALID_ITEM_KEY = 0x7FFFFFFF,
	INVALID_INDEX = 0x7FFFFFFF,
};

INT16 g_Encryption(char* pPacket, INT16 iPacketsize, INT16 iBuffersize);
INT16 g_Decryption(char* pPacket, INT16 iPacketsize, INT16 iBuffersize);

//버퍼생성 카운트 
inline	bool	IsLogInBlockError(UINT32 Error)
{
	return 0 != ((Error & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK) & (~0x80000000 & EVENT_ERROR_EVENT_LOG_IN_BLOCK_HEADER_MASK));
}

#define MAX_LOG_STRING_COUNT		1024
#define CONVERT_STRING_COUNT		1024

#define MAX_GAME_SERVER_COUNT		100			// 최대 게임서버 갯수
#define GAME_CHANNEL_COUNT			10

#define SHOP_TAG_COUNT				32			//상점 태그 개수
// ACCOUNT_KICK_TYPE
#define ACCOUNT_KICK_LOGIN			0x01			// 중복로그인으로 인한 킥
#define ACCOUNT_KICK_GM_TOOL		0x02			// 운영툴에 의한 킥
#define ACCOUNT_KICK_OTHER			0xFF			// 그 외 다른 이유로 인한 킥

//Size 
//////////////////////////////////////////////////////////////////////////
// SDOA
//////////////////////////////////////////////////////////////////////////
#define NET_SNDA_ID					16
#define NET_SNDA_UNIQUE_ID			33
#define NET_CLIENT_IP				16
#define NET_CALL_TIME				20

#define MAX_ITEM_INFO				128
#define MAX_COST_INFO				128

#define MAX_SEQUENCEID				33
#define MAX_ORDERID					33
#define MAX_REDIRECTURL				2049			// 중국 빌링 인증 재문의 URL

#define MAX_BATCHID					10

//////////////////////////////////////////////////////////////////////////
#define MAX_URL_SIZE				1024			// 중국 실명인증 URL
#define MAX_RANKING_URL_SIZE		255				// 제압모드 랭킹 URL
#define NET_ID_SIZE					65
#define NET_PW_SIZE					32
#define NET_SERVER_PW				12
#define NET_ROOM_PW					4				// null 종료 문자열 아님
#define NET_NICK_NAME_SIZE			33				// 한글/영문 16자 (note. 다른 언어일 경우 16자 아닐 수 있음.)
#define DINO_NICK_SIZE				128
#define NET_ROOM_NAME_SIZE			23
#define NET_NCGUSID_SIZE			16
#define NET_ENCRYPTED_TOKEN_SIZE	1000  // 347->500, 한국의 웹런처토큰데이터의 크기증가로인해(셧다운제 데이터포함)	// Token 최대크기
#define NET_SESSION_KEY_SIZE		(NET_ENCRYPTED_TOKEN_SIZE+1)
#define INVEN_ITEMDATA_SIZE			33
#define TURKEY_UID_SIZE				50				// 로그인시 웹에서 받아오는 UID(Game UID가 아님 ). NULL 포함. 터키사용.
#define LINK_UID_SIZE				21				// 로그인시 웹에서 받아오는 UID(Game UID가 아님 ). NULL 포함. 터키 제외한 국가들 사용.
#define RSA_KET_SIZE				1024
#define DX_DLL_MD5_SIZE				33
#define MAX_CLIENT_END_REASON		129
#define MAX_NICK_NAME_LOG_SIZE		67				// 닉네임 Zlog기록을 위한 사이즈.

#define NORTH_UID_SIZE				21				//NULL 포함 
#define NORTH_CUSTOM_MSG_SIZE		128				//북미 커스텀 메세지 크기 (2011.06.10 양승천). 터키는 tempkey.
#define COUNTRY_CODE_SIZE			3

#define KOREA_UID_SIZE				32
#define KOREA_ACCOUNTID_SIZE		50
#define KOREA_ENCRIPT_UID_SIZE		50
#define KOREA_BIRTHDAY_SIZE			16
#define KOREA_SERVICE_CODE_SIZE		2
#define KOREA_PC_CAFE_SIZE			4
#define SHUTDOWN_TIME_SIZE			168

#define MACRO_MAX_COUNT				5
#define MACRO_MAX_LENGHT			255
#define	VIEW_ROOM_COUNT				300				//채널당 명수랑 같아야 합니다. 
#define	SEND_MAX_ROOM_COUNT			10				//몇개의 카운터는 정확해야한다.
#define	SEND_MAX_LOBBY_USER_COUNT	5				//게임서버에서 보내는 로비 유저 카운트
#define VIEW_CHANNEL_COUNT			10				//GAME_CHANNEL_COUNT와 뭐가 다른가요? (영권)
#define MAX_TEAM_USER				8				// 팀당 최대 플레이어 수
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
#define BEGIN3_CHANNEL_CONDITION_TIME	36000		// 배틀시간으로 제한하는 초보채널3의 입장가능기준(이하) 플레이 초

#define MISSION_POINT_MAX_COUNT		6				// 미션 보너스 포인트가 지급되는 최대 회수의 기준 단위 개수 (5, 10, 15, 20, 25, 30)

// 메시데이터가 가장 많은 스테이지의 메시데이터 갯수
#define MAX_NAVIMESH_COUNT			8192			// 메시데이터가 가장 많은 스테이지의 메시데이터 갯수
// 길찾기를 통해 얻은 메시이동경로의 최대갯수
#define	MAX_WAYPATH_COUNT			512				// 길찾기를 통해 얻은 메시이동경로의 최대갯수

#define MAX_AI_CHARA_COUNT			8
#define MAX_KILLUSER_COUNT			16				// 최대 킬 카운터

#define NEW_MEDAL_FLAG_SIZE			32				// NEW표시를 위한 저장소

#define GM_COMMENT_SIZE				255				// GM 명령시 첨부하는 Comment 길이

#define MAX_USER_NICK				33					// 유저 닉네임 길이

#define MIN_ASSIST_DAMAGE			10				//어시스트 획득 가능 최소 데미지

#define BLOCK_COMMENT_SIZE			100				//Block 사유 출력 길이

#define GM_CHEAT_RANDOMBOX_MIN		1				// randombox cheatkey 최소 값
#define GM_CHEAT_RANDOMBOX_MAX		1000			// randombox cheatkey 최대 값

//#define DEF_OBSERVER_MODE				// 옵저버 모드
#define USE_COLOR_NICK2

/////////////////////////////////////////////////
// 실명인증 시스템
// adult 는 m_ui32BirthDay임( Y : 19800101, N : 20100101 )
// applingAdult 는 실명인증임 : ( N :APPLING_ADULT_NO, Y : APPLING_ADULT_YES )
//
// 피로도 시스템 필요 없을 시: adult(BirthDay) == Y
// 피로도 시스템 필요함：(applingAdult == Y && adult == N) or (applingAdult == N && adult == N)
//
// 성인 유저（adult == Y） ，중독 방지 시스템 적용치 않음
// 미성년 유저（applingAdult == Y && adult == N），중독 방지 시스템 적용, 게임 접속 가능.
//【실명 정보가 입력되지 않았으며 2010.8.1이전의 유저】
// applingAdult == N && adult == N），중독 방지 시스템 적용, 게임접속 허용.
// 메시지는 미성년 유저의 메시지 참조
// 실명 정보가 입력되지 않았으며 2010.8.1 이후의 유저】
// 샨다 로그인 도우미 표준 버전이 실명 입력이 필요한 유저를 처리할 것이며 클라이언트 및 서버 단말에 추가 개발이 필요 없습니다.
/////////////////////////////////////////////////
enum APPLING_ADULT_TYPE
{
	APPLING_ADULT_NO = 0,
	APPLING_ADULT_YES,
};

enum ADULT_TYPE
{
	ADULT = 0,
	KID,
};

#define RENDEZVOUS_SEND_PORT_COUNT			2
#define RENDEZVOUS_RECV_PORT_COUNT			4

enum	eINVENTORY_FLAG
{
	INVENTORY_FLAG_PARTS = 0x00000001,
	INVENTORY_FLAG_WEAPON = 0x00000002,
	INVENTORY_FLAG_COMMON_PARTS = 0x00000004,
	INVENTORY_FLAG_COMMON_WEAPON = 0x00000008,
	INVENTORY_FLAG_ITEM = 0x00000010,
};

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
	PC_GARENA_GCA,					// 태국 가레나 GCA 피시방
	PC_GARENA_GCA_PLUS,
	PC_MAX_COUNT
};

////////////////////////////////////////////////////////////////////////////////////////////
// 옵션 설정값
#define ENV_SETTING_DEFAULT			1				// 기본 옵션입니다.
#define ENV_SETTING_CHANGE			0				// 옵션이 변경 되었습니다.

// Max String Count
#define		MAX_STRING_COUNT					256
#define		MAX_CHATTING_COUNT					256
#define		MAX_REPORT_FTP_ADDRESS				255
#define		MAX_BILLING_URL						255
#define		MAX_KNOCK_SERVER_ADDRESS			255

#define		STAGE_BIT_ACTIVE_MAX				8
#define		STAGE_TOTAL_MAX						256

//변경하지 마세요.. 로그에서 사용합니다. 
enum CHATTING_TYPE
{
	CHATTING_TYPE_RADIO = 0,	//InGame에서만 사용
	CHATTING_TYPE_ALL,					//Room에서만 사용
	CHATTING_TYPE_WHISPER,
	CHATTING_TYPE_REPLY,
	CHATTING_TYPE_TEAM,					//Room에서만 사용
	CHATTING_TYPE_CLAN,
	CHATTING_TYPE_LOBBY,
	CHATTING_TYPE_MATCH,
	CHATTING_TYPE_CLAN_MEMBER_PAGE,
	CHATTING_TYPE_MEGAPHONE,
	CHATTING_TYPE_COMMENT = 100,		// Client에서 Comment를 쓰는경우 사용
};

//////////////////////////////////////////////////////////////////////////
#ifndef GAMEPORTAL_BUILD	// ZLog,SIA를 빌드 중이라면 하단의 내용은 전부 무시		2011.10.06
//////////////////////////////////////////////////////////////////////////
// Battle
//////////////////////////////////////////////////////////////////////////
enum TEAM_TYPE
{
	TEAM_NONE = -1,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_COUNT,
	TEAM_BOTH = TEAM_COUNT,
};

enum TEAM_RESULT_TYPE
{
	TEAM_RED_WIN = 0,
	TEAM_BLUE_WIN,
	TEAM_DRAW
};

// UlSanGuard MCF ( 모듈 파일 체크 기능 )
// 클라이언트 해킹 타입
enum USG_MCF_RESULT
{
	MCF_NON = 255,	//처리 되지않거나 예외 발생
	MCF_SUCCESS = 0,//MCF 처리 성공
	MCF_F_VERSION,	//MCF 파일과 빌드 버전이 다름
	MCF_F_PID,		//자기 프로세스 찾기실패 
	MCF_F_ACCESS,	//프로세스 엑세스 실패
	MCF_F_FAKE,	//변조 MODULE 발견
	MCF_F_HIDE,	//숨긴 MODULE 발견
	MCF_F_BLOCK,	//차단 MODULE 발견 확장 기능이며 의미가 없을거같아 우선구현안함..
};
// UlSanGuard AHC ( API 후킹 체크 기능 )
enum USG_AHC_RESULT
{
	AHC_NON = 255,	//처리 되지않거나 예외 발생
	AHC_OK = 0,		//이상 없음
	AHC_F_VERSION,  //AHC 파일과 빌드 버전이 다름
	AHC_F_DX_HOOK,	//DIRECT X 후킹 감지
};

inline TEAM_RESULT_TYPE SwapTeamResultType(const TEAM_RESULT_TYPE type)
{
	if (type == TEAM_RED_WIN) return TEAM_BLUE_WIN;
	else if (type == TEAM_BLUE_WIN) return TEAM_RED_WIN;

	return TEAM_DRAW;
}

inline TEAM_TYPE SwapTeamType(const TEAM_TYPE teamType)
{
	if (teamType == TEAM_NONE)
		return teamType;

	return teamType == TEAM_RED ? TEAM_BLUE : TEAM_RED;
}

TEAM_TYPE	SlotIndexToTeamType(INT32 Index);

// -1 : teamType이나 useridx 파라미터값이상
INT32 TeamTypeUserIdxToSlotIndex(INT32 teamType, INT32 userIdx);

// slot 0 -> slot 1,  slot 1-> slot 0, 반대편팀 슬롯인덱스를 넘겨줌
inline INT32 ConvertSlotIndexToOppositeTeam(INT32 slotIndex)
{
	if (slotIndex < 0)
	{
		I3ASSERT(slotIndex >= 0);
		return slotIndex;
	}

	if (slotIndex % 2)
		slotIndex--;
	else
		slotIndex++;

	return slotIndex;
}


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
#define KILLING_MESSAGE_ASSIST				0x20000


// 초대하기 목록
#define MAX_INVITE_COUNT			8			// 5명

//공지 
#define NOTICE_TYPE_NORMAL			1
#define NOTICE_TYPE_EMERGENCY		2
#define NOTICE_TYPE_SLIDING			3
#define NOTICE_TYPE_CHAT			4
#define NOTICE_STRING_MAX_COUNT		1024
#define NOTICE_STRING_COUNT			512

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
	BATTLE_SERVER_TYPE_NONE = 0,
	BATTLE_SERVER_TYPE_P2P = 1,
	BATTLE_SERVER_TYPE_RELAY = 2,
	BATTLE_SERVER_TYPE_DEDICATION = 3,

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
struct COUPON_INFO
{
	INT32 _count = 0;
	INT32 _point = 0;
};

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

	GAME_KEY_MAP_CHANGEWEAPON_7,						// 투척무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_8,						// 특수무기로 교체
	GAME_KEY_MAP_CHANGEWEAPON_9,						// 폭발물로 교체

	GAME_KEY_MAP_SPECIAL_MISSION,

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
	SLOT_STATE_GACHA,				// Gacha Shop
	SLOT_STATE_GIFTSHOP,			// GiftShop

	SLOT_STATE_NORMAL,				// 레디룸에서 상태
	SLOT_STATE_READY,				// 레디룸에서 상태
	SLOT_STATE_LOAD,				// (게임중) 로딩중입니다.			Time Out ( 2분)     처음시작 방장시작후 (10초)
	SLOT_STATE_RENDEZVOUS,			// (게임중) 랑데뷰서버 홀펀칭		Time Out ( )
	SLOT_STATE_PRESTART,			// (게임중) 홀펀칭					Time Out ( )
	SLOT_STATE_BATTLE_LOADOK,		// 게임중인 유저의 정보를 동기화한 유저
	SLOT_STATE_BATTLE_READY,		// (게임중) 홀펀칭 완료후 게임 시작을 기다림 
	SLOT_STATE_BATTLE,				// (게임중) 전투중입니다.

};

//순서 중요
enum ROOM_STATE
{
	ROOM_STATE_READY = 0,			//ReadyRoom 상태 
	ROOM_STATE_COUNTDOWN_R,			//ReadyRoom에서 START 버튼 클릭시 카운트 다운.
	ROOM_STATE_LOADING,				//로딩 상태
	ROOM_STATE_RENDEZVOUS,			//랑데뷰 서버에 접속준비 중인 상태
	ROOM_STATE_PRE_BATTLE,			//로딩 이 끝나고, 홀펀칭 상태 
	ROOM_STATE_COUNTDOWN_B,			//배틀시작 직전 카운트 다운. 
	ROOM_STATE_BATTLE,				//홀펀칭이 끝나고 게임을 시작하는 상태 
	ROOM_STATE_BATTLE_RESULT,		//배틀 종료, 결과 창 출력 
	ROOM_STATE_BATTLE_END,			//배틀  종료  바로 Ready로 변경됨 
	ROOM_STATE_EMPTY,				//깨진방: Lobby의 Room 목록에서 AutoRefresh 시, 이 빠진 방 구분 용. Lobby에서만 사용.	
};

enum MISSION_STATE
{
	MISSION_STATE_UNKNOWN = 0,		//
	MISSION_STATE_PRE_BATTLE,		//보스전으로 들어갔습니다. 모든 미션전에서 사용합니다. 
	MISSION_STATE_BATTLE,			//
	MISSION_STATE_FREE,				//
	MISSION_STATE_RESULT,			//
};

///////////////////////////////////////////////////////////////////////////////////

enum MODE_RECORD
{
	MODE_DEATHMATCH = 0,
	MODE_ANNIHILATION,
	MODE_BOMB,
	MODE_ETC,
	MODE_RECORD_MAX
};

enum SALE_ITEM_TYPE
{
	SALE_ITEM_DISGUISE_RANK = 0x01,
	SALE_ITEM_DISGUISE_NAME = 0x02,
	SALE_ITEM_COLOR_CROSSHAIR = 0x04,
	SALE_ITEM_COLOR_CHATTING = 0x08,
	SALE_ITEM_COLOR_NICK = 0x10,

	SALE_ITEM_MAX_COUNT = 5,
};

// 진급 Point
extern const INT32		g_InitialRankUpPoint;


// Game Key Mode
enum GAMEKEY_MODE_STATE
{
	GAMEKEY_MODE_MENU = 0,		// 메뉴에 사용되는 게임키 모드
	GAMEKEY_MODE_GAME,			// 게임에 사용되는 게임키 모드

	GAMEKEY_MODE_COUNT,
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
	ITEM_CHANGE_TYPE_REPAIR,
	ITEM_CHANGE_TYPE_BUY,
	ITEM_CHANGE_TYPE_GIFT_AUTH,
};

//	무기 분류 방식
//
// 아이템 아이디 세번째에 들어갑니다. ( 3 )
//
// 아이템 아이디에서 저장되는 자릿수 : 1000000-1 ~ 1000
// 유효 값 범위 : 967



#define WEAPON_CLASS_NPC_ARROW		100


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

enum TYPE_BATTLE_RESULT_EVENT
{
	TYPE_BATTLE_RESULT_EVENT_PCCAFE = 0,
	TYPE_BATTLE_RESULT_EVENT_ITEM,
	TYPE_BATTLE_RESULT_EVENT_EVENT,

	TYPE_BATTLE_RESULT_EVENT_COUNT,
};

// 행동 ID . v1.0전용. v1.5에선 사용 안함.
enum ACTION_ID
{
	ACTION_ID_NA = 0,			// 정의되지 않음.
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

const char* getActionName(ACTION_ID id);

// 캐릭터 팀 분류 방식
typedef UINT32 CHARACTER_TEAM_TYPE;
#define CHARACTER_TEAM_NONE	0x00
#define	CHARACTER_TEAM_RED	0x01
#define CHARACTER_TEAM_BLUE	0x02
#define NPC_TEAM_RED	0x04
#define NPC_TEAM_BLUE	0x08
#define CHARACTER_TEAM_RB		(CHARACTER_TEAM_RED | CHARACTER_TEAM_BLUE)

inline CHARACTER_TEAM_TYPE GetSwappedCharacterTeamValue(CHARACTER_TEAM_TYPE teamType)
{
	if (teamType == CHARACTER_TEAM_RED) return CHARACTER_TEAM_BLUE;
	else if (teamType == CHARACTER_TEAM_BLUE) return CHARACTER_TEAM_RED;

	return teamType;
}

// 캐릭터 리소스 ID
//
// 아이템 아이디 두번째에 들어갑니다. ( 2 )
//
enum CHARA_RES_ID
{
	CHARA_RES_ID_UNKNOWN = 0,

	// 인간
	CHARA_RES_ID_HUMAN_S = CHARA_RES_ID_UNKNOWN,

	// Normal
	CHARA_RES_ID_RED_BULLS = 1,	// Rebel Male
	CHARA_RES_ID_ACID_POLE = 2,	// SWAT Male
	CHARA_RES_ID_TARANTULA = 3,	// Rebel Female
	CHARA_RES_ID_SWAT_FEMALE = 4,	// Keen Eyes
	CHARA_RES_ID_VIPERRED = 5,
	CHARA_RES_ID_SKINNIE = 6,
	CHARA_RES_ID_D_FOX = 7,
	CHARA_RES_ID_LEOPARD = 8,
	CHARA_RES_ID_HIDE = 9,
	CHARA_RES_ID_RICA = 10,
	CHARA_RES_ID_CHOU = 11,

	// Exp 20% Up
	CHARA_RES_ID_EXP20_D_FOX = 12,
	CHARA_RES_ID_EXP20_LEOPARD = 13,

	// Point 30 % Up
	CHARA_RES_ID_POINT30_VIPERRED = 14,
	CHARA_RES_ID_POINT30_HIDE = 15,

	// ReinForced
	CHARA_RES_ID_RF_RED_BULLS = 16,
	CHARA_RES_ID_RF_ACID_POLE = 17,
	CHARA_RES_ID_RF_TARANTULA = 18,
	CHARA_RES_ID_RF_SWAT_FEMALE = 19,
	CHARA_RES_ID_RF_D_FOX = 20,
	CHARA_RES_ID_RF_LEOPARD = 21,
	CHARA_RES_ID_RF_VIPERRED = 22,
	CHARA_RES_ID_RF_HIDE = 23,
	CHARA_RES_ID_RF_BELLA = 24,
	CHARA_RES_ID_RF_CHOU = 25,

	// ReinForced Combo
	CHARA_RES_ID_RFC_D_FOX = 26,
	CHARA_RES_ID_RFC_LEOPARD = 27,
	CHARA_RES_ID_RFC_VIPERRED = 28,
	CHARA_RES_ID_RFC_HIDE = 29,

	// GRS 
	CHARA_RES_ID_GRS_RED_BULLS = 30,
	CHARA_RES_ID_GRS_ACID_POLE = 31,
	CHARA_RES_ID_GRS_TARANTULA = 32,
	CHARA_RES_ID_GRS_SWAT_FEMALE = 33,

	CHARA_RES_ID_KOPASSUS_HIDE = 34,
	CHARA_RES_ID_BOPE_REOPARD = 35,

	CHARA_RES_ID_HIDE_SOCCER = 36,
	CHARA_RES_ID_TARANTULA_SOCCER = 37,

	CHARA_RES_ID_HUMAN_E,

	CHARA_RES_ID_ACID_POLE_MADNESS = 58,
	CHARA_RES_ID_SWAT_FEMALE_MADNESS = 59,
	CHARA_RES_ID_HIDE_MADNESS = 60,
	CHARA_RES_ID_LEOPARD_MADNESS = 61,

	CHARA_RES_ID_ACID_POLE_MADNESS_SALES = 62,
	CHARA_RES_ID_SWAT_FEMALE_MADNESS_SALES = 63,
	CHARA_RES_ID_HIDE_MADNESS_SALES = 64,
	CHARA_RES_ID_LEOPARD_MADNESS_SALES = 65,

	// 공룡
	CHARA_RES_ID_DINO_S = 500,

	// Escape Dino
	CHARA_RES_ID_DINO_ACID,
	CHARA_RES_ID_DINO_ELITE,
	CHARA_RES_ID_DINO_RAPTOR,
	CHARA_RES_ID_DINO_STING,
	CHARA_RES_ID_DINO_TREX,
	CHARA_RES_ID_DINO_TANK,

	// Domi Dino
	CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE,
	CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN,
	CHARA_RES_ID_DINO_DOMI_MUTANTREX,
	CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB,

	// CC Dino
	CHARA_RES_ID_DINO_CC_RAPTOR,
	CHARA_RES_ID_DINO_CC_STING,
	CHARA_RES_ID_DINO_CC_ACID,
	CHARA_RES_ID_DINO_RAPTOR_MERCURY,
	CHARA_RES_ID_DINO_STING_MARS,
	CHARA_RES_ID_DINO_ACID_VULCAN,

	// Domi Dino
	CHARA_RES_ID_DINO_DOMI_RAPTOR,
	CHARA_RES_ID_DINO_DOMI_STING,
	CHARA_RES_ID_DINO_DOMI_ACID,
	CHARA_RES_ID_DINO_DOMI_ACID_ITEM,
	CHARA_RES_ID_DINO_DOMI_TANK,

	// Domi Reinforce
	CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY,
	CHARA_RES_ID_DINO_DOMI_STING_MARS,
	CHARA_RES_ID_DINO_DOMI_ACID_VULCAN,

	CHARA_RES_ID_DINO_E,
};

enum HEADGEAR_SUBTYPE
{
	HEADGEAR_SUBTYPE_NONE = 0,	// 일반 헤드 기어
	HEADGEAR_SUBTYPE_BERET = 1,	// 베레모
	HEADGEAR_SUBTYPE_MASK = 2,	// 마스크
	HEADGEAR_SUBTYPE_EVENT = 3,	// 이벤트 모자

	MAX_HEADGEAR_SUBTYPE
};

enum BERET_RES_ID
{
	BERET_RES_ID_BLACK = 1,
	BERET_RES_ID_BRAZIL = 2,
	BERET_RES_ID_BLACKPB = 3,
	BERET_RES_ID_BOPE = 4,
	BERET_RES_ID_CROSS_SWORD = 5,
	BERET_RES_ID_KOPASSUE = 6,
	BERET_RES_ID_RED_STAR = 7,
	BERET_RES_ID_SPECIAL_GENERAL_BERET = 8,
	BERET_RES_ID_TURKEY = 10,
	BERET_RES_ID_YELLOW_STAR = 11,
	BERET_RES_ID_PBNC5 = 12,
	BERET_RES_ID_PBTN_CHAMPION = 13,
	BERET_RES_ID_ASSAULT = 14, // 광폭한 맹수 베레모 (호칭 마스터)
	BERET_RES_ID_HANDGUN = 15, // 예리한 명사수 베레모 (호칭 마스터)
	BERET_RES_ID_SNIPER = 16, // 은밀한 암살자 베레모 (호칭 마스터)
	BERET_RES_ID_SUBMACHINE = 17, // 교활한 침입자 베레모 (호칭 마스터)
	BERET_RES_ID_SHOTGUN = 18, // 난폭한 파괴자 베레모 (호칭 마스터)
};

enum HEADGEAR_RES_ID
{
	HEADGEAR_RES_ID_UNKNOWN = 0,

	// 헤드기어 (HEADGEAR_SUBTYPE_NONE)
	HEADGEAR_RES_ID_DEFAULT = 101,	//	기본
	HEADGEAR_RES_ID_NORMAL = 102,	//	일반
	HEADGEAR_RES_ID_REINFORCE = 103,	//	강화
	HEADGEAR_RES_ID_TARGET = 104,
	HEADGEAR_RES_ID_STRENGTHENPLUS = 105,
	HEADGEAR_RES_ID_SUPER = 106,
	HEADGEAR_RES_ID_ANGELRING = 107,
	HEADGEAR_RES_ID_GRS = 108,

	// 헤드기어 계열 마스크 (HEADGEAR_SUBTYPE_MASK)
	HEADGEAR_RES_ID_MASK_SKULL = 122,
	HEADGEAR_RES_ID_MASK_ALIEN_BLUE_EYES = 123,
	HEADGEAR_RES_ID_MASK_ALIEN_RED_EYES = 124,
	HEADGEAR_RES_ID_MASK_PANDA = 125,
	HEADGEAR_RES_ID_MASK_EGG = 126,
	HEADGEAR_RES_ID_MASK_TIGER = 127,
	HEADGEAR_RES_ID_MASK_RED_DEVIL = 128,	// 레드 데빌
	HEADGEAR_RES_ID_MASK_DINO_SKULL = 129,	// 공룡 해골 마스크
	HEADGEAR_RES_ID_MASK_DINO_TREX = 130,	// 인도네시아 공룡 마스크
	HEADGEAR_RES_ID_MASK_GATOTKACA = 131,
	HEADGEAR_RES_ID_MASK_GATOTKACA_GOLD = 132,	// gatotkaca gold 
	HEADGEAR_RES_ID_MASK_EGG_TARANTULA = 133,
	HEADGEAR_RES_ID_MASK_EGG_REDBULLS = 134,
	HEADGEAR_RES_ID_MASK_SHEEP = 135,
	HEADGEAR_RES_ID_MASK_ZOMBIE = 136,

	// 이벤트 모자 (HEADGEAR_SUBTYPE_EVENT)
	HEADGEAR_RES_ID_EVENT_SANTA_HAT = 137,
	HEADGEAR_RES_ID_EVENT_CHINESE_HAT = 138,
	HEADGEAR_RES_ID_EVENT_HAIRBAND = 139,
	HEADGEAR_RES_ID_EVENT_COWBOY_HAT = 140,
	HEADGEAR_RES_ID_EVENT_HAIRBAND_GW = 141,
	HEADGEAR_RES_ID_EVENT_FES_HAT = 142,
	HEADGEAR_RES_ID_EVENT_BOONIE_HAT = 143,

	// 추가 아이템..
	HEADGEAR_RES_ID_BERET_PBNC5 = 146,

	HEADGEAR_RES_ID_BERET_SPECIAL_GENERAL = 159,

	HEADGEAR_RES_ID_COUNT
};

enum FACEGEAR_RES_ID
{
	FACEGEAR_RES_ID_UNKNOWN = 0,

	FACEGEAR_RES_ID_MASK_WHITE = 101,
	FACEGEAR_RES_ID_MASK_BLACK = 102,
	FACEGEAR_RES_ID_MASK_RUSSIAN_BLUE = 103,
	FACEGEAR_RES_ID_MASK_ROK_MARINE = 104,
	FACEGEAR_RES_ID_MASK_FRENCH_DESERT = 105,
	FACEGEAR_RES_ID_MASK_FLAME = 106,
	FACEGEAR_RES_ID_MASK_TWO_TONE_BR = 107,
	FACEGEAR_RES_ID_MASK_IRON = 108,
	FACEGEAR_RES_ID_MASK_TARGET_MARK = 109,

	FACEGEAR_RES_ID_MASK_PUNPKIN = 110,
	FACEGEAR_RES_ID_MASK_JEWEL_PINK = 111,
	FACEGEAR_RES_ID_MASK_SMILE_GOLD = 112,
	FACEGEAR_RES_ID_MASK_PIERROT = 113,
	FACEGEAR_RES_ID_MASK_CROSS_RED = 114,
	FACEGEAR_RES_ID_MASK_JASON = 115,
	FACEGEAR_RES_ID_MASK_DEATH = 116,
	FACEGEAR_RES_ID_MASK_WORLDCUP_ARGENTINA = 117,
	FACEGEAR_RES_ID_MASK_WORLDCUP_BRAZIL = 118,
	FACEGEAR_RES_ID_MASK_WORLDCUP_ENGLAND = 119,

	FACEGEAR_RES_ID_MASK_WORLDCUP_FRANCE = 120,
	FACEGEAR_RES_ID_MASK_WORLDCUP_GERMANY = 121,
	FACEGEAR_RES_ID_MASK_WORLDCUP_ITALIA = 122,
	FACEGEAR_RES_ID_MASK_WORLDCUP_JAPAN = 123,
	FACEGEAR_RES_ID_MASK_WORLDCUP_S_AFRICA = 124,
	FACEGEAR_RES_ID_MASK_WORLDCUP_S_KOREA = 125,
	FACEGEAR_RES_ID_MASK_WORLDCUP_SPAIN = 126,
	FACEGEAR_RES_ID_MASK_JESTER_BW = 127,
	FACEGEAR_RES_ID_MASK_ROESCHACH_WRESTLING = 128,
	FACEGEAR_RES_ID_MASK_RUSSIA = 129,

	FACEGEAR_RES_ID_MASK_UKRAINE = 130,
	FACEGEAR_RES_ID_MASK_BELORUS = 131,
	FACEGEAR_RES_ID_MASK_KAZAHSTAN = 132,
	FACEGEAR_RES_ID_MASK_TURKEY_FC_BESIKTAS = 133,	// 터키 축구 클럽 베식타스
	FACEGEAR_RES_ID_MASK_TURKEY_FC_BURSAPOR = 134,	// 터키 축구 클럽 부르사스포르
	FACEGEAR_RES_ID_MASK_TURKEY_FC_FENERBAHCE = 135,	// 터키 축구 클럽 페네르바체
	FACEGEAR_RES_ID_MASK_TURKEY_FC_GALATASARAY = 136,	// 터키 축구 클럽 갈라타사라이
	FACEGEAR_RES_ID_MASK_TURKEY_FC_TRABZONSPOR = 137,	// 터키 축구 클럽 트라브존스포르
	FACEGEAR_RES_ID_MASK_FRAIL_SKULL = 138,
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_BAHAMAS = 139,	// 중남미 마스크 (바하마)

	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_BOLIVIA = 140,	// 중남미 마스크 (볼리비아)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_CAYMAN = 141,	// 중남미 마스크 (케이먼 제도)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_CHILE = 142,	// 중남미 마스크 (칠레)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_COLOMBIA = 143,	// 중남미 마스크 (콜롬비아)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_COSTARICA = 144,	// 중남미 마스크 (코스타리카)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_HONDURAS = 145,	// 중남미 마스크 (온두라스)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_JAMAICA = 146,	// 중남미 마스크 (자메이카)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_MEXICO = 147,	// 중남미 마스크 (멕시코)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_NICARAGUA = 148,	// 중남미 마스크 (니카라과)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_PANAMA = 149,	// 중남미 마스크 (파나마)

	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_PARAGUAY = 150,	// 중남미 마스크 (파라과이)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_DOMINICAN = 151,	// 중남미 마스크 (도미니카 공화국)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_ECUADOR = 152,	// 중남미 마스크 (에콰도르)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_FRENCH_GUYANA = 153,	// 중남미 마스크 (가이아나 프랑스령)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_GUATEMALA = 154,	// 중남미 마스크 (과테말라)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_GUYANA = 155,	// 중남미 마스크 (가이아나)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_HAITI = 156,	// 중남미 마스크 (아이티)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_PERU = 157,	// 중남미 마스크 (페루)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_PUERTORICO = 158,	// 중남미 마스크 (푸에르토리코)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_SURINAME = 159,	// 중남미 마스크 (수리남)

	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_TRINIDAD = 160,	// 중남미 마스크 (트리니다드토바고)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_URUGUAY = 161,	// 중남미 마스크 (우루과이)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_VENEZUELA = 162,	// 중남미 마스크 (베네수엘라)
	FACEGEAR_RES_ID_MASK_LATIN_AMERICA_ARGENTINA = 163,	// 중남미 마스크 (아르헨티나)
	FACEGEAR_RES_ID_MASK_NA_RAPTR = 164,	// 북미 Raptr 마스크
	FACEGEAR_RES_ID_MASK_CANADA = 165,	// 카나다 국기 마스크
	FACEGEAR_RES_ID_MASK_UK = 166,	// 영국(United Kingdom) 국기 마스크
	FACEGEAR_RES_ID_MASK_USA = 167,	// 미국(United States America) 국기 마스크
	FACEGEAR_RES_ID_MASK_INDONESIA = 168,	// 인도네시아(Indonesia) 국기 마스크
	FACEGEAR_RES_ID_MASK_THAI_PBTN = 169,	// 태국 대회용 마스크

	FACEGEAR_RES_ID_MASK_CHINA_RED = 170,	// 중국 변겸 마스크
	FACEGEAR_RES_ID_MASK_REINFORCE_BLACK = 171,	// 터키 강화 블랙 마스크
	FACEGEAR_RES_ID_MASK_ITALY_AZZURRO_E_BIANCO = 172,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_AZZURRO_E_ROSSA = 173,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_BIANCA_E_NERA = 174,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_BIANCO_E_AZZURRO = 175,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_BLU_E_NERA = 176,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_BLU_E_ROSSA = 177,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_CROCE_NERA = 178,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_GIALLO_E_AMARANTO = 179,	// 이태리 프로축구

	FACEGEAR_RES_ID_MASK_ITALY_NERA_BIANCA_E_ROSSA = 180,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_NERA_E_BIANCA = 181,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_NERA_E_BLU = 182,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_NERA_E_ROSA = 183,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_NERA_GIALLA_E_BIANCA = 184,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_ROSSO_E_GIALLA = 185,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_STELLA = 186,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_ITALY_VIOLA = 187,	// 이태리 프로축구
	FACEGEAR_RES_ID_MASK_BRAZIL_TROLL = 188,	// 브라질 troll
	FACEGEAR_RES_ID_MASK_BRAZIL_DETERMINADO = 189,	// 브라질 determanido

	FACEGEAR_RES_ID_MASK_BRAZIL_FUUU = 190,	// 브라질 fuuu
	FACEGEAR_RES_ID_MASK_TROJAN = 191,	// 북미 Trojan 마스크
	FACEGEAR_RES_ID_MASK_FRAIL_SKULL_GOLD = 192,	// 인도네시아 Fail Skull Gold 마스크
	FACEGEAR_RES_ID_MASK_TURKEY_FC_BESIKTAS_V2 = 193,	// 터키 강화 축구 클럽 베식타스
	FACEGEAR_RES_ID_MASK_TURKEY_FC_BURSAPOR_V2 = 194,	// 터키 강화 축구 클럽 부르사스포르
	FACEGEAR_RES_ID_MASK_TURKEY_FC_FENERBAHCE_V2 = 195,	// 터키 강화 축구 클럽 페네르바체
	FACEGEAR_RES_ID_MASK_TURKEY_FC_GALATASARAY_V2 = 196,	// 터키 강화 축구 클럽 갈라타사라이
	FACEGEAR_RES_ID_MASK_TURKEY_FC_TRABZONSPOR_V2 = 197,	// 터키 강화 축구 클럽 트라브존스포르
	FACEGEAR_RES_ID_MASK_PBIC12 = 198,	// PBIC'12 마스크
	FACEGEAR_RES_ID_MASK_RUSSIAN_BLUE_TIGER_EV = 199,	// 대만 이벤트용 마스크	

	FACEGEAR_RES_ID_MASK_KOREA_MARINE_EV = 200,	// 대만 이벤트용 마스크	
	FACEGEAR_RES_ID_MASK_FRANCE_DESERT_EV = 201,	// 대만 이벤트용 마스크	
	FACEGEAR_RES_ID_MASK_BLACK_EV = 202,	// 대만 이벤트용 마스크	
	FACEGEAR_RES_ID_MASK_WHITE_EV = 203,	// 대만 이벤트용 마스크	
	FACEGEAR_RES_ID_MASK_RUSSIAN_BLUE_100M = 204,	// 인도네시아 새로운 아이템 ID
	FACEGEAR_RES_ID_MASK_ROK_MARINE_100M = 205,	// 인도네시아 새로운 아이템 ID
	FACEGEAR_RES_ID_MASK_FRENCH_DESERT_100M = 206,	// 인도네시아 새로운 아이템 ID
	FACEGEAR_RES_ID_MASK_PBSC = 207,	// PBSC 마스크
	FACEGEAR_RES_ID_MASK_PBTN_2012 = 208,	// PBTN_2012 마스크
	FACEGEAR_RES_ID_MASK_DEATH_GW = 209,	// 인도네시아 PB방 GWarnet용 데스마스크

	FACEGEAR_RES_ID_MASK_GRS_GOLD = 210,	// 태국 가레나 이전 기념 마스크 3종
	FACEGEAR_RES_ID_MASK_GRS_RED = 211,	// 태국 가레나 이전 기념 마스크 3종
	FACEGEAR_RES_ID_MASK_GRS_WHITE = 212,	// 태국 가레나 이전 기념 마스크 3종
	FACEGEAR_RES_ID_MASK_BLACK_SNAKE = 213,
	FACEGEAR_RES_ID_MASK_GSL = 214,
	FACEGEAR_RES_ID_MASK_PBNC4 = 215,
	FACEGEAR_RES_ID_MASK_PBIC2013 = 216,
	FACEGEAR_RES_ID_MASK_PBSC2013 = 217,
	FACEGEAR_RES_ID_MASK_AZERBAIJAN = 218,
	FACEGEAR_RES_ID_MASK_LATIN3 = 219,

	FACEGEAR_RES_ID_COUNT
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
	QUESTCARD_USAGE_UNKNOWN = 0,		// 기본 
};

enum LOBBY_USER_LIST
{
	USER_LIST_REQ_START = 0, //처음 요청
	USER_LIST_REQ_UPDATE,     //업데이트 요청중
};


// 임무카드 아이템 아이디 3번째 값(카드종류)은 ENUM_QUEST_CARDSET_TYPE 의 enum 값을 동일하게 사용합니다.

// 임무카드 아이템 아이디 4번째 값
enum QUESTCARD_NUMBER
{
	QUESTCARD_NUMBER_UNKNOWN = 0,	// 기본 
};

#pragma pack( push, 1 )
struct GAME_KEY_STRUCT
{
	INT8	_nDiviceType = 0;
	UINT32	_nState = 0;
};
#pragma pack( pop )



//
// Title System
////////////////////////////////////////////////////////////////////////////////
// 호칭 설정
#define MAX_USER_TITLE					255
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

// static_table
struct USER_TITLE_CONTEXT
{
	INT32 _category;
	INT32 _weapon;
	INT32 _character;
};

// static table
struct USER_TITLE_RIBBON
{
	UINT8	_type;
	//UINT8	_id;
	UINT8	_count;
};

#define	USER_TITLE_REQUIREMENT_COUNT		3
#define USER_TITLE_PRIORITY_COUNT			2

// static table
struct USER_TITLE_REQUIREMENT
{
	USER_TITLE_RIBBON _ribbon[USER_TITLE_REQUIREMENT_COUNT];
	UINT8	_userTitleId[USER_TITLE_PRIORITY_COUNT];
	UINT8	_rank;
};

struct USER_TITLE_EFFECT
{
	UINT8	_Id = 0;
	char	_Text[128] = { 0 };
};

#define RAPTOR_TOTAL_CNT	9
#define DINO_TOTAL_CNT		(RAPTOR_TOTAL_CNT + 1)
#define UNION_TOTAL_CNT		(SLOT_MAX_COUNT - DINO_TOTAL_CNT)

struct DINO_ROOM_INFO
{
	UINT8	_uiTRexSlotIdx = 0;
	UINT8	_uiRaptorSlotIdx[RAPTOR_TOTAL_CNT] = { 0 };

	bool	IsDinoIdx(UINT8 idx)
	{
		if (_uiTRexSlotIdx == idx) {
			return true;
		}

		INT32 i = 0;
		for (i = 0; i < RAPTOR_TOTAL_CNT; i++) {
			if (_uiRaptorSlotIdx[i] == idx) {
				return true;
			}
		}

		return false;
	}

	void Reset()
	{
		_uiTRexSlotIdx = 0;
		ZeroMemory(_uiRaptorSlotIdx, sizeof(UINT8) * RAPTOR_TOTAL_CNT);
	}
};

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

//////////////////////////////////////////////////////////////////////////////////////////////
//										Structure											//
//////////////////////////////////////////////////////////////////////////////////////////////
#define RELAY_ENCRYPTKEY_DEFAULT		0xFFFFFFFF
#define DSERVER_ENCRYPTKEY_DEFAULT		0xFFFFFFFF // duplicated!

#pragma pack( push, 1 )

// 삭제요망
enum NAT_STATE
{
	NAT_STATE_WITHOUT = 0,
	NAT_STATE_FULL_CONE_NAT,
	NAT_STATE_RESTRICTED_CONE_NAT,
	NAT_STATE_PORT_RESTRICTED_CONE_NAT,
	NAT_STATE_SYMMETRIC_NAT,
};
// 삭제요망

enum SERVER_UDP_STATE
{
	SERVER_UDP_STATE_NONE = 0,
	SERVER_UDP_STATE_DSERVER, //SERVER_UDP_STATE_RENDEZVOUS,
	SERVER_UDP_STATE_CLIENT,
	SERVER_UDP_STATE_RELAY,
	SERVER_UDP_STATE_RELAYCLIENT,
};

struct NET_UDP_INFO
{
	UINT32	m_nRealIP = 0;			// 공인 IP
	UINT16	m_nRealPort = 0;
	UINT32	m_nLocalIP = 0;			// 사설 IP
	UINT16	m_nLocalPort = 0;
	UINT8	m_nNATState = 0;

	void Reset(void)
	{
		m_nRealIP = 0;
		m_nRealPort = 0;
		m_nLocalIP = 0;
		m_nLocalPort = 0;
		m_nNATState = 0;
	}
};


//////////////////////////////////////////////////////////////////
//						Main Server to client					//
//////////////////////////////////////////////////////////////////


#pragma pack( pop )


#define BOUNDCHK( a, b)				I3ASSERT( ((a) >= 0) && ((a) < (b)))

#define NT_GIFT_FLAG		0x10000000		//	note id 에 추가되며 선물 쪽지에 사용

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
	CHARA_LOWER_ROAR,			//  고함 상태
	CHARA_LOWER_WAIT,			//  뻘짓 상태
	CHARA_LOWER_STUN,			//  스턴 상태

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
	CHARA_UPPER_LOADBULLET,		//	노리쇠 당기기~
	CHARA_UPPER_DAMAGE,			//	데미지 상태
	CHARA_UPPER_SWAPWEAPON,		//	무기 교환 상태
	CHARA_UPPER_JUMP,			//	점프 상태
	CHARA_UPPER_EXTENSION_ACTIVATE,		//	해당 무기의 Extension 기능을 활성화
	CHARA_UPPER_EXTENSION_DEACTIVATE,	//	해당 무기의 Extension 기능을 비활성화

	CHARA_UPPER_REMOVESHELL,

	CHARA_UPPER_LOADMAG_READY,
	CHARA_UPPER_FIREBULLET,		// 무기 발사후 노리쇠 당기기 (일부무기중에 LoadBullet과 모션이 달라 FireBullet을 사용하는 무기가 있다. M70)

	// UI Animation (Lobby, ReadyRoom, Inventory, Shop)
	CHARA_UPPER_NONEBATTLE_IDLE,			// UI의 Idle
	CHARA_UPPER_NONEBATTLE_WEAPONSHOW,		// UI의 WeaponViewAction
	CHARA_UPPER_NONEBATTLE_IDLE_CHANGE,		// UI의 WeaponChange (UI_Idle_Change)
	CHARA_UPPER_NONEBATTLE_IDLE_STAND,		// UI의 Idle Stand	 (UI_Idle_Stand)

	CHARA_UPPER_MAXCOUNT,
};

enum CHARA_ACTION_COMMON
{
	CHARA_ACTION_COMMON_NONE = 0,
	CHARA_ACTION_COMMON_CEREMONY,

	CHARA_ACTION_COMMON_MAXCOUNT,
};

enum CHARA_ACTION_SPACE
{
	CHARA_SPACE_NONE,	//일반상태
	CHARA_SPACE_JUMP,	//점프상태
	CHARA_SPACE_FORCE,	//점프대 상태
	CHARA_SPACE_DOWN,  //낙하
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

	CHARA_HIT_HEAD,

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

	CHARA_HIT_HEAD_OLD,

	CHARA_HIT_MAXCOUNT,
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
void ShuffleHitPart(UINT8* pOut, UINT32 ui32Key);

// -------------------------------------------------------------------------------------------------
// CalcHeadShotDamage : 
//		Desc. calc head shot damage
//		pShuffledPartsIdx	: the array of shuffled hit parts indices
//		ui32SizeOfPartsIdx	: size of the array
//		ui32RespawnIdx		: chara respawn index in the respawn packet
// -------------------------------------------------------------------------------------------------
UINT32	CalcHeadShotDamage(PBYTE pShuffledPartsArray, UINT32 ui32SizeOfShuffledParts, UINT32 ui32MyRespawnCnt, UINT32 ui32SlotIdx);

// -------------------------------------------------------------------------------------------------
// CalcAnimObjPath : 
//		Desc. calc Animation Object Path
//		pShuffledPartsIdx	: the array of shuffled hit parts indices
//		ui32RoundNum		: Current RoundNumber
//		i32TeamIdx			: Chara Team (BLUE Or RED)
//		i32OldPath			: Old Path
// -------------------------------------------------------------------------------------------------
UINT32 CalcAnimObjPath(PBYTE pShuffledPartsArray, UINT32 ui32RoundNum, INT32 i32TeamIdx, INT32 i32OldPath);
#define _SHUFFLE_HIT_PART
//--------------------------------------
//	UI_CHARA_VIEW
//--------------------------------------
enum UI_CHARA_ROTATE
{
	CHARA_LEFT = 1,
	CHARA_RIGHT,
};

#define _SUPPLY_BOX_

// 보급상자
#pragma pack(push, 1)
struct SUPPLY_BOX_ANNOUNCE_SETTING
{
	UINT16	ui16NormalStock = 0;			//일반 일일 남은 수량
	UINT16	ui16SilverStock = 0;			//실버 일일 남은 수량
	UINT16	ui16GoldStock = 0;				//골드 일일 남은 수량
	UINT16	ui16ResetTime = 0;				//리셋 시간(HHMM)
	UINT16	ui16NormalStockMax = 0;			//일반 일일 지급 수량
	UINT16	ui16SilverStockMax = 0;			//일반 일일 지급 수량
	UINT16	ui16GoldStockMax = 0;			//일반 일일 지급 수량
};

struct SUPPLY_BOX_SETTING
{
	UINT16	ui16BasicTime = 0;				//기준시간
	UINT16	ui16NextTime = 0;				//상자 재지급 가능 시간
	UINT16	ui16BasicRate = 0;				//지급 확률(100% = 1000으로 표기, 계산후 /1000으로 적용)
	UINT16	ui16RankingBonusRate = 0;		//지급 확률(100% = 1000으로 표기, 계산후 /1000으로 적용)
	UINT16	ui16ResetTime = 0;				//리셋 시간(HHMM)
	UINT32	ui32StartDate = 0;				//시작 시간(YYMMDDHHMM)
	UINT32	ui32EndDate = 0;				//종료 시간(YYMMDDHHMM)
	UINT8	ui8MaxPresentaionCount = 0;		//중복 획득 개수
	UINT16	ui16AnnouncePeriod = 0;			//지급 수량 알림 반복 주기
};

struct SUPPLY_BOX_PACKAGE
{
	UINT16	ui16NormalStock = 0;			//일반 일일 지급 수량
	UINT16	ui16NormalPrizeStock = 0;		//일반 일일 지급 수량
	UINT8	ui8NormalPresentMessage = 0;	//상자 획득 알림 메시지 설정
	UINT16	ui16SilverStock = 0;			//실버 일일 지급 수량
	UINT16	ui16SilverPrizeStock = 0;		//실버 일일 지급 수량
	UINT8	ui8SilverPresentMessage = 0;	//상자 획득 알림 메시지 설정
	UINT16	ui16GoldStock = 0;				//골드 일일 지급 수량
	UINT16	ui16GoldPrizeStock = 0;			//골드 일일 지급 수량
	UINT8	ui8GoldPresentMessage = 0;		//상자 획득 알림 메시지 설정
};

struct SUPPLY_BOX_STATUS
{
	UINT16	ui16NormalPresentaion = 0;		//일반 지급량
	UINT16	ui16NormalPrizePresentaion = 0;	//일반 상금 지급량
	UINT16	ui16SilverPresentaion = 0;		//실버 지급량
	UINT16	ui16SilverPrizePresentaion = 0;	//실버 상금 지급량
	UINT16	ui16GoldPresentaion = 0;		//골드 지급량
	UINT16	ui16GoldPrizePresentaion = 0;	//골드 상금 지급량

	void reset(void)
	{
		ui16NormalPresentaion = 0;
		ui16NormalPrizePresentaion = 0;
		ui16SilverPresentaion = 0;
		ui16SilverPrizePresentaion = 0;
		ui16GoldPresentaion = 0;
		ui16GoldPrizePresentaion = 0;
	}
};
#pragma pack(pop)

struct SUPPLY_BOX_INFO
{
	SUPPLY_BOX_SETTING	stSupplyBoxSetting;
	SUPPLY_BOX_PACKAGE	stSupplyBoxPackage;
	SUPPLY_BOX_STATUS	stSupplyBoxStatus;
};

enum SUPPLY_BOX_TYPE
{
	SUPPLY_BOX_NONE = 0,
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
struct USER_RANK_INFO
{
	INT8	i8SlotIdx = 0;
	T_UID	i64UID = 0;
	INT8	i8Ranking = 0;
};

struct USER_PRESENTED_INFO
{
	INT8	i8SlotIdx = 0;
	INT8	i8Presented = 0;
	UINT32	ui32ItemID = 0;
};
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

// 중남미 정식 참가 국가 25개국
enum LATINAMERICA_LIST
{
	LATINAMERICA_DEFAULT,			// 정식 중남미 국가 아님
	LATINAMERICA_MEXICO,			// 멕시코
	LATINAMERICA_ARGENTINA,			// 아르헨티나
	LATINAMERICA_BOLIVIA,			// 볼리비아
	LATINAMERICA_CHILE,				// 칠레
	LATINAMERICA_COLOMBIA,			// 콜롬비아
	LATINAMERICA_ECUADOR,			// 에콰도르
	LATINAMERICA_PARAGUAY,			// 파라과이
	LATINAMERICA_PERU,				// 페루
	LATINAMERICA_URUGUAY,			// 우루과이
	LATINAMERICA_VENEZUELA,			// 베네수엘라
	LATINAMERICA_GUATEMALA,			// 과테말라
	LATINAMERICA_HONDURAS,			// 온두라스
	LATINAMERICA_NICARAGUA,			// 니카라과
	LATINAMERICA_COSTARICA,			// 코스타리카
	LATINAMERICA_CAYMANISLANDS,		// 케이맨 군도
	LATINAMERICA_TRINIDADTOBAGO,	// 트리니다드 토바고
	LATINAMERICA_PUERTORICO,		// 푸에르토리코
	LATINAMERICA_JAMAICA,			// 자메이카
	LATINAMERICA_BAHAMAS,			// 바하마
	LATINAMERICA_HAITI,				// 아이티
	LATINAMERICA_DOMINICANREPUBLIC,	// 도미니카공화국
	LATINAMERICA_GUYANA,			// 가이아나
	LATINAMERICA_FRENCHGUIANA,		// 프랑스 령 기아나
	LATINAMERICA_SURINAME,			// 수리남
	LATINAMERICA_PANAMA,			// 파나마

	LATINAMERICA_MAX_COUNT
};

// 신고사유 내용 길이
#define MAX_REPORTCAUSE_SIZE		600		/// 300자

// 튜토리얼 플레이타입
enum TUTORIAL_PLAY_TYPE
{
	TUTORIAL_TRAINING = 0,
	TUTORIAL_TIMEATTACK,
};

// 채팅 제재 단계
enum CHAT_LIMIT_STATE
{
	CHAT_LIMIT_STATE_NORMAL = 0,			// 정상
	CHAT_LIMIT_STATE_ING,					// 진행중
	CHAT_LIMIT_STATE_START,					// 시작
	CHAT_LIMIT_STATE_MAX
};

enum TOUCHDOWN_BLOCK_STATE
{
	TOUCHDOWN_BLOCK_STATE_INIT,
	TOUCHDOWN_BLOCK_STATE_FULL,
};

enum BOUNDRY_NAME
{
	X_START_BOUNDRY = 0,
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
	HACK_TYPE_GRAVITY = 1,			// position 
	HACK_TYPE_ESCAPE = 2,			// touchDown 
	HACK_TYPE_RANGE = 3,			// weapon range
	HACK_TYPE_SPEED = 4,			// speed hack
	HACK_TYPE_WALLSHOT = 5,			// wall shot
	HACK_TYPE_GRENADE = 6,			// max throwing weapon
	NOT_USE_HACK_TYPE_MAGAZINE = 7,			// infinite bullet (=no magazine change)
	HACK_TYPE_GHOST = 8,			// hide my position  
	HACK_TYPE_RECALL = 9,			// recalls other users (=magnet)
	HACK_TYPE_WEAPON = 10,			// weapon change
	HACK_TYPE_TRAINING = 11,			// 만들어야함. 
	HACK_TYPE_FIRE_SPEED = 12,			// shotgun/sniper delay hack
	HACK_TYPE_GRAVITY_MOVE = 13,			// position Check
	HACK_TYPE_RESPAWN_TIME = 14,			// ignore respawn delay
	HACK_TYPE_RESPAWN_POSITION = 15,			// respawn position hack
	HACK_TYPE_INSTALL_C4 = 16,			// install C4 by hack user 
	HACK_TYPE_CLIENT_TIME = 17,			// client time check
	HACK_TYPE_HEADSHOT = 18,			// head shot
	HACK_TYPE_ITEM = 19,			// item not found ( chara, mask, beret ) : room create
	HACK_TYPE_WEAPON_PARAM = 20,			// weapon parameter hacking (recoil...)
	HACK_TYPE_WEAPON_NOT_FOUND = 21,			// weapon not found
	HACK_TYPE_UDP_VERSION = 22,			//
	HACK_TYPE_PAUSE_MOVE = 23,			// GM Pause Move
	HACK_TYPE_PAUSE_SHOOT = 24,			// GM Pause Shoot
	HACK_TYPE_USURPATION = 25,			// USURPATION Mode Check
	HACK_TYPE_GRNEADE_POS = 26,			// GRNEADE_POS Check
	HACK_TYPE_RPG_CHEAT = 27,			// Invalid RPG Use
	HACK_TYPE_SPACE = 28,			// 저중력모드용 핵체크타입
	// ------------------------
	HACK_TYPE_MAX
};

enum GS_HACK_TYPE
{
	GS_HACK_TYPE_DLL_CHEAT,			// DX Dll 

	GS_HACK_TYPE_MAX,
};

// Hack Check ratio information 
struct GSHackCheckRatio
{
	UINT8	m_aui8HackCheckRatio[GS_HACK_TYPE_MAX] = { 0 };

	void SetDefaultValue()
	{
		m_aui8HackCheckRatio[GS_HACK_TYPE_DLL_CHEAT] = 0;
	}

	UINT8 CopyToPacket(UINT8* pOut, UINT32 ui32BufLen)
	{
		I3ASSERT(pOut != nullptr);
		I3ASSERT(ui32BufLen >= GS_HACK_TYPE_MAX);
		if (ui32BufLen < GS_HACK_TYPE_MAX) return 0;

		memcpy_s(pOut, ui32BufLen, m_aui8HackCheckRatio, sizeof(m_aui8HackCheckRatio));
		return GS_HACK_TYPE_MAX;
	}
};

// Hack Check ratio information 
struct ServerHackCheckRatio
{
	UINT8	m_aui8HackCheckRatio[HACK_TYPE_MAX] = { 0 };

	// Constructor
	ServerHackCheckRatio()
	{
		SetDefaultValue();
	}

	void SetDefaultValue()
	{
#ifdef _DEBUG
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_ESCAPE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RANGE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_SPEED] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WALLSHOT] = 0;
		m_aui8HackCheckRatio[HACK_TYPE_GRENADE] = 100;
		m_aui8HackCheckRatio[NOT_USE_HACK_TYPE_MAGAZINE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GHOST] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RECALL] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_TRAINING] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_FIRE_SPEED] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY_MOVE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_TIME] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_POSITION] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_INSTALL_C4] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_CLIENT_TIME] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_HEADSHOT] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_ITEM] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_PARAM] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_NOT_FOUND] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_UDP_VERSION] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_PAUSE_MOVE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_PAUSE_SHOOT] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_USURPATION] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GRNEADE_POS] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RPG_CHEAT] = 100;

#else
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_ESCAPE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RANGE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_SPEED] = 110;
		m_aui8HackCheckRatio[HACK_TYPE_WALLSHOT] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GRENADE] = 20;
		m_aui8HackCheckRatio[NOT_USE_HACK_TYPE_MAGAZINE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GHOST] = 0;
		m_aui8HackCheckRatio[HACK_TYPE_RECALL] = 0;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_TRAINING] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_FIRE_SPEED] = 110;
		m_aui8HackCheckRatio[HACK_TYPE_GRAVITY_MOVE] = 0;
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_TIME] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RESPAWN_POSITION] = 0;
		m_aui8HackCheckRatio[HACK_TYPE_INSTALL_C4] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_CLIENT_TIME] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_HEADSHOT] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_ITEM] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_PARAM] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_WEAPON_NOT_FOUND] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_UDP_VERSION] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_PAUSE_MOVE] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_PAUSE_SHOOT] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_USURPATION] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_GRNEADE_POS] = 100;
		m_aui8HackCheckRatio[HACK_TYPE_RPG_CHEAT] = 0;

#endif
	}

	UINT8 CopyToPacket(UINT8* pOut, UINT32 ui32BufLen)
	{
		I3ASSERT(pOut != nullptr);
		I3ASSERT(ui32BufLen >= HACK_TYPE_MAX);
		if (ui32BufLen < HACK_TYPE_MAX) return 0;

		memcpy_s(pOut, ui32BufLen, m_aui8HackCheckRatio, sizeof(m_aui8HackCheckRatio));
		return HACK_TYPE_MAX;
	}
};

////////////////////////////////////////////////////////////////
// 레벨별 아이템 지급 관련
#define MAX_RANK_COUNT						RANK_51  // 랭크업아이템을 지급할 레벨의 총 갯수

////////////////////////////////////////////////////////////////
// GM Pause
#define NEXT_GM_PAUSE_TIME						3		// 한번 GM Pause명령을 내린후 다시 명령을 내리기까지의 대기 시간(초). 이 시간보다 더 빠르게 Pause명령을 내릴수 없다.
#define GM_PAUSE_MOVE_DISTANCE					4.0f	// GM Pause 중에 움직일수 있는 거리(m) 이 이상 움직이면 해킹으로 감지. 데디서버에서 클라이언트까지 정지명령이 가는 딜레이때문에 유저가 조금 움직인다고 해도 무조건 해킹은 아니다. 네트워크가 느릴 수록 이 값을 크게 해야한다.
#define GM_PAUSE_SHOOT_COUNT					50		// GM Pause 중에 발사, 히트 패킷을 수신할수 있는 횟수. 이 이상 수신하면 해킹으로 감지.

//Hack Type Value
enum HACK_CHECK_TYPE
{
	HACK_CHECK_TYPE_NONE = 0,
	HACK_CHECK_TYPE_CLIENT = 1,
	HACK_CHECK_TYPE_SERVER_G = 2,
	HACK_CHECK_TYPE_SERVER_D = 3,
};

//--------------------------------------
//	CHARARES_TYPE
//--------------------------------------
enum CHARARES_TYPE
{
	CHARARES_UNKNOWN = 0,
	// 인간
	CHARARES_REBEL_MALE = 1,	// 1번 캐릭터는 기본적 캐릭터, RED
	CHARARES_SWAT_MALE = 2,	// 1번 캐릭터는 기본적 캐릭터, Blue
	CHARARES_REBEL_FEMALE = 3,
	CHARARES_SWAT_FEMALE = 4,
	CHARARES_D_FOX = 5,
	CHARARES_LEOPARD = 6,
	CHARARES_SWAT_HIDE = 7,

	// 공룡
	CHARARES_DINO_RAPTOR,
	CHARARES_DINO_TREX,
	// 인간
	CHARARES_REBEL_VIPERRED,
	// 공룡
	CHARARES_DINO_STING,
	CHARARES_DINO_TANK,
	CHARARES_DINO_ACID,
	CHARARES_DINO_ELITE,
	//인간
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

	CHARARES_REBEL_MALE_GRS,
	CHARARES_SWAT_MALE_GRS,
	CHARARES_REBEL_FEMALE_GRS,
	CHARARES_SWAT_FEMALE_GRS,

	CHARARES_SWAT_HIDE_KOPASSUS,
	CHARARES_LEOPARD_BOPE,

	CHARARES_SWAT_HIDE_SOCCER,
	CHARARES_REBEL_SOCCER,

	CHARARES_TYPE_COUNT // 캐릭터 추가시 아래 함수도 수정해야 합니다
};

//AutoAim Log
struct	USER_BATTLE_LOG
{
	UINT8							m_ui8Kill = 0;			// 킬 카운트
	INT32							m_i32FireCnt = 0;		// 총 파이어 카운트
	INT32							m_i32Detect = 0;		// 감지 횟수
	REAL32							m_rAccuracy = 0;		// 감지 정확도
	INT32							m_i32AimChk = 0;		// Aim 각도를 이용한 감지 카운트
	UINT8							m_ui8HitPosChk = 0;		// 히트 거리를 이용한 감지 카운트
};

//-------------------------------------------------------------------------------------------------
//제압 모드 Define 

#define INVALID_IDX							-1 

struct TARGET_INFO
{
	INT16	m_iTypeIdx = 0;
	INT16	m_iSlotIdx = 0;

	void copy(TARGET_INFO* pInfo)
	{
		m_iTypeIdx = pInfo->m_iTypeIdx;
		m_iSlotIdx = pInfo->m_iSlotIdx;
		return;
	}

	BOOL isEqual(TARGET_INFO* pInfo)
	{
		if (m_iTypeIdx != pInfo->m_iTypeIdx) return FALSE;
		if (m_iSlotIdx != pInfo->m_iSlotIdx) return FALSE;

		return TRUE;
	}
};

enum DOMI_TYPE
{
	DOMI_TYPE_NO = -1,
	DOMI_TYPE_USER = 0,
	DOMI_TYPE_DINO = 1,
	DOMI_TYPE_SENTRYGUN = 2,
	DOMI_TYPE_DUMMY = 3,
	DOMI_TYPE_HPSUPPLY = 4,
	DOMI_TYPE_AMMOSUPPLY = 5,
	DOMI_TYPE_SKILLSHOP = 6,
	DOMI_TYPE_MEATBOMB = 7,

	DOMI_TYPE_COUNT
};

enum DOMI_TYPE_DINO_TARGET
{
	DOMI_TYPE_DINO_TARGET_USER = 0,
	DOMI_TYPE_DINO_TARGET_SENTRYGUN = 1,
	DOMI_TYPE_DINO_TARGET_DUMMY = 2,

	DOMI_TYPE_DINO_TARGET_COUNT
};

#define DINO_THROWWEAPON_COUNT			MAX_COUNT_DOMI_DINO

#define MAX_COUNT_DOMI_USER				7		// 7을 선언하지만, 0,2,4,6만 사용하고 1,3,5는 사용하지 않는다.
#define MAX_COUNT_DOMI_USER_REAL		4
#define MAX_COUNT_DOMI_DINO				32
#define MAX_COUNT_DOMI_SENTRYGUN		10
#define MAX_COUNT_DOMI_DUMMY			10
#define MAX_COUNT_DOMI_HPSUPPLY			5
#define MAX_COUNT_DOMI_AMMOSUPPLY		5
#define MAX_COUNT_DOMI_SKILLSHOP		1

#define MAX_COUNT_DOMI_USER_SENTRYGUN_DUMMY	10

#define DOMI_USER_INCREASE				2

#define BEGIN_IDX_DOMI_SENTRYGUN		0
#define BEGIN_IDX_DOMI_DUMMY			(MAX_COUNT_DOMI_SENTRYGUN)
#define BEGIN_IDX_DOMI_HPSUPPLY			(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY)
#define BEGIN_IDX_DOMI_AMMOSUPPLY		(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY)
#define BEGIN_IDX_DOMI_SKILLSHOP		(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY + MAX_COUNT_DOMI_AMMOSUPPLY)

#define FLASHBANG_COUNT					32
#define SMOKE_COUNT						32

#define USER_OBJECT_DISTANCE			3.0f

enum EVENT_STATE
{
	EVENT_STATE_HIT,
	EVENT_STATE_DEAD,
	//EVENT_STATE_DINOCREATE,
	//EVENT_STATE_BLIND,
	//EVENT_STATE_USERRESPAWN,

	EVENT_STATE_MAX,
};

enum WEAPON_SPECIAL_EFFECT
{
	WEAPON_SPECIAL_EFFECT_STIFFEN,		// 경직
	WEAPON_SPECIAL_EFFECT_BLIND,		// 실명
	WEAPON_SPECIAL_EFFECT_SLOW,			// 둔화
	WEAPON_SPECIAL_EFFECT_KNOCKDOWN,	// 넉다운
	WEAPON_SPECIAL_EFFECT_FATALBLOW,	// 치명타

	WEAPON_SPECIAL_EFFECT_COUNT
};

// 공룡 상태 
// 순서 중요함. (클라이언트도 쓰기 때문에. 순서 변경시 클라이언트도 재빌드해야 정상플레이 가능)
enum DINO_STATE
{
	DINO_STATE_NONE = 0,
	DINO_STATE_DEAD,
	DINO_STATE_FREEZE,			// 부등호를 사용하고 있음. 움직이지 않는 상태를 체크 하기 위한 용도로 사용함. 
	DINO_STATE_STOP,			// 정지
	DINO_STATE_ATTACK,			// 공격
	DINO_STATE_MOVE,			// 이동
	DINO_STATE_PAIN,			// 고통
	DINO_STATE_S_STIFFEN,		// 결직 ( 무기특수효과 )
	DINO_STATE_S_BLIND,			// 블라인드 ( 무기특수효과 )
	DINO_STATE_NOTUSE,
	DINO_STATE_RAPTORLEAP,		// 랩터: 도약스킬
	DINO_STATE_S_SLOW,			// 둔화 ( 무기특수효과 )
	DINO_STATE_S_KNOCKDOWN,		// 넉다운 ( 무기특수효과 )
	DINO_STATE_TANKDASH,		// 탱크: 돌진스킬
	DINO_STATE_ROAR,			// 유저를 발견했을때 괴성 지르기
	DINO_STATE_SUCCESSKILL,		// 유저를 죽였을때 기뻐하기
	DINO_STATE_RAPTOR_FEED,		// 랩터: 시체먹기
	DINO_STATE_ACID_SEEK,		// 애시드: 탐색
	DINO_STATE_STING_SHAKE,		// 스팅: 흔들기
	DINO_STATE_TANK_HOWL,		// 탱크: 외치기
	DINO_STATE_TREX_HOWL,		// 뮤턴트티렉스: 하울링스킬
	DINO_STATE_TREX_READY,		// 뮤턴트티렉스: 대쉬스킬 준비
	DINO_STATE_TREX_DASH,		// 뮤턴트티렉스: 대쉬스킬 진행
	DINO_STATE_TREX_HOWL_FAIL,	// 뮤턴트티렉스: 하울링스킬 실패

	DINO_STATE_COUNT
};

enum DINO_TANKDASH_STATE
{
	DINO_TANKDASH_STATE_READY = 0,		// 돌진 준비
	DINO_TANKDASH_STATE_RUNNING,		// 돌진중
	DINO_TANKDASH_STATE_FINISH,			// 돌진 완료

	DINO_TANKDASH_STATE_COUNT
};

enum DINO_RAPTORLEAP_STATE
{
	DINO_RAPTORLEAP_STATE_READY = 0,	// 도약 준비
	DINO_RAPTORLEAP_STATE_LEAP_UP,		// 도약 상승
	DINO_RAPTORLEAP_STATE_LEAP_DOWN,	// 도약 하강
	DINO_RAPTORLEAP_STATE_FINISH,		// 도약 완료
	DINO_RAPTORLEAP_STATE_FAIL,			// 도약 공격 실패

	DINO_RAPTORLEAP_COUNT
};

enum DINO_DEAD_STATE
{
	DINO_DEAD_STATE_NORMAL = 0,
	DINO_DEAD_STATE_HEADSHOT,
	DINO_DEAD_STATE_GRENADE,

	DINO_DEAD_STATE_COUNT
};

enum DINO_MOVE_STATE
{
	DINO_MOVE_STATE_JUMP_START = 0,
	DINO_MOVE_STATE_JUMP_UP,
	DINO_MOVE_STATE_JUMP_DOWN,
	DINO_MOVE_STATE_JUMP_END,
	DINO_MOVE_STATE_RUN,
	DINO_MOVE_STATE_WALK,
	DINO_MOVE_STATE_SOONBOOM,

	DINO_MOVE_STATE_COUNT
};

enum DINO_ATTACK_STATE
{
	DINO_ATTACK_STATE_ING = 0,
	DINO_ATTACK_STATE_WAIT_COOLTIME,

	DINO_ATTACK_STATE_COUNT
};

enum DINO_ATTACK_WEAPON_RAPTOR
{
	DINO_ATTACK_WEAPON_RAPTOR_BITE,				// 랩터 물기
	DINO_ATTACK_WEAPON_RAPTOR_CLAW,				// 랩터 할퀴기

	DINO_ATTACK_WEAPON_RAPTOR_COUNT
};

enum DINO_ATTACK_WEAPON_STING
{
	DINO_ATTACK_WEAPON_STING_SPIT,				// 스팅 침뱉기
	DINO_ATTACK_WEAPON_STING_GASBAG,			// 스팅 가스주머니발사

	DINO_ATTACK_WEAPON_STING_COUNT
};

enum DINO_ATTACK_WEAPON_ACID
{
	DINO_ATTACK_WEAPON_ACID_BITE,				// 애시드 물기

	DINO_ATTACK_WEAPON_ACID_COUNT
};

enum DINO_ATTACK_WEAPON_MUTANTTREX
{
	DINO_ATTACK_WEAPON_MUTANTTREX_BITE_LOW,		// 뮤턴트티렉스 하단물기
	DINO_ATTACK_WEAPON_MUTANTTREX_BITE_MID,		// 뮤턴트티렉스 중단물기
	DINO_ATTACK_WEAPON_MUTANTTREX_CHOP,			// 뮤턴트티렉스 밟기

	DINO_ATTACK_WEAPON_MUTANTTREX_COUNT
};

enum DINO_ATTACK_WEAPON_TANK
{
	DINO_ATTACK_WEAPON_TANK_BITE,				// 탱크 물기

	DINO_ATTACK_WEAPON_TANK_COUNT
};

enum DINO_PAIN_STATE
{
	DINO_PAIN_STATE_LOW,
	DINO_PAIN_STATE_MIDDLE,
	DINO_PAIN_STATE_HIGH,

	DINO_PAIN_STATE_COUNT
};

// 1. 유저, 2. 센트리건, 3. 더미
enum OBJ_STATE
{
	OBJ_STATE_NONE = 0,
	OBJ_STATE_IDLE,
	OBJ_STATE_DEAD,
};

#define MOVE_NODE_STATE_NORMAL		0
#define MOVE_NODE_STATE_JUMP_DOWN	1
#define MOVE_NODE_STATE_JUMP_UP		2

enum DINO_KIND
{
	DINO_KIND_NONE = -1,
	DINO_KIND_RAPTOR_RED = 0,	// 붉은색 랩터
	DINO_KIND_RAPTOR_GREEN,				// 초록색 랩터
	DINO_KIND_RAPTOR_BLUE,				// 파란색 랩터
	DINO_KIND_STING,					// 스팅
	DINO_KIND_ACID,						// 애시드
	DINO_KIND_ACID_BOMB,				// 폭발 애시드
	DINO_KIND_MUTANTTREX,				// 뮤턴트티렉스
	DINO_KIND_TANK,						// 탱크
	DINO_KIND_ACID_ITEM,				// 아이템 떨구는 애시드
	DINO_KIND_RAPTOR_REINFORCE,			// 강화 랩터
	DINO_KIND_STING_REINFORCE,			// 강화 스팅
	DINO_KIND_ACID_REINFORCE,			// 강화 애시드

	DINO_KIND_COUNT
};

// 고통 애니메이션 재생 시간
#define DINO_TIME_RAPTOR_PAIN_LOW			0.5f
#define DINO_TIME_RAPTOR_PAIN_MIDDLE		1.0f
#define DINO_TIME_RAPTOR_PAIN_HIGH			1.1f

#define DINO_TIME_STING_PAIN_LOW			0.5f
#define DINO_TIME_STING_PAIN_MIDDLE			1.0f
#define DINO_TIME_STING_PAIN_HIGH			1.1f

#define DINO_TIME_ACID_PAIN_LOW				0.5f
#define DINO_TIME_ACID_PAIN_MIDDLE			0.5f
#define DINO_TIME_ACID_PAIN_HIGH			0.5f

#define DINO_TIME_MUTANTTREX_PAIN_LOW		1.0f	
#define DINO_TIME_MUTANTTREX_PAIN_MIDDLE	1.0f
#define DINO_TIME_MUTANTTREX_PAIN_HIGH		1.0f
//#define DINO_TIME_MUTANTTREX_PAIN_DEATHBLOW	5.3f
//#define DINO_TIME_MUTANTTREX_PAIN_HOWLING		2.0f

#define DINO_TIME_TANK_PAIN_LOW				0.5f
#define DINO_TIME_TANK_PAIN_MIDDLE			1.0f
#define DINO_TIME_TANK_PAIN_HIGH			1.2f

// 유저발견시 소리지름
#define DINO_TIME_RAPTOR_ROAR				1.5f
#define DINO_TIME_STING_ROAR				1.5f
#define DINO_TIME_ACID_ROAR					2.5f
#define DINO_TIME_MUTANTTREX_ROAR			2.6f
#define DINO_TIME_TANK_ROAR					3.3f

// 유저킬시 기뻐함
#define DINO_TIME_RAPTOR_SUCCESSKILL		3.0f
#define DINO_TIME_STING_SUCCESSKILL			4.2f
#define DINO_TIME_ACID_SUCCESSKILL			2.5f
#define DINO_TIME_MUTANTTREX_SUCCESSKILL	2.6f
#define DINO_TIME_TANK_SUCCESSKILL			3.3f

// 공룡 개개인 특수 행동
#define DINO_TIME_RAPTOR_FEED				6.6f
#define DINO_TIME_STING_SHAKE				7.6f
#define DINO_TIME_ACID_SEEK					8.6f
#define DINO_TIME_MUTANTTREX_HOWL			2.6f
#define DINO_TIME_TANK_HOWL					2.5f

// 공격 시간
#define DINO_TIME_RAPTOR_CLAW_HIT1			0.33f	// 할퀴기 1번째 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_CLAW_HIT2			0.6f	// 할퀴기 2번째 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_CLAW_END			1.1f	// 할퀴기 총 공격시간
#define DINO_TIME_RAPTOR_BITE_HIT			0.5f	// 물기 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_BITE_END			1.0f	// 물기 총 공격시간
#define DINO_TIME_RAPTOR_LEAP_READY			2.5f	// 도약 준비시간
#define DINO_TIME_RAPTOR_LEAP_FINISH		1.5f	// 도약 성공시간
#define DINO_TIME_RAPTOR_LEAP_FAIL			0.5f	// 도약 실패시간

#define DINO_TIME_STING_SPIT_HIT			0.3f	// 침뱉기 데미지 들어가는 시간
#define DINO_TIME_STING_SPIT_END			1.0f	// 침뱉기 총 공격시간
#define DINO_TIME_STING_GASBAG_THROW		2.1f	// 가스주머니발사 수류탄 던지는 시간
#define DINO_TIME_STING_GASBAG_END			3.0f	// 가스주머니발사 총 공격시간

#define DINO_TIME_ACID_BITE_HIT				0.2f	// 물기 데미지 들어가는 시간
#define DINO_TIME_ACID_BITE_END				0.5f	// 물기 총 공격시간
#define DINO_TIME_ACID_EXPLOSION_READY		2.0f	// 자폭 준비 시간 (= 자폭 데미지 들어가는 시간)

#define DINO_TIME_MUTANTTREX_BITE_HIT		0.7f	// 물기 데미지 들어가는 시간
#define DINO_TIME_MUTANTTREX_BITE_END		1.5f	// 물기 총 공격시간
#define DINO_TIME_MUTANTTREX_CHOP_HIT		0.73f	// 밟기 데미지 들어가는 시간
#define DINO_TIME_MUTANTTREX_CHOP_END		1.8f	// 밟기 총 공격시간
#define DINO_TIME_MUTANTTREX_DASH_READY		1.4f	// 돌진 대기시간
#define DINO_TIME_MUTATNTREX_HOWL_FAIL		5.3f	// 하울링 실패 모션시간

#define DINO_TIME_TANK_BITE_HIT				0.5f	// 물기 데미지 들어가는 시간
#define DINO_TIME_TANK_BITE_END				1.16f	// 물기 총 공격시간
#define DINO_TIME_TANK_DASH_READY			2.5f	// 돌진 준비시간
#define DINO_TIME_TANK_DASH_FINISH			1.0f	// 돌진 성공시간

#define DOMINATION_RANDOM_DIFFICULTY		78

#define MAX_TCP_EVENT_COUNT					128

#define INTERMISSION_WAIT_SECOND			25.0f	// 인터미션 대기시간

#define DINO_KNOCKDOWN_TIME					4.0f
#define DINO_DEAD_DELAY_TIME				4.0f
#define SMOKE_GAS_TIME						11.0f	// 스모크 연기로 인한 특수효과 적용시간

#define DOMI_RESULT_WINDOW_SECOND			5.0f	// 중간결과창 or 최종결과창이 떠있는 시간
#define DOMI_FINAL_WAVE_REWARD_TIME_SECOND	3.0f	// 라운드 보상 아이템 지급 알림창이 떠 있는 시간 ( = 최종결과창이 닫힌후 스테이지 점프하기까지의 사이시간 )

#pragma pack( push, 1)

struct	DOMINATION_HIGHSCORE
{
	INT32	m_i32DomiScore = 0;
	INT32	m_i32DomiScoreBest = 0;

	BOOL IsEmpty()
	{
		return (m_i32DomiScore == 0 && m_i32DomiScoreBest == 0);
	}
};

// 제압 인게임 보상. 
struct PACKET_DOMINATION_INSERT_ITEM_USERINVEN_ACK
{
	UINT8	m_ui8Condition = 0;	// 지급 조건. DOMI_REWARD_CONDITION	
};

struct PACKET_DOMINATION_USE_DOMIITEM_ACK
{
	INT8		_i8UserIdx = 0;		// 아이템 사용 요청한 유저의 슬롯번호
	T_ItemDBIdx	_TWareDBIdx = 0;	// 사용하려는 아이템 DB Ware IDX
};

struct PACKET_DOMINATION_USE_DOMIITEM_RESULT_REQ
{
	INT16	_i16CashGroupType = 0;	// enum CASHITEM_GROUP_TYPE
};

struct PACKET_DOMINATION_STAGE_JUMP_ACK
{
	UINT32	_ui32NewStageID = 0;	// 새롭게 점프할 스테이지ID
	UINT16	_ui16UserMask = 0;		// 점프하기 직전의 플레이 유저들 마스크
};

struct PACKET_DOMINATION_USE_ITEM_ACK
{
	INT8	_i8UserIdx = 0;			// 사용한 유저슬롯번호
	INT16	_i16CashGroupType = 0;	// enum CASHITEM_GROUP_TYPE
	UINT16	_ui16PlaySlot = 0;
};

struct PACKET_KICKUSER_ACK
{
	INT8	_i8UserIdx = 0;			// 짜를 유저 슬롯번호
	INT8	_i8Option = 0;			// 0-클라이언트 종료 1-로비로 보냄.
};

struct ST_POLYGON_TRANS_INFO
{
	INT16	_i16BeginIdx = 0;		// 담겨온 데이터의 시작인덱스번호
	INT16	_i16EndIdx = 0;			// 담겨온 데이터의 끝인덱스번호
};

// PROTOCOL_BATTLE_DOMINATION_NAVIMESH_ACK과 PROTOCOL_BATTLE_DOMINATION_AI_COLLISION_ACK에서 공동으로 사용
#define MAX_COUNT_POLYGON_SEND_ONETIME	100
struct PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK : public ST_POLYGON_TRANS_INFO
{
	INT16	_i16Count = 0;			// 총 갯수
	REAL32	_v3Polygon[MAX_COUNT_POLYGON_SEND_ONETIME][3][3] = { 0 };	// 폴리곤 정보
};

// 결과창 구조체
struct ST_RESULT_WINDOW_INFO
{
	INT16	_i16KillCount = 0;	// 죽인 공룡 마리수
	INT32	_i32Score = 0;		// 획득한 점수

	void Clear()
	{
		_i16KillCount = 0;
		_i32Score = 0;
	}
};

struct ST_RESULT_WINDOW_INFO_FINAL
{
	INT16	_i16Exp = 0;
	INT16	_i16Point = 0;
	UINT8	_ui8ResultIcon = 0;

	void Clear()
	{
		_i16Exp = 0;
		_i16Point = 0;
		_ui8ResultIcon = 0x00;
	}
};

struct PACKET_DOMINATION_WAVE_END_ACK
{
	bool					_bFinal = false;	// true:최종결과창 false:중간결과창
	ST_RESULT_WINDOW_INFO	_User[MAX_COUNT_DOMI_USER_REAL];	// 유저별 점수 구조체
};

struct PACKET_DOMINATION_WAVE_END_ADD_ACK
{
	ST_RESULT_WINDOW_INFO_FINAL	_UserAdd[MAX_COUNT_DOMI_USER_REAL];
};

struct PACKET_DOMINATION_WAVE_INFO_ACK
{
	INT16	_i16Round = 0;			// 라운드값 0부터 시작
	INT8	_i8Wave = 0;			// 웨이브값 0부터 시작
	INT8	_i8MaxWave = 0;			// 최대 웨이브 갯수
	INT16	_i16TotalDinoCount = 0;	// 총 등장할 공룡 마리수
	bool	_bInter = false;			// true:인터미션 false:웨이브배틀
	INT8	_i8RemainInterSec = 0;	// 인터미션일경우 잔여시간(초)
};

struct PACKET_DOMINATION_USE_ITEM_REQ
{
	T_ItemDBIdx	_TItemWareIdx = 0;	// 사용할 아이템
};

struct PACKET_DOMINATION_WAVE_PRE_START_ACK
{
	INT16	_i16Round = 0;			// 라운드값 0부터 시작
	INT8	_i8Wave = 0;			// 웨이브값 0부터 시작
	INT8	_i8MaxWave = 0;			// 최대 웨이브 갯수
	INT16	_i16TotalDinoCount = 0;	// 총 등장할 공룡 마리수
	UINT16	_ui16PlaySlot = 0;
};

struct PACKET_DOMINATION_MUTANTTREX_HOWLING_SLOW_ACK
{
	INT8	_i8SlowTime = 0;		// 느려지는 지속시간(초)
};

struct PACKET_DOMINATION_WAVE_READY_INFO_ACK
{
	bool	_bPress[MAX_COUNT_DOMI_USER_REAL] = { false };		// 인터미션 넘김 명령 상태
};

struct PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ
{
	INT8	_i8SkillIdx = 0;		// 어떤 스킬인지	USER_SKILL_XXXX
};

struct PACKET_DOMINATION_USER_SKILL_UPGRADE_ACK
{
	INT8	_i8UserIdx = 0;		// 유저슬롯번호
	INT8	_i8SkillIdx = 0;	// 어떤 스킬인지	USER_SKILL_XXXX
	INT8	_i8Level = 0;		// 레벨업해서 몇 레벨이 되었는지, 만약 레벨업이 실패했다면 사유(FAIL_REASON_USER_SKILL_UPGRADE_XXX)가 옵니다.
};

struct PACKET_DOMINATION_DIVULGE_DINO_APPEAR_ACK
{
	INT16	_i16RespawnIdx = 0;	// 리스폰오브젝트 인덱스
	INT16	_i16Second = 0;		// 등장시간(초)
};

struct PACKET_DOMINATION_PRINT_MESSAGE_ACK
{
	INT16	_i16Time = 0;						// 메세지 출력하고 있는 시간
	char	_szMessage[MAX_STRING_COUNT] = { 0 };	// 메세지 내용
};

struct PACKET_SERVER_USER_LEAVE_ACK
{
	INT32	_i32Score = 0;		// 점수
	INT8	_i8UserIdx = 0;		// 슬롯번호
};

struct PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK
{
	INT8	_i8UserSlot = 0;	// 쏜 유저 슬롯번호
	INT8	_i8DinoSlot = 0;	// 맞은 공룡 슬롯번호
	INT8	_i8EffectID = 0;	// 어떤 특수 효과인지 ( enum WEAPON_SPECIAL_EFFECT )
	INT16	_i16Score = 0;		// 특수효과로 인한 획득 점수
};

struct PACKET_DOMINATION_APPEAR_ACIDITEM_ACK
{
	INT8	_i8Count = 0;		// 애시드아이템 마리수
	INT16	_i16Time = 0;		// 애시드아이템 등장시간(초)
};

struct PACKET_DOMINATION_CHEAT_MESSAGE_REQ
{
	char	_szMessage[MAX_STRING_COUNT] = { 0 };	// 치트 메세지
};

struct DINO_INFOMATION
{
	UINT8	_ui8Num[DINO_KIND_COUNT] = { 0 };
};

// 배틀 결과창. 멀티킬 정보
#define		MAX_MULTI_KILL_COUNT	99	// 멀티킬 최대 표시횟수.

// 2016-11-02 User PC Specs 추가
struct USER_INFO_PCSPECS
{
	UINT32 _cpuId = 0;			// Cpu ID
	UINT8 _cpuCoreCnt = 0;		// CPU core count
	UINT16 _cpuHz = 0;			// CPU hz 
	UINT16 _ramSize = 0;		// RAM 크기
	UINT32 _gpuVendorId = 0;	// GPU 제조사 ID
	UINT32 _gpuDevId = 0;		// GPU Device ID
	UINT32 _gpuDriverId = 0;	// GPU Driver ID
	UINT16 _dvm = 0;			// Dedicated Video Memory Size
	UINT8 _osVer = 0;			// OS Version
	UINT8 _64bit = 0;			// 64bit 여부
	UINT8 _spVer = 0;			// Service Pack version
	UINT8 _screenCX = 0;		// Screen CX
	UINT8 _screenCY = 0;		// Screen CY
	UINT16 _screenW = 0;		// Screen Width
	UINT16 _screenH = 0;		// Screen Height

};


#pragma pack( pop )

class CBitValue32
{
public:
	void		ResetValue() { m_ui32Value = 0; }

	UINT32		GetValue() { return m_ui32Value; }

	bool		GetBit(INT32 i32Index) { return (m_ui32Value & (1 << i32Index)) != 0; }
	void		SetBit(INT32 i32Index) { m_ui32Value |= (1 << i32Index); }
	void		ResetBit(INT32 i32Index) { m_ui32Value &= (0 << i32Index); }

	bool		HasCheckedBit() { return m_ui32Value != 0; }

	bool IsCheckedAllBit(INT32 i32BitCount)
	{
		for (INT32 i = 0; i < i32BitCount; ++i)
		{
			if (!GetBit(i))
				return false;
		}

		return true;
	}

	INT32 GetCheckedCount(INT32 i32BitCount)
	{
		INT32 i32CheckedCnt = 0;
		for (INT32 i = 0; i < i32BitCount; ++i)
		{
			if (GetBit(i))
				i32CheckedCnt++;
		}

		return i32CheckedCnt;
	}

public:
	CBitValue32() {}

private:
	UINT32		m_ui32Value = 0;
};

// 공수교대 상태
#define	ATK_DEF_CHANGED		0x01		// 현재 라운드는 공수교대 되었을 경우
#define	ATK_DEF_CHANGING	0x02		// 현재 라운드가 공수교대되는 처음으로되는 경우

// 스킬 능력치 타입
enum SKILL_TYPE
{
	SKILLLIST_UNKNOWN = -1,		// 
	SKILLLIST_RUN = 0,		//	(케릭터)달리기
	SKILLLIST_WALK,						//	(케릭터)걷기
	SKILLLIST_DEFENSE,					//	(케릭터)방어력
	SKILLLIST_HELMETDEFENSE,			//	(케릭터)헬멧 방어력
	SKILLLIST_JUMP,						//	(케릭터)점프
	SKILLLIST_FIRESPEED,				//	(무기)연사력 향상
	SKILLLIST_DAMAGE,					//	(무기)공격력 향상
	SKILLLIST_ACCURACY,					//	(무기)명중률 상승
	SKILLLIST_RANGE,					//	(무기)발사 사정거리 증가
	SKILLLIST_PENETRATE,				//	(무기)관통력 향상
	SKILLLIST_RELOAD,					//	(무기)총기 장전 속도 향상
	SKILLLIST_CHANGE,					//	(무기)총기 교체 속도 향상
	SKILLLIST_BOOMRANGE,				//	(무기)투척물 폭발 범위 증가
	SKILLLIST_BOOMLENGTH,				//	(무기)투척 거리 증가
	SKILLLIST_BOOMTIME,					//	(무기)투척물 폭발 시간 단축
	SKILLLIST_EXP,						//	(특수)경험치 획득 량 증가
	SKILLLIST_POINT,					//	(특수)포인트 획득 량 증가
	SKILLLIST_MAX,						//
};

enum ROOM_ENDTYPE
{
	ROOM_ENDTYPE_NONE = 0,
	ROOM_ENDTYPE_WIN,
	ROOM_ENDTYPE_LOSE,
	ROOM_ENDTYPE_DRAW,
	ROOM_ENDTYPE_DISCONNECT,
	ROOM_ENDTYPE_GIVEUP,
	ROOM_ENDTYPE_HACKING,
	ROOM_ENDTYPE_GM_KICK,
	ROOM_ENDTYPE_VOTE,
	ROOM_ENDTYPE_DEDI_KICK,
};

// 정보 변경 종류
enum CHANGE_INFO
{
	CHANGE_INFO_NICKNAME = 0,	// 닉네임
	CHANGE_INFO_NICKCOLOR,		// 닉네임 컬러
	CHANGE_INFO_RANK,			// 랭크

	CHANGE_INFO_MAX
};

#define KICKVOTE_THROW_NONE			0
#define KICKVOTE_THROW_AGREE		1
#define KICKVOTE_THROW_DISAGREE		2


// URL List (Server, Client 모두 사용합니다.)

#define URL_MAX_COUNT 15			// 변경시 패킷 사이즈 고려해야합니다.
#define URL_STRING_MAX_COUNT 256

enum URL_TYPE
{
	URL_TYPE_BANNER = 0,			// 배너
	URL_TYPE_CLIENT_CLOSE,		// 클라이언트 종료 시점 사용
	URL_TYPE_INGAME_WEB,		// 인게임 버튼 클릭
	URL_TYPE_ESPORTS_BANNER,	// ESPORTS 배너
	URL_TYPE_WEB_SHOP,			// 웹 상점 팝업
	URL_TYPE_MAX
};

struct URL_LIST
{
	INT32	m_i32Type = 0;							//URL 타입 URL_TYPE 사용
	INT32	m_i32Data = 0;							//URL 순서,특정 값들 필요 할 경우 사용 없으면 기본0
	wchar_t	m_strURL[URL_STRING_MAX_COUNT] = { 0 };		//URL 경로
};

//======================================================

#if defined(_DEBUG) 
#define  _CONSOLELOG
#endif

#include "Common/CommonDef_Stage.h"
#include "Common/CommonDef_Base.h"
#include "Common/CommonDef_Contents.h"
#include "Common/CommonDef_Item.h"
#include "Common/CommonDef_Char.h"
#include "Common/CommonDef_Community.h"
#include "Common/CommonDef_Clan.h"
#include "Common/CommonDef_Event.h"
#include "Common/CommonDef_Quest.h"
#include "Common/CommonDef_Room.h"
#include "Common/CommonDef_System.h"
#include "Common/CommonDef_QuickJoin.h"
#include "Common/CommonDef_Skill.h"

#endif // #ifndef GAMEPORTAL_BUILD

#endif // #ifndef BUILD_VERSION_TOOL

#endif // #ifndef __COMMONDEF_H