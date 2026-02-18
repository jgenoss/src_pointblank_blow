#include "pch.h"
#include "ModuleServiceLogFile.h"

I3_CLASS_INSTANCE(CModuleServiceLogFile, i3Thread); 

CModuleServiceLogFile * g_pModuleServiceLogFile = NULL; 

CModuleServiceLogFile::CModuleServiceLogFile(void)
{
	m_bIsRunning						= FALSE;
}

CModuleServiceLogFile::~CModuleServiceLogFile(void)
{	
	OnDestroy(); 
}

BOOL CModuleServiceLogFile::OnCreate()
{
	//Create Buffer 
	m_i32ServiceLog_DB_ThreadCount	= DATABASE_THREAD_COUNT; 
	m_pi32ServiceLog_DB_WritePos	= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ServiceLog_DB_ThreadCount) ;
	m_pi32ServiceLog_DB_ReadPos		= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ServiceLog_DB_ThreadCount) ;
	i3mem::FillZero(m_pi32ServiceLog_DB_WritePos,		sizeof(INT32) * m_i32ServiceLog_DB_ThreadCount ); 
	i3mem::FillZero(m_pi32ServiceLog_DB_ReadPos,		sizeof(INT32) * m_i32ServiceLog_DB_ThreadCount ); 

	m_ppServiceLog_DB_FileBuffer = (SERVICE_LOG_FILE_BUFFER **) i3MemAlloc( sizeof(SERVICE_LOG_FILE_BUFFER*) * m_i32ServiceLog_DB_ThreadCount );
	for(INT32 i = 0; i < m_i32ServiceLog_DB_ThreadCount; i++)
	{
		m_ppServiceLog_DB_FileBuffer[i] = (SERVICE_LOG_FILE_BUFFER*)i3MemAlloc( sizeof(SERVICE_LOG_FILE_BUFFER) * SERVICE_LOG_FILE_BUFFER_MAX);
	}

	m_pServiceLog_Buy_FileBuffer	= (SERVICE_LOG_FILE_BUFFER*)i3MemAlloc( sizeof(SERVICE_LOG_FILE_BUFFER) * SERVICE_LOG_FILE_BUFFER_MAX); 
	m_i32ServiceLog_Buy_WritePos	= 0;
	m_i32ServiceLog_Buy_ReadPos		= 0;

	m_pServiceLog_Gift_FileBuffer	= (SERVICE_LOG_FILE_BUFFER*)i3MemAlloc( sizeof(SERVICE_LOG_FILE_BUFFER) * SERVICE_LOG_FILE_BUFFER_MAX); 
	m_i32ServiceLog_Gift_WritePos	= 0;
	m_i32ServiceLog_Gift_ReadPos	= 0;

	m_pServiceLog_Repair_FileBuffer	= (SERVICE_LOG_FILE_BUFFER*)i3MemAlloc( sizeof(SERVICE_LOG_FILE_BUFFER) * SERVICE_LOG_FILE_BUFFER_MAX); 
	m_i32ServiceLog_Repair_WritePos	= 0;
	m_i32ServiceLog_Repair_ReadPos	= 0;


	//Set Time; 
	m_ui32DateHour = 0;
	m_hFileDB		= INVALID_HANDLE_VALUE;
	m_hFileBuy		= INVALID_HANDLE_VALUE;
	m_hFileGift		= INVALID_HANDLE_VALUE;
	m_hFileRepair	= INVALID_HANDLE_VALUE; 

	
	// CreateFile
	for( INT32 i32LogType = 0 ; i32LogType < SERVICE_LOG_TYPE_MAX ; i32LogType++ )
	{
		if( FALSE == _CreateFile((SERVICE_LOG_TYPE)i32LogType))	return false; 
	}
	
	//2. Make Thread 
	if( !i3Thread::Create("CModuleServiceLogFile", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE; 	

	return TRUE; 
}

void CModuleServiceLogFile::OnDestroy(void)
{
	m_bIsRunning	= FALSE;
	i3Thread::WaitForFinish();

	if( m_ppServiceLog_DB_FileBuffer )
	{
		for(INT32 i = 0; i < m_i32ServiceLog_DB_ThreadCount; i++)
		{
			I3MEM_SAFE_FREE( m_ppServiceLog_DB_FileBuffer[i] );
		}
		I3MEM_SAFE_FREE( m_ppServiceLog_DB_FileBuffer ); 
	}
	I3MEM_SAFE_FREE( m_pi32ServiceLog_DB_WritePos ); 
	I3MEM_SAFE_FREE( m_pi32ServiceLog_DB_ReadPos ); 

	
	I3MEM_SAFE_FREE(m_pServiceLog_Buy_FileBuffer); 
	I3MEM_SAFE_FREE(m_pServiceLog_Gift_FileBuffer); 
	I3MEM_SAFE_FREE(m_pServiceLog_Repair_FileBuffer); 

	return; 
}

UINT32 CModuleServiceLogFile::OnRunning( void * pUserData)
{
	INT32	i32SendCount;
	INT32	i32ReadIdx;
	m_bIsRunning = TRUE;

	while(m_bIsRunning)
	{
		//0. Create New File 
		if( m_ui32DateHour != (UINT32)i3ThreadTimer::LocalTime().GetHour() )
		{
			for( INT32 i32LogType = 0 ; i32LogType < SERVICE_LOG_TYPE_MAX ; i32LogType++ )
			{
				_CreateFile((SERVICE_LOG_TYPE)i32LogType); 
			}
		}

		//1. DB
		for(INT32 i = 0; i < m_i32ServiceLog_DB_ThreadCount; i++)
		{
			SERVICE_LOG_FILE_BUFFER	* pLogFileBuffer = m_ppServiceLog_DB_FileBuffer[i]; 
			i32SendCount = m_pi32ServiceLog_DB_WritePos[i] - m_pi32ServiceLog_DB_ReadPos[i];
			
			for(INT32 j = 0; j < i32SendCount; j++)
			{
				i32ReadIdx = m_pi32ServiceLog_DB_ReadPos[i] % SERVICE_LOG_FILE_BUFFER_MAX; 
				_WriteLogFile( SERVICE_LOG_TYPE_DB, pLogFileBuffer[i32ReadIdx].m_strMessage );
				m_pi32ServiceLog_DB_ReadPos[i]++; 
			}
		}

		// 2. Buy
	 	i32SendCount = m_i32ServiceLog_Buy_WritePos - m_i32ServiceLog_Buy_ReadPos;
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ServiceLog_Buy_ReadPos % SERVICE_LOG_FILE_BUFFER_MAX; 
			_WriteLogFile( SERVICE_LOG_TYPE_BUY, m_pServiceLog_Buy_FileBuffer[i32ReadIdx].m_strMessage );
			m_i32ServiceLog_Buy_ReadPos++;
		}

		// 3. Gift
	 	i32SendCount = m_i32ServiceLog_Gift_WritePos - m_i32ServiceLog_Gift_ReadPos;
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ServiceLog_Gift_ReadPos % SERVICE_LOG_FILE_BUFFER_MAX; 
			_WriteLogFile( SERVICE_LOG_TYPE_GIFT, m_pServiceLog_Gift_FileBuffer[i32ReadIdx].m_strMessage );
			m_i32ServiceLog_Gift_ReadPos++;
		}

		// 4. Repair
	 	i32SendCount = m_i32ServiceLog_Repair_WritePos - m_i32ServiceLog_Repair_ReadPos;
		for(INT32 i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ServiceLog_Repair_ReadPos % SERVICE_LOG_FILE_BUFFER_MAX; 
			_WriteLogFile( SERVICE_LOG_TYPE_REPAIR, m_pServiceLog_Repair_FileBuffer[i32ReadIdx].m_strMessage );
			m_i32ServiceLog_Repair_ReadPos++;
		}

		::WaitForSingleObject( m_Handle, 1 );
	}

	return 0; 
}

void CModuleServiceLogFile::WriteLog(SERVICE_LOG_TYPE eLogType, INT32 i32ThreadIdx, char * pstrLogString)
{
	SERVICE_LOG_FILE_BUFFER	*	pLogBuffer		= NULL;
	INT32					*	pi32WritePos	= NULL;
	INT32					*	pi32ReadPos		= NULL;

	switch( eLogType ) 
	{
	case SERVICE_LOG_TYPE_DB:
		pLogBuffer		= m_ppServiceLog_DB_FileBuffer[i32ThreadIdx - THREAD_S_DATABASE_IDX ];
		pi32WritePos	= &m_pi32ServiceLog_DB_WritePos[i32ThreadIdx - THREAD_S_DATABASE_IDX ];
		pi32ReadPos		= &m_pi32ServiceLog_DB_ReadPos[i32ThreadIdx - THREAD_S_DATABASE_IDX ];
		break;
	case SERVICE_LOG_TYPE_BUY:
		pLogBuffer		=  m_pServiceLog_Buy_FileBuffer;
		pi32WritePos	= &m_i32ServiceLog_Buy_WritePos;
		pi32ReadPos		= &m_i32ServiceLog_Buy_ReadPos;
		break;
	case SERVICE_LOG_TYPE_GIFT:
		pLogBuffer		=  m_pServiceLog_Gift_FileBuffer;
		pi32WritePos	= &m_i32ServiceLog_Gift_WritePos;
		pi32ReadPos		= &m_i32ServiceLog_Gift_ReadPos;
		break;
	case SERVICE_LOG_TYPE_REPAIR:
		pLogBuffer		=  m_pServiceLog_Repair_FileBuffer;
		pi32WritePos	= &m_i32ServiceLog_Repair_WritePos;
		pi32ReadPos		= &m_i32ServiceLog_Repair_ReadPos;
		break;
	default:
		return;
		break;
	}

	if(*pi32WritePos == *pi32ReadPos)
	{
		*pi32WritePos	= 0;
		*pi32ReadPos	= 0;
	}

	if( *pi32ReadPos > BUFFER_MAX_INT_COUNT )return;
	if( (*pi32WritePos - *pi32ReadPos) >= SERVICE_LOG_FILE_BUFFER_MAX )return;
	INT32 i32BufferIdx = *pi32WritePos % SERVICE_LOG_FILE_BUFFER_MAX;

	char strTime[MAX_PATH];
	i3String::Format( strTime, MAX_PATH, "[%02d:%02d]", i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond());
	i3String::Format( pLogBuffer[i32BufferIdx].m_strMessage, SERVICE_LOG_MESSAGE_SIZE, "%s%s\r\n", strTime, pstrLogString);

	*pi32WritePos = *pi32WritePos + 1;
 
	return; 
}

BOOL CModuleServiceLogFile::_CreateFile(SERVICE_LOG_TYPE eType)
{
	BOOL bRv = TRUE;
	char strFileName[MAX_PATH];
	HANDLE * pHandle = NULL;

	switch( eType )
	{
	case SERVICE_LOG_TYPE_DB:
		{ 
			i3String::Format(strFileName, MAX_PATH, "%s\\DB_%04d_%02d_%02d_%02d.txt",
				g_pConfig->m_strServiceLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
			pHandle = &m_hFileDB;
		}
		break;
	case SERVICE_LOG_TYPE_BUY:
		{ 
			i3String::Format(strFileName, MAX_PATH, "%s\\BUY_%04d_%02d_%02d_%02d.txt",
				g_pConfig->m_strServiceLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
			pHandle = &m_hFileBuy;
		}
		break;
	case SERVICE_LOG_TYPE_GIFT:
		{ 
			i3String::Format(strFileName, MAX_PATH, "%s\\GIFT_%04d_%02d_%02d_%02d.txt",
				g_pConfig->m_strServiceLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
			pHandle = &m_hFileGift;
		}
		break;
	case SERVICE_LOG_TYPE_REPAIR:
		{ 
			i3String::Format(strFileName, MAX_PATH, "%s\\REPAIR_%04d_%02d_%02d_%02d.txt",
				g_pConfig->m_strServiceLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );
			pHandle = &m_hFileRepair;
		}
		break;
	}

	if( NULL == pHandle )	return FALSE;

	if( INVALID_HANDLE_VALUE != *pHandle )
	{
		::CloseHandle( *pHandle );
		*pHandle = INVALID_HANDLE_VALUE; 
	}

	// Create New File
	*pHandle = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if(*pHandle == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( *pHandle );
		*pHandle = INVALID_HANDLE_VALUE; 
		bRv = FALSE;
	}
	else
	{
		SetFilePointer( *pHandle, 0, NULL, FILE_END);
	}

	if(TRUE == bRv)
	{
		m_ui32DateHour = i3ThreadTimer::LocalTime().GetHour();
		
		char strMessage[MAX_PATH]; 
		i3String::Format( strMessage, MAX_PATH, "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, BuildVer:%s, StartDate:%s --\r\n", 
			VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL, __DATE__, __TIME__, BUILD_VER, g_pConfig->GetServerStartTime() );
		INT32 i32Len = i3String::Length(strMessage); 
		::WriteFile( *pHandle, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return bRv;
}


void CModuleServiceLogFile::_WriteLogFile(SERVICE_LOG_TYPE eType, char * pstrLog)
{
	INT32 i32Len = i3String::Length(pstrLog);
	HANDLE * pHandle = NULL; 

	switch( eType )
	{
	case SERVICE_LOG_TYPE_DB:
		pHandle = &m_hFileDB;
		break;
	case SERVICE_LOG_TYPE_BUY:
		pHandle = &m_hFileBuy;
		break;
	case SERVICE_LOG_TYPE_GIFT:
		pHandle = &m_hFileGift;
		break;
	case SERVICE_LOG_TYPE_REPAIR:
		pHandle = &m_hFileRepair;
		break;
	}

	if( pHandle == NULL ) return;

	::WriteFile( *pHandle, pstrLog, i32Len, (LPDWORD)&i32Len, NULL );

	I3TRACE( pstrLog );

	return;
}
