#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

//User PDH
#include <pdh.h>
#include <pdhmsg.h>

#include "../../CommonServerSource/DestroyReason.h"

#define NO_STR_TBL					//스트링 테이블 사용하지 않는다. 


//Define 

//////////////////////////////////////////////////////////////////////////
// Data Base
//////////////////////////////////////////////////////////////////////////
#define USE_DATABASE				//DATABASE를 사용할때 사용합니다. 

//////////////////////////////////////////////////////////////////////////

#define SERVER_MAIN_UPDATE_TIME		1000
#define	SERVER_STRING_COUNT			512 
#define SQL_STRING_COUNT			2048

//#define SERVER_ERROR_NETWORK		0X80000001
//#define SERVER_ERROR_CONFIG			0X80000002
//#define SERVER_ERROR_LOGFILE		0X80000003
//#define SERVER_ERROR_DATABASE		0X80000004
//#define SERVER_ERROR_CONTEXT		0X80000005
////#define SERVER_ERROR_NCUBS		0X80000007
//#define SERVER_ERROR_PATH			0X80000008
////#define SERVER_ERROR_USERLIST		0x80000009
//#define SERVER_ERROR_SYSTEMLOG		0x8000000A
//#define SERVER_ERROR_LOG			0x8000000B
//#define SERVER_ERROR_GIP			0x8000000C
//#define SEVER_ERROR_CLAN			0x8000000D
//#define SERVER_ERROR_CONTROLSERVER	0x8000000F
//#define SERVER_ERROR_GATEWAY		0x80000010
//#define SERVER_ERROR_TASKPROCESSOR	0x80000011
//#define SERVER_ERROR_DIFFERENT_BUILD_VERSION	0x80000012

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

//글로벌 변수 
extern UINT32			g_ConnectServer; 
extern char				g_pCCurrentPath[SERVER_STRING_COUNT]; 
extern SERVER_STATE		g_eServerState;
extern INT32			g_i32MainLogIdx;
extern i3LogFileW*		g_pLog;

//글로벌 함수 
void				g_SetCurrentPath(char		* pPath);  
INT32				g_InitServer(void);
INT32				g_StartServer(void);
void				g_UpdateServer(void); 
void				g_DestroyServer(INT32 i32DestroyReason);

#define LENGTH_CHECK( val, check )	{ if(val>check)	{val=check;} }

struct SERVER_BASE
{
	UINT32					m_ui32IP;
};

struct SERVER_BATTLE : public SERVER_BASE
{
	BOOL					m_bUseBattle;
	INT32					m_i32BattleCount;
};

#endif
