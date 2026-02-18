#include "pch.h"
#include "ModuleLogFile.h"

I3_CLASS_INSTANCE(CModuleLogFile, i3Thread); 

CModuleLogFile* g_pLog = NULL; 
CModuleLogFile*	g_pThisClass = NULL;

void WriteLog( INT32 i32ThreadIdx, const char * strLog, ... )
{
	char strTemp[ LOG_STRING_BASIC ];

	va_list marker;

	va_start( marker, strLog );

	int i32Max = LOG_STRING_BASIC - 1;
	int i32Rv = _vsnprintf( strTemp, LOG_STRING_BASIC, strLog, marker );

	if ((i32Rv < 0) || (i32Rv >= i32Max))
	{
		strTemp[i32Max] = '\0';
	}

	va_end( marker);

	g_pThisClass->InsertBuffer( i32ThreadIdx, strTemp );
}

CModuleLogFile::CModuleLogFile(void)
{
	g_pThisClass			= this;

	m_bIsRunning			= FALSE;

	m_hFile					= INVALID_HANDLE_VALUE; 

	m_pLogList				= NULL;
	m_i32LogBufferCount		= 0;

	m_i32LogThreadIdx		= LOG_THREAD_INVALID;
}

CModuleLogFile::~CModuleLogFile(void)
{	
	OnDestroy(); 
}

BOOL CModuleLogFile::OnCreate( char* strLogFilePath )
{
	i3String::Copy( m_strLogFilePath, strLogFilePath, MAX_PATH );

	// LogList 생성
	m_pLogList = i3List::NewObject();
	if( NULL == m_pLogList )						return FALSE;

	//Set Time; 
	m_i32DateHour		= 0; 
	m_hFile				= INVALID_HANDLE_VALUE; 

	// 자신의 로그파일 생성
	m_i32LogThreadIdx	= InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )	return FALSE;

	if( FALSE == _CreateFile() )					return FALSE;

	//2. Make Thread 
	if( FALSE == i3Thread::Create( "CModuleLogFile", 0, 4096, NULL, PRIORITY_NORMAL) ) return FALSE; 	

	return TRUE; 
}

INT32 CModuleLogFile::InsertLogBuffer()
{
	INT32 i32Idx = -1;
	LOG_FILE_BUFFER* pstFileBuffer = (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) );
	if( NULL == pstFileBuffer )					return i32Idx;
	i3mem::FillZero( pstFileBuffer, sizeof( LOG_FILE_BUFFER ) );

	i32Idx = m_pLogList->Add( pstFileBuffer );

	return i32Idx;
}

void CModuleLogFile::OnDestroy(void)
{
	m_bIsRunning	= FALSE;
	i3Thread::WaitForFinish();

	INT32 i32Count = m_pLogList->GetCount();
	for( INT32 i = i32Count-1 ; i >= 0 ; i-- )
	{
		LOG_FILE_BUFFER* pstFileBuffer = (LOG_FILE_BUFFER*)m_pLogList->GetItem(i);
		m_pLogList->Delete( i );

		I3MEM_SAFE_FREE( pstFileBuffer );
	}
	I3_SAFE_RELEASE( m_pLogList );

	if( INVALID_HANDLE_VALUE != m_hFile )
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}
}

UINT32 CModuleLogFile::OnRunning( void * pUserData)
{
	INT32	i32SendCount; 
	INT32	i32ReadIdx;
	BOOL	bWorking;

	m_bIsRunning = TRUE;
	while(m_bIsRunning)
	{
		bWorking = FALSE;
		//0. Create New File 
		if( m_i32DateHour != g_pSystem->GetHour() )
		{			
			_CreateFile();
			bWorking = TRUE;
		}

		for( INT32 i = 0 ; i < m_pLogList->GetCount() ; i++ )
		{
			LOG_FILE_BUFFER* pstFileBuffer = (LOG_FILE_BUFFER*)m_pLogList->GetItem(i);

			i32SendCount = pstFileBuffer->_i32WritePos - pstFileBuffer->_i32ReadPos;
			for( INT32 j = 0; j < i32SendCount; j++ )
			{
				i32ReadIdx = pstFileBuffer->_i32ReadPos % LOG_FILE_BUFFER_COUNT;
				_WriteLogFile( pstFileBuffer->_szMessage[i32ReadIdx]);
				pstFileBuffer->_i32ReadPos++;

				bWorking = TRUE;
			}
		}

		if( FALSE == bWorking ) ::WaitForSingleObject( m_Handle, 1 );
	}

	return 0;
}

//void CModuleLogFile::WriteLog( INT32 i32ThreadIdx, const char * strLog, ... )
//{
//	char strTemp[ LOG_STRING_BASIC ];
//
//	va_list marker;
//
//	va_start( marker, strLog );
//
//	int i32Max = LOG_STRING_BASIC - 1;
//	int i32Rv = _vsnprintf( strTemp, LOG_STRING_BASIC, strLog, marker );
//
//	if ((i32Rv < 0) || (i32Rv >= i32Max))
//	{
//		strTemp[i32Max] = '\0';
//	}
//
//	va_end( marker);
//
//	s_pThisClass->_InsertBuffer( i32ThreadIdx, strTemp );
//}

void CModuleLogFile::InsertBuffer( INT32 i32ThreadIdx, char * strLog )
{
	if( (0 > m_pLogList->GetCount()) || (i32ThreadIdx > m_pLogList->GetCount()) )
	{
		I3ASSERT_0;
	}

	LOG_FILE_BUFFER* pstFileBuffer = (LOG_FILE_BUFFER*)m_pLogList->GetItem( i32ThreadIdx );

	if( pstFileBuffer->_i32WritePos == pstFileBuffer->_i32ReadPos )
	{
		pstFileBuffer->_i32WritePos	= 0; 
		pstFileBuffer->_i32ReadPos	= 0; 
	}

	if( pstFileBuffer->_i32ReadPos > BUFFER_MAX_INT_COUNT )										return; 
	if( ( pstFileBuffer->_i32WritePos - pstFileBuffer->_i32ReadPos ) >= LOG_FILE_BUFFER_COUNT )	return; 
	INT32 i32BufferIdx = pstFileBuffer->_i32WritePos % LOG_FILE_BUFFER_COUNT; 

	i3String::Format( pstFileBuffer->_szMessage[ i32BufferIdx ], LOG_STRING_MAX, "[%02d:%02d]%s\r\n", g_pSystem->GetMinute(), g_pSystem->GetSecond(), strLog );

	pstFileBuffer->_i32WritePos = pstFileBuffer->_i32WritePos + 1;
	return; 
}

BOOL CModuleLogFile::_CreateFile(void)
{
	//Close Old File
	if( INVALID_HANDLE_VALUE != m_hFile )
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	//Create New File
	char strFileName[ MAX_PATH ];
	i3String::Format( strFileName, MAX_PATH, "%s\\%s_%04d_%02d_%02d_%02d.txt", m_strLogFilePath, "Billing", g_pSystem->GetYear(), g_pSystem->GetMonth(), g_pSystem->GetDay(), g_pSystem->GetHour() );

	m_hFile = ::CreateFile( strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return FALSE;
	}
	
	SetFilePointer( m_hFile, 0, NULL, FILE_END );
	
	char strMessage[ MAX_PATH ];
	i3String::Format( strMessage, MAX_PATH, "-- COMPILE DATE : Ver:%d/%d, BuildDate:%s %s --\r\n", GAME_VER_MONTH, GAME_VER_DAY, __DATE__, __TIME__ );
	INT32 i32Len = i3String::Length( strMessage ); 
	::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );

	m_i32DateHour = g_pSystem->GetHour();

	return TRUE;
}

void CModuleLogFile::_WriteLogFile( char* strLog )
{
	INT32 i32Len = i3String::Length( strLog ); 
	::WriteFile( m_hFile, strLog, i32Len, (LPDWORD)&i32Len, NULL ); 

	I3TRACE( strLog );
}