#include "pch.h"
#include "Config.h"

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3Thread);

CConfig::CConfig(void)
{
	m_bRunning			= FALSE;
	m_i32LogThreadIdx	= LOG_THREAD_INVALID;
	m_i32UpdateMain		= 100;
}

CConfig::~CConfig(void)
{
	OnDestroy();
}

BOOL CConfig::OnCreate( char* strFileName, char* strReLoadFileName )
{
	char strDrive[4];
	char strFile[MAX_PATH];
	char strDir[MAX_PATH];
	char strExt[MAX_PATH];

	char strCorFileName[ SERVER_STRING_COUNT ];

	_CorrectPath( strFileName, SERVER_STRING_COUNT, strCorFileName );
	_CorrectPath( strReLoadFileName, SERVER_STRING_COUNT, m_strReFileName );

	_tsplitpath_s( m_strReFileName, strDrive, 4, strDir, MAX_PATH, strFile, MAX_PATH, strExt, MAX_PATH);
	i3String::Format( m_strReDirName, SERVER_STRING_COUNT, "%s%s", strDrive, strDir );
	
	if( FALSE == _LoadFromFile( strCorFileName ) )			return FALSE;
	if( FALSE == _LoadReFromFile( m_strReFileName ) )		return FALSE;

	return TRUE;
}

BOOL CConfig::OnCreateThread()
{
	m_i32LogThreadIdx		= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )			return FALSE;

	if( FALSE == i3Thread::Create( "Config" ) )				return FALSE;

	return TRUE;
}

void CConfig::OnDestroy()
{
	m_bRunning = FALSE;
	FindCloseChangeNotification( m_hChangeEvnet );
	i3Thread::WaitForFinish();
}

UINT32 CConfig::OnRunning( void* pUserData )
{
	m_bRunning = TRUE;
	while( m_bRunning )
	{
		m_hChangeEvnet = FindFirstChangeNotification( m_strReDirName, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE );
		if( INVALID_HANDLE_VALUE == m_hChangeEvnet )
		{
			WriteLog( m_i32LogThreadIdx, "[CONF] FindFirstChangeNotification Fail" );

			::WaitForSingleObject( m_Handle, 1 );
			continue;
		}

		::WaitForSingleObject( m_hChangeEvnet, INFINITE );
		if( FALSE == m_bRunning ) break;

		::WaitForSingleObject( m_Handle, 30000 );
		// Debug 모드일때 폴더에 있는 svn 으로 인해 가끔씩 ChangeEvent 가 발생할때가 있습니다.

		if( _LoadReFromFile( m_strReFileName ) )
		{
			WriteLog( m_i32LogThreadIdx, "[CONF] Re Config File Open Success" );
		}
		else
		{
			WriteLog( m_i32LogThreadIdx, "[CONF] Re Config File Open Fail" );
		}

		FindNextChangeNotification( m_hChangeEvnet );
	}
	return 0;
}

BOOL CConfig::_LoadFromFile( const char* strFileName )
{
	if( strFileName == NULL)	return FALSE;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName) )
	{		
		return FALSE;
	}

	char strTemp[ SERVER_STRING_COUNT ];
	INT32 i32Temp	= 0;
	const char* pString	= NULL;
	////////////////////////////////////////////////////////////
	// Default 
	if( FALSE == Parser.ReadSection( "Default" ) )						goto e_exit;
		
	pString = Parser.GetStringValue( "ServerIp" );
	if( NULL == pString  )												goto e_exit;
	i3String::Copy( m_strIP, pString, IP_NAME_COUNT);
	m_ui32IP = ::inet_addr( m_strIP );

	if( FALSE == Parser.GetValue( "ServerPort", 0, &i32Temp ) )			goto e_exit;
	m_ui16Port = (UINT16)i32Temp;

	pString = Parser.GetStringValue( "ServerLogFile" );
	if( NULL == pString  )												goto e_exit;
	i3String::Copy( m_strLogFilePath, pString, SERVER_STRING_COUNT );

	////////////////////////////////////////////////////////////
	// Link Server
	if( FALSE == Parser.ReadSection( "Link" ) )							goto e_exit;

	if( FALSE == Parser.GetValue( "LinkMethod", 0, &i32Temp) )			goto e_exit;
	m_eLinkMethod = (LINK_METHOD)i32Temp;
	if( LINK_METHOD_NO > m_eLinkMethod ||
		LINK_METHOD_MAX <= m_eLinkMethod )								goto e_exit;

	////////////////////////////////////////////////////////////
	// Connect
	if( FALSE == Parser.ReadSection( "Connect" ) )						goto e_exit;

	if( FALSE == Parser.GetValue( "ServerCount", 0, &i32Temp) )			goto e_exit;
	m_i32ServerCount = i32Temp;

	m_pui32ServerIp = (UINT32*)i3MemAlloc(sizeof(UINT32)*m_i32ServerCount);
	if( NULL == m_pui32ServerIp )										goto e_exit;

	for( INT32 i = 0 ; i < m_i32ServerCount ; i++ )
	{
		i3String::Format( strTemp, SERVER_STRING_COUNT, "ServerIP_%02d", i );
		pString = Parser.GetStringValue( strTemp );
		if( NULL == pString  )											goto e_exit;
		m_pui32ServerIp[i] = ::inet_addr( pString );
	}

	////////////////////////////////////////////////////////////
	// Control Server
	if( FALSE == Parser.ReadSection( "Control" ) )						goto e_exit;
	
	pString = Parser.GetStringValue( "ControlServerIp" );
	if( NULL == pString  )												goto e_exit;
	m_ui32ControlServerIP = ::inet_addr( pString );

	if( FALSE == Parser.GetValue( "ControlServerPort", 0, &i32Temp) )	goto e_exit;
	m_ui16ControlServerPort = (UINT16)i32Temp; 

	/*
	//LogD
	if( Parser.ReadSection("LogD"))
	{
		Parser.GetValue( "LogDActive", Temp, &Temp);
		if( Temp == 0)	m_bLogDActive = FALSE; 
		else			m_bLogDActive = TRUE; 

		strcpy(m_LogDServerIp, Parser.GetStringValue("LogDServerIp"));
		Parser.GetValue( "LogDServerPort", Temp, &Temp);
		m_LogDServerPort = (UINT16)Temp;
	}else return FALSE; 

	*/

e_exit:

	Parser.Close();

	return TRUE;
}

BOOL CConfig::_LoadReFromFile( const char* strFileName )
{
	if( strFileName == NULL)	return FALSE;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ) )
	{		
		return FALSE;
	}

	Parser.Close();

	return TRUE;
}

void CConfig::_CorrectPath( char* strIn, INT32 i32Size, char* strOut )
{
	INT32 i32MaxSize = i32Size-1;
	INT32 i32Count = 0;
	while( '\0' != *strIn && i32Count < i32MaxSize )
	{
		*strOut = *strIn;

		if( '/' == *strOut )
		{
			*strOut = '\\';
		}
		strIn++;
		strOut++;
		i32Count++;
	}
	*strOut = '\0';
}