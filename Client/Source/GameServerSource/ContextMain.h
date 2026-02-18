#ifndef _CONTEXT_MAIN_H__
#define _CONTEXT_MAIN_H__
//------------------------------------------------------------------------------------------------------
//CContextMain에 있는 변수들은 글로벌 변수처럼 사용합니다. 

class CShop;

struct MAP_INFO
{
	bool			bMapOpen;
	V_STAGEOPTION	T_Option;
};

#define CLAN_LIST_COUNT	2

struct FieldShopInfo;
class CContextMain :	public i3ElementBase
{
	I3_CLASS_DEFINE( CContextMain );

protected :	
	//------------------------------------------------------------------------------------------------------
	//Time
	i3Timer			m_Timer; 
	UINT32			m_ui32ServerTime; 
	INT32			m_i32LastUpdateTime;
	SYSTEMTIME		m_SystemTime;
	SYSTEMTIME		m_StandardSystemTime;
	REAL32			m_r32LocalTime; 
	UINT32			m_ui32LocalDate;
	UINT32			m_ui32InvneTime; 
	UINT32			m_ui32InvneTimeSec;
	UINT32			m_ui32DateTimeSec;
	UINT64			m_ui64DateTime;

public:	
	//------------------------------------------------------------------------------------------------------
	//Basic Info
	UINT8					m_ui8UseServerSelect; 
	UINT8					m_aui8ChannelType[GAME_CHANNEL_COUNT]; 

	UINT32					m_ui32GameServerCount;
	GAME_SERVER_INFO		m_aGameServerInfo[100];

	char					m_astrMD5Key[2][NET_MD5_KEY_SIZE+1];

	SS_VERSION				m_ControlVersion;
	SS_VERSION				m_GameVersion;
	SC_VERSION				m_ClientVersion;

	//------------------------------------------------------------------------------------------------------
	//Basic Equipment
	UINT32					m_ui32BasicEquipWeaponPrim; 
	UINT32					m_ui32BasicEquipWeaponSub; 
	UINT32					m_ui32BasicEquipWeaponMelee; 
	UINT32					m_ui32BasicEquipWeaponThrow;
	UINT32					m_ui32BasicEquipWeaponItem; 

	UINT32					m_ui32BasicEquipCharaRed;		//남자
	UINT32					m_ui32BasicEquipCharaBlue;
	UINT32					m_ui32BasicEquipCharaRed_F;		//여자
	UINT32					m_ui32BasicEquipCharaBlue_F;

	UINT32					m_ui32BasicEquipCharaHelmel; 

	UINT32					m_ui32BasicEquipCharaDino;

	UINT8					m_ui8StageTotalCount;
	UINT8					m_ui8StageBitCount;
	UINT32					m_aui32StageBitActive[ STAGE_BIT_ACTIVE_MAX ];
	V_STAGEOPTION			m_aTStageActiveOption[ STAGE_TOTAL_MAX ];
	INT32					m_ai32StageType[ STAGE_TOTAL_MAX ];
	UINT8					m_aui8StageMark[ STAGE_TOTAL_MAX ];
	STAGE_ORDINAL_ID		m_aStageDefaultActive[ STAGE_TYPE_MAX ];
	bool					m_abActiveStage[ STAGE_TOTAL_MAX ];
	char					m_astrStageName[ STAGE_TOTAL_MAX ][ STAGE_CODE_NAME_COUNT ];

	UINT8					m_ui8ActiveQuestCount;
	UINT32*					m_pui32ActiveQuest;

	bool					m_bIsNewRepair;
	bool					m_bGiftEnable;	// 선물하기 시스템 적용 여부.
	bool					m_bNationalFlag;	// 국기표기 시스템 사용 여부
	DEFAULT_ITEM_SET		m_DefaultItemSet[ DEFAULT_ITEM_TYPE_MAX ];

	//UINT8					m_ui8DefaultItemPCCafeCount[ PC_MAX_COUNT ];
	//T_ItemID				m_TDetaultItemPCCafe[ PC_MAX_COUNT ][ DEFAULT_ITEM_COUNT ];

	
	//------------------------------------------------------------------------------------------------------
	//Basic Value 
	INT32		m_i32Gateway;					// Gateway Server 인지 확인
	INT32		m_i32Version;					//Vesion
	INT32		m_i32ServerIdx;					//서버 번호
	UINT16		m_ui16ClientPort;				//게임 UDP포트 번호
	
	UINT8		m_ui8AddressCount; 
	UINT32		* m_pui32AddressIp; 
	UINT16		* m_pui16AddressPort;
	UINT32		* m_pui32ProxyAddressIp;
	UINT8		* m_pui8AddressTimeOut;	
	BOOL		m_bAbusingCheck;
	BOOL		m_bPassEquipCheck;
	INT32		m_i32ncGameId;
	UINT32		m_ui32LoginCheckTime;

	INT32		m_i32UpdateMainTime; 
	UINT32		m_ui32UpdateChannel; 

	// 서동권 - 신고하기 기능 스크린샷 전송 FTP 주소
	char		m_strReportFtpAddress[MAX_STRING_COUNT];
	UINT32		m_ui32ReportFtpPort;

	// 임준현 - 이태리 빌링 연동을 위한 Cash 충전 URL
	char		m_strBillingURL[MAX_STRING_COUNT];
	UINT16		m_ui16BillingURLLen;

	//중계 서버 
	char		m_strTransServerIp[IP_NAME_COUNT];	
	UINT16		m_ui16AuthServerPort;

	//클랜 서버 	
	BOOL		m_bClanActive;
	char		m_strClanServerIp[IP_NAME_COUNT]; 
	UINT16		m_ui16ClanServerPort;

	//메신저 서버
	BOOL		m_bMessengerActive;
	char		m_strMessengerServerIp[IP_NAME_COUNT];
	UINT16		m_ui16MessengerServerPort;

	//게임가드 연동
	INT32		m_i32GameGuard;

	//관제툴 서버
	char		m_strControlServerIp[IP_NAME_COUNT];
	UINT16		m_ui16ControlServerPort;

	//엔씨 로그D
	BOOL		m_bLogDActive;
	char		m_strLogDServerIp[IP_NAME_COUNT];
	UINT16		m_ui16LogDServerPort;

	//로그파일 패스 
	char		m_strLogFilePath[ MAX_STRING_COUNT ];					//Log 파일 패스 

	//핵 검사를 서버에서 확인할건지 - DeathBattle
	INT8		m_i8HackCheck;

	// Dino 공룡 오픈시기 조절을 위한 값(전국가 적용 필요없음, 전국가 패치후에는 삭제예정)
	INT8		m_i8CCModeOpen;

	BOOL		m_bMacAddressCheck;

	//이벤트임무카드명
	char		m_astrEventQuestCard[ LOCALE_CODE_MAX ][ MAX_STRING_COUNT ];

	char		m_strEnterPass[ NET_SERVER_PW ];
	//------------------------------------------------------------------------------------------------------
	//각종 카운트 Count 
	UINT32	m_ui32ThreadCount;					//쓰레드 카운트 
	INT32	m_i32SessionCount;					//유저 카운트		
	UINT32	m_ui32ChannelSessionCount;			//세션 카운트 
	UINT32	m_ui32ExtraSessionCount;			//Extra를위한 공간 채널을 선택하지 않은 유저들을 위한 공간 	

	//------------------------------------------------------------------------------------------------------
	// Local Time YYYY-MM-DD-HH-MM-SS
	UINT32	m_ui32Date; 
	INT32	m_i32Year; 
	INT32	m_i32Month; 
	INT32	m_i32Day; 
	INT32	m_i32Hour; 
	INT32	m_i32Minute; 
	INT32	m_i32Second; 
	// Standard Time
	INT32	m_i32Std_Year; 
	INT32	m_i32Std_Month; 
	INT32	m_i32Std_Day;
	INT32	m_i32Std_Hour; 
	INT32	m_i32Std_Minute; 
	INT32	m_i32Std_Second; 

	//------------------------------------------------------------------------------------------------------
	//Room Time (LogD용)
	SYSTEMTIME		m_TimeCreateRoom;
	SYSTEMTIME		m_TimeStartBattle;

	//------------------------------------------------------------------------------------------------------
	//공지 
	char		m_strNormalNotice[NOTICE_STRING_MAX_COUNT]; 
	char		m_strEmergencyNotice[NOTICE_STRING_MAX_COUNT]; 

	//------------------------------------------------------------------------------------------------------
	//임무카드
	//일반 임무카드
	QUEST_CTX			m_aaQuest[QUEST_CARDSET_TYPE_COUNT][MAX_QUEST_PER_CARDSET_NEW];	
	QUEST_ITEM_INFO_NEW2	m_aQuest_ItemInfo[QUEST_CARDSET_TYPE_COUNT]; 
	UINT16				m_aaui16QuestComplete[QUEST_CARDSET_TYPE_COUNT][MAX_CARD_PER_CARDSET];
	// Event 임무카드
	QUEST_CTX			m_aaQuestEvent[LOCALE_CODE_MAX][MAX_QUEST_PER_CARDSET_NEW];	
	QUEST_ITEM_INFO_NEW2	m_aQuestEvent_ItemInfo[LOCALE_CODE_MAX]; 
	UINT16				m_aaui16QuestEventComplete[LOCALE_CODE_MAX][MAX_CARD_PER_CARDSET];

	//이벤트 쿼스트 
	UINT32				m_ui32QuestEventDate; 

	//------------------------------------------------------------------------------------------------------
	//Channel	
	UINT8			*	m_pui8ChannelType;						//채널 정보
	UINT8				m_aui8HaveChannel[ CHANNEL_TYPE_MAX ];

	//------------------------------------------------------------------------------------------------------
	// 상점 리스트 버퍼 - 더블버퍼링
	CShop*				m_pShop;

	// Field shop Information
	FieldShopInfo		m_FieldShop;

	//------------------------------------------------------------------------------------------------------
	// 클랜 버퍼 	
	INT32				m_i32ClanMax;
	INT32				m_i32ClanVersion;
	CLAN_CONTEXT		m_aClanContext[CLAN_LIST_COUNT]; 
	CLAN_BASIC_INFO	*	m_apClanList[CLAN_LIST_COUNT];
	INT32				m_i32ActiveClanIdx;

	UINT8				m_ui8ClanCreateRank;
	INT32				m_i32ClanCreatePoint;

	UINT32				m_ui32DateHour; 

	INT8				m_i8PrimaryWeapon;

	// Event
	//INT32				m_i32BonusCount;
	UINT16				m_ui16EventBonusCount;
	UINT16				m_ui16ServerBonusCount;
	TYPE_EVENT_BONUS	m_stEventBouns[ EVENTBONUS_COUNT ];

	TYPE_ROOM_BONUS		m_stRoomBouns[EVENT_ROOM_BONUS_MAX];
	TYPE_EVENT_BONUS	m_stServerBouns[ MAX_GAME_SERVER_COUNT ];

	UINT8				m_ui8ChannelBonusCount;
	TYPE_CHANNEL_BONUS	m_stChannelBonus[ CHANNEL_TYPE_MAX ];
		
	// SWS
	WARNING_LEVEL		m_aWarningInfo[ WARNING_LEVEL_COUNT ];
	
	RANKUP_ITEM			m_aRankUpItem[MAX_RANK_COUNT];			// RankUp 아이템 정보

	TS_EVENT_INFO		m_aTSEvent[ TS_EVENT_COUNT ];			// TS 이벤트 정보
	UINT8				m_ui8GoodsOrder;
	// ----------------------------------------------------------------------------------

	INT32				m_i32TSInitHour;
	INT32				m_i32TSInitMin;
public:
	CContextMain(void);
	~CContextMain(void);

	CLAN_CONTEXT*		GetClanContext()		{	return &m_aClanContext[m_i32ActiveClanIdx];	}
	CLAN_CONTEXT*		GetClanContextBack()	{	return &m_aClanContext[(m_i32ActiveClanIdx+1)%CLAN_LIST_COUNT];	}

	CLAN_BASIC_INFO*	GetClanList()			{	return m_apClanList[m_i32ActiveClanIdx];	}
	CLAN_BASIC_INFO*	GetClanListBack()		{	return m_apClanList[(m_i32ActiveClanIdx+1)%CLAN_LIST_COUNT];	}

	void	SetChangeClanActive()	{	m_i32ActiveClanIdx++;	m_i32ActiveClanIdx %= CLAN_LIST_COUNT;	}

	TYPE_ROOM_BONUS*		GetRoomBonus( EVENT_ROOM_BONUS_TYPE eEvent, INT32 i32Idx = 0 );

	void					SetMapSetting();
	bool					IsActiveStage( STAGE_ID eStageID );
	bool					IsPlayStage( STAGE_ID eStageID, V_STAGEOPTION T_Option );
	char*					GetStageName( STAGE_ID eStageID );

	BOOL	CheckMD5Key(char * pstrKey); 
	//상점 
	BOOL	InitShop(void);	

	BOOL	LoadConfigFile(const char * pstrFileName);					//로딩 Config파일 

	BOOL	LoadEventQuestFile();				// 이벤트 임무카드 파일 로드

	//임무카드 
	BOOL	LoadQuestFile(const char * pstrFileName);																	//로딩하기
	
	void	GetQuest( UINT8 ui8CardType, INT8 i8QuestSlotIdx, QUESTING_INFO * pQuestInfo, UINT8* pui8QuestCtxActive, UINT8 ui8LocaleCode );

	UINT32	GetServerTime(void)						{ return m_ui32ServerTime;	}

	UINT32					GetLocalDate(void)						{	return m_ui32LocalDate;		}
	UINT32	GetInvenTime(void)						{ return m_ui32InvneTime;	}
	UINT32	GetInvenSecTime(void)					{ return m_ui32InvneTimeSec;}
	UINT32	GetDateSecTime(void)					{ return m_ui32DateTimeSec; }
	UINT64	GetDateTime64()							{ return m_ui64DateTime;	}
	void	OnUpdate();

	BOOL	ConvertDate(UINT32 ui32SrcDate, tm &ConvertDate);
	UINT32	GetServerStateInfo(void); 
	REAL32	RenewTime(void)
	{ 
		//Cal Server Time 
		REAL32 r32DeltaTime = m_Timer.GetDeltaSec(); 
		m_r32LocalTime = m_r32LocalTime + r32DeltaTime;	
		if( m_r32LocalTime > 1.0f)
		{
			//정수부 더하기 
			UINT32 ui32Time	= (UINT32)m_r32LocalTime; 
			m_ui32ServerTime	= m_ui32ServerTime + ui32Time;

			//정수부 삭제 
			REAL32	r32Time	= (REAL32)ui32Time; 
			m_r32LocalTime		= m_r32LocalTime - r32Time;
		}
        
		//Cal Local Time
		::GetLocalTime( &m_SystemTime );	
		m_i32Month		=	m_SystemTime.wMonth; 
		m_i32Day		=	m_SystemTime.wDay; 	
		m_i32Hour		=	m_SystemTime.wHour; 
		m_i32Minute	=	m_SystemTime.wMinute; 
		m_i32Second	=	m_SystemTime.wSecond;
		m_i32Year		=	m_SystemTime.wYear;	

		m_ui32DateHour		= (m_i32Month * 10000) + (m_i32Day * 100) + m_i32Hour; 
		m_ui32Date			= (m_i32Month * 100) + m_i32Day;
		m_ui32DateTimeSec	= (m_i32Month*100000000) + (m_i32Day*1000000) + (m_i32Hour*10000) + (m_i32Minute*100) + m_i32Second;

		// Standard Time
		SYSTEMTIME UTCTime;
		TIME_ZONE_INFORMATION TZInfo;

		::GetSystemTime(&UTCTime);
		::GetTimeZoneInformation(&TZInfo);
		TZInfo.DaylightBias = 0;			// 타임존 정보에서 섬머타임을 적용하지 않도록 보정값 변경
		::SystemTimeToTzSpecificLocalTime(  &TZInfo,  &UTCTime,  &m_StandardSystemTime );

		// 상점 인증에는 표준시만 사용하므로 표준시로 인벤토리 시간 계산
		m_i32Std_Month		=	m_StandardSystemTime.wMonth; 
		m_i32Std_Day		=	m_StandardSystemTime.wDay; 	
		m_i32Std_Hour		=	m_StandardSystemTime.wHour; 
		m_i32Std_Minute	=	m_StandardSystemTime.wMinute; 
		m_i32Std_Second	=	m_StandardSystemTime.wSecond;
		m_i32Std_Year		=	m_StandardSystemTime.wYear;	

		m_ui32LocalDate		= ((m_i32Year - 2000) * 100000000) + (m_i32Month * 1000000) + (m_i32Day * 10000) + (m_i32Hour* 100) + m_i32Minute; 
		m_ui32InvneTime		= ((m_i32Std_Year - 2000) * 100000000) + (m_i32Std_Month * 1000000) + (m_i32Std_Day * 10000) + (m_i32Std_Hour* 100) + m_i32Std_Minute; 
		m_ui32InvneTimeSec	= (m_i32Std_Month*100000000) + (m_i32Std_Day*1000000) + (m_i32Std_Hour*10000) + (m_i32Std_Minute*100) + m_i32Std_Second;
		m_ui64DateTime		= ((m_i32Year - 2000) * 10000000000) + (m_i32Month * 100000000) + (m_i32Day * 1000000) + (m_i32Hour * 10000) + (m_i32Minute * 100) + m_i32Second;

		return r32DeltaTime; 
	}
	INT32	GetServerTimeDay()					{	return m_i32Day;		};
	INT32	GetServerTimeHour()					{	return m_i32Hour;		};

	void SetCreateRoomTime_NC_LOG(void)
	{ 
		m_TimeCreateRoom.wYear		= m_SystemTime.wYear;
		m_TimeCreateRoom.wMonth		= m_SystemTime.wMonth; 
		m_TimeCreateRoom.wDay		= m_SystemTime.wDay; 
		m_TimeCreateRoom.wHour		= m_SystemTime.wHour; 
		m_TimeCreateRoom.wMinute	= m_SystemTime.wMinute;
		m_TimeCreateRoom.wSecond	= m_SystemTime.wSecond;	
	}

	void SetStartBattleTime_NC_LOG(void)
	{
		m_TimeStartBattle.wYear		= m_SystemTime.wYear;
		m_TimeStartBattle.wMonth	= m_SystemTime.wMonth; 
		m_TimeStartBattle.wDay		= m_SystemTime.wDay; 
		m_TimeStartBattle.wHour		= m_SystemTime.wHour; 
		m_TimeStartBattle.wMinute	= m_SystemTime.wMinute;
		m_TimeStartBattle.wSecond	= m_SystemTime.wSecond;	
	}

	UINT32 GetLastUpdateTime()		{ return m_i32LastUpdateTime; }
	// 계급 진급 처리	
	BOOL SetUserRank(UINT32 ui32Exp, UINT32 * pui32NowRank);	//exp는 -가 없다. 
	
	CShop*		GetShop()		{	return m_pShop;	};

	// Field Shop
	UINT32			GetFieldShopVersion(); 
	FieldShopInfo*	GetFieldShop()	{	return &m_FieldShop; };

	bool	GetTSEventInfo(UINT32 ui32CurEventTime, TS_EVENT_INFO* pTSEventInfo);

	void					SetDefaultItem( UINT8 ui8DefaultItemCount, DEFAULT_ITEM* pDefaultItem );
	DEFAULT_ITEM_SET*		GetDefaultItemSet( UINT8 ui8Type, UINT8 ui8Level = -1 );
};

extern CContextMain * g_pContextMain;

#endif //_CONFIG_H__