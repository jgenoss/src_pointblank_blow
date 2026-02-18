#include "pch.h"
#include "ModuleDBUserLoad.h"

struct Thread_Paramater
{
	void*	m_pClass;
	INT32	m_i32Idx;
};

I3_CLASS_INSTANCE( CModuleDBUserLoad, i3ElementBase);

static UINT32 _DefThreadProc2( void * pPointer )
{
	Thread_Paramater* pParameter	= (Thread_Paramater*)pPointer;
	CModuleDBUserLoad* pClass		= (CModuleDBUserLoad*)pParameter->m_pClass;
	INT32 i32ThreadIdx				= pParameter->m_i32Idx;
	I3_SAFE_DELETE( pParameter );

	pClass->OnWorking( i32ThreadIdx ); 
	return 0; 
}

CModuleDBUserLoad::CModuleDBUserLoad(void)
{
	i3mem::FillZero( m_ppADO,				sizeof(CADODatabase	*)		* DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_phThread,			sizeof(HANDLE)				* DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_pbIsRunning,			sizeof(BOOL)				* DATABASE_THREAD_COUNT); 

	i3mem::FillZero( m_ppCommandIn_LoadInfo,	sizeof(Local_CRingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppCommandOut_LoadInfo,	sizeof(Local_CRingBuffer *) * DATABASE_THREAD_COUNT);

	m_i32LoadInfoOutIdx	= 0;
}

CModuleDBUserLoad::~CModuleDBUserLoad(void)
{
	OnDestroy(); 
}

BOOL CModuleDBUserLoad::OnCreate()
{
	DWORD dwThreadID; 
		
	//--------------------------------------------------------------------------------------------------------
	//For User Info Save 
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		char szDBName[DB_STRING_LENGTH] = {0, };
		i3String::Format(szDBName, DB_STRING_LENGTH, "PBUser%02d", i);
		m_pUserDBConfig[i] = g_pConfig->GetDatabaseConfig(szDBName);
		if( !m_pUserDBConfig[i] )
			return FALSE;

		// Load
		m_ppADO[i] = CADODatabase::NewObject();
		if( !m_ppADO[i]->Connect(m_pUserDBConfig[i]->GetDataSource(), m_pUserDBConfig[i]->GetCatalog(), m_pUserDBConfig[i]->GetAccount(), m_pUserDBConfig[i]->GetPassword()) )
			return FALSE;

		//Make Buffer LogIn
		m_ppCommandIn_LoadInfo[i] = new Local_CRingBuffer; 
		if( !m_ppCommandIn_LoadInfo[i]->OnCreate( sizeof(LOAD_USER_INFO_IN), DB_LOADINFO_COUNT ) )return FALSE;  
		m_ppCommandOut_LoadInfo[i]= new Local_CRingBuffer; 
		if( !m_ppCommandOut_LoadInfo[i]->OnCreate( sizeof(LOAD_USER_INFO_OUT), DB_LOADINFO_COUNT ) )return FALSE; 

		Thread_Paramater* pParameter = new Thread_Paramater;
		pParameter->m_pClass	= this;
		pParameter->m_i32Idx	= i;

		// Load Thread
		m_phThread[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc2 , pParameter ,0 ,&dwThreadID);
		if(m_phThread[i] == NULL)return FALSE; 
		
		//Set Thread Priority
		if( !SetThreadPriority( m_phThread[i], THREAD_PRIORITY_NORMAL) )return FALSE;

		m_i32LoginCount[i] = 0;
		m_dwTempTime[i] = GetTickCount();
	}
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER USERINFO"  );


	return TRUE; 
}

void CModuleDBUserLoad::OnDestroy(void)
{
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_pbIsRunning[i]	= FALSE;
	}

	::WaitForMultipleObjects( DATABASE_THREAD_COUNT, m_phThread, TRUE, INFINITE );

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_ppADO[i] ); 
		I3_SAFE_DELETE( m_ppCommandIn_LoadInfo[i] ); 
		I3_SAFE_DELETE( m_ppCommandOut_LoadInfo[i] ); 
	}

	return; 
}

void CModuleDBUserLoad::OnWorking( INT32 i32ThreadIdx )
{
	BOOL					bWorked; 

	LOAD_USER_INFO_IN*		pLoadInfoIn; 
	LOAD_USER_INFO_OUT*		pLoadInfoOut;

	wchar_t wstrLog[MAX_PATH];
	i3String::Format(wstrLog, MAX_PATH, L"LD%02d", i32ThreadIdx);
	m_pi32LogIdx[i32ThreadIdx] = g_pLog->AddLogThread(wstrLog, 0, GetCurrentThreadId());

	m_pbIsRunning[i32ThreadIdx] = TRUE;

	while( m_pbIsRunning[i32ThreadIdx] )
	{
		bWorked = FALSE; 

		//1. Auth (LogIn, LogOut)
		if(m_ppCommandIn_LoadInfo[i32ThreadIdx]->GetBufferCount() > 0)
		{
			bWorked			= TRUE; 
			pLoadInfoIn		= (LOAD_USER_INFO_IN *)m_ppCommandIn_LoadInfo[i32ThreadIdx]->Pop();
			pLoadInfoOut	= (LOAD_USER_INFO_OUT *)m_ppCommandOut_LoadInfo[i32ThreadIdx]->PushPointer(); 
			if( (pLoadInfoIn == NULL) || (pLoadInfoOut == NULL) )
			{
				g_pLog->WriteLog( L"[WARRING] CModuleDBUserLoad Buffer Not Empty" );
				goto ERROR_LOAD_INFO_BUFFER; 
			}

			LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
			QueryPerformanceFrequency(&stQP_Freq_Begin);
			QueryPerformanceCounter(&stQP_Count_Begin);

			pLoadInfoOut->i32Rv		= EVENT_ERROR_SUCCESS; 
			pLoadInfoOut->pNode		= pLoadInfoIn->pNode; 
			if( FALSE == _LoadUserInfo( i32ThreadIdx, pLoadInfoOut ) )
			{
				pLoadInfoOut->i32Rv = EVENT_ERROR_DATABASE; 
			}			

			QueryPerformanceCounter(&stQP_Count_End);
			stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

			REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

			g_pStatistics->AddStat(STAT_TYPE_DB_LOGIN, i32ThreadIdx, r64ElepseTime);

			if (r64ElepseTime > CHECK_TIMEOUT )
			{	// DB TIMEOVER
				g_pLog->WriteLog( L"[ModuleDBWorker] DB Time Over (Sec) : %.6lf / UID : %I64d ", r64ElepseTime, pLoadInfoOut->pNode->_i64UID );
			}

			m_ppCommandIn_LoadInfo[i32ThreadIdx]->PopIdx();
			m_ppCommandOut_LoadInfo[i32ThreadIdx]->PushPointerIdx();
		}
ERROR_LOAD_INFO_BUFFER :

		if( FALSE == bWorked ) ::WaitForSingleObject( m_phThread[i32ThreadIdx], 1 );
	}

	return; 
}

INT32 CModuleDBUserLoad::PushLoadInfo( USER_NODE * pNode )
{
	LOAD_USER_INFO_IN * pLoadIn; 
	UINT32 ui32WriteIdx = (UINT32)(pNode->_i64UID % DATABASE_THREAD_COUNT); 
	pLoadIn = (LOAD_USER_INFO_IN*)m_ppCommandIn_LoadInfo[ ui32WriteIdx ]->PushPointer(); 
	if(NULL == pLoadIn )	return EVENT_RETURN_ERROR_BUFFER; 

	pNode->_ui32State	= USER_STATE_INFO_LOAD;			// Info Load 상태로 전환

	pLoadIn->pNode		= pNode;
	m_ppCommandIn_LoadInfo[ ui32WriteIdx ]->PushPointerIdx(); 

	return EVENT_RETURN_OK; 
}

INT32 CModuleDBUserLoad::GetBufferCount(void)
{
	INT32 i32Rv = 0; 
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Rv += m_ppCommandOut_LoadInfo[i]->GetBufferCount(); 
	}
	return i32Rv; 
}

LOAD_USER_INFO_OUT * CModuleDBUserLoad::OutGetLoadInfo(void)
{
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_i32LoadInfoOutIdx++; 
		if(m_i32LoadInfoOutIdx >= DATABASE_THREAD_COUNT)m_i32LoadInfoOutIdx = 0; 

		if(m_ppCommandOut_LoadInfo[m_i32LoadInfoOutIdx]->GetBufferCount())
		{
			return (LOAD_USER_INFO_OUT *)m_ppCommandOut_LoadInfo[m_i32LoadInfoOutIdx]->Pop(); 
		}
	}

	return NULL; 
}

void CModuleDBUserLoad::OutGetLoadInfoIdx(void)
{
	m_ppCommandOut_LoadInfo[ m_i32LoadInfoOutIdx ]->PopIdx(); 
}

BOOL CModuleDBUserLoad::_LoadUserInfo( INT32 i32ThreadIdx, LOAD_USER_INFO_OUT * pOut )
{
	INT32 i32ErrorCode;
	wchar_t wstrSQL[ SERVER_STRING_COUNT ];
	CADODatabase * pADO = m_ppADO[ i32ThreadIdx ];

	USER_NODE * pNode = pOut->pNode;
	if( USER_STATE_INFO_LOAD != pNode->_ui32State )
	{	// Load 가 아닐 경우 -- 이 경우는 발생 할 수 없습니다.
		g_pLog->WriteLog( L"[ERROR] _LoadUserInfo STATE Error Thread=%d, iUID=%I64d, State=%d", i32ThreadIdx, pNode->_i64UID, pNode->_ui32State );		
		return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 기본 유저 정보 로드
	S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel", 
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_END );

	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{						
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}

	if( 0 == pADO->GetRecordCount() ) 
	{
		// 실패이긴 하지만 아직 계정이 생성 되지 않았습니다.
		pNode->m_ui8DBLoadState = USER_DB_LOAD_STATE_NONE;
		return TRUE;
	}
	else
	{
		pNode->m_ui8DBLoadState = USER_DB_LOAD_STATE_LOAD;
		 
		pADO->GetTextData( 0, pNode->_sBasic.m_strNick, NET_NICK_NAME_SIZE );												//	  strUNickname		
		pNode->_sBasic.m_ui32Exp						= pADO->GetUINT32Data( 1 );											//,   i32EXP
		INT32 i32Point = pADO->GetINT32Data( 2 );																			//,   i32Point 
		if( i32Point < 0 ) 
		{
			pNode->_sBasic.m_ui32Point = 0;
		}
		else pNode->_sBasic.m_ui32Point = i32Point;
		pNode->_sBasic.m_ui32Rank						= pADO->GetUINT32Data( 3 );											//,   i32RANK
		if( pNode->_sBasic.m_ui32Rank >= RANK_MAX ) 
		{
			pNode->_sBasic.m_ui32Rank = 0;
		}
		pNode->_ui32CreateDate							= S2DB::ConvertSmallDateToUINT32( pADO->GetDateData( 4 ) );			//,   sdtCreat
		pNode->_ui32TotalConnectCount					= pADO->GetUINT32Data( 5 );											//,	  i32ConnectCNT
		pNode->_ui32TotalConnectTime					= pADO->GetUINT32Data( 6 );											//,   i32ConnectTime
		pNode->m_dt32LastLogInDate						= pADO->GetDateData( 7 );											//,   sdtLogin
		pNode->m_dt32LastLogoutDate						= pADO->GetDateData( 8 );											//,   sdtLogout
		pNode->_sBasic.m_ui32KeepRank					= pADO->GetUINT32Data( 9 );											//,   i32KeepRank
		pNode->_ui8ESportIDFlag							= pADO->GetUINT8Data( 10 );											//,   ui8ESportLevel
		pNode->_sBasic.m_ui32AbusingCount				= pADO->GetUINT32Data( 11 );										//,   i32AbusingCNT
		pNode->m_stUserInfoServer.m_dt32HackChatBlockDate	= pADO->GetDateData( 12 );	//,   sdtEventItem
		pNode->_sBasic.m_ui8TutorialIng					= pADO->GetUINT8Data( 13 );											//,   ui8Tutorialing
		pNode->m_stUserInfoServer.m_ui32TotalBattleTime	= pADO->GetUINT32Data( 14 );										//,   i32TotalBattleTime
		pNode->m_stUserInfoServer.m_ui32LastBuyCash		= S2DB::ConvertSmallDateToUINT32( pADO->GetDateData( 15 ) );		//,   sdtLastBuyCash
		pNode->_sBasic.m_ui64GuideComplete				= pADO->GetUINT64Data( 16 );										//,	  ui64GuideComplete
		pNode->m_stUserInfoServer.m_ui8ReportCount		= pADO->GetUINT8Data( 17 );											//,   ui8ReportCount
		pNode->_sClan._ui32Mark							= pADO->GetUINT32Data( 18 );										//,   ui32ClanMark
		pADO->GetTextData( 19, pNode->_sClan._strClanName, NET_CLAN_NAME_SIZE );											//,	  strClanName	
		pNode->m_ui16AASAccMin							= pADO->GetUINT16Data( 20 );										//,   ui16AASAccSec
		pNode->_sBasic.m_ui8AuthLevel					= pADO->GetUINT8Data( 21 );											//,   AUTH_LEVEL
		pNode->_sBasic.m_ui8SpecialAbility				= pADO->GetUINT8Data( 22 );											//,   SPECIAL_ABILITY
		pNode->m_stUserInfoServer.m_ui32ChatBlockDate	= S2DB::ConvertSmallDateToUINT32( pADO->GetDateData( 23 ) );		//,	  sdtChatBlockDate
		pNode->_sBasic.m_ui32Coin						= pADO->GetUINT32Data( 24 );										//,   ui32Coin
		pNode->_sClan._ui32ClanRankingDate				= S2DB::ConvertSmallDateToUINT32( pADO->GetDateData( 25 ) );		//,	  sdtClanRankingDate
		pNode->m_stUserInfoServer.m_ui8VoteCount		= pADO->GetUINT8Data( 26 );											//,   VoteCount
	}
		
	S2DB::MakeQueryW( wstrSQL, L"usp_EventLoginState_sel", 
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_END );

	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{						
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}
	
	INT32 i32EventCount = pADO->GetRecordCount();
	for( INT32 i = 0 ; i < i32EventCount ; i++ )
	{
		INT32 i32Type = pADO->GetINT32Data( 0 );
		if( EVENTITEM_LOGIN_SLOT_COUNT <= i32Type ) continue;

		pNode->_dt32EventItemDate[ i32Type ]			= pADO->GetDateData( 1 );		//,   sdtEventItem

		pADO->MoveNext();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 출석체크 이벤트 정보 로드

	for( INT32 i32Type = 0; i32Type < ATTENDANCE_TYPE_MAX; i32Type++ )
	{
		S2DB::MakeQueryW(wstrSQL, L"usp_UAttendanceUser_sel_Dormant",
			DB_DATA_TYPE_BIGINT, pNode->_i64UID,
			DB_DATA_TYPE_TINYINT, i32Type,
			DB_DATA_TYPE_END);

		if (FALSE == pADO->ExecuteSP(wstrSQL))
		{
			ErrorDBUserLoad(i32ThreadIdx, wstrSQL);
			return FALSE;
		}
		pNode->_Attendance[ i32Type ].m_ui32Version				= pADO->GetUINT32Data(0);
		pNode->_Attendance[ i32Type ].m_ui8AttendanceDay		= pADO->GetUINT8Data(1);
		pNode->_Attendance[ i32Type ].m_ui8AttendanceItem		= pADO->GetUINT8Data(2);

		//pNode->_Attendance[ i32Type ].m_dt32AttendanceTime		= pADO->GetUINT32Data(3);

		INT32 i32Date = pADO->GetUINT32Data(3);
					
		if (i32Date > 0)
		{
			SYSTEMTIME Converttime;
			i3mem::FillZero( &Converttime, sizeof(SYSTEMTIME) );

			Converttime.wYear		= (i32Date / 100000000) + 2000;
			Converttime.wMonth		= ((i32Date % 100000000) / 1000000);
			Converttime.wDay		= ((i32Date % 1000000) / 10000);
			pNode->_Attendance[i32Type].m_dt32AttendanceTime = Converttime;
		}
		else
		{
			pNode->_Attendance[i32Type].m_dt32AttendanceTime = pADO->GetUINT32Data(3);
		}	

		pNode->_Attendance[ i32Type ].m_ui8ItemNum				= pADO->GetUINT8Data(4);
		pNode->_Attendance[ i32Type ].m_ui8AttendanceType		= pADO->GetUINT8Data(5);
		pNode->_Attendance[ i32Type ].m_bAttendanceTarget		= pADO->GetboolData(6);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 유저 전적 정보 로드
	S2DB::MakeQueryW( wstrSQL, L"usp_Record_sel", 
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @iUSN_          BIGINT
		DB_DATA_TYPE_END );

	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}
	if( 0 < pADO->GetRecordCount() )
	{
		INT8 i8RecordType = pADO->GetINT8Data(0);
		switch( i8RecordType )
		{
		case USER_RECORD_TYPE_SEASON:
			{
				pNode->_sRecord._smatch			=	pADO->GetINT32Data(1);			//,   i32Match
				pNode->_sRecord._swin			=	pADO->GetINT32Data(2);			//,   i32Win
				pNode->_sRecord._slose			=	pADO->GetINT32Data(3);			//,   i32Lose
				pNode->_sRecord._sdraw			=	pADO->GetINT32Data(4);			//,   i32Draw
				pNode->_sRecord._skillcount		=	pADO->GetINT32Data(5);			//,   i32Kill
				pNode->_sRecord._sdeath			=	pADO->GetINT32Data(6);			//,   i32Death
				pNode->_sRecord._sheadshot		=	pADO->GetINT32Data(7);			//,   i32Headshot
				pNode->_sRecord._sdis			=	pADO->GetINT32Data(8);			//,   i32Disconnect
				pNode->_sRecord._sdmatch		=	pADO->GetINT32Data(9);			//,   i32dMatch
				pNode->_sRecord._sdkillcount	=	pADO->GetINT32Data(10);			//,   i32dKill
				pNode->_sRecord._sAssistCount	=	pADO->GetINT32Data(11);				//,   i32Assist	
			}
			break;
		case USER_RECORD_TYPE_TOTAL:
			{
				pNode->_sRecord._match			=	pADO->GetINT32Data(1);			//,   i32Match
				pNode->_sRecord._win			=	pADO->GetINT32Data(2);			//,   i32Win
				pNode->_sRecord._lose			=	pADO->GetINT32Data(3);			//,   i32Lose
				pNode->_sRecord._draw			=	pADO->GetINT32Data(4);			//,   i32Draw
				pNode->_sRecord._killcount		=	pADO->GetINT32Data(5);			//,   i32Kill
				pNode->_sRecord._death			=	pADO->GetINT32Data(6);			//,   i32Death
				pNode->_sRecord._headshot		=	pADO->GetINT32Data(7);			//,   i32Headshot
				pNode->_sRecord._dis			=	pADO->GetINT32Data(8);			//,   i32Disconnect
				pNode->_sRecord._dmatch			=	pADO->GetINT32Data(9);			//,   i32dMatch
				pNode->_sRecord._dkillcount		=	pADO->GetINT32Data(10);			//,   i32dKill
				pNode->_sRecord._AssistCount	=	pADO->GetINT32Data(11);			//,   i32Assist	
			}
			break;
		}
		pADO->MoveNext();
	}

	//pNode->_sRecord._match			+= pNode->_sRecord._smatch;
	//pNode->_sRecord._win			+= pNode->_sRecord._swin;
	//pNode->_sRecord._lose			+= pNode->_sRecord._slose;
	//pNode->_sRecord._draw			+= pNode->_sRecord._sdraw;
	//pNode->_sRecord._killcount		+= pNode->_sRecord._skillcount;
	//pNode->_sRecord._death			+= pNode->_sRecord._sdeath;
	//pNode->_sRecord._dis			+= pNode->_sRecord._sdis;
	//pNode->_sRecord._headshot		+= pNode->_sRecord._sheadshot;
	//pNode->_sRecord._dmatch			+= pNode->_sRecord._sdmatch;
	//pNode->_sRecord._dkillcount		+= pNode->_sRecord._sdkillcount;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 유저 Common 장착 정보
	UINT8 ui8RedTeamSlot = 0;
	UINT8 ui8BlueTeamSlot = 1;
	// Common Load
	S2DB::MakeQueryW( wstrSQL, L"usp_Equipment_scan", 
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_END );
	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}
	if( 0 < pADO->GetRecordCount() )
	{
		pNode->m_MultiSlot.SetCommonEquipment( CHAR_EQUIPMENT_COMMON_DINO, pADO->GetUINT32Data(1), 0 );		//, i32DinoIDX,
		ui8RedTeamSlot	= pADO->GetINT8Data(2);	// v1.5는 메인 슬롯, v1.0은 레드팀 슬롯
		ui8BlueTeamSlot = pADO->GetINT8Data(3);	// v1.0은 블루팀 슬롯

		//ui8BlueTeamSlot = pNode->m_pDesign->GetBlueTeamSlot(ui8BlueTeamSlot);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// char 정보 로드
	S2DB::MakeQueryW( wstrSQL, L"usp_Character_scan", 
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_END );
	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}
	INT32 i32CharCount = pADO->GetRecordCount();

	INT32					i32SlotIdx;
	INT32					i32Skilldata;
	UINT8					ui8Enable;
	S2MO_CHAR_BASE_INFO		CharBaseInfo;
	S2MO_CHAR_SKILL_INFO	CharSkillInfo;

	for( INT32 i = 0 ; i < i32CharCount ; i++ )
	{
		CharSkillInfo.Reset();

		i32SlotIdx							= pADO->GetINT8Data(1);														// ui8CharaSlot

		// WeaponDBIdx, CharaDBIdx
		CharBaseInfo.m_i32WeaponDBIdx		= pADO->GetINT32Data(2);													// WeaponDBIdx
		

		//CharBaseInfo.m_TItemID				= pADO->GetINT32Data(1);													// i32CharaID
		CharSkillInfo.m_ui8Class			= pADO->GetINT8Data(4);														// ui8Class
		pADO->GetTextData( 5, CharBaseInfo.m_strCharName, NET_NICK_NAME_SIZE );									// strCNickname

		CharBaseInfo.m_ui32BattleTime		= pADO->GetINT32Data(15);													// i32BattleTime	<<- 뒷쪽에 있는것을 끌어왔음
		CharBaseInfo.m_ui32SkillRelTime		= pADO->GetINT32Data(19);
		CharBaseInfo.m_ui32CreateDate		= S2DB::ConvertSmallDateToUINT32( pADO->GetDateData(21) );					// sdtCreate		<<- 뒷쪽에 있는것을 끌어왔음

		ui8Enable							= pADO->GetUINT8Data(20);

		pNode->m_MultiSlot.InsertCharInfoAll( i32SlotIdx, (MULTI_SLOT_STATE)ui8Enable, &CharBaseInfo );

		//pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_PRIMARY,	pADO->GetINT64Data(4),	0 );		//, i64MainWeaponIDX		, i32MainWeaponID
		//pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_SECONDARY,	pADO->GetINT64Data(6),	0 );		//, i64SubWeaponIDX			, i32SubWeaponID
		//pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_MELEE,		pADO->GetINT64Data(8),	0 );		//, i64CloseWeaponIDX		, i32CloseWeaponID
		//pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_THROWING1,	pADO->GetINT64Data(10),	0 );		//, i64ThrownWeaponIDX		, i32ThrownWeaponID
		//pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_THROWING2,	pADO->GetINT64Data(12), 0 );		//, i64SubThrownWeaponIDX	, i32SubThrownWeaponID

		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_CHARA,		pADO->GetINT32Data(3),	0 );		// CharaDBIdx
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_HEAD,		pADO->GetINT32Data(6),	0 );		//, i32HeadGearIDX			
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_FACE,		pADO->GetINT32Data(7),	0 );		//, i32FaceGearIDX			
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_BERET,		pADO->GetINT32Data(8),	0 );		//, i32BeretIDX			
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_UPPER,		pADO->GetINT32Data(9),	0 );		//, i32UpperIDX				
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_LOWER,		pADO->GetINT32Data(10),	0 );		//, i32LowerIDX				
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_GLOVE,		pADO->GetINT32Data(11),	0 );		//, i32GloveIDX				
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_BELT,		pADO->GetINT32Data(12),	0 );		//, i32BeltIDX				
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_HOLSTER,	pADO->GetINT32Data(13),	0 );		//, i32HolsterIDX			
		pNode->m_MultiSlot.SetCharEquipment( i32SlotIdx, CHAR_EQUIPMENT_SKIN,		pADO->GetINT32Data(14),	0 );		//, i32SkinIDX			

		i32Skilldata = pADO->GetINT32Data(16);
		GetMainSkillBit( CharSkillInfo.m_ui8MainSkill, CharSkillInfo.m_ui8MainSkillLv, i32Skilldata );
		i32Skilldata = pADO->GetINT32Data(17);
		GetAssistSkillBit( CharSkillInfo.m_ui8AssistSkill, CharSkillInfo.m_ui8AssistSkillLv, i32Skilldata );
		i32Skilldata = pADO->GetINT32Data(18);
		GetCommonSkillBit( CharSkillInfo.m_ui8CommonSkill, CharSkillInfo.m_ui8CommonSkillLv, i32Skilldata );		
		pNode->m_MultiSlot.SetCharSkillInfo( i32SlotIdx, &CharSkillInfo );

		pADO->MoveNext();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Weapon 정보 로드
	T_ItemDBIdx aTWeaponDBIdx[CHAR_EQUIPMENT_WEAPON_COUNT];

	INT32 i32LoadedWeaponCount = 0;			// 로드한 WeaponDB 갯수.
	INT32 i32LoadWaeponCount = 0;		// 로드할 WeaponDB Count;
	INT32 i32ActiveWeaponDBIdx = -1;	// 유효한 WeaponDBIdx

	pNode->m_pDesign->GetSaveWeaponDBInfo(&pNode->m_MultiSlot, i32LoadWaeponCount, i32ActiveWeaponDBIdx);

	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == pNode->m_MultiSlot.IsActiveChar(i) )	continue;

		if(i32LoadedWeaponCount >= i32LoadWaeponCount)
		{	// 이후 부터는 DB에서 굳이 로드할 필요가 없는 경우 입니다.
			pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_PRIMARY,		aTWeaponDBIdx[CHAR_EQUIPMENT_PRIMARY],		0 );		//, i32MainWeaponIDX
			pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_SECONDARY,	aTWeaponDBIdx[CHAR_EQUIPMENT_SECONDARY],	0 );		//, i32SubWeaponIDX		
			pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_MELEE,		aTWeaponDBIdx[CHAR_EQUIPMENT_MELEE],		0 );		//, i32CloseWeaponIDX	
			pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_THROWING1,	aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING1],	0 );		//, i32ThrownWeaponIDX	
			pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_THROWING2,	aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING2],	0 );		//, i32SubThrownWeaponIDX
			continue;
		}

		pNode->m_MultiSlot.GetCharBaseInfo( i, &CharBaseInfo);

		S2DB::MakeQueryW( wstrSQL, L"usp_Weapon_scan", 
			DB_DATA_TYPE_INT,		CharBaseInfo.m_i32WeaponDBIdx,				
			DB_DATA_TYPE_END );
		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
			return FALSE;
		}

		aTWeaponDBIdx[CHAR_EQUIPMENT_PRIMARY]	= pADO->GetINT32Data(1);
		aTWeaponDBIdx[CHAR_EQUIPMENT_SECONDARY] = pADO->GetINT32Data(2);
		aTWeaponDBIdx[CHAR_EQUIPMENT_MELEE]		= pADO->GetINT32Data(3);
		aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING1] = pADO->GetINT32Data(4);
		aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING2] = pADO->GetINT32Data(5);

		pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_PRIMARY,		aTWeaponDBIdx[CHAR_EQUIPMENT_PRIMARY],		0 );		//, i32MainWeaponIDX
		pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_SECONDARY,	aTWeaponDBIdx[CHAR_EQUIPMENT_SECONDARY],	0 );		//, i32SubWeaponIDX		
		pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_MELEE,		aTWeaponDBIdx[CHAR_EQUIPMENT_MELEE],		0 );		//, i32CloseWeaponIDX	
		pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_THROWING1,	aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING1],	0 );		//, i32ThrownWeaponIDX	
		pNode->m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_THROWING2,	aTWeaponDBIdx[CHAR_EQUIPMENT_THROWING2],	0 );		//, i32SubThrownWeaponIDX

		pADO->MoveNext();

		++i32LoadedWeaponCount;
	}

	pNode->m_pDesign->CheckTeamSlot(&pNode->m_MultiSlot, ui8RedTeamSlot, ui8BlueTeamSlot);

	// Team Slot 설정
	pNode->m_MultiSlot.SetTeamSlot( ui8RedTeamSlot, TEAM_RED  );
	pNode->m_MultiSlot.SetTeamSlot( ui8BlueTeamSlot, TEAM_BLUE );
	 
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load Mission
	S2DB::MakeQueryW( wstrSQL, L"usp_MissionCard_sel", 
		DB_DATA_TYPE_INT,			i32ThreadIdx,
		DB_DATA_TYPE_BIGINT,		pNode->_i64UID,							//    @i64UserUID_        BIGINT
		DB_DATA_TYPE_END );
	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
		return FALSE;
	}
	if( 0 < pADO->GetRecordCount() )
	{
		i32ErrorCode = pADO->GetINT32Data(0);
		switch( i32ErrorCode )
		{
		case 1:
			{
				pADO->GetBinaryData( 1, pNode->_sQuest._pQuestCtxActive[0],	sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW );
				pADO->GetBinaryData( 2, pNode->_sQuest._pQuestCtxActive[1],	sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW );
				pADO->GetBinaryData( 3, pNode->_sQuest._pQuestCtxActive[2],	sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW );
				pADO->GetBinaryData( 4, pNode->_sQuest._pQuestCtxActive[3],	sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW );
				pADO->GetBinaryData( 5, &pNode->_sQuest._QuestIngInfo,		sizeof(QUESTING_INFO) );
				pNode->_sQuest._pMiniature			= pADO->GetINT32Data( 6 );
				pNode->_sQuest._pInsignia			= pADO->GetINT32Data( 7 );
				pNode->_sQuest._pOrder				= pADO->GetINT32Data( 8 );
				pNode->_sQuest._pMaster				= pADO->GetINT32Data( 9 );
				pNode->_sQuest._iQuestEventDate		= pADO->GetINT32Data( 10 );
				pNode->_sQuest._ui64GainUserTitle	= pADO->GetUINT64Data( 11 );
				pNode->_sQuest._EquipUserTitle[0]	= pADO->GetUINT8Data( 12 );
				pNode->_sQuest._EquipUserTitle[1]	= pADO->GetUINT8Data( 13 );
				pNode->_sQuest._EquipUserTitle[2]	= pADO->GetUINT8Data( 14 );
			}
			break;
		case 2:
			{
			}
			break;
		default:
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				return FALSE;
			}
			break;
		}
	}

	if (!g_pConfig->IsContentsEnabled(CONTENTS_MISSION_CARD))
	{	// 미션카드(호칭) Off면 초기화 해줍니다.( 클라이언트 어썰트 팝업 방지 )
		i3mem::FillZero(pNode->_sQuest._EquipUserTitle, sizeof(UINT8) * MAX_EQUIP_USER_TITLE);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//S2DB::MakeQueryW( wstrSQL, L"usp_EventDailyRecord_ins", 
	//	DB_DATA_TYPE_BIGINT,		pNode->_i64UID,												//    @iUSN_          BIGINT
	//	DB_DATA_TYPE_END );

	S2DB::MakeQueryW(wstrSQL, L"usp_EventDailyRecord_ins_Assist",
		DB_DATA_TYPE_BIGINT, pNode->_i64UID,												//    @iUSN_          BIGINT
		DB_DATA_TYPE_END);
	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserLoad( i32ThreadIdx, wstrSQL ); 
		return FALSE;
	}

	if( 0 < pADO->GetRecordCount()) 
	{
		pNode->_sDaily._win			= (UINT16)pADO->GetINT32Data(0);
		pNode->_sDaily._lose		= (UINT16)pADO->GetINT32Data(1);
		pNode->_sDaily._draw		= (UINT16)pADO->GetINT32Data(2);
		pNode->_sDaily._killcount	= (UINT16)pADO->GetINT32Data(3);
		pNode->_sDaily._death		= (UINT16)pADO->GetINT32Data(4);
		pNode->_sDaily._headshot	= (UINT16)pADO->GetINT32Data(5);
		pNode->_sDaily._exp			= pADO->GetINT32Data(6);
		pNode->_sDaily._point		= pADO->GetINT32Data(7);

		pNode->_sDaily._playtime	= pADO->GetINT32Data(8); 
		pNode->_sDaily._playtime	= MAX( pNode->_sDaily._playtime, 0 );

		//pNode->_sDaily._EventDate	= pADO->GetINT32Data(9);

		INT32 i32DGPlaytime			= pADO->GetINT32Data(10);
		pNode->_sDaily._DGPlaytime	= MAX( i32DGPlaytime, 0 );

		pNode->_sDaily._ui8GetItem	= pADO->GetUINT8Data(11);
		pNode->_sDaily._Assist = (UINT16)pADO->GetINT32Data(12);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Medal System

		if( g_pConfig->IsContentsEnabled( CONTENTS_MEDAL) )
		{
			///////////////////////////////////////////////////////////////////////////////////////
			// 1. Load Medal Basic info
			S2DB::MakeQueryW( wstrSQL, L"usp_MedalBasic_sel",
				DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
				DB_DATA_TYPE_END);
			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{						
				ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
				return FALSE;
			}

			// 데이터가 있다.
			if( pADO->GetRecordCount() > 0 ) 
			{		
				// 알리미 등록 메달 정보.
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[0].m_ui8MedalType = pADO->GetUINT8Data(0); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[1].m_ui8MedalType = pADO->GetUINT8Data(1); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[2].m_ui8MedalType = pADO->GetUINT8Data(2); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[3].m_ui8MedalType = pADO->GetUINT8Data(3); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[0].m_ui16Idx = pADO->GetUINT16Data(4); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[1].m_ui16Idx = pADO->GetUINT16Data(5); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[2].m_ui16Idx = pADO->GetUINT16Data(6); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[3].m_ui16Idx = pADO->GetUINT16Data(7); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[0].m_ui16Count = pADO->GetUINT16Data(8); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[1].m_ui16Count = pADO->GetUINT16Data(9); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[2].m_ui16Count = pADO->GetUINT16Data(10); 
				pNode->m_MedalAllInfoDB.m_aNotifyMedal[3].m_ui16Count = pADO->GetUINT16Data(11); 
				//UINT16 ui16MasterMedal								  = pADO->GetUINT16Data(12); // 추후 다른 값으로 사용하던지.
			}	
			else // 데이터 없으면 일단 0으로 셋팅. DB INSERT는 로그 아웃시.
			{	// init
				i3mem::FillZero(  pNode->m_MedalAllInfoDB.m_aNotifyMedal, sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT ); 
			}

			///////////////////////////////////////////////////////////////////////////////////////
			// 2. Load Medal info
			S2DB::MakeQueryW( wstrSQL, L"usp_MedalNormal_sel_All",
				DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
				DB_DATA_TYPE_END);
			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{						
				ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
				return FALSE;
			}

			UINT32 ui32MedalCount = min( pADO->GetRecordCount(), MAX_MEDAL_COUNT );
			pNode->m_MedalAllInfoDB.m_ui16MedalCount = (UINT16)ui32MedalCount;

			for( UINT8 i = 0 ; i < pNode->m_MedalAllInfoDB.m_ui16MedalCount ; i++ )
			{
				pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui16Idx		= pADO->GetUINT16Data(0); 
				pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui16Count		= pADO->GetUINT16Data(1); 
				pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui8GetReward	= pADO->GetUINT8Data(2); 
				pADO->MoveNext();
			}

			///////////////////////////////////////////////////////////////////////////////////////
			// 3. Load Current MedalSet Info
			S2DB::MakeQueryW( wstrSQL, L"usp_MedalSetCurrent_sel_All",
				DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
				DB_DATA_TYPE_END);
			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{						
				ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
				return FALSE;
			}

			UINT32 ui32CurMedalSetCount = min( pADO->GetRecordCount(), MAX_CUR_MEDAL_SET_COUNT );
			pNode->m_MedalAllInfoDB.m_ui16CurMedalSetCount = (UINT16)ui32CurMedalSetCount;

			CUR_MEDAL_SET RecvCurMedalSet;
			for( UINT8 i = 0 ; i < pNode->m_MedalAllInfoDB.m_ui16CurMedalSetCount ; i++ )
			{
				RecvCurMedalSet.m_ui8Type	= pADO->GetUINT8Data(0); 
				RecvCurMedalSet.m_ui16Idx	= pADO->GetUINT16Data(1); 

				RecvCurMedalSet.m_ui32RecvDate	= S2DB::ConvertSmallDateToUINT32(pADO->GetDateData(2)); 
				RecvCurMedalSet.m_ui16Count[0] = pADO->GetUINT16Data(3); 
				RecvCurMedalSet.m_ui16Count[1] = pADO->GetUINT16Data(4); 
				RecvCurMedalSet.m_ui16Count[2] = pADO->GetUINT16Data(5); 
				RecvCurMedalSet.m_ui16Count[3] = pADO->GetUINT16Data(6); 
				RecvCurMedalSet.m_ui16Count[4] = pADO->GetUINT16Data(7); 
				RecvCurMedalSet.m_ui16Count[5] = pADO->GetUINT16Data(8); 
				RecvCurMedalSet.m_ui8GetReward = pADO->GetUINT8Data(9); 


				// 유저가 소유한 이벤트 메달 셋트가 현재 진행중인 이벤트 메달 셋트가 아니라면 완료처리 해줍니다.
				UINT16 ui16Idx = g_pConfig->m_MedalSystemInfo.m_ui16EventMedalSetIdx;	// 변수 넘길어서..
				UINT32 ui32EndTime = g_pConfig->m_MedalSystemInfo.m_ui32EventMedalSetEndTime;

				if( MEDAL_SET_TYPE_EVENT == RecvCurMedalSet.m_ui8Type && 
					( ui16Idx != RecvCurMedalSet.m_ui16Idx   ||  i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() > ui32EndTime ) )
				{
					// 진행중인 메달 셋트를 완료된 메달셋트 테이블로 이전.
					S2DB::MakeQueryW( wstrSQL, L"usp_MedalSetComplete_ins",
						DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
						DB_DATA_TYPE_TINYINT,		RecvCurMedalSet.m_ui8Type,
						DB_DATA_TYPE_SMALLINT,		RecvCurMedalSet.m_ui16Idx,
						DB_DATA_TYPE_END);

					if( FALSE == pADO->ExecuteSP( wstrSQL ) )
					{
						ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
						return FALSE;
					}

					INT32 i32Rv = EVENT_ERROR_SUCCESS;
					if( pADO->GetRecordCount() > 0 ) 
					{
						// 결과
						INT16 ui16Rv = pADO->GetINT16Data(0);
						switch( ui16Rv ) 
						{
						case -9999:	
							// 이전에 완료한적이 있던 메달 셋트가 또다시 완료처리 되려할때 발생합니다.
							// 일단 통과시킵니다.
							//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_FAIL;
							break;
						case -1:
							i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_NOUSER;
							break;
						case 0:
							i32Rv = EVENT_ERROR_SUCCESS;
							break;
						default:
							i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_UNRESULT;
							break;
						}
					}	
					else 
					{	
						i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_COM_NORESULT;
					}

					i3mem::FillZero( &pNode->m_MedalAllInfoDB.m_aCurMedalSet[i], sizeof( CUR_MEDAL_SET ) );
					pNode->m_MedalAllInfoDB.m_aCurMedalSet[i].m_ui16Idx = INIT_MEDAL_NUMBER;
					pNode->m_MedalAllInfoDB.m_ui16CurMedalSetCount--;
				}
				else
				{
					i3mem::Copy( &pNode->m_MedalAllInfoDB.m_aCurMedalSet[i], &RecvCurMedalSet, sizeof( CUR_MEDAL_SET ) );
				}


				pADO->MoveNext();
			}			 
		}
	}

	// QuickJoin
	S2DB::MakeQueryW(wstrSQL, L"usp_QuickJoinNew_sel",
		DB_DATA_TYPE_BIGINT, pNode->_i64UID,
		DB_DATA_TYPE_END);
	if (FALSE == pADO->ExecuteSP(wstrSQL))
	{
		ErrorDBUserLoad(i32ThreadIdx, wstrSQL);
		return FALSE;
	}

	// 데이터가 있다.
	if (pADO->GetRecordCount() > 0)
	{
		pNode->m_aQuickJoinInfo[0].m_ui32StageID = pADO->GetUINT32Data(0);
		pNode->m_aQuickJoinInfo[1].m_ui32StageID = pADO->GetUINT32Data(1);
		pNode->m_aQuickJoinInfo[2].m_ui32StageID = pADO->GetUINT32Data(2);
	}
	else i3mem::FillZero( pNode->m_aQuickJoinInfo, sizeof( QUICKJOIN_INFO ) * MAX_QUICK_JOIN_INFO_COUNT ) ;

#ifdef ENABLE_GACHA_SHOP
	if( g_pConfig->IsContentsEnabled(CONTENTS_GACHA) )
	{
		S2DB::MakeQueryW( wstrSQL, L"usp_GachaGloryCount_sel",
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
			DB_DATA_TYPE_END);

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserLoad( i32ThreadIdx, wstrSQL );
			return FALSE;
		}

		INT32 i32RecordCount = pADO->GetRecordCount();
		for( INT32 i = 0; i < i32RecordCount ; ++i )
		{
			INT32 i32Column = 0;
			INT32 i32ImteKey		= pADO->GetINT32Data(i32Column++);
			INT32 i32PurchaseCount	= pADO->GetINT16Data(i32Column++);
			pADO->MoveNext();

			pNode->SetGachaPurchaseCnt(i32ImteKey, i32PurchaseCount);
		}

		pNode->SetGachaPurchaseRecordCnt(i32RecordCount);
	}
#endif

	// 로그 남깁니다.
	m_i32LoginCount[ i32ThreadIdx ]++;	

	return TRUE; 
}

void CModuleDBUserLoad::ErrorDBUserLoad(INT32 i32ThreadIdx, wchar_t* wstrSQL)
{	
	g_pLog->WriteLog( L"[ErrorDBUserLoad] %s, Error Code : %08x", wstrSQL, m_ppADO[i32ThreadIdx]->GethrLastError() );
	
	HRESULT EventType = m_ppADO[i32ThreadIdx]->ReConnect(
																	m_pUserDBConfig[i32ThreadIdx]->GetDataSource(),
																	m_pUserDBConfig[i32ThreadIdx]->GetCatalog(),
																	m_pUserDBConfig[i32ThreadIdx]->GetAccount(),
																	m_pUserDBConfig[i32ThreadIdx]->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUserLoad] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUserLoad] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBUserLoad::_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t * wstrSQL)
{	
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSQL ); 
}