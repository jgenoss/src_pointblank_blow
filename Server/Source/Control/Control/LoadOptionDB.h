#ifndef _LOAD_OPTION_DB_H__
#define _LOAD_OPTION_DB_H__

#define DATABASE_RELOAD_TIME	60		// sec, Change 테이블만 적용.( ex : TPBC_Name ) 

struct TYPE_MAP_OPTION
{
	//INT32	_i32DBIdx;
	char	_strMapName[ STAGE_CODE_NAME_COUNT ];
	INT32	_i32MapState;
	INT8	_i8MapOpen;
	//INT8	_i8M_Mode[STAGE_MODE_ACTIVE_TYPE_MAX];
	INT32	_i32MapIdx;
	UINT8	_ui8Mark;
};

struct IN_DB_SUPER_USER
{
	INT32		iSessionIdx; 
	wchar_t		wstrID[NET_ID_SIZE];
	wchar_t		wstrPass[NET_PW_SIZE]; 
};

struct OUT_DB_SUPER_USER
{
	INT32	i32Rv;
	INT32	iSessionIdx; 
	INT8	i8Service; 
	wchar_t	wstrID[NET_ID_SIZE];
};

class CLoadOptionDB :	public i3Thread
{
	I3_CLASS_DEFINE( CLoadOptionDB );

protected :
	UINT32					m_ui32CheckSec_New;
	UINT32					m_ui32CheckSec_Old;

	char					m_strUdlLogPath[SERVER_STRING_COUNT];	
	CADODatabase*			m_pDB;
	
	BOOL					_LoadDatabaseConfig();

	BOOL					_LoadContentsControl( );
	BOOL					_LoadFixBasic( );
	BOOL					_LoadMap( );
	BOOL					_LoadRandomMap();
	BOOL					_LoadMissionCard( );
	BOOL					_LoadAttendance( );
	BOOL					_LoadMedal( );
	BOOL					_LoadDefaultItem( );
	BOOL					_LoadGiftBuyRanking( );
	BOOL					_LoadChannelList( );
	BOOL					_LoadGachaSystem( );
	BOOL					_LoadVoteLimit( );
	BOOL					_LoadChatHack( );
	BOOL					_LoadGameNotice( );
	BOOL					_LoadBillingURL();
	BOOL					_LoadURLList();

	BOOL					_LoadChangeBasic( );
	BOOL					_LoadDailyGift( );
	BOOL					_LoadBoostEvent( );	
	BOOL					_LoadMACAddress( );
	BOOL					_LoadNotice( );
	BOOL					_LoadGeneralRankup( );
	BOOL					_LoadNewUserEvent( );
	BOOL					_LoadLoginEvent( );
	BOOL					_LoadRankUpItem( );
	BOOL					_LoadIPBlock( );
	BOOL					_LoadNowChannelUserCount( );
	BOOL					_LoadClanMatchSeason( );
	BOOL					_LoadTagPriority( );
		
	void					_MakeChannelGroup();

public:
	INT32					m_i32Version;
	BOOL					m_bRunning;

	Local_CRingBuffer*		m_pInLogIN;
	Local_CRingBuffer*		m_pOutLogIN;
 
	CLoadOptionDB(void);
	~CLoadOptionDB(void);

	BOOL					OnCreate();
	void					OnDestroy(void); 

public: 

	virtual UINT32			OnRunning( void * pUserData );
	void					ErrorDB( wchar_t* strSQL );

	BOOL					FixLoadDB( void );
	BOOL					ChangeLoadDB( INT32 i32Version );
	INT32					GetControlVersion( );
	
	

	//Control Tool : 유저 인증 부분 ZP_LOG -> PBOption 으로 변경.
	INT32					UserLogIn( INT32 i32SessionIdx, wchar_t* strID, wchar_t* strPass );
	INT32					GetUserLogInCount(void)				{ return m_pOutLogIN->GetBufferCount(); }
	OUT_DB_SUPER_USER*		GetUserLogIn(void)					{ return (OUT_DB_SUPER_USER*) m_pOutLogIN->Pop(); }
	void					GetUserLogInIdx(void)				{ m_pOutLogIN->PopIdx(); }
	
	

private :
	INT32			m_i32LoadOptionDBIdx;

	void	InitRandomMapIdx(UINT16& ui16StartIdx, UINT16 ui16EndIdx, UINT8& ui8CurrentRule, UINT8& ui8CurrentMaxPerson);
};

extern CLoadOptionDB * g_pLoadOptionDB;

#endif //_LOAD_OPTION_DB_H__