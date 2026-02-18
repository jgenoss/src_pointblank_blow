#ifndef __COUNTRY_IP_H
#define	__COUNTRY_IP_H

//////////////////////////////////////////////////////////////////////////

// PB 작업을 위한 헤더입니다.
// PB 프로젝트 외에서 작업을 금합니다.

//////////////////////////////////////////////////////////////////////////

//#define USE_WATCH_CONTENTS				// (Client)			Watch 기능에 관한 사용 유/무
//#define USE_SERVER_WATCH_SAND_UDP		// (Control Server) Watch 서버로 정보전송 기능사용 유/무
//#define USE_SERVER_WATCH_CONTENTS		// (Game Server)	Watch 기능에 관한 사용 유/무(HeartBit2)

//////////////////////////////////////////////////////////////////////////

#define VER_WATCH_PACKET_C		1
#define VER_WATCH_PACKET_S		1
#define VER_WATCH_PACKET_GS		1

#define WATCH_DOMAIN			"pserver.zepetto.com"
#define WATCH_IP				"117.52.163.34"
#define WATCH_PORT				41000
#define WATCH_CONTROL_PORT		42000

//////////////////////////////////////////////////////////////////////////
// TypeDef
//////////////////////////////////////////////////////////////////////////

typedef UINT16					UDP_PACKET_SIZE;

//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////

enum	ENUM_GAME_CODE
{
	GAME_CORD_NONE		= 0,
	GAME_CORD_PB,
	GAME_CORD_ROAR,
	GAME_CORD_SOW,

	MAX_GAME_CORD_COUNT
};

enum	ENUM_UDP_TYPE
{
	UDP_NONE_TYPE			= 0,
	UDP_CLINET_TYPE,
	UDP_CONTROL_TYPE,

	MAX_UDP_TYPE_COUNT
};


//////////////////////////////////////////////////////////////////////////
// DEFINE
//////////////////////////////////////////////////////////////////////////
#define		MAX_NET_IP						16

#define		MAX_UDP_WATCH_PACKET_SIZE		2048
#define		MAX_PACKET_UDP_SIZE				1383

#define		MAX_WATCH_LIST_COUNT			100

//////////////////////////////////////////////////////////////////////////

#pragma pack( push, 1)
//=================================================================================
//	New Version Format
//=================================================================================
struct WATCH_VERSION	// 서버-클라
{
	UINT8		m_ui8Version;		// 패킷은 버전 :1부터 시작함 
	UINT32		m_ui32GameCode;		// 게임코드( ENUM_GAME_CODE )	
	UINT32		m_ui32LocaleCode;	// 국가 코드(LOCALE_CODE)
	UINT32		m_ui32ServerIP;		// 서버 아이피 
	UINT32		m_ui32MyIP;			// 자신 아이피  

	//////////////////////////////////////////////////////////////////////////
	WATCH_VERSION::WATCH_VERSION()
	{
		m_ui32GameCode		= GAME_CORD_NONE;
		m_ui32LocaleCode	= 0;
		m_ui32ServerIP		= 0;
		m_ui32MyIP			= 0;
	}

	void	CopyTo( WATCH_VERSION * pDest )	
	{
		pDest->m_ui8Version		= m_ui8Version;
		pDest->m_ui32GameCode	= m_ui32GameCode;
		pDest->m_ui32LocaleCode	= m_ui32LocaleCode;
		pDest->m_ui32ServerIP	= m_ui32ServerIP;
		pDest->m_ui32MyIP		= m_ui32MyIP;
	}

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (WATCH_VERSION*) pPacket);
		return (UINT16) sizeof(WATCH_VERSION);
	}
};

//////////////////////////////////////////////////////////////////////////
// Watch List
//////////////////////////////////////////////////////////////////////////

struct WATCH_LIST
{
	UINT32					m_ui32ServerCount;
	WATCH_VERSION			m_WatchVersion;
};

struct WATCH_CONTROL_INFO
{
	UINT32					m_ui32MaxCCU_HourMinSec;
	UINT32					m_ui32MaxCCU;
	UINT32					m_ui32CCU;
	UINT32					m_ui32RecvHeartBit2TotalCount;

	//////////////////////////////////////////////////////////////////////////
	WATCH_CONTROL_INFO::WATCH_CONTROL_INFO()
	{
		m_ui32MaxCCU_HourMinSec			= 0;
		m_ui32MaxCCU					= 0;
		m_ui32CCU						= 0;
		m_ui32RecvHeartBit2TotalCount	= 0;
	}

	void	CopyTo( WATCH_CONTROL_INFO * pDest )	
	{
		pDest->m_ui32MaxCCU_HourMinSec			= m_ui32MaxCCU_HourMinSec;
		pDest->m_ui32MaxCCU						= m_ui32MaxCCU;
		pDest->m_ui32CCU						= m_ui32CCU;
		pDest->m_ui32RecvHeartBit2TotalCount	= m_ui32RecvHeartBit2TotalCount;
	}

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (WATCH_CONTROL_INFO*) pPacket);
		return (UINT16) sizeof(WATCH_CONTROL_INFO);
	}
};

//////////////////////////////////////////////////////////////////////////
// Header
//////////////////////////////////////////////////////////////////////////

struct UDP_WATCH_HEADER
{
	UINT8	m_ui8ProtocolID;	// 데이타 프로토콜 타입
	REAL32	m_fTimeStampe;		// Game Time
	UINT16	m_ui16Size;			// 패킷 사이즈
	UINT8	m_ui8Type;			// 패킷 타입(ENUM_UDP_TYPE)

	void	Reset()
	{
		m_ui8ProtocolID = 0;
		m_fTimeStampe = 0.f;
		m_ui16Size = 0;
		m_ui8Type = 0;
	}

	void	CopyTo( UDP_WATCH_HEADER * pDest )
	{
		pDest->m_ui8ProtocolID		= m_ui8ProtocolID;
		pDest->m_fTimeStampe		= m_fTimeStampe;
		pDest->m_ui16Size			= m_ui16Size;
		pDest->m_ui8Type		 	= m_ui8Type;
	}

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (UDP_WATCH_HEADER*) pPacket);
		return (UINT16) sizeof(UDP_WATCH_HEADER);
	}
};	// 8bytes

//////////////////////////////////////////////////////////////////////////
// Packet Buffer
//////////////////////////////////////////////////////////////////////////

struct UDP_WATCH_PACKET_RECV
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	//UDP_PACKET_SIZE			m_TSize;
	UDP_WATCH_HEADER		m_Header;
	char					m_pBuffer[ MAX_PACKET_UDP_SIZE ];
};

struct UDP_WATCH_PACKET_SEND
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	UDP_PACKET_SIZE			m_TSize;
	char					m_pBuffer[ MAX_PACKET_UDP_SIZE ];
};

//////////////////////////////////////////////////////////////////////////
// Server Packet
//////////////////////////////////////////////////////////////////////////

struct S_PACKET_UDP_CLIENT_INFO
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	//UDP_PACKET_SIZE			m_TSize;
	UDP_WATCH_HEADER		m_Header;
	WATCH_VERSION			m_WatchVersion;	
};

struct S_PACKET_UDP_CONTROL_INFO
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	//UDP_PACKET_SIZE			m_TSize;
	UDP_WATCH_HEADER		m_Header;
	WATCH_VERSION			m_WatchVersion;	
	WATCH_CONTROL_INFO		m_WatchControlInfo;
};

struct GAME_SERVER_VERSION
{
	UINT8	_vGSVInfo;
	UINT8	_LocaleCode;
	UINT16	_vGame;
	UINT16	_vBuild;
	UINT16	_vSC;
	UINT16	_vSS;
	UINT16	_vUDPMain;
	UINT16	_vUDPSub;
	UINT8	_GameGuard;

	char	_vDate[15];
	char	_vTime[15];
	char	_vBuildType[6];

	GAME_SERVER_VERSION::GAME_SERVER_VERSION()
	{
		_vGSVInfo	= 0;
		_LocaleCode	= 0;
		_vGame		= 0;
		_vBuild		= 0;
		_vSC		= 0;
		_vSS		= 0;
		_vUDPMain	= 0;
		_vUDPSub	= 0;
		_GameGuard	= 0;

		memset( _vDate, 0, sizeof( _vDate ) );
		memset( _vTime, 0, sizeof( _vTime ) );
		memset( _vBuildType, 0, sizeof( _vBuildType ) );
	}
};
//////////////////////////////////////////////////////////////////////////
// Data Field
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//					WATCH_CLIENT_PROTOCOL UDP							//
//////////////////////////////////////////////////////////////////////////

struct PACKET_UDP_CLIENT_INFO
{
	UDP_WATCH_HEADER		m_Header;
	WATCH_VERSION			m_WatchVersion;	
};

//////////////////////////////////////////////////////////////////////////
//					WATCH_SERVER_PROTOCOL UDP							//
//////////////////////////////////////////////////////////////////////////

struct PACKET_UDP_CONTROL_INFO
{
	UDP_WATCH_HEADER		m_Header;
	WATCH_VERSION			m_WatchVersion;	
	WATCH_CONTROL_INFO		m_WatchControlInfo;
};

//////////////////////////////////////////////////////////////////////////

//=================================================================================
#pragma pack( pop )


//////////////////////////////////////////////////////////////////////////
// Function
//////////////////////////////////////////////////////////////////////////

INT16 g_WatchEncryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize );
INT16 g_WatchDecryption(char * pPacket, INT16 iPacketsize, INT16 iBuffersize);

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// ERROR CORD
//////////////////////////////////////////////////////////////////////////
#define WATCH_EVENT_SUCCESS									0x00000001	// 기본 성공
#define WATCH_EVENT_FAIL									0x00000001	// 기본 실패


//////////////////////////////////////////////////////////////////////////
//					WATCH_CLIENT_PROTOCOL UDP							//
//////////////////////////////////////////////////////////////////////////

enum UDP_WATCH_CLIENT_PROTOCOL_LIST
{
	UDP_WATCH_CLIENT				= 0x0000,
	UDP_WATCH_CLIENT_INFO,
};

//////////////////////////////////////////////////////////////////////////
//					WATCH_SERVER_PROTOCOL UDP							//
//////////////////////////////////////////////////////////////////////////

enum UDP_WATCH_CONTROL_PROTOCOL_LIST
{
	UDP_WATCH_CONTROL				= 0x0002,
	UDP_WATCH_CONTROL_INFO,
};

//////////////////////////////////////////////////////////////////////////

/****************************************************************************************/
//					국가 설정
//
//	외국일 경우 국가에 맞게 Build Symbol을 Preprocessor Definitions에 추가 바람
//	LOCALE_CODE 에 해당 나라 값을 넣어주세요.
//
//	한국:		LOCALE_KOREA
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
//  통합빌드:	LOCALE_CODE_TOTAL
/****************************************************************************************/

enum	ENUM_DEV_GATEWAY_TYPE
{
	ENUM_DEV_GATEWAY_MIN	= 0,
	ENUM_DEV_GATEWAY_MAX,

	MAX_DEV_GATEWAY_COUNT
};

const char	g_strDevGateWayIp[MAX_DEV_GATEWAY_COUNT][MAX_NET_IP] = 
{
	"115.91.29.000",
	"115.91.29.255"
};

#if defined( LOCALE_KOREA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"115.91.29.132",
	"115.91.29.132"
};

#elif defined( LOCALE_THAILAND)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"203.117.124.121", //QA
	"112.121.148.36", //Live
};

#elif defined( LOCALE_INDONESIA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,
	// TODO : PBNC 추가필요

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"203.89.146.158",
	"203.89.147.208"
};

#elif defined( LOCALE_RUSSIA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_PTS1,
	ENUM_GATEWAY_PTS2,
	ENUM_GATEWAY_PTS3,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"109.105.134.72",
	"109.105.140.112",
	"109.105.140.118",
	"109.105.140.124"
};

#elif defined( LOCALE_TURKEY)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA1,
	ENUM_GATEWAY_QA2,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"85.111.16.11",		// 터키 Live 서버
	"115.91.29.151",	// 터키 QA1 서버
	"85.111.16.31",		// 터키 QA2 서버	
};

#elif defined( LOCALE_CHINA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"115.91.29.132",
	"115.91.29.132"
};

#elif defined( LOCALE_BRAZIL)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"201.77.235.143",
	"201.77.235.189"
};

#elif defined( LOCALE_LATIN_AMERICA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA1,
	ENUM_GATEWAY_QA2,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"69.60.102.155",
	"69.60.102.153",
	"69.60.102.154"
};


#elif defined( LOCALE_NORTHAMERICA)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA1,
	ENUM_GATEWAY_QA2,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"216.4.205.141",
	"216.4.205.200",
	"216.4.205.201",
};


#elif defined( LOCALE_ITALY)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"195.81.116.48",
	"195.81.116.43",
};


#elif defined( LOCALE_ENGLISH)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"115.91.29.132",
	"115.91.29.132"
};

#elif defined( LOCALE_MIDDLE_EAST)

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"94.236.48.39",
	"94.236.48.46"
};

#elif defined( LOCALE_PHILIPPINES )

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"123.242.206.34",
	"123.242.206.80"
};

#elif defined( LOCALE_TAIWAN )

enum	ENUM_GATEWAY_TYPE
{
	ENUM_GATEWAY_LIVE	= 0,
	ENUM_GATEWAY_QA,
	ENUM_GATEWAY_QA2,

	MAX_GATEWAY_COUNT
};

const char	g_strGateWayIp[MAX_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"210.244.38.13",
	"210.244.38.49",
	"210.244.38.53"
};

#elif defined( LOCALE_TOTAL )

#define MAX_INSIDE_GATEWAY_COUNT	2

const char	g_strInsideGateWayIp[MAX_INSIDE_GATEWAY_COUNT][NET_CLIENT_IP] = 
{
	"115.91.29.132",
	"115.91.29.132"
};

#endif

//////////////////////////////////////////////////////////////////////////
#endif // #ifndef __COUNTRY_IP_H