#ifndef _CONFIG_H__
#define _CONFIG_H__

#define	SERVER_NAME_COUNT	32
#define IP_NAME_COUNT		32

#include "../../CommonServerSource/DatabaseConfig.h"
#include "UserEvent.h"

class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );

private:
	UINT32				m_ui32connectedUserCount;			//게임유저 카운트 
	BOOL				m_bRecvServerInfo;

	CGachaShop			m_GachaShop;

protected :
	UINT32				m_ui32SaleTime;
	UINT32				m_ui32GameServerCount;
	UINT32				m_ui32GameThreadCount;
	INT32				m_i32MacAddressListCount;
	UINT64		*		m_pui64MacAddressList;

	char				m_strServerStartTime[ SMALLDATE_STRING_SIZE ];

	void				_UpdateAAS();
	void				_UpdateAttendance();

public:	
	//ServerInfo
	UINT32				m_ui32IP;
	UINT16				m_ui16Port;
	SS_VERSION			m_ControlVersion;
	SS_VERSION			m_Version;

	wchar_t				m_wstrLogFilePath[SERVER_STRING_COUNT];			//Log 파일 패스
	char				m_strZLogFilePath[SERVER_STRING_COUNT];			//ZLog 파일 패스
	char				m_strZLogBackupPath[SERVER_STRING_COUNT];		//ZLog 백업 경로
	char				m_strServiceLogFilePath[SERVER_STRING_COUNT];	//ServiceLog 파일 패스 
	
	INT32				m_i32ZLogKeepDay;

	UINT32				m_ui32MaxUserCount;

	bool				m_bPCCafeActive;
	BOOL				m_bThaiESprotIDActive;
	BOOL				m_bThaiIDPlusActive;
	UINT8				m_ui8OverlapPartsItem;		// 1.5V 파츠 아이템 중첩 처리를 위한 임시 변수. Config파일로 설정합니다. 0: 중첩, 1: 비중첩
	UINT16				m_ui16DeliverStoragePeriod;

	// SIA
	BOOL				m_bSIAActive;
	UINT32				m_ui32SIAServerIP;
	UINT16				m_ui16SIAServerPort;

	//NC 로그D
	BOOL				m_bLogDActive;							//엔씨 로그D 모듈연결
	UINT32				m_ui32ZLogServerIP;						//엔씨 로그D 서버 아이피 
	UINT16				m_ui16ZLogServerPort;					//엔씨 로그D 서버 포트 

	//관제툴 서버 
	BOOL				m_bControlActive;
	UINT32				m_ui32ControlServerIP;
	UINT16				m_ui16ControlServerPort;

	//Server List
	UINT32	*			m_pui32GameServerIp;

	//Option
	UINT32				m_ui32RetryLoginCount;
	UINT32				m_ui32LogInThreadCount;
	UINT32				m_ui32LogInBufferCount;

	UINT32				m_ui32LoginTime;

	//Update 
	INT32				m_i32UpdateMain;
	INT32				m_i32UpdateLogFile;

	DATE32				m_dt32YesterDateYYMMDD;
	UINT32				m_ui32NewUserPoint;

	INT32				m_i32GeneralRank[ GENERAL_RANK_COUNT ];
	INT32				m_i32GeneralRankStartTime;

	// 이벤트 아이템 - 선물하기 시스템 사용시
	UINT32				m_ui32LastUpdateMI;
	UINT32				m_ui32LastUpdateHH;
	UINT32				m_ui32LastUpdateDD;

	UINT16				m_ui16EventItemNewCount;														// 신규 유저 아이템 개수	- 정보 저장 용
	NEW_USER_EVENT		m_aEventItemNew[ NEW_USER_EVENT_ALL_LOCALE_COUNT ];								// 신규 유저 아이템			- 정보 저장 용

	UINT16				m_ui16EventItemLoginCount;														// 로그인 유저 개수			- 정보 저장 용
	LOGIN_USER_EVENT	m_aEventItemLogin[ LOGIN_USER_EVENT_ALL_LOCALE_COUNT ];							// 로그인 유저 아이템		- 정보 저장 용
	
	CUserEventManager	m_UserEvent;

	UINT8				m_ui8IPBlockIPCount;
	IPBLOCK_IP			m_aIPBlockIPOpen[ IPBLOCK_IP_OPEN_COUNT ];

	UINT8				m_ui8IPBlockNationCount;
	UINT8				m_aui8IPBlockNationOpen[ IPBLOCK_NATION_OPEN_COUNT ];
		
	INT32				m_i32ActiveAttendanceIdx[ ATTENDANCE_TYPE_MAX ];
	ATTENDANCE_INFO		m_aAttendanceInfo[ ATTENDANCE_TYPE_MAX ][ ATTENDANCE_MAX_EACH_EVENT ];

	// 선물하기
	MEDAL_SYSTEM_INFO	m_MedalSystemInfo; // 메달 시스템 설정 정보

	// 셧다운
	AAS_OPTION_DATA		m_AASOption;
	bool				m_bAASMethodAppointed;	// 셧다운 ON + 지정시간 금지방식 ON + 금지된 지정시간안에 포함. 3가지 조건 모두 만족시 true. 1분마다 갱신된다.

	CContentsMgr		m_ContentsManager;
	CDatabaseMgr		m_DatabaseConfig;

	CClanMatchSeason	m_aClanMatchSeason[SEASON_TYPE_COUNT];
	
	PB_VER				m_eServerVersion;
	UINT32				m_ui32LimitAuthArg;

	// Statistics
	INT32					m_i32Average_Min;		// 분 카운트
	INT32					m_i32AverageCCU_Min;	// 1시간동안 평균 유저 카운트. 1분마다.
	UINT32					m_ui32LastUpdateHour;

	WORK_PROFILE			m_stWorkProfile;

public:
	CConfig(void);
	~CConfig(void);

	BOOL				Create();
	BOOL				Init();
	void				OnUpdate(); 

	INT32				LoadLogFile( const char * pstrFileName );
	BOOL				LoadConfigFile( const char * pstrFileName );					// Config File

	void				SetConnectedUserCount(UINT32 ui32count);
	UINT32				GetAuthTime(UINT32 ui32Sec);
	UINT32				GetAddAuthTime(UINT32 ui32Date, UINT32 ui32Sec);
	UINT32				GetDiffTime(UINT32 ui32DT1, UINT32 ui32DT2);

		
	UINT32				GetConnectedUserCount(void) const;	
	UINT32				GetGameServerCount(void)									{ return m_ui32GameServerCount;		}
	UINT32				GetGameThreadCount()										{ return m_ui32GameThreadCount;		}
	char*				GetServerStartTime()										{ return m_strServerStartTime; }
	void				GetYesterDay(); 

	void				SetUserEvent();

	BOOL				IsRecvServerInfo(void)										{ return m_bRecvServerInfo; }
	void				SetServerInfo(INT32 iServerCount, UINT32 * pServerList);

	CGachaShop*			GetGachaShop()												{ return &m_GachaShop; }

	void				InsertContents(ContentsInfo* pContents);
	bool				IsContentsEnabled(CONTENTS eContents)						{ return m_ContentsManager.IsContensEnabled(eContents); }

	bool				InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)			{ return m_DatabaseConfig.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*			GetDatabaseConfig(char* pstrDBName)							{ return m_DatabaseConfig.FindDatabaseConfig(pstrDBName); }
	CDBConfig*			GetDatabaseConfig(char* pstrDBName, INT32 i32DBNum)			{ return m_DatabaseConfig.FindDatabaseConfig(pstrDBName, i32DBNum); }

	void				InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason);

	// 셧다운
	bool				CheckTargetAASDailyUsage();
	bool				CheckTargetAASAppointed( INT32 i32Birth );

	BOOL				GetLoginUserEventGoods( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn );
	BOOL				GetLoginUserEventItem( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem );
	BOOL				GetNewUserEvent( UINT8 ui8LocaleCode, UINT8* pui8ItemCount, ST_ITEM* pItem );

	BOOL				IsPartsByItemID(T_ItemID TItemID);

private:
};


extern CConfig * g_pConfig;

#endif //_CONFIG_H__