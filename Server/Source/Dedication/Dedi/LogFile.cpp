// LogFile.cpp
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy:mm:dd)
//	
// Description:
//	LogFile의 구현파일
//
//
// Dependency: 
//	#include "config.h"
//	
#include "pch.h"
#include "S2CriticalSection.h"
#include "ServerStatistics.h"
#include "LogFile.h"
#include "GroupMaker.h"



// global variables
CLogFile * g_pLogFile = NULL; 
CLogFile * g_pHackLogFile = NULL; 
extern BOOL		g_bWriteSendError;
extern UINT32	g_ui32PacketError;

DWORD WINAPI LogFileUpdate(LPVOID lpParameter)
{
	g_pLogFile->OnUpdate();
	return 0;
}

#ifdef NEW_LOG

#define LOG_MAX_SEND_IDX_COUNT				2000000000		
#define LOG_MAX_ZERO_COUNT					1900000000

#define LOG_CHECKVALID_IDX		if( m_hFile == INVALID_HANDLE_VALUE )			return FALSE; \
	DWORD dwWritten = 0;\
	if(pBuffer->m_i32WriteIdx > LOG_MAX_ZERO_COUNT)\
{\
	if( pBuffer->m_i32WriteIdx == pBuffer->m_i32ReadIdx )\
{\
	pBuffer->m_i32WriteIdx	= 0;\
	pBuffer->m_i32ReadIdx	= 0;\
}\
}\
	if( pBuffer->m_i32ReadIdx > LOG_MAX_SEND_IDX_COUNT )\
{\
	char* szMsg = "Log WriteBuffer FULL\r\n";\
	WriteFile( m_hFile, szMsg, strlen(szMsg), &dwWritten, NULL );\
	return FALSE; \
}\
	if( (pBuffer->m_i32WriteIdx - pBuffer->m_i32ReadIdx) >= (DS_LOG_BUFFER_COUNT -1) )\
{\
	char* szMsg = "Log WriteBuffer Overflow\r\n";\
	WriteFile( m_hFile, szMsg, strlen(szMsg), &dwWritten, NULL );\
	return FALSE; \
}\
INT32 i32WriteIdx = pBuffer->m_i32WriteIdx % DS_LOG_BUFFER_COUNT; \


///////////////////////////////////////////////////////////////////////////////
//
// Function: CLogFile
//
// Description:
//	- constructor
//		- 
// Arguments:
// Return:
//
CLogFile::CLogFile(void)
{
	m_ppBuffers			= NULL;
	m_ui32BufferCount	= 0;
	
	m_hFileThread		= NULL;
	m_hFile				= INVALID_HANDLE_VALUE; 
	
	m_i32Hour			= 0;

	m_ui8LogType =LOG;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CLogFile
//
// Description:
//	- destructor
//
// Arguments:
// Return:
//
CLogFile::~CLogFile(void)
{
	OnDestroy(); 
}

//선행조건: m_ppBuffers setup
DS_LOG_MESSAGE_BUFFER* CLogFile::AllocBuffer( DWORD dwThreadID )
{
	I3ASSERT( m_ppBuffers != NULL );
	I3ASSERT( m_ui32BufferCount  < DS_LOG_USER_THREAD_MAX );

	I3TRACE( "[CLogFile::AllocBuffer] ThreadID = %d, TotalCount = %d\n", dwThreadID, m_ui32BufferCount+1 );
	
	CThreadSync cs(m_cs); //동시 접근 가능

	if( m_ui32BufferCount < DS_LOG_USER_THREAD_MAX )
	{
		m_ppBuffers[ m_ui32BufferCount ] = new DS_LOG_MESSAGE_BUFFER;
		if( NULL == m_ppBuffers[ m_ui32BufferCount ] ) return NULL;
		
		m_ppBuffers[ m_ui32BufferCount ]->m_dwThreadID = dwThreadID; 
		
		return m_ppBuffers[ m_ui32BufferCount++ ];
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnCreate
//
// Description:
//	- g_InitServer()에서 호출 됨. 메모리 할당.
//		- 
// Arguments:
// Return:
//	- TRUE/FALSE
//
BOOL CLogFile::OnCreate( LOG_TYPE eType )
{
	DWORD dwWritten = 0;
	char* szMsg = NULL;

	m_ui8LogType = (UINT8)eType;
	
	//File open 
	if( ! FileOpen() )
	{
		return FALSE;
	}
	
	//Alloc pointer array
	m_ppBuffers = (DS_LOG_MESSAGE_BUFFER**) new DS_LOG_MESSAGE_BUFFER*[DS_LOG_USER_THREAD_MAX];
	if( NULL == m_ppBuffers )
	{
		szMsg = "[CLogFile::OnCreate] alloc buffer_pointer_array failed\r\n";
		WriteFile( m_hFile, szMsg, strlen(szMsg), &dwWritten, NULL );
		return FALSE;
	}
	i3mem::FillZero( m_ppBuffers, sizeof(DS_LOG_MESSAGE_BUFFER*) * DS_LOG_USER_THREAD_MAX );
	
	//Alloc buffer for main thread
	I3TRACE("MainThreadID=%d\n", GetCurrentThreadId() );
	if( NULL == AllocBuffer( GetCurrentThreadId()))
	{
		szMsg = "[CLogFile::OnCreate] alloc buffer_pointer_array failed\r\n";
		WriteFile( m_hFile, szMsg, strlen(szMsg), &dwWritten, NULL );
		return FALSE;
	}
	
	//CreateThread
	m_hFileThread = CreateThread( NULL, 0, LogFileUpdate, NULL, 0, NULL);
	if( NULL == m_hFileThread )
    {
		szMsg = "[CLogFile::OnCreate] CreateThread failed\r\n";
		WriteFile( m_hFile, szMsg, strlen(szMsg), &dwWritten, NULL );
		return FALSE;
    }
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnDestroy
//
// Description:
//	- g_DestroyServer()에서 호출 됨. 메모리 정리
//
// Arguments:
// Return:
//	
void CLogFile::OnDestroy(void)
{
	//check quit signal
	if( WaitForSingleObject( g_hevtQuit, 100) != WAIT_OBJECT_0 )
	{
		I3TRACE("[CIocpServer::~CIocpServer] g_hevtQuit is not signal state!\n");
	}
	
	//check update thread
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hFileThread, 1000 ) )
	{
		DWORD ui32ExitCode = 0;
		GetExitCodeThread( m_hFileThread, &ui32ExitCode);
		if( ui32ExitCode == STILL_ACTIVE )
		{
			TerminateThread( m_hFileThread, (DWORD)-1);
		}
	}

	m_hFileThread = INVALID_HANDLE_VALUE;
	
	//free buffers
	for( UINT32 i = 0; i < m_ui32BufferCount; i++ )
	{
		SAFE_DELETE( m_ppBuffers[i] );
	}
	
	//free pointer array
	SAFE_DELETE( m_ppBuffers );

	// close file
	if(m_hFile != INVALID_HANDLE_VALUE)
	{	
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 		
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: FileOpen
//
// Description:
//	- 파일 Open 함수. Create/Update에서 호출 됨.(legacy)
//	- Thread safe 하지 않음.	
// Arguments:
// Return:
//	- TRUE/FALSE
//
BOOL CLogFile::FileOpen( )
{	
	g_bWriteSendError = FALSE;

	char	strFileName[MAX_PATH] = { '\0' };

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
	
	//Create New File 
	//Create New File 

	if (m_ui8LogType == LOG)
		i3String::Format( strFileName, sizeof(strFileName), "%s\\Dedication_%d_%02d_%02d_%02d.txt", g_pConfig->m_strLogFilePath, g_pConfig->m_i32Year, g_pConfig->m_i32Month, g_pConfig->m_i32Day, g_pConfig->m_i32Hour );
	else
		i3String::Format( strFileName, sizeof(strFileName), "%s\\Hack_%d_%02d_%02d_%02d.txt", g_pConfig->m_strLogFilePath, g_pConfig->m_i32Year, g_pConfig->m_i32Month, g_pConfig->m_i32Day, g_pConfig->m_i32Hour );

#ifdef UNICODE
	WCHAR16 wchar[MAX_PATH]; 
	i3String::CopyASCIItoWCHAR( wchar, strFileName);
	m_hFile = ::CreateFile( wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile( strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif
	if( m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return FALSE; 
	}
	
	m_i32Hour = g_pConfig->m_i32Hour;
	SetFilePointer( m_hFile, 0, NULL, FILE_END);
	
	// Write First Line
	char strMessage[512] = { '\0', };
	UINT32 ui32MsgLen = sprintf_s( strMessage, sizeof(strMessage), "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, Protocol:%d.%d --\r\n", VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL, __DATE__, __TIME__, VER_UDP_PROTOCOL_MAIN, VER_UDP_PROTOCOL_SUB );
	DWORD dwWritten = 0;
	WriteFile( m_hFile, strMessage, ui32MsgLen, &dwWritten, NULL);
	
	// 누적된 통계 찍기
	{
#ifdef _DEBUG
		INT64 i64SentBytesPerHour					= 0; 
		INT64 i64ReceivedBytesPerHour				= 0; 
		INT32 i32AverageSentBytesPerSeconds			= 0;
		INT32 i32AverageReceivedBytesPerSeconds		= 0;

		INT64 i64SentPacketsPerHour					= 0; 
		INT64 i64ReceivedPacketsPerHour				= 0; 
		INT32 i32AverageSentPacketsPerSeconds		= 0;
		INT32 i32AverageReceivedPacketsPerSeconds	= 0; 

		UINT32 ui32MaxSentPacketSize				= 0;
		UINT32 ui32MaxReceivedPacketSize			= 0;

		g_pStatistics->GetStaticsticsInfo(	&i64SentBytesPerHour, 
											&i64ReceivedBytesPerHour, 
											&i32AverageSentBytesPerSeconds,
											&i32AverageReceivedBytesPerSeconds,

											&i64SentPacketsPerHour, 
											&i64ReceivedPacketsPerHour, 
											&i32AverageSentPacketsPerSeconds,
											&i32AverageReceivedPacketsPerSeconds,
											
											&ui32MaxSentPacketSize,
											&ui32MaxReceivedPacketSize
										);

		ui32MsgLen = sprintf_s( strMessage,  sizeof(strMessage), 
											"[STATISTICS]\r\n"
											"\t* [Bytes]\r\n"
											"\t* Sent Bytes Per Hour:\t %I64d\r\n"
											"\t* Received Bytes Per Hour:\t %I64d\r\n" 
											"\t* Average Sent Bytes Per Seconds:\t %I32d\r\n"
											"\t* Average Received Bytes Per Seconds:\t %I32d\r\n"
											"\t* [Packets] \r\n"
											"\t* Sent Packets Per Hour:\t %I64d\r\n" 
											"\t* Received Packets Per Hour:\t %I64d\r\n" 
											"\t* Average Sent Packets Per Seconds:\t %I32d\r\n"
											"\t* Average Received Packets Per Seconds:\t %I32d\r\n"
											"\t* [Max]\r\n"
											"\t* Max Sent Packet Size In An Hour:\t %I32d\r\n"
											"\t* Max Received Packet Size In An Hour:\t %I32d\r\n"
											,
											i64SentBytesPerHour, 
											i64ReceivedBytesPerHour, 
											i32AverageSentBytesPerSeconds,
											i32AverageReceivedBytesPerSeconds,

											i64SentPacketsPerHour, 
											i64ReceivedPacketsPerHour, 
											i32AverageSentPacketsPerSeconds,
											i32AverageReceivedPacketsPerSeconds,
											
											ui32MaxSentPacketSize,
											ui32MaxReceivedPacketSize
										); 

		WriteFile( m_hFile, strMessage, ui32MsgLen, &dwWritten, NULL);
				
		// 패킷 에러 추가
		ui32MsgLen = sprintf_s( strMessage, sizeof(strMessage), "** Packet Error Per Hour: %u\n", g_ui32PacketError);
		WriteFile( m_hFile, strMessage, ui32MsgLen, &dwWritten, NULL);
		g_ui32PacketError = 0;
#endif 
	}
	
	return TRUE; 
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnUpdate
//
// Description:
//	- g_UpdateServer()에서 호출 됨. Fush 작업.
//
// Arguments:
// Return:
//	- 
void CLogFile::OnUpdate(void)
{
	while( 1 )
	{
		Flush();
		if( WAIT_OBJECT_0 == WaitForSingleObject( g_hevtQuit, 100 ) )	return;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Flush
//
// Description:
//	- OnUpdate()에서 호출 됨. 
//		
// Arguments:
// Return:
//
void CLogFile::Flush()
{
	I3ASSERT(m_hFile !=	INVALID_HANDLE_VALUE);
	I3ASSERT( m_ppBuffers != NULL );

	DS_LOG_MESSAGE_BUFFER*	pBuffer		= NULL;
	DS_LOG_ITEM*			pLogItem	= NULL;	
	DWORD					dwWritten	= 0;
	INT32					i32ReadIdx	= 0;
	INT32					i32Count	= 0;

	if( g_pConfig->m_i32Hour != m_i32Hour )
	{	
		FileOpen();
	}
	
	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		for( UINT32 i = 0; i < m_ui32BufferCount; i++ )
		{
			pBuffer   = m_ppBuffers[ i ]; 
			i32Count  = pBuffer->m_i32WriteIdx - pBuffer->m_i32ReadIdx; 
			
			for ( INT32 j = 0; j < i32Count; j++ )
			{
				i32ReadIdx  = pBuffer->m_i32ReadIdx % DS_LOG_BUFFER_COUNT; 
				pLogItem	= &pBuffer->m_alogList[ i32ReadIdx ];

				if ( g_pConfig->m_ui32LogLevel <= pLogItem->m_ui8Level)
				{
					WriteFile( m_hFile, pLogItem->m_strMsg, pLogItem->m_ui16MsgLen, &dwWritten, NULL);
				}
				
				pBuffer->m_i32ReadIdx++; 
			} //for
		}// for
	} // handle
}

DS_LOG_MESSAGE_BUFFER*	CLogFile::FindLocalBuffer()
{
	for( UINT32 i = 0; i < m_ui32BufferCount; i++ )
	{
		if( m_ppBuffers[i]->m_dwThreadID == GetCurrentThreadId() )
		{
			return m_ppBuffers[i];
		}
	}

	I3ASSERT(0);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: WriteLog
//
// Description:
//	-  g_pLogFile 포인터를 이용하여 호출함. 
//		- (참고)출력 형식을 기존 코드에 맞추기 위해서 
//		- ST_LOGITEM.szMsg에 출력하고자 하는 모른 정보를 채운다.
//		
// Arguments:
//	- ui32Level : error level (defined in logFile.h)
//		#define DS_ERROR_LEVEL_MSG			0
//		#define DS_ERROR_LEVEL_WARNING		1
//		#define DS_ERROR_LEVEL_ERROR		2
//
//	- szMsg			: 문자열 메시지
//	- ui32MsgLen	: 문자열 메시지 길이
// Return:
//	- 타임 태그가 붙은 최종 문자열 길이
//
BOOL CLogFile::WriteLog( UINT32 ui32Level, char* pstrMsg, UINT32 ui32MsgLen )
{
	I3ASSERT( pstrMsg );
	I3ASSERT( ui32MsgLen < 1000 && ui32MsgLen != 0 );
	
	DS_LOG_MESSAGE_BUFFER* pBuffer = FindLocalBuffer();
	if( pBuffer == NULL ) return FALSE;

	LOG_CHECKVALID_IDX;
	
	// time stamp
	SYSTEMTIME localTime;
	GetLocalTime( &localTime );
	
	//get log item
	DS_LOG_ITEM*	pLogItem = &pBuffer->m_alogList[ i32WriteIdx ];	// 로그 아이템
	
	pLogItem->Reset();
	pLogItem->m_ui8Level	= (UINT8)ui32Level;
	pLogItem->m_ui8Minute	= (UINT8)localTime.wMinute;
	pLogItem->m_ui8Second	= (UINT8)localTime.wSecond;
	pLogItem->m_ui16MsgLen	= (UINT16)sprintf_s( pLogItem->m_strMsg, sizeof( pLogItem->m_strMsg ), 
											"%02d:%02d,%s", pLogItem->m_ui8Minute, pLogItem->m_ui8Second,	pstrMsg );

	//i3String::Format() error
	if( pLogItem->m_ui16MsgLen == 0 || pLogItem->m_ui16MsgLen == sizeof( pLogItem->m_strMsg ) )
	{
		pBuffer->m_i32WriteIdx++; //버림
		return FALSE;
	}
	
	//convert CR->CR/LF
	if ( pLogItem->m_strMsg[ pLogItem->m_ui16MsgLen - 1] == 0x0a && 
		 pLogItem->m_ui16MsgLen + 1 < sizeof( pLogItem->m_strMsg ) ) 
	{
		pLogItem->m_strMsg[ pLogItem->m_ui16MsgLen-1]	= 0x0d;
		pLogItem->m_strMsg[ pLogItem->m_ui16MsgLen]		= 0x0a;
		pLogItem->m_strMsg[ pLogItem->m_ui16MsgLen+1]	= '\0';
		pLogItem->m_ui16MsgLen++;
	}
	
	pBuffer->m_i32WriteIdx++;
	return TRUE;
}


#else


///////////////////////////////////////////////////////////////////////////////
//
// Function: CLogFile
//
// Description:
//	- constructor
//		- 
// Arguments:
// Return:
//
CLogFile::CLogFile(void)
{
	m_hFile			= INVALID_HANDLE_VALUE; 
	m_hFileThread	= NULL;
	m_pBuffer		= NULL;
	m_i32Hour		= 0;
	m_ui32SerialNo	= 0;
	m_ui8LogType    = LOG;
	m_i32Average_Min = 0;
	m_i32AverageCCU_Min = 0;
	m_i32AverageRoom_Min = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CLogFile
//
// Description:
//	- destructor
//
// Arguments:
// Return:
//
CLogFile::~CLogFile(void)
{
	OnDestroy(); 
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnCreate
//
// Description:
//	- g_InitServer()에서 호출 됨. 메모리 할당.
//		- 
// Arguments:
// Return:
//	- TRUE/FALSE
//
BOOL CLogFile::OnCreate( LOG_TYPE eType )
{
	m_ui8LogType = (UINT8)eType;
	m_pBuffer = new DS_LOG_QUEUE;
	I3ASSERT( m_pBuffer );

	if( ! FileOpen() )
	{
		return FALSE;
	}
	/*
	m_hFileThread = CreateThread( NULL, 0, LogFileUpdate, NULL, 0, NULL);
	if (m_hFileThread == NULL)
    {
		DSERROR("[CLogFile::OnCreate( )] CreateThread failed\n");
        return FALSE;
    }
	*/

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnServerCreate
//
// Description:
//	- g_StartServer()에서 호출 됨. dummy.
//		
// Arguments:
//	- UNREFERENCED_PARAMETER(i32LogCount);
//		
// Return:
//	- TRUE
//
BOOL CLogFile::OnServerCreate( UINT32 i32LogCount )
{
	UNREFERENCED_PARAMETER( i32LogCount );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnDestroy
//
// Description:
//	- g_DestroyServer()에서 호출 됨. 메모리 정리
//
// Arguments:
// Return:
//	
void CLogFile::OnDestroy(void)
{
/*
	//check quit signal
	if( WaitForSingleObject( g_hevtQuit, 100) != WAIT_OBJECT_0 )
	{
		I3TRACE("[CIocpServer::~CIocpServer] g_hevtQuit is not signal state!\n");
	}
	
	//check update thread
	if( WAIT_OBJECT_0 != WaitForSingleObject( m_hFileThread, 1000 ) )
	{
		DWORD ui32ExitCode = 0;
		GetExitCodeThread( m_hFileThread, &ui32ExitCode);
		if( ui32ExitCode == STILL_ACTIVE )
		{
			TerminateThread( m_hFileThread, (DWORD)-1);
		}
	}

	m_hFileThread = NULL;
	*/

	// flush buffer
	if( m_pBuffer )
	{
		Flush();
		I3_SAFE_DELETE( m_pBuffer );
	}

	// close file
	if(m_hFile != INVALID_HANDLE_VALUE)
	{	
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 		
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: FileOpen
//
// Description:
//	- 파일 Open 함수. Create/Update에서 호출 됨.(legacy)
//	- Thread safe 하지 않음.	
// Arguments:
// Return:
//	- TRUE/FALSE
//
BOOL CLogFile::FileOpen( )
{	
	g_bWriteSendError = FALSE;

	char	szFileName[MAX_PATH] = { '\0' }; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
	
	//Create New File 
	if (m_ui8LogType == LOG)
	{
		i3String::Format( szFileName, sizeof(szFileName), "%s\\Dedication_%04d_%02d_%02d_%02d.txt",
			g_pConfig->m_strLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
	}
	else
	{
		i3String::Format( szFileName, sizeof(szFileName), "%s\\Hack_%04d_%02d_%02d_%02d.txt",
			g_pConfig->m_strLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
	}

#ifdef UNICODE
	WCHAR16 wchar[MAX_PATH]; 
	i3String::CopyASCIItoWCHAR( wchar, szFileName); 
	m_hFile = ::CreateFile( wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif
	if( m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return FALSE; 
	}
	
	m_i32Hour = i3ThreadTimer::LocalTime().GetHour();
	SetFilePointer( m_hFile, 0, NULL, FILE_END);
	
	// Write First Line
	m_ui32SerialNo = 0;		// serial no
	char szMessage[1024] = { '\0', };
	UINT32 ui32MsgLen = sprintf_s( szMessage, sizeof(szMessage), "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, Protocol:%d.%d , StartDate:%s --\r\n", VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL, __DATE__, __TIME__, VER_UDP_PROTOCOL_MAIN, VER_UDP_PROTOCOL_SUB, g_pConfig->GetServerStartTime() );
	DWORD dwWritten = 0;
	WriteFile( m_hFile, szMessage, ui32MsgLen, &dwWritten, NULL);

	m_i32Average_Min = MAX(m_i32Average_Min, 1);
	
	// 누적된 통계 찍기
	{
		INT64 i64SentBytesPerHour					= 0; 
		INT64 i64ReceivedBytesPerHour				= 0; 
		INT32 i32AverageSentBytesPerSeconds			= 0;
		INT32 i32AverageReceivedBytesPerSeconds		= 0;

		INT64 i64SentPacketsPerHour					= 0; 
		INT64 i64ReceivedPacketsPerHour				= 0; 
		INT32 i32AverageSentPacketsPerSeconds		= 0;
		INT32 i32AverageReceivedPacketsPerSeconds	= 0; 

		UINT32 ui32MaxSentPacketSize				= 0;
		UINT32 ui32MaxReceivedPacketSize			= 0;

		UINT64 ui64PacketCountPerHour				= 0;
		REAL64 r64PacketTimePerHour					= 0.f;

		g_pStatistics->GetStaticsticsInfo(	&i64SentBytesPerHour, 
											&i64ReceivedBytesPerHour, 
											&i32AverageSentBytesPerSeconds,
											&i32AverageReceivedBytesPerSeconds,

											&i64SentPacketsPerHour, 
											&i64ReceivedPacketsPerHour, 
											&i32AverageSentPacketsPerSeconds,
											&i32AverageReceivedPacketsPerSeconds,
											
											&ui32MaxSentPacketSize,
											&ui32MaxReceivedPacketSize,
											&ui64PacketCountPerHour,
											&r64PacketTimePerHour
										);

		ui32MsgLen = sprintf_s( szMessage,  sizeof(szMessage), 
											"[STATISTICS]\r\n"
											"========== COPY DOWN =========== \r\n"
											"{\"DediUDP\": \r\n" 
											"{\"Date\":\"%04d-%02d-%02d\",\r\n"
											"\"AverageUserCount\":\"%d\",\r\n"
											"\"AverageRoomCount\":\"%d\",\r\n" 
											"\"S_Count\":\"%I64d\",\r\n"	
											"\"S_Size\":\"%I64d\",\r\n"
											"\"R_Count\":\"%I64d\",\r\n"
											"\"R_Size\":\"%I64d\"\r\n"
											"}}\r\n" 
											"========== COPY UP =========== \r\n" 
											"\t* Average Sent Bytes Per Seconds:\t %I32d\r\n"
											"\t* Average Received Bytes Per Seconds:\t %I32d\r\n" 
											"\t* Average Sent Packets Per Seconds:\t %I32d\r\n"
											"\t* Average Received Packets Per Seconds:\t %I32d\r\n" 
											"\t* Max Sent Packet Size In An Hour:\t %I32d\r\n"
											"\t* Max Received Packet Size In An Hour:\t %I32d\r\n"
											"\t* Recv Packet Count In An Hour:\t %I64d\r\n"
											"\t* Recv Packet Process Time In An Hour(Sec):\t %.6lf\r\n"
											,
											i3ThreadTimer::LocalTime().GetYear(),
											i3ThreadTimer::LocalTime().GetMonth(),
											i3ThreadTimer::LocalTime().GetDay(),
											m_i32AverageCCU_Min / m_i32Average_Min,
											m_i32AverageRoom_Min / m_i32Average_Min,
											i64SentPacketsPerHour,
											i64SentBytesPerHour, 
											i64ReceivedPacketsPerHour,
											i64ReceivedBytesPerHour, 

											i32AverageSentBytesPerSeconds,
											i32AverageReceivedBytesPerSeconds, 
											i32AverageSentPacketsPerSeconds,
											i32AverageReceivedPacketsPerSeconds,
											
											ui32MaxSentPacketSize,
											ui32MaxReceivedPacketSize,
											ui64PacketCountPerHour,
											r64PacketTimePerHour
										); 

		WriteLog(DS_LOG_ERROR_LEVEL_ERROR, szMessage, ui32MsgLen);

		m_i32Average_Min = 0;
		m_i32AverageCCU_Min = 0;
		m_i32AverageRoom_Min = 0;
				
		// 패킷 에러 추가
		ui32MsgLen = sprintf_s( szMessage, sizeof(szMessage), "** Packet Error Per Hour: %u\n", g_ui32PacketError);
		g_ui32PacketError = 0;
		WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMessage, ui32MsgLen );
	}
	
	return TRUE; 
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnUpdate
//
// Description:
//	- g_UpdateServer()에서 호출 됨. Fush 작업.
//
// Arguments:
// Return:
//	- 
void CLogFile::OnUpdate(void)
{
	Flush();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Flush
//
// Description:
//	- OnUpdate()에서 호출 됨. 
//		
// Arguments:
// Return:
//
void CLogFile::Flush()
{
	I3ASSERT(m_hFile !=	INVALID_HANDLE_VALUE);
	I3ASSERT(m_pBuffer);

	DS_LOG_ITEM*	pLogItem = NULL;	// 로그 아이템
	DWORD			dwWritten = 0;
	
	m_cs.Lock();

	if (i3ThreadTimer::LocalTime().GetMinute() != m_i32Average_Min)
	{
		++m_i32Average_Min;
		if (g_pRoomManager != NULL)
		{
			m_i32AverageCCU_Min += g_pRoomManager->GetAllUserCount();
			m_i32AverageRoom_Min += g_pRoomManager->GetGroupCount();
		}
	}

	if( i3ThreadTimer::LocalTime().GetHour() != m_i32Hour )
	{	
		FileOpen();
	}
	
	INT32&			i32ReadIdx  = m_pBuffer->m_i32ReadIdx; 
	INT32&			i32writeIdx = m_pBuffer->m_i32WriteIdx;
	INT32&			i32Count	= m_pBuffer->m_i32Count;

	while ( i32ReadIdx != i32writeIdx )
	{
		pLogItem = & m_pBuffer->m_logList[i32ReadIdx];
		
		if (g_pConfig->m_ui32LogLevel <= pLogItem->m_ui16Level)
		{
			if( m_hFile != INVALID_HANDLE_VALUE )
			{
				WriteFile( m_hFile, pLogItem->m_szMsg, pLogItem->m_ui16MsgLen, &dwWritten, NULL);
			}
		}
		
		i32ReadIdx = (i32ReadIdx + 1) % DS_LOG_BUFFER_COUNT;
		if( --i32Count < 0)
		{
			i32Count = 0;
			break;
		}
	}
	
	m_cs.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: WriteLog
//
// Description:
//	-  g_pLogFile 포인터를 이용하여 호출함. 
//		- (참고)출력 형식을 기존 코드에 맞추기 위해서 
//		- ST_LOGITEM.szMsg에 출력하고자 하는 모른 정보를 채운다.
//		
// Arguments:
//	- ui32Level : error level (defined in logFile.h)
//		#define DS_ERROR_LEVEL_MSG			0
//		#define DS_ERROR_LEVEL_WARNING		1
//		#define DS_ERROR_LEVEL_ERROR		2
//
//	- szMsg			: 문자열 메시지
//	- ui32MsgLen	: 문자열 메시지 길이
// Return:
//	- 타임 태그가 붙은 최종 문자열 길이
//
INT32 CLogFile::WriteLog( UINT32 ui32Level, char* szMsg, UINT32 ui32MsgLen )
{
	I3ASSERT( szMsg );
	I3ASSERT( ui32MsgLen );
	
	m_cs.Lock();
	
	INT32&			i32ReadIdx  = m_pBuffer->m_i32ReadIdx;  // write index가 선행한다
	INT32&			i32WriteIdx = m_pBuffer->m_i32WriteIdx; // ...
	INT32&			i32Count	= m_pBuffer->m_i32Count;

	DS_LOG_ITEM*	pLogItem = NULL;	// 로그 아이템
	
	// setup log item
	pLogItem	 = & m_pBuffer->m_logList[ i32WriteIdx ];
	i3mem::FillZero( pLogItem, sizeof(DS_LOG_ITEM));

	GetLocalTime( &pLogItem->m_time );
	
	pLogItem->m_ui32SerialNo  = ++m_ui32SerialNo;
	pLogItem->m_ui16Level	= (UINT16)ui32Level;
	pLogItem->m_ui16MsgLen	= (UINT16)sprintf_s( pLogItem->m_szMsg, sizeof( pLogItem->m_szMsg ), 
											"%06d,%d,%02d:%02d:%02d,%s", 
											m_ui32SerialNo, ui32Level, 
											pLogItem->m_time.wHour, pLogItem->m_time.wMinute, pLogItem->m_time.wSecond,
											szMsg );

	if( pLogItem->m_ui16MsgLen <= 0 )
	{
		m_cs.Unlock();
		return -1;
	}

	if( pLogItem->m_ui16MsgLen >= sizeof( pLogItem->m_szMsg ) )
	{
		pLogItem->m_szMsg[  sizeof( pLogItem->m_szMsg ) - 1 ] = '\0';
		pLogItem->m_ui16MsgLen =  sizeof( pLogItem->m_szMsg ) - 1;
	}

	if( pLogItem->m_ui16MsgLen >= DS_LOG_STRING_LENGTH -1 )
	{
		m_cs.Unlock();
		return -1;
	}

	if ( pLogItem->m_szMsg[ pLogItem->m_ui16MsgLen - 1] == 0x0a )
	{
		pLogItem->m_szMsg[ pLogItem->m_ui16MsgLen-1]  = 0x0d;
		pLogItem->m_szMsg[ pLogItem->m_ui16MsgLen]	  = 0x0a;
		pLogItem->m_szMsg[ pLogItem->m_ui16MsgLen+1]  = '\0';
		pLogItem->m_ui16MsgLen++;
	}
	
	i32Count++;
	
	if( (i32WriteIdx + 1) % DS_LOG_BUFFER_COUNT == i32ReadIdx )
	{
		Flush();
	}

	i32WriteIdx = (i32WriteIdx + 1) % DS_LOG_BUFFER_COUNT;
	I3TRACE(szMsg);
	
	m_cs.Unlock();

	return pLogItem->m_ui16MsgLen;
}

#endif 