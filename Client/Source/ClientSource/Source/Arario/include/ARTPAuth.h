#ifndef __ARTPAUTH_H__
#define __ARTPAUTH_H__

#include "ARTPCommon.h"

/**
 * artpdef
 * length defien of Array in protocol structure
 */
struct artpdef 
{
	enum 
	{
		AUTH_USERID_LEN			= 32,
		AUTH_USERPASSWORD_LEN	= 60, 
		AUTH_SESSIONKEY_LEN		= 300,
		AUTH_GAMEID_LEN			= 16,
		AUTH_ENDDATE_LEN		= 12,
	};
	enum 
	{
		UI_EMAIL_LEN			= 100,
		UI_INNERCODE_LEN		= 8,
	};
	enum 
	{
		IP4_LEN					= 20,
		MAX_TEMP_BUFSIZ			= 60,
		MACADDRESS_LEN			= 20,
		GAME_SPECIFIC_STRING_PARAM = 1024,
	};
};

/**
 * ArtpPktIndex
 */
struct ArtpPktIndex 
{
	enum 
	{
		PING,			//== 0
		GHOSTCHECK,
		AUTH,
		REQRET,
		CHECK,			// pkt index for checking something
		REPORT,
	};
};

/**
 * ArtpPktSubindex
 */
struct ArtpPktSubindex 
{
	enum 
	{
		REQRESULT				= 0,
		
		AUTH_AuthInfo			= 10,
		AUTH_IssueSessionKey	= 11,
		AUTH_RefreshSessionKey	= 12,
		AUTH_AuthBySessionKey	= 13,
		AUTH_PenaltyInfo		= 14,
		AUTH_AuthInfo2			= 15,
		AUTH_AuthInfo3			= 16,
		AUTH_AuthBySessionKeyV2	= 17,
		AUTH_AuthInfo4      	= 18,   //2015.07.30 Kim Dongock Add

		CHECK_ConnSource		= 20,	// for Netcafe check, or connection source check
		CHECK_ConnSource2		= 21,
		CHECK_HisIsMovedUser	= 22,

		REPORT_PCSpec			= 30,
		REPORT_MacAddress		= 31,
	};
};

/**
 * user type define
 */
struct ArtpUserType 
{
	enum 
	{
		GHOST	= 0,	// means nobody
		HOME	= 1,	// general user
		NETCAFE	= 2,	// user connected from netcafe
	};
};

#include <PshPack1.h>

//\struct ARTP_HEADER_V2
//\brief ARTP_HEADER_V2는 in/out을 한 구조체에서 끝내도록 공통 값을 다시 모았다.
struct ARTP_HEADER_V2 : public ARTP_HEADER
{
	// 공통의 리턴 값을 저장하는 곳 (OUT)
	LONG	m_result;
	// 공통의 IN 값을 저장하는 것 (IN)
	WCHAR	m_gameId[ artpdef::AUTH_GAMEID_LEN+1 ];	///< 어느 게임인가?

	ARTP_HEADER_V2()
	{
		m_result = 0;
		m_gameId[0] = L'\0';
	}
};

struct ARTP_AUTHINFO : public ARTP_HEADER 
{	
	WCHAR		m_userId		[ artpdef::AUTH_USERID_LEN+1 ];
	WCHAR		m_userPassword	[ artpdef::AUTH_USERPASSWORD_LEN+1 ];
	long		m_userIndex;
	//WCHAR		m_gameId		[ artpdef::AUTH_GAMEID_LEN+1 ]; 
};

/**
 * 확장된 규격의 신규 인증용 함수 (2010.4.22)
 */
struct ARTP_AUTHINFO2 : public ARTP_AUTHINFO 
{	
	WCHAR		m_gameId	[ artpdef::AUTH_GAMEID_LEN+1 ]; 
	WCHAR		m_MacAddress[ artpdef::MACADDRESS_LEN+1 ];
};

/**
 * 확장된(monsternet) 규격의 신규 인증용 함수 (2010.6.11)
 */
struct ARTP_AUTHINFO3 : public ARTP_AUTHINFO2 
{
	// rename only
};

/**
 * 세션키로 인증을 확인하는 경우, 게임서버용
 */
struct ARTP_AUTHINFOBYSESKEY : public ARTP_HEADER 
{	
	CHAR m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];
};

//\struct	ARTP_AUTHINFOBYSESKEY2 ARTPAuth.h "ARTPAuth.h"
//\brief	세션키 인증을 하는 두번째 패킷, 게임 서버용.
struct ARTP_AUTHINFOBYSESKEY_V2 : public ARTP_HEADER_V2 
{
	CHAR	m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];		///< 회원정보를 확인할 세션키
	WCHAR	m_userIP	[ artpdef::MAX_TEMP_BUFSIZ+1 ];			///< 유저의 IP 
	WCHAR	m_MacAddress[ artpdef::MACADDRESS_LEN+1 ];

	ARTP_AUTHINFOBYSESKEY_V2()
	{
		m_sessionKey[0] = L'\0';
		m_userIP[0] = L'\0';
		m_MacAddress[0] = L'\0';
	}
};

//\brief 내용은 같지만 이름이 같으면 코드가 햇갈리므로 재정의
typedef struct ARTP_AUTHINFOBYSESKEY_V2 ARTP_REQRESULT_AUTHINFOBYSESKEY_V2;

/*
 * not used
 */
struct ARTP_ISSUESESSIONKEY : public ARTP_HEADER 
{
	CHAR m_sessionKey [ artpdef::AUTH_SESSIONKEY_LEN+1 ];
} ;

/*
 * not used
 */
struct ARTP_REFRESHSESSIONKEY : public ARTP_HEADER 
{
	WCHAR m_userId    [ artpdef::AUTH_USERID_LEN+1 ];
	CHAR  m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];
} ;

struct ARTP_PENALTYINFO : public ARTP_HEADER 
{
	WCHAR m_userID[ artpdef::AUTH_USERID_LEN+1 ];
	WCHAR m_gameID[ artpdef::AUTH_GAMEID_LEN+1 ];
};


// Check Connection Source : for exam > NetCafe IP Check
struct ARTP_CHECK_CONNSOURCE : public ARTP_HEADER 
{
	LONG	m_checkType;								// ref ArtpUserType
	WCHAR	m_userIP[ artpdef::MAX_TEMP_BUFSIZ+1 ];
};

// Check Connection Source : for exam > NetCafe IP Check
struct ARTP_CHECK_CONNSOURCE2 : public ARTP_HEADER 
{
	LONG	m_checkType;								// ref ArtpUserType
	WCHAR	m_userIP[ artpdef::MAX_TEMP_BUFSIZ+1 ];
	WCHAR	m_gameId[ artpdef::AUTH_GAMEID_LEN+1 ];
};

#define ARTP_DEFINE_RETCLASS( myClassName, parentClassName )	\
	typedef struct t##myClassName : public parentClassName {	\
		LONG m_result;											\
	} myClassName

ARTP_DEFINE_RETCLASS( ARTP_REQRESULT						, ARTP_HEADER );
ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_SESSIONKEY				, ARTP_ISSUESESSIONKEY );
ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_REFRESHSESSIONKEY		, ARTP_REFRESHSESSIONKEY );
ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_AUTHINFOBYSESKEY		, ARTP_AUTHINFOBYSESKEY );
ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_CHECK_CONNSOURCE		, ARTP_HEADER );
ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_CHECK_CONNSOURCE2		, ARTP_HEADER );
//ARTP_DEFINE_RETCLASS( ARTP_REQRESULT_AUTHINFOBYSESKEY_V2	, ARTP_HEADER_V2 );

/**
 * ARTP_PENALTYINFO의 리턴 결과
 */
struct ARTP_REQRESULT_PENALTYINFO : public ARTP_PENALTYINFO 
{	
	LONG m_result;
	int m_penalty;
	WCHAR m_endDate[ artpdef::AUTH_ENDDATE_LEN+1 ];
};

/**
 * ARTP_AUTHINFO의 리턴 결과
 */
struct ARTP_REQRESULT_AUTHINFO : public ARTP_AUTHINFO 
{		
	CHAR m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];	
	LONG m_result;	
};

/**
 * ARTP_AUTHINFO2의 리턴 결과
 */
struct ARTP_REQRESULT_AUTHINFO2 : public ARTP_AUTHINFO2 
{		
	CHAR m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];	
	LONG m_result;	
};

/**
 * ARTP_AUTHINFO3의 리턴 결과
 *
 * m_gameSpecificStringParam 게임 전용 파라미터 
 *	ex of ec2) "CMDRV 0 net 5 base EARTH IP patch.monsternet.gscdn.com ACC1 1 85A7XYL32DRT"
 */
struct ARTP_REQRESULT_AUTHINFO3 : public ARTP_AUTHINFO3 
{
	CHAR  m_sessionKey[ artpdef::AUTH_SESSIONKEY_LEN+1 ];
	LONG  m_result;
	WCHAR m_gameSpecificStringParam[ artpdef::GAME_SPECIFIC_STRING_PARAM+1 ];
};

//\struct	ARTP_CHECKHIS_IsMovedUser_V2 ARTPAuth.h "ARTPAuth.h"
//\brief	HIS를 위한 임시 프로토콜 - 이관한 유저인가.
struct ARTP_CHECK_HISOnly_IsMovedUser_V2 : public ARTP_HEADER_V2 
{
	WCHAR m_userId[ artpdef::AUTH_USERID_LEN+1 ];

	ARTP_CHECK_HISOnly_IsMovedUser_V2()
	{
		m_userId[0] = L'\0';
	}
};
//\brief 내용은 같지만 이름이 같으면 코드가 햇갈리므로 재정의
typedef struct ARTP_CHECK_HISOnly_IsMovedUser_V2 ARTP_REQRESULT_CHECK_HISOnly_IsMovedUser_V2;

#include <PopPack.h>
#endif

