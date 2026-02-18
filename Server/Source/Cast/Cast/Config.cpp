#include "pch.h"
#include "Config.h"
#include "windows.h"

#define IOCP_THREAD_COUNT			32

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

CConfig::CConfig()
: m_bTestMode(false)
{
	m_i32ServerCount				= 0;
	m_i32GameServerCount			= 0;
	m_pGameServer					= NULL;
	m_i32BattleServerCount			= 0;
	m_pBattleServer					= NULL;

	m_ui32ConnectedGameServerCount	= 0;

	m_ui32ControlIP					= 0;
	m_ui16ControlServerPort			= 0;

	m_i32ThreadCount				= IOCP_THREAD_COUNT;
}

CConfig::~CConfig(void)
{
	I3_SAFE_DELETE_ARRAY( m_pGameServer );
	I3_SAFE_DELETE_ARRAY( m_pBattleServer );
}

// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우
INT32 CConfig::LoadLogFile( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))				goto e_Fail;
	if( FALSE == Parser.ReadSection( "Default" ) )					goto e_Fail;

	pstrValue = Parser.GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )											goto e_Fail;
	
	MultiByteToWideChar( CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, SERVER_STRING_COUNT );

	// 정상적으로 로그가 생성 되었습니다.
	if( i3System::PrepareDirectory( m_wstrLogFilePath ) )			return SERVER_ERROR_NO;

e_Fail:
		
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	wchar_t wstrPath[ SERVER_STRING_COUNT ];
	MultiByteToWideChar( CP_ACP, NULL, g_pCCurrentPath, -1, wstrPath, SERVER_STRING_COUNT );

	wchar_t wstrDrive[ SERVER_STRING_COUNT ];
	i3System::GetDrive( wstrPath, wstrDrive, SERVER_STRING_COUNT );

	i3String::Format( m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Cast" );

	if( FALSE == i3System::PrepareDirectory( m_wstrLogFilePath ) )	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우



BOOL CConfig::LoadConfigFile( const char * szFileName )
{
	i3IniParser Parser;

	if( FALSE == Parser.OpenFromFile( szFileName ))							return FALSE;

	const char* pStringValue;
	INT32 i32Temp = 0; 

	if( Parser.ReadSection( "Default" ) )
	{
		pStringValue = Parser.GetStringValue( "Ip" );
		if( NULL == pStringValue )											goto e_Exit;
		m_ui32IP = ::inet_addr( pStringValue );
	
		if( FALSE == Parser.GetValue( "Port", 0, &i32Temp ) )				goto e_Exit;
		m_ui16Port = (UINT16)i32Temp;

		Parser.GetValue( "IOCPCount", IOCP_THREAD_COUNT, &i32Temp );
		m_i32ThreadCount = i32Temp;
	}
	else
	{
		goto e_Exit;
	}
		
	if( Parser.ReadSection("Control") )
	{
		pStringValue = Parser.GetStringValue( "ControlServerIp" );
		if( NULL == pStringValue )											goto e_Exit;
		m_ui32ControlIP = ::inet_addr( pStringValue );

		if( FALSE == Parser.GetValue( "ControlServerPort", 0, &i32Temp) )	goto e_Exit;
		m_ui16ControlServerPort = (UINT16)i32Temp;
	}
	else
	{
		goto e_Exit;
	}

	Parser.Close();

	return TRUE;

e_Exit:

	Parser.Close();

	return FALSE;
}

UINT32 CConfig::GetConnectedGameServerCount(void) const
{
	return m_ui32ConnectedGameServerCount;
}

void CConfig::OnUpdate(void)
{
	UINT32 iGameServerCount		= 0; 
	UINT32 ConnectServerFlag	= 0; 

	g_ConnectServer = ConnectServerFlag;
	m_ui32ActiveGameServerCount = iGameServerCount;
}

void CConfig::AddConnectedGameServerCount(void)
{
	//if (m_ui32ConnectedGameServerCount < m_ui32ServerCount)
	//{
	//	m_ui32ConnectedGameServerCount++;
	//}
}

void CConfig::SubConnectedGameServerCount(void)
{
	//if (0 < m_ui32ConnectedGameServerCount)
	//{
	//	m_ui32ConnectedGameServerCount--;
	//}
}

void CConfig::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
	{
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
	}
}

void CConfig::SetGameServerCount( INT32 i32ServerCount )
{
	m_i32GameServerCount	= i32ServerCount;
	m_pGameServer			= new SERVER_BASE[ m_i32GameServerCount ];

	m_i32ServerCount		= m_i32GameServerCount+m_i32BattleServerCount;
}

void CConfig::SetGameServerIP( INT32 i32Idx, UINT32 ui32IP )
{
	m_pGameServer[ i32Idx ].m_ui32IP = ui32IP;
}

void CConfig::SetBattleServerCount( INT32 i32ServerCount )
{
	m_i32BattleServerCount		= i32ServerCount;
	m_pBattleServer				= new SERVER_BATTLE[ m_i32BattleServerCount ];

	i3mem::FillZero( m_pBattleServer, sizeof(SERVER_BATTLE)*m_i32BattleServerCount ); 

	m_i32ServerCount			= m_i32GameServerCount+m_i32BattleServerCount;
}

void CConfig::SetBattleServerIP( INT32 i32Idx, UINT32 ui32IP )
{
	m_pBattleServer[ i32Idx ].m_ui32IP		= ui32IP;
}

INT32 CConfig::GetServerIdx( ASC_SERVER_TYPE eServerType, UINT32 ui32IP )
{
	INT32*			pi32ServerCount;
	SERVER_BASE*	pui32GameServer;
	SERVER_BATTLE*	pui32DediServer;
	switch (eServerType)
	{
	case ASC_SERVER_GAME:
	{
		pi32ServerCount = &m_i32GameServerCount;
		pui32GameServer = m_pGameServer;

		for ( INT32 i = 0; i < *pi32ServerCount; i++ )
		{
			if ( ui32IP == pui32GameServer[i].m_ui32IP )	return i;
		}
	}
	break;
	case ASC_SERVER_BATTLE:
	{
		pi32ServerCount = &m_i32BattleServerCount;
		pui32DediServer = m_pBattleServer;

		for ( INT32 i = 0; i < *pi32ServerCount; i++ )
		{
			if ( ui32IP == pui32DediServer[i].m_ui32IP )	return i;
		}
	}
	break;
	default:
	{
		return -1;
	}
	break;
	}


	return -1;
}

void CConfig::SetAllowRoomCreate( INT32 i32Idx, BOOL bActive )
{
	m_pBattleServer[ i32Idx ].m_bUseBattle = bActive;
	m_pBattleServer[i32Idx].m_i32BattleCount = 0;
}

INT32 CConfig::GetEmptyBattleServer( STAGE_UID eStageUID )
{
	if( 0 == m_i32BattleServerCount )		return -1;

	INT32 i32Idx = -1;
	INT32 i32BattleCount = INT32_MAX;
	for( INT32 i = 0; i < m_i32BattleServerCount; i++ )
	{
		if ( FALSE == m_pBattleServer[i].m_bUseBattle ) continue;
		//해당 데디서버에서 맵을 로드하였는지 확인한다.
		if ( !m_MapLoadManager.IsMapOpen( i, eStageUID ) ) continue;

		if ( m_pBattleServer[i].m_i32BattleCount < i32BattleCount )
		{
			i32BattleCount = m_pBattleServer[i].m_i32BattleCount;
			i32Idx = i;
		}
	}

	if ( -1 != i32Idx )
	{
		m_pBattleServer[i32Idx].m_i32BattleCount++;
	}
	
	return i32Idx;
}

void CConfig::FreeBattleServer( INT32 i32Idx )
{
	if (i32Idx < 0)								return;
	if (i32Idx >= m_i32BattleServerCount)		return;

	if( 0 < m_pBattleServer[ i32Idx ].m_i32BattleCount )
	{
		m_pBattleServer[ i32Idx ].m_i32BattleCount--;
	}
}

void CConfig::SetBattleRoomCount( INT32 i32ServerIdx, INT32 i32RoomCount )
{
	if (i32ServerIdx < 0)								return;
	if (i32ServerIdx >= m_i32BattleServerCount)			return;

	m_pBattleServer[ i32ServerIdx ].m_i32BattleCount = i32RoomCount;
}