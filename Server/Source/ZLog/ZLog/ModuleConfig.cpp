#include "pch.h"
#include "ModuleConfig.h"
#include <windows.h>

CModuleConfig * g_pConfig = nullptr;

CModuleConfig::CModuleConfig()
{
	m_ui32ServerCount		= 0;
	m_pui32ServerIP			= nullptr;

	m_ui32ControlIP			= 0;
	m_ui16ControlPort		= 0;

	m_ui32Ip				= 0;
	m_ui16Port				= 0;
	m_ui8WriteDBThreadCount = 0;
}

CModuleConfig::~CModuleConfig()
{
	OnDestroy();
}

// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우
INT32 CModuleConfig::LoadLogInfo( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = nullptr;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))				goto e_ZLogFail;
	if( FALSE == Parser.ReadSection( "Default" ) )					goto e_ZLogFail;

	// 1. ZLog Log
	pstrValue = Parser.GetStringValue( "PATH_LOG" );
	if( NULL == pstrValue )											goto e_ZLogFail;

	MultiByteToWideChar(CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, SERVER_STRING_COUNT);
	
	// 정상적으로 로그가 생성 되었습니다.
	if( ! i3System::PrepareDirectory(m_wstrLogFilePath) )			goto e_ZLogFail;


	// 2. Service Log
	pstrValue = Parser.GetStringValue("PATH_SYSTEM_LOG");
	if (NULL == pstrValue)											goto e_ServiceFail;
	i3String::Copy(m_strServiceLogFilePath, pstrValue, MAX_PATH);
	
	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strServiceLogFilePath))		goto e_ServiceFail;


	return SERVER_ERROR_NO;

e_ZLogFail:
		
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	wchar_t wstrPath[SERVER_STRING_COUNT];
	MultiByteToWideChar( CP_ACP, NULL, g_strCCurrentPath, -1, wstrPath, SERVER_STRING_COUNT);

	wchar_t wstrDrive[SERVER_STRING_COUNT];
	i3System::GetDrive( wstrPath, wstrDrive, SERVER_STRING_COUNT);

	i3String::Format(m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"ZLog" );

	if( FALSE == i3System::PrepareDirectory(m_wstrLogFilePath) )	return SERVER_ERROR_LOGFILE;


e_ServiceFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	char strDrive[MAX_PATH];
	i3System::GetDrive(g_strCCurrentPath, strDrive, MAX_PATH);

	i3String::Format(m_strServiceLogFilePath, MAX_PATH, "%s\\%s\\%s", strDrive, "PBLog", "ZLOG_SERVICE");

	if (FALSE == i3System::PrepareDirectory(m_strServiceLogFilePath))	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우

BOOL CModuleConfig::LoadConfigFile( char* strFileName )
{
	i3IniParser Parser;

	if( FALSE == Parser.OpenFromFile( strFileName ))										return FALSE;

	const char* pStringValue;
	INT32 i32Temp = 0; 
	
	if( Parser.ReadSection( "Default" ) )
	{
		pStringValue = Parser.GetStringValue( "NET_IP" );
		if( NULL == pStringValue )															goto e_Exit;	
		m_ui32Ip = ::inet_addr(pStringValue); 

		if( FALSE == Parser.GetValue( "NET_PORT", 0, &i32Temp ) )							goto e_Exit;
		m_ui16Port = (UINT16)i32Temp;

		Parser.GetValue( "WriteDB_Thread_Count", 16, &i32Temp );
		m_ui8WriteDBThreadCount = (UINT8)i32Temp;
	}
	else
	{
		goto e_Exit;
	}

	if( Parser.ReadSection( "Control" ) )
	{
		pStringValue = Parser.GetStringValue("Control_IP"); 
		if( NULL == pStringValue )															goto e_Exit;
		m_ui32ControlIP = ::inet_addr( pStringValue );
		
		if( FALSE == Parser.GetValue( "Control_PORT", 0, &i32Temp ) )						goto e_Exit;
		m_ui16ControlPort = (UINT16)i32Temp;
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

void CModuleConfig::OnDestroy()
{
	I3_SAFE_DELETE_ARRAY( m_pui32ServerIP );
}