#ifndef _CONTEXT_MAIN_H__
#define _CONTEXT_MAIN_H__

//------------------------------------------------------------------------------------------------------
//CContextMain에 있는 변수들은 글로벌 변수처럼 사용합니다. 
#include "CapsuleItem.h"
#include "../../CommonServerSource/DatabaseConfig.h"
#include "UserEvent.h"

class CShop;
class CUserSession;

struct CONNECT_SERVER_NET_BUFFER
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_WritePos;
	INT32				_ReadPos;
};

#define CLAN_LIST_COUNT	2

struct FieldShopInfo;
class CContextMain :	public i3ElementBase
{
	I3_CLASS_DEFINE( CContextMain );

protected :
	INT32					m_i32UpdateTimeSS;
	INT32					m_i32LastUpdateTime;
	INT32					m_i32UpdateTimeHH;

	void					_UpdateBoostEvent();

public:	
	PB_VER					m_eServerVersion;
	//------------------------------------------------------------------------------------------------------
	//Basic Info 
	//UINT32					m_ui32CommonServerCount;
	//COMMON_SERVER_INFO		m_aCommonServerInfo[10];
	UINT32					m_ui32GameServerCount;
	GAME_SERVER_INFO		m_aGameServerInfo[ MAX_GAME_SERVER_COUNT ];
	UINT16					m_ui16ChannelUserCount[ MAX_GAME_SERVER_COUNT ][ GAME_CHANNEL_COUNT ];		// 채널당 현재 인원  수
	UINT16					m_aui16InitChannelCount[ MAX_GAME_SERVER_COUNT ];							// 초기 채널당 MAX인원 수
	UINT16					m_ui16NowChannelCount[ MAX_GAME_SERVER_COUNT ];								// 실적용되고 동적으로 변경되는 m_aui16InitChannelCount값보다 클수없는 채널당 MAX인원 수  
	UINT32					m_ui32ChannelCountPer;														// 채널 퍼센테이지


	char					m_strMD5Key[2][NET_MD5_KEY_SIZE+1];

	SS_VERSION				m_ControlVersion;
	SS_VERSION				m_Version;
	SC_VERSION				m_ClientVersion;

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

	HMODULE					m_hGameGuard;
	
	GSHackCheckRatio		m_pHackCheckRatio;

	//------------------------------------------------------------------------------------------------------
	//Basic Value 
	INT32					m_i32Gateway;					// Gateway Server 인지 확인
	INT32					m_i32ServerIdx;					//서버 번호
	UINT16					m_ui16ClientPort;				//게임 UDP포트 번호
	
	UINT8					m_ui8AddressCount;
	UINT32*					m_pui32AddressIp;
	UINT16*					m_pui16AddressPort;
	UINT32*					m_pui32ProxyAddressIp;
	UINT8*					m_pui8AddressTimeOut;
	BOOL					m_bAbusingCheck;
	UINT32					m_ui32LoginCheckTime;

	INT32					m_i32UpdateMainTime;
	UINT32					m_ui32UpdateChannel;

	// 서동권 - 신고하기 기능 스크린샷 전송 FTP 주소
	char					m_strReportFtpAddress[ MAX_REPORT_FTP_ADDRESS ];
	UINT16					m_ui16ReportFtpPort;

	// 임준현 - 이태리 빌링 연동을 위한 Cash 충전 URL
	char					m_astrBillingURL[ LINK_METHOD_MAX ][ MAX_BILLING_URL ];

	//중계 서버 
	UINT32					m_ui32TransServerIp;
	UINT16					m_ui16TransServerPort;

	//클랜 서버 	
	BOOL					m_bClanActive;
	UINT32					m_ui32ClanServerIp;
	UINT16					m_ui16ClanServerPort;

	//메신저 서버
	BOOL					m_bMessengerActive;
	UINT32					m_ui32MessengerServerIp;
	UINT16					m_ui16MessengerServerPort;

	//게임가드 연동
	INT32					m_i32GameGuard;

	//관제툴 서버
	UINT32					m_ui32ControlServerIp;
	UINT16					m_ui16ControlServerPort;

	//관제툴 서버
	UINT32					m_ui32CastServerIp;
	UINT16					m_ui16CastServerPort;

	//엔씨 로그D
	BOOL					m_bLogDActive;
	UINT32					m_ui32LogDServerIp;
	UINT16					m_ui16LogDServerPort;

	UINT32					m_ui32ZLogWrite;	// 특정 ZLog 데이터 기록 여부. 추후 확장 구현 핋요.

	//로그파일 패스 
	wchar_t					m_wstrLogFilePath[ SERVER_STRING_COUNT ];					//Log 파일 패스 

	char					m_strZLogFilePath[SERVER_STRING_COUNT];				//ZLog 파일 패스 
	char					m_strZLogBackupPath[SERVER_STRING_COUNT];			//ZLog 백업 경로
	INT32					m_i32ZLogKeepDay;

	BOOL					m_bMacAddressCheck;
	INT32					m_i32HackKickType;

	char					m_strEnterPass[ NET_SERVER_PW ];
	//------------------------------------------------------------------------------------------------------
	//각종 카운트 Count 
	UINT32					m_ui32ThreadCount;					//쓰레드 카운트 
	INT32					m_i32SessionCount;					//유저 카운트		
	UINT32					m_ui32ChannelSessionCount;			//세션 카운트 
	UINT32					m_ui32ExtraSessionCount;			//Extra를위한 공간 채널을 선택하지 않은 유저들을 위한 공간 	

	char					m_strServerStartTime[ SMALLDATE_STRING_SIZE ];

	INT16					m_i16RSAKeySize;
	INT8					m_pRSAKey[ RSA_KET_SIZE ];
	UINT8					m_ui8MultiWeaponCount;				// 다중무기슬롯 최대개수(1~10)
	//------------------------------------------------------------------------------------------------------
	//공지 
	TTCHAR					m_strNormalNotice[ NOTICE_STRING_MAX_COUNT ];
	TTCHAR					m_strEmergencyNotice[ NOTICE_STRING_MAX_COUNT ];

	//------------------------------------------------------------------------------------------------------
	//임무카드
	//일반 임무카드
	QUEST_CTX				m_pQuest[ QUEST_CARDSET_TYPE_COUNT ][ MAX_QUEST_PER_CARDSET_NEW ];
	QUEST_ITEM_INFO_NEW2	m_pQuest_ItemInfo[ QUEST_CARDSET_TYPE_COUNT ];
	UINT16					m_ui16QuestComplete[ QUEST_CARDSET_TYPE_COUNT ][ MAX_CARD_PER_CARDSET ];
	// Event 임무카드
	QUEST_CTX				m_pQuestEvent[ LOCALE_CODE_MAX ][ MAX_QUEST_PER_CARDSET_NEW ];
	QUEST_ITEM_INFO_NEW2	m_pQuestEvent_ItemInfo[ LOCALE_CODE_MAX ];
	UINT16					m_ui16QuestEventComplete[ LOCALE_CODE_MAX ][ MAX_CARD_PER_CARDSET ];
	UINT32					m_ui32QuestCardGoodsID[QUEST_CARDSET_TYPE_COUNT];

	//이벤트 임무 카드명
	char					m_astrEventQuestCard[ LOCALE_CODE_MAX ][ MAX_STRING_COUNT ];

	//이벤트 쿼스트 
	UINT32					m_ui32QuestEventDate;

	//------------------------------------------------------------------------------------------------------
	//Channel	
	UINT8*					m_pChannelType;						//채널 정보

	//UINT8					m_ui8ChannelIdx[ GAME_CHANNEL_COUNT ][ ENTRANCE_CHANNEL_SET_COUNT ];						//채널 정보

	//UINT8					m_ui8ChannelTypeCount;
	//ENTRANCE_CHANNEL_TYPE	m_ChannelType[ ENTRANCE_CHANNEL_COUNT ];
	
	//------------------------------------------------------------------------------------------------------
	// 상점 리스트 버퍼 - 더블버퍼링
	CShop*					m_pShop;

	// Field shop Information
	FieldShopInfo			m_FieldShop;

	//------------------------------------------------------------------------------------------------------
	// 클랜 버퍼 	
	INT32					m_i32ClanMax;
	INT32					m_i32ClanVersion;
	CLAN_CONTEXT			m_ClanContext[ CLAN_LIST_COUNT ];
	CLAN_LIST_BASIC_INFO*	m_ClanList[ CLAN_LIST_COUNT ];
	INT32					m_i32ActiveClanIdx;

	UINT8					m_ui8ClanCreateRank;
	INT32					m_i32ClanCreatePoint;

	UINT8					m_ui8LocalCode;

	T_ItemID				m_TBasicWeapon[ LOCALE_CODE_MAX ][ CHAR_EQUIPMENT_WEAPON_COUNT ];
	ITEM_INFO				m_TBasicEquip[ CHAR_EQUIPMENT_PARTS_COUNT ];
	T_ItemID				m_TBasicCommonEquip[ LOCALE_CODE_MAX ][ CHAR_EQUIPMENT_COMMON_COUNT ];

	// Control 서버에서 받는 정보
	UINT16					m_ui16BoostEventCount;
	BOOST_EVENT				m_stBoostEvent[ BOOST_EVENT_ALL_LOCALE_COUNT ];
		
	// Server 에서 사용할 변수
	CUserEventManager		m_UserEvent;
		
	RANKUP_ITEM				m_aRankUpItem[ MAX_RANK_COUNT ];			// RankUp 아이템 정보

	//////////////////////////////////////////////////////////////////////////
	// 판수 보상
	//////////////////////////////////////////////////////////////////////////
	DAILY_GIFT				m_DailyGift;

	INT32					m_i32RateMax[ DAILY_GIFT_TYPE_MAX ];
	INT32					m_i32DailyGiftCount[ DAILY_GIFT_TYPE_MAX ];

	TS_EVENT_INFO			m_stTSEvent[ TS_EVENT_COUNT ];			// TS 이벤트 정보
	TS_EVENT_INFO			m_stSelectedTSEvent;
	UINT8					m_ui8GoodsOrder;
	// ----------------------------------------------------------------------------------

	DEFAULT_ITEM_SET		m_DefaultItemSet[ DEFAULT_ITEM_TYPE_MAX ];

	// v1.5 Medal
	MEDAL_SYSTEM_INFO					m_MedalSystemInfo;		// 메달 시스템 설정 정보
	MQF_ALL_MEDAL_INFO					m_MQFMedal[LOCALE_CODE_MAX];	// 전체 메달 MQF 정보. 국가별.
	MQF_ALL_DAILY_MEDAL_SET_INFO		m_MQFAllDailyMedalSet;	// 전체 일일 메달 셋트 MQF 정보.
	MQF_ALL_EVENT_MEDAL_SET_INFO		m_MQFAllEventMedalSet;	// 전체 이벤트 메달 셋트 MQF 정보.

	CGachaShop				m_GachaShop;

	// 셧다운
	AAS_OPTION_DATA			m_AASOption;
	UINT16					m_ui16AASCurTime;					// AAS리셋시간으로부터 몇 분이 지났는가?
	bool					m_bAASMethodAppointed;

	CContentsMgr			m_ContentsManager;
	CDatabaseMgr			m_DatabaseConfig;

	// IOCP 패킷 로그
	// 초. Control 서버 전송용
	UINT64					m_ui64PacketOldCount;
	UINT64					m_ui64PacketOldSize; 
	INT32					m_i32PacketCount;
	INT32					m_i32PacketSize;
	// 시간. 로그 기록용
	UINT64					m_ui64RecvPacketOldCount;
	UINT64					m_ui64RecvPacketOldSize;
	UINT64					m_ui64RecvPacketCount;
	UINT64					m_ui64RecvPacketSize; 
	UINT64					m_ui64SendPacketOldCount;
	UINT64					m_ui64SendPacketOldSize;
	UINT64					m_ui64SendPacketCount;
	UINT64					m_ui64SendPacketSize;

	INT32					m_i32Average_Min;		// 분 카운트
	INT32					m_i32AverageCCU_Min;	// 1시간동안 평균 유저 카운트. 1분마다.
	INT32					m_i32AverageRoom_Min;	// 1시간동안 평균 룸 카운트. 1분마다.

	// 유저 패킷 로그
	INT32					m_i32UserPacketIdx;

	UINT64**					m_ppui64UserPacketOldCount;
	REAL64**					m_ppr64UserPacketOldTime;

	UINT64**					m_ppui64UserPacketCount;
	REAL64**					m_ppr64UserPacketTime;

	GIFT_BUY_RANKING		m_GiftBuyRanking;
	
	// 상점 태그 
	UINT8					m_ui8TagPriority[SHOP_TAG_COUNT];
	UINT8					m_ui8TagCount;

	// 장성급 지급 아이템
	UINT8					m_ui8GeneralItemCount;
	UINT32					m_aaui32GeneralItem[MAX_GENERAL_ITEM_COUNT];

	//Vote Limit
	VoteLimit				m_vlVoteLimit;

	//ChatHack
	ChatHackLimit			m_chlChatHackLimit;

	//공지내용
	TTCHAR					m_strSlidingNotice[NOTICE_STRING_COUNT];
	TTCHAR					m_strChatNotice[NOTICE_STRING_COUNT];
	INT32					m_i32SlidingSize;
	INT32					m_i32ChatSize;
	INT32					m_i32SlidingRGB;
	INT32					m_i32ChatRGB;

	//URL List 
	UINT8					m_ui8URLVersion = 0;
	UINT8					m_ui8URLListCount;
	URL_LIST				m_ulURLList[URL_MAX_COUNT];

	//GiftShopRank
	UINT8					m_ui8GiftShopRank;

public:
	CContextMain(void);
	~CContextMain(void);

	BOOL					Create();
	BOOL					Init();	

	INT32					LoadLogInfo( const char * pstrFileName );
	BOOL					LoadConfigFile( const char * pstrFileName );					// Config File
	BOOL					LoadGameGuard( const char * pstrFilePath );					// GameGuard
	

	CLAN_CONTEXT*			GetClanContext()		{	return &m_ClanContext[m_i32ActiveClanIdx];	}
	CLAN_CONTEXT*			GetClanContextBack()	{	return &m_ClanContext[(m_i32ActiveClanIdx+1)%CLAN_LIST_COUNT];	}

	CLAN_LIST_BASIC_INFO*	GetClanList()			{	return m_ClanList[m_i32ActiveClanIdx];	}
	CLAN_LIST_BASIC_INFO*	GetClanListBack()		{	return m_ClanList[(m_i32ActiveClanIdx+1)%CLAN_LIST_COUNT];	}

	void					SetChangeClanActive()	{	m_i32ActiveClanIdx++;	m_i32ActiveClanIdx %= CLAN_LIST_COUNT;	}
		
	BOOL					GetChannelBonusInfo( UINT8 ui8LocaleCode, UINT8* pui8ChannelBonusCount, TYPE_CHANNEL_BONUS* pstChannelBonus );
	BOOL					GetRoomBonus( UINT8 ui8LocaleCode, EVENT_BONUS_TYPE eEvent, UINT32 ui32SubType, REAL32* pr32Exp, REAL32* pr32Point );

	T_ItemID*				GetDefaultCommonSetting( UINT8 m_ui8LocaleCode );
	T_ItemID*				GetDefaultCharWeaponSetting( UINT8 m_ui8LocaleCode );
	ITEM_INFO*				GetDefaultCharEquipSetting( void );
		
	BOOL					IsPlayStage( STAGE_ID ui32StageID );

	BOOL					CheckMD5Key(char * pKey);
	
	void					GetQuest( UINT8 ui8CardType, INT8 i8QuestSlotIdx, QUESTING_INFO * pQuestInfo, UINT8* pui8QuestCtxActive, UINT8 ui8LocaleCode );

	void					OnUpdate();

	BOOL					ConvertDate(UINT32 ui32SrcDate, tm &ConvertDate);

	bool					CheckAASBlockON( UINT16 ui16Mask )
	{
		return ( IsContentsEnabled( CONTENTS_SHUTDOWN ) && m_AASOption.m_ui8AASEnableFlag != 0 && ( m_AASOption.m_ui16AASProhibitionFlag & ui16Mask ) );
	}

	void					_UpdateAAS();

	void					SetServerStartTime()								{ i3ThreadTimer::LocalTime().GetSmallDate( m_strServerStartTime, SMALLDATE_STRING_SIZE ); }
	char*					GetServerStartTime()								{ return m_strServerStartTime; }

	UINT32					GetLastUpdateTime()		{ return m_i32LastUpdateTime; }
	
	CShop*					GetShop()		{	return m_pShop;	};

	// Field Shop
	UINT32					GetFieldShopVersion(); 
	FieldShopInfo*			GetFieldShop()	{	return &m_FieldShop; };

	//1.0 임무카드
	BOOL					LoadEventQuestFile();						//이벤트 임무카드 파일 로드
	BOOL					LoadQuestFile(const char * pstrFileName);	//일반 임무카드 로드
	

	// v1.5 Medal
	BOOL					LoadMedalFile(const char * pstrCurPath);		// 메달 전체 파일 로드
	INT32					LoadMedal(INT32 i32LocaleCode, const char * pstrFileName);			// 일반 메달 파일 로드
	INT32					LoadMedalSet(const char * pstrFileName, MQF_MEDAL_SET_INFO* pMedalSet);			// 메달 셋트 파일 로드


	MQF_MEDAL_SET_INFO*		GetMedalSet( UINT16 ui16MedalSetIdx); // 메달 셋트 고유 인덱스로 메달 셋트 데이터 검색.
	MQF_MEDAL_SET_INFO*		GetMedalSet_ByMedalIdx( UINT16 ui16MedalIdx); // 메달 고유 인덱스로 메달 셋트 데이터 검색.
	MQF_MEDAL_INFO*			GetMedal_MQFData(UINT16 ui16MedalIdx, MEDAL_TYPE eMedalType, UINT8 ui8LocaleCode = LOCALE_CODE_NONE);	// 메달 고유 인덱스로 MQF 메달 데이터 가져오기.  
	INT8					GetSetMedalArrIdx_ByMedalIdx( UINT16 ui16MedalIdx); // 메달 고유 인덱스로 메달 셋트에서 몇번째 메달인지 검색.

	MEDAL_MC_CHECK_POSITION	GetMMCPositionByActionID( MEDAL_MC_ACTION_ID eActionID ); // 액션 ID로 미션 체크 포지션 검색.

	BOOL					IsEnableEventMedalSet(UINT16 ui16SetIdx = 0) ;		// 서버에서 이벤트 메달 셋트가 진행중인지

	//////////////////////////////////////////////////////////////////////////
	// 판수 보상
	//////////////////////////////////////////////////////////////////////////
	BOOL					CreateDailyGift();
	T_GoodsID				GetDailyGiftReward( UINT8 ui8DailyGiftType );

	//////////////////////////////////////////////////////////////////////////

	void					GetBasicItemDBIdx(T_ItemDBIdx *pItemDBIdx);

	void					SetDefaultItem( UINT8 ui8DefaultItemCount, DEFAULT_ITEM* pDefaultItem );
	DEFAULT_ITEM_SET*		GetDefaultItemSet( UINT8 ui8Type, UINT8 ui8Level = -1 );

	void					SetUserCount( UINT16* pui16ChannelCount, UINT16 ui16ExtraCount );

	void					GetCharaPriceByItemID( T_ItemID TItemID, UINT32* pui32Point, UINT32 *pui32Cash );

	CGachaShop*				GetGachaShop()			{ return &m_GachaShop; }

	// Control contents
	void					InsertContents(ContentsInfo* pContents);
	bool					IsContentsEnabled(CONTENTS eContents)						{ return m_ContentsManager.IsContensEnabled(eContents); }
	INT32					GetContentsVersion(CONTENTS eContents)						{ return m_ContentsManager.GetContensVersion(eContents); }
	void					GetContentsStateFlag(ContentsEnableFlag* pContentsStateFlag);

	// Database config
	bool					InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)			{ return m_DatabaseConfig.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*				GetDatabaseConfig(char* pszDBName)							{ return m_DatabaseConfig.FindDatabaseConfig(pszDBName); }

	GIFT_BUY_RANKING*		GetGiftBuyRanking( void )									{	return &m_GiftBuyRanking; };

	// Clan match
public:
	void					InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason);
	/*TTCHAR*					GetClanMatchSeasonName(SEASON_TYPE eSeasonType);
	UINT32					GetClanMatchSeasonStartDate(SEASON_TYPE eSeasonType);
	UINT32					GetClanMatchSeasonEndDate(SEASON_TYPE eSeasonType);*/
	UINT32					GetClanMatchSeasonStartDate(SEASON_TYPE eSeasonType);
	void					MakePacketClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason);

	void					InsertSeasonRanking(INT32 i32SeasonRankingCount, CLAN_BASIC_INFO* pSeasonRankingClan)
	{
		m_i32SeasonRankingCount = i32SeasonRankingCount;
		i3mem::Copy( m_aSeasonRankingClan, pSeasonRankingClan, sizeof( CLAN_BASIC_INFO ) * i32SeasonRankingCount );
	}
	INT32					GetSeasonRankingCount( void )										{	return m_i32SeasonRankingCount; }
	CLAN_BASIC_INFO*		GetSeasonRankingClan( void )										{	return m_aSeasonRankingClan; }

	void					SetMatchedTeamCount( INT32 i32MatchedTeamCount )					{	m_i32MatchedTeamCount = i32MatchedTeamCount; }
	void					SetMatchingTeamCount( INT32 i32MatchingTeamCount  )					{	m_i32MatchingTeamCount = i32MatchingTeamCount; }
	INT32					GetMatchedTeamCount( void )											{	return m_i32MatchedTeamCount; }
	INT32					GetMatchingTeamCount( void )										{	return m_i32MatchingTeamCount; }
		
	UINT8					RegistPBVersion(UINT8 ui8PBVer);
	//ENTRANCE_CHANNEL_TYPE*	GetChannelType( INT32 i32ChannelIdx, INT32 i32SetIdx )				{	return &m_ChannelType[ m_ui8ChannelIdx[ i32ChannelIdx ][ i32SetIdx ] ];		}
	//BOOL					IsChannelType( ChannelType eChannelType, INT32 i32ChannelIdx );
	//INT32					GetChannelIdx( INT32 i32ChannelIdx

	UINT8					GetMaxPerson( STAGE_ID ui32StageID );
	STAGE_EVENT				GetStageEvent( STAGE_ID ui32StageID );
	INT32					GetRuleIdx( STAGE_ID ui32StageID );
	UINT8					GetDefaultStage( STAGE_ID ui32StageID );

	void					AddUserPacketProcess(INT32 i32ThreadIdx, INT32 i32ProtocolValue, REAL64 r64PacketProcessTime );
public:
	void					SetGeneralItem();

private:
	CClanMatchSeason		m_ClanMatchSeason[ SEASON_TYPE_COUNT ];
	INT32					m_i32SeasonRankingCount;
	CLAN_BASIC_INFO			m_aSeasonRankingClan[MAX_SEASON_RANKING_TEAM_COUNT]; 
	INT32					m_i32MatchedTeamCount;			// 매칭된 클랜 매치팀 수 - 전체 서버. 
	INT32					m_i32MatchingTeamCount;			// 매칭대기(퀵매치버튼누른) 클랜 매치팀 수 - 전체 서버 .

};

extern CContextMain * g_pContextMain;

#endif //_CONFIG_H__