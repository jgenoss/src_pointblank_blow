#include "pch.h"
#include "Control.h"
#include "UserSession.h"
#include <windows.h>
CControl * g_pControl = NULL;

I3_CLASS_INSTANCE( CControl, i3ElementBase );

CControl::CControl(void)
{ 	
	//m_bRunning				= FALSE;
	m_bReloadConfig				= FALSE;
	//Common	
	m_ui32IP					= 0;
	m_ui16Port					= 0;
	m_ui32UpdateTime			= 0;
	m_i32UserCount				= 0;
	m_ui8UseServerSelect		= 1;
	m_ui16WebPort				= 0;
	m_ui16WebSessionCount		= 0;

	m_i32ClanMaxCount			= 0;

	m_ui8DefaultItemCount		= 0;

	//Server List 
	m_ui32CommonServerCount		= 0;
	i3mem::FillZero(m_aCommonServerList,		sizeof(SERVER_INFO_COMMON) * MAX_COMMON_SERVER_COUNT);

	m_ui32GameServerCount		= 0;
	i3mem::FillZero(m_aGameServerList,			sizeof(SERVER_INFO_GAME) * MAX_SERVER_COUNT);
	
	m_ui32CastServerCount		= 0;
	i3mem::FillZero(m_CastServerList,			sizeof(SERVER_INFO_CAST) * MAX_SERVER_COUNT);

	m_ui32UpdateServerCount		= 0;
	i3mem::FillZero(m_aUpdateServerList,		sizeof(SERVER_INFO_COMMON) * MAX_SERVER_COUNT);

	for( INT32 i = 0 ; i < MAX_SERVER_COUNT ; i++ )
	{	// 모든 채널을 Normal 로 채웁니다.
		i3mem::Fill( m_aGameServerList[i]._EachType, CHANNEL_TYPE_NORMAL, sizeof(UINT8)*GAME_CHANNEL_COUNT );
		// 기본적인 유저 세팅
		m_aGameServerList[i]._ui16InitUserCount	= GAME_CHANNEL_USER_COUNT;
		m_aGameServerList[i]._ui16ExtraCount	= GAME_CHANNEL_EXTRA_COUNT;
		m_aGameServerList[i]._ui16MaxUserCount	= m_aGameServerList[i]._ui16InitUserCount * GAME_CHANNEL_COUNT;
		m_aGameServerList[i]._ui16NowUserCount	= m_aGameServerList[i]._ui16InitUserCount;

	}
	m_ui32BattleServerCount		= 0;
	i3mem::FillZero(m_aBattleServerList,		sizeof(SERVER_INFO_BATTLE) * MAX_SERVER_COUNT);

	i3mem::FillZero(m_aui32RecvHeartBit2Count,	sizeof(INT32) * MAX_SERVER_COUNT);	// HeartBit2 

	m_pGameHackCheckRatioList = (GSHackCheckRatio*)i3MemAlloc(sizeof(GSHackCheckRatio) * MAX_SERVER_COUNT);
	m_pDediHackCheckRatioList = (ServerHackCheckRatio*)i3MemAlloc(sizeof(ServerHackCheckRatio) * MAX_SERVER_COUNT);
	
	for (INT32 i = 0; i < MAX_SERVER_COUNT; ++i)
	{
		m_pGameHackCheckRatioList[i].SetDefaultValue();
		m_pDediHackCheckRatioList[i].SetDefaultValue();
	}

	m_pui8NotAllowRoomCreationList	= (UINT8*) i3MemAlloc(sizeof(UINT8) * MAX_SERVER_COUNT );
	i3mem::FillZero( m_pui8NotAllowRoomCreationList,sizeof(UINT8) * MAX_SERVER_COUNT );

	// Stage
	m_ui8RuleTotalCount			= 0;
	i3mem::FillZero( m_aRuleData, sizeof(MAP_RULE_DATA) * RULE_DATA_MAX );

	m_ui8StageTotalCount		= 0;	
	i3mem::FillZero( m_aStageData, sizeof(MAP_STAGE_DATA) * STAGE_TOTAL_MAX );

	m_ui16StageMatchingCount	= 0;
	i3mem::FillZero( m_aStageMatching, sizeof(MAP_STAGE_MATCHING) * STAGE_MATCHING_MAX );

	m_ui16RandomMapCount = 0;
	i3mem::FillZero(m_aRandomMapData, sizeof(MAP_RANDOM_STAGE_DATA) * STAGE_MATCHING_MAX);

	i3mem::FillZero(m_aRandomMapIdxData, sizeof(MAP_RANDOM_IDX_DATA) * RULE_DATA_MAX);

	m_ui8ActiveQuestCount		= 0;
	m_pui32ActiveQuest			= NULL;

	m_ui32BattleRoomCount		= 0;
	m_ui8BattlePortCount		= 0;
	i3mem::FillZero(m_aui16BattlePortList, sizeof(UINT16) * MAX_BATTLE_PORT_COUNT);
	
	//Contorl User List 
	m_ui32MacAddressListCount	= 0;
	i3mem::FillZero( m_aui64MacAddressList, sizeof(UINT64)*MACADDRESS_COUNT );
	
	m_ui32GameType				= 0;
	m_ui32AreaCode				= 0;
	m_ui32ServerCode			= 0;

	m_wstrLogFilePath[0]		= '\0';
	m_strServerLogFilePath[0]	= '\0';

	i3String::Copy( &m_astrServiceName[0][0],	L"_PBTransServerService",		64);
	i3String::Copy( &m_astrServiceName[1][0],	L"_PBClanServerService",		64);
	i3String::Copy( &m_astrServiceName[2][0],	L"_PBMessengerServerService",	64);
	i3String::Copy( &m_astrServiceName[3][0],	L"_PBDediServerService",		64);
	i3String::Copy( &m_astrServiceName[4][0],	L"_PBDediServerService",		64);			//배틀서버는 이름을 이렇게 변경합니다.
	i3String::Copy( &m_astrServiceName[5][0],	L"UpdateServer_Service",		64);
	i3String::Copy( &m_astrServiceName[6][0],	L"_PBControlServerService",		64);
	i3String::Copy( &m_astrServiceName[7][0],	L"_PBZLogServerService",		64);
	i3String::Copy( &m_astrServiceName[8][0],	L"_PBGatewayServerService",		64);
	i3String::Copy( &m_astrServiceName[9][0],	L"_PBSIAServerService",			64);
	i3String::Copy( &m_astrServiceName[10][0],	L"_PBGameServerService",		64);
	i3String::Copy( &m_astrServiceName[11][0],	L"_BillingServerService",		64);
	i3String::Copy( &m_astrServiceName[12][0],	L"_PBCastServerService",		64);

	i3mem::FillZero( m_aReNotice, sizeof(TYPE_RENOTICE)*NOTICE_COUNT );

	//Max CCU
	m_i32CCUNow_Day			= 0;
	m_i32CCUMaxCount_Day	= 0;
	m_ui32CCUMaxTime_Day	= 0;

	m_i32CCUNowCount		= 0;

	m_ui16BoostEventCount		= 0;
	i3mem::FillZero( m_stBoostEvent,		sizeof(BOOST_EVENT)*BOOST_EVENT_ALL_LOCALE_COUNT );

	m_ui16NewUserEventCount		= 0;
	i3mem::FillZero( m_stNewUserEvent,		sizeof(NEW_USER_EVENT)*NEW_USER_EVENT_ALL_LOCALE_COUNT );

	m_ui16LoginUserEventCount	= 0;
	i3mem::FillZero( m_stLoginUserEvent,	sizeof(LOGIN_USER_EVENT)*LOGIN_USER_EVENT_ALL_LOCALE_COUNT );

	m_strKey1[32]	= 0;
	m_strKey2[32]	= 0;

	m_i32NewUserPoint			= 0;
	m_i32ClanCreateRank			= 0;
	m_i32ClanCreatePoint		= 0;
	m_ui8ClanCreateMemberCount	= INCREASE_MEMBER_COUNT;

	// for dedication use.
	m_ui32SIAVersion	= 0;
	m_pui32WeaponList			= m_aui32Internal_WeaponList1;
	m_pui32WeaponListBack		= m_aui32Internal_WeaponList2;
	m_ui32WeaponListCount		= 0;
	m_ui32WeaponListCountBack	= 0;

	i3mem::FillZero( m_aui32Internal_WeaponList1, sizeof(m_aui32Internal_WeaponList1) );
	i3mem::FillZero( m_aui32Internal_WeaponList2, sizeof(m_aui32Internal_WeaponList2) );

	i3mem::FillZero( m_aAttendanceInfo, sizeof( ATTENDANCE_INFO ) * ATTENDANCE_MAX_EVENT_COUNT );

	i3mem::FillZero( m_aRankUpItem, sizeof(RANKUP_ITEM) * MAX_RANK_COUNT);

	m_pui32DayHackCount		= NULL;
	m_pui32HourHackCount	= NULL;

	m_strReportFtpAddress[0]	= '\0';
	m_ui16ReportFtpPort			= 0;

	m_ui8BillingUrlCount		= 0;
	i3mem::FillZero( m_astrBillingURL, sizeof(char) * LINK_METHOD_MAX * MAX_BILLING_URL );

	i3mem::FillZero( &m_MedalSystemInfo, sizeof(MEDAL_SYSTEM_INFO));

	m_ui8IPBlockNationCount		= 0;
	m_ui8IPBlockIPCount			= 0;

	i3mem::FillZero( &m_GiftBuyRanking, sizeof( GIFT_BUY_RANKING ));

	i3mem::FillZero( m_aTSEvent, sizeof(TS_EVENT_INFO)*TS_EVENT_COUNT );

	m_bUpdateServerState		= FALSE;
	m_strKnockServerAddress[0]	= '\0';
	m_ui16KnockServerPort		= 0;

	m_ui8URLListCount			= 0;
	i3mem::FillZero(m_ulURLList, sizeof(URL_LIST) * URL_MAX_COUNT);
	m_ui8MultiWeaponCount		= 10;
}

CControl::~CControl(void)
{
	Destroy(); 
}

BOOL CControl::Create(const char * pstrFileName )
{
	char strDrive[4];
	char strFile[MAX_PATH];
	char strDir[MAX_PATH];
	char strExt[16];
	char strCorFileName[ MAX_PATH ];

	_CorrectPath( pstrFileName, MAX_PATH, strCorFileName );

	_tsplitpath_s( m_strReFileName, strDrive, 4, strDir, MAX_PATH, strFile, MAX_PATH, strExt, 16 );
	i3String::Format( m_strReDirName, MAX_PATH, "%s%s", strDrive, strDir );

	//0. Load 
	if( FALSE == _LoadFromFile( strCorFileName ) ) 			return FALSE; 

	_MakeBattleGroup();

	m_pui32HourHackCount = (UINT32*)i3MemAlloc(sizeof(UINT32) * HACK_TYPE_MAX);
	i3mem::FillZero( m_pui32HourHackCount, (sizeof(UINT32) * HACK_TYPE_MAX)); 

	m_pui32DayHackCount = (UINT32*)i3MemAlloc(sizeof(UINT32) * HACK_TYPE_MAX);
	i3mem::FillZero( m_pui32DayHackCount, (sizeof(UINT32) * HACK_TYPE_MAX)); 	

	for(INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
	{
		if( m_rbWarningMessage[i].Create(sizeof(char*)  * KNOCK_SERVER_MESSAGE_COUNT, 100) == FALSE )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CControl::Destroy(void)
{
	I3MEM_SAFE_FREE(m_pui8NotAllowRoomCreationList);
	I3MEM_SAFE_FREE(m_pDediHackCheckRatioList);
	I3MEM_SAFE_FREE(m_pGameHackCheckRatioList);

	I3MEM_SAFE_FREE( m_pui32ActiveQuest );

	if( m_pui32DayHackCount != NULL) I3MEM_SAFE_FREE(m_pui32DayHackCount);
	if( m_pui32HourHackCount != NULL) I3MEM_SAFE_FREE(m_pui32HourHackCount);

	for(INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
	{
		m_rbWarningMessage[i].Destroy();
	}

	return TRUE;
}

// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우
INT32 CControl::LoadLogInfo( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;

	char strDrive[SERVER_STRING_COUNT];
	i3System::GetDrive(g_strCurrentPath, strDrive, SERVER_STRING_COUNT);

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))				goto e_Fail_ControlLog;
	if( FALSE == Parser.ReadSection( "Common" ) )					goto e_Fail_ControlLog;


	// 1. Contorl Log
	pstrValue = Parser.GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )											goto e_Fail_ControlLog;
	
	MultiByteToWideChar( CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if( !i3System::PrepareDirectory( m_wstrLogFilePath ) )			goto e_Fail_ControlLog;


	// 2. Server Log
	pstrValue = Parser.GetStringValue("ServerLogFilePach");
	if (NULL == pstrValue)											goto e_Fail_ServerLog;
	i3String::Copy(m_strServerLogFilePath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strServerLogFilePath))		goto e_Fail_ServerLog;


	// 3. Hack Log
	pstrValue = Parser.GetStringValue("HackLogFilePath");
	if (NULL == pstrValue)											goto e_Fail_HackLog;
	i3String::Copy(m_strHackLogFilePath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strHackLogFilePath))			goto e_Fail_HackLog;

	return SERVER_ERROR_NO;


e_Fail_ControlLog:
		
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	wchar_t wstrPath[SERVER_STRING_COUNT];
	MultiByteToWideChar( CP_ACP, NULL, g_strCurrentPath, -1, wstrPath, SERVER_STRING_COUNT);

	wchar_t wstrDrive[SERVER_STRING_COUNT];
	i3System::GetDrive( wstrPath, wstrDrive, SERVER_STRING_COUNT);

	i3String::Format( m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Control" );

	if( FALSE == i3System::PrepareDirectory( m_wstrLogFilePath ) )	return SERVER_ERROR_LOGFILE;


e_Fail_ServerLog:

	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strServerLogFilePath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "ServerLog");

	if (FALSE == i3System::PrepareDirectory(m_strServerLogFilePath))	return SERVER_ERROR_LOGFILE;


e_Fail_HackLog:

	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strHackLogFilePath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "Hack");

	if (FALSE == i3System::PrepareDirectory(m_strHackLogFilePath))	return SERVER_ERROR_LOGFILE;

	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우


BOOL CControl::_LoadFromFile(const char * pstrFileName)
{
	i3IniParser Parser;
	char		strTemp[MAX_PATH];
	INT32		i32TempValue;

	if( FALSE == Parser.OpenFromFile( pstrFileName) )						return FALSE;

	//----------------------------------------------------------------------------------------------------------------------
	//1. Common	
	const char* pstrStringValue;
	if( FALSE == Parser.ReadSection("Common"))								goto exit_Error;

	pstrStringValue = Parser.GetStringValue("IP");
	if( NULL == pstrStringValue )											goto exit_Error;
	m_ui32IP = ::inet_addr( pstrStringValue ); 

	if( FALSE == Parser.GetValue( "Port", 0, &i32TempValue) )				goto exit_Error;
	m_ui16Port = (UINT16)i32TempValue; 

	Parser.GetValue( "UpdateTime", 5, &i32TempValue);
	m_ui32UpdateTime = i32TempValue;
	if(m_ui32UpdateTime < 2)	m_ui32UpdateTime = 2;

	// WEB Tool
	Parser.GetValue( "WebAdminPort", WEB_TOOL_ACCEPT_DEFAULT_PORT, &i32TempValue);
	m_ui16WebPort = (UINT16)i32TempValue;

	m_ui16WebSessionCount = 100;

	//----------------------------------------------------------------------------------------------------------------------
	//MD5_KEY
	// UpdaterServer로 이전
	if( FALSE == Parser.ReadSection("MD5_KEY"))								goto exit_Error;
	pstrStringValue = Parser.GetStringValue( "KEY" );		
	if( NULL == pstrStringValue )											goto exit_Error;
	i3String::Copy( m_strKey1, pstrStringValue, NET_MD5_KEY_SIZE + 1 );

	//----------------------------------------------------------------------------------------------------------------------
	//ServerList Open 
	m_ui32CommonServerCount = 0;
	if( FALSE == Parser.ReadSection("CommonServerList"))					goto exit_Error;

	//SIA
	pstrStringValue = Parser.GetStringValue("SIA");
	if( pstrStringValue )
	{
		m_aCommonServerList[m_ui32CommonServerCount]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aCommonServerList[m_ui32CommonServerCount]._ui8Type	= ASC_SERVER_SIA;
		m_ui32CommonServerCount++;
	}
	else
	{
		goto exit_Error;
	}

	//TRANS
	pstrStringValue = Parser.GetStringValue("TRANS"); 
	if( pstrStringValue )
	{
		m_aCommonServerList[m_ui32CommonServerCount]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aCommonServerList[m_ui32CommonServerCount]._ui8Type	= ASC_SERVER_TRANS;
		m_ui32CommonServerCount++; 
	}
	else
	{
		goto exit_Error;
	}

	//CLAN		
	pstrStringValue = Parser.GetStringValue("CLAN"); 
	if( pstrStringValue )
	{
		m_aCommonServerList[m_ui32CommonServerCount]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aCommonServerList[m_ui32CommonServerCount]._ui8Type	= ASC_SERVER_CLAN;
		m_ui32CommonServerCount++;
	}
	else
	{
		goto exit_Error;
	}

	//MESSENGER
	pstrStringValue = Parser.GetStringValue("MESSENGER"); 
	if( pstrStringValue )
	{
		m_aCommonServerList[m_ui32CommonServerCount]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aCommonServerList[m_ui32CommonServerCount]._ui8Type	= ASC_SERVER_MESSENGER;
		m_ui32CommonServerCount++;
	}
	else
	{
		goto exit_Error;
	}

	//UPDATE
	pstrStringValue = Parser.GetStringValue("UPDATE"); // N개 이상 수용 가능하도록 ini가 변경되었지만 기존 ini도 사용 가능합니다.
	if( pstrStringValue )
	{
		m_aUpdateServerList[0]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aUpdateServerList[0]._ui8Type	= ASC_SERVER_UPDATER;
		m_ui32UpdateServerCount = 1;
	}

	//ZLOG
	pstrStringValue = Parser.GetStringValue("ZLOG"); 
	if( pstrStringValue )
	{
		m_aCommonServerList[m_ui32CommonServerCount]._ui32Ip	= ::inet_addr( pstrStringValue );
		m_aCommonServerList[m_ui32CommonServerCount]._ui8Type	= ASC_SERVER_ZLOG;
		m_ui32CommonServerCount++;
	}
	else
	{
		goto exit_Error;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//Update Server
	if( Parser.ReadSection("UpdateServerList") )
	{
		Parser.GetValue( "Count", m_ui32UpdateServerCount, &m_ui32UpdateServerCount);

		for(UINT32 i = 0; i < m_ui32UpdateServerCount; i++)
		{
			i3String::Format( strTemp, MAX_PATH, "%dIP", i ); 
			pstrStringValue = Parser.GetStringValue(strTemp);
			if( NULL == pstrStringValue )											goto exit_Error;
			m_aUpdateServerList[i]._ui32Ip	= ::inet_addr( pstrStringValue );
			m_aUpdateServerList[i]._ui8Type	= ASC_SERVER_UPDATER;
		}
	}

	// 해킹 관련 설정
	UINT32 ui32BaseUSGKick = 0;
	if (FALSE != Parser.ReadSection("Hack"))
	{
		Parser.GetValue("USG", 0, &ui32BaseUSGKick);

		ui32BaseUSGKick = MIN(10, ui32BaseUSGKick);

		for (INT32 i = 0; i < MAX_SERVER_COUNT; ++i)
		{
			m_pGameHackCheckRatioList[i].m_aui8HackCheckRatio[GS_HACK_TYPE_DLL_CHEAT] = (UINT8)ui32BaseUSGKick;
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	//Game Server 	
	if( Parser.ReadSection("GameServerList") )
	{
		Parser.GetValue( "Count", m_ui32GameServerCount, &m_ui32GameServerCount);

		for(UINT32 i = 0; i < m_ui32GameServerCount; i++)
		{
			i3String::Format( strTemp, MAX_PATH, "%dIP", i ); 
			pstrStringValue = Parser.GetStringValue(strTemp);
			if( NULL == pstrStringValue )											goto exit_Error;
			m_aGameServerList[i]._ui32Ip		= ::inet_addr( pstrStringValue );
			m_aGameServerList[i]._OutIP			= m_aGameServerList[i]._ui32Ip;
			m_aGameServerList[i]._OutPort		= 39190;						// 기본적으로 게임서버는 이 포트를 쓴다는 가정입니다.
			m_aGameServerList[i]._ui8Type		= ASC_SERVER_GAME;

			i3String::Format( strTemp, MAX_PATH, "%dGROUP", i );
			Parser.GetValue( strTemp, 0, &i32TempValue );
			m_aGameServerList[i].m_ui32Group	= (UINT32)i32TempValue;

			UINT32 ui32USGKick;
			i3String::Format(strTemp, MAX_PATH, "%dUSG", i);
			Parser.GetValue(strTemp, ui32BaseUSGKick, &ui32USGKick);

			ui32USGKick = MIN(10, ui32USGKick);

			m_pGameHackCheckRatioList[i].m_aui8HackCheckRatio[GS_HACK_TYPE_DLL_CHEAT] = (UINT8)ui32USGKick;
		}
	}
	else
	{
		goto exit_Error;
	}
	
	//----------------------------------------------------------------------------------------------------------------------
	//Cast Server
	if( Parser.ReadSection("CastServerList") )
	{
		Parser.GetValue( "Count", m_ui32CastServerCount, &m_ui32CastServerCount);

		for(UINT32 i = 0; i < m_ui32CastServerCount; i++)
		{
			i3String::Format( strTemp, MAX_PATH, "%dIP", i ); 
			pstrStringValue = Parser.GetStringValue(strTemp);
			if( NULL == pstrStringValue )											goto exit_Error;
			m_CastServerList[i]._ui32Ip	= ::inet_addr( pstrStringValue );
			m_CastServerList[i]._ui8Type	= ASC_SERVER_CAST;
		}
	}

	else
	{
		goto exit_Error;
	}

	//----------------------------------------------------------------------------------------------------------------------
	//Battle Server 
	if( Parser.ReadSection("BattleServerList") )
	{
		Parser.GetValue( "RoomCount", 100, &m_ui32BattleRoomCount);
		if( FALSE == Parser.GetValue( "Count", 0, &m_ui32BattleServerCount) )	goto exit_Error;
		for( UINT32 i = 0; i < m_ui32BattleServerCount; i++ )
		{
			i3String::Format(strTemp, MAX_PATH, "%dIP", i); 
			pstrStringValue = Parser.GetStringValue(strTemp);
			if( NULL == pstrStringValue )										goto exit_Error;
			m_aBattleServerList[i]._ui32Ip		= ::inet_addr( pstrStringValue );
			m_aBattleServerList[i]._ui8Type		= ASC_SERVER_BATTLE;

			i3String::Format( strTemp, MAX_PATH, "%dGROUP", i );
			Parser.GetValue( strTemp, 0, &i32TempValue );
			m_aBattleServerList[i].m_ui32Group	= (UINT32)i32TempValue;
		}
	}
	else
	{
		goto exit_Error;
	}

	//추후 삭제 합니다. 
	m_ui8BattlePortCount = 10;
	for( UINT32 i = 0; i < m_ui8BattlePortCount; i++ )
	{
		m_aui16BattlePortList[i] = (UINT16)(40000 + i);
	}

	//----------------------------------------------------------------------------------------------------------------------
	// 서동권 - 신고하기 기능 스크린샷 전송될 FTP 주소 
	if( Parser.ReadSection("ReportFTPAddress") )
	{
		pstrStringValue = Parser.GetStringValue("IP");
		if( pstrStringValue )
		{
			i3String::Copy( m_strReportFtpAddress, pstrStringValue, MAX_REPORT_FTP_ADDRESS );
		}
		Parser.GetValue( "Port", 0, &i32TempValue);
		m_ui16ReportFtpPort = (UINT16)i32TempValue;
	}

	// Knock Server Address
	if( TRUE ==  Parser.ReadSection("KnockServerAddress") )
	{
		Parser.GetValue( "active", 0, &m_bActiveKnockServer );

		if( TRUE == m_bActiveKnockServer )
		{
			pstrStringValue = Parser.GetStringValue("IP");
			if( NULL == pstrStringValue )											goto exit_Error;
			i3String::Copy( m_strKnockServerAddress, pstrStringValue, MAX_KNOCK_SERVER_ADDRESS );
		
			Parser.GetValue( "Port", 0, &i32TempValue);
			m_ui16KnockServerPort = (UINT16)i32TempValue;
		}
	}
	else m_bActiveKnockServer = 0;

	
	//----------------------------------------------------------------------------------------------------------------------

	Parser.Close();

	return TRUE;

exit_Error:

	Parser.Close();

	return FALSE;
}

void CControl::UPdateMaxCCU(void)
{
	// 날짜가 바뀌면 현재 CCU 를 넣어준다.
	if( m_i32CCUNow_Day != i3ThreadTimer::LocalTime().GetDay())
	{
		m_i32CCUMaxCount_Day	= m_i32CCUNowCount;
		m_i32CCUNow_Day			= i3ThreadTimer::LocalTime().GetDay();
		m_ui32CCUMaxTime_Day	= i3ThreadTimer::LocalTime().GetDateTimeMMDDHHMISS();
	}

	// 기존 CCU 보다 크면 바꿔준다. Day 기준
	if(m_i32CCUMaxCount_Day < m_i32CCUNowCount)
	{
		m_i32CCUMaxCount_Day	= m_i32CCUNowCount;
		m_ui32CCUMaxTime_Day	= i3ThreadTimer::LocalTime().GetDateTimeMMDDHHMISS();
	}
	return; 
}

INT32 CControl::GetServerIdx( UINT32 ui32IpAddress, UINT8 ui8Type )
{
	switch(ui8Type)
	{
	case ASC_SERVER_GAME	: 
		{
			for(UINT32 i = 0; i < m_ui32GameServerCount; i++)
			{
				if( m_aGameServerList[i]._ui32Ip == ui32IpAddress)return i;
			}
		}
		break; 
	case ASC_SERVER_BATTLE	:
		{
			for(UINT32 i = 0; i < m_ui32BattleServerCount; i++)
			{
				if( m_aBattleServerList[i]._ui32Ip == ui32IpAddress)return i;
			}
		}
		break; 
	case ASC_SERVER_CAST:
		{
			for(UINT32 i = 0; i < m_ui32CastServerCount; i++)
			{
				if( m_CastServerList[i]._ui32Ip == ui32IpAddress)return i;
			}
		}
		break;
	case ASC_SERVER_UPDATER:
		{
			for(UINT32 i = 0; i < m_ui32UpdateServerCount; i++)
			{
				if( m_aUpdateServerList[i]._ui32Ip == ui32IpAddress)return i;
			}
		}
		break;
	default					:
		for(UINT32 i = 0; i < m_ui32CommonServerCount; i++)
		{
			if(	(m_aCommonServerList[i]._ui32Ip == ui32IpAddress) && 
				(m_aCommonServerList[i]._ui8Type == ui8Type) )return i;
		}
		break; 
	}

	return -1; 
}

SERVER_INFO_COMMON* CControl::GetServerInfo( ASC_SERVER_TYPE eServer, UINT32 ui32Idx )
{
	switch( eServer )
	{
	case ASC_SERVER_BATTLE:
		{
			if( ui32Idx >= m_ui32BattleServerCount )	break;
		
			return &m_aBattleServerList[ ui32Idx ];
		}
		break;
	case ASC_SERVER_CAST:
		{
			if( ui32Idx >= m_ui32CastServerCount )	break;
		
			return &m_CastServerList[ ui32Idx ];
		}
		break;
	case ASC_SERVER_GAME:
		{
			if( ui32Idx >= m_ui32GameServerCount )	break;
		
			return &m_aGameServerList[ ui32Idx ];
		}
		break;
	case ASC_SERVER_UPDATER:
		{
			if( ui32Idx >= m_ui32UpdateServerCount )	break;
		
			return &m_aUpdateServerList[ ui32Idx ];
		}
		break;
	default:
		for( UINT32 i = 0 ; i < m_ui32CommonServerCount ; i++ )
		{
			if(	eServer != (ASC_SERVER_TYPE)m_aCommonServerList[i]._ui8Type ) continue;

			return &m_aCommonServerList[i];
		}
		break;
	}
	
	return NULL;
}

BOOL CControl::CheckMacAddress(UINT64 ui64MacAddress)
{
#ifdef I3_DEBUG	//디버그 용이면 무조건 리턴 합니다. 
	return TRUE;
#else
	m_ui32MacAddressListCount = min(m_ui32MacAddressListCount,MACADDRESS_COUNT);

	for(UINT32 i = 0; i < m_ui32MacAddressListCount; i++)
	{		
		if( m_aui64MacAddressList[i] == ui64MacAddress)return TRUE; 
	}
	return FALSE; 
#endif
}

void CControl::_CorrectPath( const char* pstrIn, INT32 i32Size, char* pstrOut )
{
	INT32 i32MaxSize = i32Size-1;
	INT32 i32Count = 0;
	while( '\0' != *pstrIn && i32Count < i32MaxSize )
	{
		*pstrOut = *pstrIn;

		if( '/' == *pstrOut )
		{
			*pstrOut = '\\';
		}
		pstrIn++;
		pstrOut++;
		i32Count++;
	}
	*pstrOut = '\0';
}

BOOL CControl::SetWeaponList(UINT32* pui32WeaponList, UINT32 ui32ItemCount )
{
	I3ASSERT( pui32WeaponList );
	I3ASSERT( ui32ItemCount > 0	 );
	I3ASSERT( ui32ItemCount < DS_MAX_WEAPON_LIST - 1);	// check range

	memcpy_s(	m_pui32WeaponListBack, 
			    DS_MAX_WEAPON_LIST * sizeof(UINT32),
				pui32WeaponList,
				ui32ItemCount * sizeof(UINT32) );

	m_ui32WeaponListCountBack = ui32ItemCount;
	
	_Swap();

	// reset back buffer
	m_ui32WeaponListCountBack = 0;
	i3mem::FillZero( m_pui32WeaponListBack, DS_MAX_WEAPON_LIST * sizeof(UINT32)) ;

	return TRUE;
}


BOOL CControl::GetWeaponList( UINT32* pui32Out, UINT32 ui32Bytes, UINT32* pui32OutItemCount ) 
{
	I3ASSERT( pui32Out );
	I3ASSERT( pui32OutItemCount );
	
	// out buffer 크기 검사
	if ( m_ui32WeaponListCount > 0 && m_ui32WeaponListCount * sizeof(UINT32) < ui32Bytes)
	{
		memcpy_s( pui32Out, ui32Bytes, m_pui32WeaponList, m_ui32WeaponListCount * sizeof(UINT32));
		*pui32OutItemCount = m_ui32WeaponListCount;
		return TRUE;
	}

	return FALSE; 
}

bool CControl::InsertDatabaseConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig)
{
	if( eServer == ASC_SERVER_CONTROL )
		m_DatabaseMgr.InsertDatabaseConfig(pDBConfig);

	return m_DBConfig.InsertDatabaseConfig(eServer, pDBConfig);
}

bool CControl::InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
		return false;

	m_ClanMatchSeason[eSeasonType].SetSeason(pMatchSeason);
	return true;
}

void CControl::MakePacketClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
		return;

	m_ClanMatchSeason[eSeasonType].MakePacket(pMatchSeason);
}

void CControl::MakeWarningMessage()
{
	if( TRUE != m_bActiveKnockServer ) return; 	// Knock Server가 활성화 되고 Warning값이 갱신됫을경우만 실행된다.
	if( TRUE != m_bUpdateServerState ) return;

	for(INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
	{
		_MakeWarningMessage(i);
	}
}

void CControl::_MakeWarningMessage( INT32 i32WarningType )
{
	UINT32	ui32WarningInfo[ASC_SERVER_COUNT]	= {0,};
	UINT32	ui32WarningCount[ASC_SERVER_COUNT]	= {0,};
	UINT32	ui32WarningInfoTemp;
	UINT32	ui32ServerType;

	char	strMessage[KNOCK_SERVER_MESSAGE_COUNT];
	char	strMessageTemp[KNOCK_SERVER_MESSAGE_COUNT];
	char	strNum[10];
	char	*pMessagePoint;
	INT32	i32Size;
	INT32	i32TempSize;
	INT32	i32Date;

	//각서버들의 Warning정보를 Type,발생수 별로 정리한다.

	//Control Server
	ui32WarningInfo[ASC_SERVER_CONTROL] |= g_pLog->GetWarning().GetWarningInfo(i32WarningType);
	
	if(ui32WarningInfo[ASC_SERVER_CONTROL] != WARNING_LEVEL_NO) ui32WarningCount[ASC_SERVER_CONTROL]++;

	//CommonServer
	for( UINT32 i = 0 ; i < m_ui32CommonServerCount; i++)
	{
		ui32WarningInfoTemp = m_aCommonServerList[i]._ui32WarningInfo[i32WarningType];
		ui32ServerType		= m_aCommonServerList[i]._ui8Type;
		if( ui32WarningInfoTemp != WARNING_LEVEL_NO)
		{
			ui32WarningInfo[ui32ServerType] |= ui32WarningInfoTemp ;
			ui32WarningCount[ui32ServerType]++;
		}
	}

	//GameServer
	for( UINT32 i = 0 ; i < m_ui32GameServerCount; i++)
	{
		ui32WarningInfoTemp = m_aGameServerList[i]._ui32WarningInfo[i32WarningType];
		ui32ServerType		= m_aGameServerList[i]._ui8Type;
		if( ui32WarningInfoTemp != WARNING_LEVEL_NO)
		{
			ui32WarningInfo[ui32ServerType] |= ui32WarningInfoTemp ;
			ui32WarningCount[ui32ServerType]++;
		}
	}

	//BattleServer
	for( UINT32 i = 0 ; i < m_ui32BattleServerCount; i++)
	{
		ui32WarningInfoTemp = m_aBattleServerList[i]._ui32WarningInfo[i32WarningType];
		ui32ServerType		= m_aBattleServerList[i]._ui8Type;
		if( ui32WarningInfoTemp != WARNING_LEVEL_NO)
		{	
			ui32WarningInfo[ui32ServerType] |= ui32WarningInfoTemp ;
			ui32WarningCount[ui32ServerType]++;
		}
	}

	strMessage[0] = '\0';
	pMessagePoint = strMessage;
	i32Size = 0;

	i32Date = i3ThreadTimer::LocalTime().GetDateTimeMMDDHHMISS();
	itoa(i32Date, strNum, 10);
	_Concat(&pMessagePoint, strNum, &i32Size);
	_Concat(&pMessagePoint, "\n", &i32Size);

	//Message 처음부분에 시간을넣어준다.

	for(INT32 i = 0 ; i < ASC_SERVER_COUNT; i++)	// 각서버마다 위에서 취합한 데이터를가지고 Message를 포맷한다.
	{
		if (ui32WarningCount[i] == 0) // Warning이 없을경우 그냥 다음줄로 넘겨준다.
		{
			strMessageTemp[0] = '\n';
			strMessageTemp[1] = '\0';
			i32TempSize = 1;
		}
		else
		{
			i32TempSize = _MakeWarningFormat( strMessageTemp, (UINT8)i, ui32WarningInfo[i], ui32WarningCount[i]); // Message포맷함수 strMessageTemp String에 "서버이름 [문재1][문재2] 문재 서버 카운트" 형식으로 포맷된다.
		}

		if( i32Size + i32TempSize >= KNOCK_SERVER_MESSAGE_COUNT )	// 포맷된 Message가 기존 Message 사이즈를 더해서 KNOCK_SERVER_MESSAGE_COUNT 보다 클경우 
		{	//그전 Messaqge를 링버퍼에 저장하고 새로운 Message에 만들어진 Messgae를 넣어준다.

			m_rbWarningMessage[i32WarningType].Push(strMessage);
			strMessage[0] = '\0';
			i32Size = 0;
			pMessagePoint = strMessage;
		}

		_Concat(&pMessagePoint, strMessageTemp, &i32Size);

	}

	m_rbWarningMessage[i32WarningType].Push(strMessage);				// Message 포맷작업이 끝났음으로 링버퍼에 저장해준다.
}

void CControl::_Concat( char ** pDest, const char * pSrc, INT32 *i32Size)
{
	while( *pSrc)
	{
		*(*pDest) = *pSrc;
		(*pDest)++;
		pSrc++;
		(*i32Size)++;
	}	

	*(*pDest) = 0;
}	

INT32 CControl::_MakeWarningFormat( char* strWarning, UINT8 ui8Type, UINT32 ui32WarningFlag, UINT32 ui32WarningCount )
{
	INT32	i32Size			= 0;
	char	*pMessagePoint	= strWarning;
	char	strNum[10];
	char	strWarningFlag[32];

	itoa(ui32WarningCount, strNum, 10);
	itoa(ui32WarningFlag, strWarningFlag, 10);

	switch( ui8Type )
	{
	case ASC_SERVER_TRANS :					_Concat(&pMessagePoint, "TRANS",		&i32Size);	break;
	case ASC_SERVER_CLAN :					_Concat(&pMessagePoint, "CLAN",			&i32Size);	break;
	case ASC_SERVER_MESSENGER :				_Concat(&pMessagePoint, "MESSENGER",	&i32Size);	break;
	case ASC_SERVER_BATTLE :				_Concat(&pMessagePoint, "BATTLE",		&i32Size);	break;
	case ASC_SERVER_UPDATER :				_Concat(&pMessagePoint, "UPDATER",		&i32Size);	break;
	case ASC_SERVER_CONTROL :				_Concat(&pMessagePoint, "CONTROL",		&i32Size);	break;
	case ASC_SERVER_ZLOG :					_Concat(&pMessagePoint, "ZLOG",			&i32Size);	break;
	case ASC_SERVER_GATEWAY :				_Concat(&pMessagePoint, "GATEWAY",		&i32Size);	break;
	case ASC_SERVER_SIA :					_Concat(&pMessagePoint, "SIA",			&i32Size);	break;
	case ASC_SERVER_GAME :					_Concat(&pMessagePoint, "GAME",			&i32Size);	break;
	default:								_Concat(&pMessagePoint, "UNKNOWN",		&i32Size);	break;
	}

	_Concat(&pMessagePoint, " ",			&i32Size);

	/*if( g_pLog->GetWarning().GetFlagWarningLevel( WARNING_LEVEL_PERFOMANCE	) & ui32WarningFlag) _Concat(&pMessagePoint, "[PERFOMANCE]",	&i32Size);
	if( g_pLog->GetWarning().GetFlagWarningLevel( WARNING_LEVEL_NETWORK		) & ui32WarningFlag) _Concat(&pMessagePoint, "[NETWORK]",		&i32Size);
	if( g_pLog->GetWarning().GetFlagWarningLevel( WARNING_LEVEL_DB			) & ui32WarningFlag) _Concat(&pMessagePoint, "[DB]",			&i32Size);
	if( g_pLog->GetWarning().GetFlagWarningLevel( WARNING_LEVEL_GAME_GUARD	) & ui32WarningFlag) _Concat(&pMessagePoint, "[GAMEGUARD]",		&i32Size);
	if( g_pLog->GetWarning().GetFlagWarningLevel( WARNING_LEVEL_CRASH		) & ui32WarningFlag) _Concat(&pMessagePoint, "[CRASH]",			&i32Size);*/

	_Concat(&pMessagePoint, strWarningFlag,	&i32Size);

	_Concat(&pMessagePoint, " ",			&i32Size);
	_Concat(&pMessagePoint, strNum,			&i32Size);
	_Concat(&pMessagePoint, "\n",			&i32Size);

	return i32Size;
}	

void CControl::_MakeBattleGroup()
{
	UINT32 ui32Cast, ui32Server;
	SERVER_INFO_CAST* pCast;
	for( ui32Cast = 0 ; ui32Cast < m_ui32CastServerCount ; ui32Cast++ )
	{
		pCast = &m_CastServerList[ ui32Cast ];

		pCast->m_ui32GroupGameCount		= 0;
		pCast->m_ui32GroupBattleCount	= 0;

		for( ui32Server = 0 ; ui32Server < m_ui32GameServerCount ; ui32Server++ )
		{
			if( ui32Cast != m_aGameServerList[ ui32Server ].m_ui32Group )		continue;

			pCast->m_ui32GroupGameIdx[ pCast->m_ui32GroupGameCount ] = ui32Server;
			pCast->m_ui32GroupGameCount++;
		}
		for( ui32Server = 0 ; ui32Server < m_ui32BattleServerCount ; ui32Server++ )
		{
			if( ui32Cast != m_aBattleServerList[ ui32Server ].m_ui32Group )		continue;

			pCast->m_ui32GroupBattleIdx[ pCast->m_ui32GroupBattleCount ] = ui32Server;
			pCast->m_ui32GroupBattleCount++;
		}
	}
}

BOOL CControl::CheckDefaultMap()
{

	for (UINT8 i = 0; i < m_ui8RuleTotalCount; i++)
	{
		if (0 == m_aRuleData[i].m_ui8DefaultStage)
			continue;

		UINT32	ui32StageID = m_aRuleData[i].m_ui32Rule + m_aRuleData[i].m_ui8DefaultStage;
		bool	bIsExistStage = false;

		for (UINT16 j = 0; j < m_ui16StageMatchingCount; j++)
		{
			if (ui32StageID == m_aStageMatching[j].m_ui32StageID)
			{
				bIsExistStage = true;
				break;
			}
		}

		if (!bIsExistStage)
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Default Map Setting is invalid. idx : %d ", i);
			return FALSE;
		}
	}

	return TRUE;
}