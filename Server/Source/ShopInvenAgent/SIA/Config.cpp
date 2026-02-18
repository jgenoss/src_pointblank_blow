#include "pch.h"
#include "Config.h"

CConfig * g_pConfig = NULL;

CConfig::CConfig()
{
	m_pServerIP				= NULL;

	m_ServiceIdForRussia	= 0;

	m_ui8LoginMethodCount	= 0;
	m_ui8BillingMethodCount = 0;
}

CConfig::~CConfig()
{
	I3_SAFE_DELETE_ARRAY( m_pServerIP ); 
}

// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우
INT32 CConfig::LoadLogInfo( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))				goto e_Fail;
	if( FALSE == Parser.ReadSection( "Default" ) )					goto e_Fail;

	pstrValue = Parser.GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )											goto e_Fail;

	MultiByteToWideChar(CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, PATH_STRING_COUNT);
	
	// 정상적으로 로그가 생성 되었습니다.
	if( i3System::PrepareDirectory(m_wstrLogFilePath) )			return SERVER_ERROR_NO;

e_Fail:
		
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	wchar_t wstrPath[ SERVER_STRING_COUNT ];
	MultiByteToWideChar( CP_ACP, NULL, g_strCCurrentPath, -1, wstrPath, SERVER_STRING_COUNT );

	wchar_t wstrDrive[ SERVER_STRING_COUNT ];
	i3System::GetDrive( wstrPath, wstrDrive, SERVER_STRING_COUNT );

	i3String::Format(m_wstrLogFilePath, PATH_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"SIA" );

	if( FALSE == i3System::PrepareDirectory(m_wstrLogFilePath) )	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우

BOOL CConfig::LoadConfigFile( const char * strFileName )
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

		m_i32ServerCount = 1;
		m_pServerIP = new UINT32[ m_i32ServerCount ]; 
		pStringValue = Parser.GetStringValue( "TRANS_IP" );
		if( NULL == pStringValue )															goto e_Exit;
		m_pServerIP[ 0 ] = ::inet_addr( pStringValue ); 
	}
	else
	{
		goto e_Exit;
	}

	if( Parser.ReadSection( "Link" ) )
	{
		if( FALSE == Parser.GetValue( "LoginThreadCount",	1,		&m_i32LoginThreadCount ) )		goto e_Exit;
		if( FALSE == Parser.GetValue( "LoginBufferCount",	100,	&m_i32LogInBufferCount ) )		goto e_Exit;

		m_i32LoginThreadCount = MIN(m_i32LoginThreadCount, MAX_LINK_THREAD_COUNT);
		m_i32LogInBufferCount = MIN(m_i32LogInBufferCount, MAX_LINK_BUFFER_COUNT);
		
		Parser.GetValue( "LoginMethodCount", 0, &i32Temp );
		
		if ( i32Temp )
		{
			m_ui8LoginMethodCount = static_cast<UINT8>(i32Temp);

			for( UINT8 i = 0; i < m_ui8LoginMethodCount; ++i )
			{
				char strTemp[MAX_PATH];

				i3String::Format(strTemp, MAX_PATH, "LoginMethod%02d", i);
				Parser.GetValue( strTemp, -1, &i32Temp );
				if ( i32Temp < 0 )	goto e_Exit;
				m_aeLoginMethod[i] = static_cast<LINK_METHOD>(i32Temp);

				i3String::Format(strTemp, MAX_PATH, "LoginHttp%02d", i);
				pStringValue = Parser.GetStringValue(strTemp);
				if ( pStringValue )	strcpy_s( m_astrLoginHttp[i], PATH_STRING_COUNT, pStringValue );

				i3String::Format(strTemp, MAX_PATH, "LoginHttpMethod%02d", i);
				Parser.GetValue(strTemp, 0, &i32Temp);
				m_aui8LoginHttpMethod[i] = static_cast<UINT8>(i32Temp);

				i3String::Format(strTemp, MAX_PATH, "LoginTcpIP%02d", i);
				pStringValue = Parser.GetStringValue(strTemp);
				if ( pStringValue )	m_aui32LoginIp[i] = ::inet_addr( pStringValue );

				i3String::Format(strTemp, MAX_PATH, "LoginTcpPort%02d", i);
				Parser.GetValue(strTemp, 0, &i32Temp);
				m_aui16LoginPort[i] = static_cast<UINT16>(i32Temp);
			}
		}
		else
		{//구 ini 로드루틴 입니다.(라이브 서비스 버전이므로 부득이하게 처리)
			m_ui8LoginMethodCount = 1;

			if (FALSE == Parser.GetValue("LoginMethod", 0, &i32Temp))					goto e_Exit;
			m_aeLoginMethod[0] = (LINK_METHOD)i32Temp;

			pStringValue = Parser.GetStringValue("LoginHttp");
			if ( pStringValue )	i3String::Copy(m_astrLoginHttp[0], pStringValue, PATH_STRING_COUNT);

			Parser.GetValue("LoginHttpMethod", 0, &i32Temp);
			m_aui8LoginHttpMethod[0] = (UINT8)i32Temp;

			pStringValue = Parser.GetStringValue("LoginTcpIP");
			if ( pStringValue )	m_aui32LoginIp[0] = ::inet_addr(pStringValue);

			Parser.GetValue("LoginTcpPort", 0, &i32Temp);
			m_aui16LoginPort[0] = (UINT16)i32Temp;
		}


		if( FALSE == Parser.GetValue( "BillingThreadCount", 1, &m_i32BillingThreadCount ) )		goto e_Exit;
		if( FALSE == Parser.GetValue( "BillingBufferCount", 100, &m_i32BillingBufferCount) )		goto e_Exit;

		m_i32BillingThreadCount = MIN(m_i32BillingThreadCount, MAX_LINK_THREAD_COUNT);
		m_i32BillingBufferCount = MIN(m_i32BillingBufferCount, MAX_LINK_BUFFER_COUNT);

		Parser.GetValue( "BillingMethodCount", 0, &i32Temp );

		if( i32Temp )
		{
			m_ui8BillingMethodCount = static_cast<UINT8>( i32Temp );

			for (UINT8 i = 0; i < m_ui8BillingMethodCount; ++i)
			{
				char strTemp[MAX_PATH];

				i3String::Format(strTemp, MAX_PATH, "BillingMethod%02d", i);
				Parser.GetValue(strTemp, -1, &i32Temp);
				if (i32Temp < 0)	goto e_Exit;
				m_aeBillingMethod[i] = static_cast<LINK_METHOD>(i32Temp);

				i3String::Format(strTemp, MAX_PATH, "BillingHttp%02d", i);
				pStringValue = Parser.GetStringValue(strTemp);
				if (pStringValue)	strcpy_s(m_astrBillingHttp[i], PATH_STRING_COUNT, pStringValue);
				
				i3String::Format(strTemp, MAX_PATH, "BillingTcpIP%02d", i);
				pStringValue = Parser.GetStringValue(strTemp);
				if (pStringValue)	m_aui32BillingIp[i] = ::inet_addr(pStringValue);

				i3String::Format(strTemp, MAX_PATH, "BillingTcpPort%02d", i);
				Parser.GetValue(strTemp, 0, &i32Temp);
				m_aui16BillingPort[i] = static_cast<UINT16>(i32Temp);
			}
		}
		else
		{//구 ini 로드루틴 입니다.(라이브 서비스 버전이므로 부득이하게 처리)
			m_ui8BillingMethodCount = 1;

			if (FALSE == Parser.GetValue("BillingMethod", 0, &i32Temp))					goto e_Exit;
			m_aeBillingMethod[0] = (LINK_METHOD)i32Temp;

			pStringValue = Parser.GetStringValue("BillingHttp");
			if (pStringValue)	i3String::Copy(m_astrBillingHttp[0], pStringValue, PATH_STRING_COUNT);

			pStringValue = Parser.GetStringValue("BillingTcpIP");
			if (pStringValue)	m_aui32BillingIp[0] = ::inet_addr(pStringValue);

			Parser.GetValue("BillingTcpPort", 0, &i32Temp);
			m_aui16BillingPort[0] = (UINT16)i32Temp;
		}
		
		Parser.GetValue( "PCCafeThreadCount",	1,		&m_i32PCCafeThreadCount );		
		Parser.GetValue( "PCCafeBufferCount",	100,	&m_i32PCCafeBufferCount );

		m_i32PCCafeThreadCount = MIN(m_i32PCCafeThreadCount, MAX_LINK_THREAD_COUNT);
		m_i32PCCafeBufferCount = MIN(m_i32PCCafeBufferCount, MAX_LINK_BUFFER_COUNT);

		Parser.GetValue( "PCCafeMethod",		0,		&i32Temp );
		m_ePCCafeMethod = (LINK_METHOD)i32Temp;

		pStringValue = Parser.GetStringValue( "PCCafeTcpIP" );
		if( pStringValue )	m_ui32PCCafeIp = ::inet_addr(pStringValue);

		Parser.GetValue( "PCCafeTcpPort",		0,		&i32Temp );
		m_ui16PCCafePort = (UINT16)i32Temp;
	}
	else
	{
		goto e_Exit;
	}

	if( Parser.ReadSection( "Control" ) )
	{
		pStringValue = Parser.GetStringValue("IP"); 
		if( NULL == pStringValue )												goto e_Exit;
		m_ui32ControlIP = ::inet_addr( pStringValue );
		
		if( FALSE == Parser.GetValue( "PORT", 0, &i32Temp ) )					goto e_Exit;
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

void CConfig::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
	{
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
	}
}