#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

////////////////////////////////////////////////
// DEDICATION SERVER 컴파일 스위치 

#define USE_VRD			1			//physX 디버거 사용
typedef	void*	NxPtr;					// PhysX 2.8.4에서 define. 현재 버전은 2.8.1 사용

/////////////////////////////////////////////////////////

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
#define SERVER_ERROR_UDPSERVICE		0x80000009 
#define SERVER_ERROR_TASKMANAGER	0x8000000a
#define SERVER_ERROR_STATISTICS		0x8000000b
#define SERVER_ERROR_RESOURCE		0x8000000c
#define SERVER_ERROR_PHYSICS		0x8000000d
#define SERVER_ERROR_MEMORY			0x8000000e

#define SERVER_STATE_NONE			0
#define SERVER_STATE_LOADING		1
#define SERVER_STATE_INIT			2
#define SERVER_STATE_INITING		3
#define SERVER_STATE_START			4


// global variable
extern HANDLE g_hevtQuit;

// service interface
INT32			g_InitServer(void); 
INT32			g_StartServer(void);
void			g_UpdateServer(void); 
void			g_DestroyServer(void);
void			g_SetCurrentPath(LPCTSTR pPath);

#define INFINIY_HP		0xFFFF
UINT16			g_GetHP( UINT32 iCharacter , UINT32 iItem ); 
#endif
