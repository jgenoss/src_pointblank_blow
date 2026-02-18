// Config.cpp
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	Config.h 구현 파일
//
//
// Dependency: 
//		
//	

#include "pch.h"
#include "Config.h"

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
CConfig::CConfig(void) : m_ui8NationalCode(1) 
{
	m_bPhysicsActive			= FALSE;
	m_ui32PhysicsUpdateTime		= DS_PHYSICS_TIMEOUT;
	m_ui32PhysicsEpsilonTime	= 1;
	m_ui32RayCount				= 3;
	m_bBothDirection			= TRUE;
	m_ui8MultiWeaponCount		= 10;
	m_ui32UdpIp					= 0;
	m_ui32UdpActiveIp			= 0;
	m_ui32UdpPortCount			= 0;		// control server에서 받음
	m_pui16UdpPort				= NULL;		// control server에서 받음

	m_i32TaskThreadCount		= DS_DEFAULT_TASK_THREAD_COUNT; 
	m_ui32IoThreadCount			= DS_DEFAULT_IO_THREAD_COUNT; 

	// control server로부터 수신
	i3mem::FillZero( m_aui8HackCheckLevel, sizeof(m_aui8HackCheckLevel));
	
	m_i32RoomCount				= DS_DEFAULT_ROOM_COUNT;

	m_dwUpdateTime				= 61; 

	m_strLogFilePath[0]			= '\0';
	m_ui32LogLevel				= DS_DEFAULT_LOG_LEVEL;
	
	m_bControlActive			= FALSE;
	m_ui32ControlServerIP		= 0;
	m_ui16ControlServerPort		= 0;

	m_ui32CastServerIP			= 0;
	m_ui16CastServerPort		= 0;

	m_bMapDataReceived			= FALSE;
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
	I3MEM_SAFE_FREE( m_pui16UdpPort );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: LoadFromFile
//
// Description:
//	- "config.ini" 파일로부터 구성값을 읽어서 멤버 변수에 설정한다
//	- 값이 없으면 디폴트 값으로 설정하고, 잘못된 값일 경우는 검사하지 않는다
//
// Arguments:
//	- szFileName : "config.ini" 파일 경로
//		- 
// Return:
//	- TRUE/FALSE
//

BOOL CConfig::LoadConfigFile(const char * pstrFileName)
{
	const char*	pstrValue = NULL;
	INT32 Temp = 0;  

	i3IniParser Parser;															// parser object
	
	if( pstrFileName == NULL)	return FALSE;

	if( !Parser.OpenFromFile( pstrFileName ))									// file open
	{
		return FALSE;
	}

	// Default 
	if( Parser.ReadSection(_T("Default")) )										// [Default]
	{
		// logFile Path
		//strcpy_s( m_szLogFilePath, sizeof( m_szLogFilePath ), Parser.GetStringValue(_T("LogFilePath")) );		// log file path
		// logLevel
		if( !Parser.GetValue(_T("LogLevel"), &Temp) )
		{
			Temp = DS_DEFAULT_LOG_LEVEL;
		}
		m_ui32LogLevel = MIN(Temp, DS_LOG_ERROR_LEVEL_ERROR );

		// UdpIp
		char strUdpIP[ MAX_PATH ];
		strcpy_s( strUdpIP, sizeof( strUdpIP ), Parser.GetStringValue(_T("UdpIp")));					// 클라이언트가 붙을 Udp Ip
		m_ui32UdpIp			= ::inet_addr( strUdpIP );

		// UdpIpActive
		strcpy_s( strUdpIP, sizeof( strUdpIP ), Parser.GetStringValue(_T("UdpIpActive")));				// Proxy존재시 실제로 클라이언트가 붙을 Ip
		m_ui32UdpActiveIp	= ::inet_addr( strUdpIP );

		//Update Time
		if( !Parser.GetValue(_T("UpdateTime"), &Temp))						// UpdateTime
		{
			Temp = DS_TASK_TIMEOUT;
		}
		m_dwUpdateTime = (DWORD)Temp; 

		// IoThreadCount
		if( !Parser.GetValue(_T("IoThreadCount"), &Temp))					// i/o thread count
		{
			Temp = DS_DEFAULT_IO_THREAD_COUNT;
		}
		m_ui32IoThreadCount = (UINT8) Temp;

		//TaskThreadCount
		if( !Parser.GetValue(_T("TaskThreadCount"), &Temp))					// task thread count
		{
			Temp = DS_DEFAULT_TASK_THREAD_COUNT;
		}
		m_i32TaskThreadCount = (UINT8) Temp;

		// RoomCount
		if( !Parser.GetValue(_T("RoomCount"), &Temp))						// room count
		{
			Temp = DS_DEFAULT_ROOM_COUNT;
		}
		m_i32RoomCount = (UINT16)Temp;
	}
	else
	{
		Parser.Close();
		return FALSE; 
	}

	// HackCheck
	if( Parser.ReadSection("HackCheck"))
	{
		UINT32 ui32Temp = 0;

		// Hack Check Active
		if( !Parser.GetValue(_T("HACK_CHECK_ACTIVE"), &ui32Temp) )
		{
			ui32Temp = 1;
		}

		m_bHackCheck = (BOOL)ui32Temp;


		// National Code
		if( !Parser.GetValue(_T("TRANS_METHOD"), &ui32Temp) )
		{
			return FALSE; 
		}
		m_ui8NationalCode = (UINT8)ui32Temp;

		//하유종 - 이부분 설정 해주세요. LOCALE_CODE
		//I3_ASSERT_0; 

	} 
	else 
	{
		Parser.Close();
		return FALSE; 
	}

	// Raycast
	if( Parser.ReadSection( "Raycast" )) 
	{
		if( Parser.GetValue("PhysicsActive", &Temp ) == TRUE )
		{
			if(  Temp == 1 )
			{
				m_bPhysicsActive = TRUE;

				UINT32 ui32Temp = 0;

				// Update Time
				if( !Parser.GetValue(_T("PhysicsUpdateTime"), &ui32Temp) )
				{
					ui32Temp = DS_PHYSICS_TIMEOUT; 
				}
				m_ui32PhysicsUpdateTime = (UINT8)ui32Temp;

				// Epsilon Time
				if( !Parser.GetValue(_T("PhysicsEpsilonTime"), &ui32Temp) )
				{
					ui32Temp = 1; 
				}
				m_ui32PhysicsEpsilonTime = (UINT8)ui32Temp;

				// ray count
				if( !Parser.GetValue(_T("RayCount"), &ui32Temp) )
				{
					ui32Temp = 3; 
				}
				m_ui32RayCount = (UINT8)ui32Temp;

				// raycast dir
				if( !Parser.GetValue(_T("BothDirection"), &ui32Temp) )
				{
					ui32Temp = 1;
				}
				m_bBothDirection = (BOOL)ui32Temp;
			}
		}
	}
	else
	{
		Parser.Close();
		return FALSE; 
	}

	INT32	i32Temp = 0;
	if( Parser.ReadSection("Control"))
	{
		m_bControlActive = TRUE;
		pstrValue = Parser.GetStringValue("ControlServerIp");
		m_ui32ControlServerIP = ::inet_addr( pstrValue );
		Parser.GetValue( "ControlServerPort", i32Temp, &i32Temp);
		m_ui16ControlServerPort = (UINT16)i32Temp; 
	}
	else
	{
		Parser.Close();
		return FALSE;
	}

	if(LoadLogInfo(&Parser)!= SERVER_ERROR_NO)
		return FALSE;


	Parser.Close();

	return TRUE;
}

INT32 CConfig::LoadLogInfo(i3IniParser* pIniParser)
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;

	if( FALSE == pIniParser->ReadSection( "Default" ) )		goto e_Fail;
		
	pstrValue = pIniParser->GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )									goto e_Fail;

	i3String::Copy( m_strLogFilePath, pstrValue, MAX_PATH);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strLogFilePath))			goto e_Fail;
	
	return SERVER_ERROR_NO;

e_Fail:

	//// 로그파일이 없거나, 해당 폴더가 없을 경우 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	//INT32 i32Idx = 0;
	//char strDrive[ SERVER_STRING_COUNT ];
	//while( '\\' != g_pCCurrentPath[ i32Idx ] )
	//{
	//	strDrive[i32Idx] = g_pCCurrentPath[i32Idx];
	//	i32Idx++;
	//	if( SERVER_STRING_COUNT <= i32Idx )					return SERVER_ERROR_LOGFILE;
	//}
	//strDrive[i32Idx] = '\0';

	//i3String::Format( m_strLogFilePath, MAX_PATH, "%s\\%s", strDrive, "PBLog" );
	//CreateDirectory( m_strLogFilePath, NULL );

	//i3String::Concat( m_strLogFilePath, "\\Dedi" );
	//CreateDirectory( m_strLogFilePath, NULL );

	//dwResult = GetFileAttributes( m_strLogFilePath );
	//if( 0xFFFFFFFF == dwResult )							return SERVER_ERROR_LOGFILE;
	//if( 0 == (FILE_ATTRIBUTE_DIRECTORY & dwResult ) )		return SERVER_ERROR_LOGFILE;


	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	char strDrive[MAX_PATH];
	i3System::GetDrive(g_pCCurrentPath, strDrive, MAX_PATH);

	i3String::Format(m_strLogFilePath, MAX_PATH, "%s\\%s\\%s", strDrive, "PBLog", "Dedi");

	if (FALSE == i3System::PrepareDirectory(m_strLogFilePath))	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}


UINT8 CConfig::RegistPBVersion(UINT8 ui8UserPBVer)
{
	switch (g_pConfig->m_eServerVersion)
	{
	case PB_VER_V10:
		return PB_VER_V10;
	case PB_VER_V15:
		return PB_VER_V15;
	default:
		switch (ui8UserPBVer)
		{
		case PB_VER_V10:
		case PB_VER_V15:
			return ui8UserPBVer; 
		default:
			return PB_VER_V15;
		}
	}
	return PB_VER_V15;
}