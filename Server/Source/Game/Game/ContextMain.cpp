#include "pch.h"
#include "ContextMain.h"
#include "ServerContext.h"
#include "DllInjection_NProtect.h"
#include "DllInjection_AhnHS.h"
#include "RoomManager.h"

#include <windows.h>

boost::BOOST_RANDOM_ENGINE	g_RandomEngine;

CContextMain * g_pContextMain = NULL;

I3_CLASS_INSTANCE( CContextMain, i3ElementBase);

CContextMain::CContextMain(void)
{
	m_ui32ThreadCount			= MAX_USER_THREAD_COUNT;

	m_FieldShop.m_ui32Version	= 0;

	m_i32LastUpdateTime			= 0;

	m_ui32ChannelSessionCount	= 0;
	m_ui32ExtraSessionCount		= 0;
	m_i32SessionCount			= 0;
	m_i32UpdateMainTime			= 500;
	m_ui32UpdateChannel			= 0;
	m_ui32ExtraSessionCount		= 100;
	m_pChannelType				= NULL;
	m_ui32QuestEventDate		= 0;
	m_i32ClanMax				= 0;

	m_hGameGuard				= NULL;

	m_wstrLogFilePath[0]		= 0;

	i3mem::FillZero(m_strNormalNotice,		NOTICE_STRING_MAX_COUNT ); 
	i3mem::FillZero(m_strEmergencyNotice,	NOTICE_STRING_MAX_COUNT );

	//Set Shop Value	
	m_pShop						= NULL;

	m_ui8AddressCount			= 0;
	m_pui32AddressIp			= NULL;
	m_pui16AddressPort			= NULL;
	m_pui8AddressTimeOut		= NULL;

	m_bAbusingCheck				= FALSE;

	m_ui32CastServerIp			= ::inet_addr( "127.0.0.1" );
	m_ui16CastServerPort		= 39130;

	m_ui8ClanCreateRank			= 0;
	m_i32ClanCreatePoint		= 0;

	m_i32ServerIdx				= -1;
	m_i32Gateway				= SERVER_TYPE_GAME;

	m_strEnterPass[0]			= '\0';

	//Clan 초기화 
	m_i32ClanVersion			= 0;
	i3mem::FillZero( &m_ClanContext,	sizeof(CLAN_CONTEXT)*CLAN_LIST_COUNT);
	m_ClanList[0] = NULL;
	m_ClanList[1] = NULL;
	
	m_i32LastUpdateTime			= 0;

	m_ui8MultiWeaponCount		= 10;

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

	//NET_MD5_KEY_SIZE
	m_strMD5Key[0][NET_MD5_KEY_SIZE] = 0; 
	m_strMD5Key[1][NET_MD5_KEY_SIZE] = 0; 

	m_i32ActiveClanIdx			= 0;

	m_ui32ControlServerIp		= 0;
	m_ui16ControlServerPort		= 0;

	m_i32PacketCount			= 0;
	m_i32PacketSize				= 0;
	m_ui64PacketOldCount		= 0;
	m_ui64PacketOldSize			= 0;

	m_ui64RecvPacketOldCount	= 0;
	m_ui64RecvPacketOldSize		= 0;
	m_ui64RecvPacketCount		= 0;
	m_ui64RecvPacketSize		= 0;
	m_ui64SendPacketOldCount	= 0;
	m_ui64SendPacketOldSize		= 0;
	m_ui64SendPacketCount		= 0;
	m_ui64SendPacketSize		= 0;
	m_i32Average_Min			= 0;
	m_i32AverageCCU_Min			= 0;
	m_i32AverageRoom_Min		= 0;

	m_i32UserPacketIdx = 0;
	m_ppui64UserPacketOldCount	= NULL;
	m_ppr64UserPacketOldTime	= NULL;
	m_ppui64UserPacketCount		= NULL;
	m_ppr64UserPacketTime		= NULL;

	i3mem::FillZero( &m_DailyGift, sizeof(DAILY_GIFT) );

	//m_pi32DailyGiftIdx		= NULL;
	//m_i32DailyGiftCount		= 0;
	//m_ppDailyGiftItem		= NULL;

	// PC_CAFE_BONUS
	//m_i32PCCafeBonusActive = 0;
	//i3mem::FillZero( m_stPCCafeBouns, sizeof(TYPE_PC_CAFE_BONUS)*PC_MAX_COUNT );

	// EVENT_PC_CAFE_BONUS
	//m_i32EventPCCafeBonusActive = 0;
	//i3mem::FillZero( m_stEventPCCafeBouns, sizeof(TYPE_EVENT_PC_CAFE_BONUS)*PC_MAX_COUNT );

	// Event
	m_ui16BoostEventCount		= 0;
	i3mem::FillZero( m_stBoostEvent, sizeof(BOOST_EVENT)*BOOST_EVENT_ALL_LOCALE_COUNT );
	
	// 임무카드	
	i3mem::FillZero( m_pQuest,					sizeof( QUEST_CTX ) * (QUEST_CARDSET_TYPE_COUNT)*MAX_QUEST_PER_CARDSET_NEW );
	i3mem::FillZero( m_pQuest_ItemInfo,			sizeof( QUEST_ITEM_INFO_NEW2 ) * ( QUEST_CARDSET_TYPE_COUNT ) );
	i3mem::FillZero( m_ui16QuestComplete,		sizeof( UINT16 ) * (QUEST_CARDSET_TYPE_COUNT)*MAX_CARD_PER_CARDSET );

	i3mem::FillZero( m_pQuestEvent,				sizeof( QUEST_CTX ) * ( LOCALE_CODE_MAX )*MAX_QUEST_PER_CARDSET_NEW );
	i3mem::FillZero( m_pQuestEvent_ItemInfo,	sizeof( QUEST_ITEM_INFO_NEW2 ) * ( LOCALE_CODE_MAX ) );
	i3mem::FillZero( m_ui16QuestEventComplete,	sizeof( UINT16 ) * ( LOCALE_CODE_MAX )*MAX_CARD_PER_CARDSET );

	i3mem::FillZero( m_aRankUpItem, sizeof(RANKUP_ITEM) * MAX_RANK_COUNT);
	i3mem::FillZero( m_stTSEvent, sizeof(TS_EVENT_INFO) * TS_EVENT_COUNT);
	
	i3mem::FillZero( &m_stSelectedTSEvent, sizeof(TS_EVENT_INFO));
	m_ui8GoodsOrder = 0;

	i3mem::FillZero( &m_ui32QuestCardGoodsID, sizeof(UINT32) * QUEST_CARDSET_TYPE_COUNT);
	//임무카드 GoodsId 
	for(int i = 0 ; i < QUEST_CARDSET_TYPE_COUNT; i++)
	{
		if(i < QUEST_CARDSET_TYPE_GOLD) continue;
		if(i != QUEST_CARDSET_TYPE_EVENT )
		{
			m_ui32QuestCardGoodsID[i] = 260000000 + i*100;
		}
	}
	// 이벤트 임무카드명
	for( UINT8 i = 0; i < LOCALE_CODE_MAX; ++i )
	{
		i3mem::FillZero(m_astrEventQuestCard[i],	MAX_STRING_COUNT );
	}


	m_strReportFtpAddress[0]	= '\0';
	m_ui16ReportFtpPort = 0;

	i3mem::FillZero( m_astrBillingURL, sizeof(char) * LINK_METHOD_MAX * MAX_BILLING_URL );

	i3mem::FillZero( m_DefaultItemSet, sizeof(DEFAULT_ITEM_SET)*DEFAULT_ITEM_TYPE_MAX );

	m_ui8URLListCount				= 0;
	i3mem::FillZero(m_ulURLList, sizeof(URL_LIST)*URL_MAX_COUNT);

	//////////////////////////////////////////////////////////////////////////
	// v1.5 Medal
	i3mem::FillZero(&m_MedalSystemInfo, sizeof(MEDAL_SYSTEM_INFO) ); // 메달 시스템 설정 정보
	
	for (INT32 i = 0; i < LOCALE_CODE_MAX; ++i)
	{
		i3mem::FillZero(&m_MQFMedal[i], sizeof(MQF_ALL_MEDAL_INFO)); // 전체 메달 MQF 정보.
		m_MQFMedal[i].Reset();	// 메달 인덱스 초기화
	}

	i3mem::FillZero(&m_MQFAllDailyMedalSet, sizeof(MQF_ALL_DAILY_MEDAL_SET_INFO) ); 
	for(UINT16 i = 0 ; i < MAX_LOAD_DAILY_MEDAL_SET_COUNT; i++ )
	{
		m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].Reset();
	}

	i3mem::FillZero(&m_MQFAllEventMedalSet, sizeof(MQF_ALL_EVENT_MEDAL_SET_INFO) );
	for(UINT16 i = 0 ; i < MAX_LOAD_EVENT_MEDAL_SET_COUNT; i++ )
	{
		m_MQFAllEventMedalSet.m_aEventMedalSetInfo[i].Reset();
	}

	// 스킬 시스템 개방 조건 세팅
	SetMainSkillInfo();
	SetAssistSkillInfo();
	SetCommonSkillInfo();
	
	m_bAASMethodAppointed	= false;

	i3mem::FillZero( &m_GiftBuyRanking, sizeof( GIFT_BUY_RANKING ) );

	m_i32SeasonRankingCount = 0;
	i3mem::FillZero( m_aSeasonRankingClan, sizeof( CLAN_BASIC_INFO ) * MAX_SEASON_RANKING_TEAM_COUNT );

	m_i32MatchedTeamCount = 0;
	m_i32MatchingTeamCount = 0;

	//장성전용 지급 아이템 ---- 유동적으로 변경할경우 DB로 올리는것도 생각해 봅시다.
	i3mem::FillZero( m_aaui32GeneralItem, sizeof(UINT32) * MAX_GENERAL_ITEM_COUNT );
	m_ui8GeneralItemCount		= 0;	// 잠시 막습니다. 2015.07.20 레몬사탕
	//m_aaui32GeneralItem[0]		= MAKE_ITEM_ID(ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SPECIAL_GENERAL_BERET);
}

CContextMain::~CContextMain(void)
{
	I3_SAFE_RELEASE(m_pShop);

	for( INT32 i = 0 ; i < CLAN_LIST_COUNT ; i++ )
	{
		I3MEM_SAFE_FREE( m_ClanList[i] );
	}

	//m_pAddressIp
	I3MEM_SAFE_FREE( m_pui32ActiveQuest );
	I3MEM_SAFE_FREE( m_pui32ProxyAddressIp );
	I3MEM_SAFE_FREE( m_pui32AddressIp );
	I3MEM_SAFE_FREE( m_pui16AddressPort	);
	I3MEM_SAFE_FREE( m_pui8AddressTimeOut );
	I3MEM_SAFE_FREE( m_pChannelType );

	//for( INT32 i = 0 ; i < m_i32DailyGiftCount ; i++ )
	//{
	//	I3_SAFE_RELEASE( m_ppDailyGiftItem[i] );
	//}
	//I3MEM_SAFE_FREE( m_ppDailyGiftItem );

	//I3MEM_SAFE_FREE( m_pi32DailyGiftIdx	);

	for (INT32 i = 0; i < MAX_PROTOCOL_BASE_VALUE; i++)
	{
		I3MEM_SAFE_FREE(m_ppui64UserPacketOldCount[i]);
		I3MEM_SAFE_FREE(m_ppr64UserPacketOldTime[i]);
		I3MEM_SAFE_FREE(m_ppui64UserPacketCount[i]);
		I3MEM_SAFE_FREE(m_ppr64UserPacketTime[i]);
	}
	I3MEM_SAFE_FREE(m_ppui64UserPacketOldCount);
	I3MEM_SAFE_FREE(m_ppr64UserPacketOldTime);
	I3MEM_SAFE_FREE(m_ppui64UserPacketCount);
	I3MEM_SAFE_FREE(m_ppr64UserPacketTime);

	if( m_hGameGuard )
	{
		FreeLibrary( m_hGameGuard );
	}
}

BOOL CContextMain::Create()
{
	m_ui32ZLogWrite = 0;

	m_ppui64UserPacketOldCount = (UINT64**)i3MemAlloc(sizeof(UINT64*) * MAX_PROTOCOL_BASE_VALUE);
	m_ppr64UserPacketOldTime = (REAL64**)i3MemAlloc(sizeof(REAL64*) * MAX_PROTOCOL_BASE_VALUE);
	m_ppui64UserPacketCount = (UINT64**)i3MemAlloc(sizeof(UINT64*) * MAX_PROTOCOL_BASE_VALUE);
	m_ppr64UserPacketTime = (REAL64**)i3MemAlloc(sizeof(REAL64*) * MAX_PROTOCOL_BASE_VALUE);

	for (INT32 i = 0; i < MAX_PROTOCOL_BASE_VALUE; ++i)
	{
		m_ppui64UserPacketOldCount[i]	= (UINT64*)i3MemAlloc(sizeof(UINT64) * MAX_USER_THREAD_COUNT);
		i3mem::FillZero(m_ppui64UserPacketOldCount[i], sizeof(UINT64)*MAX_USER_THREAD_COUNT);

		m_ppr64UserPacketOldTime[i]	= (REAL64*)i3MemAlloc(sizeof(REAL64) * MAX_USER_THREAD_COUNT);
		i3mem::FillZero(m_ppr64UserPacketOldTime[i], sizeof(REAL64)*MAX_USER_THREAD_COUNT);

		m_ppui64UserPacketCount[i]		= (UINT64*)i3MemAlloc(sizeof(UINT64) * MAX_USER_THREAD_COUNT);
		i3mem::FillZero(m_ppui64UserPacketCount[i], sizeof(UINT64)*MAX_USER_THREAD_COUNT);

		m_ppr64UserPacketTime[i]		= (REAL64*)i3MemAlloc(sizeof(REAL64) * MAX_USER_THREAD_COUNT);
		i3mem::FillZero(m_ppr64UserPacketTime[i], sizeof(REAL64)*MAX_USER_THREAD_COUNT);
	}

	return TRUE;
}

BOOL CContextMain::Init(void)
{
	m_pShop = CShop::NewObject();
	if( NULL == m_pShop )	return FALSE;

	// 국가별 무기 세팅
	for( INT32 i = 0 ; i < LOCALE_CODE_MAX ; i++ )
	{
		m_TBasicWeapon[i][ CHAR_EQUIPMENT_WEAPON_PRIMARY   ]		= 103004;					// K2
		m_TBasicWeapon[i][ CHAR_EQUIPMENT_WEAPON_SECONDARY ]		= 202003;					// K5 : MAKE_ITEM_ID( ITEM_TYPE_SECONDARY,	WEAPON_CLASS_HANDGUN, 3)	
		m_TBasicWeapon[i][ CHAR_EQUIPMENT_WEAPON_MELEE     ]		= 301001;					// MAKE_ITEM_ID( ITEM_TYPE_MELEE,		WEAPON_CLASS_KNIFE, 1)			//  301001 : M7
		m_TBasicWeapon[i][ CHAR_EQUIPMENT_WEAPON_THROWING1 ]		= 407001;					// MAKE_ITEM_ID( ITEM_TYPE_THROWING1,	WEAPON_CLASS_THROWING1, 1)		//  407001 : K400
		m_TBasicWeapon[i][ CHAR_EQUIPMENT_WEAPON_THROWING2 ]		= 508001;					// MAKE_ITEM_ID( ITEM_TYPE_THROWING2,	WEAPON_CLASS_THROWING2, 1)		//  508001 : SMOKE

		m_TBasicCommonEquip[i][ CHAR_EQUIPMENT_COMMON_DINO ]		= 1500511;					// MAKE_ITEM_ID( ITEM_TYPE_DINO,		CHARACTER_CLASS_CHARA_DINO,	CHARA_RES_ID_DINO_CC_RAPTOR);		// CC랩터

	}	

	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		switch( i )
		{	// 기본 장착 파츠가 없는 경우
		case CHAR_EQUIPMENT_PARTS_CHARA:
		//case CHAR_EQUIPMENT_PARTS_SKIN:
		case CHAR_EQUIPMENT_PARTS_BERET:
			m_TBasicEquip[i].m_TItemDBIdx	= 0;
			m_TBasicEquip[i].m_TItemID		= 0;
			continue;
		}
		m_TBasicEquip[i].m_TItemID		= MAKE_DEFAULT_PARTS_ITEMID( i + ITEM_TYPE_CHARA);
		m_TBasicEquip[i].m_TItemDBIdx	= MAKE_DEFAULT_EQUIP_DBIDX( i + ITEM_TYPE_CHARA);
	}

	return TRUE;
}

UINT32 CContextMain::GetFieldShopVersion()
{
	return m_FieldShop.m_ui32Version;
}

BOOL CContextMain::CheckMD5Key(char * pKey)
{
	if( i3String::Compare(pKey, m_strMD5Key[0]) == 0)return TRUE; 
	if( i3String::Compare(pKey, m_strMD5Key[1]) == 0)return TRUE; 

	return FALSE; 
}

INT32 CContextMain::LoadLogInfo( const char * strFileName )
{
	// Log를 기록 할 File 경로를 찾습니다.
	const char*	pstrValue = NULL;

	char strDrive[SERVER_STRING_COUNT];
	i3System::GetDrive(g_pCCurrentPath, strDrive, SERVER_STRING_COUNT);
	
	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ))		goto e_GameFail;

	// 1. Game Log
	if( FALSE == Parser.ReadSection( "Common" ) )			goto e_GameFail;
	
	pstrValue = Parser.GetStringValue( "LogFilePath" );
	if( NULL == pstrValue )									goto e_GameFail;
	::MultiByteToWideChar( CP_ACP, NULL, pstrValue, -1, m_wstrLogFilePath, SERVER_STRING_COUNT );

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_wstrLogFilePath))		goto e_GameFail;
	

	// 2. ZLog
	if (FALSE == Parser.ReadSection("LogD"))				goto e_ZLogFail;

	pstrValue = Parser.GetStringValue("ZLogFilePath");
	if (NULL == pstrValue)									goto e_ZLogFail;
	i3String::Copy(m_strZLogFilePath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strZLogFilePath))		goto e_ZLogFail;


	// 3. ZLog BackUp
	pstrValue = Parser.GetStringValue("ZLogBackupPath");
	if (NULL == pstrValue)									goto e_ZLogBackUpFail;
	i3String::Copy(m_strZLogBackupPath, pstrValue, SERVER_STRING_COUNT);

	// 정상적으로 로그가 생성 되었습니다.
	if (!i3System::PrepareDirectory(m_strZLogBackupPath))	goto e_ZLogBackUpFail;

	return SERVER_ERROR_NO;

e_GameFail:
	wchar_t wstrPath[SERVER_STRING_COUNT];
	MultiByteToWideChar(CP_ACP, NULL, g_pCCurrentPath, -1, wstrPath, SERVER_STRING_COUNT);

	wchar_t wstrDrive[SERVER_STRING_COUNT];
	i3System::GetDrive(wstrPath, wstrDrive, SERVER_STRING_COUNT);
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_wstrLogFilePath, SERVER_STRING_COUNT, L"%s\\%s\\%s", wstrDrive, L"PBLog", L"Game");

	if (FALSE == i3System::PrepareDirectory(m_wstrLogFilePath))	return SERVER_ERROR_LOGFILE;


e_ZLogFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strZLogFilePath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "Game_ZLog");

	if (FALSE == i3System::PrepareDirectory(m_strZLogFilePath))	return SERVER_ERROR_LOGFILE;


e_ZLogBackUpFail:
	// 로그파일이 없거나, 해당 폴더가 없을 경우나 폴더를 생성하지 못했을 때 임의로 폴더를 생성합니다. ( 해당폴더\\PBLog\\서버이름 )
	i3String::Format(m_strZLogBackupPath, SERVER_STRING_COUNT, "%s\\%s\\%s", strDrive, "PBLog", "Game_ZLogBackup");

	if (FALSE == i3System::PrepareDirectory(m_strZLogBackupPath))	return SERVER_ERROR_LOGFILE;


	return SERVER_ERROR_NO;
}

BOOL CContextMain::LoadConfigFile( const char * strFileName )
{
	const char* pStringValue;
	INT32	i32Temp = 0;

	i3IniParser Parser;
	if( FALSE == Parser.OpenFromFile( strFileName ) )							return FALSE;
	
	if( Parser.ReadSection("Common"))
	{
		Parser.GetValue( "Gateway",			1,					&m_i32Gateway);

		if( FALSE == Parser.GetValue( "UdpClientPort",	29890,	&i32Temp) )		return FALSE;
		m_ui16ClientPort = (UINT16)i32Temp;

		Parser.GetValue( "GameGuard",		GAMEGUARD_NONE, &m_i32GameGuard);

		Parser.GetValue("AbusingCheck",		0,				&i32Temp); 
		m_bAbusingCheck = (i32Temp==0?FALSE:TRUE);

		Parser.GetValue("LoginCheck",		3,				&m_ui32LoginCheckTime); 
		
		pStringValue	= Parser.GetStringValue("ServerPass");
		if( pStringValue )
		{
			i3String::Copy( m_strEnterPass, pStringValue, NET_SERVER_PW );
		}
	}
	else return FALSE; 

	if( Parser.ReadSection("Control") )
	{
		pStringValue = Parser.GetStringValue("ControlServerIp");
		if( NULL == pStringValue )												return FALSE;
		m_ui32ControlServerIp = ::inet_addr( pStringValue );
				
		Parser.GetValue( "ControlServerPort", 39120,	&i32Temp);
		m_ui16ControlServerPort = (UINT16)i32Temp; 
	}
	else return FALSE;

	//게임서버는 특성상 컨트롤서버로 부터 특정 정보들을 더 가져오기 때문에 해당 변수들 초기화를 위해...
	if( Parser.ReadSection("Address") )
	{
		char strTemp[ SERVER_STRING_COUNT ]; 
		Parser.GetValue( "Count",		1,		&i32Temp);
		m_ui8AddressCount = (UINT8)i32Temp;

		m_pui32AddressIp		= (UINT32*)i3MemAlloc(sizeof(UINT32) * m_ui8AddressCount);
		m_pui16AddressPort		= (UINT16*)i3MemAlloc(sizeof(UINT16) * m_ui8AddressCount);
		m_pui8AddressTimeOut	= (UINT8*)i3MemAlloc(sizeof(UINT8) * m_ui8AddressCount);
		m_pui32ProxyAddressIp	= (UINT32*)i3MemAlloc(sizeof(UINT32) * m_ui8AddressCount);

		for(UINT8 i = 0; i < m_ui8AddressCount; i++)
		{
			i3String::Format( strTemp, SERVER_STRING_COUNT, "Ip%d", i ); 
			pStringValue = Parser.GetStringValue( strTemp );
			if( NULL == pStringValue )											return FALSE;

			m_pui32AddressIp[i]				= ::inet_addr( pStringValue );
			
			i3String::Format( strTemp, SERVER_STRING_COUNT, "ProxyIp%d", i ); 
			pStringValue = Parser.GetStringValue( strTemp );
			if( pStringValue )
			{
				m_pui32ProxyAddressIp[i]	= ::inet_addr( pStringValue );
			}
			else
			{
				m_pui32ProxyAddressIp[i]	= m_pui32AddressIp[i];
			}

			i3String::Format( strTemp, SERVER_STRING_COUNT, "Port%d", i );
			Parser.GetValue( strTemp,	39190,		&i32Temp);
			m_pui16AddressPort[i] = (UINT16)i32Temp;

			i3String::Format( strTemp, SERVER_STRING_COUNT, "TimeOut%d", i );
			Parser.GetValue( strTemp,	5,			&i32Temp);
			m_pui8AddressTimeOut[i] = (UINT8)i32Temp;
		}
	}
	else return FALSE; 
	
	if( Parser.ReadSection("Clan"))
	{
		Parser.GetValue("ClanActive", i32Temp, &i32Temp);	
		m_bClanActive = (i32Temp == 0 ? FALSE : TRUE );
	}
	else return FALSE; 

	if (Parser.ReadSection("Messenger"))
	{
		Parser.GetValue("MessengerActive", i32Temp, &i32Temp);	
		m_bMessengerActive = (i32Temp == 0 ? FALSE : TRUE );
	}
	else return FALSE;

	if( Parser.ReadSection("LogD"))
	{
		Parser.GetValue( "LogDActive", i32Temp, &i32Temp);
		m_bLogDActive = (i32Temp == 0 ? FALSE : TRUE );

		Parser.GetValue( "ZLogBackupKeep", 28, &m_i32ZLogKeepDay );

		Parser.GetValue("ZLogWrite", 0, &m_ui32ZLogWrite);
		
	}
	else return FALSE; 

	m_i32UpdateMainTime = 24;		//한번 업데이트 할때마다 한 채널을 업데이트 합니다. 만약 채널이 10개이고 업데이트 타임이 1000(1초)이면 10초가 되어야 모든 채널의 로테이트가 가능합니다.

	if( Parser.ReadSection("Notice"))
	{
		pStringValue = Parser.GetStringValue("NormalText");
		if( pStringValue )
		{
			i3String::CopyASCIItoWCHAR( m_strNormalNotice, pStringValue );
		}

		pStringValue = Parser.GetStringValue("EmergencyText");
		if( pStringValue )
		{
			i3String::CopyASCIItoWCHAR( m_strEmergencyNotice, pStringValue );
		}
	}
	else return FALSE; 

	m_bMacAddressCheck = FALSE;
	m_i32HackKickType = 0;
	if( Parser.ReadSection("OPTION"))
	{
		if ( Parser.GetValue( "MacAddressCheck", i32Temp, &i32Temp) )
			m_bMacAddressCheck = (BOOL)i32Temp;

		if (Parser.GetValue("HackKickType", 0, &i32Temp))
			m_i32HackKickType = i32Temp;
	}
	
	Parser.Close();

	return TRUE;
}

BOOL CContextMain::LoadGameGuard( const char * strFilePath )
{
	char strFileName[ SERVER_STRING_COUNT ];

	switch( m_i32GameGuard )
	{
		case GAMEGUARD_NPROTECT:		// nProtect
			{
#ifdef USE_NPROTECT
				//i3String::Format( strFileName, SERVER_STRING_COUNT, "%s%s", strFilePath, "\\ggsrv30lib_MTd.lib" );

				m_hGameGuard = LoadLibrary( strFileName );
				if( NULL == m_hGameGuard )		return FALSE;
				g_pInitCSAuth3 = (FnInitCSAuth3)GetProcAddress( m_hGameGuard, "_InitCSAuth3" );
				if( NULL == g_pInitCSAuth3 )	return FALSE;
				g_pCloseCSAuth3	= (FnCloseCSAuth3)GetProcAddress( m_hGameGuard, "_CloseCSAuth3" );
				if( NULL == g_pCloseCSAuth3 )	return FALSE;
				g_pCsAuth3 = (FnCsAuth3)GetProcAddress( m_hGameGuard, "CCSAuth3" );
				if( NULL == g_pCsAuth3 )		return FALSE;

				char strErrorLog[ SERVER_STRING_COUNT ];
				i3String::Format( strFileName, SERVER_STRING_COUNT, "%s\\nProtect", strFilePath );
				UINT32 ui32Return = g_pInitCSAuth3( strFileName );
				if( ERROR_SUCCESS != ui32Return )
				{
					g_pLog->WriteLog( strErrorLog, L"[ ERROR ] NProtect Load Fail - %d", ui32Return );
					return FALSE; 
				}
				g_pLog->WriteLog( L"[SUCCESS] NProtect Load");
#endif
			}
			break;
		case GAMEGUARD_HACKSHIELD:		// Ahn HackShield
			{
				// Load Dll
				i3String::Format( strFileName, SERVER_STRING_COUNT, "%s%s", strFilePath, "\\AntiCpXSvr.dll" );

				m_hGameGuard = LoadLibrary( strFileName );
				if( NULL == m_hGameGuard )
				{
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Lib Load Fail" );
					return FALSE;
				}
				g_pAhnHS_CreateServerObject = (FnAhnHS_CreateServerObject)GetProcAddress( m_hGameGuard, "__AhnHS_CreateServerObject@4" );
				if( NULL == g_pAhnHS_CreateServerObject )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 1" );
					return FALSE;
				}
				g_pAhnHS_CloseServerHandle	= (FnAhnHS_CloseServerHandle)GetProcAddress( m_hGameGuard, "__AhnHS_CloseServerHandle@4" );
				if( NULL == g_pAhnHS_CloseServerHandle )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 2" );
					return FALSE;
				}
				g_pAhnHS_CreateClientObject	= (FnAhnHS_CreateClientObject)GetProcAddress( m_hGameGuard, "__AhnHS_CreateClientObject@4" );
				if( NULL == g_pAhnHS_CreateClientObject )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 3" );
					return FALSE;
				}
				g_pAhnHS_CloseClientHandle	= (FnAhnHS_CloseClientHandle)GetProcAddress( m_hGameGuard, "__AhnHS_CloseClientHandle@4" );
				if( NULL == g_pAhnHS_CloseClientHandle )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 4" );
					return FALSE;
				}
				g_pAhnHS_MakeRequest		= (FnAhnHS_MakeRequest)GetProcAddress( m_hGameGuard, "__AhnHS_MakeRequest@8" );
				if( NULL == g_pAhnHS_MakeRequest )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 5" );
					return FALSE;
				}
				g_pAhnHS_VerifyResponseEx	= (FnAhnHS_VerifyResponseEx)GetProcAddress( m_hGameGuard, "__AhnHS_VerifyResponseEx@16" );
				if( NULL == g_pAhnHS_VerifyResponseEx )
				{	// 보안을 위해 함수 명을 적지 않습니다.
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield Fn Load Fail 6" );
					return FALSE;
				}
				g_pLog->WriteLog( L"[SUCCESS] AhnHackShield Lib Load" );

				// Load HSB
				i3String::Format( strFileName, SERVER_STRING_COUNT, "%s%s", strFilePath, "\\HackShield\\AntiCpX.hsb" );

				g_hAhnhsServer = g_pAhnHS_CreateServerObject( strFileName ); // .hsb 파일 경로
				if( ANTICPX_INVALID_HANDLE_VALUE == g_hAhnhsServer )
				{
					g_pLog->WriteLog( L"[ ERROR ] AhnHackShield HSB Load Fail" );
					return FALSE;
				}
				g_pLog->WriteLog( L"[SUCCESS] AhnHackShield HSB Load" );
			}
			break;
		case GAMEGUARD_XTRAP:		// Xtrap
			{
				i3String::Format( strFileName, SERVER_STRING_COUNT, "%s%s", strFilePath, "\\XTrap\\PointBlank.CS3" );

				g_pXTrapModule = new CXTrapModule;
				if( NULL == g_pXTrapModule )
				{
					g_pLog->WriteLog( L"[ ERROR ] XTrap Init Memory" );
					return FALSE;
				}
				if( FALSE == g_pXTrapModule->OnCreate( strFileName ) )
				{
					g_pLog->WriteLog( L"[ ERROR ] XTrap Load Fail" );
					return FALSE;
				}
				g_pLog->WriteLog( L"[SUCCESS] XTrap Load" );
			}
			break;
		case GAMEGUARD_GPK:
			{
			}
			break;
		case GAMEGUARD_XIGNCODE:	
			{	 
				char strPath[ MAX_PATH];
				 
				i3String::Copy( strPath, strFilePath, sizeof(strPath)); 		
				i3String::Concat(strPath, "\\XignCode\\Dll");

				g_pXignCode = new CXignCode;
				if( !g_pXignCode->OnCreate(strPath) ) 
				{
					g_pLog->WriteLog( L"[START] Error GameGuard XIGNCODE / OnCreate" );
					return FALSE;
				}
				 
				g_pLog->WriteLog( L"[START] Success GameGuard XIGNCODE" );
			}
			break;
	}

	return TRUE;
}

void CContextMain::OnUpdate()
{
	// 1초가 지났는지 체크
	if (m_i32UpdateTimeSS == i3ThreadTimer::LocalTime().GetSecond())		return;
	m_i32UpdateTimeSS = i3ThreadTimer::LocalTime().GetSecond();

	// 1초마다 패킷 체크
	UINT64 ui64Count;
	UINT64 ui64Size;
	UINT64 ui64SendCount;
	UINT64 ui64SendSize;
	g_pServerContext->GetPacketInfo(&ui64Count, &ui64Size, &ui64SendCount, &ui64SendSize);
	m_i32PacketCount = INT32(ui64Count - m_ui64PacketOldCount);
	m_i32PacketSize = INT32(ui64Size - m_ui64PacketOldSize);
	m_ui64PacketOldCount = ui64Count;
	m_ui64PacketOldSize = ui64Size;

	// 업데이트 한지 1분이 되지 않았으면 그냥 지나간다.
	if (i3ThreadTimer::LocalTime().GetMinute() == m_i32LastUpdateTime)	return;
	m_i32LastUpdateTime = i3ThreadTimer::LocalTime().GetMinute();

	if (g_pServerContext && g_pRoomManager)
	{
		++m_i32Average_Min;
		m_i32AverageCCU_Min		+= g_pServerContext->GetSessionCount();
		m_i32AverageRoom_Min	+= g_pRoomManager->GetUseRoomCount();
	}

	_UpdateBoostEvent();
	_UpdateAAS();

	INT32 i = 0;
	for (i = 0; i < TS_EVENT_COUNT; i++)
	{
		if (m_stTSEvent[i]._ui32StartDate == 0) continue;

		if (i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() >= m_stTSEvent[i]._ui32StartDate  && i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() < m_stTSEvent[i]._ui32EndDate)
		{
			tm InveneDate, TSDate;

			InveneDate.tm_year = i3ThreadTimer::LocalTime().GetYear();
			InveneDate.tm_mon = i3ThreadTimer::LocalTime().GetMonth();
			InveneDate.tm_mday = i3ThreadTimer::LocalTime().GetDay();
			InveneDate.tm_hour = i3ThreadTimer::LocalTime().GetHour();
			InveneDate.tm_min = i3ThreadTimer::LocalTime().GetMinute();
			InveneDate.tm_sec = i3ThreadTimer::LocalTime().GetSecond();
			InveneDate.tm_year -= 1900;
			InveneDate.tm_mon -= 1;
			InveneDate.tm_isdst = SUMMER_TIME_FOLLOW_SYSTEM;

			ConvertDate(m_stTSEvent[i]._ui32StartDate, TSDate);

			time_t CurTime = mktime(&InveneDate);
			time_t StartTime = mktime(&TSDate);

			m_ui8GoodsOrder = (UINT8)((CurTime - StartTime) / 86400);
			m_stSelectedTSEvent = m_stTSEvent[i];
			break;
		}

	}

	if (TS_EVENT_COUNT == i)
	{
		i3mem::FillZero(&m_stSelectedTSEvent, sizeof(TS_EVENT_INFO));
	}

	// 중독방지시스템 시간인 m_ui16AASCurTime을 갱신한다.
	if (m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_DAILY_USAGE)
	{
		m_ui16AASCurTime = UINT16(i3ThreadTimer::LocalTime().GetHour() * 60 + i3ThreadTimer::LocalTime().GetMinute() - m_AASOption.m_ui16AASDailyUsageResetTime);
		if (m_ui16AASCurTime < 0) m_ui16AASCurTime += MINUTE_PER_DAY;
	}

	// 업데이트 한지 1시간이 되지 않았으면 그냥 지나간다.
	if (i3ThreadTimer::LocalTime().GetHour() == m_i32UpdateTimeHH)	return;
	m_i32UpdateTimeHH = i3ThreadTimer::LocalTime().GetHour();

	// IOCP 패킷 로그
	m_ui64RecvPacketCount = ui64Count - m_ui64RecvPacketOldCount;
	m_ui64RecvPacketSize = ui64Size - m_ui64RecvPacketOldSize;
	m_ui64RecvPacketOldCount = ui64Count;
	m_ui64RecvPacketOldSize = ui64Size;

	m_ui64SendPacketCount = ui64SendCount - m_ui64SendPacketOldCount;
	m_ui64SendPacketSize = ui64SendSize - m_ui64SendPacketOldSize;
	m_ui64SendPacketOldCount = ui64SendCount;
	m_ui64SendPacketOldSize = ui64SendSize;

	m_i32Average_Min = MAX(m_i32Average_Min, 1);
	
	g_pLog->WriteLog(L"[STATISTICS]\r\n"
		L"========== COPY DOWN =========== \r\n"
		L"{\"GameTCP\": \r\n"
		L"{\"Date\":\"%04d-%02d-%02d\",\r\n"
		L"\"AverageUserCount\":\"%d\",\r\n"
		L"\"AverageRoomCount\":\"%d\",\r\n"
		L"\"S_Count\":\"%I64d\",\r\n"
		L"\"S_Size\":\"%I64d\",\r\n"
		L"\"R_Count\":\"%I64d\",\r\n"
		L"\"R_Size\":\"%I64d\"\r\n"
		L"}}\r\n"
		L"========== COPY UP =========== \r\n",
		i3ThreadTimer::LocalTime().GetYear(),
		i3ThreadTimer::LocalTime().GetMonth(),
		i3ThreadTimer::LocalTime().GetDay(),
		m_i32AverageCCU_Min / m_i32Average_Min, 
		m_i32AverageRoom_Min/ m_i32Average_Min,
		m_ui64SendPacketCount,
		m_ui64SendPacketSize,
		m_ui64RecvPacketCount,
		m_ui64RecvPacketSize
		);

	m_i32Average_Min	= 0;
	m_i32AverageCCU_Min	= 0;
	m_i32AverageRoom_Min = 0;
 
	// 1시간동안 유저 패킷수와 처리 시간 로그 기록.
	UINT64 ui64UserPacketCount = 0;
	REAL64 r64UserPacketTime = 0.f; // sec
	UINT64 ui64TotalUserPacketCount = 0;
	REAL64 r64TotalUserPacketTime = 0.f; // sec

	UINT64** ppui64UserPacketCount;
	REAL64** ppr64UserPacketTime;

	if (0 == m_i32UserPacketIdx)
	{
		ppui64UserPacketCount = m_ppui64UserPacketOldCount;
		ppr64UserPacketTime = m_ppr64UserPacketOldTime;
	}
	else
	{
		ppui64UserPacketCount = m_ppui64UserPacketCount;
		ppr64UserPacketTime = m_ppr64UserPacketTime;
	}

	for (INT32 t = 0; t < MAX_PROTOCOL_BASE_VALUE; ++t)
	{
		ui64UserPacketCount = 0;
		r64UserPacketTime = 0.f;

		for (INT32 k = 0; k < MAX_USER_THREAD_COUNT; ++k)
		{
			ui64UserPacketCount += ppui64UserPacketCount[t][k];
			r64UserPacketTime += ppr64UserPacketTime[t][k];
		}

		ui64TotalUserPacketCount += ui64UserPacketCount;
		r64TotalUserPacketTime += r64UserPacketTime;

		if (0 < ui64UserPacketCount)
		{	// 추후 제거 예정
			g_pLog->WriteLog(L"--------- USER PACKET LOG[%d] IN AN HOUR / PROTOCOL : %d / COUNT : %I64d / TIME(Sec) : %.6lf", m_i32UserPacketIdx, t, ui64UserPacketCount, r64UserPacketTime);
		} 
	}

	g_pLog->WriteLog(L"--------- USER PACKET LOG[%d] TOTAL IN AN HOUR / COUNT : %I64d / TIME(Sec) : %.6lf", m_i32UserPacketIdx, ui64TotalUserPacketCount, r64TotalUserPacketTime);

	// 누적시키는 위치를 변경합니다.
	m_i32UserPacketIdx++;
	if (m_i32UserPacketIdx >= 2) m_i32UserPacketIdx = 0;

	// 초기화
	for (INT32 s = 0; s < MAX_PROTOCOL_BASE_VALUE; ++s)
	{
		i3mem::FillZero(ppui64UserPacketCount[s], sizeof(UINT64)*MAX_USER_THREAD_COUNT);
		i3mem::FillZero(ppr64UserPacketTime[s], sizeof(REAL64)*MAX_USER_THREAD_COUNT);
	}
}

void CContextMain::_UpdateBoostEvent()
{
	// 이벤트 Bouns
		
	BOOST_EVENT*		pBonus;
		
	m_UserEvent.Reset();

	for( INT32 i = 0 ; i < m_ui16BoostEventCount ; i++ )
	{
		pBonus = &m_stBoostEvent[i];

		if( i3ThreadTimer::LocalTime() > pBonus->m_dt32EndDate )	continue;
		if( i3ThreadTimer::LocalTime() < pBonus->m_dt32StartDate )	continue;
		if( USER_EVENT_COUNT <= pBonus->m_ui8LocaleCode )			continue;
		
		m_UserEvent.SetBoostValue( pBonus->m_ui8LocaleCode , (EVENT_BONUS_TYPE)pBonus->m_ui16Type, pBonus->m_ui32SubType, pBonus->m_ui32Exp, pBonus->m_ui32Point );
	}
	
	m_UserEvent.MakeChannelBonus();

	// Buffer Swap
	m_UserEvent.SwapBuffer();
}

void CContextMain::_UpdateAAS()
{
	m_bAASMethodAppointed = false;

	if ( !IsContentsEnabled( CONTENTS_SHUTDOWN ) )	return;

	INT32 i32AASTimeIdx = (i3ThreadTimer::LocalTime().GetHour()*2) + (i3ThreadTimer::LocalTime().GetMinute()/30);
	if ( m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_APPOINTED )
	{
		if ( m_AASOption.m_ui64AASAppointedTimeTable & ((UINT64)0x0000000000000001 << i32AASTimeIdx) )
			m_bAASMethodAppointed = true;
	}
}

BOOL CContextMain::ConvertDate(UINT32 SrcDate, tm &ConvertDate)
{
	ConvertDate.tm_year = SrcDate / 100000000 + 2000;
	ConvertDate.tm_mon	= (SrcDate % 100000000) / 1000000;
	ConvertDate.tm_mday = (SrcDate % 1000000) / 10000;
	ConvertDate.tm_hour = (SrcDate % 10000) / 100;
	ConvertDate.tm_min  = SrcDate % 100;
	ConvertDate.tm_sec = 0;


	ConvertDate.tm_year -= 1900;
	ConvertDate.tm_mon	-= 1;
	
	ConvertDate.tm_isdst = SUMMER_TIME_FOLLOW_SYSTEM;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CContextMain::GetQuest( UINT8 ui8CardType,	INT8 i8QuestSlotIdx, QUESTING_INFO * pQuestInfo, UINT8* pui8QuestCtxActive, UINT8 ui8LocaleCode )
{
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD)) return;
	//Set QUESTING_INFO	
	// 진행할 퀘스트 카드 세팅
	if( MAX_CARDSET_PER_USER <= i8QuestSlotIdx )		return;

	pQuestInfo->m_ActiveIndexUse						= i8QuestSlotIdx;	// 진행중인 카드를 새로 산 카드로 변경
	pQuestInfo->m_pActiveIndexCardSet[i8QuestSlotIdx]	= 0;				// 진행 상태를 첫번째 카드로 변경
	pQuestInfo->m_pCardSetType[i8QuestSlotIdx]			= ui8CardType;		// 진행 상태에 현재 카드셋 입력
	UINT16* pui16QuestComplete							= NULL;

	switch( ui8CardType )
	{
	case QUEST_CARDSET_TYPE_EVENT:
		if( LOCALE_CODE_MAX <= ui8LocaleCode )			return;
		pui16QuestComplete								= m_ui16QuestEventComplete[ ui8LocaleCode ];
		break;
	default:
		if( QUEST_CARDSET_TYPE_COUNT <= ui8CardType ) return;
		pui16QuestComplete								= m_ui16QuestComplete[ ui8CardType ];
		break;
	}
	if( NULL == pui16QuestComplete )					return;

	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		pQuestInfo->m_pActiveIndexCard[i8QuestSlotIdx][i]	= pui16QuestComplete[i];				// 각 카드의 진행상태를 세팅해준다.
	}
	i3mem::FillZero( pui8QuestCtxActive, sizeof(UINT8)*MAX_QUEST_PER_CARDSET_NEW );
}
 
//////////////////////////////////////////////////////////////////////////

BOOL CContextMain::GetChannelBonusInfo( UINT8 ui8LocaleCode, UINT8* pi8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus )
{
	return m_UserEvent.GetChannelBonusInfo( ui8LocaleCode, pi8ChannelBonusCount, pstChannelBonus );
}

BOOL CContextMain::GetRoomBonus( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point )
{
	return m_UserEvent.GetRoomBonus( ui8LocaleCode, eEvent, ui32SubType, pr32Exp, pr32Point );
}

BOOL CContextMain::IsPlayStage( STAGE_ID ui32StageID )
{
	for (UINT16 i = 0; i < m_ui16StageMatchingCount; ++i)
	{
		if (ui32StageID == m_aStageMatching[i].m_ui32StageID)
			return TRUE;
	}
	
	return FALSE;
}
// v 1.0  임무카드
BOOL	CContextMain::LoadEventQuestFile()
{
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD)) return true;

	char strTemp[SERVER_STRING_COUNT];
	i3mem::FillZero( strTemp, sizeof( SERVER_STRING_COUNT ) );
	i3String::Format(strTemp, SERVER_STRING_COUNT,  "%s\\%s", g_pCCurrentPath, "MissionCard\\QuestConfig.ini");	

	i3IniParser Parser;

	if( !Parser.OpenFromFile( strTemp ))
	{		
		g_pLog->WriteLog( L"[LoadEventQuestFile] QuestConfig.ini Open Fail");
		return FALSE;
	}

	// 이벤트 임무카드 설정 
	if( Parser.ReadSection("EventQuestCard"))
	{
		char strQuest[3];
		for( UINT8 i = 0; i < LOCALE_CODE_MAX; ++i )
		{
			i3String::Format( strQuest, 3, "%d", i );
			const char* pstrPass = Parser.GetStringValue( strQuest );
			if( pstrPass )
			{
				strcpy( m_astrEventQuestCard[i], pstrPass );
			}
		}
	}
	else 
	{
		g_pLog->WriteLog( L"[LoadEventQuestFile] Wrong QuestConfig.ini ");
		return FALSE;
	}

	Parser.Close();
	return TRUE;
}

BOOL CContextMain::LoadQuestFile(const char * pstrFileName)
{
	//임무카드 비활성화시 true 리턴
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD)) return true;

	INT32 i32Result = 0;

	char strFileName[SERVER_STRING_COUNT]; 

	// 일반 임무카드
	for( INT32 i = QUEST_CARDSET_TYPE_TUTORIAL ; i < QUEST_CARDSET_TYPE_COUNT ; i++ )
	{
		switch( i )
		{
		case QUEST_CARDSET_TYPE_TUTORIAL		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\TutorialCard.mqf");		break;
		case QUEST_CARDSET_TYPE_DINO_TUTORIAL	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\DIno_Tutorial.mqf");	break; 
		case QUEST_CARDSET_TYPE_HUMAN_TUTORIAL	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Human_Tutorial.mqf");	break; 		
		case QUEST_CARDSET_TYPE_GOLD			: continue;																		break; //삭제됨
		case QUEST_CARDSET_TYPE_ASSAULT			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\AssaultCard.mqf");		break; 			
		case QUEST_CARDSET_TYPE_BACKUP			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\BackUpCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_INFILTRATION	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\InfiltrationCard.mqf");	break; 
		case QUEST_CARDSET_TYPE_SPECIAL			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\SpecialCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_DEFCON			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\DefconCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_COMMISSIONED_O	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Commissioned_o.mqf");	break; 
		case QUEST_CARDSET_TYPE_COMPANY_O		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Company_o.mqf");		break; 
		case QUEST_CARDSET_TYPE_FIELD_O			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Field_o.mqf");			break; 
		case QUEST_CARDSET_TYPE_EVENT			: continue;
		case QUEST_CARDSET_TYPE_DINO_2ND		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Dino_Basic.mqf");		break; 
		case QUEST_CARDSET_TYPE_HUMAN_2ND		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Human_Basic.mqf");		break; 
		case QUEST_CARDSET_TYPE_DINO_3RD		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Dino_Intensify.mqf");	break; 
		case QUEST_CARDSET_TYPE_HUMAN_3RD		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "MissionCard\\Human_Intensify.mqf");	break; 
		default									:
			{
				g_pLog->WriteLog( L"[CContextMain::LoadQuestFile]잘못된 공통 임무카드 종류입니다.");
				return FALSE;
			}
			break;
		}

		i32Result = QuestFileLoadEx( strFileName, m_pQuest[i], &m_pQuest_ItemInfo[i] );

		if( 0 > i32Result )	
			return FALSE;
		else
		{
			QUEST_CTX * pQuest = m_pQuest[i];
			UINT16 * pui16QuestComplete = m_ui16QuestComplete[i];
			UINT16	ui16QuestComplete = 0;
			INT16	i16Shift = 0;
			for( INT32 k = 0 ; k < MAX_CARD_PER_CARDSET ; k++ )
			{
				for( INT32 j = 0 ; j < 16 ; j += 4 )
				{
					if( 0 == pQuest->m_ID )
					{	// 빈 임무카드는 0xF로 완료처리를 해 줍니다. // 20100101 김현우
						ui16QuestComplete = ui16QuestComplete | (UINT16)( 0xF << (4*i16Shift));
					}
					i16Shift++;
					pQuest++;
				}

				pui16QuestComplete[k]		= ui16QuestComplete;
			}
		}
	}

	// Event 임무카드
	for( INT32 i = 0 ; i < LOCALE_CODE_MAX ; i++ )
	{
		i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, m_astrEventQuestCard[i] );

		i32Result = QuestFileLoadEx( strFileName, m_pQuestEvent[i], &m_pQuestEvent_ItemInfo[i] );

		if( 0 > i32Result )
		{
			g_pLog->WriteLog( L"[CContextMain::LoadQuestFile] Error Event Card Setting = LOCALE CODE %d", i );
			continue;
		}
		else
		{
			QUEST_CTX * pQuest = m_pQuestEvent[i];
			UINT16 * pui16QuestComplete = m_ui16QuestEventComplete[i];
			UINT16	ui16QuestComplete = 0;
			INT16	i16Shift = 0;
			for( INT32 k = 0 ; k < MAX_CARD_PER_CARDSET ; k++ )
			{
				for( INT32 j = 0 ; j < 16 ; j += 4 )
				{
					if( 0 == pQuest->m_ID )
					{	// 빈 임무카드는 0xF로 완료처리를 해 줍니다. // 20100101 김현우
						ui16QuestComplete = ui16QuestComplete | (UINT16)( 0xF << (4*i16Shift));
					}
					i16Shift++;
					pQuest++;
				}

				pui16QuestComplete[k]		= ui16QuestComplete;
			}
		}
	}
	return TRUE; 
}

// v1.5 Medal
BOOL CContextMain::LoadMedalFile(const char * strCurPath)
{
	// 시스템 비활성화시 강제 성공.
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) ) return TRUE;

	char strDailyMedalSetFolder[MAX_PATH]; 
	char strDailyMedalSetFolderPath[MAX_PATH]; 

	MEDAL_SET_FILE_NAME aDailyMedalSetFileName[MAX_LOAD_DAILY_MEDAL_SET_COUNT];
	i3mem::FillZero( aDailyMedalSetFileName, sizeof( MEDAL_SET_FILE_NAME ) * MAX_LOAD_DAILY_MEDAL_SET_COUNT );


	char strEventMedalSetFolder[MAX_PATH]; 
	char strEventMedalSetFolderPath[MAX_PATH]; 

	MEDAL_SET_FILE_NAME aEventMedalSetFileName[MAX_LOAD_EVENT_MEDAL_SET_COUNT];
	i3mem::FillZero( aEventMedalSetFileName, sizeof( MEDAL_SET_FILE_NAME ) * MAX_LOAD_EVENT_MEDAL_SET_COUNT );
	
	char strDefaultMedalFileName[SERVER_STRING_COUNT];
	i3String::Format(strDefaultMedalFileName, SERVER_STRING_COUNT, "%s\\%s", strCurPath, NORMAL_MEDAL_PATH_NONE);

	INT32 i32Rv = EVENT_ERROR_SUCCESS;
	// 메달 로드
	{
		for (INT32 i = 0; i < LOCALE_CODE_MAX; ++i)
		{
			char strMedalFileName[SERVER_STRING_COUNT];

			i3String::Format(strMedalFileName, SERVER_STRING_COUNT, "%s\\%s", strCurPath, g_strNomalMedalPath[i]);
			i32Rv = LoadMedal(i, strMedalFileName);

			switch (i32Rv)
			{
			case EVENT_ERROR_SUCCESS:
			{
				g_pLog->WriteLog(L"[CContextMain::LoadMedalFile] Load NormalMedal Success / Locale : %d  ", i); 
			}
			break;
			case EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_OPEN:
			{
				// 해당 국가 파일 미존재지 디폴트 파일명으로 다시 로드합니다.
				i32Rv = LoadMedal(i, strDefaultMedalFileName);
				if (EV_FAILED(i32Rv))
				{
					g_pLog->WriteLog(L"[CContextMain::LoadMedalFile] Fail Load Medal / Locale : %d / Error Code : %x", i, i32Rv );
					return FALSE;
				}

				g_pLog->WriteLog(L"[CContextMain::LoadMedalFile] Load NormalMedal Success / Locale : %d / Default NormalMedal ", i);
			}
			break;
			default:
			{
				g_pLog->WriteLog(L"[CContextMain::LoadMedalFile] Fail Load Medal / Locale : %d / Error Code : %x", i, i32Rv);

				return FALSE;
			}
			break;
			}
		}
	}


	// 일일 메달 셋트 로드.
	{
		// Medal\\Daily 폴더에 있는 .mqfex 확장자 파일들을 검색해서 리스트를 만든다.
		i3String::Format(strDailyMedalSetFolder, MAX_PATH,  "%s\\%s", strCurPath, "Quest\\Daily\\");	
		i3String::Format(strDailyMedalSetFolderPath, MAX_PATH,  "%s\\%s", strCurPath, "Quest\\Daily\\*.mqfex");	

		HANDLE hDailyMedalSetFolder;
		WIN32_FIND_DATA fname;

		hDailyMedalSetFolder = ::FindFirstFile( strDailyMedalSetFolderPath, &fname );
		if ( hDailyMedalSetFolder != INVALID_HANDLE_VALUE ) 
		{ 
			// 첫번째 파일 명 가져오긔.
			m_MQFAllDailyMedalSet.m_ui16ActiveCount = 0;
			i3String::Format(aDailyMedalSetFileName[0].m_strFileName, MAX_PATH,  "%s%s",strDailyMedalSetFolder,fname.cFileName);	

			// 나머지 파일 명 가져오긔.
			for(  m_MQFAllDailyMedalSet.m_ui16ActiveCount  =1; ::FindNextFile(hDailyMedalSetFolder, &fname) > 0 ; m_MQFAllDailyMedalSet.m_ui16ActiveCount++  )
			{
				if( m_MQFAllDailyMedalSet.m_ui16ActiveCount >= MAX_LOAD_DAILY_MEDAL_SET_COUNT ) break;

				i3String::Format(aDailyMedalSetFileName[m_MQFAllDailyMedalSet.m_ui16ActiveCount].m_strFileName, MAX_PATH,  "%s%s",strDailyMedalSetFolder,fname.cFileName);	
			}

			// MQF 파일 로드.
			for ( UINT8 i = 0; i < m_MQFAllDailyMedalSet.m_ui16ActiveCount  ; i++ )
			{
				i32Rv = LoadMedalSet( aDailyMedalSetFileName[i].m_strFileName, &m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i]  );
				if( EV_FAILED(i32Rv) )
				{
					g_pLog->WriteLog( L"[CContextMain::LoadMedalFile] Fail Load Daily Medal Set / Error Code : %x", i32Rv );
					return FALSE;
				}
			}


			// 메달 셋트 고유인덱스 오름차순으로 정렬.
			UINT16 r,t;
			MQF_MEDAL_SET_INFO temp; 
			for( r=0; r< MAX_LOAD_DAILY_MEDAL_SET_COUNT -1; r++)
			{
				for( t=0; t< MAX_LOAD_DAILY_MEDAL_SET_COUNT-r-1; t++)
				{

					if(m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t].m_ui16Idx > m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t+1].m_ui16Idx)
					{
						temp = m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t+1];
						m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t+1] = m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t];
						m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[t] = temp;
					}
				}
			}
		}
		else
		{
			g_pLog->WriteLog( L"[CContextMain::LoadMedalFile] Find DailyMedalSet Fail" );
		}
	}




	// 이벤트 메달 셋트 로드.
	{
		// Medal\\Event 폴더에 있는 .mqfex 확장자 파일들을 검색해서 리스트를 만든다.
		i3String::Format(strEventMedalSetFolder, MAX_PATH,  "%s\\%s", strCurPath, "Quest\\Event\\");	
		i3String::Format(strEventMedalSetFolderPath, MAX_PATH,  "%s\\%s", strCurPath, "Quest\\Event\\*.mqfex");	

		HANDLE hEventMedalSetFolder;
		WIN32_FIND_DATA fname;

		hEventMedalSetFolder = ::FindFirstFile( strEventMedalSetFolderPath, &fname );
		if ( hEventMedalSetFolder != INVALID_HANDLE_VALUE ) 
		{ // (HANDLE)-1


			// 첫번째 파일 명 가져오긔.
			m_MQFAllEventMedalSet.m_ui16ActiveCount = 0;
			i3String::Format(aEventMedalSetFileName[0].m_strFileName, MAX_PATH,  "%s%s",strEventMedalSetFolder,fname.cFileName);	

			// 나머지 파일 명 가져오긔.
			for(  m_MQFAllEventMedalSet.m_ui16ActiveCount  =1; ::FindNextFile(hEventMedalSetFolder, &fname) > 0 ; m_MQFAllEventMedalSet.m_ui16ActiveCount++  )
			{
				if( m_MQFAllEventMedalSet.m_ui16ActiveCount >= MAX_LOAD_EVENT_MEDAL_SET_COUNT ) break;

				i3String::Format(aEventMedalSetFileName[m_MQFAllEventMedalSet.m_ui16ActiveCount].m_strFileName, MAX_PATH,  "%s%s",strEventMedalSetFolder,fname.cFileName);	
			}

			// MQF 파일 로드.
			for ( UINT8 i = 0; i < m_MQFAllEventMedalSet.m_ui16ActiveCount  ; i++ )
			{
				i32Rv = LoadMedalSet( aEventMedalSetFileName[i].m_strFileName, &m_MQFAllEventMedalSet.m_aEventMedalSetInfo[i]  );
				if( EV_FAILED(i32Rv) )
				{
					g_pLog->WriteLog( L"[CContextMain::LoadMedalFile] Fail Load Event Medal Set / Error Code : %x", i32Rv );
					return FALSE;
				}
			}


			// 메달 셋트 고유인덱스 오름차순으로 정렬.
			UINT16 r,t;
			MQF_MEDAL_SET_INFO temp; 
			for( r=0; r< MAX_LOAD_EVENT_MEDAL_SET_COUNT -1; r++)
			{
				for( t=0; t< MAX_LOAD_EVENT_MEDAL_SET_COUNT-r-1; t++)
				{

					if(m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t].m_ui16Idx > m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t+1].m_ui16Idx)
					{
						temp = m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t+1];
						m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t+1] = m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t];
						m_MQFAllEventMedalSet.m_aEventMedalSetInfo[t] = temp;
					}
				}
			}
		}
		else
		{
			g_pLog->WriteLog( L"[CContextMain::LoadMedalFile] Find EventMedalSet Fail " );
		}
	}


	if( g_pContextMain->IsEnableEventMedalSet() )
	{
		// 이벤트 메달 셋트 MQF 파일 정보 검색 실패면 서버 종료.
		MQF_MEDAL_SET_INFO* pMedalSet = g_pContextMain->GetMedalSet( g_pContextMain->m_MedalSystemInfo.m_ui16EventMedalSetIdx ); 
		if( NULL == pMedalSet  )
		{
			// 로그남기고 서버 종료.	
			g_pLog->WriteLog( L"No Event MedalSet MQF File Info / MedalSetIdx : %d ", 
				g_pContextMain->m_MedalSystemInfo.m_ui16EventMedalSetIdx );
#ifdef I3_DEBUG
			g_pContextMain->m_MedalSystemInfo.m_ui16EventMedalSetIdx = 0;
#else
			return FALSE;
#endif
		}
	}

	return TRUE; 
}

INT32	CContextMain::LoadMedal(INT32 i32LocaleCode, const char * strFileName)
{
	// Read
	MQF_MEDAL_INFO  aMQFMedalInfo[MAX_MEDAL_COUNT];
 
	CMedalSystemFile	MedalSystem;
	MedalSystem.SetAutoMemoryDelete();
	if(  !MedalSystem.LoadMQF(strFileName) )
	{
		return EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_OPEN;
	}

	INT32 i32TotalCount = MedalSystem.GetMedalInfoCount();
	for( INT32 i = 0 ; i < i32TotalCount ; ++i)
	{	
		i3mem::Copy( &aMQFMedalInfo[i] , MedalSystem.GetMedalInfo(i), sizeof(MQF_MEDAL_INFO) );  
	}

	// 서버에서 필요한 데이터만 셋팅.
	MQF_ALL_MEDAL_INFO * pServerMQF = &m_MQFMedal[i32LocaleCode];

	for( INT32 MedalArrIdx = 0 ; MedalArrIdx < i32TotalCount ; MedalArrIdx++)
	{	
		for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
		{
			if( Type == aMQFMedalInfo[MedalArrIdx].m_eCheckPosition ) 
			{
				UINT16 ui16MedalCount = pServerMQF->m_aui16MQFMedalCount[Type];	// 해당 타입(체크포지션)에서의 메달 갯수

				i3mem::Copy(&pServerMQF->m_aMedalInfo[Type][ui16MedalCount], &aMQFMedalInfo[MedalArrIdx], sizeof(MQF_MEDAL_INFO));

				pServerMQF->m_aui16MQFMedalCount[Type]++;
				break;
			}
		}

	}

	

	// 메달 고유인덱스 오름차순으로 정렬.
	MQF_MEDAL_INFO temp; 
	for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
	{
		for( INT32 r=0; r< MAX_MEDAL_COUNT -1; r++)
		{
			for( INT32 t=0; t< MAX_MEDAL_COUNT-r-1; t++)
			{
				if (pServerMQF->m_aMedalInfo[Type][t].m_ui16Idx> pServerMQF->m_aMedalInfo[Type][t + 1].m_ui16Idx)
				{
					temp = pServerMQF->m_aMedalInfo[Type][t + 1];
					pServerMQF->m_aMedalInfo[Type][t + 1] = pServerMQF->m_aMedalInfo[Type][t];
					pServerMQF->m_aMedalInfo[Type][t] = temp;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}


// 메달 로드
INT32	CContextMain::LoadMedalSet(const char * strFileName, MQF_MEDAL_SET_INFO* pMedalSet)			// 메달 셋트 파일 로드
{
	if( NULL == pMedalSet ) return	EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_ARGUMENT;	// 매개변수 값 잘못됨

	MQF_MEDAL_INFO  aMQFMedalInfo[MAX_MEDAL_COUNT_PER_MEDAL_SET];
 
	CMedalSystemFile	MedalSystem;
	MedalSystem.SetAutoMemoryDelete();
	if(  !MedalSystem.LoadMQF(strFileName) )
	{
		return EVENT_ERROR_PBV15_MEDAL_FAIL_MEDAL_FILE_OPEN;
	}

	pMedalSet->m_ui16Idx = (UINT16)MedalSystem.GetMedalUniqueIdx();
	pMedalSet->m_ui8ActiveMedalCount = MIN( (UINT8)MedalSystem.GetMedalInfoCount(), MAX_MEDAL_COUNT_PER_MEDAL_SET );

	for( INT32 i = 0 ; i < pMedalSet->m_ui8ActiveMedalCount ; ++i)
	{	
		i3mem::Copy( &pMedalSet->m_aMedalInfo[i] , MedalSystem.GetMedalInfo(i), sizeof(MQF_MEDAL_INFO) );  
	}

	// 메달 셋트안에 메달 고유인덱스 오름차순으로 정렬.
	MQF_MEDAL_INFO temp; 	for( INT32 r=0; r< MAX_MEDAL_COUNT_PER_MEDAL_SET -1; r++)
	{
		for( INT32 t=0; t< MAX_MEDAL_COUNT_PER_MEDAL_SET-r-1; t++)
		{
			if( pMedalSet->m_aMedalInfo[t].m_ui16Idx > pMedalSet->m_aMedalInfo[t+1].m_ui16Idx)
			{
				temp = pMedalSet->m_aMedalInfo[t+1];
				pMedalSet->m_aMedalInfo[t+1] = pMedalSet->m_aMedalInfo[t];
				pMedalSet->m_aMedalInfo[t] = temp;
			}
		}
	}

	return ERROR_SUCCESS;
}

MQF_MEDAL_SET_INFO* CContextMain::GetMedalSet( UINT16 ui16MedalSetIdx) // 메달 셋트 고유 인덱스로 메달 셋트 데이터 검색.
{	
	// 전체 일일 메달 셋트에서 검색.
	for( INT32 i = 0 ; i < m_MQFAllDailyMedalSet.m_ui16ActiveCount ; i++ )
	{
		if( ui16MedalSetIdx == m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].m_ui16Idx ) 
		{
			return &m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i];
		}
	}

	// 전체 이벤트 메달 셋트에서 검색
	for( INT32 k = 0 ; k < m_MQFAllEventMedalSet.m_ui16ActiveCount ; k++ )
	{
		if( ui16MedalSetIdx == m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k].m_ui16Idx )
		{
			return &m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k];
		}
	}

	// 못찾았으면.
	return NULL;
}

MQF_MEDAL_SET_INFO* CContextMain::GetMedalSet_ByMedalIdx( UINT16 ui16MedalIdx) // 메달 고유 인덱스로 메달 셋트 데이터 검색.
{	
	// 전체 일일 메달 셋트에서 검색.
	for( INT32 i = 0 ; i < m_MQFAllDailyMedalSet.m_ui16ActiveCount ; i++ )
	{
		for( INT32 j = 0; j < m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].m_ui8ActiveMedalCount ; j++ )
		{
			if( ui16MedalIdx == m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].m_aMedalInfo[j].m_ui16Idx ) 
			{
				return &m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i];
			}
		}
	}

	// 전체 이벤트 메달 셋트에서 검색
	for( INT32 k = 0 ; k < m_MQFAllEventMedalSet.m_ui16ActiveCount ; k++ )
	{
		for( INT32 m = 0; m < m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k].m_ui8ActiveMedalCount ; m++ )
		{
			if( ui16MedalIdx == m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k].m_aMedalInfo[m].m_ui16Idx ) 
			{
				return &m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k];
			}
		}
	}

	// 못찾았으면.
	return NULL;
}


INT8	CContextMain::GetSetMedalArrIdx_ByMedalIdx( UINT16 ui16MedalIdx) // 메달 고유 인덱스로 메달 셋트에서 몇번째 메달인지 검색.
{	
	// 전체 일일 메달 셋트에서 검색.
	for( INT32 i = 0 ; i < m_MQFAllDailyMedalSet.m_ui16ActiveCount ; i++ )
	{
		for( INT32 j = 0; j < m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].m_ui8ActiveMedalCount ; j++ )
		{
			if( ui16MedalIdx == m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[i].m_aMedalInfo[j].m_ui16Idx ) 
			{
				return (INT8)j+1;
			}
		}
	}

	// 전체 이벤트 메달 셋트에서 검색
	for( INT32 k = 0 ; k < m_MQFAllEventMedalSet.m_ui16ActiveCount ; k++ )
	{
		for( INT32 m = 0; m < m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k].m_ui8ActiveMedalCount ; m++ )
		{
			if( ui16MedalIdx == m_MQFAllEventMedalSet.m_aEventMedalSetInfo[k].m_aMedalInfo[m].m_ui16Idx ) 
			{
				return (INT8)m+1;
			}
		}
	}

	// 못찾았으면.
	return -1;
}


// 메달 고유 인덱스로 MQF 메달 데이터 가져오기.
MQF_MEDAL_INFO*	CContextMain::GetMedal_MQFData(UINT16 ui16MedalIdx, MEDAL_TYPE eMedalType, UINT8 ui8LocaleCode)
{
	// 메달 타입에 따라 검색한다.
	switch ( eMedalType) 
	{
	case MEDAL_TYPE_NORMAL:
		{
			MQF_ALL_MEDAL_INFO*	pServerMQF = &m_MQFMedal[ui8LocaleCode];

			for (INT32 Type = MEDAL_MC_CHECK_POSITION_START; Type < MEDAL_MC_CHECK_POSITION_MAX; Type++)
			{
				for (INT32 MedalCount = 0; MedalCount < pServerMQF->m_aui16MQFMedalCount[Type]; MedalCount++)
				{
					if (ui16MedalIdx == pServerMQF->m_aMedalInfo[Type][MedalCount].m_ui16Idx)
					{
						return  &pServerMQF->m_aMedalInfo[Type][MedalCount];
					}
				}
			}
		}
		break;


	case MEDAL_TYPE_SET:
		{
			//  전체 일일 메달 셋트 리스트에서 검색
			for( INT32 k = 0 ; k < m_MQFAllDailyMedalSet.m_ui16ActiveCount ; k++ )
			{
				for( INT32 m = 0 ; m < MAX_MEDAL_COUNT_PER_MEDAL_SET ; m++ )
				{
					if( ui16MedalIdx ==  m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[k].m_aMedalInfo[m].m_ui16Idx )
					{
						return  &m_MQFAllDailyMedalSet.m_aDailyMedalSetInfo[k].m_aMedalInfo[m];
					}
				}
			}


			//  전체 이벤트 메달 셋트 리스트에서 검색
			for( INT32 j = 0 ; j < m_MQFAllEventMedalSet.m_ui16ActiveCount ; j++ )
			{
				for( INT32 n = 0 ; n < MAX_MEDAL_COUNT_PER_MEDAL_SET ; n++ )
				{
					if( ui16MedalIdx ==  m_MQFAllEventMedalSet.m_aEventMedalSetInfo[j].m_aMedalInfo[n].m_ui16Idx )
					{
						return  &m_MQFAllEventMedalSet.m_aEventMedalSetInfo[j].m_aMedalInfo[n];
					}
				}
			}
		}
		break;
	}

	// 못찾으면 리턴
	return NULL;
}

MEDAL_MC_CHECK_POSITION		CContextMain::GetMMCPositionByActionID( MEDAL_MC_ACTION_ID eActionID ) // 액션 ID로 미션 체크 포지션 검색.
{
	if( eActionID < MEDAL_MC_ACTION_ID_C4_PLANT ) return MEDAL_MC_CHECK_POSITION_NONE;
	if( eActionID <= MEDAL_MC_ACTION_ID_BATTLE_MAX ) return MEDAL_MC_CHECK_POSITION_BATTLE;
	if( eActionID <= MEDAL_MC_ACTION_ID_SHOP_MAX ) return MEDAL_MC_CHECK_POSITION_SHOP;
	if( eActionID <= MEDAL_MC_ACTION_ID_RS_MAX ) return MEDAL_MC_CHECK_POSITION_RS;
	if( eActionID <= MEDAL_MC_ACTION_ID_BATTLE_END_MAX ) return MEDAL_MC_CHECK_POSITION_BATTLE_END;
	return MEDAL_MC_CHECK_POSITION_NONE;
}

// 서버에서 이벤트 메달 셋트가 진행중인지
BOOL	CContextMain::IsEnableEventMedalSet(UINT16 ui16SetIdx)
{
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )				return FALSE;	// 메달 시스템 활성화 여부 체크
	if( i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() < m_MedalSystemInfo.m_ui32EventMedalSetStartTime )	return FALSE;	// 이벤트 메달 셋트 시작 날짜 체크
	if( i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() > m_MedalSystemInfo.m_ui32EventMedalSetEndTime )		return FALSE;	// 이벤트 메달 셋트 종료 날짜 체크
	if( 0 == m_MedalSystemInfo.m_ui16EventMedalSetIdx )						return FALSE;	// 이벤트 메달 셋트 인덱스 체크
	if( 0 == ui16SetIdx )													return TRUE;	// 비교할 대상이 없으면 성공.
	if( m_MedalSystemInfo.m_ui16EventMedalSetIdx != ui16SetIdx )			return FALSE;	// 

	return TRUE;
}

T_ItemID* CContextMain::GetDefaultCommonSetting( UINT8 m_ui8LocaleCode )
{
	return m_TBasicCommonEquip[ m_ui8LocaleCode ];
}

T_ItemID* CContextMain::GetDefaultCharWeaponSetting( UINT8 m_ui8LocaleCode )
{
	return m_TBasicWeapon[ m_ui8LocaleCode ];
}

ITEM_INFO* CContextMain::GetDefaultCharEquipSetting(void)
{
	return m_TBasicEquip;
}

void CContextMain::SetDefaultItem( UINT8 ui8DefaultItemCount, DEFAULT_ITEM* pDefaultItem )
{
	DEFAULT_ITEM_SET* pDefaultSet;
	for( INT32 i = 0 ; i < ui8DefaultItemCount ; i++ )
	{
		if( DEFAULT_ITEM_TYPE_MAX > pDefaultItem->m_ui8Type )
		{
			pDefaultSet = &m_DefaultItemSet[ pDefaultItem->m_ui8Type ];
			// ui8DefaultItemCount 이 이미 DEFAULT_ITEM_COUNT 를 넘지 않기 때문에 pDefaultSet->m_ui8Count 에 대한 체크는 안합니다.

			pDefaultSet->m_TItemID[ pDefaultSet->m_ui8Count ] = pDefaultItem->m_TItemID;
			pDefaultSet->m_ui8Count++;
		}
		pDefaultItem++;
	}
}

DEFAULT_ITEM_SET* CContextMain::GetDefaultItemSet( UINT8 ui8Type, UINT8 ui8Level )
{
	INT32 i32Idx = -1;
	switch( ui8Type )
	{
	case DEFAULT_ITEM_TYPE_NORMAL:
		{
			i32Idx = DEFAULT_ITEM_TYPE_NORMAL;
		}
		break;
	case DEFAULT_ITEM_TYPE_PCCAFE:
		{
			if( PC_NOT_CAFE != ui8Level )
			{
				i32Idx = DEFAULT_ITEM_TYPE_PCCAFE + (ui8Level - (PC_NOT_CAFE+1));
			}
		}
		break;
	}

	if( 0 > i32Idx )						return NULL;
	if( DEFAULT_ITEM_TYPE_MAX <= i32Idx )	return NULL;

	return &m_DefaultItemSet[ i32Idx ];
} 

void CContextMain::SetUserCount( UINT16* pui16ChannelCount, UINT16 ui16ExtraCount )
{
	i3mem::Copy( m_aui16InitChannelCount, pui16ChannelCount, sizeof(UINT16) * m_ui32GameServerCount );

	m_ui32ChannelSessionCount	= pui16ChannelCount[m_i32ServerIdx];
	m_ui32ExtraSessionCount		= ui16ExtraCount;
	m_i32SessionCount			= (GAME_CHANNEL_COUNT * m_ui32ChannelSessionCount) + m_ui32ExtraSessionCount;
}

void CContextMain::GetCharaPriceByItemID( T_ItemID TItemID, UINT32* ui32Point, UINT32 *ui32Cash )
{
	*ui32Point	= 0;
	*ui32Cash	= 0;

	SHOP_MATCHING_DATA*	pMatching = m_pShop->GetMatchingAllData();
	UINT32 ui32MatchingCount = m_pShop->GetMatchingCount();

	UINT32 ui32GoodsID = 0;
	for( UINT32 i = 0; i < ui32MatchingCount; ++i )
	{
		if( pMatching->_ui32ItemID != TItemID )
		{
			++pMatching;
			continue;
		}

		ui32GoodsID = pMatching->_ui32GoodsID;
		break;
	}

	if( 0 == ui32GoodsID ) return;

	SHOP_GOODS_EXPANSION*	pGoods = m_pShop->GetGoodsAllData();
	UINT32 ui32GoodsCount = m_pShop->GetGoodsCount();
	for( UINT32 k = 0; k < ui32GoodsCount; ++k )
	{
		if( pGoods->_ui32GoodsID != ui32GoodsID )
		{
			++pGoods;
			continue;
		}

		*ui32Point	= pGoods->GetPoint();
		*ui32Cash	= pGoods->GetCash();
		break;
	}
	return;
}

void CContextMain::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
}

void CContextMain::GetContentsStateFlag(ContentsEnableFlag* pContentsStateFlag)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
	{
		if( m_ContentsManager.IsContensEnabled(static_cast<CONTENTS>(i)) )
			pContentsStateFlag->SetContentsEnabled( static_cast<CONTENTS>(i) );
	}
}

void CContextMain::InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
		return;

	m_ClanMatchSeason[eSeasonType].SetSeason(pClanMatchSeason);
}

//TTCHAR* CContextMain::GetClanMatchSeasonName(SEASON_TYPE eSeasonType )
//{
//	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
//	return NULL;
//
//	return m_ClanMatchSeason[eSeasonType].GetSeasonName();
//}

void CContextMain::MakePacketClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )	return;

	m_ClanMatchSeason[eSeasonType].MakePacket(pMatchSeason);
}

UINT32 CContextMain::GetClanMatchSeasonStartDate(SEASON_TYPE eSeasonType )
{
	if( eSeasonType <= SEASON_TYPE_NONE || SEASON_TYPE_COUNT <= eSeasonType )
	{
		return 0;
	}

	return m_ClanMatchSeason[eSeasonType].GetSeasonStartDate();
}
BOOL	CContextMain::CreateDailyGift()
{	
	for( INT32 i = 0 ; i < DAILY_GIFT_TYPE_MAX ; i++ )
	{
		m_i32RateMax[i]			= 0;
		m_i32DailyGiftCount[i]	= 0;
		for( INT32 j = 0 ; j < MAX_DAILY_GIFT_GOODS_LIST ; j++ )
		{
			if( 0 < m_DailyGift.m_DailyGiftGoodsList[i][j].m_i32Rate )
			{
				m_i32RateMax[i] += m_DailyGift.m_DailyGiftGoodsList[i][j].m_i32Rate;
				m_i32DailyGiftCount[i]++;
			}
		}
	}

	return TRUE;
} 

T_GoodsID	CContextMain::GetDailyGiftReward( UINT8 ui8DailyGiftType  )
{
	DAILY_GIFT_GOODS_LIST* pDailyGift = m_DailyGift.m_DailyGiftGoodsList[ ui8DailyGiftType ];

	INT32 i32Rand = m_i32RateMax[ui8DailyGiftType];
	if (0 == i32Rand) return 0;

	INT32 i32RandIdx = rand() % i32Rand;
	 
	INT32 i32Rate = 0;
	for( INT32 i = 0 ; i < m_i32DailyGiftCount[ ui8DailyGiftType ] ; i++ )
	{
		i32Rate += pDailyGift[ i ].m_i32Rate;
		if( i32RandIdx < i32Rate )
		{
			return pDailyGift[ i ].m_ui32GoodsID;
		}
	}

	return 0;
}


UINT8 CContextMain::RegistPBVersion(UINT8 ui8PBVer)
{
	switch (g_pContextMain->m_eServerVersion)
	{
	case PB_VER_V10:
		return (UINT8)PB_VER_V10;
	case PB_VER_V15:
		return (UINT8)PB_VER_V15;
	default:
		switch (ui8PBVer)
		{
		case PB_VER_V10:
		case PB_VER_V15:
			return ui8PBVer;
		default:
			return (UINT8)PB_VER_V15;
		}
	}
}

UINT8 CContextMain::GetMaxPerson( STAGE_ID ui32StageID )
{
	for ( UINT16 i = 0; i < m_ui16StageMatchingCount; ++i )
	{
		if (ui32StageID == m_aStageMatching[i].m_ui32StageID)
			return static_cast<INT32>(m_aStageMatching[i].m_ui8MaxPerson);
	}

	return 0;
}

STAGE_EVENT CContextMain::GetStageEvent(STAGE_ID ui32StageID)
{
	for (UINT16 i = 0; i < m_ui16StageMatchingCount; ++i)
	{
		if (ui32StageID == m_aStageMatching[i].m_ui32StageID)
			return static_cast<STAGE_EVENT>(m_aStageMatching[i].m_ui8Event);
	}

	return STAGE_EVENT_SEASON;
}

INT32 CContextMain::GetRuleIdx(STAGE_ID ui32StageID)
{
	for ( UINT16 ui16Idx = 0; ui16Idx < m_ui16StageMatchingCount; ui16Idx++ )
	{
		if ( ui32StageID == m_aStageMatching[ui16Idx].m_ui32StageID )
			return m_aStageMatching[ui16Idx].m_i32RuleIdx;

	}
	return -1;
}

UINT8 CContextMain::GetDefaultStage(STAGE_ID ui32StageID)
{
	UINT32 ui32Rule = StageID::GetRule( ui32StageID );

	for ( UINT8 ui8Idx = 0; ui8Idx < m_ui8RuleTotalCount; ui8Idx++ )
	{
		if ( ui32Rule == m_aRuleData[ui8Idx].m_ui32Rule )
			return m_aRuleData[ui8Idx].m_ui8DefaultStage;

	}

	return 1;

}

void	CContextMain::AddUserPacketProcess(INT32 i32ThreadIdx, INT32 i32ProtocolValue, REAL64 r64PacketProcessTime)
{
	if (i32ThreadIdx < 0) return;
	if (i32ThreadIdx >= MAX_USER_THREAD_COUNT) return;
	if (i32ProtocolValue < 0) return;
	if (i32ProtocolValue >= MAX_PROTOCOL_BASE_VALUE) return;

	if (0 == m_i32UserPacketIdx)
	{
		++m_ppui64UserPacketOldCount[i32ProtocolValue][i32ThreadIdx];
		m_ppr64UserPacketOldTime[i32ProtocolValue][i32ThreadIdx] += (REAL64)r64PacketProcessTime;
	}
	else
	{
		++m_ppui64UserPacketCount[i32ProtocolValue][i32ThreadIdx];
		m_ppr64UserPacketTime[i32ProtocolValue][i32ThreadIdx] += (REAL64)r64PacketProcessTime;
	}
}

void CContextMain::SetGeneralItem()
{
	// Locale Code에 맞춰 세팅합니다.
	switch (m_ui8LocalCode)
	{
	case LOCALE_CODE_NONE:
	case LOCALE_CODE_BRAZIL:
	case LOCALE_CODE_RUSSIA:
	case LOCALE_CODE_THAILAND:
	case LOCALE_CODE_INDONESIA:
	case LOCALE_CODE_LATIN_AMERICA:
		// 장성 베레모!
		m_ui8GeneralItemCount = 1; 
		m_aaui32GeneralItem[0] = MAKE_ITEM_ID(ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SPECIAL_GENERAL_BERET);
		break;
	}
}


 