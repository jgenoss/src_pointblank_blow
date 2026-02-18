#include "pch.h"
#include "LogFile.h"
#include "Config.h"

I3_CLASS_INSTANCE(CLogFile, i3ElementBase); 

CLogFile * g_pLogFile = NULL; 

CLogFile::CLogFile(void)
{
	m_hFile					= INVALID_HANDLE_VALUE; 

	m_pLogBufferMain		= NULL;
	m_pLogBufferControl		= NULL;
	m_ppLogBufferRelay		= NULL;

	m_ui32RelayCount		= 0;

	m_i32Day				= 0;
}

CLogFile::~CLogFile(void)
{
	OnDestroy(); 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::OnCreate( )
{
	m_pLogBufferMain	= (ST_LOGBUFFER*)i3MemAlloc( sizeof(ST_LOGBUFFER) );
	m_pLogBufferControl	= (ST_LOGBUFFER*)i3MemAlloc( sizeof(ST_LOGBUFFER) );

	if( FALSE == FileOpen() )	return FALSE;
	
	return TRUE;
}

BOOL CLogFile::OnServerCreate( UINT32 i32LogCount )
{
	m_ppLogBufferRelay = (ST_LOGBUFFER**)i3MemAlloc( sizeof(ST_LOGBUFFER*) * i32LogCount );
	for( UINT32 i = 0 ; i < i32LogCount ; i++ )
	{
		m_ppLogBufferRelay[i] = (ST_LOGBUFFER*)i3MemAlloc( sizeof(ST_LOGBUFFER) );
	}

	m_ui32RelayCount = i32LogCount;	// 대입은 꼭 메모리 생성 마지막에서 해야합니다. 안그러면 크래쉬합니다. -- 김현우

	return TRUE;
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pLogBufferMain );
	I3MEM_SAFE_FREE( m_pLogBufferControl );
	if( m_ppLogBufferRelay )
	{
		for( UINT32 i = 0 ; i < m_ui32RelayCount ; i++ )
		{
			I3MEM_SAFE_FREE( m_ppLogBufferRelay[i] );
		}
		I3MEM_SAFE_FREE( m_ppLogBufferRelay );
	}

	if(m_hFile == INVALID_HANDLE_VALUE)
	{	
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 		
	}
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::FileOpen( )
{	
	char FileName[128]; 

	UINT32 ui32Date = (g_pConfig->m_Year * 10000) + ( g_pConfig->m_Month * 100) + g_pConfig->m_Day; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
	
	//Create New File 
	sprintf(FileName, "%s\\%s_%d.txt", g_pConfig->m_LogFilePath, "Relay", ui32Date );
#ifdef UNICODE
	WCHAR16 wchar[256]; 
	i3String::CopyASCIItoWCHAR(wchar, FileName); 
	m_hFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile(FileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif
	if(m_hFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return FALSE; 
	}
	
	m_i32Day	= g_pConfig->m_Day;
	SetFilePointer( m_hFile, 0, NULL, FILE_END);
	
	char szMessage[MAX_STRING_COUNT];
	sprintf( szMessage, "-- COMPILE DATE : Ver:%d/%d, BuildDate:%s %s --\r\n", GAME_VER_MONTH, GAME_VER_DAY, __DATE__, __TIME__ );
	WRITE_LOG_MAIN( szMessage );

	return TRUE; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::OnUpdate(void)
{
	if( g_pConfig->m_Day != m_i32Day )
	{	// 날짜가 바뀌면 로그파일 생성
		FileOpen();
	}

	if( INVALID_HANDLE_VALUE == m_hFile )
	{
		FileOpen();
		return;
	}

	ST_LOGBUFFER*	pLog;
	INT32			i32Pos;
	INT32			i32ReadPos;
	// Main
	pLog = m_pLogBufferMain;
	i32Pos = pLog->_i32WritePos - pLog->_i32ReadPos;
	for( INT32 i = 0 ; i < i32Pos ; i++ )
	{
		i32ReadPos = pLog->_i32ReadPos % LOGBUFFERCOUNT;
        
		INT32 Len = i3String::Length( pLog->_szLog[ i32ReadPos ] ); 
		::WriteFile( m_hFile, pLog->_szLog[ i32ReadPos ], Len, (LPDWORD)&Len, NULL );
		pLog->_i32ReadPos++;
	}

	// Control
	pLog = m_pLogBufferControl;
	i32Pos = pLog->_i32WritePos - pLog->_i32ReadPos;
	for( INT32 i = 0 ; i < i32Pos ; i++ )
	{
		i32ReadPos = pLog->_i32ReadPos % LOGBUFFERCOUNT;
        
		INT32 Len = i3String::Length( pLog->_szLog[ i32ReadPos ] ); 
		::WriteFile( m_hFile, pLog->_szLog[ i32ReadPos ], Len, (LPDWORD)&Len, NULL );
		pLog->_i32ReadPos++;
	}

	// Relay
	for( UINT32 i = 0 ; i < m_ui32RelayCount ; i++ )
	{
		pLog = m_ppLogBufferRelay[i];
		i32Pos = pLog->_i32WritePos - pLog->_i32ReadPos;
		for( INT32 j = 0 ; j < i32Pos ; j++ )
		{
			i32ReadPos = pLog->_i32ReadPos % LOGBUFFERCOUNT;
		    
			INT32 Len = i3String::Length( pLog->_szLog[ i32ReadPos ] ); 
			::WriteFile( m_hFile, pLog->_szLog[ i32ReadPos ], Len, (LPDWORD)&Len, NULL );
			pLog->_i32ReadPos++;
		}
	}
	
	return; 
}

void CLogFile::WriteLog( INT32 i32Thread, char* pLogString)
{
	ST_LOGBUFFER* pstLog;
	char szTime[32];
	switch( i32Thread ) 
	{
	case LOGTHREADIDX_MAIN:
		pstLog = m_pLogBufferMain;
		sprintf( szTime, "[%s|[%02d:%02d]]", "MAI", g_pConfig->m_Hour, g_pConfig->m_Minute);
		break;
	case LOGTHREADIDX_CON:
		pstLog = m_pLogBufferControl;
		sprintf( szTime, "[%s|[%02d:%02d]]", "CON", g_pConfig->m_Hour, g_pConfig->m_Minute);
		break;
	default:
		pstLog = m_ppLogBufferRelay[ i32Thread - LOGTHREADIDX_RELAY ];
		sprintf( szTime, "[%s%02d|[%02d:%02d]]", "R", i32Thread, g_pConfig->m_Hour, g_pConfig->m_Minute);
		break;
	}

	if( pstLog->_i32WritePos == pstLog->_i32ReadPos )
	{
		pstLog->_i32WritePos	= 0;
		pstLog->_i32ReadPos		= 0;
	}
	if( pstLog->_i32ReadPos > 2000000000 )
	{
		//WRITE_LOG(NULL,"Relay Buffer FULL");
		return ; 
	}

	INT32 WriteIdx = pstLog->_i32WritePos % LOGBUFFERCOUNT; 
	if(WriteIdx >= LOGBUFFERCOUNT)
	{
		//WRITE_LOG(NULL,"Relay Buffer Overflow");
		return ; 
	}

	sprintf( pstLog->_szLog[ WriteIdx ], "%s%s\r\n", szTime, pLogString);
	pstLog->_i32WritePos++;
}