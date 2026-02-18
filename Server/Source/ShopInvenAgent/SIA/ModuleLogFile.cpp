#include "pch.h"
//#include "ModuleLogFile.h"

CModuleLogFile * g_pModuleLogFile = NULL; 

static UINT32 _DefThreadProc( void * pPointer )
{
	pPointer;
	g_pModuleLogFile->OnWorking();
	return 0;
}

CModuleLogFile::CModuleLogFile(void)
{
	m_hThread						= NULL;
	m_bIsRunning					= FALSE;

	m_ui32WorkCounter				= 0;
	i3mem::FillZero( m_ppLogBuffer, sizeof(i3RingBuffer*)*LOG_BUFFER_MAX_COUNTER );
	
	for( INT32 i = 0; i < WARNING_TYPE_COUNT; i++)
	{
		for( INT32 j = 0; j < WARNING_LEVEL_COUNT; j++ )
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

	//2. Create File ; 	
	if( FALSE == _CreateFile() )									return false; 

	//3. Make Thread 
	UINT32	ui32ThreadID;
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ,0 ,(DWORD *)&ui32ThreadID);
	if(m_hThread == NULL)											return false; 
	if( !SetThreadPriority( m_hThread, THREAD_PRIORITY_LOG) )		return false; 

	return true; 
}

bool CModuleLogFile::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	for( INT32 i = 0 ; i < m_ui32WorkCounter ; i++ )
	{
		I3_SAFE_RELEASE( m_ppLogBuffer[i] );
	}
	return true; 
}

BOOL CModuleLogFile::AddLogThread( TCHAR* strPreString )
{
	INT32 i32Rv = RV_ADD_FAIL;
	m_pAddCs.Enter(); 

	// 더이상 로그 파일을 생성 할 수 없습니다.
	if( LOG_BUFFER_MAX_COUNTER <= m_ui32WorkCounter )		goto e_Exit;

	m_ppLogBuffer[ m_ui32WorkCounter  ] = i3RingBuffer::NewObject();
	if( NULL == m_ppLogBuffer[ m_ui32WorkCounter ] )		goto e_Exit;
	
	if( false == m_ppLogBuffer[ m_ui32WorkCounter ]->Create( sizeof(LOG_FILE_BUFFER), LOG_FILE_BUFFER_COUNT ) )
	{	// Ring Buffer 메모리 할당 실패
		delete m_ppLogBuffer[ m_ui32WorkCounter ];
		goto e_Exit;
	}

	i3String::Copy( m_astrLogInit[ m_ui32WorkCounter ], strPreString, LOG_INIT_STRING );

	i32Rv = m_ui32WorkCounter;
	m_ui32WorkCounter++;

e_Exit:

	m_pAddCs.Leave(); 

	return i32Rv;
}

void CModuleLogFile::OnWorking(void)
{
	INT32 i, j;
	INT32 i32SendCount; 
	LOG_FILE_BUFFER * pLogBuffer;
	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		if( m_ui32FileHour != g_ui32Hour )_CreateFile(); 

		for( i = 0 ; i < m_ui32WorkCounter ; i++ )
		{
			//Main Thread 
	 		i32SendCount = m_ppLogBuffer[i]->GetBufferCount();
			for( j = 0 ; j < i32SendCount ; j++ )
			{
				pLogBuffer = (LOG_FILE_BUFFER *)m_ppLogBuffer[i]->Pop(); 
				_WriteLogFile( pLogBuffer->_szMessage );	
				m_ppLogBuffer[i]->PopIdx(); 
			}
		}

		Sleep(1); 
	}

	return; 
}

bool CModuleLogFile::_CreateFile(void)
{
	bool bRv = true; 
	char strFileName[MAX_PATH]; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format(strFileName, MAX_PATH, "%s\\SIA_%04d_%02d_%02d_%02d.txt", g_pConfig->m_strLogFilePath, g_ui32Year, g_ui32Month, g_ui32Day, g_ui32Hour);

	m_hFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( i3String::Length(strFileName) >= 256)		return false;

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
		char strMessage[MAX_PATH]; 
		i3String::Format( strMessage, MAX_PATH, "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s, %s --\r\n", VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL,  __DATE__, __TIME__ );
		INT32 i32Len = i3String::Length(strMessage);
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return bRv; 
}

void CModuleLogFile::Write_LogFile( INT32 i32WorkIdx, INT32 i32WarningLevel, TCHAR * pstrLogString, ... )
{
	char strTemp[ MAX_LOG_PATH ];

	va_list marker;
	va_start( marker, pstrLogString );
	_vsntprintf_s( strTemp, MAX_LOG_PATH, _TRUNCATE, pstrLogString, marker );
	va_end( marker);


	i3RingBuffer*		pRingBuffer = m_ppLogBuffer[i32WorkIdx]; 
	LOG_FILE_BUFFER			LogBuffer; 

	//Make PreString
	i3String::Format( LogBuffer._szMessage, MAX_LOG_PATH, _T("[%02d:%02d][%s]%s\r\n"), g_ui32Minute, g_ui32Second, m_astrLogInit[i32WorkIdx], strTemp );
	pRingBuffer->Push( (void*)&LogBuffer );
}

void CModuleLogFile::Write_WarningLogFile( INT32 i32WorkIdx, INT32 i32WarningType, INT32 i32WarningLevel, TCHAR * pstrLogString, ... )
{
	char strTemp[ MAX_LOG_PATH ];

	va_list marker;
	va_start( marker, pstrLogString );
	_vsntprintf_s( strTemp, MAX_LOG_PATH, _TRUNCATE, pstrLogString, marker );
	va_end( marker);
		

	i3RingBuffer*		pRingBuffer = m_ppLogBuffer[i32WorkIdx]; 
	LOG_FILE_BUFFER			LogBuffer; 

	//Make PreString
	i3String::Format( LogBuffer._szMessage, MAX_LOG_PATH, _T("[%02d:%02d][%s]%s\r\n"), g_ui32Minute, g_ui32Second, m_astrLogInit[i32WorkIdx], strTemp );

	SetWarningInfo( i32WarningType, i32WarningLevel);
}

UINT32	CModuleLogFile::GetWarningInfo( INT32 i32WarningType )
{
	if( 0 > i32WarningType )						return 0;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return 0;

	UINT32 ui32WarningInfo = 0;

	for (INT32 i = 0; i < WARNING_LEVEL_COUNT; i++)
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
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return FALSE;
	if( WARNING_LEVEL_NO > i32WarningLevel )		return FALSE;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return FALSE;

	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();

	m_ui32WriteTime[i32WarningType][i32WarningLevel - 1] = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.

	return TRUE;
}

UINT32	CModuleLogFile::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}