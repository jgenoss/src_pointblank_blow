#include "pch.h"
#include "DataBase.h"
//#include "Ado.h"
//#include "LogFile.h"
#include "TaskProcessor.h"
#include "ModuleControl.h"

CDataBase * g_pDataBase = NULL;

I3_CLASS_INSTANCE( CDataBase, i3ElementBase);

static UINT32 _DefThreadProc( INT32 * pi32TheadIdx )
{
	g_pDataBase->OnWorking( *pi32TheadIdx );
	return 0;
}

static char	s_strClanTable[DB_THREAD_COUNT][SERVER_STRING_COUNT] = { "TClanMem", "TClanMem", "TClanMem", "TClanMem", "TClanMem","TClanMem", "TClanMem", "TClanMem", "TClanMem",	"TClanMem"};

CDataBase::CDataBase(void)
{	
	// 추가
	m_pTaskDB		= NULL;

	m_pbRunning		= NULL;
	m_pi32ThreadIdx	= NULL;
	m_phThread		= NULL;

	m_ppRingInDB	= NULL;	
	m_ppRingOutDB	= NULL;
}

CDataBase::~CDataBase(void)
{
	OnDestroy(); 
}

BOOL	CDataBase::OnCreate()
{
	//Main Thread 
	// Task Thread
	m_ppRingInDB			= (NSM_CRingBuffer**)i3MemAlloc(sizeof(NSM_CRingBuffer*) * DB_THREAD_COUNT );
	m_ppRingOutDB			= (NSM_CRingBuffer**)i3MemAlloc(sizeof(NSM_CRingBuffer*) * DB_THREAD_COUNT );
	m_pi32LogDataBaseIdx	= (INT32*)i3MemAlloc( sizeof(INT32)	* DB_THREAD_COUNT );

	m_pTaskDB				= (DB_QUERY_INFO*)i3MemAlloc(sizeof(DB_QUERY_INFO) * DB_THREAD_COUNT );
	m_pi32ThreadIdx			= (INT32*)i3MemAlloc(sizeof(INT32) * DB_THREAD_COUNT );
	m_phThread				= (HANDLE*)i3MemAlloc(sizeof(HANDLE) * DB_THREAD_COUNT );
	m_pbRunning				= (BOOL*)i3MemAlloc(sizeof(BOOL) * DB_THREAD_COUNT );
	
	m_pClanDBConfig = g_pConfig->GetDatabaseConfig("PBClan");
	if( !m_pClanDBConfig )
		return FALSE;

	for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
	{
		m_ppRingInDB[i]		= new NSM_CRingBuffer;
		m_ppRingOutDB[i]	= new NSM_CRingBuffer;

		m_ppRingInDB[i]->OnCreate( sizeof(RING_IN_DB), 100 );
		m_ppRingOutDB[i]->OnCreate( sizeof(RING_OUT_DB) , 100 );

		m_pTaskDB[ i ]._pDB = CADODatabase::NewObject(); 
		if( FALSE == m_pTaskDB[ i ]._pDB->Connect(m_pClanDBConfig->GetDataSource(), m_pClanDBConfig->GetCatalog(), m_pClanDBConfig->GetAccount(), m_pClanDBConfig->GetPassword()) )
		{
			return FALSE;
		}
				
		DWORD dwThreadID;
		m_pi32ThreadIdx[i] = i;
		m_phThread[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , &m_pi32ThreadIdx[i] ,0 , &dwThreadID);
		if( NULL == m_phThread[i] )
		{
			return FALSE; 
		}

		//Set Thread Priority
		if( FALSE == SetThreadPriority( m_phThread[i], THREAD_PRIORITY_NORMAL) )
		{
			return FALSE;
		}		
	}



	return TRUE; 
}

void	CDataBase::OnDestroy(void)
{	
	for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )	m_pbRunning[ i ] = FALSE;

	if( m_phThread )
	{
		::WaitForMultipleObjects( DB_THREAD_COUNT, m_phThread, TRUE, INFINITE );
	
		for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
		{
			::CloseHandle( m_phThread[ i ]);
		}
		I3MEM_SAFE_FREE( m_phThread );
	}
	I3MEM_SAFE_FREE( m_pi32ThreadIdx );
	I3MEM_SAFE_FREE( m_pi32LogDataBaseIdx );
	I3MEM_SAFE_FREE( m_pbRunning );
	if( m_pTaskDB )
	{
		for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
		{
			I3_SAFE_RELEASE( m_pTaskDB[ i ]._pDB );
		}
		I3MEM_SAFE_FREE( m_pTaskDB );
	}
	if( m_ppRingOutDB )
	{
		for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
		{
			I3_SAFE_DELETE( m_ppRingOutDB[i] );
		}
		I3MEM_SAFE_FREE( m_ppRingOutDB );
	}
	if( m_ppRingInDB )
	{
		for( INT32 i = 0 ; i < DB_THREAD_COUNT ; i++ )
		{
			I3_SAFE_DELETE( m_ppRingInDB[i] );
		}
		I3MEM_SAFE_FREE( m_ppRingInDB );
	}

	return; 
}

INT32	CDataBase::InsertWorking( UINT32 ui32CID, void* pData )
{
	INT32 i32TheadIdx = ui32CID % DB_THREAD_COUNT;

	return m_ppRingInDB[ i32TheadIdx ]->Push( pData );
}

INT32	CDataBase::InsertWorkingUID( T_UID i64UID, void* pData )
{
	INT32 i32TheadIdx = (INT32)(i64UID % DB_THREAD_COUNT);

	return m_ppRingInDB[ i32TheadIdx ]->Push( pData );
}

void	CDataBase::OnWorking( INT32 i32TheadIdx )
{
	INT32	i32Count;
	INT32	i;

	wchar_t wstrDBName[DB_STRING_LENGTH];
	i3String::Format(wstrDBName, DB_STRING_LENGTH, L"DB%02d", i32TheadIdx);
	m_pi32LogDataBaseIdx[i32TheadIdx] = g_pLog->AddLogThread(wstrDBName, 0, GetCurrentThreadId());

	m_pbRunning[ i32TheadIdx ] = TRUE;

	while( m_pbRunning[ i32TheadIdx ] )
	{
		i32Count = m_ppRingInDB[ i32TheadIdx ]->GetBufferCount();
		for( i = 0; i < i32Count ; i++ )
		{
			if( FALSE == OnWorkingClan( i32TheadIdx, m_ppRingInDB[ i32TheadIdx ]->Pop() ) ) break;
				
			m_ppRingInDB[ i32TheadIdx ]->PopIdx();
		}

		if( 0 < i32Count ) continue;
			
		::WaitForSingleObject( m_phThread[ i32TheadIdx ], 1 );
	}
}

BOOL	CDataBase::OnWorkingClan( INT32 i32TheadIdx, void* pData )
{
	RING_IN_DB*	pInDB	= (RING_IN_DB*)pData;
	RING_OUT_DB* pOutDB	= (RING_OUT_DB*)m_ppRingOutDB[i32TheadIdx]->PushPointer();
	if( NULL == pOutDB )
	{
		return FALSE;
	}
	BOOL bOut = TRUE;

	DWORD dwOldTime = GetTickCount();
	switch( pInDB->_ui8Type )
	{
	case RING_DB_NONE:
		break;
	case RING_DB_USER_LOGIN:
		_UserLogin( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_CREATE:
		_ClanCreate( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_DEATROY:
		_ClanDestory( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REQUEST_JOIN:
		ClanRequestJoin( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REQUEST_ACCEPT:
		ClanRequestAccept( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REQUEST_CANCEL:
		ClanRequestCancel( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REQUEST_DENIAL:
		ClanRequestDenial( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_MEMBER_SECESSION:
		ClanMemberSecession( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_MEMBER_DEPORTATION:
		ClanMemberDeportatio( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_INVITE:
		ClanInvite( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_INVITE_ACCEPT:
		ClanInviteAccept( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_COMMISSION_MASTER:
		ClanCommissionMaster( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_COMMISSION_STAFF:
		ClanCommissionStaff( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_COMMISSION_REGULAR:
		ClanCommissionRegular( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_GET_MEMBER_INFO:
		ClanGetMemberInfo( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_GET_REQUEST_INFO:
		ClanGetRequestInfo( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_REPLACE_NOTICE:
		ClanReplaceNotice( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REPLACE_INTRO:
		ClanReplaceIntro( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REPLACE_CLANNAME:
		ClanReplaceClanName( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REPLACE_MARK:
		ClanReplaceMark( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REPLACE_PERSONMAX:
		ClanReplacePersonMax( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_REPLACE_MANAGEMENT:
		ClanReplaceManagement( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_REPLACE_MEMBER_NICK:
		ClanReplaceMemberNick( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;
	case RING_DB_REPLACE_MEMBER_NICKCOLOR:
		ClanReplaceMemberNickColor( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;
	case RING_DB_REPLACE_MEMBER_RANK:
		ClanReplaceMemberRank( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;

	case RING_DB_CHECK_NAME:
		ClanCheckName( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_CHECK_AZIT:
		ClanCheckAzit( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_ITEM_PERIOD:
		ClanItemPeriod( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;
	case RING_DB_ITEM_RESETRECORD:
		ClanItemResetRecord( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_ITEM_RESETPOINT:
		ClanItemResetPoint( i32TheadIdx, pInDB, pOutDB );
		break;
	case RING_DB_ITEM_DELETE:
		ClanItemDelete( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;

	case RING_DB_SET_ACTIVE_STATE:
		ClanSetActiveState( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;

	case RING_DB_BATTLE_RESULT_CLAN:
		ClanBattleResult( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;
	case RING_DB_BATTLE_RESULT_USER:
		ClanBattleResultUser( i32TheadIdx, pInDB );
		bOut = FALSE;
		break;

	case RING_DB_MEMBER_ORDER:
		ClanMemberOrder( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_CLAN_FIND_USER_INFO:
		ClanFindUserInfo( i32TheadIdx, pInDB, pOutDB );
		break;

	case RING_DB_UPDATE_CLAN_MATCH_RECORD:
		UpdateClanMatchRecord(i32TheadIdx, pInDB, pOutDB);
		//bOut = FALSE;
		break;

	case RING_DB_UPDATE_MERCENARY_MATCH_RECORD:
		UpdateMercenaryMatchRecord(i32TheadIdx, pInDB);
		bOut = FALSE;
		break;

	case RING_DB_UPDATE_MERCENARY_DISCONNECT:
		UpdateMercenaryDisconnect(i32TheadIdx, pInDB);
		bOut = FALSE;
		break;

	case RING_DB_LOAD_MERCENARY:
		LoadMercenary(i32TheadIdx, pInDB, pOutDB);
		break;

	case RING_DB_LOAD_MERCENARY_PENALTY_TIME:
		LoadMercenaryPenaltyTime(i32TheadIdx, pInDB, pOutDB);
		break;
	}

	DWORD dwNewTime = GetTickCount() - dwOldTime;
	if( dwNewTime > DEF_DB_LIMIT_TIME )
	{	// DB TIMEOVER
		g_pLog->WriteLog( L"[OnWorkingClan()] DB Time Over : %d,  pInDB->_ui8Type  : %d ", dwNewTime, pInDB->_ui8Type );
	}

	if( bOut )
	{	// Return 값이 있는 것은 PointIdx 를 올려줍니다.
		m_ppRingOutDB[i32TheadIdx]->PushPointerIdx();
	}

	return TRUE;
}

INT32 CDataBase::LoadSeasonRanking(INT32 i32SeasonIdx, DB_SORT_TYPE  eSortType, CLAN_BASIC_INFO* pClanInfo )
{
	INT32 i32TheadIdx = 0;
	wchar_t* wstrSQL	= m_pTaskDB[i32TheadIdx]._pQuery;
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_AllSeasonRanking",  
		DB_DATA_TYPE_INT, i32SeasonIdx,
		DB_DATA_TYPE_INT, eSortType,
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB(i32TheadIdx, 1);
		return 0;
	}

	INT32 i32ClanCount = min(pDB->GetRecordCount(), MAX_SEASON_RANKING_TEAM_COUNT);
	for( INT32 i = 0; i < i32ClanCount; ++i )
	{
		
		INT32 i32Column = 0;
		pClanInfo[i]._idx				= pDB->GetUINT32Data(i32Column++);				// no
		pClanInfo[i]._date				= pDB->GetUINT32Data(i32Column++);				// builddate
		pClanInfo[i]._mark				= pDB->GetUINT32Data(i32Column++);				// mark 
		pClanInfo[i]._person			= pDB->GetUINT8Data(i32Column++);				// curper
		pClanInfo[i]._maxperson			= pDB->GetUINT8Data(i32Column++);				// maxper 
		pDB->GetTextData(i32Column++,	pClanInfo[i]._name, NET_CLAN_NAME_SIZE);		// name

		pDB->MoveNext();
	}

	return i32ClanCount;
}

INT32 CDataBase::LoadClanDetailData(CLAN_DETAIL_INFO_SERVER* pDetail, i3BinListEx* pList)
{
	INT32 i32TheadIdx = 0;
	wchar_t* wstrSQL	= m_pTaskDB[i32TheadIdx]._pQuery;
	S2DB::MakeQueryW(wstrSQL, L"usp_Basic_sel_Data", 
		DB_DATA_TYPE_INT, g_pConfig->GetSeasonIndex(SEASON_TYPE_PRE),
		DB_DATA_TYPE_INT, g_pConfig->GetSeasonIndex(SEASON_TYPE_ONGOING),
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB(i32TheadIdx, 1);
		return -1;
	}

	INT32 i32ClanCount = min(pDB->GetRecordCount(), static_cast<INT32>(g_pConfig->m_ui32ClanMax));
	for( INT32 i = 0; i < i32ClanCount; ++i )
	{
		CLAN_DETAIL_INFO_SERVER* pClanDetail = &pDetail[i];

		INT32 i32Column = 0;
		pClanDetail->_ClanDetailInfo.m_ui32CID			= pDB->GetUINT32Data(i32Column++);					// no

		pClanDetail->_ClanDetailInfo.m_date				= pDB->GetUINT32Data(i32Column++);					// builddate
		pClanDetail->_ClanDetailInfo.m_mark				= pDB->GetUINT32Data(i32Column++);					// mark
		pClanDetail->_ClanDetailInfo.m_exp				= pDB->GetUINT32Data(i32Column++);					// exp
		pClanDetail->_ClanDetailInfo.m_currentPerson	= pDB->GetUINT8Data(i32Column++);					// curper
		pClanDetail->_ClanDetailInfo.m_maxPerson		= pDB->GetUINT8Data(i32Column++);					// maxper
		pClanDetail->_ClanDetailInfo.m_ranklock			= pDB->GetUINT8Data(i32Column++);					// ranklock
		pClanDetail->_ClanDetailInfo.m_lagelock			= pDB->GetUINT8Data(i32Column++);					// ranklock
		pClanDetail->_ClanDetailInfo.m_hagelock			= pDB->GetUINT8Data(i32Column++);					// ranklock
		pClanDetail->_ClanDetailInfo.m_i64masterUID		= pDB->GetINT64Data(i32Column++);					// muid
		pClanDetail->_ClanDetailInfo.m_masterRank		= pDB->GetUINT8Data(i32Column++);					// mrank
		pClanDetail->_ClanDetailInfo.m_masterNickColor	= pDB->GetUINT8Data(i32Column++);					// mnickcolor
		pClanDetail->_ClanDetailInfo.m_authstaff		= pDB->GetUINT8Data(i32Column++);					// authstaff
		pClanDetail->_statedate							= pDB->GetUINT32Data(i32Column++);					// statedate

		pDB->GetTextData(i32Column++,	pClanDetail->_ClanDetailInfo.m_name, NET_CLAN_NAME_SIZE);			// name
		pDB->GetTextData(i32Column++,	pClanDetail->_ClanDetailInfo.m_masterNick, NET_NICK_NAME_SIZE);		// mnick
		pDB->GetTextData(i32Column++,	pClanDetail->_ClanDetailInfo.m_intro, NET_CLAN_INTRO_SIZE);			// intro
		pDB->GetTextData(i32Column++,	pClanDetail->_ClanDetailInfo.m_announce, NET_CLAN_INTRO_SIZE);		// announce
		pDB->GetTextData(i32Column++,	pClanDetail->_ClanDetailInfo.m_azitURL, NET_CLAN_AZIT_URL_SIZE);	// azitURL

		pClanDetail->_ClanDetailInfo.m_match			= pDB->GetUINT32Data(i32Column++);					// 초기화 가능 Match
		pClanDetail->_ClanDetailInfo.m_win				= pDB->GetUINT32Data(i32Column++);					// 초기화 가능 Win
		pClanDetail->_ClanDetailInfo.m_lose				= pDB->GetUINT32Data(i32Column++);					// 초기화 가능 Lose

		pClanDetail->_ClanDetailInfo.m_Totalmatch		= pDB->GetUINT32Data(i32Column++);					// 초기화 불가능 Match
		pClanDetail->_ClanDetailInfo.m_Totalwin			= pDB->GetUINT32Data(i32Column++);					// 초기화 불가능 Win
		pClanDetail->_ClanDetailInfo.m_Totallose		= pDB->GetUINT32Data(i32Column++);					// 초기화 불가능 Lose
		
		for( INT32 k = 0; k < SEASON_TYPE_COUNT; ++k )
		{
			SEASON_TYPE eSeasonType = static_cast<SEASON_TYPE>(k);
			INT32 i32Match	 = 0;
			INT32 i32Win	 = 0;
			INT32 i32Lose	 = 0;
			double RatingMean		=	CLAN_BASIC_POINT;
			double RatingDeviation  =	DEFAULT_DEVIATION;
			if( g_pConfig->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )
			{
				i32Match			= pDB->GetINT32Data(i32Column++);					// Season Match
				i32Win				= pDB->GetINT32Data(i32Column++);					// Season Win
				i32Lose				= pDB->GetINT32Data(i32Column++);					// Season Lose

				RatingMean			= (double)pDB->GetFloatData(i32Column++);
				RatingDeviation		= (double)pDB->GetFloatData(i32Column++);	
			}
			CRating SeasonRating(RatingMean, RatingDeviation);

			pClanDetail->_ClanDetailInfo.SetClanRecord(eSeasonType, i32Match, i32Win, i32Lose, SeasonRating );
		}

		pClanDetail->_bClanOrder = TRUE;

		pList->Add( &(pDetail[i]) );

		pDB->MoveNext();
	}

	return i32ClanCount;
}

INT32	CDataBase::LoadClanItem()
{
	INT32 i32TheadIdx = 0, i32TheadIdx2 = 1;

	wchar_t* wstrSQL		= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB		= m_pTaskDB[ i32TheadIdx ]._pDB;

	wchar_t* wstrSQLUser	= m_pTaskDB[ i32TheadIdx2 ]._pQuery;
	CADODatabase* pDBUser	= m_pTaskDB[ i32TheadIdx2 ]._pDB;

	S2DB::MakeQueryW( wstrSQL, L"usp_Item_scan",
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 2 );
		return NET_ERROR_DB_QUERY;
	}
	if( 0 == pDB->GetRecordCount() )	return EVENT_ERROR_SUCCESS;

	for( INT32 i = 0 ; i < pDB->GetRecordCount() ; i++ )
	{
		INT32 i32Idx		= pDB->GetINT32Data( 0 );
		UINT32 ui32CID		= pDB->GetUINT32Data( 1 );
		UINT32 ui32ItemID	= pDB->GetUINT32Data( 2 );
		UINT32 ui32Date		= pDB->GetUINT32Data( 3 );

		// 클랜을 검색한다.
		CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2( ui32CID );
		if( NULL == pstClan )
		{
			pDB->MoveNext();
			continue;
		}

		switch( ui32ItemID )
		{
			// 클랜 경험치 150
			case CASHITEM_ITEMID_CLAN_XP_PLUS_150:
				{
					pstClan->_ItemExpUpDate = ui32Date;
					
					if( pstClan->_ItemExpUpDate < i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
					{
						pstClan->_ItemExpUpDate = 0;

						S2DB::MakeQueryW( wstrSQLUser, L"usp_Item_del_ExpireItem",
							DB_DATA_TYPE_INT,			i32Idx,
							DB_DATA_TYPE_END);
						if( FALSE == pDBUser->ExecuteSP( wstrSQLUser ) )
						{
							ErrorDB( i32TheadIdx2, 3 );
						}
						else
						{
							if( 1 != pDBUser->GetRecordCount() )
							{
								ErrorDB( i32TheadIdx2, 2 );
							}
							if( 1 != pDBUser->GetINT32Data(0))
							{
								ErrorDB( i32TheadIdx2, 3 );
							}
						}
					}
				}
				break;
			// 컬러 클랜명
			case CASHITEM_ITEMID_COLOR_CLAN:
				{
					pstClan->_ItemColorNameDate		= ui32Date;
					pstClan->_ClanDetailInfo.m_color	= pDB->GetUINT8Data( 4 );

					if( pstClan->_ItemColorNameDate < i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI() )
					{
						pstClan->_ItemColorNameDate = 0;

						S2DB::MakeQueryW( wstrSQLUser, L"usp_Item_del_ExpireItem",
							DB_DATA_TYPE_INT,			i32Idx,
							DB_DATA_TYPE_END);
						if( FALSE == pDBUser->ExecuteSP( wstrSQLUser ) )
						{
							ErrorDB( i32TheadIdx2, 4 );
						}
						else
						{
							if( 1 != pDBUser->GetRecordCount() )
							{
								ErrorDB( i32TheadIdx2, 4 );
							}
							if( 1 != pDBUser->GetINT32Data(0))
							{
								ErrorDB( i32TheadIdx2, 5 );
							}
						}
					}
				}
				break;
		}
		pDB->MoveNext();
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CDataBase::LoadMemberData()
{
	INT32 i32TheadIdx = 0;

	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;
	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;

	INT32	i32Count;
	T_UID	i64MaxIdx = 0;
	bool	bContinue = true;
	while (bContinue)
	{
		S2DB::MakeQueryW(wstrSQL, L"usp_Member_sel_Group",
			DB_DATA_TYPE_BIGINT, i64MaxIdx,
			DB_DATA_TYPE_END);
		if (!pDB->ExecuteSP(wstrSQL))
		{
			ErrorDB(i32TheadIdx, 5);
			return NET_ERROR_DB_QUERY;
		}

		i32Count = pDB->GetRecordCount();
		if (0 == i32Count) bContinue = false;

		UINT32 ui32CID;
		for (INT32 i = 0; i < i32Count; i++)
		{
			CLAN_MEMBER_DETAIL_INFO* pstMember = g_pTaskProcessor->PopTempMember();//new CLAN_MEMBER_DETAIL_INFO;
			if (NULL == pstMember)
			{
				g_pLog->WriteLog(L"[ERROR] Load Clan Mem Memory Full MaxIdx : %I64d ", i64MaxIdx );
				return EVENT_ERROR_CLAN_MEMORY_FULL;
			}

			pstMember->_i64UID = pDB->GetINT64Data(0);
			i64MaxIdx = pstMember->_i64UID;
			ui32CID = pDB->GetUINT32Data(1);
			pstMember->_level = pDB->GetUINT8Data(2);
			pstMember->_date = pDB->GetINT32Data(3);
			pDB->GetTextData(4, pstMember->_nick, NET_NICK_NAME_SIZE);
			pstMember->_rank = pDB->GetUINT8Data(5);
	
			pstMember->_color = pDB->GetUINT8Data(6);
			pstMember->SetRating(pDB->GetDoubleData(7), pDB->GetDoubleData(8));
			pstMember->m_win = pDB->GetINT32Data(9);
			pstMember->m_lose = pDB->GetINT32Data(10);
			pstMember->m_kill = pDB->GetINT32Data(11);
			pstMember->m_death = pDB->GetINT32Data(12);
			pstMember->m_Assist = pDB->GetINT32Data(13);

			CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2(ui32CID);
			if (pstClan)
			{
				if (0 > pstClan->_pMember->Add(pstMember))
				{
					g_pLog->WriteLog(L"[ERROR] Load Clan Mem Add CID:%d, UID:%I64d", ui32CID, pstMember->_i64UID);
					I3_SAFE_DELETE(pstMember);
					return EVENT_ERROR_CLAN_MEMORY_FULL;
				}

				pstClan->_stMemberContext._totalCount++;

				if (MAX_MEMBER_COUNT < pstClan->_pMember->GetCount())
				{
					g_pLog->WriteLog(L"[ERROR] Load Clan Mem Count CID:%d, Count:%d", ui32CID, pstClan->_pMember->GetCount());
				}
			}
			else
			{
				g_pLog->WriteLog(L"[ERROR] Load Clan Mem Wrong Clan UID:%I64d, CID:%d", pstMember->_i64UID, ui32CID); 
				I3_SAFE_DELETE(pstMember);
			}

			if (FALSE == pDB->MoveNext())
			{
				g_pLog->WriteLog(L"[ERROR] Load Clan Mem Move next UID:%I64d, MaxIdx : %I64d,  CID:%d ", pstMember->_i64UID, i64MaxIdx, ui32CID);
				I3_SAFE_DELETE(pstMember);
				return EVENT_ERROR_FAIL;
			}
		}
	}
	return EVENT_ERROR_SUCCESS;
}

INT32 CDataBase::LoadRequestData()
{
	INT32 i32TheadIdx = 0;

	wchar_t* wstrSQL		= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB		= m_pTaskDB[ i32TheadIdx ]._pDB;

	S2DB::MakeQueryW( wstrSQL, L"usp_Request_scan",
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 6 );
		return NET_ERROR_DB_QUERY;
	}

	UINT32 ui32CID;	
	INT32 i32Count = pDB->GetRecordCount();
	for( INT32 i = 0 ; i <  i32Count ; i++ )
	{
		CLAN_REQUEST_SERVER_INFO* pstRequest = g_pTaskProcessor->PopTempRequest();//new CLAN_REQUEST_SERVER_INFO;
		if( NULL == pstRequest ) return EVENT_ERROR_CLAN_MEMORY_FULL;

		pstRequest->_i64UID	= pDB->GetINT64Data(0);
		ui32CID				= pDB->GetUINT32Data(1);
		pstRequest->_date	= pDB->GetUINT32Data(2);
		pDB->GetTextData( 3, pstRequest->_nick, NET_NICK_NAME_SIZE );
		pstRequest->_rank	= pDB->GetUINT8Data(4);
		pstRequest->_ui8NickColor	= pDB->GetUINT8Data(5);

		pstRequest->_ui32Kill	= pDB->GetUINT32Data(6);
		pstRequest->_ui32Death	= pDB->GetUINT32Data(7);
		pstRequest->_ui32Match	= pDB->GetUINT32Data(8);
		pstRequest->_ui32Win	= pDB->GetUINT32Data(9);
		pstRequest->_ui32Lose	= pDB->GetUINT32Data(10);
		pDB->GetTextData( 11, pstRequest->_strContents, NET_CLAN_REQUEST_SIZE );
		pstRequest->_ui32Assist = pDB->GetUINT32Data(12);

		CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2( ui32CID );
		if( pstClan )
		{
			if( 0 > pstClan->_pRequest->Add( pstRequest ) ) 
			{
				I3_SAFE_DELETE(pstRequest);
				return EVENT_ERROR_CLAN_MEMORY_FULL;
			}

			pstClan->_stRequestContext._totalCount++;
		}
		else
		{
			g_pLog->WriteLog( L"[ERROR] Load Clan Req UID:%I64d, CID:%d", pstRequest->_i64UID, ui32CID );
			I3_SAFE_DELETE(pstRequest);
		}

		pDB->MoveNext();
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CDataBase::LoadClanWarResultData()
{
	// 로드 속도 개선되어 굳이 핦 필요 없을듯
	/*if (g_pModuleControl->IsActive())
	{
		if (!g_pConfig->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW)) return EVENT_ERROR_SUCCESS;
	}*/

	INT32	i32TheadIdx = 0;
	INT32	i32Count = 0;

	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;
	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;

	// 1. 클랜 매치 결과 삭제. 최근 15개 초과분.
	S2DB::MakeQueryW(wstrSQL, L"usp_ClanWarResult_del_All",
		DB_DATA_TYPE_INT, MAX_MATCH_RESULT_COUNT,
		DB_DATA_TYPE_END);

	if (FALSE == pDB->ExecuteSP(wstrSQL))
	{
		ErrorDB(i32TheadIdx);
		return NET_ERROR_DB_QUERY;
	}
	g_pLog->WriteLog(L"[SUCC] Delete ClanWarResult Complete");


	// 2. 클랜 매치 결과 조회. 최근 15개.
	S2DB::MakeQueryW(wstrSQL, L"usp_ClanWarResult_scan", 
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP(wstrSQL))
	{
		ErrorDB(i32TheadIdx);
		return NET_ERROR_DB_QUERY;
	}

	i32Count = pDB->GetRecordCount();
	UINT32 ui32ClanNo = 0;
	CLAN_WAR_RESULT	ClanWarResult;
	for (INT32 i = 0; i < i32Count; ++i )
	{
		ClanWarResult.m_i32MatchIdx = pDB->GetINT32Data(0);
		ui32ClanNo = pDB->GetINT32Data(1);
		pDB->GetTextData(2, ClanWarResult.m_strClanName, NET_CLAN_NAME_SIZE);
		ClanWarResult.m_ui32StageID = pDB->GetUINT32Data(3);
		ClanWarResult.m_Result = pDB->GetUINT8Data(4);
		ClanWarResult.m_MyClanScore = pDB->GetUINT8Data(5);
		ClanWarResult.m_OtherClanScore = pDB->GetUINT8Data(6);

		CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2(ui32ClanNo);
		if (!pstClan)
		{
			pDB->MoveNext();
			g_pLog->WriteLog(L"[ERROR] Load ClanWarResult Wrong Clan CID:%d, MatchIdx:%d, Loop:%d, Total:%d", ui32ClanNo, ClanWarResult.m_i32MatchIdx, i, i32Count);
			continue;
		}

		pstClan->InsertClanWarResult(&ClanWarResult);
		
		pDB->MoveNext();
	}
	g_pLog->WriteLog(L"[SUCC] Load ClanWarResult Complete");
	


	// 3. 클랜 매치 플레이어 조회. 최근 1개.
	S2DB::MakeQueryW(wstrSQL, L"usp_ClanWarPlayer_scan",
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP(wstrSQL))
	{
		ErrorDB(i32TheadIdx);
		return NET_ERROR_DB_QUERY;
	}

	i32Count = pDB->GetRecordCount();
	ui32ClanNo = 0;
	T_UID	aClanWarPlayerUID[CLAN_TEAM_MEMBER_COUNT];
	for (INT32 i = 0; i < i32Count; ++i)
	{
		ui32ClanNo = pDB->GetINT32Data(0);
		for (INT32 i32Player = 0; i32Player < CLAN_TEAM_MEMBER_COUNT; ++i32Player)
		{
			aClanWarPlayerUID[i32Player] = pDB->GetINT64Data(i32Player+1);
		}

		CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2(ui32ClanNo);
		if (!pstClan)
		{
			pDB->MoveNext();
			g_pLog->WriteLog(L"[ERROR] Load ClanWarPlayer Wrong Clan CID:%d, Loop:%d, Total:%d", ui32ClanNo, i, i32Count);
			continue;
		}

		i3mem::Copy(pstClan->m_aClanWarPlayerUID, aClanWarPlayerUID, sizeof(T_UID) * CLAN_TEAM_MEMBER_COUNT);

		pDB->MoveNext();
	}
	g_pLog->WriteLog(L"[SUCC] Load ClanWarPlayer Complete");

	return EVENT_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB Write
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CDataBase::_UserLogin( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;

	// TClanMem 확인(클랜 가입자일 경우에 실행됨)
	// TClanReq 확인(클랜 가입 신청자인 경우에만 처리됨)
	S2DB::MakeQueryW( wstrSQL, L"usp_Member_sel_CheckLogin",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 7 );
		return FALSE;
	}

	if( pDB->GetRecordCount() < 1 )
		return FALSE;

	INT32 i32Result = pDB->GetINT32Data(0);
	switch( i32Result )
	{
	case 1:
	case 2:
		{
			UINT32	ui32CID		= pDB->GetUINT32Data(1);
			UINT8	ui8Level	= pDB->GetUINT8Data(2);

			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;

			pOutDB->Write( &ui32CID,	sizeof(UINT32) );
			pOutDB->Write( &ui8Level,	sizeof(UINT8) );
		}
		break;

	case 3:
		pOutDB->_i32Result = EVENT_ERROR_CLAN_NOMEMBER;
		break;
	}

	return TRUE;
}

BOOL	CDataBase::_ClanCreate( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT16	ui16ClanIdx;
	CLAN_CS_CREATE_INFO stCreateInfo;

	pInDB->Reset();
	pInDB->Read( &ui16ClanIdx,	sizeof(UINT16) );
	pInDB->Read( &stCreateInfo,	sizeof(CLAN_CS_CREATE_INFO) );

	pOutDB->Write( &ui16ClanIdx,	sizeof(UINT16) );

	//////////////////////////////////////////////////////////////////////
	UINT32 ui32Date = i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD();
	
	if(FALSE == g_pConfig->m_bAzitActive) { i3String::Copy( stCreateInfo.szAzitURL, _TT("Temp"), NET_CLAN_AZIT_URL_SIZE ); }

	// TClanMem 가입유무확인,TClanReq 신청유무확인,클랜명중복확인,아지트URL중복확인
	// TClan 에 등록
	// 마지막으로 추가된 Clan no 를 얻어옵니다.
	// TClanMem에 등록
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_ins_ClanCreate",
		DB_DATA_TYPE_INT,		g_pConfig->m_bAzitActive,
		DB_DATA_TYPE_BIGINT,	pInDB->_i64UID,
		DB_DATA_TYPE_INT,		ui32Date,
		DB_DATA_TYPE_INT,		stCreateInfo.ui32Rank,
		DB_DATA_TYPE_INT,		stCreateInfo.wMark,
		DB_DATA_TYPE_TTCHAR,	stCreateInfo.strNick,
		DB_DATA_TYPE_TTCHAR,	stCreateInfo.szClanName,
		DB_DATA_TYPE_TTCHAR,		stCreateInfo.szAzitURL,
		DB_DATA_TYPE_TTCHAR,		stCreateInfo.szClanIntro,
		DB_DATA_TYPE_TINYINT,	stCreateInfo.m_ui8MemberCount,
		DB_DATA_TYPE_TINYINT,	stCreateInfo.m_ui8NickColor,
		DB_DATA_TYPE_END );
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 9 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 6 );
		return FALSE;
	}

	INT32 ECode	= pDB->GetINT32Data( 0 );
	UINT32 ui32CID	= 0;
	switch(ECode)
	{
	case 1:
		{
			// Success
			ui32CID	= pDB->GetUINT32Data( 1 );
		}
		break;
	case -1:
	case -2:
	case -3:
	case -4:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			//ReConnectDataLog( i32TheadIdx, 8 );
			return FALSE;
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 7 );
			return FALSE;
		}
		break;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	
	pOutDB->Write( &ui32CID,		sizeof(UINT32) );
	pOutDB->Write( &stCreateInfo,	sizeof(CLAN_CS_CREATE_INFO) );
	pOutDB->Write( &ui32Date,		sizeof(UINT32) );

	return TRUE;
}

BOOL	CDataBase::_ClanDestory( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	// 클랜이 있는지
	// 클랜장이 맞는지.(필요한가?)
	// 클랜원이 남아있는지.
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_del_ClanDestroy",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 19 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 9 );
		return FALSE;
	}

	INT32 i32Result = pDB->GetINT32Data(0);
	switch( i32Result )
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
		}
		break;
	case -1:	// 못찾을 경우
	case -2:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_CLANMEMBERIDX;
			return FALSE;
		}
		break;
	case -3:	// 클랜원이 남아있을 경우
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			return FALSE;
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 10 );
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL	CDataBase::ClanRequestJoin( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	CLAN_CS_JOIN_INFO stClanJoinInfo;
	TTCHAR strTemp[CONVERT_STRING_COUNT];
	TTCHAR strNick[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( &stClanJoinInfo,	sizeof(CLAN_CS_JOIN_INFO) );

	S2DB::ConvertString( strTemp, NET_CLAN_REQUEST_SIZE, stClanJoinInfo.szContents );
	S2DB::ConvertString( strNick, NET_NICK_NAME_SIZE, stClanJoinInfo.strNick );

	UINT32 ui32Date = i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD();

	//S2DB::MakeQueryW( wstrSQL, L"usp_Request_ins", 
	//	DB_DATA_TYPE_BIGINT,	pInDB->_i64UID,
	//	DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
	//	DB_DATA_TYPE_INT,		ui32Date,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo.wViewRank,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo._ui32Killcount,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo._ui32Death,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo._ui32Match,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo._ui32Win,
	//	DB_DATA_TYPE_INT,		stClanJoinInfo._ui32Lose,
	//	DB_DATA_TYPE_TTCHAR,	strNick,
	//	DB_DATA_TYPE_TTCHAR,	strTemp,
	//	DB_DATA_TYPE_TINYINT,	stClanJoinInfo.ui8NickColor,
	//	DB_DATA_TYPE_END	);

	S2DB::MakeQueryW(wstrSQL, L"usp_Request_ins_Assist",
		DB_DATA_TYPE_BIGINT, pInDB->_i64UID,
		DB_DATA_TYPE_INT, pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT, ui32Date,
		DB_DATA_TYPE_INT, stClanJoinInfo.wViewRank,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32Killcount,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32Death,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32Match,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32Win,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32Lose,
		DB_DATA_TYPE_TTCHAR, strNick,
		DB_DATA_TYPE_TTCHAR, strTemp,
		DB_DATA_TYPE_TINYINT, stClanJoinInfo.ui8NickColor,
		DB_DATA_TYPE_INT, stClanJoinInfo._ui32AssistCount,
		DB_DATA_TYPE_END);

	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 27 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 14 );
		return FALSE;
	}

	INT32 i32Result = pDB->GetINT32Data(0);
	switch( i32Result )
	{
	case 1:
		{
			// 성공
		}
		break;
	case -1:
	case -2:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			return FALSE;
		}
		break;	
	case -3:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_MAXREQUEST;
			return FALSE;
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 16 );
			return FALSE;
		}
		break;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	
	pOutDB->Write( &ui32Date,		sizeof(UINT32) );
	pOutDB->Write( &stClanJoinInfo, sizeof(CLAN_CS_JOIN_INFO) );

	return TRUE;
}

BOOL	CDataBase::ClanRequestAccept( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT8 uiCount;
	T_UID ai64UID[ MAX_REQUEST_COUNT ];

	pInDB->Reset();
	pInDB->Read( &uiCount, sizeof(UINT8) );
	pInDB->Read( ai64UID, sizeof(T_UID)*uiCount );

	pOutDB->Write( &uiCount, sizeof(UINT8) );
	pOutDB->Write( ai64UID, sizeof(T_UID)*uiCount );

	UINT32 ui32Date = i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD();

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_PersonNumber",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 34 );
		return FALSE;
	}
	INT32 i32CurPer = (INT32)pDB->GetUINT8Data(0);
	INT32 i32MaxPer = (INT32)pDB->GetUINT8Data(1);
	if( (i32CurPer + uiCount) > i32MaxPer )
	{
		pOutDB->_i32Result = EVENT_ERROR_CLAN_MAXMEMBER;
		ErrorDB( i32TheadIdx, 17 );
		return FALSE;
	}

	for( INT32 i = 0 ; i < uiCount ; i++ )
	{
		S2DB::MakeQueryW( wstrSQL, L"usp_Request_sel_AcceptCheck",
			DB_DATA_TYPE_BIGINT,		ai64UID[i],
			DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
			DB_DATA_TYPE_END);
		if( FALSE == pDB->ExecuteSP( wstrSQL ) )
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 36 );
			return FALSE;
		}
		if( 1 != pDB->GetRecordCount() ) 
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 18 );
			return TRUE;
		}

		TTCHAR strNick[ NET_NICK_NAME_SIZE ];
		UINT32 ui32Rank;
		UINT8  ui8NickColor;

		INT32 i32ECode = pDB->GetINT32Data(0);
		pDB->GetTextData( 1, strNick, NET_NICK_NAME_SIZE );
		ui32Rank		= pDB->GetUINT32Data(2);
		ui8NickColor	= pDB->GetUINT8Data(3);

		switch(i32ECode)
		{
			case 1:
				{
					pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
				}
				break;
			case 2:
				{
					pOutDB->_i32Result = EVENT_ERROR_CLAN_MEMBER;
					ErrorDB( i32TheadIdx, 19 );
					return FALSE;
				}
				break;
			case 3:
				{
					pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_REQUESTIDX;
					ErrorDB( i32TheadIdx, 20 );
					return FALSE;
				}
				break;
			case 4:
				{
					pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_REQUESTIDX;
					ErrorDB( i32TheadIdx, 21 );
					return FALSE;
				}
				break;
			default:
				{
					pOutDB->_i32Result = NET_ERROR_DB_QUERY;
					ErrorDB( i32TheadIdx, 22 );
					return FALSE;
				}
				break;
		}	

		TTCHAR strTemp[CONVERT_STRING_COUNT];
		S2DB::ConvertString(strTemp, NET_NICK_NAME_SIZE, strNick);

		// Member Insert
		// Request Delete
		S2DB::MakeQueryW( wstrSQL, L"usp_Member_ins", 
			DB_DATA_TYPE_BIGINT,	ai64UID[i], 
			DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx, 
			DB_DATA_TYPE_INT,		ui32Date, 
			DB_DATA_TYPE_INT,		ui32Rank, 
			DB_DATA_TYPE_TTCHAR,	strTemp,
			DB_DATA_TYPE_TINYINT,	ui8NickColor,
			DB_DATA_TYPE_END	);
		if( FALSE == pDB->ExecuteSP( wstrSQL ) )
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 38 );
			return TRUE;
		}
		if( 0 == pDB->GetRecordCount() ) 
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 23 );
			return FALSE;
		}
		if(1 != (INT32)pDB->GetUINT32Data(0))
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 24 );
			return FALSE;
		}
	}

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_PersonNumber",
		DB_DATA_TYPE_TINYINT,		uiCount,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 41 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 25 );
		return FALSE;
	}
	if(1 != (INT32)pDB->GetUINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 26 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &ui32Date, sizeof(UINT32) );

	return TRUE;
}

BOOL	CDataBase::ClanRequestCancel( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	S2DB::MakeQueryW( wstrSQL, L"usp_Request_del_Cancel",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 44 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 27 );
		return FALSE;
	}
	
	INT32 i32ECode = pDB->GetINT32Data(0);
	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			return TRUE;
		}
		break;
	case -1:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_REQUESTIDX;
			ErrorDB( i32TheadIdx, 28 );
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 29 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanRequestDenial( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT8 ui8Count;
	T_UID ai64UID[ MAX_REQUEST_COUNT ];

	pInDB->Reset();
	pInDB->Read( &ui8Count,	sizeof(UINT8) );
	LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
	pInDB->Read( ai64UID,	sizeof(T_UID)*ui8Count );

	if(0 >= ui8Count) { return FALSE; }

	wchar_t wstrUID[2048];
	i3String::Format( wstrUID, 2048, L"%I64d", ai64UID[0]);
	for( INT32 i = 1 ; i < ui8Count ; i++ )
	{
		i3String::Format( wstrUID, 2048, L"%s,%I64d", wstrUID, ai64UID[i]);
	}
	i3String::Format( wstrUID, 2048, L"%s,", wstrUID );

	S2DB::MakeQueryW( wstrSQL, L"usp_Request_del_Denial",
		DB_DATA_TYPE_TINYINT,		ui8Count,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR,		wstrUID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 49 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 30 );
		return FALSE;
	}

	INT32 i32ECode = pDB->GetINT32Data(0);
	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &ui8Count,	sizeof(UINT8) );
			pOutDB->Write( ai64UID,		sizeof(T_UID)*ui8Count );

			return TRUE;
		}
	case -1:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_REQUESTIDX;
			ErrorDB( i32TheadIdx, 31 );
		}
		break;
	case -2:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_NOSEARCH_REQUESTIDX;
			ErrorDB( i32TheadIdx, 32 );
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 33 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanMemberSecession( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	// TClanMem 확인
	// 클랜 멤버에서 삭제

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_del_Secession",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 54 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 34 );
		return FALSE;
	}

	INT32 i32ECode = pDB->GetINT32Data(0);
	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			return TRUE;
		}
		break;
	case -1:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			ErrorDB( i32TheadIdx, 35 );
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 36 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanMemberDeportatio( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT8 ui8Count;
	T_UID ai64UID[ MAX_REQUEST_COUNT ];

	pInDB->Reset();
	pInDB->Read( &ui8Count, sizeof(UINT8) );			LENGTH_CHECK( ui8Count, MAX_REQUEST_COUNT );
	pInDB->Read( ai64UID,	sizeof(T_UID)*ui8Count );

	if(0 >= ui8Count) { return FALSE; }

	wchar_t wstrUID[2048];		
	i3String::Format( wstrUID, 2048, L"%I64d", ai64UID[0]);
	for( INT32 i = 1 ; i < ui8Count ; i++ )
	{
		i3String::Format( wstrUID, 2048, L"%s,%I64d", wstrUID, ai64UID[i]);
	}	
	i3String::Format(wstrUID, 2048, L"%s,", wstrUID);

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_del_Deportatio",
		DB_DATA_TYPE_INT,			ui8Count,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR,		wstrUID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 59 );
		return TRUE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 37 );
		return FALSE;
	}
	INT32 i32ECode = pDB->GetINT32Data(0);

	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &ui8Count,	sizeof(UINT8) );
			pOutDB->Write( ai64UID,		sizeof(T_UID)*ui8Count );

			return TRUE;
		}
	case -1:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			ErrorDB( i32TheadIdx, 38 );
		}
		break;
	case -2:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_NOMEMBER;
			ErrorDB( i32TheadIdx, 39 );
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 40 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanInvite( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	INT64 i64UID;
	pInDB->Reset();
	pInDB->Read( &i64UID, sizeof(T_UID) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_sel_ClanInvite",
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 64 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		return FALSE;
	}
	INT32 i32ECode = pDB->GetINT32Data(0);
	
	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &i64UID,	sizeof(T_UID) );

			return TRUE;
		}
		break;
	case -1:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
			pOutDB->Write( &i64UID,	sizeof(T_UID) );

			return TRUE;
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 42 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanInviteAccept( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strNick[ NET_NICK_NAME_SIZE ];
	UINT32 ui32Rank;
	UINT8  ui8NickColor;

	pInDB->Reset();
	pInDB->Read( strNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
	pInDB->Read( &ui32Rank,		sizeof(UINT32) );
	pInDB->Read( &ui8NickColor,	sizeof(UINT8) );

	UINT32 ui32Date = i3ThreadTimer::LocalTime().GetDateTimeYYYYMMDD();

	TTCHAR strTemp[CONVERT_STRING_COUNT];
	S2DB::ConvertString(strTemp, NET_NICK_NAME_SIZE, strNick);

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_ins_InviteAccept", 
		DB_DATA_TYPE_BIGINT,	pInDB->_i64UID,
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,		ui32Date,		
		DB_DATA_TYPE_INT,		ui32Rank,
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_TINYINT,	ui8NickColor,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 68 );
		return TRUE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		return FALSE;
	}
	INT32 i32ECode = pDB->GetINT32Data(0);

	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &ui32Date, sizeof(UINT32) );

			return TRUE;
		}
		break;
	case -2:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_MAXMEMBER;
			pOutDB->Write( &ui32Date, sizeof(UINT32) );

			return TRUE;
		}
		break;
	case -3:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_MEMBER;
			pOutDB->Write( &ui32Date, sizeof(UINT32) );

			return TRUE;
		}
		break;
	case -4:
		{
			pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_REQUESTIDX;
			pOutDB->Write( &ui32Date, sizeof(UINT32) );

			return TRUE;
		}
		break;
		
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 46 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanCommissionMaster( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB; 
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	T_UID i64UID;
	pInDB->Reset();
	pInDB->Read( &i64UID, sizeof(T_UID));

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_CommissionMaster",
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 75 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 47 );
		return FALSE;
	}
	//INT32 i32ECode = pDB->GetUINT8Data(0); //sp에서 리턴 되는 값 중에 -값이 있는데 UINT로 받아서 제대로 된 에러코드를 받을 수 없다.
	INT32 i32ECode = pDB->GetINT32Data(0);
	
	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &i64UID, sizeof(T_UID) );

			return TRUE;
		}
		break;
	case -2:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		}
		break;
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 48 );
		}
		break;

	}
	
	return FALSE;
}

BOOL	CDataBase::ClanCommissionStaff( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT8 ui8Count;
	T_UID ai64UID[ MAX_MEMBER_COUNT ];

	pInDB->Reset();
	pInDB->Read( &ui8Count, sizeof(UINT8) );
	pInDB->Read( ai64UID,	sizeof(T_UID)*ui8Count );

	if(0 >= ui8Count) { return FALSE; }

	wchar_t wstrUID[2048];		
	i3String::Format(wstrUID, 2048, L"%I64d", ai64UID[0]);
	for( INT32 i = 1 ; i < ui8Count ; i++ )
	{
		i3String::Format(wstrUID, 2048, L"%s,%I64d", wstrUID, ai64UID[i]);
	}	
	i3String::Format(wstrUID, 2048, L"%s,", wstrUID);
	
	// 등급을 스탭으로 임명한다.
	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_CommissionStaff",
		DB_DATA_TYPE_TINYINT,		ui8Count,
		DB_DATA_TYPE_TTCHAR,		wstrUID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 82 );
		return TRUE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 49 );
		return FALSE;
	}
	INT32 i32ECode = pDB->GetINT32Data(0);

	switch(i32ECode)
	{
	case 1:
		{
			pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
			pOutDB->Write( &ui8Count,	sizeof(UINT8) );
			pOutDB->Write( ai64UID,		sizeof(T_UID)*ui8Count );

			return TRUE;
		}
	default:
		{
			pOutDB->_i32Result = NET_ERROR_DB_QUERY;
			ErrorDB( i32TheadIdx, 50 );
		}
		break;
	}

	return FALSE;
}

BOOL	CDataBase::ClanCommissionRegular( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT8 ui8Count;
	T_UID ai64UID[ MAX_MEMBER_COUNT ];

	pInDB->Reset();
	pInDB->Read( &ui8Count, sizeof(UINT8) );	LENGTH_CHECK( ui8Count, MAX_MEMBER_COUNT );
	pInDB->Read( ai64UID,	sizeof(T_UID)*ui8Count );

	if(0 >= ui8Count) { return FALSE; }

	wchar_t wstrUID[2048];		
	i3String::Format(wstrUID, 2048, L"%I64d", ai64UID[0]);
	for( INT32 i = 1 ; i < ui8Count ; i++ )
	{
		i3String::Format(wstrUID, 2048, L"%s,%I64d", wstrUID, ai64UID[i]);
	}	
	i3String::Format(wstrUID, 2048, L"%s,", wstrUID);

	// 등급을 일반으로 임명한다.
	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_CommissionRegular",
		DB_DATA_TYPE_TINYINT,		ui8Count,
		DB_DATA_TYPE_TTCHAR,		wstrUID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 87 );
		return TRUE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 51 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 52 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &ui8Count,	sizeof(UINT8) );
	pOutDB->Write( ai64UID,		sizeof(T_UID)*ui8Count );

	return TRUE;
}

BOOL	CDataBase::ClanGetMemberInfo( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	T_UID i64UID;
	pInDB->Reset();
	pInDB->Read( &i64UID,	sizeof(T_UID) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_sel",
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 92 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
		ErrorDB( i32TheadIdx, 53 );
		return FALSE;
	}
	
	UINT32	ui32Win, ui32Lose, ui32Kill, ui32Death, ui32Headshot, ui32Connect, ui32Assist;
	
	ui32Win			= pDB->GetUINT32Data(0);
	ui32Lose		= pDB->GetUINT32Data(1);
	ui32Kill		= pDB->GetUINT32Data(2);
	ui32Death		= pDB->GetUINT32Data(3);
	ui32Headshot	= pDB->GetUINT32Data(4);
	ui32Connect		= pDB->GetUINT32Data(5);
	ui32Assist		= pDB->GetUINT32Data(6);

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	
	pOutDB->Write( &ui32Win,		sizeof(UINT32) );
	pOutDB->Write( &ui32Lose,		sizeof(UINT32) );
	pOutDB->Write( &ui32Kill,		sizeof(UINT32) );
	pOutDB->Write( &ui32Death,		sizeof(UINT32) );
	pOutDB->Write( &ui32Headshot,	sizeof(UINT32) );
	pOutDB->Write( &ui32Connect,	sizeof(UINT32) );

	return TRUE;
}

BOOL	CDataBase::ClanGetRequestInfo( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	T_UID i64UID;
	pInDB->Reset();
	pInDB->Read( &i64UID,	sizeof(T_UID) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Request_sel",
		DB_DATA_TYPE_BIGINT,		i64UID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 93 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
		ErrorDB( i32TheadIdx, 54 );
		return FALSE;
	}

	CLAN_REQUEST_DETAIL_INFO stRequest;
	stRequest._i64UID	= i64UID;
	pDB->GetTextData(0, stRequest._nick,	NET_NICK_NAME_SIZE);
	stRequest._rank		= pDB->GetUINT8Data(1);
	stRequest._ui8NickColor		= pDB->GetUINT8Data(2);
	stRequest._kill		= pDB->GetUINT32Data(3);
	stRequest._death	= pDB->GetUINT32Data(4);
	stRequest._match	= pDB->GetUINT32Data(5);
	stRequest._win		= pDB->GetUINT32Data(6);
	stRequest._lose		= pDB->GetUINT32Data(7);
	pDB->GetTextData(8, stRequest._contents, NET_CLAN_REQUEST_SIZE);
	stRequest._Assist	= pDB->GetUINT32Data(9);
	
	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &stRequest,		sizeof(CLAN_REQUEST_DETAIL_INFO) );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceNotice( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strNotice[ NET_CLAN_NOTICE_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( strNotice,	sizeof(TTCHAR) * NET_CLAN_NOTICE_SIZE );
	strNotice[ NET_CLAN_NOTICE_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_CLAN_NOTICE_SIZE, strNotice );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Notice", 
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 94 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 56 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 57 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( strNotice, sizeof(TTCHAR) * NET_CLAN_NOTICE_SIZE );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceIntro( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strIntro[ NET_CLAN_INTRO_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( strIntro,	sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE );
	strIntro[ NET_CLAN_INTRO_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_CLAN_INTRO_SIZE, strIntro );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Intro", 
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 95 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 58 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 59 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( strIntro, sizeof(TTCHAR) * NET_CLAN_INTRO_SIZE );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceClanName( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strName[ NET_CLAN_NAME_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( strName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
	strName[ NET_CLAN_NAME_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_CLAN_NAME_SIZE, strName );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_ClanName", 
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_END	);	
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 96 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 60 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 61 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( strName, NET_CLAN_NAME_SIZE );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceMark( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	UINT32 ui32Mark;

	pInDB->Reset();
	pInDB->Read( &ui32Mark,	sizeof(UINT32) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Mark",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,			ui32Mark,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 97 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 62 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 63 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &ui32Mark, sizeof(UINT32) );

	return TRUE;
}

BOOL	CDataBase::ClanReplacePersonMax( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	UINT16	ui16AddPersonCount = 0;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	pInDB->Reset();
	pInDB->Read( &ui16AddPersonCount,	sizeof(UINT16) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_MaxCapacity",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_SMALLINT,		ui16AddPersonCount,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 98 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 64 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 65 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &ui16AddPersonCount, sizeof(UINT16) );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceManagement( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	CLAN_MANAGEMENT stManagement;

	pInDB->Reset();
	pInDB->Read( &stManagement,	sizeof(CLAN_MANAGEMENT) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Management",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TINYINT,		stManagement._authority,
		DB_DATA_TYPE_TINYINT,		stManagement._limitRank,
		DB_DATA_TYPE_TINYINT,		stManagement._limitAgeLow,
		DB_DATA_TYPE_TINYINT,		stManagement._limitAgeHigh,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 99 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 66 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 67 );
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	pOutDB->Write( &stManagement, sizeof(CLAN_MANAGEMENT) );

	return TRUE;
}

BOOL	CDataBase::ClanReplaceMemberNick( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	UINT8 ui8Level;
	TTCHAR strName[ NET_NICK_NAME_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( &ui8Level,	sizeof(UINT8) );
	pInDB->Read( strName,	sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
	strName[ NET_NICK_NAME_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_NICK_NAME_SIZE, strName );

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_Nick",
		DB_DATA_TYPE_BIGINT,	pInDB->_i64UID,
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,		ui8Level,
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 100 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		ErrorDB( i32TheadIdx, 68 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 69 );
		return FALSE;
	}

	return TRUE;
}

BOOL	CDataBase::ClanReplaceMemberNickColor( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	UINT8 ui8Level;
	UINT8 ui8NickColor;
	
	pInDB->Reset();
	pInDB->Read( &ui8Level,	sizeof(UINT8) );
	pInDB->Read( &ui8NickColor,		sizeof(UINT8) );

	
	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_NickColor",
		DB_DATA_TYPE_BIGINT,	pInDB->_i64UID,
		DB_DATA_TYPE_INT,		pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,		ui8Level,
		DB_DATA_TYPE_TINYINT,	ui8NickColor,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 100 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		ErrorDB( i32TheadIdx, 68 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 69 );
		return FALSE;
	}

	return TRUE;
}

BOOL	CDataBase::ClanReplaceMemberRank( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	UINT8 ui8Level;
	UINT8 ui8Rank;

	pInDB->Reset();
	pInDB->Read( &ui8Level,	sizeof(UINT8) );
	pInDB->Read( &ui8Rank,	sizeof(UINT8) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_Rank",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TINYINT,		ui8Level,
		DB_DATA_TYPE_TINYINT,		ui8Rank,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 103 );
		return FALSE;
	}
	if( 0 == pDB->GetRecordCount() ) 
	{
		ErrorDB( i32TheadIdx, 70 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 71 );
		return FALSE;
	}

	return TRUE;
}


BOOL	CDataBase::ClanCheckName( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strName[ NET_CLAN_NAME_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( strName,	sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
	strName[ NET_CLAN_NAME_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_CLAN_NAME_SIZE, strName );
	
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_NameOverlapCheck", 
		DB_DATA_TYPE_TTCHAR,	strTemp,
		DB_DATA_TYPE_END);

	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 106 );
		return FALSE;
	}

	if( 0 == pDB->GetRecordCount() )
	{
		//ErrorDB( i32TheadIdx, 72 );
		pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	}
	else
	{
		//ErrorDB( i32TheadIdx, 73 );
		pOutDB->_i32Result = EVENT_ERROR_FAIL;
	}

	return TRUE;
}

BOOL	CDataBase::ClanCheckAzit( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	TTCHAR strAzit[ NET_CLAN_AZIT_URL_SIZE ];
	TTCHAR strTemp[CONVERT_STRING_COUNT];

	pInDB->Reset();
	pInDB->Read( strAzit,	sizeof(TTCHAR) * NET_CLAN_AZIT_URL_SIZE );
	strAzit[ NET_CLAN_AZIT_URL_SIZE-1 ] = '\0';

	S2DB::ConvertString( strTemp, NET_CLAN_AZIT_URL_SIZE, strAzit );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_AzitOverlapCheck",
		DB_DATA_TYPE_TTCHAR, strTemp,
		DB_DATA_TYPE_END	);
	if( FALSE == pDB->ExecuteSP(wstrSQL))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 107 );
		return FALSE;
	}

	if( 0 == pDB->GetRecordCount() )
	{
		//ErrorDB( i32TheadIdx, 74 );
		pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	}
	else
	{
		ErrorDB( i32TheadIdx, 75 );
		pOutDB->_i32Result = EVENT_ERROR_FAIL;
	}

	return TRUE;
}

BOOL	CDataBase::ClanItemPeriod( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	UINT32 ui32ItemID;
	UINT32 ui32Date;
	UINT32 ui32Arg;

	pInDB->Reset();
	pInDB->Read( &ui32ItemID,	sizeof(UINT32) );
	pInDB->Read( &ui32Date,		sizeof(UINT32) );
	pInDB->Read( &ui32Arg,		sizeof(UINT32) );

	// 먼저 아이템이 있는지를 확인한다.
	// 만약에 선택된게 없으면 새로 추가한다.
	// 존재하면 Update
	S2DB::MakeQueryW( wstrSQL, L"usp_Item_mer",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,			ui32ItemID,
		DB_DATA_TYPE_INT,			ui32Date,
		DB_DATA_TYPE_INT,			ui32Arg,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB( i32TheadIdx, 108 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 76 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 77 );
		return FALSE;
	}

	// 기존 변수값도 변경

	CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2( pInDB->_ui32ClanDBIdx );

	if ( NULL == pstClan ) return FALSE; 

	pstClan->_ItemExpUpDate =	ui32Date;

	return TRUE;
}

BOOL	CDataBase::ClanItemResetRecord( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_ItemRecordReset",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 111 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 78 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 79 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;

	return TRUE;
}

BOOL	CDataBase::ClanItemResetPoint( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	S2DB::MakeQueryW( wstrSQL, L"usp_SeasonRecord_upd",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,			g_pConfig->GetSeasonIndex(SEASON_TYPE_ONGOING),
		DB_DATA_TYPE_REAL,			CLAN_BASIC_POINT,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP(wstrSQL))
	{
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;

		ErrorDB( i32TheadIdx, 112 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 80 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 81 );
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;

	return TRUE;
}

BOOL	CDataBase::ClanItemDelete( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	UINT32 ui32ItemID;

	pInDB->Reset();
	pInDB->Read( &ui32ItemID,	sizeof(UINT32) );

	// 먼저 아이템이 있는지를 확인한다.
	// 만약에 선택된게 없으면 새로 추가한다.
	// 존재하면 Update
	S2DB::MakeQueryW( wstrSQL, L"usp_Item_del_SelectItem",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,			ui32ItemID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB( i32TheadIdx, 108 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 76 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 77 );
		return FALSE;
	}

	return TRUE;
}

BOOL	CDataBase::ClanSetActiveState( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	UINT32 ui32NowDate;

	pInDB->Reset();
	pInDB->Read( &ui32NowDate, sizeof(UINT32) );

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_ActiveDate",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT,			ui32NowDate,
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 113 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 82 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 83 );
		return FALSE;
	}

	return TRUE;
}

// Battle Server
BOOL	CDataBase::ClanBattleResult( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	pInDB->Reset();

	CLAN_RECORD stRecord;
	pInDB->Read(&stRecord, sizeof(CLAN_RECORD));
	
	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;

	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_BattleResult",
		DB_DATA_TYPE_INT,			stRecord.m_i32Experience,
		DB_DATA_TYPE_INT,			stRecord.m_i32MatchCount,
		DB_DATA_TYPE_INT,			stRecord.m_i32WinMatch,
		DB_DATA_TYPE_INT,			stRecord.m_i32LoseMatch,
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB( i32TheadIdx, 122 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 85 );
		return FALSE;
	}
	if(1 != pDB->GetINT32Data(0))
	{
		ErrorDB( i32TheadIdx, 86 );
		return FALSE;
	}

	return TRUE;
}

BOOL CDataBase::UpdateClanMatchRecord(INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB)
{
	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;
	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;

	pInDB->Reset();

	pOutDB->Reset();
	pOutDB->Copy(pInDB);

	pOutDB->_i32Result = EVENT_ERROR_DATABASE_SP_RESULT;

	CLAN_RECORD		stRecord;
	CLAN_WAR_RESULT	stWarResult;
	T_UID			aPlayerUID[CLAN_TEAM_MEMBER_COUNT];

	// Read
	pInDB->Read(&stRecord, sizeof(CLAN_RECORD));
	pInDB->Read(&stWarResult, sizeof(CLAN_WAR_RESULT));
	pInDB->Read(aPlayerUID, sizeof(T_UID)*CLAN_TEAM_MEMBER_COUNT);

	// 1. 시즌 전적.
	S2DB::MakeQueryW(wstrSQL, L"usp_SeasonRecord_mer",
		DB_DATA_TYPE_INT, pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_INT, g_pConfig->GetSeasonIndex(SEASON_TYPE_ONGOING),
		DB_DATA_TYPE_INT, stRecord.m_i32MatchCount,
		DB_DATA_TYPE_INT, stRecord.m_i32WinMatch,
		DB_DATA_TYPE_INT, stRecord.m_i32LoseMatch,
		DB_DATA_TYPE_REAL, stRecord.m_ClanRating.GetClanPoint(),
		DB_DATA_TYPE_REAL, stRecord.m_ClanRating.GetDeviation(),
		DB_DATA_TYPE_END);
	if (!pDB->ExecuteSP(wstrSQL))
	{
		pOutDB->_i32Result = EVENT_ERROR_DATABASE_EXECUTE_SP;
		ErrorDB(i32TheadIdx, 200);
		return FALSE;
	}

	if (1 != pDB->GetRecordCount())
	{
		ErrorDB(i32TheadIdx, 201);
		return FALSE;
	}

	if (1 != pDB->GetINT32Data(0))
	{
		ErrorDB(i32TheadIdx, 202);
		return FALSE;
	}


	// 2. 매치메이킹 결과
	S2DB::MakeQueryW(wstrSQL, L"usp_ClanWarResult_ins",
		DB_DATA_TYPE_INT, pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_TTCHAR, stWarResult.m_strClanName,
		DB_DATA_TYPE_INT, stWarResult.m_ui32StageID,
		DB_DATA_TYPE_TINYINT, stWarResult.m_Result,
		DB_DATA_TYPE_TINYINT, stWarResult.m_MyClanScore,
		DB_DATA_TYPE_TINYINT, stWarResult.m_OtherClanScore,
		DB_DATA_TYPE_END);

	if (!pDB->ExecuteSP(wstrSQL))
	{
		pOutDB->_i32Result = EVENT_ERROR_DATABASE_EXECUTE_SP;
		ErrorDB(i32TheadIdx, 203);
		return FALSE;
	}

	if (1 != pDB->GetRecordCount())
	{
		ErrorDB(i32TheadIdx, 204);
		return FALSE;
	}

	if (1 != pDB->GetINT32Data(0))
	{
		ErrorDB(i32TheadIdx, 205);
		return FALSE;
	}

	// DB IDX를 저장합니다.
	stWarResult.m_i32MatchIdx = pDB->GetINT32Data(1);

	// 3. 매치메이킹 플레이어 UID. ( 용병 제외 )
	S2DB::MakeQueryW(wstrSQL, L"usp_ClanWarPlayer_mer",
		DB_DATA_TYPE_INT, pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_BIGINT, aPlayerUID[0],
		DB_DATA_TYPE_BIGINT, aPlayerUID[1],
		DB_DATA_TYPE_BIGINT, aPlayerUID[2],
		DB_DATA_TYPE_BIGINT, aPlayerUID[3],
		DB_DATA_TYPE_BIGINT, aPlayerUID[4],
		DB_DATA_TYPE_BIGINT, aPlayerUID[5],
		DB_DATA_TYPE_BIGINT, aPlayerUID[6],
		DB_DATA_TYPE_BIGINT, aPlayerUID[7],
		DB_DATA_TYPE_END);

	if (!pDB->ExecuteSP(wstrSQL))
	{
		pOutDB->_i32Result = EVENT_ERROR_DATABASE_EXECUTE_SP;
		ErrorDB(i32TheadIdx, 206);
		return FALSE;
	}

	if (1 != pDB->GetRecordCount())
	{
		ErrorDB(i32TheadIdx, 207);
		return FALSE;
	}

	if (1 != pDB->GetINT32Data(0))
	{
		ErrorDB(i32TheadIdx, 208);
		return FALSE;
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	// Write
	pOutDB->Write(&stWarResult, sizeof(CLAN_WAR_RESULT));

	return TRUE;
}

BOOL	CDataBase::ClanBattleResultUser( INT32 i32TheadIdx, RING_IN_DB* pInDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	CLAN_RESULT_USER Result;
	
	pInDB->Reset();
	pInDB->Read( &Result,			sizeof( CLAN_RESULT_USER ));

	UINT32 ui32Win = 0;
	UINT32 ui32Lose = 0; 

	switch( Result._WinLose )
	{
	case ROOM_ENDTYPE_WIN:		ui32Win		= 1;		break;
	case ROOM_ENDTYPE_LOSE:		ui32Lose	= 1;		break;
	}
	
	//S2DB::MakeQueryW( wstrSQL, L"usp_Member_upd_BattleResultUser",
	//	DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
	//	DB_DATA_TYPE_INT,			i32TheadIdx,
	//	DB_DATA_TYPE_INT,			Result._Exp,
	//	DB_DATA_TYPE_INT,			ui32Win,
	//	DB_DATA_TYPE_INT,			ui32Lose,
	//	DB_DATA_TYPE_SMALLINT,		Result._Kill,
	//	DB_DATA_TYPE_SMALLINT,		Result._Death,
	//	DB_DATA_TYPE_SMALLINT,		Result._Head,
	//	DB_DATA_TYPE_TINYINT,		1,
	//	DB_DATA_TYPE_END);

	S2DB::MakeQueryW(wstrSQL, L"usp_Member_upd_BattleResultUser_Assist",
		DB_DATA_TYPE_BIGINT, pInDB->_i64UID,
		DB_DATA_TYPE_INT, i32TheadIdx,
		DB_DATA_TYPE_INT, Result._Exp,
		DB_DATA_TYPE_INT, ui32Win,
		DB_DATA_TYPE_INT, ui32Lose,
		DB_DATA_TYPE_SMALLINT, Result._Kill,
		DB_DATA_TYPE_SMALLINT, Result._Death,
		DB_DATA_TYPE_SMALLINT, Result._Head,
		DB_DATA_TYPE_TINYINT, 1,
		DB_DATA_TYPE_SMALLINT, Result._Assist,
		DB_DATA_TYPE_END);
	if (FALSE == pDB->ExecuteSP( wstrSQL ))
	{
		ErrorDB( i32TheadIdx, 124 );
		return FALSE;
	}
	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32TheadIdx, 89 );
		return FALSE;
	}

	INT32 i32Rv = pDB->GetINT32Data(0);

	if(1 != i32Rv)
	{
		ErrorDB( i32TheadIdx, 90 );
		return FALSE;
	}

	return FALSE;
}

BOOL	CDataBase::ClanMemberOrder( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;
	
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	// 공로자 UID 찾기
	T_UID i64ExpUID = 0;
	T_UID i64WinUID = 0;
	T_UID i64KillUID = 0;
	T_UID i64HeadUID = 0;
	T_UID i64ConnectUID = 0;

	// 공로자 최대값
	UINT32 ui32ExpTop = 0;
	UINT32 ui32WinTop = 0;
	UINT32 ui32KillTop = 0;
	UINT32 ui32HeadTop = 0;
	UINT32 ui32ConnectTop = 0;

	T_UID i64UID;
	UINT32 ui32Exp, ui32Win, ui32Kill, ui32Head, ui32Connect;

	// TClanMem 확인

	S2DB::MakeQueryW( wstrSQL, L"usp_Member_sel_Order",
		DB_DATA_TYPE_INT,			pInDB->_ui32ClanDBIdx,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{ 
		pOutDB->_i32Result = NET_ERROR_DB_QUERY;
		ErrorDB( i32TheadIdx, 125 );
		return TRUE;
	}
	if( 0 == pDB->GetRecordCount() )
	{
		pOutDB->_i32Result = EVENT_ERROR_CLAN_SEARCH_CLANMEMBERIDX;
		//ErrorDB( i32TheadIdx, 91 );
		return TRUE;
	}

	for( INT32 i = 0 ; i < pDB->GetRecordCount()  ; i++ )
	{
		i64UID			= pDB->GetINT64Data(0);
		ui32Exp			= pDB->GetUINT32Data(1);
		ui32Win			= pDB->GetUINT32Data(2);
		ui32Kill		= pDB->GetUINT32Data(3);
		ui32Head		= pDB->GetUINT32Data(4);
		ui32Connect		= pDB->GetUINT32Data(5);
		
		if( ui32ExpTop < ui32Exp )
		{
			ui32ExpTop	= ui32Exp;
			i64ExpUID	= i64UID;
		}
		if( ui32WinTop < ui32Win )
		{
			ui32WinTop = ui32Win;
			i64WinUID = i64UID;
		}
		if( ui32KillTop < ui32Kill )
		{
			ui32KillTop = ui32Kill;
			i64KillUID = i64UID;
		}
		if( ui32HeadTop < ui32Head )
		{
			ui32HeadTop = ui32Head;
			i64HeadUID = i64UID;
		}
		if( ui32ConnectTop < ui32Connect )
		{
			ui32ConnectTop = ui32Connect;
			i64ConnectUID = i64UID;
		}
		 
		pDB->MoveNext();
	}

	pOutDB->_i32Result = EVENT_ERROR_SUCCESS;
	
	pOutDB->Write( &i64ExpUID,		sizeof(T_UID) );
	pOutDB->Write( &i64WinUID,		sizeof(T_UID) );
	pOutDB->Write( &i64KillUID,		sizeof(T_UID) );
	pOutDB->Write( &i64HeadUID,		sizeof(T_UID) );
	pOutDB->Write( &i64ConnectUID,	sizeof(T_UID) );

	return TRUE;
}

void	CDataBase::ErrorDB( INT32 i32TheadIdx, INT32 i32LogNo )
{
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	g_pLog->WriteLog( L"[ErrorDB][%d] %s, Error Code : %08x", i32LogNo, m_pTaskDB[ i32TheadIdx ]._pQuery, pDB->GethrLastError() );
	
	HRESULT EventType = pDB->ReConnect(
												m_pClanDBConfig->GetDataSource(),
												m_pClanDBConfig->GetCatalog(),
												m_pClanDBConfig->GetAccount(),
												m_pClanDBConfig->GetPassword()
											);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDB] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDB] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

UINT32	CDataBase::GetAddAuthTime(UINT32 ui32Date, UINT32 ui32Sec)
{	// 입력받은 ui32Date( yymmddhhmi) 값에 ui32Sec를 더한후 ( yymmddhhmi ) 형태로 리턴.

	struct tm oldTime;
	
	oldTime.tm_year = (ui32Date / 100000000) + 100;				ui32Date %= 100000000;
	oldTime.tm_mon  = (ui32Date / 1000000) - 1;					ui32Date %= 1000000;
	oldTime.tm_mday = (ui32Date / 10000);						ui32Date %= 10000;
	oldTime.tm_hour = (ui32Date / 100) ;						ui32Date %= 100;
	oldTime.tm_min	= ui32Date; 
	oldTime.tm_sec  = ui32Sec; 
	oldTime.tm_isdst= SUMMER_TIME_FOLLOW_SYSTEM;

	mktime(&oldTime);
	
	UINT32 ui32Rv	=	(((oldTime.tm_year + 1900) - 2000) * 100000000) +
						((oldTime.tm_mon+1)  * 1000000) +
						(oldTime.tm_mday * 10000) +
						(oldTime.tm_hour * 100) +
						(oldTime.tm_min);

	return ui32Rv;

}


BOOL	CDataBase::ClanFindUserInfo( INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB )
{
	wchar_t* wstrSQL	= m_pTaskDB[ i32TheadIdx ]._pQuery;
	CADODatabase* pDB	= m_pTaskDB[ i32TheadIdx ]._pDB;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	pOutDB->_i32SessionIdx	= pInDB->_i32SessionIdx;
	pOutDB->_i64UID			= pInDB->_i64UID;
	T_UID	i64FindUID;

	pInDB->Reset();
	pInDB->Read(&i64FindUID, sizeof(T_UID));

	// TClanMem 확인(클랜 가입자일 경우에 실행됨)
	// TClanReq 확인(클랜 가입 신청자인 경우에만 처리됨)
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel",
		DB_DATA_TYPE_BIGINT,		i64FindUID,
		DB_DATA_TYPE_END);
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( i32TheadIdx, 1 );
		return FALSE;
	}
	if( 1 == pDB->GetRecordCount() )
	{
		UINT32	ui32ClanMark;;
		char	strClanName[ NET_CLAN_NAME_SIZE ];

		pOutDB->_i32Result		= EVENT_ERROR_SUCCESS;
		pOutDB->_ui32ClanDBIdx	= pDB->GetINT32Data(0);
		ui32ClanMark			= pDB->GetINT32Data(1);
		pDB->GetTextData( 2, strClanName, NET_CLAN_NAME_SIZE );

		pOutDB->Write( &i64FindUID,			sizeof(T_UID));
		pOutDB->Write( &ui32ClanMark,		sizeof(UINT32));
		pOutDB->Write( strClanName,			NET_CLAN_NAME_SIZE);
	}
	else
	{
		pOutDB->_i32Result		= EVENT_ERROR_FAIL;
	}

	return TRUE;
}

BOOL CDataBase::UpdateMercenaryMatchRecord(INT32 i32ThreadIdx, RING_IN_DB* pInDB)
{
	pInDB->Reset();

	CLAN_RESULT_USER stResult;
	INT32	i32MerRank;
	pInDB->Read(&i32MerRank, sizeof(INT32));
	pInDB->Read(&stResult,	 sizeof(CLAN_RESULT_USER));

	wchar_t* wstrSQL	= m_pTaskDB[i32ThreadIdx]._pQuery;

	/*S2DB::MakeQueryW( wstrSQL, L"usp_Mercenary_upd_Record",
	DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
	DB_DATA_TYPE_INT,			i32MerRank,
	DB_DATA_TYPE_TINYINT,		stResult._WinLose == ROOM_ENDTYPE_WIN ? 1 : 0 ,
	DB_DATA_TYPE_TINYINT,		stResult._Kill,
	DB_DATA_TYPE_TINYINT,		stResult._Death,
	DB_DATA_TYPE_TINYINT,		stResult._Head,
	DB_DATA_TYPE_END);*/

	S2DB::MakeQueryW(wstrSQL, L"usp_Mercenary_upd_Record_Assist",
		DB_DATA_TYPE_BIGINT, pInDB->_i64UID,
		DB_DATA_TYPE_INT, i32MerRank,
		DB_DATA_TYPE_TINYINT, stResult._WinLose == ROOM_ENDTYPE_WIN ? 1 : 0,
		DB_DATA_TYPE_TINYINT, stResult._Kill,
		DB_DATA_TYPE_TINYINT, stResult._Death,
		DB_DATA_TYPE_TINYINT, stResult._Head,
		DB_DATA_TYPE_TINYINT, stResult._Assist,
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32ThreadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB( i32ThreadIdx, 124 );
		return FALSE;
	}

	if( 1 != pDB->GetRecordCount() )
	{
		ErrorDB( i32ThreadIdx, 89 );
		return FALSE;
	}

	if( 1 != pDB->GetINT32Data(0) )
	{
		ErrorDB( i32ThreadIdx, 90 );
		return FALSE;
	}

	return TRUE;
}

BOOL CDataBase::UpdateMercenaryDisconnect(INT32 i32ThreadIdx, RING_IN_DB* pInDB)
{
	pInDB->Reset();
	DATE32 dtMerPenaltyTime;
	wchar_t wstrMerPenalty[ SMALLDATE_STRING_SIZE ];
	wchar_t* wstrSQL	= m_pTaskDB[i32ThreadIdx]._pQuery;

	pInDB->Read(&dtMerPenaltyTime, sizeof(DATE32));
	dtMerPenaltyTime.GetSmallDate(wstrMerPenalty, SMALLDATE_STRING_SIZE);

	S2DB::MakeQueryW( wstrSQL, L"usp_Mercenary_upd_Disconnect",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_SMALLDATETIME,	wstrMerPenalty,
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32ThreadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		ErrorDB( i32ThreadIdx, 124 );
		return FALSE;
	}

	return TRUE;
}

BOOL CDataBase::LoadMercenary(INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB)
{
	T_MerID TMerID;

	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	pInDB->Reset();
	pInDB->Read(&TMerID, sizeof(T_MerID));

	pOutDB->Write(&TMerID,		sizeof(T_MerID));
	
	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;

	S2DB::MakeQueryW( wstrSQL, L"usp_Mercenary_sel",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_END);
	
	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		pOutDB->_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
		ErrorDB( i32TheadIdx, 155 );
		return FALSE;
	}

	INT32 i32Count = pDB->GetRecordCount();
	if( i32Count < 1 )
	{	// 용병 전적 생성 실패.
		pOutDB->_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
		ErrorDB( i32TheadIdx, 156 );
		return FALSE;
	}

	INT32 i32Column = 0;
	MERCENARY_RECORD stRecord;
	DATE32 dtPenaltyTime;

	INT32	i32MerRank			= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32MatchCount	= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32WinMatchCnt	= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32LoseMatchCnt	= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32DisconMatchCnt= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32KillCount		= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32DeathCount	= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32Headshot		= pDB->GetINT32Data(i32Column++);
	stRecord.m_i32AssistCount	= pDB->GetINT32Data(i32Column++);
	dtPenaltyTime	= pDB->GetDateData(i32Column++);

	INT32 i32DiffTime = i3ThreadTimer::LocalTime().DiffTime( dtPenaltyTime );	// 용병 패널티 남은 시간 체크
	if( i32DiffTime < 0 )
	{
		pOutDB->_i32Result = EVENT_ERROR_CLAN_WAR_MERCENAY_DIS_TIME;
		return FALSE;
	}

	pOutDB->_i32Result	= EVENT_ERROR_SUCCESS;

	pOutDB->Write(&i32MerRank,	sizeof(INT32));
	pOutDB->Write(&stRecord,	sizeof(MERCENARY_RECORD));

	return TRUE;
}

BOOL CDataBase::LoadMercenaryPenaltyTime(INT32 i32TheadIdx, RING_IN_DB* pInDB, RING_OUT_DB* pOutDB)
{
	pOutDB->Reset();
	pOutDB->Copy( pInDB );

	pInDB->Reset();

	DATE32 dtMerPenaltyTime;
	wchar_t* wstrSQL = m_pTaskDB[i32TheadIdx]._pQuery;

	S2DB::MakeQueryW( wstrSQL, L"usp_Mercenary_sel_PenaltyTime",
		DB_DATA_TYPE_BIGINT,		pInDB->_i64UID,
		DB_DATA_TYPE_END);

	CADODatabase* pDB = m_pTaskDB[i32TheadIdx]._pDB;
	if( !pDB->ExecuteSP(wstrSQL) )
	{
		pOutDB->_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
		ErrorDB( i32TheadIdx, 1122 );
		return FALSE;
	}

	dtMerPenaltyTime = pDB->GetDateData(0);

	pOutDB->_i32Result	= EVENT_ERROR_SUCCESS;
	pOutDB->Write( &dtMerPenaltyTime, sizeof(DATE32) );

	return TRUE;
}