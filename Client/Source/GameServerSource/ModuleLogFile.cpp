#include "pch.h"
#include "DataBase.h"
#include "UserSession.h"
#include "UserManager.h"

I3_CLASS_INSTANCE(CModuleLogFile, i3Thread); 

#define	LOG_FILE_BUFFER_COUNT	100

CModuleLogFile * g_pModuleLogFile = NULL; 

CModuleLogFile::CModuleLogFile(void)
{
	m_bRunning						= FALSE;
	m_pMainLogFileBuffer			= NULL;	
	m_i32Main_WritePos				= 0;
	m_i32Main_ReadPos				= 0;

	m_pModuleNCLogFileBuffer		= NULL;	
	m_i32ModuleNc_WritePos			= 0;
	m_i32ModuleNc_ReadPos			= 0;

	m_pModuleControlLogFileBuffer	= NULL;
	m_i32ModuleControl_WritePos		= 0;
	m_i32ModuleControl_ReadPos		= 0;

	m_pModuleTransLogFileBuffer		= NULL;
	m_i32ModuleTrans_WritePos		= 0;
	m_i32ModuleTrans_ReadPos		= 0;

	m_pModuleClanLogFileBuffer		= NULL;
	m_i32ModuleClan_WritePos		= 0;
	m_i32ModuleClan_ReadPos			= 0;

	m_pModuleMessengerLogFileBuffer	= NULL;
	m_i32ModuleMessenger_WritePos	= 0;
	m_i32ModuleMessenger_ReadPos	= 0;

	m_pModuleDBLogFileBuffer		= NULL;	
	m_i32ModuleDB_WritePos			= 0; 
	m_i32ModuleDB_ReadPos			= 0; 

	m_ppUserLogFileBuffer			= NULL;	
	m_pUser_WritePos				= NULL;	
	m_pUser_ReadPos					= NULL;	

	m_pModuleXTrapLogFileBuffer		= NULL;	
	m_i32ModuleXTrap_WritePos		= 0;
	m_i32ModuleXTrap_ReadPos		= 0;
}

CModuleLogFile::~CModuleLogFile(void)
{	
	OnDestroy(); 
}

BOOL CModuleLogFile::OnCreate(INT32 i32ThreadCount)
{
	m_i32ThreadCount	= i32ThreadCount; 	

	//1. Make Buffer 
	m_pMainLogFileBuffer			= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); 
	m_i32Main_WritePos				= 0; 
	m_i32Main_ReadPos				= 0; 

	m_pModuleNCLogFileBuffer		= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); 
	m_i32ModuleNc_WritePos			= 0;	
	m_i32ModuleNc_ReadPos			= 0;	
	
	m_pModuleControlLogFileBuffer	= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); 
	m_i32ModuleControl_WritePos		= 0;	
	m_i32ModuleControl_ReadPos		= 0;	

	m_pModuleTransLogFileBuffer		= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); ;		//
	m_i32ModuleTrans_WritePos		= 0;
	m_i32ModuleTrans_ReadPos		= 0;

	m_pModuleClanLogFileBuffer		= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); ;		//
	m_i32ModuleClan_WritePos		= 0;
	m_i32ModuleClan_ReadPos			= 0;

	m_pModuleMessengerLogFileBuffer	= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); ;		//
	m_i32ModuleMessenger_WritePos	= 0;
	m_i32ModuleMessenger_ReadPos	= 0;

	m_pModuleDBLogFileBuffer		= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); 
	m_i32ModuleDB_WritePos			= 0; 
	m_i32ModuleDB_ReadPos			= 0; 

	m_pModuleXTrapLogFileBuffer		= (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT); 
	m_i32ModuleXTrap_WritePos		= 0; 
	m_i32ModuleXTrap_ReadPos		= 0;
	
	m_pUser_WritePos				= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ThreadCount );	
	i3mem::FillZero( m_pUser_WritePos, sizeof(INT32) * m_i32ThreadCount ); 
	m_pUser_ReadPos					= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32ThreadCount );	
	i3mem::FillZero( m_pUser_ReadPos, sizeof(INT32) * m_i32ThreadCount ); 

	m_ppUserLogFileBuffer		= (LOG_FILE_BUFFER**)i3MemAlloc( sizeof(LOG_FILE_BUFFER*) * m_i32ThreadCount ); 
	for(INT32 i = 0; i < m_i32ThreadCount; i++)
	{
		m_ppUserLogFileBuffer[i] = (LOG_FILE_BUFFER*)i3MemAlloc( sizeof(LOG_FILE_BUFFER) * LOG_FILE_BUFFER_COUNT);        
	}

	//Set Time; 
	m_ui32DateHour = 0; 
	m_hFile		= INVALID_HANDLE_VALUE; 

	if(!_CreateFile()) return FALSE; 
	//2. Make Thread 
	if( !i3Thread::Create("CModuleLogFile", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE; 	

	return TRUE; 
}

void CModuleLogFile::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();	

	//Delete 
	I3MEM_SAFE_FREE(m_pMainLogFileBuffer); 
	I3MEM_SAFE_FREE(m_pModuleNCLogFileBuffer); 
	I3MEM_SAFE_FREE(m_pModuleControlLogFileBuffer);
	I3MEM_SAFE_FREE(m_pModuleTransLogFileBuffer);
	I3MEM_SAFE_FREE(m_pModuleClanLogFileBuffer);
	I3MEM_SAFE_FREE(m_pModuleMessengerLogFileBuffer);
	I3MEM_SAFE_FREE(m_pModuleDBLogFileBuffer); 
	I3MEM_SAFE_FREE(m_pModuleXTrapLogFileBuffer);

	for(INT32 i = 0; i < m_i32ThreadCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppUserLogFileBuffer[i] );
	}
	I3MEM_SAFE_FREE( m_ppUserLogFileBuffer); 
	I3MEM_SAFE_FREE( m_pUser_WritePos ); 
	I3MEM_SAFE_FREE( m_pUser_ReadPos ); 

	return; 
}

UINT32 CModuleLogFile::OnRunning( void * pUserData)
{
	m_bRunning = TRUE;
	BOOL	bWorking;
	INT32	i32SendCount; 
	INT32	i32ReadIdx;
	INT32	i;
	while( m_bRunning )
	{
		bWorking = FALSE;
		//Create New File 
		if(m_ui32DateHour != g_pContextMain->m_ui32DateHour)
		{
			_CreateFile(); 
			bWorking = TRUE;
		}

		//1. Main 
	 	i32SendCount = m_i32Main_WritePos - m_i32Main_ReadPos;
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32Main_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pMainLogFileBuffer[i32ReadIdx]._szMessage );
	
			WriteWarningLevel( m_pMainLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32Main_ReadPos++; 
			bWorking = TRUE;
		}

		//2. Module Nc		
		i32SendCount = m_i32ModuleNc_WritePos - m_i32ModuleNc_ReadPos;
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleNc_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleNCLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleNCLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleNc_ReadPos++; 
			bWorking = TRUE;
		}
		
		//3. Module Control
		i32SendCount = m_i32ModuleControl_WritePos - m_i32ModuleControl_ReadPos;		
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleControl_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleControlLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleControlLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleControl_ReadPos++;
			bWorking = TRUE;
		}

		//3. Module Trans
		i32SendCount = m_i32ModuleTrans_WritePos - m_i32ModuleTrans_ReadPos;		
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleTrans_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleTransLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleTransLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleTrans_ReadPos++;
			bWorking = TRUE;
		}

		//3. Module Clan
		i32SendCount = m_i32ModuleClan_WritePos - m_i32ModuleClan_ReadPos;		
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleClan_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleClanLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleClanLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleClan_ReadPos++;
			bWorking = TRUE;
		}

		//3. Module Messenger
		i32SendCount = m_i32ModuleMessenger_WritePos - m_i32ModuleMessenger_ReadPos;		
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleMessenger_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleMessengerLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleMessengerLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleMessenger_ReadPos++;
			bWorking = TRUE;
		}

		//4. Module Db
		i32SendCount = m_i32ModuleDB_WritePos - m_i32ModuleDB_ReadPos;
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleDB_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleDBLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleDBLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleDB_ReadPos++;
			bWorking = TRUE;
		}

		//4. Module Db
		i32SendCount = m_i32ModuleXTrap_WritePos - m_i32ModuleXTrap_ReadPos;
		for( i = 0; i < i32SendCount; i++)
		{
			i32ReadIdx = m_i32ModuleXTrap_ReadPos % LOG_FILE_BUFFER_COUNT; 
			_WriteLogFile( m_pModuleXTrapLogFileBuffer[i32ReadIdx]._szMessage );

			WriteWarningLevel( m_pModuleXTrapLogFileBuffer[i32ReadIdx]._i32WarningLevel );

			m_i32ModuleXTrap_ReadPos++;
			bWorking = TRUE;
		}

		//5. Module User 
		for( i = 0; i < m_i32ThreadCount; i++)
		{
			LOG_FILE_BUFFER	* pLogFileBuffer = m_ppUserLogFileBuffer[i]; 
			i32SendCount = m_pUser_WritePos[i] - m_pUser_ReadPos[i];
			
			for(INT32 j = 0; j < i32SendCount; j++)
			{
				i32ReadIdx = m_pUser_ReadPos[i] % LOG_FILE_BUFFER_COUNT; 
				_WriteLogFile( pLogFileBuffer[i32ReadIdx]._szMessage );

				WriteWarningLevel( pLogFileBuffer[i32ReadIdx]._i32WarningLevel );

				m_pUser_ReadPos[i]++; 
				bWorking = TRUE;
			}
		}

		if( !bWorking )
		{
			::WaitForSingleObject( m_Handle, 1 );
		}
	}

	return 0; 
}

void CModuleLogFile::Write_LogFile(INT32 i32ThreadIdx, CUserSession * pSession,char * pLogString, /*SWS*/ INT32 i32WarningLevel )
{
	LOG_FILE_BUFFER	*	pLogBuffer;
	INT32			*	pi32WritePos;	
	INT32			*	pi32ReadPos;

	switch(i32ThreadIdx)
	{
	case MAIN_THREAD_IDX			: 
		pLogBuffer	= m_pMainLogFileBuffer; 
		pi32WritePos= &m_i32Main_WritePos;
		pi32ReadPos	= &m_i32Main_ReadPos;
		break; 
	case MODULE_NC_THREAD_IDX		: 
		pLogBuffer	= m_pModuleNCLogFileBuffer; 
		pi32WritePos= &m_i32ModuleNc_WritePos;
		pi32ReadPos	= &m_i32ModuleNc_ReadPos;
		break; 
	case MODULE_CONTROL_THREAD_IDX	:
		pLogBuffer	= m_pModuleControlLogFileBuffer; 
		pi32WritePos= &m_i32ModuleControl_WritePos;
		pi32ReadPos	= &m_i32ModuleControl_ReadPos;		
		break;
	case MODULE_TRANS_THREAD_IDX	:
		pLogBuffer	= m_pModuleTransLogFileBuffer; 
		pi32WritePos= &m_i32ModuleTrans_WritePos;
		pi32ReadPos	= &m_i32ModuleTrans_ReadPos;		
		break;
	case MODULE_CLAN_THREAD_IDX	:
		pLogBuffer	= m_pModuleClanLogFileBuffer; 
		pi32WritePos= &m_i32ModuleClan_WritePos;
		pi32ReadPos	= &m_i32ModuleClan_ReadPos;		
		break;
	case MODULE_MESSENGER_THREAD_IDX	:
		pLogBuffer	= m_pModuleMessengerLogFileBuffer; 
		pi32WritePos= &m_i32ModuleMessenger_WritePos;
		pi32ReadPos	= &m_i32ModuleMessenger_ReadPos;		
		break;
	case MODULE_DATABASE_THREAD_IDX	: 
		pLogBuffer	= m_pModuleDBLogFileBuffer; 
		pi32WritePos= &m_i32ModuleDB_WritePos;
		pi32ReadPos	= &m_i32ModuleDB_ReadPos;
		break; 
	case MODULE_GAMEGUARD_THREAD_IDX:		
		pLogBuffer	= m_pModuleXTrapLogFileBuffer; 
		pi32WritePos= &m_i32ModuleXTrap_WritePos;
		pi32ReadPos	= &m_i32ModuleXTrap_ReadPos;
		break; 
	default	: 
		pLogBuffer	= m_ppUserLogFileBuffer[i32ThreadIdx]; 
		pi32WritePos= &m_pUser_WritePos[i32ThreadIdx];
		pi32ReadPos	= &m_pUser_ReadPos[i32ThreadIdx];
		break; 
	}

	if(*pi32WritePos == *pi32ReadPos)
	{
		*pi32WritePos	= 0; 
		*pi32ReadPos	= 0; 
	}

	if( *pi32ReadPos > 2000000000 )return; 
	if( (*pi32WritePos - *pi32ReadPos) >= LOG_FILE_BUFFER_COUNT )return; 
	INT32 iBufferIdx = *pi32WritePos % LOG_FILE_BUFFER_COUNT; 

	char strTime[256];
	i3String::Format( strTime, 256, "[%02d:%02d:%04d]", g_pContextMain->m_i32Minute, g_pContextMain->m_i32Second, i32ThreadIdx);

	if( strlen( pLogString ) > (LOG_FILE_STRING_COUNT - 50) )// szTime 를 더한 사이즈 보다 작아야 로그를 출력할 수 있다. 
	{//로그파일에서 지정한 크기보다 크다. 		
		i3String::Format(pLogBuffer[iBufferIdx]._szMessage, LOG_FILE_STRING_COUNT, "%s[E]%s\r\n", strTime, "LOG_FILE_STRING_COUNT ERROR");
		return; 
	}

	if( pSession != NULL)
	{
		i3String::Format(pLogBuffer[iBufferIdx]._szMessage, LOG_FILE_STRING_COUNT, "%s[%s:%d:%I64d:%d]%s\r\n", strTime, pSession->m_pIpString, pSession->m_ConPort, pSession->m_i64UID, pSession->m_ui32KillReason, pLogString);
	}
	else
	{
		i3String::Format(pLogBuffer[iBufferIdx]._szMessage, LOG_FILE_STRING_COUNT, "%s[S]%s\r\n", strTime, pLogString);
	}

	pLogBuffer[iBufferIdx]._i32WarningLevel = i32WarningLevel;	// 워닝 레벨 저장

	*pi32WritePos = *pi32WritePos + 1;

#ifdef I3_DEBUG
	char strTemp[ 2048 ];
	i3String::Format( strTemp, 2048, "%s\r\n", pLogString );
	I3TRACE( strTemp );
#endif

	return; 
}

BOOL CModuleLogFile::_CreateFile(void)
{
	BOOL bRv = TRUE; 
	char strFileName[128]; 

	//Close Old File 
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		// 패킷 처리 테스트코드
		char strMessage[512]; 
		i3String::Format( strMessage, 512, "-- High parse time Protocol:%d, elepseTime:%d --\r\n", g_pUserManager->m_ui16ParseTimeHighProtocol, g_pUserManager->m_dwParseTimeHigh );
		INT32 i32Len = i3String::Length(strMessage); 
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
		g_pUserManager->m_bParseTimeReset = TRUE;

		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	//Create New File 
	i3String::Format(strFileName, 128, "%s\\%s_%04d_%02d_%02d_%02d.txt", g_pContextMain->m_strLogFilePath, "Game", g_pContextMain->m_i32Year,  g_pContextMain->m_i32Month, g_pContextMain->m_i32Day, g_pContextMain->m_i32Hour);
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
		bRv = FALSE; 
	}
	else
	{
		SetFilePointer( m_hFile, 0, NULL, FILE_END);
	}

	if(bRv)
	{
		m_ui32DateHour = g_pContextMain->m_ui32DateHour; 
		
		char strMessage[512];
		i3String::Format( strMessage, 512, 
#ifdef USE_GM_PAUSE
			"-- COMPILE DATE : Ver:%d.%d.%d.%d + GM_PAUSE, BuildDate:%s %s, BuildVer:%s --"
#else
			"-- COMPILE DATE : Ver:%d.%d.%d.%d, BuildDate:%s %s, BuildVer:%s --"
#endif
			, VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL, __DATE__, __TIME__, BUILD_VER );

		i3String::Concat( strMessage, LOCALE_INITIAL );
		i3String::Concat( strMessage, "\r\n" );
		INT32 i32Len = i3String::Length(strMessage); 
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return bRv; 
}

// SWS
void CModuleLogFile::WriteWarningLevel( INT32 i32WarningLevel )
{
	UINT32 ui32ServerTime = g_pContextMain->GetServerTime();

	//성백 : 이코드는 설마하는 코드 임. 그리고 문제 많음. 
	if ( WARNING_NO < i32WarningLevel && WARNING_NETWORK >= i32WarningLevel )  
	{
		g_pContextMain->m_aWarningInfo[ i32WarningLevel - 1 ]._ui32WriteTime = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.
	}

	return; 
}