// Config.cpp
//
// YouJong Ha
//	Last update : 2011-10-07 (yyyy:mm:dd)
//	
// Description:
//	Config.h 구현 파일
//
//
// Dependency: 
//		
//	

#include "pch.h"

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

///////////////////////////////////////////////////////////////////////////////
//
// Function: CConfig
//
// Description:
//	- constructor
//		- 
// Arguments:
// Return:
//	- 
CConfig::CConfig(void)
{
	m_LocalTime					= 0.0f; 
	m_iServerTime				= 10000; //계산을 위해서 이 값부터 시작... UINT32 때문
	m_ui32UdpIp					= 0;
	m_ui16UdpPort				= 0;		
	m_i32TaskThreadCount		= STAGE_ORD_MAX - 1;
	m_ui32UpdateTime			= PX_TASK_TIMEOUT;
	m_ui32EpsilonTime			= PX_TASK_EPSILON_TIME;
	
	m_ui32BattleServerCount		= 0;
	memset( &m_ui32BattleServerIpList, 0, sizeof(m_ui32BattleServerIpList));

	m_szLogFilePath[0]			= '\0';
	m_ui32LogLevel				= 0;
	m_ui32RayCount				= 0;

	RenewTime();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CConfig
//
// Description:
//	- destructor
//		- 
// Arguments:
// Return:
//	- 
CConfig::~CConfig(void)
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: LoadFromFile
//
// Description:
//	- "config.ini" 파일로부터 구성값을 읽어서 멤버 변수에 설정한다
//		- 
// Arguments:
//	- szFileName : "config.ini" 파일 경로
//		- 
// Return:
//	- TRUE/FALSE
//
BOOL CConfig::LoadFromFile(const char * szFileName)
{
	i3IniParser Parser;														// parser object

	INT32 Temp			= 0;
	char  szItem[16]	= { '\0', };
	
	if( szFileName == NULL)	return FALSE;

	if( FALSE == Parser.OpenFromFile( szFileName))							// file open
	{		
		return FALSE;
	}
	
	// Default 
	if( Parser.ReadSection(_T("Default")) )									// [Default]
	{
		// logFile Path
		strcpy_s( m_szLogFilePath, sizeof( m_szLogFilePath ), Parser.GetStringValue( "LogFilePath" ) );		// log file path
		// logLevel
		if( FALSE == Parser.GetValue( "LogLevel", &Temp) )
		{
			Temp = PX_DEFAULT_LOG_LEVEL;
		}
		m_ui32LogLevel = Temp;
		// UdpIp
		char szUdpIP[ MAX_PATH ] = { '\0', };
		strcpy_s( szUdpIP, sizeof( szUdpIP ), Parser.GetStringValue(_T("UdpIp")));					// 클라이언트가 붙을 Udp Ip
		m_ui32UdpIp			= ::inet_addr( szUdpIP );
		// UdpPort
		if( FALSE == Parser.GetValue( "UdpPort", &Temp) )
		{
			m_ui16UdpPort = PX_DEFAULT_PORT_BASE;
		}
		m_ui16UdpPort = (UINT16)Temp;
		
		// Timing
		if( FALSE == Parser.GetValue( "UpdateTime", &Temp) )
		{
			m_ui32UpdateTime = PX_TASK_TIMEOUT;
		}
		m_ui32UpdateTime = Temp;

		if( FALSE == Parser.GetValue( "EpsilonTime", &Temp ))
		{
			m_ui32EpsilonTime = PX_TASK_EPSILON_TIME;
			
		}
		m_ui32EpsilonTime = Temp;
	}
	else
	{
		Parser.Close();
		return FALSE; 
	}

	// battle serverlist
	if( Parser.ReadSection(_T("BattleServerList")) )
	{
		if( FALSE == Parser.GetValue( "Count", &Temp))					// battle server count
		{
			Temp = DEF_SERVER_COUNT;
		}
		m_ui32BattleServerCount = Temp;

		if( m_ui32BattleServerCount > MAX_SERVER_COUNT )
		{
			Parser.Close();
			return FALSE; 
		}

		for( UINT32 i = 0; i < m_ui32BattleServerCount; i++ )
		{
			sprintf_s(szItem, sizeof(szItem), "%dIP", i);
			const char* ip = Parser.GetStringValue( szItem );
			if( ip )
			{
				DSMSG("Dedicated Server[%d]:%s\n", i, ip );
				m_ui32BattleServerIpList[i] = inet_addr( ip );
			}
			else
			{
				m_ui32BattleServerIpList[i] = inet_addr( DEF_SERVER_IP );
			}
		}
	}
	else
	{
		Parser.Close();
		return FALSE; 
	}

	// raycast
	if( Parser.ReadSection(_T("Raycast")) )
	{
		if( FALSE == Parser.GetValue( "RayCount", &Temp)) 
		{
			Temp = DEF_RAY_COUNT;
		}
		
		if( Temp != 1 && Temp != 3 && Temp != 5)
		{
			Parser.Close();
			return FALSE; 
		}
		m_ui32RayCount = Temp;
	}
	
	Parser.Close();
	return TRUE;
}

BOOL	CConfig::IsValidBattleServer( UINT32 ui32Ip )
{
	if( ui32Ip == 0)
	{
		DSERROR("ui32Ip==0\n");
		return FALSE;
	}

	for( UINT32 i = 0; i < m_ui32BattleServerCount; i++ )
	{
		if( (m_ui32BattleServerIpList[i] - ui32Ip) == 0 ) 
			return TRUE;
	}
	return FALSE;
}