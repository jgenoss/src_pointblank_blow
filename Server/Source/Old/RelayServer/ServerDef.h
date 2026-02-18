#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

#define NO_STR_TBL
//#define USE_OVER_MULTICAST			// Host -> Guest packet은 무조건 모든 slot에 relay


#define SERVER_MAIN_UPDATE_TIME		1000


#define	SERVER_STRING_COUNT			512
#define LOG_UPDATE_TIME_STATE		9			// 10초

#define SERVER_UPDATE_MAIN			0x00000001
#define SERVER_UPDATE_LOGFILE		0x00000002

#define SERVER_ERROR_NETWORK		0X80000001
#define SERVER_ERROR_CONFIG			0X80000002
#define SERVER_ERROR_LOGFILE		0X80000003
#define SERVER_ERROR_SYSTEMLOG		0x80000004
#define SERVER_ERROR_CONTROLSERVER	0x80000005
#define SERVER_ERROR_UDPMODULE		0x80000006
#define SERVER_ERROR_GROUPMAKER		0x80000007
#define SERVER_ERROR_LOG			0x80000008
#define SERVER_ERROR_DIFFERENT_BUILD_VERSION		0x80000009

#define SERVER_STATE_NONE			0
#define SERVER_STATE_LOADING		1
#define SERVER_STATE_INIT			2
#define SERVER_STATE_INITING		3
#define SERVER_STATE_START			4

extern UINT32	g_ConnectServer;
extern char		g_pCCurrentPath[SERVER_STRING_COUNT];
extern BOOL		g_bPath;
extern INT32	g_i32StartServer;
extern UINT32	g_ui32StartTime;
extern BOOL		g_PdhStatusError;

extern UINT8	g_ui8ConBuildVerMonth;		// 컨트롤 서버 빌드 버전 Month
extern UINT8	g_ui8ConBuildVerDay;		// 컨트롤 서버 빌드 버전 Day

INT32			g_InitServer(void); 
INT32			g_StartServer(void);
void			g_UpdateServer(); 
void			g_DestroyServer(void);

void			g_SetCurrentPath(char * pPath);

#endif
