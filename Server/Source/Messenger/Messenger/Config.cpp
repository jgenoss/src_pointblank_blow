#include "pch.h"
#include "Gateway.h"
#include "Receiver.h"
#include "Config.h"

#include <windows.h>

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

CConfig::CConfig(void)
{
	m_pui32ServerIp				= NULL;

	m_i32MacAddressListCount	= 0;
	m_pui64MacAddressList		= NULL;

	m_ui32connectedGameServerCount	= 0;

	m_strControlServerIp[0]		='\0';
	m_ui16ControlServerPort		= 0;

	m_i32UserMaxCount = 0;
}

CConfig::~CConfig(void)
{
	I3MEM_SAFE_FREE( m_pui64MacAddressList );
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

	i3String::Format( m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Messenger" );

	if( FALSE == i3System::PrepareDirectory( m_wstrLogFilePath ) )	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우



BOOL CConfig::LoadConfigFile(const char * pstrFileName)
{
	i3IniParser Parser;

	INT32 i32Temp = 0;  
	const char* pStringValue;

	if( FALSE == Parser.OpenFromFile( pstrFileName) )		return FALSE;

	if( Parser.ReadSection("Default") )
	{
		pStringValue = Parser.GetStringValue("Ip");
		if(NULL == pStringValue)							return FALSE;
		i3String::Copy( m_strIpAddress, pStringValue, IP_NAME_COUNT);

		Parser.GetValue( "Port",	i32Temp,	&i32Temp);
		m_ui16Port = (UINT16)i32Temp;
	}
	else
	{
		return FALSE;
	}

	//Control
	if( Parser.ReadSection("Control"))
	{
		m_bControlActive = TRUE;

		pStringValue = Parser.GetStringValue("ControlServerIp");
		if( NULL == pStringValue )							return FALSE;
		i3String::Copy(m_strControlServerIp, pStringValue, IP_NAME_COUNT);

		Parser.GetValue( "ControlServerPort", 0, &i32Temp);
		m_ui16ControlServerPort = (UINT16)i32Temp;
	}
	else
	{
		return FALSE;
	}

	// 개발용. 절대 퍼블리셔에 배포하면 안됩니다.
	if( Parser.ReadSection("Dev"))
	{
		Parser.GetValue( "UC", 0, &m_i32UserMaxCount );
	} 

	Parser.Close();

	return TRUE;
}

UINT32 CConfig::GetConnectedGameServerCount(void) const

{
	return m_ui32connectedGameServerCount;
}

void CConfig::AddConnectedGameServer(void)
{
	if (m_ui32connectedGameServerCount < m_ui32ServerCount)
	{
		m_ui32connectedGameServerCount++;
	}
}

void CConfig::SubConnectedGameServer(void)
{
	if (0 < m_ui32connectedGameServerCount)
	{
		m_ui32connectedGameServerCount--;
	}
}

void CConfig::OnUpdate(void)
{
	UINT32 ui32GameServerCount		= 0;
	UINT32 ui32ConnectServerFlag	= 0;

	for(UINT32 i = 0; i < m_ui32ServerCount; i++)
	{
		if(g_pGateway->GetReceiver(i)->IsWork())
		{
			ui32GameServerCount++;
			ui32ConnectServerFlag = ui32ConnectServerFlag | (0x01 << i);
		}
	}

	g_ui32ConnectServer = ui32ConnectServerFlag;
	m_ui32connectedGameServerCount = ui32GameServerCount;
	return;
}

