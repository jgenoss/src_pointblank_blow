#include "pch.h"
#include "ModuleLogFile.h"
#include "ModuleConfig.h"
#include <process.h>

 // 2011.04.19 - 서동권 , 현재 이 클래스는 사용하지 않습니다.

CModuleLogFile * g_pModuleLogFile = NULL; 

static unsigned __stdcall _DefThreadProc( void* pPointer )
{pPointer;
	g_pModuleLogFile->OnWorking();
	_endthreadex(0);
	return 0;
}

CModuleLogFile::CModuleLogFile(void)
{
	m_hThread			= NULL;
	m_bIsRunning		= false;

	m_pMain				= NULL;
	m_pControl			= NULL;
	m_ppDBUploader		= NULL;
	m_pFileManager		= NULL;
	m_pNetwork			= NULL;
	m_ppNetServer		= NULL;
	
	m_i32DBUploader		= 0;
	m_i32NetServer		= 0;

	for( INT32 i = 0; i < SIA_WARNING_TYPE_COUNT; i++)
	{
		for( INT32 j = 0; j < SIA_WARNING_LEVEL_COUNT; j++ )
		{
			m_ui32WriteTime[i][j] = 0;
			m_ui32ReadTime[i][j] = 0; 
		}
	}
}

CModuleLogFile::~CModuleLogFile(void)
{	
	OnDestroy(); 
}

bool CModuleLogFile::OnCreate(void)
{
	m_ui32FileHour	= 2000000; 
	m_hFile			= INVALID_HANDLE_VALUE; 

	//1. Create Buffer 
	m_pMain = new NSM_CRingBuffer;
	if( NULL == m_pMain )																			return false;
	if( false == m_pMain->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT) )				return false;

	m_pControl = new NSM_CRingBuffer;
	if( NULL == m_pControl )																		return false;
	if( false == m_pControl->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT) )				return false;

	m_pFileManager	= new NSM_CRingBuffer;
	if( NULL == m_pFileManager )																	return false;
	if( false == m_pFileManager->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT))			return false;

	m_pNetwork = new NSM_CRingBuffer;
	if( NULL == m_pNetwork )																		return false;
	if( false == m_pNetwork->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT))				return false;

	m_i32DBUploader = g_pConfig->m_ui8WriteDBThreadCount;	
	m_ppDBUploader	= new NSM_CRingBuffer*[ m_i32DBUploader ];
	if( NULL == m_ppDBUploader )																	return false;
	for( INT32 i = 0 ; i < m_i32DBUploader ; i++ )
	{
		m_ppDBUploader[i]	= new NSM_CRingBuffer;
		if( NULL == m_ppDBUploader[i] )																return false;
		if( false == m_ppDBUploader[i]->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT) )	return false;
	}

	//2. Create File ; 	
	if( FALSE == _CreateFile() )																	return false; 

	//3. Make Thread 
	UINT32	ui32ThreadID;
	m_hThread = ( HANDLE )_beginthreadex(NULL, 0, _DefThreadProc, this, 0, &ui32ThreadID);
	if( NULL == m_hThread )																			return false; 
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_LOG) )								return false; 
    
	return true; 
}

bool CModuleLogFile::OnCreate2th()
{
	m_ppNetServer = (NSM_CRingBuffer**)NSM_ALLOC( sizeof(NSM_CRingBuffer*) * g_pConfig->m_ui32ServerCount );
	if( NULL == m_ppNetServer )																		return false;
	for( UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; i++ )
	{
		m_ppNetServer[i] = new NSM_CRingBuffer;
		if( NULL == m_ppNetServer[i] )																return false;
		if( false == m_ppNetServer[i]->OnCreate(sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT) )	return false;
	}
	m_i32NetServer = g_pConfig->m_ui32ServerCount;

	return true;
}

void CModuleLogFile::OnDestroy(void)
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );

	//Destroy Buffer 
	if( m_ppNetServer )
	{
		for( INT32 i = 0; i < m_i32NetServer ; i++ )
		{
			SAFE_DELETE( m_ppNetServer[ i ] );
		}
		NSM_SAFE_FREE( m_ppNetServer );
	}
	if( m_ppDBUploader )
	{
		for( INT32 i = 0; i < m_i32DBUploader ; i++ )
		{
			SAFE_DELETE( m_ppDBUploader[ i ] );
		}
		SAFE_DELETE_ARRAY( m_ppDBUploader );
	}

	SAFE_DELETE( m_pNetwork );
	SAFE_DELETE( m_pFileManager );
	SAFE_DELETE( m_ppDBUploader );
	SAFE_DELETE( m_pControl );
	SAFE_DELETE( m_pMain );
}

void CModuleLogFile::OnWorking(void)
{
	INT32 iSendCount; 
	LOG_FILE_BUFFER * pLogBuffer;
	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		if( m_ui32FileHour != g_ui32Hour )_CreateFile();

		// Main
		iSendCount = m_pMain->GetBufferCount();
		for(INT32 i = 0; i < iSendCount; ++i)
		{
			pLogBuffer = (LOG_FILE_BUFFER *)m_pMain->Pop(); 
			_WriteLogFile( pLogBuffer->_szMessage );
			m_pMain->PopIdx(); 			
		}

		// Control 
	 	iSendCount = m_pControl->GetBufferCount();
		for(INT32 i = 0; i < iSendCount; ++i)
		{
			pLogBuffer = (LOG_FILE_BUFFER *)m_pControl->Pop(); 
			_WriteLogFile( pLogBuffer->_szMessage );
			m_pControl->PopIdx(); 			
		}

		//m_pFileManager
		iSendCount = m_pFileManager->GetBufferCount(); 
		for(INT32 i = 0; i < iSendCount; ++i)
		{
			pLogBuffer = (LOG_FILE_BUFFER *)m_pFileManager->Pop(); 
			_WriteLogFile( pLogBuffer->_szMessage ); 
			m_pFileManager->PopIdx(); 
		}

		//m_pNetwork
		iSendCount = m_pNetwork->GetBufferCount(); 
		for(INT32 i = 0; i < iSendCount; ++i)
		{
			pLogBuffer = (LOG_FILE_BUFFER*)m_pNetwork->Pop(); 
			_WriteLogFile( pLogBuffer->_szMessage ); 
			m_pNetwork->PopIdx();
		}

		// m_ppDBUploader
		for( INT32 i = 0; i < m_i32DBUploader ; i++ )
		{
			iSendCount = m_ppDBUploader[ i ]->GetBufferCount();
			for( INT32 j = 0; j < iSendCount; ++j )
			{
				pLogBuffer = (LOG_FILE_BUFFER *)m_ppDBUploader[ i ]->Pop();
				_WriteLogFile( pLogBuffer->_szMessage );
				m_ppDBUploader[ i ]->PopIdx();
			}
		}

		// m_ppNetServer
		for( INT32 i = 0; i < m_i32NetServer ; i++ )
		{
			iSendCount = m_ppNetServer[ i ]->GetBufferCount();
			for( INT32 j = 0; j < iSendCount; ++j )
			{
				pLogBuffer = (LOG_FILE_BUFFER *)m_ppNetServer[ i ]->Pop();
				_WriteLogFile( pLogBuffer->_szMessage );
				m_ppNetServer[ i ]->PopIdx();
			}
		}
		::WaitForSingleObject( m_hThread, 1 );
	}

	return; 
}

bool CModuleLogFile::_CreateFile(void)
{
	bool bRv = true;
	char FileName[PATH_STRING_COUNT]; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format(FileName, PATH_STRING_COUNT, "%s\\ZLog_%04d_%02d_%02d_%02d.txt", g_pConfig->m_strLogFilePath, g_ui32Year, g_ui32Month, g_ui32Day, g_ui32Hour);

#ifdef UNICODE
	WCHAR16 wchar[PATH_STRING_COUNT]; 
	i3String::CopyASCIItoWCHAR(wchar, FileName); 
	m_hFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	if( NSMString::Length(FileName) >= PATH_STRING_COUNT)		return false;

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		bRv = false;
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);
	}

	if(bRv)
	{
		m_ui32FileHour = g_ui32Hour;

		char strMessage[512]; 
		i3String::Format( strMessage, 512, "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s , StartDate:%d --\r\n", VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL, __DATE__, __TIME__, g_ui32StartTime );
		INT32 i32Len = i3String::Length( strMessage ); 
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return bRv;
}

void CModuleLogFile::Write_LogFile(INT32 i32ThreadIdx, char * pLogString, INT32 i32WarningType, INT32 i32WarningLevel)
{
	NSM_CRingBuffer	* pRingBuffer = NULL; 
	LOG_FILE_BUFFER	 LogBuffer; 

	switch( i32ThreadIdx )
	{
	case THREAD_MAIN_IDX			:	pRingBuffer = m_pMain;				break; 
	case THREAD_CONTROL_IDX			:	pRingBuffer = m_pControl;			break; 
	case THREAD_FILE_MANAGER_IDX	:	pRingBuffer = m_pFileManager;		break; 
	case THREAD_NETWORK_IDX			:	pRingBuffer = m_pNetwork;			break; 
	default							:
		{
			if( THREAD_NET_SERVER_IDX <= i32ThreadIdx &&
				(m_i32NetServer+THREAD_NET_SERVER_IDX) > i32ThreadIdx )
			{
				pRingBuffer = m_ppNetServer[ i32ThreadIdx-THREAD_NET_SERVER_IDX ];
			}
			else if( THREAD_DB_UPLOADER_IDX <= i32ThreadIdx &&
				(m_i32DBUploader+THREAD_DB_UPLOADER_IDX) > i32ThreadIdx )
			{
				pRingBuffer = m_ppDBUploader[ i32ThreadIdx-THREAD_DB_UPLOADER_IDX ];
			}
			else
			{
				return;
			}
		}
		break;
	}

	char szTime[256];

	switch( i32ThreadIdx )
	{
	case THREAD_MAIN_IDX			:	i3String::Format( szTime, 256, "[%02d:%02d][MAIN]",	g_ui32Minute, g_ui32Second);	break; 
	case THREAD_CONTROL_IDX			:	i3String::Format( szTime, 256, "[%02d:%02d][CONT]",	g_ui32Minute, g_ui32Second);	break; 
	case THREAD_FILE_MANAGER_IDX	:	i3String::Format( szTime, 256, "[%02d:%02d][FLMA]",	g_ui32Minute, g_ui32Second);	break; 
	case THREAD_NETWORK_IDX			:	i3String::Format( szTime, 256, "[%02d:%02d][NETW]",	g_ui32Minute, g_ui32Second);	break;
	default					: 
		{
			if( THREAD_NET_SERVER_IDX <= i32ThreadIdx &&
				(INT32)g_pConfig->m_ui32ServerCount > i32ThreadIdx )
			{
				i3String::Format( szTime, 256, "[%02d:%02d][NE%02d]",g_ui32Minute, g_ui32Second, i32ThreadIdx-THREAD_NET_SERVER_IDX );
			}
			else if( THREAD_DB_UPLOADER_IDX <= i32ThreadIdx &&
				(m_i32DBUploader+THREAD_DB_UPLOADER_IDX) > i32ThreadIdx )
			{
				i3String::Format( szTime, 256, "[%02d:%02d][DB%02d]",g_ui32Minute, g_ui32Second, i32ThreadIdx-THREAD_DB_UPLOADER_IDX );
			}
		}
	}

	i3String::Format(LogBuffer._szMessage, 1024, "%s%s\r\n", szTime, pLogString );

	SetWarningInfo( i32WarningType, i32WarningLevel);

	if( LogBuffer._szMessage[0] != '[' )
	{
		return;
	}

	if( RINGBUFFER_SUCCESS != pRingBuffer->Push( (void*)&LogBuffer) )
	{
		//Error 
		TRACE("RingBuffer Push Error\n");
	}
}

UINT32	CModuleLogFile::GetWarningInfo( INT32 i32WarningType )
{
	if( 0 > i32WarningType )						return 0;
	if( SIA_WARNING_TYPE_COUNT - 1 < i32WarningType )	return 0;

	UINT32 ui32WarningInfo = 0;

	for (INT32 i = 0; i < SIA_WARNING_LEVEL_COUNT; i++)
	{
		if ( m_ui32WriteTime[i32WarningType][i] > m_ui32ReadTime[i32WarningType][i] )
		{
			ui32WarningInfo = ui32WarningInfo | GetFlagWarningLevel( i + 1);
			m_ui32ReadTime[i32WarningType][i] = i3ThreadTimer::GetServerTime();
		}
	}

	return ui32WarningInfo;
}

BOOL	CModuleLogFile::SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel )
{
	if( 0 > i32WarningType )						return FALSE;
	if( SIA_WARNING_TYPE_COUNT - 1 < i32WarningType )	return FALSE;
	if( SIA_WARNING_LEVEL_NO > i32WarningLevel )		return FALSE;
	if( SIA_WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return FALSE;

	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();

	m_ui32WriteTime[i32WarningType][i32WarningLevel - 1] = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.

	return TRUE;
}

UINT32	CModuleLogFile::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( SIA_WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( SIA_WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}