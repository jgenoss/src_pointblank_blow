#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

#include <pdh.h>
#include <pdhmsg.h>

#include "../../ShopInvenAgent/SIA/ControlProtocol.h"

#ifdef _DEBUG
	//#define RECREATE_BY_ONE_MINUTE
#endif

#define USE_MSSQL_2008				// MS SQL 2008 사용하는 경우

#define LOG_DB_COLUMN_MAX			37 // 최대 칼럼수
#define USE_SYSTEM_LOG



#define SERVER_LINK_NONE			0
#define SERVER_LINK_INDONESIA		1
#define SERVER_LINK_RUSSIA			2
#define SERVER_LINK_THAI			3
#define SERVER_LINK_KOREA			4

#define SERVER_MAIN_UPDATE_TIME			100
//----------------------------------------------------------------------------------------------
//Evnet Return Code 
#define EVENT_SUCCESS										0X00000000
#define EVENT_FAIL_ERROR									0x80000000	// 기본 실패

//#define EVENT_FAIL_CREATE_TASK								0X80000001
//#define EVENT_FAIL_CREATE_DATABASE							0X80000002
//#define EVENT_FAIL_CREATE_INI								0X80000003
//#define EVENT_FAIL_CREATE_LOG								0X80000006
//#define EVENT_FAIL_CREATE_SERVICE_LOG						0X80000007
//#define EVENT_FAIL_CREATE_INI_VALUE							0X80000008
//#define EVENT_FAIL_CREATE_NETWORK							0X80000009
//#define EVENT_FAIL_CREATE_CONTROL							0X8000000B
//#define EVENT_FAIL_CREATE_SYSTEMLOG							0X8000000C
//#define EVENT_FAIL_DATABASE_SQL								0X81000003

#define SERVER_EVENT_FAIL_REASON	30	

//Count or Size Define 
#define PATH_STRING_COUNT				512
#define BUFFER_MAX_INT_COUNT			2000000000
#define IP_NAME_COUNT					32

#define	LOG_FILE_BUFFER_COUNT			250
#define	SERVICE_LOG_BUFFER_COUNT		250
#define DATABASE_BUFFER_COUNT			500
#define NET_PACKET_BUFFER_SIZE			8912
#define SERVICE_LOG_FILE_SIZE			256
#define ACTION_ID_START_IDX				25
#define ACTION_ID_BUFFER_SIZE				6
#define CHATTING_COLUM						32
#define LOG_FILE_BUFFER_MAX				100000000
#define FILE_SIZE_MAX							1000000000 // 파일 로드시 최대 크기 ( 바이트 )
#define LOG_STRING_SIZE					256

#define	LOG_DB_QUERY_SIZE					2048
#define	LOG_DB_QUERY_COLUMN_SIZE			10
#define	LOG_DB_QUERY_VALUE_SIZE			(LOG_DB_QUERY_SIZE - (LOG_DB_QUERY_COLUMN_SIZE*LOG_DB_COLUMN_MAX) - 100)

//#define	CHECK_TRACE

//#include <process.h>
#define MAX_SIZE_DB_UPLOAD_THREAD	16
#define FILE_NAME_SIZE				MAX_PATH

#define WEBSHOP_MAX						10
// Server 에서 실제 데이타가 들어가는 인덱스번호입니다.
#define STRING_START_IDX				5		// 11 - 2 9 이 값을 수정하면 ACTION_ID_START_IDX 값도 수정하여야 합니다. 이 값이 줄어든만큼 더해주면 됩니다.	
//Thread Priority 
#define THREAD_PRIORITY_TASK			THREAD_PRIORITY_ABOVE_NORMAL	//
#define THREAD_PRIORITY_NETWORK			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LOG				THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_SERVICE_LOG		THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_DATABASE		THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LINK			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_WEBSHOP			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_CONTROL			THREAD_PRIORITY_NORMAL			//
#define THREAD_PRIORITY_LOG_PROCESSOR	THREAD_PRIORITY_NORMAL			//

//Thread Idx
#define THREAD_MAIN_IDX				-100
#define THREAD_CONTROL_IDX			-200
#define THREAD_FILE_MANAGER_IDX		-400
#define	THREAD_NETWORK_IDX			-500
#define THREAD_NET_SERVER_IDX		0
#define THREAD_DB_UPLOADER_IDX		1000

#define SERVER_STRING_COUNT			512

enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

//----------------------------------------------------------------------------------------------
//글로벌 변수	프로그램 시작시 설정됨 
extern char				g_strCCurrentPath[PATH_STRING_COUNT];			//파일 패스 
extern SERVER_STATE		g_eServerState;
extern i3LogFileW*		g_pLog;

//-------------------------------------------------------------------------
//글로벌 변수	내부에서 사용하는 변수 
extern UINT32			g_ui32StartTime;

//----------------------------------------------------------------------------------------------
//글로벌 함수 
void					g_SetCurrentPath(char* pstrPath);
int						g_InitServer(void); 
void					g_UpdateServer(void); 
void					g_DestroyServer(INT32 i32ErrorReason);

//유틸리티 함수
bool	GetDateFromFileName( wchar_t* pwstrFileName, wchar_t* pwbuffer, UINT8 ui8size );
bool	ChangeFileExtension( char* pstrFileName, char* pstrExtension, INT32 i32Size );
HANDLE	MakeFile( char* pstrFileName );

#define I3_PACKET_HEADER_SIZE	4

#pragma pack( push, 1 )
//////////////////////////////////////////////////////////////////////////////////////////
// Packet 정보
struct NET_I3_PACKET_BUFFER
{
	UINT16				_ui16DataSize;
	UINT16				_ui16Protocol; 	
	char				_Data[NET_PACKET_BUFFER_SIZE];
	UINT16				_ui16TotalSize;

	NET_I3_PACKET_BUFFER()
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
	}
	NET_I3_PACKET_BUFFER( UINT16 ui16Protocol )
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
		_ui16Protocol		= ui16Protocol;
	}
	void	Clear()
	{
		_ui16TotalSize		= 4;
		_ui16DataSize		= 0;
	}

	void	ReadClear()
	{
		_ui16TotalSize		= 0;
	}

	void	ReadData( void* pBuffer, UINT16 ui16BufferSize )
	{
#ifdef _DEBUG
		assert( (_ui16TotalSize + ui16BufferSize) < NET_PACKET_BUFFER_SIZE );
#endif
		if( 0 == ui16BufferSize ) return;

		i3mem::Copy( pBuffer, &_Data[_ui16TotalSize], ui16BufferSize );
		_ui16TotalSize	= _ui16TotalSize + ui16BufferSize;
	}

	void	ReadData(void* pBuffer, UINT16 ui16BufferSize, INT32* _i32CurrentReadPosition )
	{
		if( 0 == ui16BufferSize ) return;
		if(_ui16DataSize < (*_i32CurrentReadPosition + ui16BufferSize))	return;

		i3mem::Copy( pBuffer, &_Data[*_i32CurrentReadPosition], ui16BufferSize);
		*_i32CurrentReadPosition	+= ui16BufferSize;		
	}

	void	WriteData( void* pBuffer, UINT16 ui16BufferSize )
	{
#ifdef _DEBUG
		assert( (_ui16TotalSize + ui16BufferSize) < NET_PACKET_BUFFER_SIZE );
#endif
		if( 0 == ui16BufferSize ) return;

		i3mem::Copy( &_Data[_ui16DataSize], pBuffer, ui16BufferSize );
		_ui16DataSize	= _ui16DataSize + ui16BufferSize;
		_ui16TotalSize	= _ui16TotalSize + ui16BufferSize;
	}
	// 특정 위치에 데이터를 저장합니다. DataCount 는 증가하지 않습니다.
	void	WriteData( UINT16 ui16Pos, void* pBuffer, UINT16 ui16BufferSize )
	{
		if( 0 == ui16BufferSize ) return;
		i3mem::Copy( &_Data[ui16Pos], pBuffer, ui16BufferSize );
	}
};

//NC에서 쏴주는 패킷 형태.
//pBuff[0]~[1]		- 총 패킷 사이즈	( sizeof(UINT16)	)
//pBuff[2]			- ID				( sizeof(UINT8)		)		<<- 확실히 잘 모르겠음
//pBuff[3]~[6]		- GameID			( sizeof(UINT32)	)
//pBuff[7]~[10]		- 이벤트ID			( sizeof(UINT32)	)		시작관련-3, 서버관련-6, 유저-7, 체팅-9
//pBuff[11]			- 실제 데이타		( pBuff[0]			)

enum LOGD_EVENT_TYPE
{
	LOGD_EVENT_START		= 3,
	LOGD_EVENT_USER			= 6,
	LOGD_EVENT_SERVER		= 7,
	LOGD_EVENT_CHATTING		= 9,
};

enum Column_Name
{
	// 해당 항목의 변동이 생기면 ZLog 소스도 수정해야 합니다. ModuleTask.cpp 에 InsertDB() 함수에 보면 문자열 컬럼항목을 확인하여 입력하는 부분이 있습니다.
	COLUMN_ACT_TIME = 0, 
	COLUMN_LOG_ID,
	COLUMN_LOCALE_CODE,

	COLUMN_L_DATA01,
	COLUMN_L_DATA02, 
	COLUMN_L_DATA03, 

	COLUMN_I_DATA01, 
	COLUMN_I_DATA02, 
	COLUMN_I_DATA03, 
	COLUMN_I_DATA04,
	COLUMN_I_DATA05, 
	COLUMN_I_DATA06, 
	COLUMN_I_DATA07, 
	COLUMN_I_DATA08, 
	COLUMN_I_DATA09, 
	COLUMN_I_DATA10, 
	COLUMN_I_DATA11, 
	COLUMN_I_DATA12,
	COLUMN_I_DATA13,
	COLUMN_I_DATA14,
	COLUMN_I_DATA15,
	COLUMN_I_DATA16,
	COLUMN_I_DATA17,

	COLUMN_S_DATA01,
	COLUMN_S_DATA02, 
	COLUMN_S_DATA03,
	COLUMN_S_DATA04, 
	COLUMN_S_DATA05, 
	COLUMN_S_DATA06, 
	COLUMN_S_DATA07,
	COLUMN_S_DATA08, 
	COLUMN_S_DATA09, 
	COLUMN_S_DATA10,

	COLUMN_COUNT,
};

#define LOG_DATA_I_COUNT		COLUMN_I_DATA15-COLUMN_I_DATA1+1
#define LOG_DATA_STRING_COUNT	COLUMN_S_DATA10-COLUMN_S_DATA1+1

// DB Creator
#define		DATETIME_LENGTH		13				// DB로 전달할 시간 값 길이 (YYYY_MM_DD_HH)
#define		TABLENAME_LENGTH	50

#define		PRE_CREATE_SECOND	(60)			// 몇 초 전에 새로 테이블을 만들지

#define		NEXT_CREATE_TABLE	(60*60*24)		// 몇 시간 후에 새로 테이블을 만들지 (초)

#define		PBLOG_FORMAT_MAX	100				// PBLog Format 최대 개수

//////////////////////////////////////////////////////////////////////////////////////////

#pragma pack( pop )

#endif
