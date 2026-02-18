#include "pch.h"
#include "Config.h"

I3_CLASS_INSTANCE(CLogFile, i3Thread); 

CLogFile * g_pLogFile = NULL; 
#define LOG_BUFFER_COUNT		50

CLogFile::CLogFile(void)
{
	m_hFile				= INVALID_HANDLE_VALUE;
	m_hChangeNickFile	= INVALID_HANDLE_VALUE;
	m_hChangeRankFile	= INVALID_HANDLE_VALUE;
	m_i32Hour			= -1;

	m_ppReceiverBuffer	= NULL;
	m_ppDataBaseBuffer	= NULL;
	m_pMainBuffer		= NULL;
	m_pTaskBuffer		= NULL;
	m_pGateWay			= NULL;
	m_pLogDBuffer		= NULL;
	m_pControlBuffer	= NULL;

	// Change Nick, Rank ErrorLog
	m_pChangeNickBuffer = NULL;
	m_pChangeRankBuffer	= NULL;

	m_ppChangeNickErrorDBBuffer	= NULL;
	m_ppChangeRankErrorDBBuffer	= NULL;

} // CLogFile()

CLogFile::~CLogFile(void)
{
	OnDestroy();
} // ~CLogFile

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::OnCreate(void)
{	
	m_ui32WorkingCounter = 0;
	m_ui32ChangeNickWorkingCounter = 0;
	m_ui32ChangeRankWorkingCounter = 0;
    
	m_pMainBuffer = new Local_CRingBuffer;
	if(m_pMainBuffer == NULL) return FALSE;
	if(!m_pMainBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer(m_pMainBuffer);

	m_pTaskBuffer = new Local_CRingBuffer;
	if(m_pTaskBuffer == NULL) return FALSE;
	if(!m_pTaskBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer(m_pTaskBuffer);

	m_pLogDBuffer = new Local_CRingBuffer;
	if(m_pLogDBuffer == NULL) return FALSE;
	if(!m_pLogDBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer(m_pLogDBuffer);

	m_pGateWay = new Local_CRingBuffer;
	if(m_pGateWay == NULL) return FALSE;
	if(!m_pGateWay->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer(m_pGateWay);

	m_pControlBuffer = new Local_CRingBuffer;
	if(m_pControlBuffer == NULL) return FALSE;
	if(!m_pControlBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddRingBuffer(m_pControlBuffer);

	m_pChangeNickBuffer = new Local_CRingBuffer;
	if(m_pChangeNickBuffer == NULL) return FALSE;
	if(!m_pChangeNickBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddChangeNickRingBuffer(m_pChangeNickBuffer);

	m_pChangeRankBuffer = new Local_CRingBuffer;
	if(m_pChangeRankBuffer == NULL) return FALSE;
	if(!m_pChangeRankBuffer->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE;
	AddChangeRankRingBuffer(m_pChangeRankBuffer);

	if(!FileOpen() )return FALSE;
	//if(!ChangeNickErrorLogFileOpen() )return FALSE;
	//if(!ChangeRankErrorLogFileOpen() )return FALSE;

	if( !i3Thread::Create("CLogFile", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE;

	return TRUE;
} // OnCreate()

BOOL CLogFile::OnCreateGame(void)
{
	m_ppReceiverBuffer = (Local_CRingBuffer**)i3MemAlloc( sizeof(Local_CRingBuffer*) * g_pConfig->m_ui32ServerCount);
	for(UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; i++)
	{
		m_ppReceiverBuffer[i]  = new Local_CRingBuffer;
		if(m_ppReceiverBuffer[i] == NULL)return FALSE;
		if(! m_ppReceiverBuffer[i]->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE; 
		AddRingBuffer(m_ppReceiverBuffer[i]);
	}

	m_ppDataBaseBuffer = (Local_CRingBuffer**)i3MemAlloc( sizeof(Local_CRingBuffer*) * DB_THREAD_COUNT); 
	for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_ppDataBaseBuffer[i]  = new Local_CRingBuffer;
		if(m_ppDataBaseBuffer[i] == NULL)return FALSE;
		if(! m_ppDataBaseBuffer[i]->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE; 
		AddRingBuffer(m_ppDataBaseBuffer[i]);
	}

	m_ppChangeNickErrorDBBuffer = (Local_CRingBuffer**)i3MemAlloc( sizeof(Local_CRingBuffer*) * DB_THREAD_COUNT); 
	for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_ppChangeNickErrorDBBuffer[i]  = new Local_CRingBuffer;
		if(m_ppChangeNickErrorDBBuffer[i] == NULL)return FALSE;
		if(! m_ppChangeNickErrorDBBuffer[i]->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE; 
		AddChangeNickRingBuffer(m_ppChangeNickErrorDBBuffer[i]);
	}

	m_ppChangeRankErrorDBBuffer = (Local_CRingBuffer**)i3MemAlloc( sizeof(Local_CRingBuffer*) * DB_THREAD_COUNT); 
	for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
	{
		m_ppChangeRankErrorDBBuffer[i]  = new Local_CRingBuffer;
		if(m_ppChangeRankErrorDBBuffer[i] == NULL)return FALSE;
		if(! m_ppChangeRankErrorDBBuffer[i]->OnCreate( sizeof(LOG_FILE_MESSAGE), LOG_BUFFER_COUNT ) )return FALSE; 
		AddChangeRankRingBuffer(m_ppChangeRankErrorDBBuffer[i]);
	}

	return TRUE;
} // OnCreateGame()
/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::FileOpen(void)
{
	char strFileName[LOG_FILE_NAME];

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	//Create New File 
	i3String::Format(strFileName, LOG_FILE_NAME, "%s\\Messenger_%04d_%02d_%02d_%02d.txt",
		g_pConfig->m_strLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay(), i3ThreadTimer::LocalTime().GetHour() );

#ifdef UNICODE
	WCHAR16 wchar[256]; 
	i3String::CopyASCIItoWCHAR(wchar, strFileName);
	m_hFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);
		m_i32Hour = i3ThreadTimer::LocalTime().GetHour();
	}

	{
		INT32 i32Len; 
		char strMessage[MAX_STRING_COUNT];
		i3String::Format( strMessage, MAX_STRING_COUNT, "-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, StartDate:%s --\r\n", VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL, __DATE__, __TIME__, g_pConfig->GetServerStartTime() );
		i32Len = i3String::Length( strMessage ); 
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return TRUE; 
} // FileOpen()

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::ChangeNickErrorLogFileOpen()
{
	char strFileName[LOG_FILE_NAME];

	//Close Old File 
	if( m_hChangeNickFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hChangeNickFile );
		m_hChangeNickFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format(strFileName, LOG_FILE_NAME, "%s\\ChangeNickErrorLog_%04d_%02d_%02d.txt",
		g_pConfig->m_strErrorLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay() );

#ifdef UNICODE
	WCHAR16 wchar[256];
	i3String::CopyASCIItoWCHAR(wchar, strFileName);
	m_hChangeNickFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hChangeNickFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	if(m_hChangeNickFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hChangeNickFile );
		m_hChangeNickFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	else
	{
		SetFilePointer( m_hChangeNickFile, 0, NULL, FILE_END);
		m_i32Hour = i3ThreadTimer::LocalTime().GetHour();
	}

	return TRUE;
} // ChangeNickErrorLogFileOpen()

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::ChangeRankErrorLogFileOpen()
{
	char strFileName[LOG_FILE_NAME];

	//Close Old File 
	if( m_hChangeRankFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hChangeRankFile );
		m_hChangeRankFile = INVALID_HANDLE_VALUE;
	}

	//Create New File 
	i3String::Format(strFileName, LOG_FILE_NAME, "%s\\ChangeRankErrorLog_%04d_%02d_%02d.txt", 
		g_pConfig->m_strErrorLogFilePath, i3ThreadTimer::LocalTime().GetYear(), i3ThreadTimer::LocalTime().GetMonth(), i3ThreadTimer::LocalTime().GetDay() );

#ifdef UNICODE
	WCHAR16 wchar[25 6]; 
	i3String::CopyASCIItoWCHAR(wchar, strFileName);
	m_hChangeRankFile = ::CreateFile(wchar, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#else
	m_hChangeRankFile = ::CreateFile(strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
#endif

	if(m_hChangeRankFile == INVALID_HANDLE_VALUE)
	{			
		::CloseHandle( m_hChangeRankFile );
		m_hChangeRankFile = INVALID_HANDLE_VALUE;
		return FALSE; 
	}
	else
	{
		SetFilePointer( m_hChangeRankFile, 0, NULL, FILE_END);
		m_i32Hour = i3ThreadTimer::LocalTime().GetHour();
	}

	return TRUE; 
} // ChangeRankErrorLogFileOpen()

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::ChangeNickErrorLogFileClose()
{
	INT32 i32result = TRUE;

	if(m_hChangeNickFile != INVALID_HANDLE_VALUE)
	{	
		i32result = ::CloseHandle( m_hChangeNickFile );
		m_hChangeNickFile = INVALID_HANDLE_VALUE;
	}

	return i32result;
} // ChangeNickErrorLogFileClose()

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CLogFile::ChangeRankErrorLogFileClose()
{
	INT32 i32result = TRUE;

	if(m_hChangeRankFile != INVALID_HANDLE_VALUE)
	{
		i32result = ::CloseHandle( m_hChangeRankFile );
		m_hChangeRankFile = INVALID_HANDLE_VALUE;
	}

	return i32result;
} // ChangeRankErrorLogFileClose()

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CLogFile::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	if(m_ppReceiverBuffer != NULL)
	{
		for(UINT32 i = 0; i < g_pConfig->m_ui32ServerCount; i++)
		{
			delete m_ppReceiverBuffer[i];
		}
		I3MEM_SAFE_FREE( m_ppReceiverBuffer ); 
	}

	if(m_ppDataBaseBuffer != NULL)
	{
		for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
		{
			delete m_ppDataBaseBuffer[i];
		}
		I3MEM_SAFE_FREE( m_ppDataBaseBuffer ); 
	}	

	if(m_ppChangeNickErrorDBBuffer != NULL)
	{
		for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
		{
			delete m_ppChangeNickErrorDBBuffer[i]; 
		}
		I3MEM_SAFE_FREE( m_ppChangeNickErrorDBBuffer );
	}	

	if(m_ppChangeRankErrorDBBuffer != NULL)
	{
		for(UINT32 i = 0; i < DB_THREAD_COUNT; i++)
		{
			delete m_ppChangeRankErrorDBBuffer[i];
		}
		I3MEM_SAFE_FREE( m_ppChangeRankErrorDBBuffer );
	}	

	if( m_pMainBuffer	!= NULL )	delete m_pMainBuffer;
	if( m_pTaskBuffer	!= NULL )	delete m_pTaskBuffer;
	if( m_pGateWay		 != NULL )	delete m_pGateWay;
	if( m_pLogDBuffer	 != NULL )	delete m_pLogDBuffer;
	if( m_pControlBuffer != NULL )	delete m_pControlBuffer;

	if( m_pChangeNickBuffer != NULL )	delete m_pChangeNickBuffer;
	if( m_pChangeRankBuffer != NULL )	delete m_pChangeRankBuffer;

	m_pMainBuffer		= NULL;
	m_pTaskBuffer		= NULL;
	m_pTaskBuffer		= NULL;
	m_pGateWay			= NULL;
	m_pLogDBuffer		= NULL;

	m_pChangeNickBuffer	= NULL;
	m_pChangeRankBuffer = NULL;

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	ChangeNickErrorLogFileClose();
	ChangeRankErrorLogFileClose();

	return; 
}

void CLogFile::WriteLog(INT32 i32idx, char* pstrLogString, INT32 i32WarningLevel)
{
	INT32 i32Length;
	Local_CRingBuffer	  * pRingBuffer;
	LOG_FILE_MESSAGE	  *	pBuffer = NULL;

	switch(i32idx)
	{
	case THREAD_IDX_MAIN		: pRingBuffer = m_pMainBuffer;			 break;
	case THREAD_IDX_TASK		: pRingBuffer = m_pTaskBuffer;			 break;
	case THREAD_IDX_GATEWAY		: pRingBuffer = m_pGateWay;				 break;
	case THREAD_IDX_LOGD		: pRingBuffer = m_pLogDBuffer;			 break;
	case THREAD_IDX_CONTROL		: pRingBuffer = m_pControlBuffer;		 break;
	case THREAD_IDX_CHANGE_NICK : pRingBuffer = m_pChangeNickBuffer;	 break;
	case THREAD_IDX_CHANGE_RANK : pRingBuffer = m_pChangeRankBuffer;	 break;
	default						:
		{
			if(i32idx < THREAD_IDX_DATABASE_ST)
			{
				pRingBuffer = m_ppReceiverBuffer[i32idx];
			}
			else if (i32idx >= THREAD_IDX_DATABASE_ST && i32idx < THREAD_IDX_CHANGE_NICK_DB_ST)
			{
				pRingBuffer = m_ppDataBaseBuffer[i32idx - THREAD_IDX_DATABASE_ST];
			}
			else if (i32idx >= THREAD_IDX_CHANGE_NICK_DB_ST && i32idx < THREAD_IDX_CHANGE_RANK_DB_ST)
			{
				pRingBuffer = m_ppChangeNickErrorDBBuffer[i32idx - THREAD_IDX_CHANGE_NICK_DB_ST];
			}
			else
			{
				pRingBuffer = m_ppChangeRankErrorDBBuffer[i32idx - THREAD_IDX_CHANGE_RANK_DB_ST];
			}
		}
		break;
	}

	pBuffer = (LOG_FILE_MESSAGE  *)pRingBuffer->PushPointer(); 

	if(pBuffer != NULL)
	{
		i32Length = i3String::Length(pstrLogString) + 1;
		if(i32Length >= LOG_STRING_LEN) i32Length = LOG_STRING_LEN;

		i3mem::Copy(pBuffer->strLog, pstrLogString, i32Length);

		pBuffer->strLog[LOG_STRING_LEN-1]	= '\0';
		pBuffer->i32Length					= i32Length;
		pBuffer->_i32WarningLevel			= i32WarningLevel;
		pRingBuffer->PushPointerIdx(); 
	}
	return;	
}

UINT32 CLogFile::OnRunning( void * pUserData)
{
	char strTime[32];

	BOOL bWorking;
	INT32 i32Count;
	INT32 i32Len;

	m_bRunning = TRUE;
	LOG_FILE_MESSAGE * pLogBuffer;
	while( m_bRunning )
	{
		bWorking = FALSE;

		if( i3ThreadTimer::LocalTime().GetHour() != m_i32Hour )
		{
			FileOpen();
		}

		if( m_hFile != INVALID_HANDLE_VALUE)
		{
			i3String::Format( strTime, 32, "[%02d:%02d]", i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond() );
			for(UINT32 i = 0; i < m_ui32WorkingCounter; i++)
			{
				i32Count = m_pWorkingBuffer[i]->GetBufferCount();
				for(INT32 j = 0; j < i32Count; j++)
				{
					pLogBuffer = (LOG_FILE_MESSAGE *)m_pWorkingBuffer[i]->Pop();
					::WriteFile( m_hFile, "\r\n", 2, (LPDWORD)&i32Len, NULL );
					::WriteFile( m_hFile, strTime, 8, (LPDWORD)&i32Len, NULL );
					::WriteFile( m_hFile, pLogBuffer->strLog, pLogBuffer->i32Length, (LPDWORD)&i32Len, NULL );
					WriteWarningLevel( pLogBuffer->_i32WarningLevel );
					m_pWorkingBuffer[i]->PopIdx();
					bWorking = TRUE;
				}
			}
		}
		

		if( m_hChangeNickFile == INVALID_HANDLE_VALUE)
		{
			for(UINT32 i = 0; i < m_ui32ChangeNickWorkingCounter; i++)
			{
				i32Count = m_pChangeNickWorkingBuffer[i]->GetBufferCount();
				if ( 0 < i32Count)
				{
					i3String::Format( strTime, 32, "[%02d:%02d:%02d]",
						i3ThreadTimer::LocalTime().GetHour(),
						i3ThreadTimer::LocalTime().GetMinute(),
						i3ThreadTimer::LocalTime().GetSecond() );
					ChangeNickErrorLogFileOpen();
				
					for(INT32 j = 0; j < i32Count; j++)
					{
						pLogBuffer = (LOG_FILE_MESSAGE *)m_pChangeNickWorkingBuffer[i]->Pop();
						::WriteFile( m_hChangeNickFile, "\r\n", 2, (LPDWORD)&i32Len, NULL );
						::WriteFile( m_hChangeNickFile, strTime, 11, (LPDWORD)&i32Len, NULL );
						::WriteFile( m_hChangeNickFile, pLogBuffer->strLog, pLogBuffer->i32Length, (LPDWORD)&i32Len, NULL );			
						WriteWarningLevel( pLogBuffer->_i32WarningLevel );

						m_pChangeNickWorkingBuffer[i]->PopIdx();
						bWorking = TRUE;
					}
					ChangeNickErrorLogFileClose();
				}
			}
		}
		else
		{
			ChangeNickErrorLogFileClose();
		}

		if( m_hChangeRankFile == INVALID_HANDLE_VALUE)
		{
			for(UINT32 i = 0; i < m_ui32ChangeRankWorkingCounter; i++)
			{
				i3String::Format( strTime, 32, "[%02d:%02d:%02d]", i3ThreadTimer::LocalTime().GetHour(), i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond() );
				i32Count = m_pChangeRankWorkingBuffer[i]->GetBufferCount();
				if ( 0 < i32Count)
				{
					ChangeRankErrorLogFileOpen();

					for(INT32 j = 0; j < i32Count; j++)
					{
						pLogBuffer = (LOG_FILE_MESSAGE *)m_pChangeRankWorkingBuffer[i]->Pop();
						::WriteFile( m_hChangeRankFile, "\r\n", 2, (LPDWORD)&i32Len, NULL );
						::WriteFile( m_hChangeRankFile, strTime, 11, (LPDWORD)&i32Len, NULL );
						::WriteFile( m_hChangeRankFile, pLogBuffer->strLog, pLogBuffer->i32Length, (LPDWORD)&i32Len, NULL );
						WriteWarningLevel( pLogBuffer->_i32WarningLevel );

						m_pChangeRankWorkingBuffer[i]->PopIdx();
						bWorking = TRUE; 
					}
					ChangeRankErrorLogFileClose();
				}
			}
		}
		else
		{
			ChangeRankErrorLogFileClose();
		}

		//////////////////////////////////////////////////////////////////////////
		if( !bWorking )::WaitForSingleObject( m_Handle, 1 ); 
	}

	return 0;
}

// SWS
void CLogFile::WriteWarningLevel( INT32 i32WarningLevel )
{
	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();
	if ( WARNING_NO < i32WarningLevel && WARNING_NETWORK >= i32WarningLevel )
	{
		g_pConfig->m_aWarningInfo[ i32WarningLevel - 1 ]._ui32WriteTime = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.
	}
}