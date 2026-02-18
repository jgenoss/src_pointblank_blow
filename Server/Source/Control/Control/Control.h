#ifndef _CONTROL_H__
#define _CONTROL_H__

struct TYPE_RENOTICE
{
	BOOL			_bNoticeActive;
	UINT32			_ui32NoticeReTime;
	UINT32			_ui32NoticeSendTime;
	TTCHAR			_strNotice[ STR_NOTICE_MESS_SIZE ];

	// 공지기능에 시작시간, 끝시간 추가	2011.07.12 안국정
	UINT32			_ui32BeginTime;
	UINT32			_ui32FinishTime;

};

class CSession;
struct ServerHackCheckRatio;

class CControl :	public i3ElementBase
{
	I3_CLASS_DEFINE( CControl );

private :	
	HANDLE					m_hChangeEvnet;
	char					m_strReFileName[ MAX_PATH ];
	char					m_strReDirName[ MAX_PATH ];

	i3Timer					m_Timer;

	CGachaShop				m_GachaShop;
	CContentsMgr			m_ContentsManager;
	CClanMatchSeason		m_ClanMatchSeason[SEASON_TYPE_COUNT];

public:	
	 BOOL					m_bReloadConfig;

	//Common 
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	UINT32					m_ui32UpdateTime;
	INT32					m_i32UserCount;
	UINT8					m_ui8UseServerSelect;					// 한방에 삭제 합시다.
	UINT32					m_ui32QuestEvent;
	INT32					m_i32ClanMaxCount;
	UINT8					m_ui8LocaleCode;
	//INT8					m_i8PrimaryWeapon;
	UINT8					m_ui8MultiWeaponCount;		// 다중무기슬롯 최대개수(1~10)
	UINT16					m_ui16WebPort;
	UINT16					m_ui16WebSessionCount;
	PB_VER					m_eServerVersion;

	UINT32					m_ui32GameType;
	UINT32					m_ui32AreaCode;
	UINT32					m_ui32ServerCode;

	TYPE_RENOTICE			m_aReNotice[ NOTICE_COUNT ];

	// Version
	SS_VERSION				m_ControlVersion;

	//Server List 
	UINT32					m_ui32CommonServerCount;
	SERVER_INFO_COMMON		m_aCommonServerList[MAX_SERVER_COUNT];
	
	UINT32					m_ui32UpdateServerCount;
	SERVER_INFO_COMMON		m_aUpdateServerList[MAX_SERVER_COUNT];

	UINT32					m_ui32GameServerCount;
	SERVER_INFO_GAME		m_aGameServerList[MAX_SERVER_COUNT];
	UINT32					m_aui32RecvHeartBit2Count[MAX_SERVER_COUNT];	// HeartBit2 

	UINT32					m_ui32BattleServerCount;
	SERVER_INFO_BATTLE		m_aBattleServerList[MAX_SERVER_COUNT];

	UINT32					m_ui32CastServerCount;
	SERVER_INFO_CAST		m_CastServerList[MAX_SERVER_COUNT];

	// IP Block
	UINT8					m_ui8IPBlockIPCount;
	IPBLOCK_IP				m_stIPBlockIPOpen[ IPBLOCK_IP_OPEN_COUNT ];

	UINT8					m_ui8IPBlockNationCount;
	UINT8					m_ui8IPBlockNationOpen[ IPBLOCK_NATION_OPEN_COUNT ];

	// Dedication server information from control tool
	GSHackCheckRatio*		m_pGameHackCheckRatioList;
	ServerHackCheckRatio*	m_pDediHackCheckRatioList;

	UINT8*					m_pui8NotAllowRoomCreationList; // 0 : default, 1 : Not allow Room creation (from control tool)

	UINT8					m_ui8RuleTotalCount;
	MAP_RULE_DATA			m_aRuleData[RULE_DATA_MAX];

	UINT8					m_ui8StageTotalCount;
	MAP_STAGE_DATA			m_aStageData[STAGE_TOTAL_MAX];

	UINT16					m_ui16StageMatchingCount;
	MAP_STAGE_MATCHING		m_aStageMatching[STAGE_MATCHING_MAX];

	UINT16					m_ui16RandomMapCount;
	MAP_RANDOM_STAGE_DATA	m_aRandomMapData[STAGE_MATCHING_MAX];
	MAP_RANDOM_IDX_DATA		m_aRandomMapIdxData[RULE_DATA_MAX];
	
	UINT8					m_ui8ActiveQuestCount;
	UINT32*					m_pui32ActiveQuest;

	UINT32					m_ui32BattleRoomCount;
	UINT8					m_ui8BattlePortCount;
	UINT16					m_aui16BattlePortList[ 20 ];
	
	//Contorl User List 
	UINT32					m_ui32MacAddressListCount; 
	UINT64					m_aui64MacAddressList[ MACADDRESS_COUNT ];

	// Event Bonus
	UINT16					m_ui16BoostEventCount;
	BOOST_EVENT				m_stBoostEvent[ BOOST_EVENT_ALL_LOCALE_COUNT ];
	
	// Event Item
	UINT16					m_ui16NewUserEventCount;
	NEW_USER_EVENT			m_stNewUserEvent[ NEW_USER_EVENT_ALL_LOCALE_COUNT ];	// 선물받기 시스템 사용시

	UINT16					m_ui16LoginUserEventCount;
	LOGIN_USER_EVENT		m_stLoginUserEvent[ LOGIN_USER_EVENT_ALL_LOCALE_COUNT ];	// 선물받기 시스템 사용시

	// TS Event Info
	TS_EVENT_INFO			m_aTSEvent[ TS_EVENT_COUNT ];

	UINT8					m_ui8DefaultItemCount;
	DEFAULT_ITEM			m_TDetaultItem[ DEFAULT_ITEM_COUNT ];

	// GeneralRankup
	INT32 					m_i32GeneralRank[ GENERAL_RANK_COUNT ];
	INT32 					m_i32GeneralRankStartTime;

	char					m_strServerStartTime[ SMALLDATE_STRING_SIZE ];

	wchar_t					m_wstrLogFilePath[SERVER_STRING_COUNT];
	char					m_strServerLogFilePath[SERVER_STRING_COUNT];
	char					m_strHackLogFilePath[SERVER_STRING_COUNT];
	wchar_t					m_astrServiceName[ASC_SERVER_COUNT][64];		//0: 중계서버 1: 클랜서버 2: 메신저서버 3: 릴레이 서버 4:랑데뷰서버 10: 게임 서버 11: Dedication 서버
	// 서동권 - 신고하기 기능 스크린샷 전송 HTTP 주소
	char					m_strReportFtpAddress[ MAX_REPORT_FTP_ADDRESS ];
	UINT16					m_ui16ReportFtpPort;

	// 임준현 - 빌링 연동을 위한 Cash 충전 URL 주소
	UINT8					m_ui8BillingUrlCount;
	char					m_astrBillingURL[LINK_METHOD_MAX][MAX_BILLING_URL];

	//Max CCU
	INT32					m_i32CCUNow_Day;				// CCU가 기록되고 있는 마지막 날짜
	INT32					m_i32CCUMaxCount_Day;			// 최대 CCU Count Day
	UINT32					m_ui32CCUMaxTime_Day;			// 최대 CCU 날짜
	
	INT32					m_i32CCUNowCount;				// 현재 CCU Count
	UINT32					m_ui32CCUNowTime;				// 현재 CCU 받은 시간

	char					m_strKey1[NET_MD5_KEY_SIZE + 1];
	char					m_strKey2[NET_MD5_KEY_SIZE + 1];

	
	INT32					m_i32NewUserPoint;				// 신규 유저 지급 포인트
	INT32					m_i32ClanCreateRank;			// 클랜 생성 가능한 최소 랭크
	INT32					m_i32ClanCreatePoint;			// 클랜 생성시 차감되는 포인트
	UINT8					m_ui8ClanCreateMemberCount;		// 클랜 생성시 가입 맴버 수

	// 출석체크 이벤트 - 서동권 2011.07.05
	ATTENDANCE_INFO			m_aAttendanceInfo[ATTENDANCE_TYPE_MAX][ATTENDANCE_MAX_EACH_EVENT];

	RANKUP_ITEM				m_aRankUpItem[MAX_RANK_COUNT];			// RankUp 아이템 정보

	DAILY_GIFT				m_DailyGift;

	// dedication server
	BOOL					SetWeaponList( UINT32* pui32WeaponList, UINT32 ui32ItemCount);
	UINT32					GetSIAVersion()							{ return m_ui32SIAVersion; }
	void					SetSIAVersion( UINT32 ui32SIAVersion)	{ m_ui32SIAVersion = ui32SIAVersion; }
	
	BOOL					GetWeaponList( UINT32* pui32Out, UINT32 ui32Bytes, UINT32* pui32OutItemCount ) ;

	//For hack 
	UINT32			*		m_pui32DayHackCount;
	UINT32			*		m_pui32HourHackCount;

	// E-mail Send System
	//INT32					m_i32ServerType;	// 크래쉬한 서버 타입.
	//char					m_strServerIp[32];	// 크래쉬한 서버 Ip . ex : 192.168.1.1

	MEDAL_SYSTEM_INFO		m_MedalSystemInfo;	// 메달 시스템 설정 정보.

	// 중독방지 시스템 설정정보
	AAS_OPTION_DATA			m_AASOption;
	CDBConfigMgr			m_DBConfig;
	CDatabaseMgr			m_DatabaseMgr;

	//상점 태그
	UINT8					m_ui8TagPriority[SHOP_TAG_COUNT];
	UINT8					m_ui8TagCount;

	//Vote Limit
	VoteLimit				m_vlVoteLimit;

	//ChatHack
	ChatHackLimit			m_chlChatHackLimit;

	//슬라이딩, 채팅 공지
	TTCHAR					m_strSlidingNotice[NOTICE_STRING_COUNT];
	TTCHAR					m_strChatNotice[NOTICE_STRING_COUNT];

	INT32					m_i32SlidingRGB;
	INT32					m_i32ChatRGB;

	//URL List 
	UINT8					m_ui8URLVersion = 0;
	UINT8					m_ui8URLListCount;
	URL_LIST				m_ulURLList[URL_MAX_COUNT];

	// Knock Server
	i3RingBuffer			m_rbWarningMessage[WARNING_TYPE_COUNT];	// Message가 만들어지는부분은 Main 쓰레드지만 꺼내가는부분은 다른 쓰레드임으로 링버퍼를 사용합니다.
	BOOL					m_bUpdateServerState;
	BOOL					m_bActiveKnockServer;
	char					m_strKnockServerAddress[ MAX_KNOCK_SERVER_ADDRESS ];
	UINT16					m_ui16KnockServerPort;

	UINT8					m_ui8GiftShopRank;
	
private:
	// dedication server

	UINT32		m_ui32SIAVersion;
	// weapon list : double buffering을 이용. "thread-safe 하지 않음"
	// control server에서만 writing. 항상 back buffer에 writing을 하고 리스트 완성 후 front buffer와 swap
	
	void					_Swap()
	{
		UINT32* p = m_pui32WeaponList;
		m_pui32WeaponList = m_pui32WeaponListBack;
		m_pui32WeaponListBack = p;

		UINT32  n = m_ui32WeaponListCount;
		m_ui32WeaponListCount = m_ui32WeaponListCountBack;
		m_ui32WeaponListCountBack = n;
	}

	UINT32*					m_pui32WeaponList;
	UINT32*					m_pui32WeaponListBack;
	UINT32					m_ui32WeaponListCount;
	UINT32					m_ui32WeaponListCountBack;

	UINT32					m_aui32Internal_WeaponList1 [ DS_MAX_WEAPON_LIST ];
	UINT32					m_aui32Internal_WeaponList2 [ DS_MAX_WEAPON_LIST ];

	GIFT_BUY_RANKING		m_GiftBuyRanking;

private:
	void					_CorrectPath( const char* pstrIn, INT32 i32Size, char* pstrOut );

	BOOL					_LoadFromFile(const char * pstrFileName);

	void					_MakeWarningMessage( INT32 i32WarningType );
	INT32					_MakeWarningFormat( char* strWarning, UINT8 ui8Type, UINT32 ui32WarningFlag, UINT32 ui32WarningCount);
	void					_Concat( char ** pDest, const char * pSrc, INT32 *i32Size);
	void					_MakeBattleGroup();

public:
	CControl(void);
	~CControl(void);

	BOOL					Create(const char * pstrFileName );
	BOOL					Destroy(void);

	INT32					LoadLogInfo(const char * pstrFileName);

	void					SetNowCCU( INT32 i32CCU )							{ m_i32CCUNowCount = i32CCU; m_ui32CCUNowTime = i3ThreadTimer::LocalTime().GetDateTimeMMDDHHMISS();	}
	void					UPdateMaxCCU( void );
	void					SetServerStartTime()								{ i3ThreadTimer::LocalTime().GetSmallDate( m_strServerStartTime, SMALLDATE_STRING_SIZE ); }
	char*					GetServerStartTime()								{ return m_strServerStartTime; }
	INT32					GetServerIdx( UINT32 ui32IpAddress, UINT8 ui8Type );
	
	//For Server 
	SERVER_INFO_COMMON*		GetServerInfo( ASC_SERVER_TYPE eServer, UINT32 ui32Idx = 0 );

	UINT32					GetCommonServerCount(void)	{ return m_ui32CommonServerCount; }
	SERVER_INFO_COMMON *	GetCommonServerList(void)	{ return m_aCommonServerList; }

	void					SetGameServerIP(INT32 i32Idx, UINT32 ui32IP); 
	SERVER_INFO_GAME *		GetGameServerInfo(INT32 i32Idx)	{ return &m_aGameServerList[i32Idx]; }

	UINT32					GetAllServerCount(void)	{ return (m_ui32CommonServerCount + m_ui32UpdateServerCount + m_ui32CastServerCount + m_ui32GameServerCount + m_ui32BattleServerCount); }

	void					DisConnectServer(INT32 i32ServerType, INT32 i32ServerIdx, CSession* pSession )
	{
		switch( i32ServerType )
		{
		case ASC_SERVER_GAME	:
			if( m_aGameServerList[i32ServerIdx]._pSession == pSession )
			{
				m_aGameServerList[i32ServerIdx]._i32IsActive	= ASC_ICON_DISCONNECTION;
				m_aGameServerList[i32ServerIdx]._pSession		= NULL;
			}
			break; 
		case ASC_SERVER_BATTLE	:
			if( m_aBattleServerList[i32ServerIdx]._pSession == pSession )
			{
				m_aBattleServerList[i32ServerIdx]._i32IsActive	= ASC_ICON_DISCONNECTION;
				m_aBattleServerList[i32ServerIdx]._pSession		= NULL;
			}
			break; 
		case ASC_SERVER_CAST:
			m_CastServerList[i32ServerIdx]._i32IsActive			= ASC_ICON_DISCONNECTION;
			break;
		case ASC_SERVER_UPDATER:
			m_aUpdateServerList[i32ServerIdx]._i32IsActive		= ASC_ICON_DISCONNECTION;
			break;
		default					:
			m_aCommonServerList[i32ServerIdx]._i32IsActive		= ASC_ICON_DISCONNECTION;
			break; 
		}
		return; 
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	//For Super User
	BOOL					CheckMacAddress( UINT64 ui64MacAddress ); 
	UINT32					GetSuperUserCount(void)	{ return m_ui32MacAddressListCount; }

	///////////////////////////////////////////////////////////////////////////////////////////

	BOOL					SetDefaultStage(const char* pstrStringValue, STAGE_MODE eType);

	CGachaShop*				GetGachaShop()																{ return &m_GachaShop; }

	void					InsertContents(CONTENTS eContents, INT32 i32Version, bool bEnable)			{ m_ContentsManager.InsertContents(eContents, i32Version, bEnable); }
	void					GetContentsList(ContentsInfo* pPacket)										{ m_ContentsManager.GetContentsList(pPacket); }

	bool					IsContentsEnabled(CONTENTS eContents)										{ return m_ContentsManager.IsContensEnabled(eContents); }
	bool					CheckDefaultContents()														{ return m_ContentsManager.CheckDefaultContents(); }

	INT32					GetDatabaseConfigCount(ASC_SERVER_TYPE eServer)								{ return m_DBConfig.GetDatabaseConfigCount(eServer); }
	bool					InsertDatabaseConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig);
	void					MakePacketDBConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig)		{ m_DBConfig.MakePacketDBConfig(eServer, pDBConfig); }

	CDBConfig*				GetDatabaseConfig(char* pszDBName)											{ return m_DatabaseMgr.FindDatabaseConfig(pszDBName); }

	bool					InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason);
	void					MakePacketClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason);

	void					SetGiftRanking( GIFT_BUY_RANKING *pGift )	
	{ 
		m_GiftBuyRanking.m_ui8GiftCount = pGift->m_ui8GiftCount;
		i3mem::Copy( m_GiftBuyRanking.m_aGiftRanking, pGift->m_aGiftRanking, sizeof( GIFT_RANKING ) * pGift->m_ui8GiftCount ); 
	}
	GIFT_BUY_RANKING*		GetGiftBuyRanking( void ) {	return &m_GiftBuyRanking; };	

	void					MakeWarningMessage();
	void					UpdateServerState()  { m_bUpdateServerState = TRUE; }

	BOOL					CheckDefaultMap();
};


extern CControl * g_pControl;

#endif //_CONFIG_H__
