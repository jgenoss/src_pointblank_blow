#include "pch.h"
#include "ContextMain.h"


CContextMain * g_pContextMain = NULL;

I3_CLASS_INSTANCE( CContextMain, i3ElementBase);

CContextMain::CContextMain(void)
{
	// 맵 정보 초기화
	//InitStageInfo();

	//초기 장착 무기 --- 인벤토리에 없으면 바로 크래쉬 합니다. 
#ifdef DEF_WEAPON_NOT_CHECK_USAGE_PRIMARY
	// 주무기 장착이 되어있지 않아도 억지로 넣어서 설정하지 않는다.
	m_ui32BasicEquipWeaponPrim	= 0;
#else
	m_ui32BasicEquipWeaponPrim	= 100003004;		// K2				//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT,	WEAPON_USAGE_PRIMARY,	WEAPON_CLASS_ASSAULT,	WEAPON_NUM_ASSAULT_K2);
#endif	

	// Middle East : Set first Equipped weapon
#if defined(LOCALE_MIDDLE_EAST)
	m_ui32BasicEquipWeaponPrim = 100003059;
#elif defined(LOCALE_TAIWAN)
	m_ui32BasicEquipWeaponPrim = 100003060;	//M4A1
#else
#endif

	m_FieldShop.m_ui32Version	= 0;

	m_ui32BasicEquipWeaponSub	= 601002003;		// K5				//MAKE_ITEM_FLAG(ITEM_CLASS_HANDGUN,	WEAPON_USAGE_SECONDARY,	WEAPON_CLASS_HANDGUN,	WEAPON_NUM_HANDGUN_K5);
	m_ui32BasicEquipWeaponMelee	= 702001001;		// M7				//MAKE_ITEM_FLAG(ITEM_CLASS_KNIFE,	WEAPON_USAGE_MELEE,		WEAPON_CLASS_KNIFE,		WEAPON_NUM_KNIFE_M7);
	m_ui32BasicEquipWeaponThrow	= 803007001;		// K400				//MAKE_ITEM_FLAG(ITEM_CLASS_GRENADE,	WEAPON_USAGE_THROWING,	WEAPON_CLASS_THROWING,	WEAPON_NUM_THROWING_K400);
	m_ui32BasicEquipWeaponItem	= 904007002;		// SMOKE			//MAKE_ITEM_FLAG(ITEM_CLASS_THROWING,	WEAPON_USAGE_ITEM,		WEAPON_CLASS_THROWING,	WEAPON_NUM_THROWING_SMOKE);

	//초기 장착 캐릭터 ---- 인벤토리에 없으면 바로 크래쉬 합니다.
	m_ui32BasicEquipCharaRed	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,		CHARACTER_RES_ID_MALE_RED);
	m_ui32BasicEquipCharaBlue	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_MALE_BLUE);			//Male_Basic_Class
	m_ui32BasicEquipCharaRed_F	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,		CHARACTER_RES_ID_FEMALE_RED);
	m_ui32BasicEquipCharaBlue_F	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_FEMALE_BLUE);			//Male_Basic_Class
	m_ui32BasicEquipCharaHelmel	= MAKE_ITEM_FLAG(ITEM_CLASS_HEAD,		CHARACTER_CLASS_HEAD,		CHARACTER_TEAM_RB,		HEAD_RES_ID_DEFAULT);					//Helmet
	m_ui32BasicEquipCharaDino	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_CC_Raptor);		// CC랩터

	m_r32LocalTime				= 0.0f; 
	m_ui32ServerTime			= 10000; //계산을 위해서 이 값부터 시작... UINT32 때문
	m_i32LastUpdateTime			= 0;
	m_ui16EventBonusCount		= 0;

	m_i32SessionCount			= 0;
	m_i32UpdateMainTime			= 500;
	m_ui32UpdateChannel			= 0;
	m_ui32ExtraSessionCount		= 100;
	m_pui8ChannelType			= NULL;
	m_ui32QuestEventDate		= 0;
	m_i32ClanMax				= 0;
	m_bIsNewRepair				= false;
	m_bGiftEnable				= false;

	i3mem::FillZero( m_DefaultItemSet, sizeof(DEFAULT_ITEM_SET)*DEFAULT_ITEM_TYPE_MAX );

	i3String::Copy(m_strLogFilePath, "C:\\LwLog", 512); 

	i3mem::FillZero(m_strNormalNotice,		NOTICE_STRING_MAX_COUNT ); 
	i3mem::FillZero(m_strEmergencyNotice,	NOTICE_STRING_MAX_COUNT );

	//Time
	m_TimeCreateRoom.wMonth		= 0; 
	m_TimeCreateRoom.wDay		= 0; 	
	m_TimeCreateRoom.wHour		= 0; 
	m_TimeCreateRoom.wMinute	= 0;
	m_TimeCreateRoom.wSecond	= 0;
	m_TimeCreateRoom.wYear		= 0;
	m_TimeStartBattle.wMonth	= 0;
	m_TimeStartBattle.wDay		= 0;
	m_TimeStartBattle.wHour		= 0;
	m_TimeStartBattle.wMinute	= 0;
	m_TimeStartBattle.wSecond	= 0;
	m_TimeStartBattle.wYear		= 0;


	//Set Shop Value	
	m_pShop					= NULL;	

	m_ui8AddressCount		= 0; 
	m_pui32AddressIp		= NULL; 
	m_pui16AddressPort		= NULL;	
	m_pui8AddressTimeOut	= NULL;

	m_bAbusingCheck			= FALSE; 
	m_i32ncGameId			= 0;

	m_ui8ClanCreateRank		= 0;
	m_i32ClanCreatePoint	= 0;

	m_i32ServerIdx			= -1;
	m_i32Gateway			= SERVER_TYPE_GAME;

	m_strEnterPass[0]		= '\0';

	//Clan 초기화 
	m_i32ClanVersion		= 0;
	i3mem::FillZero( &m_aClanContext,	sizeof( m_aClanContext ) );
	m_apClanList[0] = NULL;
	m_apClanList[1] = NULL;

	i3mem::FillZero( m_aStageDefaultActive, sizeof( m_aStageDefaultActive ) );

	m_ui16EventBonusCount		= 0;
	m_i32LastUpdateTime			= 0;

	m_ui8StageTotalCount		= 0;
	m_ui8StageBitCount			= 0;

	m_ui8ActiveQuestCount		= 0;
	m_pui32ActiveQuest			= NULL;

	//NET_MD5_KEY_SIZE
	m_astrMD5Key[0][NET_MD5_KEY_SIZE] = 0; 
	m_astrMD5Key[1][NET_MD5_KEY_SIZE] = 0; 

	m_i32ActiveClanIdx	= 0;

	// Event
	i3mem::FillZero( m_stEventBouns, sizeof(TYPE_EVENT_BONUS)*EVENTBONUS_COUNT );

	// SWS
	for( INT32 i = 0 ; i <WARNING_LEVEL_COUNT; i++)	
	{
		m_aWarningInfo[ i ]._ui32WriteTime = 0;
		m_aWarningInfo[ i ]._ui32ReadTime = 0;
	}

	// 임무카드	
	i3mem::FillZero( m_aaQuest,					sizeof( m_aaQuest ) );
	i3mem::FillZero( m_aQuest_ItemInfo,			sizeof( m_aQuest_ItemInfo ) );
	i3mem::FillZero( m_aaui16QuestComplete,		sizeof( m_aaui16QuestComplete ) );

	i3mem::FillZero( m_aaQuestEvent,			sizeof( m_aaQuestEvent ) );
	i3mem::FillZero( m_aQuestEvent_ItemInfo,	sizeof( m_aQuestEvent_ItemInfo ) );
	i3mem::FillZero( m_aaui16QuestEventComplete,sizeof( m_aaui16QuestEventComplete )  );

	i3mem::FillZero( m_aRankUpItem,				sizeof( m_aRankUpItem ) );
	i3mem::FillZero( m_aTSEvent,				sizeof( m_aTSEvent ) );
		
	m_ui8GoodsOrder = 0;
	// 이벤트 임무카드명
	for( UINT8 i = 0; i < LOCALE_CODE_MAX; ++i )
	{
		i3mem::FillZero(m_astrEventQuestCard[i],	MAX_STRING_COUNT );
	}

	i3mem::FillZero( m_strReportFtpAddress,		MAX_STRING_COUNT );
	m_ui32ReportFtpPort = 0;

	i3mem::FillZero( m_strBillingURL,			MAX_STRING_COUNT );
	m_ui16BillingURLLen = 0;

	m_i32TSInitHour		= 0;
	m_i32TSInitMin		= 0;

}

CContextMain::~CContextMain(void)
{
	I3_SAFE_RELEASE(m_pShop);

	for( INT32 i = 0 ; i < CLAN_LIST_COUNT ; i++ )
	{
		I3MEM_SAFE_FREE( m_apClanList[i] );
	}

	//m_pAddressIp
	I3MEM_SAFE_FREE( m_pui32ActiveQuest );
	I3MEM_SAFE_FREE( m_pui32ProxyAddressIp );
	I3MEM_SAFE_FREE( m_pui32AddressIp );
	I3MEM_SAFE_FREE( m_pui16AddressPort	);
	I3MEM_SAFE_FREE( m_pui8AddressTimeOut );
	I3MEM_SAFE_FREE( m_pui8ChannelType );
}

BOOL CContextMain::InitShop(void)
{
	m_pShop = CShop::NewObject();
	I3ASSERT(m_pShop != NULL);

	if (m_pShop == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

UINT32 CContextMain::GetFieldShopVersion()
{
	return m_FieldShop.m_ui32Version;
}

BOOL CContextMain::CheckMD5Key(char * pstrKey)
{
	if( i3String::Compare(pstrKey, m_astrMD5Key[0]) == 0)return TRUE; 
	if( i3String::Compare(pstrKey, m_astrMD5Key[1]) == 0)return TRUE; 

	return FALSE; 
}

BOOL	CContextMain::LoadEventQuestFile()
{
	char strTemp[SERVER_STRING_COUNT];
	i3String::Copy( strTemp, g_strCCurrentPath, SERVER_STRING_COUNT);
	i3String::Concat(strTemp, "\\Quest\\QuestConfig.ini");
	if( strTemp == NULL)	
	{
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[LoadEventQuestFile] There is no QuestConfig.ini ");
		return FALSE;
	}

	i3IniParser Parser;

	if( !Parser.OpenFromFile( strTemp ))
	{		
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[LoadEventQuestFile] QuestConfig.ini Open Fail");
		return FALSE;
	}

	// 이벤트 임무카드 설정 - 서동권
	if( Parser.ReadSection("EventQuestCard"))
	{
		char strTemp[3];
		for( UINT8 i = 0; i < LOCALE_CODE_MAX; ++i )
		{
			i3String::Format( strTemp, 3, "%d", i );
			const char* pstrPass = Parser.GetStringValue( strTemp );
			if( pstrPass )
			{
				strcpy( m_astrEventQuestCard[i], pstrPass );
			}
		}
	}
	else 
	{
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[LoadEventQuestFile] Wrong QuestConfig.ini ");
		return FALSE;
	}

	Parser.Close();
	return TRUE;
}


BOOL CContextMain::LoadConfigFile( const char * pstrFileName )
{
	if( pstrFileName == NULL)	return FALSE;

	i3IniParser Parser;
	char	strTemp[256]; 

	if( !Parser.OpenFromFile( pstrFileName))
	{		
		return FALSE;
	}

	INT32 i32Temp = 0;  
	if( Parser.ReadSection("Common"))
	{
		Parser.GetValue( "Gateway",			m_i32Gateway,		&m_i32Gateway);
		Parser.GetValue( "Version",			m_i32Version,		&m_i32Version);
		Parser.GetValue( "UdpClientPort",	i32Temp,			&i32Temp);
		m_ui16ClientPort = (UINT16)i32Temp; 

		m_ui32ThreadCount = 64;	

		//LogFile 
		strcpy(m_strLogFilePath, Parser.GetStringValue("LogFilePath"));

		Parser.GetValue("NcGameId", m_i32ncGameId, &m_i32ncGameId); 
		Parser.GetValue( "GameGuard", m_i32GameGuard, &m_i32GameGuard);

		Parser.GetValue("AbusingCheck", 0, &i32Temp); 
		if(i32Temp == 0)	m_bAbusingCheck = FALSE; 
		else			m_bAbusingCheck = TRUE;

#ifdef I3_DEBUG
		Parser.GetValue("PassEquipCheck", 0, &i32Temp);
		if(i32Temp == 1)	m_bPassEquipCheck = TRUE;
		else			m_bPassEquipCheck = FALSE;
#endif

		Parser.GetValue("LoginCheck", 3, &m_ui32LoginCheckTime); 
		switch( m_i32GameGuard )
		{
		case GAMEGUARD_NPROTECT:
#ifndef USE_NPROTECT
			return FALSE;
#endif
			break;
		case GAMEGUARD_HACKSHIELD:
#ifndef USE_AHNHS
			return FALSE;
#endif
			break;
		case GAMEGUARD_XTRAP:
#ifndef USE_XTRAP
			return FALSE;
#endif
			break;
		case GAMEGUARD_GPK:
#ifndef USE_GPK
			return FALSE;
#endif
			break;
		}

		const char* pstrPass = Parser.GetStringValue("ServerPass");
		if( pstrPass )
		{
			strncpy( m_strEnterPass, Parser.GetStringValue("ServerPass"), NET_SERVER_PW );
		}
	}
	else return FALSE; 

	if( Parser.ReadSection("Address"))
	{
		char strIpAddress[128];
		Parser.GetValue( "Count", i32Temp,	&i32Temp);
		m_ui8AddressCount = (UINT8)i32Temp;

		m_pui32AddressIp		= (UINT32*)i3MemAlloc(sizeof(UINT32) * m_ui8AddressCount);
		m_pui16AddressPort		= (UINT16*)i3MemAlloc(sizeof(UINT16) * m_ui8AddressCount);
		m_pui8AddressTimeOut	= (UINT8*)i3MemAlloc(sizeof(UINT8) * m_ui8AddressCount);
		m_pui32ProxyAddressIp	= (UINT32*)i3MemAlloc(sizeof(UINT32) * m_ui8AddressCount);

		for(UINT8 i = 0; i < m_ui8AddressCount; i++)
		{
			i3String::Format(strTemp, 256, "Ip%d", i); 
			strcpy(strIpAddress, Parser.GetStringValue(strTemp) );
			m_pui32AddressIp[i] = ::inet_addr(strIpAddress); 
			m_pui32ProxyAddressIp[i] = m_pui32AddressIp[i];

			i3String::Format(strTemp, 256, "Port%d", i);
			Parser.GetValue( strTemp, i32Temp,	&i32Temp);
			m_pui16AddressPort[i] = (UINT16)i32Temp; 

			i3String::Format(strTemp, 256, "ProxyIp%d", i); 
			const char * pTemp = Parser.GetStringValue(strTemp);
			if( pTemp )
			{
				strcpy(strIpAddress, pTemp );
				m_pui32ProxyAddressIp[i] = ::inet_addr(strIpAddress); 
			}

			i3String::Format(strTemp, 256, "TimeOut%d", i);
			Parser.GetValue( strTemp, i32Temp,	&i32Temp);
			m_pui8AddressTimeOut[i] = (UINT8)i32Temp; 
		}
	}else return FALSE; 

	if( Parser.ReadSection("Trans"))
	{
		strcpy(m_strTransServerIp, Parser.GetStringValue("TransServerIp"));
		Parser.GetValue( "TransServerPort", i32Temp, &i32Temp);
		m_ui16AuthServerPort = (UINT16)i32Temp; 
	}else return FALSE; 


	if( Parser.ReadSection("Clan"))
	{
		Parser.GetValue("ClanActive", i32Temp, &i32Temp);
		if (i32Temp == 0)	m_bClanActive = FALSE;
		else			m_bClanActive = TRUE;
		strcpy(m_strClanServerIp, Parser.GetStringValue("ClanServerIp"));
		Parser.GetValue( "ClanServerPort", i32Temp, &i32Temp);
		m_ui16ClanServerPort = (UINT16)i32Temp; 
	}else return FALSE; 

	if (Parser.ReadSection("Messenger"))
	{
		Parser.GetValue("MessengerActive", i32Temp, &i32Temp);
		if (i32Temp == 0)	m_bMessengerActive = FALSE;
		else			m_bMessengerActive = TRUE;
		strcpy(m_strMessengerServerIp, Parser.GetStringValue("MessengerServerIp"));
		Parser.GetValue("MessengerServerPort", i32Temp, &i32Temp);
		m_ui16MessengerServerPort = (UINT16)i32Temp;
	}else return FALSE;

	if( Parser.ReadSection("Control"))
	{
		strcpy(m_strControlServerIp, Parser.GetStringValue("ControlServerIp"));
		Parser.GetValue( "ControlServerPort", i32Temp, &i32Temp);
		m_ui16ControlServerPort = (UINT16)i32Temp; 
	}else return FALSE; 


	if( Parser.ReadSection("LogD"))
	{
		Parser.GetValue( "LogDActive", i32Temp, &i32Temp);
		if( i32Temp == 0)	m_bLogDActive = FALSE; 
		else			m_bLogDActive = TRUE; 
		strcpy(m_strLogDServerIp, Parser.GetStringValue("LogDServerIp"));
		Parser.GetValue( "LogDServerPort", i32Temp, &i32Temp);
		m_ui16LogDServerPort = (UINT16)i32Temp;
	}else return FALSE; 

	if( Parser.ReadSection("Channel"))
	{
		Parser.GetValue( "ChannelSessionCount",	m_ui32ChannelSessionCount,		&m_ui32ChannelSessionCount);
		Parser.GetValue( "ExtraUserCount",		m_ui32ExtraSessionCount,		&m_ui32ExtraSessionCount);
		m_i32SessionCount = (GAME_CHANNEL_COUNT * m_ui32ChannelSessionCount)	+	m_ui32ExtraSessionCount;
	}
	else return FALSE; 

	m_i32UpdateMainTime = 24;		//한번 업데이트 할때마다 한 채널을 업데이트 합니다. 만약 채널이 10개이고 업데이트 타임이 1000(1초)이면 10초가 되어야 모든 채널의 로테이트가 가능합니다.

	if( Parser.ReadSection("Notice"))
	{
		strcpy(m_strNormalNotice,		Parser.GetStringValue("NormalText"));
		strcpy(m_strEmergencyNotice,	Parser.GetStringValue("EmergencyText"));
	}
	else return FALSE; 

	// 핵체크 0:체크안함 1:체크함 2:체크하되 중력핵은 체크하지 않음. 2011.09.16 안국정
	// 3 으로 할경우 무기 핵 검사 - 서동권
	if( Parser.ReadSection("Hack"))
	{
		Parser.GetValue( "HackCheck", i32Temp, &i32Temp);
		m_i8HackCheck = (INT8)i32Temp;
	}
	else	m_i8HackCheck = 0;

	// Dino 공룡 오픈시기 조절을 위한 값(전국가 적용 필요없음, 전국가 패치후에는 삭제예정)
	if( Parser.ReadSection("CCMode"))
	{
		Parser.GetValue( "StingCCModeOpen", i32Temp, &i32Temp);
		m_i8CCModeOpen = (INT8)i32Temp;
	}
	else	
	{
		m_i8CCModeOpen = 0;
	}

	if( Parser.ReadSection("OPTION"))
	{
		Parser.GetValue( "MacAddressCheck", i32Temp, &i32Temp);
		m_bMacAddressCheck = (BOOL)i32Temp;
	}
	else	
	{
		m_bMacAddressCheck = TRUE;
	}
	Parser.Close();

#if defined( LOCALE_TURKEY) || defined( LOCALE_BRAZIL)
	// CCMode 오픈시까지만 임시사용
	if(0 == g_pContextMain->m_i8CCModeOpen)
	{
		m_ui32BasicEquipCharaDino	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_Raptor);	// 랩터
	}

#elif defined( LOCALE_TOTAL)
	// CCDino 오픈시까지만 임시사용
	if(2 == g_pContextMain->m_i8CCModeOpen)
	{	
		m_ui32BasicEquipCharaDino	= MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,	CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_Raptor);	// 랩터
	}

#endif

	return TRUE;
}

void CContextMain::OnUpdate()
{
	// 시간 갱신
	RenewTime();

	// 업데이트 한지 1분이 되지 않았으면 그냥 지나간다.
	if( m_i32Minute == m_i32LastUpdateTime ) return;

	m_i32LastUpdateTime = m_i32Minute;



	// 이벤트 Bouns
	TYPE_ROOM_BONUS		stRoomBouns[ EVENT_ROOM_BONUS_MAX ];
	TYPE_CHANNEL_BONUS	stChannel[ CHANNEL_TYPE_MAX ];
	
	i3mem::FillZero( &m_stServerBouns, sizeof(TYPE_EVENT_BONUS) * EVENTBONUS_COUNT );
	i3mem::FillZero( &stChannel, sizeof(TYPE_CHANNEL_BONUS)*CHANNEL_TYPE_MAX );
	i3mem::FillZero( &stRoomBouns, sizeof(TYPE_ROOM_BONUS)*EVENT_ROOM_BONUS_MAX );

	TYPE_ROOM_BONUS* pRoomBouns;
	m_ui16ServerBonusCount = 0;

	for( INT32 i = 0 ; i < m_ui16EventBonusCount ; i++ ) // 받아온 이벤트 보너스 개수만큼만 돕니다 - 서동권
	{
		TYPE_EVENT_BONUS* pBonus = &m_stEventBouns[i];

		if( pBonus == NULL )							continue;
		if( m_ui32LocalDate > pBonus->_ui32EndDate )	continue;
		if( m_ui32LocalDate < pBonus->_ui32StartDate )	continue;
	
		pRoomBouns = NULL;

		switch( pBonus->_ui16Type )
		{
		case EVENT_BONUS_NORMAL				:
			pRoomBouns = &stRoomBouns[EVENT_ROOM_BONUS_NORMAL];
			break;		
		default:
			// 서버 보너스 구조체 크기가 EVENTBONUS_COUNT 이므로
			if( EVENT_BONUS_SERVER + MAX_GAME_SERVER_COUNT < pBonus->_ui16Type )	continue;

			// 서버 보너스 정보를 받습니다. - 서동권
			if( EVENT_BONUS_SERVER <= pBonus->_ui16Type )
			{
				if( m_i32ServerIdx == pBonus->_ui16Type - EVENT_BONUS_SERVER )
				{	
					pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_SERVER ];
				}

				m_stServerBouns[ m_ui16ServerBonusCount ]._ui16Type		 = pBonus->_ui16Type;
				m_stServerBouns[ m_ui16ServerBonusCount ]._ui32EndDate   = pBonus->_ui32EndDate;
				m_stServerBouns[ m_ui16ServerBonusCount ]._ui32Exp		 = pBonus->_ui32Exp;
				m_stServerBouns[ m_ui16ServerBonusCount ]._ui32Point	 = pBonus->_ui32Point;
				m_stServerBouns[ m_ui16ServerBonusCount ]._ui32StartDate = pBonus->_ui32StartDate;
				++m_ui16ServerBonusCount;
			}
			else if( EVENT_BONUS_NATION <= pBonus->_ui16Type )
			{
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_NATION + (pBonus->_ui16Type-EVENT_BONUS_NATION) ];
			}
			else if( EVENT_BONUS_STAGE <= pBonus->_ui16Type )
			{
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_STAGE + (pBonus->_ui16Type-EVENT_BONUS_STAGE) ];
			}
			else if ( EVENT_BONUS_STAGE_MASK <= pBonus->_ui16Type )
			{
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_STAGE_MASK + (pBonus->_ui16Type-EVENT_BONUS_STAGE_MASK) ];
			}
			else if ( EVENT_BONUS_MODE <= pBonus->_ui16Type )
			{
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_MODE + (pBonus->_ui16Type-EVENT_BONUS_MODE) ];
			}
			else if( EVENT_BONUS_CHANNEL <= pBonus->_ui16Type )
			{
				INT32 i32ChannelType = pBonus->_ui16Type-EVENT_BONUS_CHANNEL;

				if( m_aui8HaveChannel[ i32ChannelType ] == 1 )
				{
					stChannel[i32ChannelType]._ui16Type			= i32ChannelType;
					stChannel[i32ChannelType]._ui32Exp			= pBonus->_ui32Exp;
					stChannel[i32ChannelType]._ui32Point		= pBonus->_ui32Point;
				}
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_CHANNEL + (pBonus->_ui16Type-EVENT_BONUS_CHANNEL) ];
			}
			else if( EVENT_BONUS_PC_CAFE <= pBonus->_ui16Type )
			{
				pRoomBouns = &stRoomBouns[ EVENT_ROOM_BONUS_CAFE + (pBonus->_ui16Type-EVENT_BONUS_PC_CAFE) ];
			}
			else 
				continue;
		}

		if( pRoomBouns )
		{
			pRoomBouns->_bActive		= TRUE;
			pRoomBouns->_r32Exp			+= ((0.01f*pBonus->_ui32Exp) + 0.00001f);
			pRoomBouns->_r32Point		+= ((0.01f*pBonus->_ui32Point) + 0.00001f);
		}
	}

	// 더블 버퍼를 쓰지 않기 위해 한번에 업데이트 합니다.
	i3mem::Copy( m_stRoomBouns, stRoomBouns, sizeof( TYPE_ROOM_BONUS ) * EVENT_ROOM_BONUS_MAX );

	TYPE_CHANNEL_BONUS stMyChannelBonus[ CHANNEL_TYPE_MAX ];
	i3mem::FillZero( &stMyChannelBonus, sizeof(TYPE_CHANNEL_BONUS)*CHANNEL_TYPE_MAX );
	UINT8 ui8ChannelBonusCount = 0;
	for( UINT8 i = 0; i < CHANNEL_TYPE_MAX; ++i )
	{
		if( m_aui8HaveChannel[i] != 1 ) continue;

		stMyChannelBonus[ ui8ChannelBonusCount ]._ui16Type	= stChannel[ i ]._ui16Type;
		stMyChannelBonus[ ui8ChannelBonusCount ]._ui32Exp	= stChannel[ i ]._ui32Exp;
		stMyChannelBonus[ ui8ChannelBonusCount ]._ui32Point = stChannel[ i ]._ui32Point;
		++ui8ChannelBonusCount;
	}
	// 한번에 업데이트 합니다.
	i3mem::Copy( m_stChannelBonus, stMyChannelBonus, sizeof( TYPE_CHANNEL_BONUS ) * CHANNEL_TYPE_MAX );
	m_ui8ChannelBonusCount = ui8ChannelBonusCount;
}


bool	CContextMain::GetTSEventInfo(UINT32 ui32CurEventTime, TS_EVENT_INFO* pTSEventInfo)
{
	if( NULL == ui32CurEventTime ) return false;

	TS_EVENT_INFO	stEventInfo;
	INT32			i = 0;
	for(i = 0; i < TS_EVENT_COUNT; i++)
	{
		if(m_aTSEvent[i]._ui32StartDate == 0) continue;

		if(ui32CurEventTime >= m_aTSEvent[i]._ui32StartDate && ui32CurEventTime < m_aTSEvent[i]._ui32EndDate)
		{	
			tm InveneDate, TSDate;

			InveneDate.tm_year	 = 2000 + (ui32CurEventTime / 100000000);
			InveneDate.tm_mon	 = (ui32CurEventTime % 100000000) / 1000000;
			InveneDate.tm_mday	 = (ui32CurEventTime % 1000000) / 10000;
			InveneDate.tm_hour	 = (ui32CurEventTime % 10000) / 100;
			InveneDate.tm_min	 = ui32CurEventTime % 100;
			InveneDate.tm_sec	 = 0;
			InveneDate.tm_year	-= 1900;
			InveneDate.tm_mon	-= 1;
			InveneDate.tm_isdst	 = -1;

			ConvertDate(m_aTSEvent[i]._ui32StartDate, TSDate);	

			time_t CurTime = mktime(&InveneDate);
			time_t StartTime = mktime(&TSDate);

			m_ui8GoodsOrder = ( CurTime- StartTime) / 86400;
			*pTSEventInfo = m_aTSEvent[i];
			break;
		}
	}

	if(i == TS_EVENT_COUNT)
	{
		return false;
	}

	return true;
}

BOOL CContextMain::ConvertDate(UINT32 ui32SrcDate, tm &ConvertDate)
{
	ConvertDate.tm_year = ui32SrcDate / 100000000 + 2000;
	ConvertDate.tm_mon	= (ui32SrcDate % 100000000) / 1000000;
	ConvertDate.tm_mday = (ui32SrcDate % 1000000) / 10000;
	ConvertDate.tm_hour = (ui32SrcDate % 10000) / 100;
	ConvertDate.tm_min  = ui32SrcDate % 100;
	ConvertDate.tm_sec = 0;


	ConvertDate.tm_year -= 1900;
	ConvertDate.tm_mon	-= 1;
	
	ConvertDate.tm_isdst = -1;

	return TRUE;
}

BOOL CContextMain::LoadQuestFile(const char * pstrFileName)
{
	// 2011.04.14 - 퀘스트 로드시 튜토리얼을 각 국가별로 파일로 만들어서 로드하게 하였습니다. - 서동권
	INT32 i32Result = 0;

	char strFileName[SERVER_STRING_COUNT]; 

	// 일반 임무카드
	for( INT32 i = QUEST_CARDSET_TYPE_TUTORIAL ; i < QUEST_CARDSET_TYPE_COUNT ; i++ )
	{
		switch( i )
		{
		case QUEST_CARDSET_TYPE_TUTORIAL		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\TutorialCard.mqf");		break;
		case QUEST_CARDSET_TYPE_DINO_TUTORIAL	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\DIno_Tutorial.mqf");	break; 
		case QUEST_CARDSET_TYPE_HUMAN_TUTORIAL	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Human_Tutorial.mqf");	break; 		
		case QUEST_CARDSET_TYPE_GOLD			: continue;																		break; //삭제됨
		case QUEST_CARDSET_TYPE_ASSAULT			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\AssaultCard.mqf");		break; 			
		case QUEST_CARDSET_TYPE_BACKUP			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\BackUpCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_INFILTRATION	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\InfiltrationCard.mqf");	break; 
		case QUEST_CARDSET_TYPE_SPECIAL			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\SpecialCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_DEFCON			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\DefconCard.mqf");		break; 
		case QUEST_CARDSET_TYPE_COMMISSIONED_O	: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Commissioned_o.mqf");	break; 
		case QUEST_CARDSET_TYPE_COMPANY_O		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Company_o.mqf");		break; 
		case QUEST_CARDSET_TYPE_FIELD_O			: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Field_o.mqf");			break; 
		case QUEST_CARDSET_TYPE_EVENT			: continue;
		case QUEST_CARDSET_TYPE_DINO_2ND		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Dino_Basic.mqf");		break; 
		case QUEST_CARDSET_TYPE_HUMAN_2ND		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Human_Basic.mqf");		break; 
		case QUEST_CARDSET_TYPE_DINO_3RD		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Dino_Intensify.mqf");	break; 
		case QUEST_CARDSET_TYPE_HUMAN_3RD		: i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, "Quest\\Human_Intensify.mqf");	break; 
		default									:
			{
				g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[CContextMain::LoadQuestFile]잘못된 공통 임무카드 종류입니다.");
				I3TRACE("[CContextMain::LoadQuestFile]잘못된 종류입니다. \n");
				return FALSE;
			}
			break;
		}

		i32Result = QuestFileLoadEx( strFileName, m_aaQuest[i], &m_aQuest_ItemInfo[i] );

		if( 0 > i32Result )	
			return FALSE;
		else
		{
			QUEST_CTX * pQuest = m_aaQuest[i];
			UINT16 * pui16QuestComplete = m_aaui16QuestComplete[i];
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
		if( NULL == m_astrEventQuestCard[i] )
		{
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[CContextMain::LoadQuestFile] Error Event Card Setting");
			I3TRACE("[CContextMain::LoadQuestFile] Error Event Card Setting \n");
			return FALSE;
		}

		i3String::Format(strFileName, SERVER_STRING_COUNT, "%s\\%s", pstrFileName, m_astrEventQuestCard[i] );

		i32Result = QuestFileLoadEx( strFileName, m_aaQuestEvent[i], &m_aQuestEvent_ItemInfo[i] );

		if( 0 > i32Result )
		{
			char strErrorLog[512];
			i3String::Format( strErrorLog, 512, "[CContextMain::LoadQuestFile] Error Event Card Setting = LOCALE CODE %d - %s", i, strFileName );
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strErrorLog );
			I3TRACE("[CContextMain::LoadQuestFile] Error Event Card Setting \n");
			continue;
		}
		else
		{
			QUEST_CTX * pQuest = m_aaQuestEvent[i];
			UINT16 * pui16QuestComplete = m_aaui16QuestEventComplete[i];
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CContextMain::GetQuest( UINT8 ui8CardType,	INT8 i8QuestSlotIdx, QUESTING_INFO * pQuestInfo, UINT8* pui8QuestCtxActive, UINT8 ui8LocaleCode )
{
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
		pui16QuestComplete								= m_aaui16QuestEventComplete[ ui8LocaleCode ];
		break;
	default:
		if( QUEST_CARDSET_TYPE_COUNT <= ui8CardType ) return;
		pui16QuestComplete								= m_aaui16QuestComplete[ ui8CardType ];
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
/// 계급 진급 처리 ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL CContextMain::SetUserRank(UINT32 ui32Exp, UINT32 * pui32NowRank)	//exp는 -가 없다. 
{
	BOOL bRv = FALSE;

	// 영웅의 경험치를 얻으면 영웅계급으로 승급시킨다.
	if(g_ExpValue[RANK_51] <= ui32Exp)
	{
		if(*pui32NowRank >= RANK_51) return FALSE;

		*pui32NowRank = RANK_51;
		bRv = TRUE; 
	}

	// 상대평가 사용. 준장까지만 랭크업 시킵니다.
	if(*pui32NowRank < RANK_46)
	{
		while(g_ExpValue[*pui32NowRank + 1] <= ui32Exp)
		{	
			*pui32NowRank = *pui32NowRank + 1; 
			bRv = TRUE; 	

			if(*pui32NowRank > RANK_46)
			{				
				break; //무한 루프에 빠질것 같아서..
			}
		}
	}
	return bRv; 
}
//////////////////////////////////////////////////////////////////////////

UINT32	CContextMain::GetServerStateInfo(void)
{
	UINT32 ui32Rv = 0; 

	for( INT32 i = 0 ; i < WARNING_LEVEL_COUNT ; i++)
	{	// 워닝 발생 여부 체크
		if( g_pContextMain->m_aWarningInfo[ i ]._ui32WriteTime > g_pContextMain->m_aWarningInfo[ i ]._ui32ReadTime ) 
		{	// 워닝 발생
			ui32Rv = ui32Rv | (0x01 << i); 
			g_pContextMain->m_aWarningInfo[ i ]._ui32ReadTime = GetServerTime();
		}
	}

	return ui32Rv; 
}

TYPE_ROOM_BONUS* CContextMain::GetRoomBonus( EVENT_ROOM_BONUS_TYPE eEvent, INT32 i32Idx )
{
	switch( eEvent )
	{
	case EVENT_ROOM_BONUS_NORMAL:							break;
	case EVENT_ROOM_BONUS_CAFE:								break;
	case EVENT_ROOM_BONUS_CHANNEL:							break;
	case EVENT_ROOM_BONUS_MODE:								break;
	case EVENT_ROOM_BONUS_STAGE_MASK:						break;
	case EVENT_ROOM_BONUS_STAGE:							break;
	case EVENT_ROOM_BONUS_NATION:							break;
	default:												return NULL;
	}

	INT32 i32TotalIdx = eEvent + i32Idx;
	if( EVENT_ROOM_BONUS_MAX <= i32TotalIdx )				return NULL;

	if( FALSE == m_stRoomBouns[ i32TotalIdx ]._bActive )	return NULL;
	else													return &m_stRoomBouns[ i32TotalIdx ];
}

void CContextMain::SetMapSetting()
{
	for( INT32 i = 0 ; i < g_pContextMain->m_ui8StageTotalCount ; i++ )
	{
		if( 0x1 & (m_aui32StageBitActive[i/32] >> (i%32) ) )
		{
			m_abActiveStage[i]	= true;
		}
	}
}

bool CContextMain::IsActiveStage( STAGE_ID eStageID )
{
	if( m_ai32StageType[GET_STAGEORD( eStageID )] == GET_STAGETYPE( eStageID ) )
		return m_abActiveStage[GET_STAGEORD( eStageID )];

	return false;
}

bool CContextMain::IsPlayStage( STAGE_ID eStageID, V_STAGEOPTION T_Option )
{
	STAGE_ORDINAL_ID eStageOrdID = GET_STAGEORD( eStageID );
	if( T_Option & m_aTStageActiveOption[ eStageOrdID ] )	return true;
	
	return false;
}

char* CContextMain::GetStageName( STAGE_ID eStageID )
{
	return m_astrStageName[ GET_STAGEORD( eStageID ) ];
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