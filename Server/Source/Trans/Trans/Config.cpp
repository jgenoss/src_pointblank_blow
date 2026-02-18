#include "pch.h"
#include "Config.h"
#include "TaskProcessor.h"

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

CConfig::CConfig(void)
{
	m_pui32GameServerIp			= NULL;

	m_i32MacAddressListCount	= 0;
	m_pui64MacAddressList		= NULL;
	m_ui32connectedUserCount	= 0;

	m_ui32GameServerCount		= 0;

	m_bRecvServerInfo			= FALSE;

	m_bPCCafeActive				= FALSE;
	m_bThaiESprotIDActive		= FALSE;
	m_bThaiIDPlusActive	= FALSE;
	
	m_ui32NewUserPoint			= 0; 

	m_ui32LastUpdateMI			= 0;
	m_ui32LastUpdateHH			= 0;
	m_ui32LastUpdateDD			= 0;

	m_ui16EventItemNewCount		= 0;
	m_ui16EventItemLoginCount	= 0;

	memset( m_aAttendanceInfo, 0, sizeof( ATTENDANCE_INFO ) * ATTENDANCE_MAX_EVENT_COUNT );

	// Config에서 읽어오지 않도록 초기화시 설정합니다.
	m_ui32MaxUserCount = MAX_CONNECT_USER_COUNT;

	i3mem::FillZero( &m_MedalSystemInfo, sizeof(MEDAL_SYSTEM_INFO) );

	m_ui8IPBlockNationCount		= 0;
	m_ui8IPBlockIPCount			= 0;

	m_bAASMethodAppointed		= false;

	m_ui32LimitAuthArg			= LIMIT_ITEM_DATE;

	m_ui8OverlapPartsItem = 0;

	m_i32Average_Min = 0;
	m_i32AverageCCU_Min = 0;
	m_ui32LastUpdateHour = 0;
}

CConfig::~CConfig(void)
{
	I3MEM_SAFE_FREE( m_pui32GameServerIp );
	I3MEM_SAFE_FREE( m_pui64MacAddressList );
}

BOOL CConfig::Create()
{
	i3ThreadTimer::LocalTime().GetSmallDate( m_strServerStartTime, SMALLDATE_STRING_SIZE );

	return TRUE;
}

BOOL CConfig::Init()
{
	return TRUE;
}

// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우
INT32 CConfig::LoadLogFile( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;
	char strDrive[SERVER_STRING_COUNT];
	i3System::GetDrive(g_pCCurrentPath, strDrive, SERVER_STRING_COUNT);


	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))				goto e_TransFail;
	if( FALSE == Parser.ReadSection( "Default" ) )					goto e_TransFail;


	// 1.Trans Log
	pstrValue = Parser.GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )											goto e_TransFail;
	
	MultiByteToWideChar( CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, SERVER_STRING_COUNT );

	// 정상적으로 로그가 생성 되었습니다.
	if( !i3System::PrepareDirectory( m_wstrLogFilePath ) )			goto e_TransFail;

	// 2. Service Log
	pstrValue = Parser.GetStringValue("ServiceLogFilePath");
	if (NULL == pstrValue)											goto e_ServiceFail;
	i3String::Copy(m_strServiceLogFilePath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strServiceLogFilePath))		goto e_ServiceFail;


	if (FALSE == Parser.ReadSection("LogD"))						goto e_TransZLogFail;
	// 3. Trans ZLog
	pstrValue = Parser.GetStringValue("ZLogFilePath");
	if (NULL == pstrValue)											goto e_TransZLogFail;
	i3String::Copy(m_strZLogFilePath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strZLogFilePath))				goto e_TransZLogFail;

	// 4. Trans ZLog BackUp
	pstrValue = Parser.GetStringValue("ZLogBackupPath");
	if (NULL == pstrValue)											goto e_TransZLogBackUpFail;
	i3String::Copy(m_strZLogBackupPath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strZLogBackupPath))		goto e_TransZLogBackUpFail;

	return SERVER_ERROR_NO;
 
e_TransFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	wchar_t wstrPath[ SERVER_STRING_COUNT ];
	MultiByteToWideChar( CP_ACP, NULL, g_pCCurrentPath, -1, wstrPath, SERVER_STRING_COUNT );

	wchar_t wstrDrive[ SERVER_STRING_COUNT ];
	i3System::GetDrive( wstrPath, wstrDrive, SERVER_STRING_COUNT );

	i3String::Format( m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Trans" );

	if( FALSE == i3System::PrepareDirectory( m_wstrLogFilePath ) )	return SERVER_ERROR_LOGFILE;

e_ServiceFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strServiceLogFilePath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "TRANS_SERVICE");

	if (FALSE == i3System::PrepareDirectory(m_strServiceLogFilePath))	return SERVER_ERROR_LOGFILE;


e_TransZLogFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strZLogFilePath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "Trans_ZLog");

	if (FALSE == i3System::PrepareDirectory(m_strZLogFilePath))	return SERVER_ERROR_LOGFILE;

e_TransZLogBackUpFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strZLogBackupPath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "Trans_ZLogBackup");

	if (FALSE == i3System::PrepareDirectory(m_strZLogBackupPath))	return SERVER_ERROR_LOGFILE;



	return SERVER_ERROR_NO;
}
// 여기에 소스 추가하지 마세요. 이 함수는 Config.ini 를 읽기 전에 로그 경로만 받아 오는 부분입니다. 김현우


BOOL CConfig::LoadConfigFile( const char * pstrFileName )
{
	i3IniParser Parser;

	if( FALSE == Parser.OpenFromFile( pstrFileName))							return FALSE;

	const char* pStringValue;
	INT32 i32Temp = 0;  
	//Read Default 
	if( Parser.ReadSection("Default") )
	{
		pStringValue = Parser.GetStringValue("Ip");
		if( NULL == pStringValue )												goto e_Exit;
		m_ui32IP = ::inet_addr( pStringValue );

		if( FALSE == Parser.GetValue( "Port", 0, &i32Temp) )					goto e_Exit;
		m_ui16Port = (UINT16)i32Temp;
	
		m_i32UpdateMain = 100; 
	
		Parser.GetValue("PCCafeActive", 0, &i32Temp);
		m_bPCCafeActive = ( i32Temp == 0) ? false : true;

		Parser.GetValue("ESportIDList", 0, &i32Temp);
		m_bThaiESprotIDActive = ( i32Temp == 0)	? FALSE : TRUE;

		Parser.GetValue("IDPlusList", 0, &i32Temp);
		m_bThaiIDPlusActive = ( i32Temp == 0) ? FALSE : TRUE;

		Parser.GetValue("PartsItemInsert", 0, &i32Temp);
		m_ui8OverlapPartsItem = UINT8((i32Temp == 0) ? FALSE : i32Temp);

		Parser.GetValue("IOCPCount", 32, &i32Temp);
		m_ui32GameThreadCount = (UINT32)i32Temp;

		Parser.GetValue("DeliverStoragePeriod", 90, &i32Temp);
		m_ui16DeliverStoragePeriod =  (UINT16)i32Temp;
	}
	else
	{
		goto e_Exit;
	}
	
	//Control
	if( Parser.ReadSection("Control"))
	{
		m_bControlActive = TRUE;

		pStringValue = Parser.GetStringValue("ControlServerIp");
		if( NULL == pStringValue )												goto e_Exit;
		m_ui32ControlServerIP = ::inet_addr( pStringValue ); 

		Parser.GetValue( "ControlServerPort", i32Temp, &i32Temp);
		m_ui16ControlServerPort = (UINT16)i32Temp; 
	}
	else 
	{
		goto e_Exit;
	}
	
	if (Parser.ReadSection("SIA"))
	{
		Parser.GetValue( "SIAActive", i32Temp, &i32Temp);
		m_bSIAActive = ( i32Temp == 0) ? FALSE : TRUE;
	}
	else
	{
		return FALSE;
	}

	//LogD
	if( Parser.ReadSection("LogD"))
	{
		Parser.GetValue( "LogDActive", i32Temp, &i32Temp);
		m_bLogDActive = ( i32Temp == 0)	? FALSE : TRUE;

		Parser.GetValue( "ZLogBackupKeep", 28, &m_i32ZLogKeepDay );
	}
	else
	{
		return FALSE;
	}

	// 개발용. 절대 퍼블리셔에 배포하면 안됩니다.
	if( Parser.ReadSection("Dev"))
	{
		Parser.GetValue( "UC", MAX_CONNECT_USER_COUNT, &m_ui32MaxUserCount );
	} 

	Parser.Close();

	return TRUE;

e_Exit:

	Parser.Close();

	return FALSE;
}

void CConfig::OnUpdate(void)
{
	UINT32 ui32ConnectServerFlag	= 0;

	g_ui32ConnectServer = ui32ConnectServerFlag;

	// 1분에 한번씩 갱신 합니다.
	if( (UINT32)i3ThreadTimer::LocalTime().GetMinute() == m_ui32LastUpdateMI )		return;
	m_ui32LastUpdateMI = i3ThreadTimer::LocalTime().GetMinute();

	_UpdateAAS();
	m_UserEvent.Update( i3ThreadTimer::LocalTime() );

	m_ui32LimitAuthArg = GetDiffTime(i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), LIMIT_ITEM_DATE);

	// Statistics
	++m_i32Average_Min;
	m_i32AverageCCU_Min += m_ui32connectedUserCount;

	// 1시간에 한 번씩 갱신 합니다.
	if( (UINT32)i3ThreadTimer::LocalTime().GetHour() == m_ui32LastUpdateHH )		return;
	m_ui32LastUpdateHH = i3ThreadTimer::LocalTime().GetHour();

	m_i32Average_Min = MAX(m_i32Average_Min, 1);

	STAT_DATA	astStat[STAT_TYPE_MAX];
	i3mem::FillZero(astStat, sizeof(STAT_DATA) * STAT_TYPE_MAX);

	if (g_pStatistics)
	{
		g_pStatistics->GetAllStat(astStat);
		g_pStatistics->Reset();
	}

	g_pLog->WriteLog( L"AverageUserCount : %d \r\n"
		L"[STATISTICS]\r\n"
		L"========== COPY DOWN =========== \r\n"
		L"{\"WORKTIME\": \r\n"
		L"{\"Date\":\"%04d-%02d-%02d\",\r\n"
		L"\"Data\":{\r\n"
		L"\t\"Type\":\"%d\",\r\n"
		L"\t\"Count\":\"%I64d\",\r\n"
		L"\t\"Time\":\"%.6lf\"},{\r\n"
		L"\t\"Type\":\"%d\",\r\n"
		L"\t\"Count\":\"%I64d\",\r\n"
		L"\t\"Time\":\"%.6lf\"},{\r\n"
		L"\t\"Type\":\"%d\",\r\n"
		L"\t\"Count\":\"%I64d\",\r\n"
		L"\t\"Time\":\"%.6lf\" } \r\n"
		L"}}\r\n"
		L"========== COPY UP =========== \r\n",
		m_i32AverageCCU_Min / m_i32Average_Min,
		i3ThreadTimer::LocalTime().GetYear(),
		i3ThreadTimer::LocalTime().GetMonth(),
		i3ThreadTimer::LocalTime().GetDay(),
		STAT_TYPE_DB_LOGIN,
		astStat[STAT_TYPE_DB_LOGIN].m_ui64Count,
		astStat[STAT_TYPE_DB_LOGIN].m_r64Time,
		STAT_TYPE_DB_LOGOUT,
		astStat[STAT_TYPE_DB_LOGOUT].m_ui64Count,
		astStat[STAT_TYPE_DB_LOGOUT].m_r64Time,
		STAT_TYPE_DB_SHOP,
		astStat[STAT_TYPE_DB_SHOP].m_ui64Count,
		astStat[STAT_TYPE_DB_SHOP].m_r64Time
	);

	m_i32Average_Min = 0;
	m_i32AverageCCU_Min = 0;

	// TASK WORK PROFILE
	WORK_PROFILE	stTaskWorkProfile, stLogWorkProfile;

	if (g_pTaskProcessor) g_pTaskProcessor->CopyWorkProFile(&stTaskWorkProfile);

	m_stWorkProfile.UpdateLogProfile(&stTaskWorkProfile, &stLogWorkProfile);

	g_pLog->WriteLog(L"\r\n"
		L"================================ \r\n"
		L"[TASK WORK PROFILE] \r\n"
		L"PacketCount: %I64d  \r\n"
		L"WorkCount	: %I64d  \r\n"
		L"WorkTime	: %.6lf  \r\n"
		L"================================ \r\n",
		stLogWorkProfile.m_ui64Packet,
		stLogWorkProfile.m_ui64Work,
		stLogWorkProfile.m_r64Time
	);

	// 1일에 한번 씩 갱싱 합니다.
	if( (UINT32)i3ThreadTimer::LocalTime().GetDay() == m_ui32LastUpdateDD )			return;
	m_ui32LastUpdateDD = i3ThreadTimer::LocalTime().GetDay();

	GetYesterDay();
	_UpdateAttendance();


	
	return; 
}

void CConfig::_UpdateAAS()
{
	m_bAASMethodAppointed = false;

	INT32 i32AASTimeIdx = (i3ThreadTimer::LocalTime().GetHour()*2) + (i3ThreadTimer::LocalTime().GetMinute()/30);
	if ( !IsContentsEnabled( CONTENTS_SHUTDOWN ) )	return;

	if ( m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_APPOINTED )
	{
		if ( m_AASOption.m_ui64AASAppointedTimeTable & ((UINT64)0x0000000000000001 << i32AASTimeIdx) )
			m_bAASMethodAppointed = true;
	}
}

void CConfig::_UpdateAttendance()
{
	ATTENDANCE_INFO * pEventInfo;
	for( INT32 i32attendanceType = 0; i32attendanceType < ATTENDANCE_TYPE_MAX; i32attendanceType++ )
	{
		m_i32ActiveAttendanceIdx[i32attendanceType] = -1;

		for( INT32 i = 0; i < ATTENDANCE_MAX_EACH_EVENT; i++ )
		{
			pEventInfo = &m_aAttendanceInfo[i32attendanceType][i];

			if (i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() > pEventInfo->m_ui32EndDate)			continue;
			if (i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() < pEventInfo->m_ui32StartDate)		continue;

			m_i32ActiveAttendanceIdx[i32attendanceType] = i;
			break;
		}
	}
}

UINT32 CConfig::GetConnectedUserCount(void) const
{
	return m_ui32connectedUserCount;
}

void CConfig::SetConnectedUserCount(UINT32 ui32count)
{
	m_ui32connectedUserCount = ui32count;
}

UINT32 CConfig::GetAuthTime(UINT32 ui32Sec)
{
	if (ui32Sec > m_ui32LimitAuthArg)
	{	// 계산된 만료 날짜가 최대치 보다 크면 예외처리.
		return LIMIT_ITEM_DATE;
	}

	struct tm oldTime;

	oldTime.tm_year = i3ThreadTimer::StandTime().GetYear() - 1900;
	oldTime.tm_mon  = (i3ThreadTimer::StandTime().GetMonth() -1);
	oldTime.tm_mday = i3ThreadTimer::StandTime().GetDay();
	oldTime.tm_hour = i3ThreadTimer::StandTime().GetHour();
	oldTime.tm_min	= i3ThreadTimer::StandTime().GetMinute() + 1;	//오차를 위해서 1분을 더 준다. 		
	oldTime.tm_sec  = ui32Sec;
	oldTime.tm_isdst= -1;

	mktime(&oldTime);

	UINT32 ui32Rv	=	(((oldTime.tm_year + 1900) - 2000) * 100000000) +
						((oldTime.tm_mon+1)  * 1000000) +
						(oldTime.tm_mday * 10000) +
						(oldTime.tm_hour * 100) +
						(oldTime.tm_min);

	return ui32Rv;
}



UINT32 CConfig::GetAddAuthTime(UINT32 ui32Date, UINT32 ui32Sec)
{
	UINT32 ui32Diff = GetDiffTime( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), ui32Date);

	if (ui32Diff + ui32Sec > m_ui32LimitAuthArg)
	{	// 계산된 만료 날짜가 최대치 보다 크면 예외처리.
		return LIMIT_ITEM_DATE;
	}

	struct tm oldTime;
	
	oldTime.tm_year = (ui32Date / 100000000) + 100;				ui32Date %= 100000000;
	oldTime.tm_mon  = (ui32Date / 1000000) - 1;					ui32Date %= 1000000;
	oldTime.tm_mday = (ui32Date / 10000);						ui32Date %= 10000;
	oldTime.tm_hour = (ui32Date / 100) ;						ui32Date %= 100;
	oldTime.tm_min	= ui32Date; 
	oldTime.tm_sec  = ui32Sec; 
	oldTime.tm_isdst= -1;

	mktime(&oldTime);
	
	UINT32 ui32Rv	=	(((oldTime.tm_year + 1900) - 2000) * 100000000) +
						((oldTime.tm_mon+1)  * 1000000) +
						(oldTime.tm_mday * 10000) +
						(oldTime.tm_hour * 100) +
						(oldTime.tm_min);

	return ui32Rv;
}

UINT32 CConfig::GetDiffTime(UINT32 ui32DT1, UINT32 ui32DT2)
{
	UINT32 ui32Temp;

	if (ui32DT2 < ui32DT1)
	{
		ui32Temp = ui32DT2;
		ui32DT2 = ui32DT1;
		ui32DT1 = ui32Temp;
	}

	struct tm tm1;

	tm1.tm_year = (INT32)((ui32DT1 / 100000000) + 100);			ui32DT1 %= 100000000;
	tm1.tm_mon = (INT32)((ui32DT1 / 1000000) - 1);				ui32DT1 %= 1000000;
	tm1.tm_mday = (INT32)(ui32DT1 / 10000);						ui32DT1 %= 10000;
	tm1.tm_hour = (INT32)(ui32DT1 / 100);						ui32DT1 %= 100;
	tm1.tm_min = ui32DT1;
	tm1.tm_sec = 0;
	tm1.tm_isdst = -1;

	time_t tmT1 = mktime(&tm1);

	struct tm tm2;

	tm2.tm_year = (INT32)((ui32DT2 / 100000000) + 100);			ui32DT2 %= 100000000;
	tm2.tm_mon = (INT32)((ui32DT2 / 1000000) - 1);				ui32DT2 %= 1000000;
	tm2.tm_mday = (INT32)(ui32DT2 / 10000);						ui32DT2 %= 10000;
	tm2.tm_hour = (INT32)(ui32DT2 / 100);						ui32DT2 %= 100;
	tm2.tm_min = ui32DT2;
	tm2.tm_sec = 0;
	tm2.tm_isdst = -1;

	time_t tmT2 = mktime(&tm2);

	return (UINT32)(difftime(tmT2, tmT1));
}

void CConfig::GetYesterDay()
{
	m_dt32YesterDateYYMMDD = i3ThreadTimer::LocalTime().GetDateTimeBin( TIME_ATTENDANCE_CHECK );
	m_dt32YesterDateYYMMDD.AddTime( I3DATE_ADD_TYPE_DAY, -1 );
}

void CConfig::SetServerInfo(INT32 i32ServerCount, UINT32 * pServerList)
{
	if(!m_bRecvServerInfo)
	{
		m_ui32GameServerCount	= i32ServerCount; 
		m_pui32GameServerIp		= (UINT32*)i3MemAlloc( sizeof(UINT32) * i32ServerCount );
		i3mem::Copy(m_pui32GameServerIp, pServerList, sizeof(UINT32) * i32ServerCount ); 
		m_bRecvServerInfo = TRUE;
	}

	// IOCP Thread 가 Socket 보다 많으면 개수를 맞춥니다.
	if( m_ui32GameThreadCount > m_ui32GameServerCount ) m_ui32GameThreadCount = m_ui32GameServerCount;
}

void CConfig::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
}

void CConfig::InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
		return;

	m_aClanMatchSeason[eSeasonType].SetSeason(pClanMatchSeason);
}

bool CConfig::CheckTargetAASDailyUsage()
{
	if ( !IsContentsEnabled( CONTENTS_SHUTDOWN ) )						return false;
	if ( !(m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_DAILY_USAGE) )	return false;

	// 하루이용량 제한 방식의 대상자를 정할 조건이 정해지면 이곳에 입력해 주세요.	2014.05.09

	return true;
}

bool CConfig::CheckTargetAASAppointed( INT32 i32Birth )
{
	if ( !IsContentsEnabled( CONTENTS_SHUTDOWN ) )					return false;
	if ( !(m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_APPOINTED) )	return false;
	if ( i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD() - i32Birth < AAS_APPOINTED_SHUTDOWN_AGE*10000 )	return true;
	
	return false;
}

void CConfig::SetUserEvent()
{
	m_UserEvent.SetLoginUserInfo(	&m_ui16EventItemLoginCount,	m_aEventItemLogin );
	m_UserEvent.SetNewUserInfo(		&m_ui16EventItemNewCount,	m_aEventItemNew );
}

BOOL CConfig::GetLoginUserEventGoods( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn )
{
	return m_UserEvent.GetLoginUserEventGoods( ui8LocaleCode, pdt32EventSendDate, pGiftIn );
}

BOOL CConfig::GetLoginUserEventItem( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem )
{
	return m_UserEvent.GetLoginUserEventItem( ui8LocaleCode, pdt32EventSendDate, pui8ItemCount, pItem );
}

BOOL CConfig::GetNewUserEvent( UINT8 ui8LocaleCode, UINT8* pui8ItemCount, ST_ITEM* pItem )
{
	return m_UserEvent.GetNewUserEvent( ui8LocaleCode, pui8ItemCount, pItem );
}


BOOL CConfig::IsPartsByItemID(T_ItemID TItemID )
{
	// DB에 ItemInsert하는 시점에 유저 버전을 알 수 없으므로 예외처리 합니다.( ex : GiftDeliver )
	if (PB_VER_V15 == m_eServerVersion && PB_VER_V15 == m_ui8OverlapPartsItem)
	{
		switch (GET_ITEM_TYPE(TItemID))
		{
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
		case ITEM_TYPE_SKIN:
		case ITEM_TYPE_BERET:
		{
			return TRUE;
		}
		default:
			return FALSE; 
		}
	}

	return FALSE;
}