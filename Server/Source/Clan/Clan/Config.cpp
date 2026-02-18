#include "pch.h"
#include "Config.h"
#include "Gateway.h"
#include "Receiver.h"
#include "windows.h"

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

CConfig::CConfig()
: m_bTestMode(false)
{
	m_pui32ServerIp				= NULL;

	m_ui32ConnectedGameServerCount	= 0;
	m_ui32ClanCount				= 0;

	m_ui32ClanMax				= 0;
	m_ui8CreateMemberCount		= BASIC_MEMBER_COUNT;

	m_ui32ControlIP				= 0;
	m_ui16ControlServerPort		= 0;

	m_bAzitActive				= FALSE;

}

CConfig::~CConfig(void)
{
	I3MEM_SAFE_FREE( m_pui32ServerIp );
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

	i3String::Format( m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Clan" );

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
		m_ui32IPAddress = ::inet_addr( pStringValue );
	
		if( FALSE == Parser.GetValue( "Port", 0, &i32Temp ) )				goto e_Exit;
		m_ui16Port = (UINT16)i32Temp;
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

UINT32 CConfig::GetClanCount(void) const
{
	return m_ui32ClanCount;
}

void CConfig::OnUpdate(void)
{
	UINT32 iGameServerCount		= 0; 
	UINT32 ConnectServerFlag	= 0; 
	for( INT32 i = 0; i < static_cast<INT32>(m_ui32ServerCount); i++ )
	{
		if(g_pGateway->GetReceiver( i )->IsWork())
		{
			iGameServerCount++;
			ConnectServerFlag = ConnectServerFlag | (0x01 << i);
		}
	}

	g_ConnectServer = ConnectServerFlag;
	m_ui32ActiveGameServerCount = iGameServerCount;
}

void CConfig::AddConnectedGameServerCount(void)
{
	if (m_ui32ConnectedGameServerCount < m_ui32ServerCount)
	{
		m_ui32ConnectedGameServerCount++;
	}
}

void CConfig::SubConnectedGameServerCount(void)
{
	if (0 < m_ui32ConnectedGameServerCount)
	{
		m_ui32ConnectedGameServerCount--;
	}
}

void CConfig::SetClanCount(UINT32 ui32Count)
{
	m_ui32ClanCount = ui32Count;
}

void CConfig::InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
		return;

	m_ClanMatchSeason[eSeasonType].SetSeason(pClanMatchSeason);
}

void CConfig::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
}

bool CConfig::CheckGameServerIdx(UINT32 ui32ServerIdx)
{
	if( 0 == ui32ServerIdx  ) return false;
	if( ui32ServerIdx > GetServerCount() ) return false;
	return true;
}

UINT64 CConfig::MakeStateMatch(UINT64 ui64State, T_TeamID TTeamID)
{
	UINT64 ui64state = GET_FRIEND_STATE(ui64State);
	UINT64 ui64Server = GET_FRIEND_SERVER(ui64State);
	UINT64 ui64Channel = GET_FRIEND_CHANNEL(ui64State);
	UINT64 ui64Room = GET_FRIEND_ROOMIDX(ui64State);

	return MAKE_FRIEND_MATCH_STATE( TTeamID, ui64state, ui64Server, ui64Channel, ui64Room );
}