#include "pch.h"
#include "ModuleLog.h"

I3_CLASS_INSTANCE( CModuleLog, i3Thread );

CModuleLog * g_pModuleLogD = NULL; 

#define NEW_WHERE_DATA_IDX					0
#define NEW_WHERE_STRING_IDX				NEW_WHERE_DATA_IDX+1
#define NEW_STRING_START_IDX				NEW_WHERE_STRING_IDX+1

enum ZLOG_INSERT_TYPE
{
	ZLOG_LDATA01,	ZLOG_LDATA02,	ZLOG_LDATA03,
	ZLOG_IDATA01,	ZLOG_IDATA02,	ZLOG_IDATA03,
	ZLOG_IDATA04,	ZLOG_IDATA05,	ZLOG_IDATA06,
	ZLOG_IDATA07,	ZLOG_IDATA08,	ZLOG_IDATA09,
	ZLOG_IDATA10,	ZLOG_IDATA11,	ZLOG_IDATA12,
	ZLOG_IDATA13,	ZLOG_IDATA14,	ZLOG_IDATA15,
	ZLOG_IDATA16,	ZLOG_IDATA17,
	ZLOG_SDATA01,	ZLOG_SDATA02,	ZLOG_SDATA03,
	ZLOG_SDATA04,	ZLOG_SDATA05,	ZLOG_SDATA06,
	ZLOG_SDATA07,	ZLOG_SDATA08,	ZLOG_SDATA09,
	ZLOG_SDATA10,
};

#ifdef I3_DEBUG
#define ASSERT_COMP( expr )  { char Temp[ (expr) ? 1 : 0 ]; Temp;	} 
#else
#define ASSERT_COMP( expr )  (void)0
#endif

// ID, ID의 스트링 사이즈
#define START_DATA( ID, size )			memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], ID, size );				i16LogSize+= size;\
										_itoa(ui8LocaleCode, strtoa, 10);							i32StrLenth = i3String::Length(strtoa);\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;\
										strSendBuffer[NEW_WHERE_DATA_IDX]	= 'z';		strSendBuffer[NEW_WHERE_STRING_IDX]	= 'z';
// Insert Type, 값, String 일때 size
#define ADD_LDATA( datatype, values )	ASSERT_COMP( (ZLOG_LDATA01<= datatype) && (ZLOG_LDATA03 >= datatype) );\
										_i64toa(values, strtoa, 10);	i32StrLenth = i3String::Length(strtoa);\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_IDATA( datatype, values )	ASSERT_COMP( (ZLOG_IDATA01<= datatype) && (ZLOG_IDATA17 >= datatype) );\
										_itoa(values, strtoa, 10);	i32StrLenth = i3String::Length(strtoa);\
										if( ZLOG_IDATA01==datatype )	strSendBuffer[NEW_WHERE_DATA_IDX]	= (char)i32DataIdx + 'a';\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], strtoa, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;	i32DataIdx++;
#define ADD_SDATA( datatype, values )	ASSERT_COMP( (ZLOG_SDATA01<= datatype) && (ZLOG_SDATA10 >= datatype) );\
										i32StrLenth = i3String::Length(values);\
										if( ZLOG_SDATA01==datatype )	strSendBuffer[NEW_WHERE_STRING_IDX]	= (char)i32DataIdx + 'a';\
										memcpy( &strSendBuffer[i16LogSize], "&,", 2 );				i16LogSize += 2;\
										memcpy( &strSendBuffer[i16LogSize], values, i32StrLenth );	i16LogSize += (INT16)i32StrLenth;
#define END_DATA()						memcpy( &strSendBuffer[i16LogSize], "\r\n\0", 3 );			i16LogSize += 2;


//#define TRACE_PACKET			//로그를 TRACE로 출력

CModuleLog::CModuleLog(void)
{
	m_bRunning			= FALSE;;
	m_i32DateHour		= 0;
	m_hFile				= INVALID_HANDLE_VALUE;
	m_hBackupFile		= INVALID_HANDLE_VALUE;

	//Log
	m_i32RingCount			= LOG_MESSAGE_COUNT;
	m_pRingBuffer			= NULL;
}

CModuleLog::~CModuleLog(void)
{
	OnDestroy(); 
}

BOOL CModuleLog::OnCreate( void )
{
	m_pRingBuffer = new Local_CRingBuffer[ LOG_MESSAGE_COUNT ];
	if( NULL == m_pRingBuffer )																		return SERVER_ERROR_LOG;
	for( INT32 i = 0 ; i < LOG_MESSAGE_COUNT ; i++ )
	{
		if( FALSE == m_pRingBuffer[i].OnCreate( sizeof(LOG_SEND_MESSAGE), SEND_LOG_BUFFER_COUNT ) )	return SERVER_ERROR_LOG;
	}

	if( FALSE == _CreateFile( m_hFile, g_pConfig->m_strZLogFilePath ) )								return SERVER_ERROR_LOG;
	if( FALSE == _CreateFile( m_hBackupFile, g_pConfig->m_strZLogBackupPath ) )						return SERVER_ERROR_LOG;

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextB", 0, 4096, NULL, PRIORITY_NORMAL) )				return SERVER_ERROR_LOG; 
	g_pLog->WriteLog( L"[SUCC] MODULE_B(NC LOG_D, NC GIP) CONTEXT READY!" );

	_CleanupOldBackup(g_pConfig->m_i32ZLogKeepDay);
	
	return TRUE; 
}

void CModuleLog::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_DELETE_ARRAY( m_pRingBuffer );
}

#define STRING_START_IDX	11
#define MACHINE_NAME		"PB"
#define SERVER_TYPE			"AS"

void CModuleLog::SendLogMessage( LOG_MESSAGE LogId, UINT8 ui8LocaleCode, void* pParam1, void* pParam2 )
{
	if( LogId < 0 || LOG_MESSAGE_COUNT <= LogId )
	{
		g_pLog->WriteLog( L"Invalid LogId. (%d)", LogId);
		return;
	}
	
	Local_CRingBuffer* pRing = &m_pRingBuffer[LogId];
	
	LOG_SEND_MESSAGE* pSendBuffer = static_cast<LOG_SEND_MESSAGE*>( pRing->PushPointer() );
	if( NULL == pSendBuffer )
	{
		g_pLog->WriteLog( L"LogD Send Buffer Full");
		return;
	}

	//2. Set Buffer
	INT16 i16LogSize = NEW_STRING_START_IDX;
	char strtoa[32];
	INT32 i32StrLenth;
	INT32 i32DataIdx	= 3;
	char* strSendBuffer = pSendBuffer->_strMessage;
	sprintf( &strSendBuffer[i16LogSize], "%02d/%02d/%04d %02d:%02d:%02d.00",
		i3ThreadTimer::LocalTime().GetMonth(),
		i3ThreadTimer::LocalTime().GetDay(),
		i3ThreadTimer::LocalTime().GetYear(),
		i3ThreadTimer::LocalTime().GetHour(),
		i3ThreadTimer::LocalTime().GetMinute(),
		i3ThreadTimer::LocalTime().GetSecond() );
	i16LogSize += 22;

	//2. Set Buffer 
	switch(LogId)
	{
	case LOG_MESSAGE_CCU:
		{
			UINT32 ui32StCCU		= (UINT32)*((UINT32*)pParam1);

			START_DATA( "101", 3 );
			ADD_IDATA( ZLOG_IDATA01, ui32StCCU );
			END_DATA();
		}
		break; 
	case LOG_MESSAGE_LOCALE_CCU:
		{
			UINT32 ui32StLocaleCCU	= (UINT32)*((UINT32*)pParam1);

			START_DATA( "101", 3 );
			ADD_IDATA( ZLOG_IDATA01, ui32StLocaleCCU );
			END_DATA();
		}
		break;
	case LOG_MESSAGE_BUY_WEBSHOP:
		{
			if( NULL == pParam1 )		return;

			LOG_BUY*  stLog = (LOG_BUY*)pParam1;
			
			START_DATA("1110", 4);
			ADD_LDATA( ZLOG_LDATA01, stLog->m_TUID );
			ADD_IDATA( ZLOG_IDATA01, stLog->m_i32BuyPrice); 
			ADD_IDATA( ZLOG_IDATA02, stLog->m_TGoodsID );
			END_DATA();
		}
		break;

	case LOG_MESSAGE_GIFT_TASK			:
	case LOG_MESSAGE_GIFT_DBSHOP_0:
	case LOG_MESSAGE_GIFT_DBSHOP_1:
	case LOG_MESSAGE_GIFT_DBSHOP_2:
	case LOG_MESSAGE_GIFT_DBSHOP_3:
	case LOG_MESSAGE_GIFT_DBSHOP_4:
	case LOG_MESSAGE_GIFT_DBSHOP_5:
	case LOG_MESSAGE_GIFT_DBSHOP_6:
	case LOG_MESSAGE_GIFT_DBSHOP_7:
	case LOG_MESSAGE_GIFT_DBSHOP_8:
	case LOG_MESSAGE_GIFT_DBSHOP_9:
		{
			if( NULL == pParam1 )		return;
			
			LOG_GIFT*	pLog	= (LOG_GIFT*)pParam1;

			START_DATA( "1204", 4 );
			ADD_LDATA( ZLOG_LDATA01, pLog->m_TReceiverUID );
			ADD_LDATA( ZLOG_LDATA02, pLog->m_TSenderUID );

			ADD_IDATA( ZLOG_IDATA01, pLog->m_eRequestType );
			ADD_IDATA( ZLOG_IDATA02, pLog->m_TGoodsID );
			ADD_IDATA( ZLOG_IDATA03, pLog->m_eState );
			ADD_IDATA( ZLOG_IDATA04, pLog->m_ui32ExpireDate );
			
			END_DATA();
		}
		break;

	default: 
		i16LogSize = 0;
		break; 
	}
	
	if( i16LogSize == 0 )
		return;

	if( i16LogSize > LOGD_BUFFER_SIZE )
		i16LogSize = LOGD_BUFFER_SIZE;

	//Set Size
	pSendBuffer->_i16Size = i16LogSize;

	pRing->PushPointerIdx();
}

UINT32 CModuleLog::OnRunning( void * pUserData)
{
	g_pLog->AddLogThread( L"ZLOG_WRITE", 0, m_ThreadID );

	INT32				i;
	INT32				i32Count;
	INT32				i32SendCount;
	INT32				i32WriteSize;
	LOG_SEND_MESSAGE*	pSendMessage;
	BOOL				bWorking;

	m_bRunning	= TRUE;
	while( m_bRunning )
	{
		bWorking = FALSE;
		//Main Thread 
		for( i = 0 ; i < m_i32RingCount ; i++ )
		{
			i32SendCount = m_pRingBuffer[i].GetBufferCount();
			for( i32Count = 0 ; i32Count < i32SendCount ; i32Count++ )
			{
				pSendMessage = (LOG_SEND_MESSAGE*)m_pRingBuffer[i].Pop();
				::WriteFile( m_hFile, pSendMessage->_strMessage, pSendMessage->_i16Size, (LPDWORD)&i32WriteSize, NULL );
				::WriteFile( m_hBackupFile, pSendMessage->_strMessage, pSendMessage->_i16Size, (LPDWORD)&i32WriteSize, NULL );
				m_pRingBuffer[i].PopIdx();
				bWorking = TRUE;
			}

		}

		if( m_i32DateHour != i3ThreadTimer::LocalTime().GetHour() )
		{
			_CreateFile( m_hFile, g_pConfig->m_strZLogFilePath );
			_CreateFile( m_hBackupFile, g_pConfig->m_strZLogBackupPath );
			bWorking = TRUE;
		}

		if( FALSE == bWorking )
		{	// 한일이 없으면 1ms 휴식.
			::WaitForSingleObject( m_Handle, 1);
		}
	}
	return 0; 
}

BOOL CModuleLog::_CreateFile( HANDLE &hFile, char* strLogPath )
{
	char strFileName[128];
	//Close Old File 
	if( INVALID_HANDLE_VALUE != hFile )
	{
		::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format( strFileName, 128, "%s\\%04d%02d%02d%02d_TRANS.txt", strLogPath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );

	hFile = ::CreateFile( strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
	{
		::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE; 
		return FALSE;
	}
	else
	{
		SetFilePointer( hFile, 0, NULL, FILE_END );
	}

	m_i32DateHour = i3ThreadTimer::LocalTime().GetHour();

	return TRUE;
}

UINT32 CModuleLog::_CleanupOldBackup( INT32 i32Day )
{
	//Config값이 -1이면 자동삭제를 진행하지 않습니다.
	if( -1 == i32Day ) return 0;

	BOOL bRv = FALSE;
	struct _finddata_t	sc_file;
	char strSerachFolder[ MAX_PATH ];
	strcpy( strSerachFolder, g_pConfig->m_strZLogBackupPath );
	strcat( strSerachFolder, "\\*.*" );

	intptr_t hFile = _findfirst( strSerachFolder, &sc_file );
	if( -1L == hFile )
	{
		_findclose(hFile);
		// 로그남깁니다.
		//WRITE_LOG_FILE_MANAGER( "[ERR] Make Folder List Fail" );
		return bRv;
	}

	// 대기열 폴더 리스트를 재 정렬 합니다.
	while( 0 == _findnext( hFile, &sc_file ) ) 
	{
		// ".", ".." 는 검색 대상에서 제외 시킵니다.
		if( '.' == sc_file.name[0] ) continue;

		INT32 i32Time;
		i32Time = _GetFileDate( sc_file.name );
		SYSTEMTIME FileTime;
		
		FileTime.wYear		= (WORD)(i32Time / 1000000);
		FileTime.wMonth		= (i32Time / 10000) % 100;
		FileTime.wDay		= (i32Time / 100) % 100;
		FileTime.wHour		= i32Time % 100;
		FileTime.wMinute	= 0;
		FileTime.wSecond	= 0;

		DATE32 stDate;
		stDate = FileTime;

		if( i3ThreadTimer::LocalTime().DiffTime( stDate ) < i32Day * 86400 )		continue;

		char strDeleteFile[ MAX_STRING_COUNT ];
		i3String::Format( strDeleteFile, MAX_STRING_COUNT, "%s\\%s", g_pConfig->m_strZLogBackupPath, sc_file.name );
		DeleteFile( strDeleteFile );
				
	}
	_findclose(hFile);

	return 0;
}

INT32 CModuleLog::_GetFileDate( char* strHour )
{	
	INT32 i32Hour = 0;
	for( INT32 i = 0 ; i < 10 ; i++ )
	{
		i32Hour = i32Hour * 10 + (strHour[i]-'0');
	}
	return i32Hour;
}