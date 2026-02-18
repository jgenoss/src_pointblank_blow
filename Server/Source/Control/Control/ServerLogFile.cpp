#include "pch.h"
#include "ServerLogFile.h"
#include "UserSession.h"

I3_CLASS_INSTANCE(CServerLogFile, i3Thread);

CServerLogFile * g_pServerLogFile = NULL;

CServerLogFile::CServerLogFile(void)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_i32ServerLogSaveTime = 0;
	m_bRunning	= FALSE;
}

CServerLogFile::~CServerLogFile(void)
{
	OnDestroy(); 
}

BOOL CServerLogFile::OnCreate(void)
{
	BOOL bRv;
	bRv = FileOpen();
		
	//Create Thread 
	if( !i3Thread::Create("CServiceLogFile", 0, 4096, NULL, PRIORITY_NORMAL) )return FALSE;
	
	return bRv;
}

BOOL CServerLogFile::FileOpen(void)
{
	BOOL bRv = TRUE;
	char strFileName[256];
	
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
	} 
	
	//Create New File 	
	UINT32 ui32Date = (i3ThreadTimer::LocalTime().GetYear() * 1000000) + ( i3ThreadTimer::LocalTime().GetMonth() * 10000) + (i3ThreadTimer::LocalTime().GetDay() * 100) + i3ThreadTimer::LocalTime().GetHour();
	i3String::Format(strFileName, 256, "%s\\%d.txt", g_pControl->m_strServerLogFilePath, ui32Date );
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

	m_i32Hour = i3ThreadTimer::LocalTime().GetHour();


	return bRv;
}

void CServerLogFile::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	if(m_hFile == INVALID_HANDLE_VALUE)
	{	
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

void CServerLogFile::OnUpdate(void)
{
	return;
}

UINT32 CServerLogFile::OnRunning( void * pUserData )
{
	m_i32ServerLogIdx = g_pLog->AddLogThread(L"SERVERLOG", 0, m_ThreadID);
	
	m_bRunning = TRUE;

	while( m_bRunning )
	{
		if( (m_i32ServerLogSaveTime + g_pControl->m_ui32UpdateTime) <= i3ThreadTimer::GetServerTime() )
		{
			WriteServerLog();
			m_i32ServerLogSaveTime = i3ThreadTimer::GetServerTime();
		}
		
		::WaitForSingleObject( m_Handle, 500 );
	}
	return 1;
}

void CServerLogFile::WriteServerLog(void)
{
	if( i3ThreadTimer::LocalTime().GetHour() != m_i32Hour )
	{
		FileOpen();
	}
	
	char strMessage[MAX_LOG_STRING_COUNT];
	if( m_hFile == INVALID_HANDLE_VALUE)
	{
		g_pLog->WriteLog( L"WriteServerLog File Error"); 
		return; 
	}
	i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "[%d:%d]\r\n", i3ThreadTimer::LocalTime().GetMinute(), i3ThreadTimer::LocalTime().GetSecond() );
	INT32 i32Len = i3String::Length(strMessage);
	::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL);

	char strName[256];
	for(UINT32 i = 0; i < g_pControl->m_ui32CommonServerCount; i++)
	{
		SERVER_INFO_COMMON * pList = &g_pControl->m_aCommonServerList[i];
		switch(pList->_ui8Type)
		{
		case ASC_SERVER_SIA			: i3String::Copy(strName, "SI", 256); break;
		case ASC_SERVER_TRANS		: i3String::Copy(strName, "TR", 256); g_pControl->SetNowCCU( pList->_i32Arg1 ); break;
		case ASC_SERVER_CLAN		: i3String::Copy(strName, "CS", 256); break;
		case ASC_SERVER_MESSENGER	: i3String::Copy(strName, "MS", 256); break;
		case ASC_SERVER_UPDATER		: i3String::Copy(strName, "UP", 256); break;
		case ASC_SERVER_ZLOG		: i3String::Copy(strName, "LG", 256); break;		
		default						: i3String::Copy(strName, "??", 256); break;
		}

		if(pList->_i32IsActive)
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "%s O[%d:%d][%d-%d]\r\n", strName, pList->_i32Cpu, pList->_i32Handle, pList->_i32Arg0, pList->_i32Arg1);
		else 
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "%s X[]\r\n", strName);

		i32Len = i3String::Length(strMessage);
		::WriteFile( m_hFile, strMessage, static_cast<DWORD>(i32Len), (LPDWORD)&i32Len, NULL );
	}


	for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
	{
		if(g_pControl->m_aGameServerList[i]._i32IsActive)
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "GS O[%d:%d][%d-%d]\r\n", g_pControl->m_aGameServerList[i]._i32Cpu, g_pControl->m_aGameServerList[i]._i32Handle, g_pControl->m_aGameServerList[i]._i32Arg0, g_pControl->m_aGameServerList[i]._i32Arg1);
		else 
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "GS X[]\r\n");

		i32Len = i3String::Length(strMessage);
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	for(UINT32 i = 0; i < g_pControl->m_ui32BattleServerCount; i++)
	{
		if(g_pControl->m_aBattleServerList[i]._i32IsActive)
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "BT O[%d:%d][%d-%d]\r\n", g_pControl->m_aBattleServerList[i]._i32Cpu, g_pControl->m_aBattleServerList[i]._i32Handle, g_pControl->m_aBattleServerList[i]._i32Arg0, g_pControl->m_aBattleServerList[i]._i32Arg1);
		else 
			i3String::Format( strMessage, MAX_LOG_STRING_COUNT, "BT X[]\r\n");

		i32Len = i3String::Length(strMessage);
		::WriteFile( m_hFile, strMessage, i32Len, (LPDWORD)&i32Len, NULL );
	}

	return;
}
