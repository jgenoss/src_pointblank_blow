#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

#include "ControlServerDll.h"
#include "../../CommonServerSource/DestroyReason.h"
////////////////////////////////////////////////
// DEDICATION SERVER 컴파일 스위치 
//#define YJ_DEBUG_PROTOCOL			// #include "..\..\CommonServerSource\Debug_ProtocolUtil.h"를 끄거나 켠다.

//#define USE_SIA_DATABASE			// SIA Data base를 사용하는 기능을 켜거나 끈다.

#define NO_STR_TBL

#define	SERVER_MAIN_UPDATE_TIME	1

#define LOG_STRING_LEN			512

#define SQL_STRING_COUNT				512
#define SERVER_STRING_COUNT				512
#define MACADDRESS_COUNT				40
#define NOTICE_COUNT					5
#define STR_NOTICE_MESS_SIZE			255
#define DS_MAX_WEAPON_LIST				1024
#define MAX_COMMON_SERVER_COUNT			16
#define MAX_SERVER_COUNT				100
#define MAX_BATTLE_PORT_COUNT			20
#define MAX_SNDA_MONITER_USER_COUNT		10
#define KNOCK_SERVER_MESSAGE_COUNT		512

//#define ERROR_LOAD_CONFIG		0x80000001			// Config 에러
//#define ERROR_NETWORK			0x80000002			 
//#define ERROR_LOG_FILE			0x80000003			 
//#define ERROR_LOAD_OPTION_DB	0x80000004
//#define ERROR_DATABASE			0x80000005
//#define ERROR_SERVER_LOG_FILE	0x80000006
//#define ERROR_SESSION_MANAGER	0x80000007
//#define	ERROR_LOAD_SIA_DB		0x80000008
//#define ERROR_WEB_MANAGER		0x80000009
//#define ERROR_EMAIL				0x8000000A

//////////////////////////////////////////////////////////////////////////
// Data Base
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Update Server 스위치
//
// 기존 업데이트서버를 사용할 경우 이 디파인 있어야 합니다.
// 이후에 런처와 업데이트서버 버전을 맞추게 되면
// 이 디파인 제거 후 업데이트서버도 버전체크를 해서
// 다르면 종료되도록 합니다.
// 12월빌드 업데이트서버는 버전체크하는 부분이 이미 들어가있으니
// 참고 바랍니다.
// 2013-01-14 김용수
//#define OLD_UPDATESERVER
//////////////////////////////////////////////////////////////////////////

#define GAME_CHANNEL_USER_COUNT			500				// 게임서버 채널당 유저 수
#define GAME_CHANNEL_EXTRA_COUNT		300				// 게임서버 여유 유저 수

extern char g_strCurrentPath[512];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//글로벌 함수 
SERVER_EXPORT_BASE INT32	g_InitServer(void); 
SERVER_EXPORT_BASE void		g_UpdateServer(void); 
SERVER_EXPORT_BASE void		g_DestroyServer(void); 

// WEB Tool command
#define	WEB_TOOL_ACCEPT_DEFAULT_PORT		39020			// 기본 웹 툴 접속 포트 (config.ini에 설정 누락시 사용)
#define	WEB_SESSION_DEFAULT_COUNT			100				// 기본 웹 세션 카운트  (config.ini에 설정 누락시 사용)
#define WEB_GM_KICK_MAX_COUNT				100				// 동시에 킥 할 수 있는 최대 수

// SNDA Moniter Command
#define	SNDA_MONTER_ACCEPT_DEFAULT_PORT		7000			// 기본 샨다 모니터링 툴 접속 포트 (config.ini에 설정 누락시 사용)
#define	SNDA_MONTER_SESSION_DEFAULT_COUNT	100				// 기본 샨다 모니터링 툴 세션 카운트  (config.ini에 설정 누락시 사용)

#define MAX_ERROR_LOG_SIZE			256
  

#define ERROR_CREATE_MODULE_EMAIL_CREATEBUFFER	-100		// ModuleEmail 생성 실패 - 버퍼 생성 실패



/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////	  eMail Send System

// 메일 전송 사유
enum SEND_REASON
{	
	SEND_REASON_UNKNOWN					= 0,	// 알수 없는 이유.
	SEND_REASON_SERVER_UNNORMAL_DOWN	= 1,	// 서버 비정상 종료
	SEND_REASON_SERVER_TIMEOUT			= 2,	// 일정 시간 동안 컨트롤 서버와 통신이 안된다.
	SEND_REASON_DB						= 3,	// DB 문제
	SEND_REASON_NETWORK					= 4,	// 네트웤 문제
	SEND_REASON_TASK					= 5,	// TASK 문제
	
	SEND_REASON_MAX,	
};

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

#define SERVER_IP_SIZE				32

//new sc & control Tool과 호환용
#define PROTOCOL_SC_SERVER_OPER_REQ  0x1000

#define IP_ADDR_SIZE				32
#define SERVICE_NAME_COUNT			64

struct P_SC_SERVER_OPER_REQ
{
	WCHAR	_strIp[IP_ADDR_SIZE];						//서버IP 주소
	WCHAR	_strServerName[ SERVICE_NAME_COUNT ];		//서버이름	(서비스 이름)
	INT8	_i8ServerOper;								//수행동작 1 : ON		2 : OFF
};

struct SERVER_INFO_GAME : public SERVER_INFO_COMMON
{
	INT32   				_UserCount[GAME_CHANNEL_COUNT];
	UINT8					_EachType[GAME_CHANNEL_COUNT];
	void*					_pSession;
	UINT32					_OutIP;
	UINT16					_OutPort;
	UINT8					_AllType; 
	UINT16					_ui16MaxUserCount;		// 채널개수 * 채널당 MAX인원 수 + 추가 인원 MAX인원 수 = 총 한서버당 수용할수있는 최대 인원수
	UINT16					_ui16InitUserCount;		// 초기 채널당 MAX인원 수
	UINT16					_ui16ExtraCount;		// 서버가 수용할수있는 여분 인원수
	UINT16					_ui16NowUserCount;		// 실적용되고 동적으로 변경되는 _ui16InitUserCount값보다 클수없는 채널당 MAX인원 수  
	UINT32					m_ui32Group;
};

struct SERVER_INFO_BATTLE : public SERVER_INFO_COMMON
{
	UINT32					_OutIP;
	UINT8					_AllType; 
	UINT8					_EachType[GAME_CHANNEL_COUNT]; 
	BOOL					bNotAllowRoomCreation; // from control tool
	void*					_pSession;
	UINT32					m_ui32Group;

	SERVER_INFO_BATTLE::SERVER_INFO_BATTLE()
	{
		bNotAllowRoomCreation = FALSE;
	}
};

struct SERVER_INFO_CAST : public SERVER_INFO_COMMON
{
	UINT32					m_ui32GroupGameCount;
	UINT32					m_ui32GroupGameIdx[ MAX_SERVER_COUNT ];
	UINT32					m_ui32GroupBattleCount;
	UINT32					m_ui32GroupBattleIdx[ MAX_SERVER_COUNT ];
};

//글로벌 변수 
extern INT32			g_i32MainLogIdx;
extern i3LogFileW*		g_pLog;


#endif
