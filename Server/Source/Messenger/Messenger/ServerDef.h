#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

//User PDH
#include <pdh.h>
#include <pdhmsg.h>
#include "../../CommonServerSource/DestroyReason.h"

#define USE_LOG_FILE

//Define 
#define NO_STR_TBL					//스트링 테이블 사용하지 않는다. 

#define	SERVER_MAIN_UPDATE_TIME			1000
#define	SERVER_STRING_COUNT				512 
#define SQL_STRING_COUNT				2048

#define THREAD_IDX_MAIN					-100
#define THREAD_IDX_TASK					-200
#define THREAD_IDX_GATEWAY				-300
#define THREAD_IDX_LOGD					-400
#define THREAD_IDX_CONTROL				-500
#define THREAD_IDX_DATABASE_ST			100

#define THREAD_IDX_CHANGE_NICK			-600
#define THREAD_IDX_CHANGE_RANK			-700
#define THREAD_IDX_CHANGE_NICK_DB_ST	200
#define THREAD_IDX_CHANGE_RANK_DB_ST	300

//////////////////////////////////////////////////////////////////////////
// 개발 테스트용
//////////////////////////////////////////////////////////////////////////
//#define DEF_CPU_LOG
//#define DEF_TEST_LOG
//////////////////////////////////////////////////////////////////////////
// Data Base
//////////////////////////////////////////////////////////////////////////
#define DB_THREAD_COUNT				10			//DataBase Thread는 10개로 픽스됨 유저테이블을 3개로 픽스함  

//////////////////////////////////////////////////////////////////////////


enum FRIEND_WORK_TYPE
{
	FRIEND_CONTROL_AUTH		 = 0,
	FRIEND_CONTROL_DELETE,
	FRIEND_CONTROL_ADD,
	FRIEND_CHANGE_NICK,
	FRIEND_CHANGE_RANK,
	FRIEND_CHANGE_NICKCOLOR,
};

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

//글로벌 변수 
extern UINT32			g_ui32ConnectServer; 
extern char				g_pCCurrentPath[SERVER_STRING_COUNT]; 
extern SERVER_STATE		g_eServerState;
extern INT32			g_i32MainLogIdx;
extern i3LogFileW*		g_pLog;

//글로벌 함수 
void					g_SetCurrentPath(char* pstrPath);
INT32					g_InitServer(void); 
INT32					g_StartServer( void );
void					g_UpdateServer( void ); 
void					g_DestroyServer(INT32 i32DestroyReason);

static HANDLE			s_hHandleProcess = NULL;

#endif
