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

I3_CLASS_INSTANCE(CLogFile, i3ElementBase); 

// global variables
CLogFile * g_pLogFile = NULL; 
extern BOOL bWriteSendError;

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
	m_pBuffer		= NULL;
	m_i32Hour		= 0;
	m_ui32SerialNo	= 0;
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
BOOL CLogFile::OnCreate( )
{
	m_pBuffer = new DS_LOG_QUEUE;
	I3ASSERT( m_pBuffer );

	if( ! FileOpen() )
	{
		return FALSE;
	}
	
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
	//bWriteSendError = FALSE;

	char	szFileName[MAX_PATH] = { '\0' }; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
	
	//Create New File 
	sprintf_s( szFileName, sizeof(szFileName), "%s\\%s[%d_%d]_%d.txt", g_pConfig->m_szLogFilePath, "Physics", g_pConfig->m_Month, g_pConfig->m_Day, g_pConfig->m_Hour );
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
	
	m_i32Hour = g_pConfig->m_Hour;
	SetFilePointer( m_hFile, 0, NULL, FILE_END);
	
	// Write First Line
	m_ui32SerialNo = 0;		// serial no
	char szMessage[512] = { '\0', };
	UINT32 ui32MsgLen = sprintf_s( szMessage, sizeof(szMessage), "-- COMPILE DATE : Ver:%d/%d, BuildDate:%s %s, --\r\n", GAME_VER_MONTH, GAME_VER_DAY, __DATE__, __TIME__ );

	WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMessage, ui32MsgLen);

	
	// 누적된 통계 찍기
	{
		INT64	i64RequestsPerHour	= 0; 
		INT64	i64ErrorsPerHour	= 0; 
		INT64	i64WallShotsPerHour	= 0;

		g_pStatistics->GetStaticsticsInfo(	&i64RequestsPerHour, 
											&i64ErrorsPerHour, 
											&i64WallShotsPerHour );

		ui32MsgLen = sprintf_s( szMessage,  sizeof(szMessage), 
											"[STATISTICS]\r\n"
											"\t* Raycast Requests Per Hour:\t %I64d\r\n"
											"\t* Error Packets Per Hour:\t %I64d\r\n" 
											"\t* Wall Shot Users Per Hour:\t %I64d\r\n",
			
											i64RequestsPerHour, 
											i64ErrorsPerHour, 
											i64WallShotsPerHour); 

		WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMessage, ui32MsgLen);

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
	I3ASSERT( m_hFile != INVALID_HANDLE_VALUE );
	
	m_cs.Lock();
	if( g_pConfig->m_Hour != m_i32Hour )
	{	
		FileOpen();
	}
	m_cs.Unlock();
	
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
	
	INT32&			i32ReadIdx  = m_pBuffer->m_i32ReadIdx; 
	INT32&			i32writeIdx = m_pBuffer->m_i32WriteIdx;
	UINT32&			i32Count	= m_pBuffer->m_ui32Count;

	while ( i32ReadIdx != i32writeIdx )
	{
		pLogItem = & m_pBuffer->m_logList[i32ReadIdx];
		
		if (g_pConfig->m_ui32LogLevel <= pLogItem->m_ui16Level)
		{
			if( FALSE == WriteFile( m_hFile, pLogItem->m_szMsg, pLogItem->m_ui16MsgLen, &dwWritten, NULL) ) 
				break;
		}
		
		i32ReadIdx = (i32ReadIdx + 1) % DS_LOG_BUFFER_COUNT;
		if( --i32Count < 0) i32Count = 0;
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
	UINT32&			ui32Count	= m_pBuffer->m_ui32Count;

	DS_LOG_ITEM*	pLogItem;	// 로그 아이템
	
	// setup log item
	pLogItem	 = & m_pBuffer->m_logList[ i32WriteIdx ];
	memset( pLogItem, 0, sizeof(DS_LOG_ITEM));

	GetLocalTime( &pLogItem->m_time );
	
	pLogItem->m_ui32SerialNo  = ++m_ui32SerialNo;
	pLogItem->m_ui16Level	= (UINT16)ui32Level;
	pLogItem->m_ui16MsgID	= 0;	// reserved
	pLogItem->m_ui16LangID	= 0;	// reserved
	pLogItem->m_ui16MsgLen	= (UINT16) sprintf_s( pLogItem->m_szMsg, sizeof( pLogItem->m_szMsg ), 
											"%06d,%d,%02d:%02d:%02d,%s", 
											m_ui32SerialNo, ui32Level, 
											pLogItem->m_time.wHour, pLogItem->m_time.wMinute, pLogItem->m_time.wSecond,
											szMsg );
	
	if( pLogItem->m_ui16MsgLen <= 0 )
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
	
	ui32Count++;
	
	if( (i32WriteIdx + 1) % DS_LOG_BUFFER_COUNT == i32ReadIdx )
	{
		Flush();
	}

	i32WriteIdx = (i32WriteIdx + 1) % DS_LOG_BUFFER_COUNT;
	I3TRACE(szMsg);
	
	m_cs.Unlock();

	return pLogItem->m_ui16MsgLen;
}

// 리턴된 문자열은 반드시 LocalFree() 시켜야 한다.
LPSTR CLogFile::GetErrorString(DWORD dwErrorCode)
{
    LPSTR pMessage = NULL;
    
    FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_MAX_WIDTH_MASK,
					NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPSTR)&pMessage, 1024, NULL);
    return pMessage;
}