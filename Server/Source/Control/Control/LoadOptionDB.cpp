#include "pch.h"
#include "LoadOptionDB.h"

CLoadOptionDB * g_pLoadOptionDB = NULL;
 
I3_CLASS_INSTANCE( CLoadOptionDB, i3Thread );

CLoadOptionDB::CLoadOptionDB(void)
{	
	m_i32Version			= 0;
	m_pDB					= NULL;
	m_ui32CheckSec_Old		= 0;
	m_ui32CheckSec_New		= 0;

	m_pInLogIN				= NULL;
	m_pOutLogIN				= NULL;

	m_bRunning				= FALSE;
}


CLoadOptionDB::~CLoadOptionDB(void)
{
	OnDestroy();
}

void CLoadOptionDB::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();
	I3_SAFE_RELEASE( m_pDB );

	I3_SAFE_DELETE( m_pInLogIN );
	I3_SAFE_DELETE( m_pOutLogIN );
}
 

BOOL CLoadOptionDB::OnCreate()
{
	m_pInLogIN = new Local_CRingBuffer; 
	if( NULL == m_pInLogIN || false == m_pInLogIN->OnCreate(sizeof(IN_DB_SUPER_USER), 10) )
	{
		return FALSE;
	}
	
	m_pOutLogIN = new Local_CRingBuffer;
	if( NULL == m_pOutLogIN || false == m_pOutLogIN->OnCreate(sizeof(OUT_DB_SUPER_USER), 10) )
	{
		return FALSE;
	}

	//1. DataBase Connect 
	m_pDB = CADODatabase::NewObject();
	i3String::Format(m_strUdlLogPath, SERVER_STRING_COUNT, "File Name=%s\\LoadOptionDataBase.udl;", g_strCurrentPath);
	if( NULL == m_pDB || FALSE == m_pDB->Connect(m_strUdlLogPath) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] PBOption Connect Failed");
		return FALSE;
	}

	if( FALSE == _LoadDatabaseConfig() )
	{
		return FALSE;
	}

	if( !FixLoadDB() )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] FixLoadDB() Failed");
		return FALSE;
	}

	INT32 i32NowVersion = GetControlVersion( );
	if( i32NowVersion < 0 )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] GetControlVersion() Failed");
		return FALSE; 
	}

	if ( FALSE == ChangeLoadDB( i32NowVersion ) ) 
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] ChangeLoadDB() Failed");
		return FALSE;
	}
	
	if( FALSE == i3Thread::Create("CLoadOptionDB", 0, 4096, NULL, PRIORITY_NORMAL) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] i3Thread Failed");
		return FALSE;
	}
	
	return TRUE; 
}

BOOL CLoadOptionDB::FixLoadDB( void )
{
	if( FALSE == _LoadContentsControl( ) )			return FALSE;
	if( FALSE == _LoadFixBasic(  ) )				return FALSE;
	if( FALSE == _LoadMap( ) )						return FALSE;
	if (FALSE == _LoadRandomMap())					return FALSE;
	if( FALSE == _LoadMissionCard( ) )				return FALSE;
	if( FALSE == _LoadDailyGift( ) )				return FALSE;
	if( FALSE == _LoadAttendance( ) )				return FALSE;
	if( FALSE == _LoadMedal( ) )					return FALSE;
	if( FALSE == _LoadDefaultItem( ) )				return FALSE;
	if( FALSE == _LoadGiftBuyRanking( ) )			return FALSE;
	if( FALSE == _LoadChannelList( ) )				return FALSE;
	if( FALSE == _LoadClanMatchSeason( ) )			return FALSE;
	if( FALSE == _LoadGachaSystem( ) )				return FALSE;
	if( FALSE == _LoadVoteLimit( ) )				return FALSE;
	if( FALSE == _LoadChatHack( ) )					return FALSE;
	if (FALSE == _LoadGameNotice())					return FALSE;
	if (FALSE == _LoadBillingURL())					return FALSE;

	return TRUE;
}

BOOL CLoadOptionDB::ChangeLoadDB( INT32 i32Version )
{	
	if( FALSE == _LoadChangeBasic(  ) )				return FALSE;
	if( FALSE == _LoadBoostEvent(  ) )				return FALSE;	
	if( FALSE == _LoadMACAddress(  ) )				return FALSE;
	if( FALSE == _LoadNotice(  ) )					return FALSE;
	if( FALSE == _LoadGeneralRankup(  ) )			return FALSE;
	if( FALSE == _LoadNewUserEvent(  ) )			return FALSE;
	if( FALSE == _LoadLoginEvent(  ) )				return FALSE;
	if( FALSE == _LoadRankUpItem(  ) )				return FALSE;	
	if( FALSE == _LoadIPBlock(  ) )					return FALSE;
	if( FALSE == _LoadNowChannelUserCount(  ) )		return FALSE;
	if( FALSE == _LoadTagPriority( ) )				return FALSE;
	if (FALSE == _LoadURLList())					return FALSE;

	m_i32Version = i32Version;

	g_pControl->m_bReloadConfig = TRUE;

	return TRUE;
}

UINT32 CLoadOptionDB::OnRunning( void * pUserData)
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	BOOL bWorking;

	m_i32LoadOptionDBIdx = g_pLog->AddLogThread(L"LODB", 0, m_ThreadID);

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		m_ui32CheckSec_New = i3ThreadTimer::GetServerTime();

		bWorking = FALSE;
		if( m_pInLogIN->GetBufferCount() > 0 )
		{			
			OUT_DB_SUPER_USER * pOutUser = (OUT_DB_SUPER_USER *) m_pOutLogIN->PushPointer(); 
			if(pOutUser != NULL)
			{
				IN_DB_SUPER_USER  * pInUser = (IN_DB_SUPER_USER *)m_pInLogIN->Pop(); 
				//------------------------------------------------------------------------------------------------------------

				pOutUser->iSessionIdx	= pInUser->iSessionIdx; 
				pOutUser->i32Rv			= NET_ERROR_DB_QUERY;
				i3String::Copy( pOutUser->wstrID, pInUser->wstrID, NET_ID_SIZE );

				S2DB::MakeQueryW( wstrSQL, L"usp_WebToolUser_sel",
					DB_DATA_TYPE_TTCHAR,		pInUser->wstrID,
					DB_DATA_TYPE_TTCHAR,		pInUser->wstrPass,
					DB_DATA_TYPE_END);
				if( !m_pDB->ExecuteSP( wstrSQL ) )		
				{
					ErrorDB( wstrSQL );
					pOutUser->i32Rv = NET_ERROR_DB_QUERY;
				}
				else
				{
					if( m_pDB->GetRecordCount() == 1)
					{
						pOutUser->i32Rv		= 1;
						pOutUser->i8Service	= (INT8)m_pDB->GetINT32Data(0);
					}
					else
					{
						pOutUser->i32Rv = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT; 
					}
				}

				//#ifdef I3_DEBUG	//디버그 용이면 무조건 리턴 합니다. 
				//	pOutUser->i32Rv = 1;
				//	pOutUser->i8Service = SERVICE_AUTH_ADMIN;
				//#endif


				//------------------------------------------------------------------------------------------------------------
				m_pInLogIN->PopIdx();
				m_pOutLogIN->PushPointerIdx(); 

				bWorking = TRUE;
			}
		}

		if( ( m_ui32CheckSec_New - m_ui32CheckSec_Old ) >= DATABASE_RELOAD_TIME )
		{
			m_ui32CheckSec_Old = m_ui32CheckSec_New;

			INT32 i32NowVersion = GetControlVersion( );
			if( i32NowVersion > -1)
			{
				if( i32NowVersion != m_i32Version ) // 버전이 증가했다면 
				{
					if ( FALSE == ChangeLoadDB( i32NowVersion ) )
					{
						g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[CLoadOptionDB::OnRunning] ChangeLoadDB() Failed"); 
					}
					else
					{
						g_pLog->WriteLog( L"[CLoadOptionDB::OnRunning] ChangeLoadDB() SUCCESS / Version : %d", m_i32Version ); 
					}
				}
			}
			else
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[CLoadOptionDB::OnRunning] GetControlVersion() Failed"); 
			}
		}
		

		if( !bWorking )
		{
			::WaitForSingleObject( m_Handle, 1 );
		}
	}
	return 0; 
}

 
INT32 CLoadOptionDB::GetControlVersion( )
{
	INT32 i32Rv = -1;

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_OptionVersion_scan",
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
	}
	else 
	{
		i32Rv = m_pDB->GetINT32Data(0);
	}

	return i32Rv;
}

void CLoadOptionDB::ErrorDB( wchar_t* wstrSQL )
{
	g_pLog->WriteLog( L"[ErrorDB] %s, Error Code : %08x", wstrSQL, m_pDB->GethrLastError() );
	
	HRESULT EventType = m_pDB->ReConnect( m_strUdlLogPath );
	
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


BOOL CLoadOptionDB::_LoadDatabaseConfig()
{
	if( !m_pDB->ExecuteSP("EXEC usp_DBList_scan") )
	{
		g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::LoadDatabaseConfig] usp_DBList_scan");
		return FALSE;
	}

	INT32 i32Count = m_pDB->GetRecordCount();
	
	for( INT32 i = 0; i < i32Count; ++i )
	{
		INT32 i32Index = 0;
		
		DATABASE_CONFIG stDatabaseConfig;
		i32Index++;		// Index
		m_pDB->GetTextData(i32Index++, stDatabaseConfig.m_szDBName, DB_STRING_LENGTH);
		m_pDB->GetTextData(i32Index++, stDatabaseConfig.m_szDataSource, DB_STRING_LENGTH);
		m_pDB->GetTextData(i32Index++, stDatabaseConfig.m_szCatalog, DB_STRING_LENGTH);
		m_pDB->GetTextData(i32Index++, stDatabaseConfig.m_szAccount, DB_STRING_LENGTH);
		m_pDB->GetTextData(i32Index++, stDatabaseConfig.m_szPassword, DB_STRING_LENGTH);

		UINT32 ui32ServerFlag = m_pDB->GetINT32Data(i32Index++);
		for( INT32 i32Idx = 0; i32Idx < ASC_SERVER_COUNT; ++i32Idx )
		{
			if( !(ui32ServerFlag & (1 << i32Idx )) )
				continue;
			
			ASC_SERVER_TYPE eServer = static_cast<ASC_SERVER_TYPE>( i32Idx );
			if( !g_pControl->InsertDatabaseConfig(eServer, &stDatabaseConfig) )
			{
				g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::LoadDatabaseConfig] Too many DB config.");
				return FALSE;
			}
		}
		
		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadContentsControl( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_ControlContents_scan",
		DB_DATA_TYPE_END);
	if( !m_pDB->ExecuteSP(wstrSQL) )
	{
		g_pLog->WriteLog( L"[DB Error][CLoadOptionDB::FixLoadDB] usp_ControlContents_scan");
		ErrorDB( wstrSQL );
		return FALSE;
	}

	wchar_t szContentsName[MAX_STRING_COUNT];
	INT32 i32ContentsCount = m_pDB->GetRecordCount();
	i32ContentsCount = MIN( i32ContentsCount, CONTENTS_COUNT ); 

	for( INT32 i = 0; i < i32ContentsCount; ++i )
	{
		CONTENTS eContents		= static_cast<CONTENTS>(m_pDB->GetINT32Data(0));
		INT32 i32ContentsVer	= m_pDB->GetINT32Data(2);
		BOOL bEnable			= m_pDB->GetboolData(3);

		g_pControl->InsertContents(eContents, i32ContentsVer, bEnable != 0);		
		m_pDB->GetTextData(1, szContentsName, MAX_STRING_COUNT);
		
		g_pLog->WriteLog( L"[Contents] %d : %s(Ver : %d) - %s", eContents, szContentsName, i32ContentsVer, bEnable ? L"ON" : L"OFF" );

		m_pDB->MoveNext();
	}

#ifndef ENABLE_GACHA_SHOP
	g_pControl->InsertContents(CONTENTS_GACHA, 0, false);
#endif
	/*if( !g_pControl->CheckDefaultContents() )
	{
		g_pLog->WriteLog( L"[Error][CLoadOptionDB::FixLoadDB] Disabled default contents.");
		return FALSE;
	}*/

	return TRUE;
}

BOOL CLoadOptionDB::_LoadFixBasic( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	//1.TPBF_Basic 
	S2DB::MakeQueryW( wstrSQL, L"usp_Config_scan",
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] TPBF_Basic SELECT Fail" );
		ErrorDB( wstrSQL );
		return FALSE;
	}

	// 로우 없으면 리턴.
	INT32 i32FixBasicCount = m_pDB->GetRecordCount();
	if( 1 > i32FixBasicCount )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] TPBF_Basic No Records" );
		return FALSE;
	}

	// 행이 2개이상 존재시 로그만 찍는다.
	if( 1 < i32FixBasicCount )  g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] TPBF_Basic have more than 2 Records"); 

	g_pControl->m_ui32QuestEvent							= m_pDB->GetUINT32Data(0);
	g_pControl->m_i32ClanMaxCount							= m_pDB->GetINT32Data(1);
	//g_pControl->m_i8PrimaryWeapon							= m_pDB->GetINT8Data(2);
	//g_pControl->m_bNewRepair								= m_pDB->GetINT8Data(3)>0?true:false;
	//g_pControl->m_bGiftEnable								= m_pDB->GetINT8Data(4)>0?true:false;
	g_pControl->m_eServerVersion							= (PB_VER)m_pDB->GetUINT8Data(2);

	g_pControl->m_AASOption.m_ui8AASEnableFlag				= m_pDB->GetUINT8Data(3);
	g_pControl->m_AASOption.m_ui16AASProhibitionFlag		= m_pDB->GetUINT16Data(4);
	g_pControl->m_AASOption.m_ui16AASDailyUsageMaxMin		= m_pDB->GetUINT16Data(5);
	g_pControl->m_AASOption.m_ui16AASDailyUsageResetTime	= m_pDB->GetUINT16Data(6);
	g_pControl->m_AASOption.m_ui64AASAppointedTimeTable		= m_pDB->GetUINT64Data(7);

	g_pControl->m_ui8LocaleCode								= m_pDB->GetUINT8Data(8);
	g_pControl->m_ui8GiftShopRank							= m_pDB->GetUINT8Data(9);

	UINT8 ui8MultiWeaponCount								= m_pDB->GetUINT8Data(10);
	g_pControl->m_ui8MultiWeaponCount = MINMAX(MULTI_WEAPON_SLOT_MIN, ui8MultiWeaponCount, MULTI_WEAPON_SLOT_MAX);
	return TRUE;
}	

BOOL CLoadOptionDB::_LoadMap( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	//INT32	i32Idx = 0;
	
	//1. MapRuleList Scan
	S2DB::MakeQueryW( wstrSQL, L"usp_MapRule_scan",
		DB_DATA_TYPE_END);
 	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] usp_MapRule_scan Fail");
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32RuleCount = m_pDB->GetRecordCount();
	g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Rule Count : %d", i32RuleCount);
	INT32 i32Ruleidx = 0;
	
	for( INT32 i = 1 ; i < i32RuleCount + 1 ; i++ )
	{
		i32Ruleidx = m_pDB->GetINT32Data(0);
		if (i32Ruleidx < 0)
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Invalid Rule Data. RuleIdx = %d", i32Ruleidx);
			m_pDB->MoveNext();
			continue;
		}
		
		STAGE_MODE			eStageMode			= static_cast<STAGE_MODE>(m_pDB->GetUINT8Data(2));
		WEAPON_RESTRICTION	eWeaponRestriction	= static_cast<WEAPON_RESTRICTION>(m_pDB->GetUINT8Data(3));
		AI_USE				eAIUse				= static_cast<AI_USE>(m_pDB->GetUINT8Data(4));
 		BOOL				bClanMatch			= m_pDB->GetboolData(5);
		BOOL				bAtkDefChange		= m_pDB->GetboolData(6);
		INT32				i32MinorRules		= m_pDB->GetINT32Data(8);
		
		g_pControl->m_aRuleData[i].m_i32RuleIdx		= i32Ruleidx;
		g_pControl->m_aRuleData[i].m_ui32Rule		= StageID::MakeStageID(eStageMode, eWeaponRestriction, eAIUse, bClanMatch, bAtkDefChange, i32MinorRules);
		g_pControl->m_aRuleData[i].m_ui8DefaultStage	= static_cast<UINT8>(m_pDB->GetINT32Data(7));
		m_pDB->GetTextData(9, g_pControl->m_aRuleData[i].m_strRuleLabel, RULE_STRING_MAX);
		g_pControl->m_aRuleData[i].m_strRuleLabel[RULE_STRING_MAX - 1] = '\0';

		m_pDB->MoveNext();
	}

	g_pControl->m_ui8RuleTotalCount = static_cast<UINT8>(i32RuleCount + 1);

	//2. MapStageList Scan
	S2DB::MakeQueryW(wstrSQL, L"usp_MapStage_scan",
		DB_DATA_TYPE_END);
	if (FALSE == m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] usp_MapStage_scan Fail");
		ErrorDB(wstrSQL);
		return FALSE;
	}

	INT32 i32StageCount = m_pDB->GetRecordCount();
	g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Stage Count : %d", i32StageCount);
	INT32 i32Stageidx = 0;

	for ( INT32 i = 1 ; i < i32StageCount + 1 ; i++ )
	{
		i32Stageidx = m_pDB->GetINT32Data(0);
		if (i32Stageidx < 0)
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Invalid Stage Data. StageUID = %d", i32Stageidx);
			m_pDB->MoveNext();
			continue;
		}

		m_pDB->GetTextData( 2, g_pControl->m_aStageData[i].m_strCodeName, STAGE_CODE_NAME_COUNT );
		g_pControl->m_aStageData[i].m_i32Priority = m_pDB->GetINT32Data(3);

		m_pDB->MoveNext();
	}

	g_pControl->m_ui8StageTotalCount = static_cast<UINT8>(i32StageCount + 1);
	
	//3. MapRuleStageMatching Sel_Open
	S2DB::MakeQueryW(wstrSQL, L"usp_MapRuleStageMatching_sel",
		DB_DATA_TYPE_END);
	if (FALSE == m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] usp_MapRuleStageMatching_sel Fail");
		ErrorDB(wstrSQL);
		return FALSE;
	}

	INT32 i32MatchingCount = m_pDB->GetRecordCount();
	g_pControl->m_ui16StageMatchingCount = static_cast<UINT16>(i32MatchingCount);
	g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] RuleStageMatching Count : %d", i32MatchingCount);

	INT32 i32MatchingIdx = 0;

	for ( INT32 i = 0 ; i < i32MatchingCount ; i++)
	{
		INT32 i32RuleIdx	= m_pDB->GetINT32Data(0);
		if( i32RuleIdx < 0 || STAGE_MODE_NA == StageID::GetStageMode(g_pControl->m_aRuleData[i32RuleIdx].m_ui32Rule) )
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Invalid StageMatching Data. RuleIdx = %d", i32RuleIdx);
			m_pDB->MoveNext();
			continue;
		}

		INT32 i32StageUID	= m_pDB->GetINT32Data(1);
		if( i32StageUID < 0 || 0 == i3String::Compare( g_pControl->m_aStageData[i32StageUID].m_strCodeName, "" ) )
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Invalid StageMatching Data. StageUID = %d", i32StageUID);
			m_pDB->MoveNext();
			continue;
		}

		g_pControl->m_aStageMatching[i32MatchingIdx].m_i32RuleIdx	= i32RuleIdx;
		g_pControl->m_aStageMatching[i32MatchingIdx].m_ui32StageID	= g_pControl->m_aRuleData[i32RuleIdx].m_ui32Rule + static_cast<UINT32>(i32StageUID);
		g_pControl->m_aStageMatching[i32MatchingIdx].m_ui8MaxPerson	= m_pDB->GetUINT8Data(2);
		g_pControl->m_aStageMatching[i32MatchingIdx].m_ui8Event		= m_pDB->GetUINT8Data(3);
		g_pControl->m_aStageMatching[i32MatchingIdx].m_ui8Mark		= m_pDB->GetUINT8Data(4);		
		g_pControl->m_aStageData[i32StageUID].m_bIsOpen = true;

		i32MatchingIdx++;
		m_pDB->MoveNext();
	}
	INT32 test = 0;
	for (INT32 i = 0; i < i32MatchingCount; i++)
	{
		if (g_pControl->m_aStageData[i].m_bIsOpen == true)
			test++;
	}
		
	if ( FALSE == g_pControl->CheckDefaultMap() )
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Default Map Set failed.");
		return FALSE;
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadRandomMap()
{
	if (!g_pControl->IsContentsEnabled(CONTENTS_RANDOMMAP))
		return TRUE;

	wchar_t wstrSQL[SERVER_STRING_COUNT];

	//1. MapRuleList Scan
	S2DB::MakeQueryW(wstrSQL, L"usp_MapRandomProbability_scan", DB_DATA_TYPE_END);
	if (FALSE == m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] usp_MapRandomProbability_scan Fail");
		ErrorDB(wstrSQL);
		return FALSE;
	}

	UINT16 ui16RandomMapCount = (UINT16)m_pDB->GetRecordCount();
	g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Rule Count : %d", ui16RandomMapCount);
	INT32 i32Ruleidx = 0;

	UINT16 ui16StartIdx		= 0;
	UINT8 ui8CurrentRule	= 0;
	UINT8 ui8CurrentMaxPerson = 0;

	for (UINT16 i = 0; i < ui16RandomMapCount; ++i)
	{
		i32Ruleidx = m_pDB->GetINT32Data(0);
		if (i32Ruleidx < 0)
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Invalid Rule Data. RuleIdx = %d", i32Ruleidx);
			m_pDB->MoveNext();
			continue;
		}

		g_pControl->m_aRandomMapData[i].m_ui8RuleIdx		= m_pDB->GetUINT8Data(0);
		g_pControl->m_aRandomMapData[i].m_ui8StageUID		= m_pDB->GetUINT8Data(1);
		g_pControl->m_aRandomMapData[i].m_ui8MaxPerson		= m_pDB->GetUINT8Data(2);
		g_pControl->m_aRandomMapData[i].m_ui16Percentage	= m_pDB->GetUINT16Data(3);

		if (g_pControl->m_aRandomMapData[i].m_ui8RuleIdx != ui8CurrentRule
			|| g_pControl->m_aRandomMapData[i].m_ui8MaxPerson != ui8CurrentMaxPerson)
		{
		
			InitRandomMapIdx(ui16StartIdx, i, ui8CurrentRule, ui8CurrentMaxPerson);
		}
		
		m_pDB->MoveNext();
	}

	g_pControl->m_ui16RandomMapCount = ui16RandomMapCount;

	return TRUE;
}

BOOL CLoadOptionDB::_LoadMissionCard( )
{
	if( !g_pControl->IsContentsEnabled( CONTENTS_MISSION_CARD ) ) 
	{
		return TRUE;
	}

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	// 3.TPBF_MissionCard 
	g_pControl->m_ui8ActiveQuestCount	= ((QUEST_CARDSET_TYPE_COUNT-1) / 32)+1;	// bit 단위로 보내기 때문에 UINT32 기준으로 나눕니다.
	g_pControl->m_pui32ActiveQuest		= (UINT32*) i3MemAlloc(sizeof(UINT32) * g_pControl->m_ui8ActiveQuestCount );
	if( NULL == g_pControl->m_pui32ActiveQuest )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] TPBF_Map ActiveStage is Null" ); 
		return FALSE;
	}
	i3mem::FillZero( g_pControl->m_pui32ActiveQuest, sizeof(UINT32) * g_pControl->m_ui8ActiveQuestCount );

	S2DB::MakeQueryW( wstrSQL, L"usp_MissionCard_scan", 
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] TPBF_MissionCard SELECT Fail" ); 
		return FALSE; 
	}
	
	for( INT32 i = 0 ; i < m_pDB->GetRecordCount() ; i++ )
	{
		if( 0 != m_pDB->GetINT8Data(0) )
		{
			INT32 i32Type	= m_pDB->GetINT8Data(1);
			if( i32Type != QUEST_CARDSET_TYPE_EVENT )
			{
				g_pControl->m_pui32ActiveQuest[ i32Type / 32 ] |= 1<< (i32Type%32);
			}
		}

		m_pDB->MoveNext();
	}

	// 이벤트 임무카드 활성화 여부는 m_ui32QuestEvent 값을 따라간다.
 	if ( 0 != g_pControl->m_ui32QuestEvent ) 
	{
		g_pControl->m_pui32ActiveQuest[ QUEST_CARDSET_TYPE_EVENT / 32 ] |= 1<< (QUEST_CARDSET_TYPE_EVENT%32);
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadDailyGift( )
{
	if( !g_pControl->IsContentsEnabled( CONTENTS_TS_EVENT ) ) 
	{
		g_pControl->m_DailyGift.m_bActive = false;
		return TRUE;
	}

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	// 판수보상 정보 로드
	S2DB::MakeQueryW( wstrSQL, L"usp_EventDailyGift_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	if( 1 != m_pDB->GetRecordCount() )
	{
		g_pLog->WriteLog( L"[LoadDailyGift][Not Use][Invalid Insert Row]" );
		return TRUE;
	}

	if( false == S2DB::ConvertUINT8Tobool( m_pDB->GetUINT8Data(0) ) )
	{
		g_pLog->WriteLog( L"[LoadDailyGift][Not Use]" );
		return TRUE;
	}

	//m_pDB->GetUINT32Data(1);
	//if( 23 <  g_pControl->m_DailyGift.m_ui32ResetTime )
	//{ // 1시간보다 작으면 웹툴 설정 오류
	//	WRITE_LOG_LOAD_OPTION_DB( "[LoadDailyGift][GiftInfo]"); 
	//	return FALSE;
	//}

	// 제한 시간 저장
	for( INT32 i = 0; i < MAX_DAILY_GIFT_COUNT; i++ )
	{
		g_pControl->m_DailyGift.m_ui32PlayTime[i]			= (UINT32)( m_pDB->GetUINT8Data( 2+i ) * 60 ) ; // 초단위 변환
		if(0 < i)
		{			
			for(INT32 InnerLoop = 0; InnerLoop < i; InnerLoop++)
			{
				g_pControl->m_DailyGift.m_ui32PlayTime[i]	= ( g_pControl->m_DailyGift.m_ui32PlayTime[i] - g_pControl->m_DailyGift.m_ui32PlayTime[InnerLoop] );
			}
		}
	}

	// 판수보상 Goods List 로드
	S2DB::MakeQueryW( wstrSQL, L"usp_EventDailyGiftReward_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32GoodsCount = m_pDB->GetRecordCount();
	if( MAX_DAILY_GIFT_GOODS_COUNT < i32GoodsCount )
	{ // 개수를 넘을 수 없다.
		g_pLog->WriteLog( L"[LoadDailyGift][DG_GoodsID_1]"); 
		return FALSE;
	}
		
	UINT8	ui8Type = 0;
	UINT32	ui32Count[MAX_DAILY_GIFT_COUNT][2]	= {0,};
	INT32	i32Rate[MAX_DAILY_GIFT_COUNT]		= {0,};
	UINT32	ui32GoodsIDTemp;
	INT32	i32RateTemp;

	for(INT32 Loop = 0; Loop < i32GoodsCount; Loop++)
	{
		ui8Type = m_pDB->GetUINT8Data(1);
			
		if( ui32Count[ui8Type][0] > MAX_DAILY_GIFT_GOODS_LIST)	
		{
			g_pLog->WriteLog( L"[LoadDailyGift][DG_GoodsID_2]"); 
			return FALSE;
		}

		ui32GoodsIDTemp		= m_pDB->GetUINT32Data(2);
		i32RateTemp			= (INT32)m_pDB->GetUINT16Data(3);

		ui32Count[ui8Type][0]++;
		if( 0 < ui32GoodsIDTemp )
		{
			g_pControl->m_DailyGift.m_DailyGiftGoodsList[ui8Type][ui32Count[ui8Type][1]].m_ui32GoodsID		= ui32GoodsIDTemp;
			g_pControl->m_DailyGift.m_DailyGiftGoodsList[ui8Type][ui32Count[ui8Type][1]].m_i32Rate			= i32RateTemp * ADD_DAILY_GIFT_RATE;

			i32Rate[ui8Type] += g_pControl->m_DailyGift.m_DailyGiftGoodsList[ui8Type][ui32Count[ui8Type][1]].m_i32Rate;
			ui32Count[ui8Type][1]++;
		}

		m_pDB->MoveNext();
	}

	for(INT32 Loop = 0; Loop < MAX_DAILY_GIFT_COUNT; Loop++)
	{
		if( MAX_DAILY_GIFT_RATE != i32Rate[Loop] )
		{
			if(0 == i32Rate[Loop]) 
			{ 
				g_pControl->m_DailyGift.m_DailyGiftGoodsList[Loop][0].m_i32Rate	= MAX_DAILY_GIFT_RATE;
				continue; 
			}

			g_pLog->WriteLog( L"[LoadDailyGift][DG_GoodsID_3]"); 
			return FALSE;
		}
	}

	g_pControl->m_DailyGift.m_bActive = true;

	return TRUE;
}

BOOL CLoadOptionDB::_LoadAttendance( )
{
	CONTENTS attendanceContents[ATTENDANCE_TYPE_MAX] = 
	{ // 컨트롤 컨텐츠에 출석 이벤트 타입 추가시 맨 아래에 추가해 주세요.
		CONTENTS_DORMANT,
		CONTENTS_ATTENDANCE, 
	};

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	// 3.TPBF_Attendance
	wchar_t wstrTemp[ SMALLDATE_STRING_SIZE ];
	S2DB::ConvertUINT32ToSmallDate( wstrTemp, SMALLDATE_STRING_SIZE, i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() );
	// 타입별로 SP 호출
	for ( INT32 i32attendanceType = 0; i32attendanceType < ATTENDANCE_TYPE_MAX; i32attendanceType++)
	{
		if (!g_pControl->IsContentsEnabled( attendanceContents[i32attendanceType]) )
		{
			continue;
		}
		S2DB::MakeQueryW(wstrSQL, L"usp_EventAttendance_sel_All",
			DB_DATA_TYPE_SMALLDATETIME, wstrTemp,
//			DB_DATA_TYPE_TINYINT, i32attendanceType,
			DB_DATA_TYPE_END);
		if (FALSE == m_pDB->ExecuteSP(wstrSQL))
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Attendance SELECT Fail");
			return FALSE;
		}

		// 이벤트 개수가 최대 설정 개수보다 많이 등록되어있으면 최대 설정 개수만큼만 읽는다.
		INT32 i32EventCount = m_pDB->GetRecordCount();
		if (i32EventCount > ATTENDANCE_MAX_EACH_EVENT)
		{
			i32EventCount = ATTENDANCE_MAX_EACH_EVENT;
			g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Attendance Event Count > ATTENDANCE_MAX_EVENT_COUNT");
		}

		UINT8 ui8Day;
		INT32 i32PrevEventIdx = -1;

		for (INT32 i = 0; i < i32EventCount; ++i)
		{
			ui8Day = m_pDB->GetUINT8Data(4);
			// 최대 지정할 수 있는 기간보다 길 경우 넘김 ( 이벤트 기간 자체가 아니라 연속 일수 )
			if (ui8Day > ATTENDANCE_MAX_CONSECUTIVE_DAY)
			{
				m_pDB->MoveNext();
				continue;
			}
			ATTENDANCE_INFO *pEventInfo = &g_pControl->m_aAttendanceInfo[i32attendanceType][i];

			pEventInfo->m_ui32Version		= m_pDB->GetUINT32Data(0);
			m_pDB->GetTextData(1, pEventInfo->m_strEventName, ATTENDANCE_NAME_MAX_SIZE);
			pEventInfo->m_ui32StartDate		= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(2) );
			pEventInfo->m_ui32EndDate		= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(3) );

			if (i32PrevEventIdx >= 0)
			{
				g_pControl->m_aAttendanceInfo[i32attendanceType][i32PrevEventIdx].m_ui32NextStartDate = pEventInfo->m_ui32StartDate;
				g_pControl->m_aAttendanceInfo[i32attendanceType][i32PrevEventIdx].m_ui32NextEndDate = pEventInfo->m_ui32EndDate;
			}

			pEventInfo->m_ui8AttendanceDay	= ui8Day;
			pEventInfo->m_bRepeat			= m_pDB->GetboolData(5);
			pEventInfo->m_bReset			= m_pDB->GetboolData(6);
			pEventInfo->m_ui32ConditionDate	= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(7) );
			pEventInfo->m_ui8AttendanceType = (UINT8)i32attendanceType;

			i32PrevEventIdx = i;

			m_pDB->MoveNext();
		}

		INT32		i32GoodsCount;
		INT32		i32Order;
		T_GoodsID	TGoodsID;
		ATTENDANCE_CHOICE_ITEM* pChoiceItem;
		// 활성화된 이벤트들의 아이템 설정
		for (INT32 i = 0; i < i32EventCount; ++i)
		{
			ATTENDANCE_INFO *pEventInfo = &g_pControl->m_aAttendanceInfo[i32attendanceType][i];

			S2DB::MakeQueryW(wstrSQL, L"usp_EventAttendanceReward_sel_All",
				DB_DATA_TYPE_INT, pEventInfo->m_ui32Version,
				DB_DATA_TYPE_END);
			if (FALSE == m_pDB->ExecuteSP(wstrSQL))
			{
				g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] Attendance Goods SELECT Fail");
				return FALSE;
			}

			i32GoodsCount = m_pDB->GetRecordCount();
			for (INT32 j = 0; j < i32GoodsCount; ++j)
			{
				i32Order = m_pDB->GetUINT8Data(0) - 1;
				TGoodsID = m_pDB->GetUINT32Data(1);
				if (i32Order + 1 > ATTENDANCE_MAX_CONSECUTIVE_DAY)
				{
					g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] ERROR - Attendance Order Count is Wrong");
					m_pDB->MoveNext();
					continue;
				}
				if (0 == TGoodsID)
				{
					m_pDB->MoveNext();
					continue;
				}

				pChoiceItem = &pEventInfo->m_ui32GiveGoodsID[i32Order];

				if (ATTENDANCE_CHOICE_MAX_COUNT < pChoiceItem->m_ui8GoodsCount + 1)
				{
					g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] ERROR - Attendance Goods Count is Wrong");
					m_pDB->MoveNext();
					continue;
				}

				pChoiceItem->m_ui32GoodsID[pChoiceItem->m_ui8GoodsCount] = TGoodsID;
				pChoiceItem->m_ui8GoodsCount++;

				m_pDB->MoveNext();
			}
		}
	}
	return TRUE;
}

BOOL CLoadOptionDB::_LoadMedal( )
{
	if( !g_pControl->IsContentsEnabled(CONTENTS_MEDAL) )	
	{
		g_pControl->m_MedalSystemInfo.m_bMedalEnable = FALSE;
		return TRUE;
	}

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_MedalEvent_scan",
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		return FALSE;
	}

	g_pControl->m_MedalSystemInfo.m_ui32EventMedalSetStartTime	= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(1) );
	g_pControl->m_MedalSystemInfo.m_ui32EventMedalSetEndTime	= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(2) );
	g_pControl->m_MedalSystemInfo.m_ui16EventMedalSetIdx		= m_pDB->GetUINT16Data(3);

	return TRUE;
}

BOOL CLoadOptionDB::_LoadDefaultItem( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	// Default Weapon
	CADODatabase* pDB = m_pDB;

	S2DB::MakeQueryW( wstrSQL, L"usp_DefaultItem_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32Count = pDB->GetRecordCount();
	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		g_pControl->m_TDetaultItem[ g_pControl->m_ui8DefaultItemCount ].m_ui8Type = pDB->GetUINT8Data(1);
		g_pControl->m_TDetaultItem[ g_pControl->m_ui8DefaultItemCount ].m_TItemID = pDB->GetUINT32Data(2);

		if( 0 == g_pControl->m_TDetaultItem[ g_pControl->m_ui8DefaultItemCount ].m_TItemID )
		{
			pDB->MoveNext();
			continue;
		}

		g_pControl->m_ui8DefaultItemCount++;

		if( DEFAULT_ITEM_COUNT <= g_pControl->m_ui8DefaultItemCount )
			break;

		pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadGiftBuyRanking( )  //탑5 없어지니 사용 안해도 될듯
{
	if( !g_pControl->IsContentsEnabled(CONTENTS_GIFT) )	return TRUE;

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	// Default Weapon
	CADODatabase* pDB = m_pDB;

	S2DB::MakeQueryW( wstrSQL, L"usp_ShopGiftStat_scan", 
		DB_DATA_TYPE_INT,	MAX_GIFT_RANKING_COUNT ,
		DB_DATA_TYPE_END );

	if( FALSE == pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32Count = pDB->GetRecordCount();
	i32Count = MIN( MAX_TOTAL_GIFT_RANKING_COUNT, i32Count );

	GIFT_BUY_RANKING	GiftBuyRanking;
	i3mem::FillZero( &GiftBuyRanking, sizeof( GIFT_BUY_RANKING ) );

	GiftBuyRanking.m_ui8GiftCount = (UINT8)i32Count;

	for( INT32 i = 0 ; i < i32Count ; i++ )
	{
		DATE dtExpire										=	pDB->GetDateData(0);
		GiftBuyRanking.m_aGiftRanking[i].m_ui32Date			=	S2DB::ConvertSmallDateToUINT32(dtExpire);

		GiftBuyRanking.m_aGiftRanking[i].m_ui8Rank			=	pDB->GetUINT8Data(1);
		GiftBuyRanking.m_aGiftRanking[i].m_ui32GiftID		=	pDB->GetUINT32Data(2);
		 
		pDB->MoveNext();
	}

	g_pControl->SetGiftRanking( &GiftBuyRanking );

	return TRUE;

}

BOOL CLoadOptionDB::_LoadChannelList( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	// Get Channel List
	INT32				i32ServerIdx;
	SERVER_INFO_GAME*	pServerInfo;

	S2DB::MakeQueryW( wstrSQL, L"usp_ChannelType_scan",
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
	
	INT32 i32ServerCount = m_pDB->GetRecordCount();
	for( INT32 i = 0 ; i < i32ServerCount ; i++ )
	{
		i32ServerIdx = m_pDB->GetUINT16Data( 0 );
		if( MAX_SERVER_COUNT <= i32ServerIdx ) continue;

		pServerInfo = &g_pControl->m_aGameServerList[ i32ServerIdx ];

		for( INT32 j = 0 ; j < GAME_CHANNEL_COUNT ; j++ )
		{
			pServerInfo->_EachType[ j ] = (UINT8)m_pDB->GetUINT16Data( j+1 );
		}
		pServerInfo->_ui16InitUserCount	= m_pDB->GetUINT16Data( GAME_CHANNEL_COUNT+1 );
		pServerInfo->_ui16ExtraCount	= m_pDB->GetUINT16Data( GAME_CHANNEL_COUNT+2 );
		pServerInfo->_ui16MaxUserCount	= pServerInfo->_ui16InitUserCount * GAME_CHANNEL_COUNT;

		m_pDB->MoveNext();
	}

	_MakeChannelGroup();

#ifdef USE_NEW_CHANNEL_SYS

	// 그룹에 대한 정의
	ENTRANCE_GROUP_TYPE
	// 서버에 대한 정의
	// 채널에 대한 정의

#endif
	return TRUE;
}

BOOL CLoadOptionDB::_LoadGachaSystem( )
{
 	if( !g_pControl->IsContentsEnabled(CONTENTS_GACHA) )
 		return TRUE;


	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	//////////////////////////////////////////////////////////////////////////
	// Shop Goods 및 Goods에 관련된 Item Auth 검증 코드 추가
	//////////////////////////////////////////////////////////////////////////
	S2DB::MakeQueryW( wstrSQL, L"usp_GachaMessage_scan",
		DB_DATA_TYPE_END );

	if( !m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::LoadGachaSystem] usp_LuckyItemAction_scan");
		return FALSE;
	}

	CGachaShop* pGachaShop = g_pControl->GetGachaShop();

	for( INT32 i = 0; i < GACHA_NOTICE_COUNT; ++i )
	{
		if( m_pDB->GetUINT8Data(i+GACHA_NOTICE_COUNT) == 0 )
			continue;

		TTCHAR szNotices[GACHA_NOTICE_LENGTH];
		m_pDB->GetTextData(i, szNotices, GACHA_NOTICE_LENGTH);
		pGachaShop->InsertNotice(szNotices);
	}

	S2DB::MakeQueryW( wstrSQL, L"usp_GachaBasic_sel_All",
		DB_DATA_TYPE_END );

	if( !m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] Failed to procedure call - usp_LuckyItem_scan");
		return FALSE;
	}

	INT32 i32ActiveCount = m_pDB->GetRecordCount();
	if( i32ActiveCount < 1 || i32ActiveCount > GACHA_GROUP_COUNT )
	{
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] Active Gacha GroupCount Error / Count : %d ", i32ActiveCount);
		return FALSE;
	}

	SS_GACHA_GROUP_INFO stGachaGroup[GACHA_GROUP_COUNT];
	for( INT32 i = 0; i < i32ActiveCount; ++i )
	{
		INT32 i32Column = 0;

		stGachaGroup[i].m_eGachaGroup = static_cast<GACHA_GROUP>( m_pDB->GetUINT8Data(i32Column++) - 1 );
		if( stGachaGroup[i].m_eGachaGroup < GACHA_GROUP_POOR || GACHA_GROUP_COUNT <= stGachaGroup[i].m_eGachaGroup )
		{
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] Invalid group index");
			return FALSE;
		}

		//stGachaGroup[i].m_i32GloryCount			= m_pDB->GetINT16Data(i32Column++);
		stGachaGroup[i].m_ui32LuckyGoodsID		= m_pDB->GetUINT32Data(i32Column++);
		stGachaGroup[i].m_i32LuckyProbability	= m_pDB->GetINT32Data(i32Column++);

		if( stGachaGroup[i].m_i32LuckyProbability > GACHA_MAX_LUCKY_PROBABILITY )
		{
			
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] Invalid Lucky Probability. (Probability : %f)", (1.0f / stGachaGroup[i].m_i32LuckyProbability));
#ifndef _DEBUG
			return FALSE;
#endif
		}

		g_pLog->WriteLog( L"[LoadGachaSystem] GachaGroupLoad Success / GroupIdx : %d / ", stGachaGroup[i].m_eGachaGroup );

		m_pDB->MoveNext();
	}

	for( INT32 i = 0; i < i32ActiveCount; ++i )
	{
		if( GACHA_GROUP_NONE >= stGachaGroup[i].m_eGachaGroup ) continue;
		if( GACHA_GROUP_COUNT <= stGachaGroup[i].m_eGachaGroup ) continue;

		// Win item information
		S2DB::MakeQueryW( wstrSQL, L"usp_GachaWinItem_sel_All",
			DB_DATA_TYPE_TINYINT,		stGachaGroup[i].m_eGachaGroup+1,
			DB_DATA_TYPE_END);
		if( !m_pDB->ExecuteSP(wstrSQL) )
		{
			g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::_LoadGachaSystem] usp_LuckyItemWin_sel");
			return FALSE;
		}

		INT32 i32DBWinItemCount = m_pDB->GetRecordCount();

		if( WIN_ITEM_COUNT_BY_TAB < i32DBWinItemCount )
		{
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::_LoadGachaSystem] Too many items %d", i32DBWinItemCount );
			i32DBWinItemCount = WIN_ITEM_COUNT_BY_TAB;
			//return FALSE;
		}

		stGachaGroup[i].m_i32WinItemCount = i32DBWinItemCount;

		// WinItem information 
		for( INT32 j = 0; j < i32DBWinItemCount; ++j )
		{
			INT32 i32Column	= 0;

			SS_GACHA_ITEM_INFO& stGachaItem = stGachaGroup[i].m_arWinItems[j];
			stGachaItem.m_i32ItemKey		= m_pDB->GetINT32Data(i32Column++);
			stGachaItem.m_ui32GoodsID		= m_pDB->GetUINT32Data(i32Column++);
			stGachaItem.m_i32TotalAmount	= m_pDB->GetINT16Data(i32Column++);

			for( INT32 k = 0; k < PROB_SECTION_COUNT; ++k )
				stGachaItem.m_arWinProbs[k] = m_pDB->GetINT16Data(i32Column++);

			stGachaItem.m_i32GloryCount =  m_pDB->GetINT16Data(i32Column++);

			m_pDB->MoveNext();
		}

		// Random item information by WinItem
		for( INT32 j = 0; j < i32DBWinItemCount; ++j )
		{
			SS_GACHA_ITEM_INFO& stGachaItem = stGachaGroup[i].m_arWinItems[j];
			S2DB::MakeQueryW( wstrSQL, L"usp_GachaBustNormal_sel_All",
				DB_DATA_TYPE_INT,			stGachaItem.m_i32ItemKey,
				DB_DATA_TYPE_END);
			if( !m_pDB->ExecuteSP(wstrSQL) )
			{
				g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::_LoadGachaSystem] Failed to procedure call - usp_LuckyItemBustNormal_sel");
				return FALSE;
			}

			stGachaItem.m_i32RandomItemCnt = m_pDB->GetRecordCount();
			for( INT32 k = 0; k < stGachaItem.m_i32RandomItemCnt; ++k )
			{
				INT32 i32Column = 0;

				GACHA_RANDOM_ITEM& stRandomItem = stGachaItem.m_arRandomItems[k];
				stRandomItem.m_ui32GoodsId		= m_pDB->GetUINT32Data(i32Column++);
				i32Column++;															// Random Item Name
				stRandomItem.m_i32Probability	= m_pDB->GetINT32Data(i32Column++);

				m_pDB->MoveNext();
			}
		}

		// Random item information by group
		S2DB::MakeQueryW( wstrSQL, L"usp_GachaBustRandom_sel_All",
			DB_DATA_TYPE_TINYINT,		stGachaGroup[i].m_eGachaGroup+1,
			DB_DATA_TYPE_END);
		if( !m_pDB->ExecuteSP(wstrSQL) )
		{
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] Failed to procedure call - usp_LuckyItemBustRandom_sel");
			return FALSE; 
		}

		stGachaGroup[i].m_i32RandomItemCnt = m_pDB->GetRecordCount();
		if( stGachaGroup[i].m_i32RandomItemCnt == 0 )
		{
			g_pLog->WriteLog( L"[ERROR][CLoadOptionDB::LoadGachaSystem] No random item data in group.");
			return FALSE;
		}

		for( INT32 j = 0; j < stGachaGroup[i].m_i32RandomItemCnt; ++j )
		{
			INT32 i32Column	= 0;
			stGachaGroup[i].m_arRandomGoodsIDs[j] = m_pDB->GetUINT32Data(i32Column++);

			m_pDB->MoveNext();
		}

		pGachaShop->SetGachaGroupInfo(&stGachaGroup[i]);
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadClanMatchSeason( )
{
 	if( !g_pControl->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) )
 		return TRUE;

	if( !m_pDB->ExecuteSP("usp_ClanMatchSeason_sel") )
	{
		g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::LoadClanMatchSeason] usp_ClanMatchSeason_sel.");
		return FALSE;
	}

	UINT32 ui32RecordCount = m_pDB->GetRecordCount();

	if (SEASON_TYPE_COUNT < ui32RecordCount)
	{
		g_pLog->WriteLog( L"[DB ERROR][CLoadOptionDB::LoadClanMatchSeason] Season Count Over");
		ui32RecordCount = SEASON_TYPE_COUNT;
	}

	INT32 i32DuplicateSeasonCheck = 0;
	 
	for( UINT32 i = 0; i < ui32RecordCount; ++i )
	{
		INT32 i32Column = 0;

		CLAN_MATCH_SEASON stClanMatchSeason;
		stClanMatchSeason.m_i32SeasonIndex = m_pDB->GetINT32Data(i32Column++);
		m_pDB->GetTextData(i32Column++, stClanMatchSeason.m_szSeasonName, SEASON_NAME_LENGTH);
		stClanMatchSeason.m_ui32StartDate = S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(i32Column++) );
		stClanMatchSeason.m_ui32EndDate = S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(i32Column++) );
		SEASON_TYPE eSeasonType = static_cast<SEASON_TYPE>(m_pDB->GetINT8Data(i32Column++));

		INT32 i32Flag = 1 << eSeasonType;
		if( i32DuplicateSeasonCheck & i32Flag )
		{	// 동일한 타입의 시즌이 존재합니다. 
			g_pLog->WriteLog( L"[CLoadOptionDB::LoadClanMatchSeason] Duplicate season.");
			return FALSE;
		}
		i32DuplicateSeasonCheck |= i32Flag;

		if (SEASON_TYPE_ONGOING == eSeasonType)
		{
			UINT32 ui32CurrentTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			// 현재 시즌의 날짜가 미래일 경우 처리하지 않습니다.
			if (ui32CurrentTime < stClanMatchSeason.m_ui32StartDate)
			{
				g_pLog->WriteLog(L"[CLoadOptionDB::LoadClanMatchSeason] Not yet Ongoing season.");
				m_pDB->MoveNext();
				continue;
			}

			// 현재 시즌의 날짜가 과거일 경우 이전시즌으로 셋팅 하고 종료합니다.( 등록된 다른 이전 시즌을 무시합니다. )
			if (ui32CurrentTime > stClanMatchSeason.m_ui32EndDate)
			{
				g_pLog->WriteLog(L"[CLoadOptionDB::LoadClanMatchSeason] Over Ongoing season.");

				if (!g_pControl->InsertClanMatchSeason(SEASON_TYPE_PRE, &stClanMatchSeason))
				{
					g_pLog->WriteLog(L"[CLoadOptionDB::LoadClanMatchSeason] Fail Insert season 1");
					return FALSE;
				}

				break;
			}
		}

		if (!g_pControl->InsertClanMatchSeason(eSeasonType, &stClanMatchSeason))
		{
			g_pLog->WriteLog(L"[CLoadOptionDB::LoadClanMatchSeason] Fail Insert season 2");
			return FALSE;
		}

		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadChangeBasic( )
{
	wchar_t wstrSQL[ SQL_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_ConfigSync_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
	g_pControl->m_i32NewUserPoint				= m_pDB->GetINT32Data(0);
	g_pControl->m_i32ClanCreateRank				= m_pDB->GetINT32Data(1);
	g_pControl->m_i32ClanCreatePoint			= m_pDB->GetINT32Data(2);
	g_pControl->m_ui8ClanCreateMemberCount		= m_pDB->GetUINT8Data(3);

	return TRUE;
}

BOOL CLoadOptionDB::_LoadBoostEvent( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_EventBoost_scan", 
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	// 이벤트 정보들을 받아올 변수들
	DATE32			dt32EndDate;
	BOOST_EVENT*	pBoostEvent;

	//등록되어 있는 이벤트들의 총 개수를 받아온다
	UINT16 ui16EventCount = (UINT16)m_pDB->GetRecordCount();

	g_pControl->m_ui16BoostEventCount = 0;
	for( INT32 i = 0 ; i < ui16EventCount ; ++i )
	{
		dt32EndDate		= m_pDB->GetDateData(3);
		dt32EndDate.AddTime( I3DATE_ADD_TYPE_SECOND, 59 );				// DB값이 smalldatetime 이기 때문에 59초를 더하여 마지막시간으로 변경해 줍니다.

		if( dt32EndDate < i3ThreadTimer::LocalTime() )		// 이미 종료된 이벤트라면 건너뛴다.
		{
			m_pDB->MoveNext();
			continue;
		}

		pBoostEvent = &g_pControl->m_stBoostEvent[ g_pControl->m_ui16BoostEventCount ];
		
		pBoostEvent->m_ui8LocaleCode	= m_pDB->GetUINT8Data(0);
		pBoostEvent->m_ui16Type			= m_pDB->GetINT16Data(1);
		pBoostEvent->m_dt32StartDate	= m_pDB->GetDateData(2);
		pBoostEvent->m_dt32EndDate		= dt32EndDate;
		pBoostEvent->m_ui32Exp			= m_pDB->GetINT32Data(4);
		pBoostEvent->m_ui32Point		= m_pDB->GetINT32Data(5);

		++g_pControl->m_ui16BoostEventCount;
		if( BOOST_EVENT_ALL_LOCALE_COUNT <= g_pControl->m_ui16BoostEventCount )
		{
			g_pLog->WriteLog( L"[CLoadOptionDB::LoadBoostEvent] Event Bouns Full" );
			
			break;
		}
		m_pDB->MoveNext();
	}
	return TRUE;
}

BOOL CLoadOptionDB::_LoadMACAddress( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_MacAddress_scan", 
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	g_pControl->m_ui32MacAddressListCount = m_pDB->GetRecordCount();
	if( MACADDRESS_COUNT < g_pControl->m_ui32MacAddressListCount ) g_pControl->m_ui32MacAddressListCount = MACADDRESS_COUNT;	// Max 20개로 제한
	i3mem::FillZero( g_pControl->m_aui64MacAddressList, sizeof(UINT64)*MACADDRESS_COUNT );

	UINT64	ui64Temp;
	char	strStringValue[SERVER_STRING_COUNT];
	char*	pStringValue;
	for( INT32 i = 0; i < (INT32)g_pControl->m_ui32MacAddressListCount ; i++ )
	{
		pStringValue = strStringValue;
		m_pDB->GetTextData( 0, pStringValue, SERVER_STRING_COUNT );

		for( INT32 j = 0 ; j < 6 ; j++ )
		{
			switch( *pStringValue )
			{
			case '0':			ui64Temp = 0x00; break;
			case '1':			ui64Temp = 0x10; break;
			case '2':			ui64Temp = 0x20; break;
			case '3':			ui64Temp = 0x30; break;
			case '4':			ui64Temp = 0x40; break;
			case '5':			ui64Temp = 0x50; break;
			case '6':			ui64Temp = 0x60; break;
			case '7':			ui64Temp = 0x70; break;
			case '8':			ui64Temp = 0x80; break;
			case '9':			ui64Temp = 0x90; break;
			case 'A': case 'a': ui64Temp = 0xA0; break;
			case 'B': case 'b': ui64Temp = 0xB0; break;
			case 'C': case 'c': ui64Temp = 0xC0; break;
			case 'D': case 'd': ui64Temp = 0xD0; break;
			case 'E': case 'e': ui64Temp = 0xE0; break;
			case 'F': case 'f': ui64Temp = 0xF0; break;
			default:
			{
				g_pLog->WriteLog(L"Wrong Mac Address");
			}
			return FALSE;
			break;
			}

			pStringValue++;

			switch( *pStringValue )
			{
			case '0':			ui64Temp |= 0x00; break;
			case '1':			ui64Temp |= 0x01; break;
			case '2':			ui64Temp |= 0x02; break;
			case '3':			ui64Temp |= 0x03; break;
			case '4':			ui64Temp |= 0x04; break;
			case '5':			ui64Temp |= 0x05; break;
			case '6':			ui64Temp |= 0x06; break;
			case '7':			ui64Temp |= 0x07; break;
			case '8':			ui64Temp |= 0x08; break;
			case '9':			ui64Temp |= 0x09; break;
			case 'A': case 'a': ui64Temp |= 0x0A; break;
			case 'B': case 'b': ui64Temp |= 0x0B; break;
			case 'C': case 'c': ui64Temp |= 0x0C; break;
			case 'D': case 'd': ui64Temp |= 0x0D; break;
			case 'E': case 'e': ui64Temp |= 0x0E; break;
			case 'F': case 'f': ui64Temp |= 0x0F; break;
			default:
			{
				g_pLog->WriteLog(L"Wrong Mac Address");
			}
			return FALSE;
			break;
			}

			g_pControl->m_aui64MacAddressList[i] |= ui64Temp<<(8*j);	//( 4*2 ) 4비트 * 2개씩

			pStringValue++;
			pStringValue++;								// '-' 통과..
		}
		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadNotice( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_Notice_scan", 
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32NoticeCount = m_pDB->GetRecordCount();
	if ( i32NoticeCount > NOTICE_COUNT ) i32NoticeCount = NOTICE_COUNT; // 갯수 고정
	for( INT32 i = 0 ; i < i32NoticeCount ; i++ )
	{
		g_pControl->m_aReNotice[i]._bNoticeActive		= m_pDB->GetINT8Data(0);
		g_pControl->m_aReNotice[i]._ui32NoticeReTime	= m_pDB->GetINT32Data(1) * 60;
		m_pDB->GetTextData( 2,  g_pControl->m_aReNotice[i]._strNotice, STR_NOTICE_MESS_SIZE );
		g_pControl->m_aReNotice[i]._strNotice[STR_NOTICE_MESS_SIZE - 1] = 0; 

		// 새로 BeginTime, FinishTime 컬럼 추가 된 부분 2011.07.12 안국정
		DATE dateStart	= m_pDB->GetDateData( 3 );
		g_pControl->m_aReNotice[i]._ui32BeginTime		= S2DB::ConvertSmallDateToUINT32( dateStart );
		DATE dateEnd	= m_pDB->GetDateData( 4 );
		g_pControl->m_aReNotice[i]._ui32FinishTime		= S2DB::ConvertSmallDateToUINT32( dateEnd );

		// 예외처리. 
		if( 0 == g_pControl->m_aReNotice[i]._ui32NoticeReTime ) 
		{
			g_pControl->m_aReNotice[i]._bNoticeActive = 0;
		}

		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadGeneralRankup( )
{
	if( !g_pControl->IsContentsEnabled( CONTENTS_RANKING ) ) 
	{
		return TRUE;
	}

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_GeneralRankup_scan", 
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
		
	if( 0 < m_pDB->GetRecordCount() )
	{
		g_pControl->m_i32GeneralRankStartTime	= m_pDB->GetINT32Data(0);
		for( INT32 i = 0 ; i < GENERAL_RANK_COUNT ; i++ )
		{
			g_pControl->m_i32GeneralRank[ i ]	= m_pDB->GetINT32Data(1+i);
		}
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadNewUserEvent( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_EventNewUser_scan",
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	DATE32			dt32EndDate;
	NEW_USER_EVENT*	pNewUserEvent;
	INT32 i32EventCount = m_pDB->GetRecordCount();

	g_pControl->m_ui16NewUserEventCount = 0;
	for( INT32 i = 0 ; i < i32EventCount ; i++ )
	{
		dt32EndDate = m_pDB->GetDateData(2);
		dt32EndDate.AddTime( I3DATE_ADD_TYPE_SECOND, 59 );				// DB값이 smalldatetime 이기 때문에 59초를 더하여 마지막시간으로 변경해 줍니다.

		if( dt32EndDate < i3ThreadTimer::LocalTime() ) // 이미 종료된 이벤트라면 로드 하지 않는다.
		{
			m_pDB->MoveNext();
			continue;
		}

		pNewUserEvent = &g_pControl->m_stNewUserEvent[ g_pControl->m_ui16NewUserEventCount ];

		pNewUserEvent->m_ui8LocaleCode		= m_pDB->GetUINT8Data(0);
		pNewUserEvent->m_dt32StartDate		= m_pDB->GetDateData(1);
		pNewUserEvent->m_dt32EndDate		= dt32EndDate;
		pNewUserEvent->m_ui32GoodsID		= m_pDB->GetUINT32Data(3);

		++g_pControl->m_ui16NewUserEventCount;

		if( NEW_USER_EVENT_ALL_LOCALE_COUNT<= g_pControl->m_ui16NewUserEventCount )
		{
			g_pLog->WriteLog( L"New User Event Full" );
			break;
		}
		m_pDB->MoveNext();
	}
	return TRUE;
}

BOOL CLoadOptionDB::_LoadLoginEvent( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_EventLogin_scan",
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
	
	T_GoodsID			TGoodsID;
	DATE32				dt32EndDate;
	LOGIN_USER_EVENT*	pLoginUserEvent;

	INT32 i32EventCount = m_pDB->GetRecordCount();
	g_pControl->m_ui16LoginUserEventCount = 0;
	for( INT32 i = 0 ; i < i32EventCount ; i++ )
	{
		dt32EndDate = m_pDB->GetDateData(3);
		dt32EndDate.AddTime( I3DATE_ADD_TYPE_SECOND, 59 );				// DB값이 smalldatetime 이기 때문에 59초를 더하여 마지막시간으로 변경해 줍니다.

		if( dt32EndDate < i3ThreadTimer::LocalTime() ) // 이미 종료된 이벤트라면 로드 하지 않는다.
		{
			m_pDB->MoveNext();
			continue;
		}

		pLoginUserEvent = &g_pControl->m_stLoginUserEvent[ g_pControl->m_ui16LoginUserEventCount ];

		pLoginUserEvent->m_ui8LocaleCode	= m_pDB->GetINT8Data(0);
		pLoginUserEvent->m_ui8Slot			= (UINT8)m_pDB->GetINT32Data(1);
		pLoginUserEvent->m_dt32StartDate	= m_pDB->GetDateData(2);
		pLoginUserEvent->m_dt32EndDate		= dt32EndDate;

		pLoginUserEvent->m_ui8GoodsCount	= 0;
		
		for( INT32 i32Idx = 0 ; i32Idx < EVENT_ITEM_GOODS_COUNT ; i32Idx++ )
		{
			TGoodsID = m_pDB->GetUINT32Data(4+ i32Idx );
			if( 0 == TGoodsID ) continue;
			pLoginUserEvent->m_ui32GoodsID[ pLoginUserEvent->m_ui8GoodsCount ]	= TGoodsID;
			pLoginUserEvent->m_ui8GoodsCount++;
		}
		// 등록된 GoodsID 가 없기 때문에 이벤트를 패스 합니다.
		if( 0 == pLoginUserEvent->m_ui8GoodsCount )
		{
			m_pDB->MoveNext();
			continue;
		}
		

		++g_pControl->m_ui16LoginUserEventCount;

		if( LOGIN_USER_EVENT_ALL_LOCALE_COUNT<= g_pControl->m_ui16LoginUserEventCount )
		{
			g_pLog->WriteLog( L"Login User Event Full" );
			break;
		}
		m_pDB->MoveNext();
	}
	return TRUE;
}

BOOL CLoadOptionDB::_LoadRankUpItem( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	S2DB::MakeQueryW( wstrSQL, L"usp_EventRankUp_scan", 
		DB_DATA_TYPE_END);
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32RankupCount = m_pDB->GetRecordCount();
	if ( i32RankupCount > RANK_51 ) i32RankupCount = RANK_51; // 갯수 고정

	UINT32 ui32GoodsID = 0;
	for( INT32 i = 0 ; i < i32RankupCount ; i++ )
	{
		g_pControl->m_aRankUpItem[i].m_ui8Rank					= (UINT8)(i+1); // DB값 무시합니다. pDB->GetINT8Data(0);

		UINT8	ui8Idx = 0;
		for( INT32 j = 1; j < MAX_RANKUP_ITEM_COUNT + 1; j++)
		{
			ui32GoodsID = m_pDB->GetINT32Data( j );
			if( 0 != ui32GoodsID )
			{
				g_pControl->m_aRankUpItem[i].m_ui32GoodsID[ui8Idx] = ui32GoodsID;
				ui8Idx++;
			}
		}		
		m_pDB->MoveNext();
	}
	return TRUE;
}


BOOL CLoadOptionDB::_LoadIPBlock( )
{
	if( !g_pControl->IsContentsEnabled( CONTENTS_IP_BLOCK ) ) return TRUE;

	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	// IP Block
	// Open IP
	S2DB::MakeQueryW( wstrSQL, L"usp_AllowIP_scan",
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
	
	g_pControl->m_ui8IPBlockIPCount = (UINT8)m_pDB->GetRecordCount();
	if( IPBLOCK_IP_OPEN_COUNT <= g_pControl->m_ui8IPBlockIPCount ) 
	{
		g_pLog->WriteLog( L"[WARR] IP Block IP Buffer Full Count : %d", g_pControl->m_ui8IPBlockIPCount );
		g_pControl->m_ui8IPBlockIPCount = IPBLOCK_IP_OPEN_COUNT;
	}
	char strIP[ 16 ];
	for( INT32 i = 0 ; i < g_pControl->m_ui8IPBlockIPCount ; i++ )
	{
		m_pDB->GetTextData( 0, strIP, 16 );
		g_pControl->m_stIPBlockIPOpen[i].m_ui32StartIP	= ::htonl(::inet_addr( strIP ));
		m_pDB->GetTextData( 1, strIP, 16 );
		g_pControl->m_stIPBlockIPOpen[i].m_ui32EndIP	= ::htonl(::inet_addr( strIP ));

		m_pDB->MoveNext();
	}

	// Open Nation
	S2DB::MakeQueryW( wstrSQL, L"usp_AllowNation_scan",
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}
	
	g_pControl->m_ui8IPBlockNationCount = (UINT8)m_pDB->GetRecordCount();
	if( IPBLOCK_NATION_OPEN_COUNT <= g_pControl->m_ui8IPBlockNationCount ) 
	{
		g_pLog->WriteLog( L"[WARR] IP Block Nation Buffer Full Count : %d", g_pControl->m_ui8IPBlockNationCount );
		g_pControl->m_ui8IPBlockNationCount = IPBLOCK_NATION_OPEN_COUNT;
	}
	for( INT32 i = 0 ; i < g_pControl->m_ui8IPBlockNationCount ; i++ )
	{
		g_pControl->m_ui8IPBlockNationOpen[i]  = (UINT8)m_pDB->GetUINT32Data( 0 );
		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL	CLoadOptionDB::_LoadNowChannelUserCount( )  // 채널당 유저 MAX치가 변경되었을때 불러진다
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	INT32				i32ServerIdx;
	SERVER_INFO_GAME*	pServerInfo;

	S2DB::MakeQueryW( wstrSQL, L"usp_ChannelType_scan_UserCount",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32ServerCount = m_pDB->GetRecordCount();
	for( INT32 i = 0 ; i < i32ServerCount ; i++ )
	{
		i32ServerIdx = m_pDB->GetUINT16Data( 0 );
		if( MAX_SERVER_COUNT <= i32ServerIdx ) continue;

		pServerInfo = &g_pControl->m_aGameServerList[ i32ServerIdx ];

		pServerInfo->_ui16NowUserCount = m_pDB->GetUINT16Data( 1 );

		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL	CLoadOptionDB::_LoadTagPriority( ) // 상점 태그 우선순위 불러오기
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
		
	S2DB::MakeQueryW( wstrSQL, L"usp_ShopTagPriority_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDB( wstrSQL );
		return FALSE;
	}

	INT32 i32TagCount = m_pDB->GetRecordCount();

	if( i32TagCount > 32 )
	{
		g_pLog->WriteLog( L"[WARR] ShopTagCount Error : %d", i32TagCount);
		i32TagCount = 32;
	}
	g_pControl->m_ui8TagCount = (UINT8)i32TagCount;
	for( INT32 i = 0 ; i < i32TagCount ; i++ )
	{
		g_pControl->m_ui8TagPriority[i]= m_pDB->GetUINT8Data( 0 );
		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL	CLoadOptionDB::_LoadVoteLimit( )	// Vote 제한 값 불러오기
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_Vote_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] Vote SELECT Fail" );
		ErrorDB( wstrSQL );
		return FALSE;
	}

	// 로우 없으면 리턴.
	INT32 i32VoteLimitCount = m_pDB->GetRecordCount();
	if( 1 > i32VoteLimitCount )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] Vote No Records" );
		return FALSE;
	}

	g_pControl->m_vlVoteLimit.m_ui8MaxVoteCount			= m_pDB->GetUINT8Data(0);
	g_pControl->m_vlVoteLimit.m_ui8MinRank				= m_pDB->GetUINT8Data(1);
	g_pControl->m_vlVoteLimit.m_ui32ReportGuardTime		= m_pDB->GetUINT32Data(2);


	return TRUE;
}	
BOOL	CLoadOptionDB::_LoadGameNotice( )
{
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	S2DB::MakeQueryW(wstrSQL, L"usp_LobbyNotice_scan",
		DB_DATA_TYPE_END);

	if (!m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[DB Error][CLoadOptionDB::FixLoadDB] usp_LobbyNotice_scan");
		ErrorDB(wstrSQL);
		return FALSE;
	}

	INT32 NoticeCount = m_pDB->GetRecordCount();
	if (1 > NoticeCount)
	{
		g_pLog->WriteLog(L"[DB Error][CLoadOptionDB::FixLoadDB] usp_LobbyNotice_scan");
		return FALSE;
	}

	for (INT32 i = 0; i < NoticeCount; ++i)
	{
		INT32 type = m_pDB->GetINT32Data(0);
		switch (type)
		{
		case NOTICE_TYPE_SLIDING:
		{
			m_pDB->GetTextData(1, g_pControl->m_strSlidingNotice, NOTICE_STRING_COUNT);
			g_pControl->m_i32SlidingRGB = m_pDB->GetINT32Data(2);
		}
		break;
		case NOTICE_TYPE_CHAT:
		{
			m_pDB->GetTextData(1, g_pControl->m_strChatNotice, NOTICE_STRING_COUNT);
			g_pControl->m_i32ChatRGB = m_pDB->GetINT32Data(2);
		}
		break;
		default:
			return FALSE;
			break;
		}
		m_pDB->MoveNext();
	}
	return TRUE;
}

BOOL	CLoadOptionDB::_LoadChatHack( )
{
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];

	S2DB::MakeQueryW( wstrSQL, L"usp_ChatHack_scan",
		DB_DATA_TYPE_END );

	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] Vote SELECT Fail" );
		ErrorDB( wstrSQL );
		return FALSE;
	}


	INT32	i32ItemCount = m_pDB->GetRecordCount();

	if( i32ItemCount == 0 )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] ChatHack Row 0" );
		return FALSE;
	}

	if( i32ItemCount > 1 )
	{
		g_pLog->WriteLog( L"[CLoadOptionDB::OnCreate] ChatHack Row 1 over" );
	}

	g_pControl->m_chlChatHackLimit.m_ui8Size		= m_pDB->GetUINT8Data(0);
	g_pControl->m_chlChatHackLimit.m_ui8Count		= m_pDB->GetUINT8Data(1);
	g_pControl->m_chlChatHackLimit.m_ui8Time		= m_pDB->GetUINT8Data(2);
	g_pControl->m_chlChatHackLimit.m_ui32LimitTime	= m_pDB->GetUINT32Data(3);

	return TRUE;
}

BOOL	CLoadOptionDB::_LoadBillingURL()
{
	wchar_t wstrSQL[SERVER_STRING_COUNT];

	//BillingURLList
	S2DB::MakeQueryW(wstrSQL, L"usp_BillingURLList_scan",
		DB_DATA_TYPE_END);
	if (FALSE == m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] LoadBillingURL Select Fail");
		ErrorDB(wstrSQL);
		return FALSE;
	}

	g_pControl->m_ui8BillingUrlCount = static_cast<UINT8>(m_pDB->GetRecordCount());
	
	for (INT32 i = 0; i < g_pControl->m_ui8BillingUrlCount; i++)
	{
		INT32 i32LinkMethod = m_pDB->GetINT32Data(0);

		if (i32LinkMethod >= 0 && i32LinkMethod < LINK_METHOD_MAX)
		{
			m_pDB->GetTextData(1, g_pControl->m_astrBillingURL[i32LinkMethod], MAX_BILLING_URL);
			
		}
		
		m_pDB->MoveNext();
	}

	return TRUE;
}

BOOL CLoadOptionDB::_LoadURLList()
{
	wchar_t wstrSQL[SERVER_STRING_COUNT];

	g_pControl->m_ui8URLListCount = 0;
	i3mem::FillZero(g_pControl->m_ulURLList, sizeof(URL_LIST) * URL_MAX_COUNT);

	//URLList
	S2DB::MakeQueryW(wstrSQL, L"usp_URLList_scan",
		DB_DATA_TYPE_END);
	if (FALSE == m_pDB->ExecuteSP(wstrSQL))
	{
		g_pLog->WriteLog(L"[CLoadOptionDB::OnCreate] LoadURLList Select Fail");
		g_pControl->m_ui8URLListCount = 0;	// 실패 시 카운트 0으로 무시하고 진행합니다.
		return TRUE;
	}

	UINT8		ui8Count		= static_cast<UINT8>(m_pDB->GetRecordCount());
	UINT8		ui8RealCount	= 0;
	URL_LIST	*pulURLList;

	for (INT32 i = 0; i < ui8Count; i++)
	{
		INT32 i32Type = m_pDB->GetINT32Data(0);

		if (i32Type >= 0 && i32Type < URL_TYPE_MAX)
		{
			pulURLList = &g_pControl->m_ulURLList[ui8RealCount];
			
			pulURLList->m_i32Type = i32Type;
			pulURLList->m_i32Data = m_pDB->GetINT32Data(1);
			m_pDB->GetTextData(2, pulURLList->m_strURL, URL_STRING_MAX_COUNT);

			ui8RealCount++;

			if (URL_MAX_COUNT == ui8RealCount) break;
		}

		m_pDB->MoveNext();
	}

	g_pControl->m_ui8URLListCount = ui8RealCount;

	if (INIT_UINT8 > g_pControl->m_ui8URLVersion)
		++g_pControl->m_ui8URLVersion;
	else
		g_pControl->m_ui8URLVersion = 0;

	return TRUE;
}

void CLoadOptionDB::_MakeChannelGroup()
{
	INT32 i32TempValue;
	for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
	{
		for( UINT32 j = 0; j < GAME_CHANNEL_COUNT; j++ )
		{
			switch( g_pControl->m_aGameServerList[i]._EachType[j] )
			{
			case CHANNEL_TYPE_NONE			: 
			case CHANNEL_TYPE_NORMAL		:
				i32TempValue = S_CHANNEL_TYPE_NORMAL;
				break; 
			case CHANNEL_TYPE_NORMAL2		:
				i32TempValue = S_CHANNEL_TYPE_NORMAL2;
				break;
			case CHANNEL_TYPE_BEGIN1		: 
			case CHANNEL_TYPE_BEGIN2		:
				i32TempValue = S_CHANNEL_TYPE_BEGIN;
				break; 
			case CHANNEL_TYPE_CLAN 			:
				i32TempValue = S_CHANNEL_TYPE_CLAN;
				break; 
			case CHANNEL_TYPE_EXPERT		: 
			case CHANNEL_TYPE_EXPERT2		:
				i32TempValue = S_CHANNEL_TYPE_EXPERT;
				break; 
			case CHANNEL_TYPE_CHAMPIONSHIP	:
				i32TempValue = S_CHANNEL_TYPE_CHAMPIONSHIP;
				break; 
			case CHANNEL_TYPE_NATIONAL		:
				i32TempValue = S_CHANNEL_TYPE_NATIONAL;
				break;
			case CHANNEL_TYPE_MIDDLE		:
				i32TempValue = S_CHANNEL_TYPE_CONBINATION;
				break;
			case CHANNEL_TYPE_AZERBAIJAN	:
				i32TempValue = S_CHANNEL_TYPE_AZERBAIJAN;
				break;
			default							:
				g_pControl->m_aGameServerList[i]._EachType[j] = CHANNEL_TYPE_NORMAL; 
				i32TempValue = S_CHANNEL_TYPE_NORMAL;
				break;
			}

			if( j == 0)	
			{
				g_pControl->m_aGameServerList[i]._AllType = (UINT8)i32TempValue; 
			}
			else 
			{
				if( g_pControl->m_aGameServerList[i]._AllType != i32TempValue)
				{
					g_pControl->m_aGameServerList[i]._AllType = S_CHANNEL_TYPE_CONBINATION;
					break;
				}
			}
		}
	}
}

INT32 CLoadOptionDB::UserLogIn(INT32 iSessionIdx, wchar_t *strID, wchar_t *strPass)
{
	IN_DB_SUPER_USER * pLogIn = (IN_DB_SUPER_USER *)m_pInLogIN->PushPointer(); 
	if(pLogIn == NULL)return LOCAL_RINGBUFFER_ERROR_BUFFER_FULL;
	
	pLogIn->iSessionIdx = iSessionIdx;
	i3String::Copy( pLogIn->wstrID,		strID,		NET_ID_SIZE );
	i3String::Copy( pLogIn->wstrPass,	strPass,	NET_PW_SIZE );

	m_pInLogIN->PushPointerIdx();
	return LOCAL_RINGBUFFER_SUCCESS;
}

void CLoadOptionDB::InitRandomMapIdx(UINT16& ui16StartIdx, UINT16 ui16EndIdx, UINT8& ui8CurrentRule, UINT8& ui8CurrentMaxPerson)
{
	if ( (0 == ui8CurrentMaxPerson && 0 == ui8CurrentRule)
		|| 0 == ui16EndIdx)
	{
		ui8CurrentRule = g_pControl->m_aRandomMapData[ui16EndIdx].m_ui8RuleIdx;
		ui8CurrentMaxPerson = g_pControl->m_aRandomMapData[ui16EndIdx].m_ui8MaxPerson;
		return;
	}

	switch (ui8CurrentMaxPerson)
	{
	case RANDOM_MAX_PERSON16:
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aStartIdx[TYPE_RANDOM_MAX_PERSON16] = ui16StartIdx;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aEndIdx[TYPE_RANDOM_MAX_PERSON16] = ui16EndIdx - 1;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag = g_addFlagByEnum<RANDOM_MAX_PERSON_TYPE>(g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON16);
		break;
	case RANDOM_MAX_PERSON10:
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aStartIdx[TYPE_RANDOM_MAX_PERSON10] = ui16StartIdx;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aEndIdx[TYPE_RANDOM_MAX_PERSON10] = ui16EndIdx - 1;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag = g_addFlagByEnum<RANDOM_MAX_PERSON_TYPE>(g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON10);
		break;
	case RANDOM_MAX_PERSON8:
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aStartIdx[TYPE_RANDOM_MAX_PERSON8] = ui16StartIdx;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_aEndIdx[TYPE_RANDOM_MAX_PERSON8] = ui16EndIdx - 1;
		g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag = g_addFlagByEnum<RANDOM_MAX_PERSON_TYPE>(g_pControl->m_aRandomMapIdxData[ui8CurrentRule].m_ui8MaxPersonFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON8);
		break;
	}

	ui8CurrentMaxPerson = g_pControl->m_aRandomMapData[ui16EndIdx].m_ui8MaxPerson;
	ui8CurrentRule = g_pControl->m_aRandomMapData[ui16EndIdx].m_ui8RuleIdx;
	ui16StartIdx = ui16EndIdx;
}