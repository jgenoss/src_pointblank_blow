#include "pch.h"
#include "TaskProcessor.h"
#include "GeneralRankup.h"
#include "ModuleSIA.h"
#include "UserFinder.h"
#include "ModuleLog.h"
#include "IPChecker.h"
#include "ModuleDBShop.h"
#include "GeoIP2.h"
#include "SystemTimer.h"

#define USER_LEAVE_TIME				(60)
#define USER_LAST_RECVTIME			(60 * 30)
#define DEF_LOGIN_DELAY_TIME		5000

I3_CLASS_INSTANCE(CTaskProcessor, i3Thread);

CTaskProcessor* g_pTaskProcessor = NULL;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning			= FALSE;
	m_ui32ReadedLength		= 0;
	m_ui32GoodsUpdateTime	= 0;
	m_ui32CheckUserIdx		= 0;
	m_ui32ShopTime			= 0;

	m_pDBNick				= NULL;
	m_pDBUID				= NULL;
	m_pDBStats				= NULL;
	m_pDBUserInfo			= NULL;
	m_pDBKickList			= NULL;
	m_pGeneralRankUP		= NULL;

	m_bFirstLogin			= TRUE;
	m_ui32LoginTimeArg		= 0;

	m_pDBUserLoad			= NULL;
	m_pDBUserSave			= NULL;

	m_i32CapsuleCount		= 0;
	m_ppCapsule				= NULL;
#ifdef I3_DEBUG
	m_pi32CapsuleIdx		= NULL;
#endif
	m_pIPChecker			= NULL;
	m_pGeoIP2				= NULL;

	i3mem::FillZero( &m_StatsInfo, sizeof( STATS_INFO ) );
	
	m_ui32ShopInfoUpdateTime= i3ThreadTimer::GetServerTime();

	if( g_pConfig->IsContentsEnabled( CONTENTS_GACHA ) )
	{
		m_ui32GachaShopUpdateTime= i3ThreadTimer::GetServerTime();
	}
	else m_ui32GachaShopUpdateTime= 0xFFFFFFFF - GACHA_SHOP_UPDATE_TIME;

	m_pServerContext		= NULL;
	m_pSessionManager		= NULL;

	m_ui32GeneralRankupSendCount = 0;
	m_pSystemTimer = nullptr;
}

CTaskProcessor::~CTaskProcessor()
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pServerContext );
	I3_SAFE_RELEASE( m_pSessionManager );

	if( m_ppCapsule )
	{
		for( INT32 i = 0 ; i < m_i32CapsuleCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppCapsule[ i ] );
		}
		I3MEM_SAFE_FREE( m_ppCapsule );
	}
#ifdef I3_DEBUG
	I3MEM_SAFE_FREE( m_pi32CapsuleIdx );
#endif

	I3_SAFE_DELETE( m_pRingASCWeb );
	I3_SAFE_DELETE( m_pRingSIAPop );
	I3_SAFE_RELEASE( m_pDBUserSave );
	I3_SAFE_RELEASE( m_pDBUserLoad );
	I3_SAFE_DELETE( m_pDBNick );
	I3_SAFE_DELETE( m_pDBUID );	
	I3_SAFE_DELETE( m_pDBStats );
	I3_SAFE_DELETE( m_pDBUserInfo );
	I3_SAFE_DELETE( m_pDBKickList );

	I3_SAFE_DELETE( m_pIPChecker );
	I3_SAFE_DELETE( m_pGeoIP2 );
	I3_SAFE_DELETE( m_pGeneralRankUP );
	I3_SAFE_DELETE( m_pSystemTimer );

}

INT32 CTaskProcessor::Create( UINT32 ui32DataSize )
{	

	ResetCounter();

	m_ui32CheckUserIdx	= 0;
	m_ui32ShopTime		= i3ThreadTimer::GetServerTime();

	m_i32CapsuleLoadFlag = 0;
	m_i32CapsuleLoadMax = 0;
	for (INT32 i = 0; i < CAPSULE_LOAD_MAX; ++i)
	{
		m_i32CapsuleLoadMax |= 0x01 << i;
	}

	if( FALSE == CreateCapsule() )
	{
		return SERVER_ERROR_CAPSULE;
	}

	SYSTEMTIME stLocalSystemTime;
	::GetLocalTime( &stLocalSystemTime );

	// Stats 용 날짜 계산 초기화
	m_StatsInfo.m_ui32StDayDate		= (i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() / 10000) * 10000;
	struct tm Time;
	Time.tm_year					= i3ThreadTimer::LocalTime().GetYear() - 1900;
	Time.tm_mon						= i3ThreadTimer::LocalTime().GetMonth() - 1;
	Time.tm_mday					= i3ThreadTimer::LocalTime().GetDay() - (stLocalSystemTime.wDayOfWeek!=0?stLocalSystemTime.wDayOfWeek-1:6);
	Time.tm_hour					= Time.tm_min = Time.tm_sec	= 0;
	Time.tm_isdst					= SUMMER_TIME_FOLLOW_SYSTEM;
	mktime( &Time );

	m_StatsInfo.m_ui32StMinDate		= ((i3ThreadTimer::LocalTime().GetYear()%2000)*100000000)+(i3ThreadTimer::LocalTime().GetMonth()*1000000)+(i3ThreadTimer::LocalTime().GetDay()*10000)+(i3ThreadTimer::LocalTime().GetHour()*100)+i3ThreadTimer::LocalTime().GetMinute();
	m_StatsInfo.m_ui32StWeekDate	= (((Time.tm_year + 1900) - 2000) * 100000000) + ((Time.tm_mon+1)  * 1000000) + (Time.tm_mday * 10000);
	m_StatsInfo.m_ui32StMonthDate	= (i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() / 1000000) * 1000000;
	m_i32StOldDay					= i3ThreadTimer::LocalTime().GetDay();
	m_i32StOldMonth					= i3ThreadTimer::LocalTime().GetMonth();
	m_i32StOldHour					= i3ThreadTimer::LocalTime().GetHour();
	// Stats 용 날짜 계산 초기화

	g_pLog->WriteLog( L"[SUCC] Create Capsule" );

	// GeneralRankUp
	m_pGeneralRankUP = CGeneralRankUP::NewObject();
	if(FALSE == m_pGeneralRankUP->OnCreate())
	{
		g_pLog->WriteLog( L"[FAIL] CreateModule GeneralRankUp" );
		return SERVER_ERROR_GENERAL_RANKUP;
	}
	g_pLog->WriteLog( L"[SUCC] CreateModule GeneralRankUp" );

	m_pDBNick	= new CModuleDBNick;
	if( false == m_pDBNick->Create() )
	{
		g_pLog->WriteLog( L"[FAIL] Create CModuleDBNick Fail" );
		return SERVER_ERROR_DATABASE;
	}
	g_pLog->WriteLog( L"[SUCC] Create CModuleDBNick Success" );

	m_pDBUID	= new CModuleDBUID;
	if( FALSE == m_pDBUID->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create CModuleDBUID Fail" );
		return SERVER_ERROR_DATABASE;
	}
	g_pLog->WriteLog( L"[SUCC] Create CModuleDBUID Success" );

	//Create DataBase Worker
	m_pDBUserLoad = CModuleDBUserLoad::NewObject();
	if( FALSE == m_pDBUserLoad->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create Database Work Fail"  );
		return SERVER_ERROR_DB_WORKER;
	}
	g_pLog->WriteLog( L"[SUCC] Create Database Work Success"  );

	m_pDBUserSave = CModuleDBUserSave::NewObject();
	if( FALSE == m_pDBUserSave->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create DB User Save Fail"  );
		return SERVER_ERROR_DB_WORKER; 
	}
	g_pLog->WriteLog( L"[SUCC] Create DB User Save Success"  );

	m_pDBStats	= new CModuleDBStats;
	if( NULL == m_pDBStats || false == m_pDBStats->Create() )
	{
		g_pLog->WriteLog( L"[FAIL] Create DB Stats Fail"  );
		return SERVER_ERROR_DATABASE;
	}
	g_pLog->WriteLog( L"[SUCC] Create DB Stats Success"  );

	m_pDBUserInfo = new CModuleDBUserInfo;
	if( NULL == m_pDBUserInfo || FALSE == m_pDBUserInfo->Create() )	
	{
		g_pLog->WriteLog( L"[FAIL] Create DB User Info Fail"  );
		return SERVER_ERROR_DB_WORKER;
	}
	g_pLog->WriteLog( L"[SUCC] Create DB User Info Success"  );

	m_pDBKickList = new CModuleDBKickList;
	if( NULL == m_pDBKickList || FALSE == m_pDBKickList->Create() )	
	{
		g_pLog->WriteLog( L"[FAIL] Create DB Kick List Fail"  );
		return SERVER_ERROR_DB_WORKER;
	}
	g_pLog->WriteLog( L"[SUCC] Create DB Kick List Success"  );

	m_pIPChecker = new CIPChecker;
	if( FALSE == m_pIPChecker->Create( g_pConfig->IsContentsEnabled( CONTENTS_IP_BLOCK ), g_pConfig->m_ui8IPBlockIPCount, g_pConfig->m_aIPBlockIPOpen, 
		g_pConfig->m_ui8IPBlockNationCount, g_pConfig->m_aui8IPBlockNationOpen ) )
	{
		g_pLog->WriteLog( L"[FAIL] IPChecker Create Fail"  );
		return SERVER_ERROR_GEIP;
	}
	g_pLog->WriteLog( L"[SUCC] IPChecker Create Success"  );

	m_pGeoIP2 = new CGeoIP2;
	if( FALSE == m_pGeoIP2->Create() )
	{
		g_pLog->WriteLog( L"[FAIL] GeoIP2 Create Fail" );
		return SERVER_ERROR_GEOIP2;
	}
	g_pLog->WriteLog( L"[LOAD] GeoIP2 Create Success" );

	m_pRingSIAPop	= new S2RingBuffer;
	if( FALSE == m_pRingSIAPop->Create( sizeof(TASK_INVEN_BUFFER), TASK_RING_BUFFER_COUNT ) )
	{
		g_pLog->WriteLog( L"[FAIL] RingSIAPop Create"  );
		return SERVER_ERROR_SIABUFFER;
	}
	g_pLog->WriteLog( L"[SUCC] RingSIAPop Create Success"  );

	m_pRingASCWeb	= new Local_CRingBuffer;
	if( false == m_pRingASCWeb->OnCreate( sizeof(ASC_WEB_REQ_INFO), TASK_RING_BUFFER_COUNT ) )
	{
		g_pLog->WriteLog( L"[FAIL] RingASCWeb Create"  );
		return SERVER_ERROR_WEB_BUFFER;
	}
	g_pLog->WriteLog( L"[SUCC] RingASCWeb Create Success"  );

	m_pSessionManager = CUserSessionManager::NewObject();
	if( (NULL == m_pSessionManager) || (FALSE == m_pSessionManager->OnCreate( g_pConfig->GetGameServerCount(), g_pConfig->GetGameThreadCount() )) )
	{
		g_pLog->WriteLog( L"[FAIL] Session Manager Create"  );
		return SERVER_ERROR_CONTEXT;
	}
	g_pLog->WriteLog( L"[SUCC] Session Manager Create"  );

	UINT8 ui8TimeOut = 5;
	m_pServerContext = i3NetworkServerContext::NewObject();
	if( (NULL == m_pServerContext) || 
		(FALSE == m_pServerContext->OnCreate( 1, &g_pConfig->m_ui32IP, &g_pConfig->m_ui16Port, &ui8TimeOut, g_pConfig->GetGameThreadCount(), m_pSessionManager ) ))
	{
		g_pLog->WriteLog( L"[FAIL] Server Context Create"  );
		return SERVER_ERROR_CONTEXT;
	}
	g_pLog->WriteLog( L"[SUCC] Server Context Create"  );

	//Add LogThread
	for( UINT32 i = 0 ; i < g_pConfig->GetGameThreadCount() ; ++i )
	{
		UINT32 ui32ThreadID = m_pServerContext->GetCompletionHandler()->GetIocpWorker( i )->GetThreadID();

		wchar_t wstrLog[MAX_PATH];
		i3String::Format( wstrLog, MAX_PATH, L"US%02d", i );

		//UserSession에 등록된 Logidx값을 셋팅
		m_pSessionManager->SetLogidx( (UINT32)i, g_pLog->AddLogThread( wstrLog, 0, (DWORD)ui32ThreadID ) );

	}

	m_pSystemTimer = new CSystemTimer;
	if (nullptr == m_pSystemTimer)
	{
		g_pLog->WriteLog(L"[LOAD] CSystemTimer Init Fail");
		return SERVER_ERROR_STATISTICS;
	}
	if (false == m_pSystemTimer->Create())
	{
		g_pLog->WriteLog(L"[LOAD] CSystemTimer Create Fail");
		return SERVER_ERROR_STATISTICS;
	}

	if (FALSE == i3Thread::Create("TaskProcessor", 0, 4096, NULL, PRIORITY_HIGHEST))
	{
		g_pLog->WriteLog( L"[FAIL] i3Thread::Create Fail"  );

		return SERVER_ERROR_THREAD;
	}

	return SERVER_ERROR_NO;
}

BOOL CTaskProcessor::_SaveUserInfo(USER_NODE * pNode, i3NetworkPacket * pPacket, INT32 i32State, UINT32 ui32ServerIdx )	// Log 확인용 땜빵i32State 입니다. 추후 삭제 예장
{
	// 체크섬이 깨진 경우, 닉네임에 문제가 있는경우도 TRUE 를 return 합니다.
	// FALSE 를 리턴할 때는 AlivePacket에서 패킷을 비워줄 목적이며
	// 패킷을 읽은 후에는 예외상황에 걸려도 로그를 남기고 무조건 TRUE 를 리턴합니다.
	// 기존에 void 형이었기 때문에 문제는 없으나 차후 본 함수에서 return 값을 받아 처리해야하는 경우가 생긴다면
	// 해당 부분을 꼭 참고하시기 바랍니다. - 서동권

	// Playing 상태보다 낮으면 저장하지 않습니다.
	if( USER_STATE_PLAYING > pNode->_ui32State )	return FALSE;
	if( USER_STATE_INFO_SAVE == pNode->_ui32State )
	{	// 저장중이면 다시 받지 않습니다. 이 메시지가 뜨면 게임서버쪽을 수정해줘야 합니다.
		g_pLog->WriteLog( L"[ERROR]_SaveUserInfo STATE SAVE State:%d", i32State );
		return FALSE;
	}

	// 이 루틴에서 필요한 값들을 한번 얻어 옵니다.
	USER_SAVE_DATA				stUserSaveData;
	_GetUserSaveData( pPacket, &stUserSaveData );

	if( 0x123ABCDE != stUserSaveData.m_ui32CheckNum )
	{
		g_pLog->WriteLog( L"#### [ERROR]_SaveUserInfo CheckNum UID:%I64d, UpdateInfo:%d, State:%d, Server:%d", pNode->_i64UID, stUserSaveData.m_ui32UpdateInfo, i32State, ui32ServerIdx );
		return TRUE;
	}

	if( SAVE_DATABASE_BASEINFO & stUserSaveData.m_ui32UpdateInfo )
	{
		i3mem::Copy( &pNode->_sBasic,				&stUserSaveData.m_stBasic,				sizeof( USER_INFO_BASIC_BASE ) );
		i3mem::Copy( &pNode->m_stUserInfoServer,	&stUserSaveData.m_stUserInfoServer,		sizeof( USER_INFO_SERVER ) ); 
	}
	if( SAVE_DATABASE_CLAN & stUserSaveData.m_ui32UpdateInfo )
	{
		i3mem::Copy( &pNode->_sClan, &stUserSaveData.m_stClan,			sizeof( USER_INFO_CLAN ) );
	}
	/*if( SAVE_DATABASE_CHARA_INFO_ALL & stUserSaveData.m_ui32UpdateInfo )
	{
		for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
		{
			if( 0 == (stUserSaveData.m_ui32UpdateInfo & (SAVE_DATABASE_CHARA_INFO_1<<i )) )	continue;

			pNode->m_MultiSlot.SetActiveChar( i, (MULTI_SLOT_STATE)stUserSaveData.m_aui8MultiSlotState[i] );
			pNode->m_MultiSlot.SetCharBaseInfo( i, &stUserSaveData.m_CharBaseInfo[i] );
			pNode->m_MultiSlot.SetCharSkillInfo( i, &stUserSaveData.m_CharSkillInfo[i] );
			pNode->m_MultiSlot.SetCharEquipmentAll( i, stUserSaveData.m_aCharEquipment[i] );

		}

		pNode->m_MultiSlot.RefreshSlotCount();
	}*/
	if( SAVE_DATABASE_CHARA_COMMON & stUserSaveData.m_ui32UpdateInfo )
	{
		pNode->m_MultiSlot.SetCommonEquipmentAll( stUserSaveData.m_aCommonEquips );
		pNode->m_MultiSlot.SetTeamSlot( stUserSaveData.m_ui8TeamSlot[TEAM_RED], TEAM_RED );
		pNode->m_MultiSlot.SetTeamSlot( stUserSaveData.m_ui8TeamSlot[TEAM_BLUE], TEAM_BLUE );
	}
	if( SAVE_DATABASE_RECORD & stUserSaveData.m_ui32UpdateInfo )
	{   					
		i3mem::Copy( &pNode->_sRecord,	&stUserSaveData.m_stRecord,		sizeof(USER_INFO_SEASON_RECORD) );
		i3mem::Copy( &pNode->_sDaily,	&stUserSaveData.m_stDaily,		sizeof(USER_INFO_DAILY) );
	}	
	if( SAVE_DATABASE_QUEST & stUserSaveData.m_ui32UpdateInfo )
	{
		i3mem::Copy( &pNode->_sQuest,	&stUserSaveData.m_stQuest,		sizeof(USER_INFO_QUEST) );
	}

	// 1. Medal Basic Info 
	if( SAVE_DATABASE_MEDAL_BASIC & stUserSaveData.m_ui32UpdateInfo )
	{
		i3mem::Copy(  pNode->m_MedalAllInfoDB.m_aNotifyMedal,	stUserSaveData.m_MedalAllInfoDB.m_aNotifyMedal,	sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT );
	}

	// 2. Normal Medal 
	if(  SAVE_DATABASE_MEDAL & stUserSaveData.m_ui32UpdateInfo)
	{
		for( UINT16 i = 0; i < stUserSaveData.m_MedalAllInfoDB.m_ui16RecvMedalCount ; i ++ )
		{
			SetMedal( &stUserSaveData.m_MedalAllInfoDB.m_aMedal[i].m_Medal, pNode );
		}
	}

	// 3. Current Medal Set
	if( SAVE_DATABASE_CUR_MEDAL_SET & stUserSaveData.m_ui32UpdateInfo)
	{
		i3mem::Copy( pNode->m_MedalAllInfoDB.m_aCurMedalSet,	stUserSaveData.m_MedalAllInfoDB.m_aCurMedalSet,		sizeof( CUR_MEDAL_SET ) * MAX_CUR_MEDAL_SET_COUNT ); 
	}

	// Quick Join
	if( SAVE_DATABASE_QUICK_JOIN & stUserSaveData.m_ui32UpdateInfo)
	{
		i3mem::Copy( pNode->m_aQuickJoinInfo,	stUserSaveData.m_aQuickJoinInfo,	sizeof(QUICKJOIN_INFO)*MAX_QUICK_JOIN_INFO_COUNT);
	}

	if( SAVE_DATABASE_AAS & stUserSaveData.m_ui32UpdateInfo)
	{
		pNode->m_ui16AASAccMin = stUserSaveData.m_ui16AASAccMin;
	}
	if(	SAVE_DATABASE_ITEM & stUserSaveData.m_ui32UpdateInfo )
	{
		i3mem::Copy( &pNode->_sInvenData,	&stUserSaveData.m_stInvItem,	sizeof(USER_INFO_INVITEM_DATA) );
	}

	pNode->_ui32UpdateInfo	= pNode->_ui32UpdateInfo | stUserSaveData.m_ui32UpdateInfo;
	return TRUE;
}

BOOL CTaskProcessor::_SaveCharaInfo(USER_NODE * pNode, UINT8 ui8Count, i3NetworkPacket * pPacket, UINT32 ui32ServerIdx )
{
	// Playing 상태보다 낮으면 저장하지 않습니다.
	if( USER_STATE_PLAYING > pNode->_ui32State )	
	{
		return FALSE;
	}

	// 이 루틴에서 필요한 값들을 한번 얻어 옵니다.
	USER_SAVE_CHARA_DATA			stUserSaveData;
	stUserSaveData.m_ui64UpdateInfo	= 0;
	_GetCharaSaveData( ui8Count, pPacket, &stUserSaveData );

	if( 0x123ABCDE != stUserSaveData.m_ui32CheckNum )
	{
		g_pLog->WriteLog( L"#### [ERROR]_SaveCharaInfo CheckNum UID:%I64d, Count:%d, UpdateInfo:%I64d, Server:%d", pNode->_i64UID, ui8Count, stUserSaveData.m_ui64UpdateInfo, ui32ServerIdx );
		return TRUE;
	}

	for( INT32 i = 0 ; i < ui8Count ; i++ )
	{
		S2MO_CHAR_INFO *pChara = &stUserSaveData.m_CharaInfo[i];

		INT32 i32SlotIdx = (INT32)pChara->m_ui8SlotIdx;

		pNode->m_MultiSlot.SetActiveChar(		i32SlotIdx, (MULTI_SLOT_STATE)pChara->m_ui8SlotState );
		pNode->m_MultiSlot.SetCharBaseInfo(		i32SlotIdx, &pChara->m_BasicInfo );
		pNode->m_MultiSlot.SetCharSkillInfo(	i32SlotIdx, &pChara->m_SkillInfo );
		pNode->m_MultiSlot.SetCharEquipmentAll( i32SlotIdx, pChara->m_EquipInfo.m_aCharEquipment );
	}

	pNode->m_MultiSlot.RefreshSlotCount();
	 
	pNode->m_ui64CharaUpdateInfo	= pNode->m_ui64CharaUpdateInfo | stUserSaveData.m_ui64UpdateInfo;
	return TRUE;
}

void CTaskProcessor::_GetUserSaveData( i3NetworkPacket* pPacket, USER_SAVE_DATA* pstUserData )
{
	i3mem::FillZero( &pstUserData->m_stBasic,		sizeof(USER_INFO_BASIC_BASE) );

	pPacket->ReadData(&pstUserData->m_ui32UpdateInfo ,sizeof(UINT32) ); 

	if( SAVE_DATABASE_BASEINFO & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( &pstUserData->m_stBasic,							sizeof(USER_INFO_BASIC_BASE) );
		pPacket->ReadData( &pstUserData->m_stUserInfoServer,				sizeof(USER_INFO_SERVER) );
	}
	if( SAVE_DATABASE_CLAN & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( &pstUserData->m_stClan,							sizeof(USER_INFO_CLAN) );
	}
	/*if( SAVE_DATABASE_CHARA_INFO_ALL & pstUserData->m_ui32UpdateInfo )
	{
		for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
		{
			if( 0 == (pstUserData->m_ui32UpdateInfo & (SAVE_DATABASE_CHARA_INFO_1<<i )) )	continue;

			pPacket->ReadData( &pstUserData->m_aui8MultiSlotState[i],		sizeof(UINT8) );
			pPacket->ReadData( &pstUserData->m_CharBaseInfo[i],				sizeof(S2MO_CHAR_BASE_INFO) );
			pPacket->ReadData( &pstUserData->m_CharSkillInfo[i],			sizeof(S2MO_CHAR_SKILL_INFO) );
			pPacket->ReadData( &pstUserData->m_aCharEquipment[i],			sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
		}
	}*/
	if( SAVE_DATABASE_CHARA_COMMON & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( pstUserData->m_aCommonEquips,					sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
		pPacket->ReadData( &pstUserData->m_ui8TeamSlot[TEAM_RED],			sizeof(UINT8) );
		pPacket->ReadData( &pstUserData->m_ui8TeamSlot[TEAM_BLUE],			sizeof(UINT8) );
	}
	if( SAVE_DATABASE_RECORD & pstUserData->m_ui32UpdateInfo )
	{   					
		pPacket->ReadData( &pstUserData->m_stRecord,						sizeof(USER_INFO_SEASON_RECORD) );
		pPacket->ReadData( &pstUserData->m_stDaily,							sizeof(USER_INFO_DAILY) ); 
	}	
	if( SAVE_DATABASE_QUEST & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( &pstUserData->m_stQuest,							sizeof(USER_INFO_QUEST) ); 
	}

	// 1. Medal Basic Info 
	if( SAVE_DATABASE_MEDAL_BASIC & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( pstUserData->m_MedalAllInfoDB.m_aNotifyMedal,	sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT ); 
	}

	// 2. Normal Medal 
	if( SAVE_DATABASE_MEDAL & pstUserData->m_ui32UpdateInfo )
	{
		// DB 플래그가 켜진( m_ui8IsSave 가 1인) 것들만 패킷으로 날라옵니다.
		pPacket->ReadData( &pstUserData->m_MedalAllInfoDB.m_ui16RecvMedalCount,	sizeof( UINT16 ) ); 
		for( INT32 i = 0 ; i < pstUserData->m_MedalAllInfoDB.m_ui16RecvMedalCount ; ++i )
		{
			pPacket->ReadData( &pstUserData->m_MedalAllInfoDB.m_aMedal[i].m_Medal, sizeof( MEDAL )); 
		}
	}

	// 3. Current Medal Set
	if( SAVE_DATABASE_CUR_MEDAL_SET & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( pstUserData->m_MedalAllInfoDB.m_aCurMedalSet,	sizeof( CUR_MEDAL_SET ) * MAX_CUR_MEDAL_SET_COUNT ); 
	}

	// Quick join
	if( SAVE_DATABASE_QUICK_JOIN & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( pstUserData->m_aQuickJoinInfo,	sizeof(QUICKJOIN_INFO)*MAX_QUICK_JOIN_INFO_COUNT ); 
	}

	// AAS
	if( SAVE_DATABASE_AAS & pstUserData->m_ui32UpdateInfo )
	{
		pPacket->ReadData( &pstUserData->m_ui16AASAccMin,					sizeof(UINT16) );
	}
	if(	SAVE_DATABASE_ITEM & pstUserData->m_ui32UpdateInfo )
	{	//계정 아이템 사용 정보
		pPacket->ReadData( &pstUserData->m_stInvItem,						sizeof( USER_INFO_INVITEM_DATA ) );
	}

	pPacket->ReadData(&pstUserData->m_ui32CheckNum,							sizeof(UINT32) ); 
}

void CTaskProcessor::_GetCharaSaveData( UINT8 ui8Count, i3NetworkPacket* pPacket, USER_SAVE_CHARA_DATA* pstUserData )
{
	for( UINT8 i = 0; i < ui8Count; ++i )
	{
		S2MO_CHAR_INFO	*pChara = &pstUserData->m_CharaInfo[i];

		pPacket->ReadData( &pChara->m_ui8SlotIdx,				sizeof(UINT8) );

		pstUserData->m_ui64UpdateInfo |= ( (UINT64)0x01 << pChara->m_ui8SlotIdx );

		pPacket->ReadData( &pChara->m_ui8SlotState,					sizeof(UINT8) );
		pPacket->ReadData( &pChara->m_BasicInfo,					sizeof(S2MO_CHAR_BASE_INFO) );
		pPacket->ReadData( &pChara->m_SkillInfo,					sizeof(S2MO_CHAR_SKILL_INFO) );
		pPacket->ReadData( pChara->m_EquipInfo.m_aCharEquipment,	sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
	}

	pPacket->ReadData(&pstUserData->m_ui32CheckNum,							sizeof(UINT32) ); 
}


void CTaskProcessor::_UserLogin( ST_PACKET_LOGIN_REQ* pLoginReq )
{
	INT32 i32NationalCode = m_pGeoIP2->GetNationCode( pLoginReq->m_ui32IP );

	pLoginReq->m_ui8Country = (UINT8)i32NationalCode;
	
	if( FALSE == m_pIPChecker->CheckBlock( pLoginReq->m_ui32IP, i32NationalCode ) )
	{
		_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_BLOCK_IP, pLoginReq->m_i32ServerIdx, pLoginReq->m_i32SessionIdx, pLoginReq->m_ui32AuthKey, 0, NULL ); 
		return;
	}

	// SIA 활성화
	if( g_pConfig->m_bSIAActive )
	{
		if( FALSE == g_pModuleSIA->SendLogin( pLoginReq ) )
		{
			g_pLog->WriteLog( L"[ERROR]LOGIN ID BUFFER FULL" );
			_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, pLoginReq->m_i32ServerIdx, pLoginReq->m_i32SessionIdx, pLoginReq->m_ui32AuthKey, 0, NULL ); 
		}
	}
	else
	{
		UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
		if(pInUID != NULL)
		{
			pInUID->_ui8Type				= TYPE_UID_LOAD;
			pInUID->m_ui8PBVer				= pLoginReq->m_ui8PBVer;
			pInUID->_ui32ServerIdx			= pLoginReq->m_i32ServerIdx;
			pInUID->_ui32SessionIdx			= pLoginReq->m_i32SessionIdx;
			pInUID->_i64UID					= 0;
			pInUID->_i32AuthKey				= pLoginReq->m_ui32AuthKey;
			if((UINT8)(rand()%2)) 			pInUID->_birthday	= 20130101;
			else							pInUID->_birthday	= 19800101;
			pInUID->_ui8Gender				= 1;
			pInUID->_pcCafe					= (UINT8)(rand()%3);
			pInUID->_ui32PCCafeGUID			= 0;
			pInUID->_ui32PCCafeBillingUID	= 0;
			pInUID->_ncCoin					= 999999999;
			pInUID->_IpAddress				= pLoginReq->m_ui32IP;
			pInUID->_UniqueKey				= pLoginReq->m_ui64UniqueKey;
			pInUID->m_ui32LoginTime			= 0;
			pInUID->m_ui8LinkThreadIdx		= 0;
			pInUID->m_i8UIDCreateType		= TYPE_UID_CREATE_INTER;
			pInUID->m_i8LogOutSendType		= TYPE_LOGOUT_ACK_NOT;
			pInUID->m_ui8LocaleCode			= pLoginReq->m_ui8LocaleCode;
			pInUID->m_ui8Country			= pLoginReq->m_ui8Country;

			i3String::Copy( pInUID->_strID,			pLoginReq->m_strID,			NET_ID_SIZE );
			i3mem::Copy(	pInUID->m_strBillingID,	pLoginReq->m_strID,			BILLING_ID_SIZE );
			pInUID->_strID[ NET_ID_SIZE-1 ]				= '\0';
			pInUID->m_strBillingID[ BILLING_ID_SIZE-1 ]	= '\0';

			i3mem::FillZero( pInUID->m_aui8ShutDownTime, sizeof(UINT8) * SHUTDOWN_TIME_SIZE );

			m_pDBUID->PushUIDPTIdx(); 
		}
	}
}

void CTaskProcessor::_SendLogInPacket(INT32 i32Result, INT32 i32ServerIdx, INT32 i32SessionIdx, UINT32 ui32AuthKey, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo )
{
	i3NetworkPacket	SendPacket( PROTOCOL_LOGIN_ACK );
	SendPacket.WriteData( &i32SessionIdx,		sizeof(INT32) );
	SendPacket.WriteData( &ui32AuthKey,			sizeof(UINT32) );
	SendPacket.WriteData( &i32Result,			sizeof(INT32) );
	SendPacket.WriteData( &ui32BlockDate,		sizeof(UINT32) );
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData( pLoginInfo,		sizeof(LOGIN_INFO) );
	}
	
	SendGamePacket(i32ServerIdx, &SendPacket);
}
void CTaskProcessor::_SendLoginBlock(UID_OUT* pUidOut)
{
	i3NetworkPacket	SendPacket(PROTOCOL_LOGIN_ACK);
	SendPacket.WriteData(&pUidOut->_ui32SessionIdx, sizeof(INT32));
	SendPacket.WriteData(&pUidOut->_i32AuthKey, sizeof(UINT32));
	SendPacket.WriteData(&pUidOut->_i32Rv, sizeof(INT32));
	SendPacket.WriteData(&pUidOut->m_ui32BlockDate, sizeof(UINT32));
	if (EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT == pUidOut->_i32Rv)
	{
		UINT8 ui8size = (UINT8)i3String::Length(pUidOut->m_strUserNoticeComment);
		SendPacket.WriteData(&ui8size, sizeof(UINT8));
		SendPacket.WriteData(pUidOut->m_strUserNoticeComment, sizeof(TTCHAR) * ui8size );
		SendPacket.WriteData(&pUidOut->m_i32PenaltyAmount, sizeof(INT32 ));
	}

	SendGamePacket(pUidOut->_ui32ServerIdx, &SendPacket);
}



void CTaskProcessor::_LogoutCommand(USER_NODE * pNode)
{
	// 이미 로그아웃 중입니다.
	//if( pNode->_i8LogOut ) return;
	if( 0 < pNode->_i32ReferenceCount )
	{
		pNode->_bLogOut = true;
		return;
	}
	if( USER_STATE_INFO_LOAD == pNode->_ui32State )
	{	// 아직 로드가 덜 됐으므로 로딩이 완료 될때까지 기다립니다.
		pNode->_bLogOut = true;
		return;
	}
	if( USER_STATE_INFO_SAVE == pNode->_ui32State )
	{
		g_pLog->WriteLog( L"[ERROR]LogOut STATE Error UID=%I64d, State=%d", pNode->_i64UID, pNode->_ui32State );
		return;
	}

	pNode->_bLogOut	= false;

	// 유저 정보를 로드 했을 경우 Save 를 시키고 아닐 경우 그냥 로그아웃을 시킵니다.
	if( USER_DB_LOAD_STATE_LOAD == pNode->m_ui8DBLoadState )
	{
		// 로그아웃 실패시 저장하는 루틴 추가
		UINT32 ui32OldState				= pNode->_ui32State;
		UINT32 ui32NowConnectTime		= i3ThreadTimer::GetServerTime() - pNode->_ui32NowConnectTime;
		pNode->_ui32State				= USER_STATE_INFO_SAVE;

		pNode->_ui32TotalConnectTime	+= ui32NowConnectTime;
		pNode->_ui32TotalConnectCount	+= 1;

		RING_EXIT_SAVE_IN ExitSave;
		ExitSave.pNode = pNode;
		if( RING_BUFFER_SUCCESS != m_pDBUserSave->PushExitSave( &ExitSave ) )
		{
			pNode->_ui32State	= ui32OldState;
			// 이 메시지가 있으면 Save 버퍼를 늘려줍니다.
			g_pLog->WriteLog( L"LOGOUT DB BUFFER FULL" );

			// LogOutBuffer Full 로 인해 실패가 난경우 LogOut 플래그를 켜두어 나중에라도 저장할수 있게 해줍니다.
			pNode->_bLogOut = true;
		}
	}
	else
	{
		_UserExit( pNode );
	}

	return; 
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	char strTemp[MAX_PATH];

	m_i32LogIdx = g_pLog->AddLogThread( L"TASK", 0, m_ThreadID );

	m_bIsRunning = TRUE;
	INT32	i32ServerIdx;
	while(m_bIsRunning)
	{
		
		WORK_PROFILE	stFWorkProfile;
		stFWorkProfile.m_ui64Packet = 0;
		stFWorkProfile.m_ui64Work = 0;

		m_pSystemTimer->RenewElapsedTime();

		_UpdateStatsTime();

		// Server Packet Process  
		i3NetworkPacket Packet;
		INT32 i3GameServerCount = m_pSessionManager->GetBufferCount();
		for( INT32 idx = 0; idx < i3GameServerCount; idx++)
		{
			i32ServerIdx = -1;
			if( m_pSessionManager->GetBuffer( idx, i32ServerIdx, &Packet ) )
			{
				_ParsingGame( i32ServerIdx, &Packet );
				++stFWorkProfile.m_ui64Packet;
			}
		}

		// Capsule Check
		if ( m_i32CapsuleLoadFlag == m_i32CapsuleLoadMax && SERVER_STATE_START == g_eServerState )
		{
			ReSetCapsuleLoad(CAPSULE_LOAD_SHOP);	// 상점은 실시간 UPDATE될수 있으므로 리셋합니다.
			_CheckCapsuleMatching();
		}

		if (_UpdateSIA())					++stFWorkProfile.m_ui64Work;
		if( _UpdateUIDOut() )				++stFWorkProfile.m_ui64Work;
		if( _UpdateNick() )					++stFWorkProfile.m_ui64Work;
		if( _UpdateCreateNick() )			++stFWorkProfile.m_ui64Work;
		if( _UpdateCreateChar() )			++stFWorkProfile.m_ui64Work;
		if( _UpdateExitSave() )				++stFWorkProfile.m_ui64Work;
		if( _UpdateUserLoad() )				++stFWorkProfile.m_ui64Work;
		if( _UpdateASCWeb() )				++stFWorkProfile.m_ui64Work;
		if( _UpdateShop() )					++stFWorkProfile.m_ui64Work;
		if( _UpdateDBUserInfo() )			++stFWorkProfile.m_ui64Work;
		if( _UpdateKickList() )				++stFWorkProfile.m_ui64Work;
		if( _UpdateGeneralRankResult() )	++stFWorkProfile.m_ui64Work;

		_UpdateCheckUser();

		//_UpdateAutoSave();


		// 2초마다 뽑기상점을 확인해서 업데이트 합니다.
		if( m_ui32GachaShopUpdateTime + GACHA_SHOP_UPDATE_TIME < i3ThreadTimer::GetServerTime() )
		{
			SendGachaShopState(true);
			m_ui32GachaShopUpdateTime = i3ThreadTimer::GetServerTime();
		}
		 

		//1분마다 상점을 확인해서 업데이트 합니다.
		if( m_ui32ShopInfoUpdateTime + (SHOP_VERSION_UPDATE_TIME * 60)  < i3ThreadTimer::GetServerTime() )
		{
			g_pModuleDBShop->UpdateShopInfo(); 
			m_ui32ShopInfoUpdateTime = i3ThreadTimer::GetServerTime();
		}
		//For Log
		if(m_ui32LogFileHour != g_pModuleServiceLogFile->GetFileHour())
		{
			//----------------------------------------------------------------------------
			//FILE Save System Log
			g_pLog->WriteLog( L"TASK NET=%d LINK=%d DB_ITEM=%d DB_BUY=%d DB_INVEN=%d DB_SHOP=%d DB_GIFT=%d REPAIR=%d COUPON=%d GACHA=%d WORK=%d SLEEP=%d",
				m_ui32WorkCount_Net, m_ui32WorkCount_Link, m_ui32WorkCount_DB_Item, m_ui32WorkCount_DB_Buy, m_ui32WorkCount_DB_Inven, m_ui32WorkCount_DB_Shop, m_ui32WorkCount_DB_Gift,  
				m_ui32WorkCount_DB_Repair, m_ui32WorkCount_DB_Coupon, m_ui32WorkCount_DB_Gacha, m_ui32WorkCount, m_ui32SleepCount );

			//FILE Save Cash Info
			i3String::Format(strTemp, MAX_PATH, "Cash Hour=%d Day=%d", m_ui32UseCash, m_ui32TodayUseCash); 
			WRITE_SERVICE_LOG_BUY_TASK(strTemp);

			//----------------------------------------------------------------------------
			//Reset Data 
			g_pShop->ResetSaleCount_T();
			this->ResetCounter();
			m_ui32LogFileHour = g_pModuleServiceLogFile->GetFileHour();
			if(m_ui32LogFileHour == 0) m_ui32TodayUseCash = 0;
		}
		
		//-------------------------------------------------------------------------------------------------
		//Update TimeOut Check 
		stFWorkProfile.m_r64Time = m_pSystemTimer->RenewElapsedTime();
		if (stFWorkProfile.m_r64Time > CHECK_TIMEOUT)
		{	//Update가 3초 보다 오래 시간이 걸리면
			g_pLog->WriteLog( L"[WARNING] Task Update Time Out / Time : %.6lf / Total Time : %.6lf / Work : %I64d / Total Work : %I64d / Packet : %I64d / Total Packet : %I64d  ",
				stFWorkProfile.m_r64Time, m_stWorkProfile.m_r64Time, stFWorkProfile.m_ui64Work, m_stWorkProfile.m_ui64Work, stFWorkProfile.m_ui64Packet, m_stWorkProfile.m_ui64Packet);
		}

		m_pSessionManager->OnUpdate( m_i32LogIdx );

		g_pConfig->SetConnectedUserCount( g_pUserFinder->GetUserCount() );

		if (0 == stFWorkProfile.m_ui64Work && 0 == stFWorkProfile.m_ui64Packet)
		{
			::WaitForSingleObject(m_Handle, 1);
		}

		else m_stWorkProfile.Add(&stFWorkProfile);

	}

	return 0;
}

void CTaskProcessor::_UpdateCheckUser()
{
	//-------------------------------------------------------------------------------------------------
	// User Update Time Check  아래 코드와 상관 있음 ()
	m_ui32CheckUserIdx++;
	if( g_pUserFinder->GetMaxUserCount() <= m_ui32CheckUserIdx	) m_ui32CheckUserIdx = 0;
	USER_NODE * pNode = g_pUserFinder->GetUserNodeByIdx( m_ui32CheckUserIdx );

	// 정상적인 유저가 아닐때 킥				
	if( USER_STATE_INIT == pNode->_ui32State )	return;
	
	if( pNode->_bLogOut )
	{
		// 이부분은 차후 수정 예정입니다. 20110819 김현우
		if( 0 == pNode->_i32ReferenceCount )
		{
			if( USER_STATE_INFO_LOAD != pNode->_ui32State )
			{
				_LogoutCommand( pNode );
			}
		}
	}
	else if( pNode->_LastRecvTimeST < i3ThreadTimer::GetServerTime() - USER_LAST_RECVTIME )//30분
	{
		//Error Log
		g_pLog->WriteLog( L"Kill User AlivePacket TimeOut Server = %d SessionIdx = %d UID = %I64d", pNode->_i32Serveridx, pNode->_i32Sessionidx, pNode->_i64UID );

		//LogOut User
		_LogoutCommand( pNode );
	}
	else if( (pNode->_ServerLeaveTimeST > 0) && (pNode->_ServerLeaveTimeST < i3ThreadTimer::GetServerTime() - USER_LEAVE_TIME) )
	{
		//Error Log
		g_pLog->WriteLog( L"Kill User Leave Server TimeOut = %d SessionIdx = %d UID = %I64d", pNode->_i32Serveridx, pNode->_i32Sessionidx, pNode->_i64UID );

		//LogOut User
		_LogoutCommand( pNode );
	}

	switch( pNode->m_PCCafeInfo.GetState() )
	{
	case PCCAFE_WORK_STATE_REMOVE_RIGHT:
		{
			// 1. 유저노드 정보에 PC방 혜택을 제거한다.
			pNode->_ui8RoleFlag = PC_NOT_CAFE;
					
			// 2. 게임서버와 클라이언트에게 노티스한다.
			SendPCCafeStatusAck( pNode );

			pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_NONE );
		}
		break;
	case PCCAFE_WORK_STATE_LOGIN:
		{
			PACKET_PCCAFE_LOGIN_REQ LoginReq;
			LoginReq.m_TUID				= pNode->_i64UID;
			LoginReq.m_ui32IP			= pNode->_IpAddress;
			LoginReq.m_ui32PCCafeGUID	= pNode->m_PCCafeInfo.GetPCCafeGUID();
			g_pModuleSIA->SendPCCafeLogin( &LoginReq );

			wchar_t wstrIP[ MAX_PATH ];
			i3Net::GetIPToLongW( LoginReq.m_ui32IP, wstrIP );
			g_pLog->WriteLog( L"[PCCAFE_WORK_STATE_LOGIN_Send], UID = %I64d, IP = %s, PCCafeGUID = %d",
				 LoginReq.m_TUID, wstrIP, LoginReq.m_ui32PCCafeGUID );

			pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_LOGIN_REQ );
		}
		break;
	case PCCAFE_WORK_STATE_LOGIN_REQ:
		{
			if ( pNode->m_PCCafeInfo.CheckTimeOut() )
			{
				//Error Log
				g_pLog->WriteLog( L"PCCafe Login Req TimeOut, ServerIdx = %d SessionIdx = %d UID = %I64d", 
					pNode->_i32Serveridx, pNode->_i32Sessionidx, pNode->_i64UID );

				pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_LOGIN );
			}
		}
		break;
	case PCCAFE_WORK_STATE_CHARGE_CHECK:
		{
			if ( pNode->m_PCCafeInfo.IsChargeCheckTime() )
			{
				PACKET_PCCAFE_CHECK_AMOUNT_REQ CheckAmountReq;
				CheckAmountReq.m_TUID			= pNode->_i64UID;
				CheckAmountReq.m_ui32IP			= pNode->_IpAddress;
				CheckAmountReq.m_ui32PCCafeGUID	= pNode->m_PCCafeInfo.GetPCCafeGUID();
				g_pModuleSIA->SendPCCafeCheckAmount( &CheckAmountReq );

				pNode->m_PCCafeInfo.SetChargeCheckTime();
			}
		}
		break;
	}
}

bool CTaskProcessor::_UpdateAutoSave()
{
	if( m_ui32AutoSaveTime + AUTO_SAVE_DELAY_TIME > i3ThreadTimer::GetServerTimeMilli() )	return false;	// 20 ms 마다 한명의 유저를 저장합니다.
	m_ui32AutoSaveTime = i3ThreadTimer::GetServerTimeMilli();

	USER_NODE * pNode = g_pUserFinder->GetUserNodeByIdx( m_ui32AutoSaveUserIdx );

	m_ui32AutoSaveUserIdx++; 
	if( g_pUserFinder->GetMaxUserCount() <= m_ui32AutoSaveUserIdx	) m_ui32AutoSaveUserIdx = 0;

	if( USER_STATE_PLAYING != pNode->_ui32State )										return false;	// 일반적인 게임 중일 경우
	if( -1 == pNode->_i32Sessionidx )													return false;	// 유저가 없을 경우	
	if( 0 == pNode->_ui32UpdateInfo )													return false;	// 저장 할게 없을 경우

	//m_pDBUserSave->PushExitSave( NODE_SAVE_TYPE_AUTO, pNode );

	return true;
}

bool CTaskProcessor::_UpdateGeneralRankResult()
{
	if( NULL == m_pGeneralRankUP ) return false;
	if( 0 == m_pGeneralRankUP->GetOutBufferCount() ) return false;

	GENERAL_RANK_RESULT*	pGeneralResult	= m_pGeneralRankUP->GetOutBuffer();
	USER_NODE*				pNode			= g_pUserFinder->GetUserNodeByUID( pGeneralResult->m_TUID );

	if( 0 == pNode )
	{
		m_pGeneralRankUP->GetOutBufferIdx();
		return false;
	}

	i3NetworkPacket SendPacket(PROTOCOL_BASE_GENERALRANK_CHANGE_ACK);
	SendPacket.WriteData( &pNode->_i32Sessionidx,	sizeof( INT32 ) );
	SendPacket.WriteData( pGeneralResult,			sizeof(GENERAL_RANK_RESULT) );

	SendGamePacket( pNode->_i32Serveridx, &SendPacket );

	m_ui32GeneralRankupSendCount++;

	m_pGeneralRankUP->GetOutBufferIdx();

	return true;
}

BOOL CTaskProcessor::SendGamePacket(UINT32 idx,i3NetworkPacket* pPacket)
{
	return m_pSessionManager->SendPacketMessage(idx, pPacket);
}

BOOL CTaskProcessor::SendAllGamePacket(i3NetworkPacket* pPacket)
{
	return m_pSessionManager->SendAllPacketMessage( pPacket );
}

BOOL CTaskProcessor::_AccountKick( T_UID i64DBIdx, INT32 i32Reason )
{
	BOOL	bRv = FALSE;
	USER_NODE * pNode = g_pUserFinder->GetUserNodeByUID( i64DBIdx );

	if( pNode )
	{
		if( USER_STATE_ACCOUNTING != pNode->_ui32State )
		{
			pNode->_ui32State		= USER_STATE_ACCOUNTING;
			pNode->_LastRecvTimeST	= i3ThreadTimer::GetServerTime() - USER_LAST_RECVTIME + 60;	// 20초뒤 강제 종료 할수 있게 합니다. 
		}

		bRv = TRUE;

		UINT8 ui8Reason = (UINT8)i32Reason;
		i3NetworkPacket	SendPacket(PROTOCOL_GSM_ACCOUNT_KICK_REQ);
		SendPacket.WriteData(&pNode->_i32Sessionidx,	sizeof(UINT32));
		SendPacket.WriteData(&i64DBIdx,					sizeof(T_UID));
		SendPacket.WriteData(&ui8Reason,				sizeof(UINT8));
		SendGamePacket(pNode->_i32Serveridx, &SendPacket);

		g_pLog->WriteLog( L"AccountKick Success- serverId(%d) sessionIdx(%d), uid(%I64d), Reason(%d)",
			pNode->_i32Serveridx, pNode->_i32Sessionidx, pNode->_i64UID, i32Reason );
	}
	else
	{
		g_pLog->WriteLog( L"AccountKick Fail - uid(%I64d)", i64DBIdx );
	}

	return bRv;
}

void CTaskProcessor::SetPointItem( USER_NODE* pUserNode, UINT32 ui32ItemID, GET_POINT_ROUTE eGetRoute )
{
	if( ITEM_TYPE_POINT != GET_ITEM_TYPE( ui32ItemID ) )	return;

	UINT32 ui32Point = GET_POINTITEM( ui32ItemID );

	_PlusPoint( MODULE_TASK_THREAD_IDX, pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, pUserNode->_i64UID, ui32Point, eGetRoute );
}

void CTaskProcessor::SetPointItemCount(USER_NODE * pUserNode, UINT32 ui32ItemID, GET_POINT_ROUTE eGetRoute, TLoopCnt tCount)
{
	if (ITEM_TYPE_POINT != GET_ITEM_TYPE(ui32ItemID))	return;

	UINT32 ui32Point = GET_POINTITEM(ui32ItemID) * static_cast<UINT32>(tCount);

	_PlusPoint(MODULE_TASK_THREAD_IDX, pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, pUserNode->_i64UID, ui32Point, eGetRoute);
}

void CTaskProcessor::_PlusPoint( INT32 i32ThreadIdx, INT32 i32Serveridx, INT32 i32Sessionidx, T_UID T_DBIdx, UINT32 ui32Point, GET_POINT_ROUTE eGetRoute )
{
	UINT8 ui8GetRoute = (UINT8)eGetRoute;
	i3NetworkPacket	SendPacket( PROTOCOL_SHOP_PLUS_POINT_ACK );
	SendPacket.WriteData( &i32Sessionidx,	sizeof( INT32 ) );
	SendPacket.WriteData( &T_DBIdx,			sizeof( T_UID ) );
	SendPacket.WriteData( &ui32Point,		sizeof( UINT32 ) );
	SendPacket.WriteData( &ui8GetRoute,		sizeof( UINT8 ) );
	SendGamePacket( i32Serveridx, &SendPacket );
}

void CTaskProcessor::_AlarmCoin( INT32 i32Serveridx, INT32 i32Sessionidx, T_UID i64DBIdx, INT32 i32Result, UINT32 ui32Point, UINT32 ui32Cash )
{
	i3NetworkPacket	SendPacket( PROTOCOL_AUTH_GET_POINT_CASH_ACK );
	SendPacket.WriteData( &i32Sessionidx,	sizeof( INT32 ) );
	SendPacket.WriteData( &i64DBIdx,		sizeof( T_UID ) );
	SendPacket.WriteData( &i32Result,		sizeof( INT32 ) );
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData( &ui32Point,	sizeof( UINT32 ) );
		SendPacket.WriteData( &ui32Cash,	sizeof( UINT32 ) );
	}
	SendGamePacket( i32Serveridx, &SendPacket );
}

void CTaskProcessor::SendNewVersionAll()
{
	CShop* pShop = g_pShop;
	UINT32 ui32Version	= pShop->GetVersion();

	i3NetworkPacket	VersionPacket( PROTOCOL_AUTH_SHOP_VERSION_ACK );
	VersionPacket.WriteData( &ui32Version,			sizeof( UINT32 ) );

	SendAllGamePacket( &VersionPacket);
}

void CTaskProcessor::SendNewVersion(UINT32 ui32ServerIdx )
{
	CShop* pShop = g_pShop;
	UINT32 ui32Version	= pShop->GetVersion();

	i3NetworkPacket	VersionPacket( PROTOCOL_AUTH_SHOP_VERSION_ACK );
	VersionPacket.WriteData( &ui32Version,			sizeof( UINT32 ) );
	SendGamePacket( ui32ServerIdx, &VersionPacket );
}

void CTaskProcessor::SendFieldShop( UINT32 ui32ServerIdx )
{
	i3NetworkPacket	SendPacket( PROTOCOL_FIELDSHOP_SEND_NEWVERSION_ACK );
	SendPacket.WriteData( g_pShop->GetFieldShopInfo(),	sizeof( FieldShopInfo ) );
	SendGamePacket( ui32ServerIdx, &SendPacket );
}

void CTaskProcessor::SendGoodsList( UINT32 ui32ServerIdx )
{
	CShop* pShop = g_pShop;

	UINT32 ui32Version				= pShop->GetVersion();
	UINT32 ui32GoodsCount			= pShop->GetGoodsCount();
	UINT32 ui32ItemCount			= pShop->GetItemCount();
	UINT32 ui32MatchingCount		= pShop->GetMatchingCount();
	UINT32 ui32RepairCount			= pShop->GetRepairCount();

	SHOP_GOODS_DATA*		pGoodsData	= pShop->GetGoodsAllData();
	SHOP_ITEM_DATA*			pItemData	= pShop->GetItemAllData();
	SHOP_MATCHING_DATA*		pMatching	= pShop->GetMatchingAllData();
	SHOP_ITEM_REPAIR_DATA*	pRepairData	= pShop->GetRepairAllData();


	// 1. Item Send
	INT32	i32PacketSize		= PACKETBUFFERSIZE - PACKETHEADERSIZE - ( sizeof( UINT32 ) + sizeof( UINT32 ) + sizeof( UINT32 ) );
	UINT32	ui32RemainCount		= ui32ItemCount;
	UINT32	ui32CurItemCount	= 0;
	UINT32	ui32StartIdx		= 0;
	UINT32	ui32ItemBufferIdx	= 0;

	UINT32	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_ITEM_DATA ));
	while( 0 < ui32RemainCount )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
		else									ui32CurItemCount	= ui32RemainCount;
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	ItemPacket( PROTOCOL_AUTH_SHOP_ITEMLIST_ACK );
		ItemPacket.WriteData( &ui32ItemCount,				sizeof( UINT32 ) );
		ItemPacket.WriteData( &ui32CurItemCount,			sizeof( UINT32 ) );
		ItemPacket.WriteData( &ui32StartIdx,				sizeof( UINT32 ) );

		//UINT32 i = ui32StartIdx;
		UINT32 i32Count = 0;
		while( i32Count <  ui32CurItemCount )
		{
			ItemPacket.WriteData( &pItemData[ui32ItemBufferIdx],			sizeof( SHOP_ITEM_BASE ) );
			ItemPacket.WriteData( pItemData[ui32ItemBufferIdx]._strName,	sizeof( SHOP_ITEM_EXPANSION ) );

			i32Count++;
			ui32ItemBufferIdx++;
		}

		SendGamePacket( ui32ServerIdx, &ItemPacket );
	}

	// 3. Goods Send
	ui32RemainCount		= ui32GoodsCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;
	UINT32	ui32GoodsBufferIdx	= 0;

	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_GOODS_DATA ));
	while( 0 < ui32RemainCount )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
		else									ui32CurItemCount	= ui32RemainCount;
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	GoodsPacket( PROTOCOL_AUTH_SHOP_GOODSLIST_ACK );
		GoodsPacket.WriteData( &ui32GoodsCount,			sizeof( UINT32 ) );
		GoodsPacket.WriteData( &ui32CurItemCount,		sizeof( UINT32 ) );
		GoodsPacket.WriteData( &ui32StartIdx,			sizeof( UINT32 ) );

		//UINT32 i = ui32StartIdx;
		UINT32 i32Count = 0;
		while( i32Count <  ui32CurItemCount )
		{

			if( GOODS_USE_ENABLE == pGoodsData[ui32GoodsBufferIdx]._ui8Enable )
			{
				GoodsPacket.WriteData( &pGoodsData[ui32GoodsBufferIdx],		(sizeof( SHOP_GOODS_EXPANSION )) );
				i32Count++;
			} 

			ui32GoodsBufferIdx++;
		}

		SendGamePacket( ui32ServerIdx, &GoodsPacket );
	}

	// 4. Repair Send
	ui32RemainCount		= ui32RepairCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_ITEM_REPAIR_DATA ));
	while( 0 < ui32RemainCount )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
		else									ui32CurItemCount	= ui32RemainCount;
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	RepairPacket( PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK );
		RepairPacket.WriteData( &ui32RepairCount,			sizeof( UINT32 ) );
		RepairPacket.WriteData( &ui32CurItemCount,			sizeof( UINT32 ) );
		RepairPacket.WriteData( &ui32StartIdx,				sizeof( UINT32 ) );
		RepairPacket.WriteData( &pRepairData[ui32StartIdx],	(UINT16)(sizeof( SHOP_ITEM_REPAIR_DATA )*ui32CurItemCount) );
		SendGamePacket( ui32ServerIdx, &RepairPacket );
	}

	// 5. Matching Send
	i32PacketSize		= PACKETBUFFERSIZE - PACKETHEADERSIZE - sizeof( UINT32 ) - sizeof( UINT32 ) - sizeof( UINT32 ) - sizeof( UINT32 ) - ( sizeof(char) * NET_MD5_KEY_SIZE )  ;
	ui32RemainCount		= ui32MatchingCount;
	ui32CurItemCount	= 0;
	ui32StartIdx		= 0;

	ui32MaxCount		= (i32PacketSize / sizeof( SHOP_MATCHING_DATA ));
	while( 0 < ui32RemainCount )
	{
		ui32StartIdx			+= ui32CurItemCount;
		if( ui32RemainCount > ui32MaxCount )	ui32CurItemCount	= ui32MaxCount;
		else									ui32CurItemCount	= ui32RemainCount;
		ui32RemainCount -= ui32CurItemCount;

		i3NetworkPacket	MatchingPacket( PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK );
		MatchingPacket.WriteData( &ui32MatchingCount,		sizeof( UINT32 ) );
		MatchingPacket.WriteData( &ui32CurItemCount,		sizeof( UINT32 ) );
		MatchingPacket.WriteData( &ui32StartIdx,			sizeof( UINT32 ) );
		MatchingPacket.WriteData( &pMatching[ui32StartIdx],	(UINT16)(sizeof( SHOP_MATCHING_DATA )*ui32CurItemCount) );

		MatchingPacket.WriteData( &ui32Version,				sizeof( UINT32 ) );
		MatchingPacket.WriteData( pShop->GetShopMD5Key(),	sizeof(char) * NET_MD5_KEY_SIZE );

		SendGamePacket( ui32ServerIdx, &MatchingPacket );
	}
}


void CTaskProcessor::UserInsertItem(USER_NODE* pUserNode, INVEN_BUFFER* pInvenItem, INT64 &i64CharaSync)
{
	INT32 i32BlankIdx = -1;
	// ID 가 0 이면 삭제된 아이템이다.
	if (0 == pInvenItem->_ui32ItemID)	return;

	// 정상 구매 되지 않은 아이템은 인벤토리에 집어 넣지 않는다.
	if (ITEM_ATTR_TYPE_STAND_BY == pInvenItem->_ui8ItemType)	return;

	BOOL bNew = TRUE;
	for (INT32 j = 0; j < MAX_INVEN_COUNT; j++)
	{
		if (0 == pUserNode->_Inven[j]._TItemWareDBIdx)
		{
			if (-1 == i32BlankIdx)
			{
				i32BlankIdx = j;
			}
		}
		else if (pUserNode->_Inven[j]._TItemWareDBIdx == pInvenItem->_TItemWareDBIdx)
		{
			i32BlankIdx = j;
			bNew = FALSE;
			break;
		}
	}
	// 아이템 복사
	if (-1 != i32BlankIdx)
	{
		i3mem::Copy(&pUserNode->_Inven[i32BlankIdx], pInvenItem, sizeof(INVEN_BUFFER));
		if (bNew)
		{
			pUserNode->_ui16InvenCount++;

			// 이벤트/인벤토리 로 강제지금된 캐릭터를 멀티슬롯에도 활성화 시켜줍니다.
			_SyncChara(pUserNode, pInvenItem, i64CharaSync);
		}
	}
}

UINT32 CTaskProcessor::UserDeleteItem( USER_NODE* pUserNode, T_ItemDBIdx TItemWareIdx )
{
	for( INT32 j = 0 ; j < MAX_INVEN_COUNT ; j++ )
	{
		if( TItemWareIdx == pUserNode->_Inven[j]._TItemWareDBIdx )
		{
			UINT32 ui32ItemID = pUserNode->_Inven[j]._ui32ItemID;
			i3mem::FillZero( &pUserNode->_Inven[j], sizeof( INVEN_BUFFER ) );
			pUserNode->_ui16InvenCount--;
			return ui32ItemID;
		}
	}
	return 0;
}

void CTaskProcessor::_ItemAbility( USER_NODE* pUserNode, UINT32 ui32ItemID )
{
	if( ITEM_TYPE_WRAP_COUNT != GET_ITEM_TYPE( ui32ItemID ) )	return;

	switch( ui32ItemID )
	{
	case CASHITEM_ITEMID_CLR_HISTORY:
		{
			pUserNode->_sRecord._match		= 0;
			pUserNode->_sRecord._win		= 0;
			pUserNode->_sRecord._lose		= 0;
			pUserNode->_sRecord._draw		= 0;

			pUserNode->_sRecord._smatch		= 0;
			pUserNode->_sRecord._swin		= 0;
			pUserNode->_sRecord._slose		= 0;
			pUserNode->_sRecord._sdraw		= 0;

			pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD;
			//pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD_RESET;
		}
		break;
	case CASHITEM_ITEMID_CLR_ESCAPE:
		{
			pUserNode->_sRecord._dis		= 0;
			pUserNode->_sRecord._dmatch		= 0;

			pUserNode->_sRecord._sdis		= 0;
			pUserNode->_sRecord._sdmatch	= 0;

			pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD;
			//pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD_RESET;
		}
		break;
	case CASHITEM_ITEMID_CLR_SCORE:
		{
			pUserNode->_sRecord._death		= 0;
			pUserNode->_sRecord._killcount	= 0;

			pUserNode->_sRecord._sdeath		= 0;
			pUserNode->_sRecord._skillcount	= 0;

			pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD;
			//pUserNode->_ui32UpdateInfo |= SAVE_DATABASE_RECORD_RESET;
		}
		break;
	case CASHITEM_ITEMID_CHANGE_NICK:				// 닉네임 변경권
		{
			UINT8 ui8Size = (UINT8)i3String::Length( pUserNode->_sBasic.m_strNick ) + 1;
			i3NetworkPacket	SendPacket( PROTOCOL_AUTH_CHANGE_NICKNAME_ACK );
			SendPacket.WriteData( &pUserNode->_i32Sessionidx,	sizeof( INT32 ) );
			SendPacket.WriteData( &pUserNode->_i64UID,			sizeof( T_UID ) );
			SendPacket.WriteData( &ui8Size,						sizeof( UINT8 ) );
			SendPacket.WriteData( pUserNode->_sBasic.m_strNick,	sizeof( TTCHAR ) * ui8Size );
			SendGamePacket( pUserNode->_i32Serveridx, &SendPacket );
		}
		break;
		// 컬러 크로스 헤어는 이 곳으로 들어오지 않는다.
	case CASHITEM_ITEMID_COLOR_CROSSHAIR:			// 컬러 크로스 헤어
		{	
			UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
			if(pInUID != NULL)
			{
				pInUID->_i64UID = pUserNode->_i64UID;
				if( SALE_ITEM_COLOR_CROSSHAIR & pUserNode->_sInvenData._ui8Item )
				{
					pInUID->_ui8Type	= TYPE_UID_UPDATE_ITEM; 
				}
				else
				{
					pInUID->_ui8Type	= TYPE_UID_INSERT_ITEM; 
				}
				pUserNode->_sInvenData._ui8Item |= SALE_ITEM_COLOR_CROSSHAIR;
				// 세이브..
				//pInUID->InvenItem._ui32ItemID	= SALE_ITEM_COLOR_CROSSHAIR;
				pInUID->InvenItem._ui32ItemID	= MAKE_ITEM_ID(ITEM_TYPE_WRAP_PERIOD, CASHITEM_DAY_NO, 14 );
				i3mem::Copy( pInUID->InvenItem._pui8Data, &pUserNode->_sInvenData._ui8CrossHairColor, sizeof( UINT8) );

				m_pDBUID->PushUIDPTIdx();
			}
		}
		break;
	}
}

void CTaskProcessor::_SendGiftAuth( USER_NODE* pNode, DB_GIFT_OUT* pGift )
{
	i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK) ;
	SendPacket.WriteData( &pNode->_i32Sessionidx,				sizeof(INT32)	);
	SendPacket.WriteData( &pNode->_i64UID,						sizeof(T_UID)	);
	SendPacket.WriteData( &pGift->m_ui8Type,					sizeof(UINT8)	);
	SendPacket.WriteData( &pGift->m_ui8GiftCount,				sizeof(UINT8)	);
	SendPacket.WriteData( pGift->m_aAuthGift,					sizeof(AUTH_GIFT_INVEN) * pGift->m_ui8GiftCount	);

	switch( pGift->m_ui8Type ) 
	{	 
	case TYPE_SHOP_GIFT_AUTH :	
		{	// 유저 인벤토리 업데이트.
			INT64 i64CharaSync = 0;
			for( UINT8 i = 0 ; i < pGift->m_ui8GiftCount ; ++i )
			{
				if( EV_FAILED( pGift->m_aAuthGift[i].m_i32Rv )) continue;  

				for( UINT8 k = 0 ; k < pGift->m_aAuthGift[i].m_ui8ItemCount ; ++k )
				{
					UserInsertItem( pNode, &pGift->m_aAuthGift[i].m_Inven[k], i64CharaSync);
				}
			} 
		}
		break;
	}

	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

void CTaskProcessor::_SendGiftRecv( T_UID TUID, SEND_GIFT* pGift )
{
	USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pGift->m_TReceiverUID);
	if( NULL == pNode ) return;
	
	i3NetworkPacket	SendPacket(PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK);//
	SendPacket.WriteData(&pNode->_i32Sessionidx, sizeof(INT32));
	SendPacket.WriteData(&pNode->_i64UID, sizeof(T_UID));

	// 선물 만료 날짜는 현재로 부터 30일 뒤.
	UINT32 ui32Expire = g_pConfig->GetAddAuthTime(i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), SECOND_PER_DAY * MAX_GET_GIFT_EXPIRE_DAY);
	SendPacket.WriteData(&ui32Expire, sizeof(UINT32));

	UINT8	ui8SenderNickSize = (UINT8)i3String::Length(pGift->m_strSenderNick);
	SendPacket.WriteData(&ui8SenderNickSize, sizeof(UINT8));
	SendPacket.WriteData(pGift->m_strSenderNick, sizeof(TTCHAR) * ui8SenderNickSize);

	UINT8	ui8MessageSize = (UINT8)i3String::Length(pGift->m_strMessage);
	SendPacket.WriteData(&ui8MessageSize, sizeof(UINT8));
	SendPacket.WriteData(pGift->m_strMessage, sizeof(TTCHAR) * ui8MessageSize);

	SendPacket.WriteData(&pGift->m_ui8GiftCount, sizeof(UINT8));
	SendPacket.WriteData(pGift->m_aGift, sizeof(S2MO_RECV_GIFT) * pGift->m_ui8GiftCount);

	SendGamePacket(pNode->_i32Serveridx, &SendPacket);
	
}

void CTaskProcessor::UsedWeaponList_T( USER_NODE* pNode, UINT16 ui16Count, USED_ITEM* pUsedItemList )
{
	UINT16			ui16InvenCount = 0;
	INVEN_BUFFER	SendInven[ MAX_INVEN_COUNT ];		// 보내기 위한 버퍼

	ITEM_AUTH_DATA AuthData[ MAX_SHOP_ITEM_AUTH_COUNT ];
	UINT8 ui8AuthCount = 0;

	DB_SHOP_IN DBShopIn;
	DBShopIn._ui8InsertType	= TYPE_SHOP_ITEM_AUTH_NOACK;
	DBShopIn._i64UID		= pNode->_i64UID;

	for( UINT32 i = 0 ; i < ui16Count ; i++ )
	{
		for( UINT32 j = 0 ; j < MAX_INVEN_COUNT ; j++ )
		{
			if( pUsedItemList[ i ].m_TUsedItemDBIdx != pNode->_Inven[ j ]._TItemWareDBIdx )	continue;

			// 리스트를 찾았다.
			AuthData[ ui8AuthCount ]._TItemIdx = pNode->_Inven[ j ]._TItemWareDBIdx;
			AuthData[ ui8AuthCount ]._i32ItemArg = pUsedItemList[ i ].m_ui16UsedDurablity; //WEAPON_AUTH_COUNT;				
			ui8AuthCount++;

			// m_ui16UsedDurablity 만큼 차감
			if( pNode->_Inven[ j ]._ui32ItemArg > pUsedItemList[ i ].m_ui16UsedDurablity )
			{
				pNode->_Inven[ j ]._ui32ItemArg -= pUsedItemList[ i ].m_ui16UsedDurablity; //WEAPON_AUTH_COUNT;
			}
			else
			{
				pNode->_Inven[ j ]._ui32ItemArg = 0;
			}

			// 보내기 위한 버퍼를 설정한다.
			i3mem::Copy( &SendInven[ ui16InvenCount ], &pNode->_Inven[j], sizeof( INVEN_BUFFER ) );

			// 인증 기간이 만료되었을때
			if( 0 == pNode->_Inven[ j ]._ui32ItemArg )
			{
				// 아이템이 만료후 삭제 아이템이라면 아이템을 삭제해준다.
				SHOP_ITEM_BASE* pItem = g_pShop->GetItemData( pNode->_Inven[ j ]._ui32ItemID );
				if( NULL != pItem && (ITEM_EXPIRE_DELETE == pItem->_ui8AuthExpireType))
				{
					// 캐릭터 만료일 경우. 추가 처리.
					if (ITEM_TYPE_CHARA == GET_ITEM_TYPE(pNode->_Inven[j]._ui32ItemID))
					{
						DeleteCharaDB(pNode, pNode->_Inven[j]._TItemWareDBIdx);
						continue;	// 멀티슬롯DB에서 삭제된 후 Game서버에 전달 됩니다.
					}

					i3mem::FillZero( &pNode->_Inven[j], sizeof( INVEN_BUFFER ) );
					SendInven[ ui16InvenCount ]._ui32ItemID = 0;
				}
			}
			ui16InvenCount++;
			break;
		}
	}

	for( UINT32 ui32ItemCount = 0 ; ui32ItemCount < ui8AuthCount ; ui32ItemCount++ )
	{
		DBShopIn._TWareDBIdx	= AuthData[ui32ItemCount]._TItemIdx;
		DBShopIn._ui32ItemArg	= AuthData[ui32ItemCount]._i32ItemArg;

		if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InInsertItem( &DBShopIn ) )
		{	// Error
			g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Auth Data NoAck 2 / UID : %I64d / ItemWareIdx : %d / ItemArg : %d", 
				pNode->_i64UID, AuthData[ui32ItemCount]._TItemIdx, AuthData[ui32ItemCount]._i32ItemArg );	
		}
	}

	if (0 == ui16InvenCount) return;

	// 바로 게임서버로 보내준다.
	i3NetworkPacket	SendPacket( PROTOCOL_AUTH_USED_WEAPON_ACK );
	SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
	SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
	SendPacket.WriteData( &ui16InvenCount,				sizeof( UINT16 ) );
	SendPacket.WriteData( SendInven,					sizeof( INVEN_BUFFER )*ui16InvenCount );

	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

INT32 CTaskProcessor::_SetItemAbilityAuth( USER_NODE * pNode, UINT32 ui32ItemID, T_ItemDBIdx TItemWareIdx, UINT8 ui8Ability, void* pAbility )
{
	INT32 i32Rv = EVENT_ERROR_SUCCESS;
	
	switch( GET_ITEM_TYPE( ui32ItemID ) )
	{
	case ITEM_TYPE_WRAP_PERIOD:
		{
			// 기간제 껍데기인데.. 기간제로 변환 해야 하는가보다.
			UINT32 ui32MaintenenceID = MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, GET_ITEM_NUMBER(ui32ItemID));
			switch(ui32MaintenenceID)
			{
				case CASHITEM_ITEMID_COLOR_NICK:			// 컬러 닉네임					
					break;
				case CASHITEM_ITEMID_DISGUISE_NICK:		// 위장 닉네임
					break;
				case CASHITEM_ITEMID_DISGUISE_RANK:		// 위장 계급
					{
						UINT8 ui8Rank;
						i3mem::Copy( &ui8Rank, pAbility, sizeof(UINT8) );
						
						pNode->_sInvenData._wRank = ui8Rank;
					}
					break;
				case CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL:
					{
						UINT8 ui8Rank;
						i3mem::Copy( &ui8Rank, pAbility, sizeof(UINT8) );

						pNode->_sInvenData._wAprilFoolRank = ui8Rank;
					}
					break;
				case CASHITEM_ITEMID_COLOR_CHATTING:		// 컬러 체팅
					break;
				case CASHITEM_ITEMID_COLOR_CROSSHAIR:	// 컬러 크로스 헤어
					{
						UINT8 ui8Color;
						i3mem::Copy( &ui8Color, pAbility, sizeof(UINT8) );
						// 임시로 저장을 해놓는다.
						pNode->_sInvenData._ui8CrossHairColor	= ui8Color;
					}
					break;
				case CASHITEM_ITEMID_COLOR_MUZZLE_FLASH:
					{
						UINT16 ui16Color;
						i3mem::Copy( &ui16Color, pAbility, sizeof(UINT16) );
						// 임시로 저장을 해놓는다.
						pNode->_sInvenData._ui16MuzzleFlashColor = ui16Color;
					}
					break;
			}
		}
		break;

	case ITEM_TYPE_WRAP_COUNT:
		{
			switch( ui32ItemID ) 
			{
				case CASHITEM_ITEMID_CHANGE_NICK:		// 닉네임 변경
				{
					TTCHAR strNickName[ NET_NICK_NAME_SIZE ];
					i3mem::Copy( strNickName, pAbility, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
					if( EVENT_RETURN_OK == m_pDBNick->PushNickName( TYPE_NICK_T_CHANGE, pNode, strNickName, pNode->_sBasic.m_strNick, TItemWareIdx ) )
					{
						i32Rv = EVENT_ERROR_SUCCESS_1;
					}
					else
					{
						i32Rv = EVENT_ERROR_FAIL;
					}
				}
				break;	
	
				case CASHITEM_ITEMID_MEGAPHONE:
					{
						i3NetworkPacket SendPacket;
						UINT8	ui8NickLength = (UINT8)i3String::Length(pNode->_sBasic.m_strNick);

						SendPacket.SetProtocol(PROTOCOL_BASE_MEGAPHONE_ACK);
						SendPacket.WriteData( &ui8NickLength,			sizeof(UINT8) ); 
						SendPacket.WriteData( pNode->_sBasic.m_strNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
						SendPacket.WriteData( &ui8Ability,				sizeof(UINT8) );
						SendPacket.WriteData( pAbility,					ui8Ability );

						SendAllGamePacket( &SendPacket );

					}
					break;
			}
		}
		break;
	}

	return i32Rv;
}

void CTaskProcessor::_CheckaleItem( USER_NODE* pNode )
{
	UINT8 ui8Item = 0;
	for( INT32 i = 0 ; i < pNode->_ui16InvenCount ; i++ )
	{
		if(CASHITEM_ITEMID_COLOR_CROSSHAIR == pNode->_Inven[ i ]._ui32ItemID )
		{
			ui8Item |= SALE_ITEM_COLOR_CROSSHAIR;
		}
	}

	if( (SALE_ITEM_COLOR_CROSSHAIR & pNode->_sInvenData._ui8Item) && 
		FALSE == ( SALE_ITEM_COLOR_CROSSHAIR & ui8Item ) )
	{
		pNode->_sInvenData._ui8Item &= ~SALE_ITEM_COLOR_CROSSHAIR;

		UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
		if(pInUID != NULL)
		{
			pInUID->_ui8Type				= TYPE_UID_DELETE_ITEM;
			pInUID->_i64UID					= pNode->_i64UID;
			pInUID->InvenItem._ui32ItemID	= MAKE_ITEM_ID(ITEM_TYPE_WRAP_PERIOD, CASHITEM_DAY_NO, 14);
			m_pDBUID->PushUIDPTIdx();
		}
	}
}


T_RESULT CTaskProcessor::_PacketCreateChara(USER_NODE* pNode, i3NetworkPacket* pPacket )
{
	UINT8				ui8CreateType;
	RING_CREATE_CHAR	CreateChar;
	UINT8				ui8NameSize;

	pPacket->ReadData( &ui8CreateType,					sizeof(UINT8) );
	pPacket->ReadData( &CreateChar.m_TGoodsID,			sizeof(T_GoodsID) );
	pPacket->ReadData( &CreateChar.m_Inven._TItemWareDBIdx,	sizeof(T_ItemDBIdx) );
	pPacket->ReadData( &CreateChar.m_ui8Class,			sizeof(UINT8) );
	pPacket->ReadData( &ui8NameSize,					sizeof(UINT8) );
	pPacket->ReadData( CreateChar.m_CharBaseInfo.m_strCharName,			sizeof(TTCHAR) * ui8NameSize );

	SHOP_GOODS_LIST* pGoods = g_pShop->GetGoodsByID( CreateChar.m_TGoodsID );
	if( NULL == pGoods )					return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;		// 없는 상품

	T_RESULT TResult = EVENT_ERROR_SUCCESS;
	CreateChar.m_eType = (CHARA_RING_TYPE)ui8CreateType;

	

	switch( ui8CreateType )
	{
	case GOODS_BUY_CHAR_FREE:		
		{
			CreateChar.m_ui8SlotState = MULTI_SLOT_STATE_NORMAL;
			CreateChar.m_TUID = pNode->_i64UID;
			CreateChar.m_bAddCharaSlot = true;
			INT32 i32InsertSlot = pNode->m_MultiSlot.GetInsertSlot();
			if (i32InsertSlot == CHAR_SLOT_NONE) return EVENT_ERROR_CHARA_FULL_SLOT;

			CreateChar.m_ui8CharSlot = (UINT8)i32InsertSlot;
			return _CreateChar( &CreateChar );
		}
		break;
	}

	// 인벤토리에 STAND BY 상태로 INSERT 합니다. 
	DB_SHOP_GOODS DBShopBuy;
	
	DBShopBuy.m_TUID			= pNode->_i64UID;
	DBShopBuy.m_ui8JobAction	= TYPE_SHOP_CHARA_BUY;
	DBShopBuy.m_ui8GoodsCount	= 1;
	DBShopBuy.m_stDBGoods[0].m_TGoodsID		= CreateChar.m_TGoodsID;
	DBShopBuy.m_stDBGoods[0].m_ui8BuyType	= ui8CreateType;
	
	i3mem::Copy( &DBShopBuy.m_stChara, &CreateChar, sizeof( RING_CREATE_CHAR ));

	DBShopBuy.m_stChara.m_bAddCharaSlot = false;

	TResult = _BuyGoods( pNode, &DBShopBuy );

	return TResult;
}

INT32 CTaskProcessor::_CreateChar(  RING_CREATE_CHAR* pCreateChar )
{
	switch( pCreateChar->m_eType )
	{
	//case CHARA_RING_TYPE_CREATE:  
	case CHARA_RING_TYPE_USE:  
		{
			pCreateChar->m_ui8SlotState		= MULTI_SLOT_STATE_NORMAL;
		}
		break;

	case CHARA_RING_TYPE_BUY_POINT: 
	case CHARA_RING_TYPE_BUY_CASH: 
		{
			if( CHAR_SLOT_NONE == pCreateChar->m_ui8CharSlot )
			{
				return EVENT_ERROR_CHARA_FULL_SLOT;
			}
		}
		break;
	}

	if( RING_BUFFER_SUCCESS != m_pDBUserSave->PushCreateChar( pCreateChar ) )
	{
		return EVENT_ERROR_EVENT_BUFFER_FULL;
	}

	return EVENT_ERROR_SUCCESS;
}

T_RESULT CTaskProcessor::_BuyGoods( USER_NODE* pNode, DB_SHOP_GOODS* pDBShopBuy )
{
	if( !pNode ) return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_USER;

	char strLog[MAX_PATH];

	// Goods ID Check and Set
	DB_SHOP_GOODS Buy[GOODS_BUY_CASH];
	i3mem::FillZero( Buy, sizeof( DB_SHOP_GOODS ) * GOODS_BUY_CASH );

	UINT8 ui8ItemInsertType = ITEM_INSERT_REQUEST_BUY;

	for( UINT8 i = 0 ; i < pDBShopBuy->m_ui8GoodsCount ; i ++ )
	{
		T_RESULT TResult = _CheckAndSet_Goods( pDBShopBuy->m_TUID, &pDBShopBuy->m_stDBGoods[i], ui8ItemInsertType );
		if( EV_FAILED( TResult ) ) return TResult;

		// 결제 타입별로 버퍼에 담는다.
		INT32 Idx = pDBShopBuy->m_stDBGoods[i].m_ui8BuyType - 1;	// enum은 1부터시작 하므로 맞춥니다. 

		//Buy[Idx].m_ui32BufferIdx = pDBShopBuy->m_ui32BufferIdx;
		Buy[Idx].m_i32Rv		= EVENT_ERROR_SHOP_BUY_FAIL; 
		Buy[Idx].m_TUID			= pNode->_i64UID;
		Buy[Idx].m_TItemDBIdx	= pDBShopBuy->m_TItemDBIdx;
		Buy[Idx].m_ui8JobAction	= pDBShopBuy->m_ui8JobAction;
		i3mem::Copy( &Buy[Idx].m_stDBGoods[Buy[Idx].m_ui8GoodsCount], &pDBShopBuy->m_stDBGoods[i], sizeof(DB_GOODS)  );
		Buy[Idx].m_ui8GoodsCount++;

		// Service 로그 기록.
		i3String::Format( strLog, MAX_PATH, "BUYGOODS,REQ,ID = %s, UID = %I64d, GoodsId = %d, BuyType = %d", pNode->_strID, pNode->_i64UID, pDBShopBuy->m_stDBGoods[i].m_TGoodsID, pDBShopBuy->m_stDBGoods[i].m_ui8BuyType );
		WRITE_SERVICE_LOG_BUY_TASK( strLog );
	}



	// 결제 타입별로 처리.
	for( INT32 k = 0 ; k < GOODS_BUY_CASH ; k++ )
	{
		if( 0 == Buy[k].m_ui8GoodsCount ) continue;	// 요청이 없었다.

		switch( pDBShopBuy->m_ui8JobAction ) 
		{
		case TYPE_SHOP_CHARA_BUY :
			i3mem::Copy( &Buy[k].m_stChara, &pDBShopBuy->m_stChara, sizeof(RING_CREATE_CHAR) );
			break;
		}
	
		switch( k + 1 )		// enum은 1부터 시작하므로.
		{
		case GOODS_BUY_POINT :	// Goods Insert도 이쪽으로 옵니다.
			{	// DB로 보냅니다.
				Buy[k].m_ui8DBAction = TYPE_SHOP_GOODS_INSERT;
			}
			break;
		case GOODS_BUY_CASH :
			{	// DB로 보냅니다.				
				Buy[k].m_ui8DBAction = TYPE_SHOP_GOODS_BUY;
			}
			break;
		}	// switch

		if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopBuy( &Buy[k] ) )
		{	// Error. 가장 첫번째 GoodsID만 기록.
			g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Buy / BuyType : %d / GoodsCount : %d / GoodsID : %d / UID : %I64d",
				k + 1, Buy[k].m_ui8GoodsCount, Buy[k].m_stDBGoods[0].m_TGoodsID, pNode->_i64UID );
			return EVENT_ERROR_EVENT_BUFFER_FULL;
		}

	}	// for

	return EVENT_ERROR_SUCCESS;
}

void CTaskProcessor::_UpdateStatsTime()
{
	// 기존 CCU 보다 크면 바꿔준다. Hour 기준
	m_StatsInfo.m_ui32StCCU = g_pUserFinder->GetUserCount();
	if( m_StatsInfo.m_ui32StMCU_10Min < m_StatsInfo.m_ui32StCCU )
	{
		m_StatsInfo.m_ui32StMCU_10Min	= m_StatsInfo.m_ui32StCCU;
	}
	if( m_StatsInfo.m_ui32StMCU_1Min < m_StatsInfo.m_ui32StCCU )
	{
		m_StatsInfo.m_ui32StMCU_1Min	= m_StatsInfo.m_ui32StCCU;
	}

	UINT32 ui32LocalUserCount;
	for( INT32 i = 0 ; i < LOCALE_CODE_MAX ; i++ )
	{
		ui32LocalUserCount = g_pUserFinder->GetLocalUserCount( (LOCALE_CODE)i );
		if( m_StatsInfo.m_ui32StLocaleMCU_1Min[ i ] < ui32LocalUserCount )
		{
			m_StatsInfo.m_ui32StLocaleMCU_1Min[ i ] = ui32LocalUserCount;
		}
	}

	if( m_i32StOldMinute == i3ThreadTimer::LocalTime().GetMinute() )	return;
	m_i32StOldMinute	= i3ThreadTimer::LocalTime().GetMinute();

	//매분 단위로 ZLOG 전송용 파일에 기록합니다.
	if( g_pModuleLogD != NULL ) 
	{
		g_pModuleLogD->SendLogMessage( LOG_MESSAGE_CCU, LOCALE_CODE_NONE, &m_StatsInfo.m_ui32StMCU_1Min );			//동접자수
		for( INT32 i = (LOCALE_CODE_NONE+1) ; i < LOCALE_CODE_MAX ; i++ )
		{
			if( 0 == m_StatsInfo.m_ui32StLocaleMCU_1Min[i] )	continue;
			g_pModuleLogD->SendLogMessage( LOG_MESSAGE_LOCALE_CCU, (UINT8)i, &m_StatsInfo.m_ui32StLocaleMCU_1Min[i] );							//동접자수
		}
		m_StatsInfo.m_ui32StMCU_1Min = 0;
		i3mem::FillZero( m_StatsInfo.m_ui32StLocaleMCU_1Min, sizeof(m_StatsInfo.m_ui32StLocaleMCU_1Min) );
	}
		
	// 10분에 MCU 정보를 전달합니다.(ZPLOG 기록)
	if( 0 == ( m_i32StOldMinute % 10 ) )
	{
		m_StatsInfo.m_ui32StMinDate	= i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
		m_pDBStats->Push( STATS_TYPE_CCU_MIN, m_StatsInfo.m_ui32StMinDate, m_StatsInfo.m_ui32StMCU_10Min );

		m_StatsInfo.m_ui32StMCU_10Min = 0;
	}
	
	// 날이 변경 되었는지 체크합니다.
	if( m_i32StOldDay == i3ThreadTimer::LocalTime().GetDay() )			return;
	m_i32StOldDay		= i3ThreadTimer::LocalTime().GetDay();

	//------------------장성계급실시간반영패킷유통량 조사-----------------------	
	g_pLog->WriteLog( L"[General_Rankup] Real-time applied User Count : %d", m_ui32GeneralRankupSendCount );
	m_ui32GeneralRankupSendCount = 0;
	//--------------------------------------------------------------------------
	
	// 달이 변경 되었는지 체크합니다.
	if( m_i32StOldMonth == i3ThreadTimer::LocalTime().GetMonth() )		return;
	m_i32StOldMonth		= i3ThreadTimer::LocalTime().GetMonth();
}

void CTaskProcessor::AllUserKick()
{
	for( UINT32 i = 0 ; i < g_pUserFinder->GetMaxUserCount() ; i++ )
	{
		USER_NODE * pUserNode = g_pUserFinder->GetUserNodeByIdx( i );
		if( NULL == pUserNode ) continue;
		if( -1 == pUserNode->_i32Sessionidx ) continue;

		pUserNode->_LastRecvTimeST = 0;
	}
}

void CTaskProcessor::BlockUser( T_UID i64UID, const TTCHAR *pstrComment )
{
	UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
	if(pInUID != NULL)
	{
		pInUID->_ui8Type	= TYPE_UID_BLOCK_USER;
		pInUID->_i64UID		= i64UID;
		i3String::Copy( pInUID->_strComment, pstrComment, GM_COMMENT_SIZE );
		pInUID->_strComment[i3String::Length(pstrComment)] = 0;

		m_pDBUID->PushUIDPTIdx();
	}
}

void	CTaskProcessor::InsertEventItemOldUser( USER_NODE * pNode )
{
	// 닉네임 생성 전인 유저입니다.
	if( USER_DB_LOAD_STATE_NONE == pNode->m_ui8DBLoadState )					return;
	
	T_RESULT	TResult;

	if( g_pConfig->IsContentsEnabled( CONTENTS_GIFT ) )			// 선물 함으로 INSERT
	{
		SEND_GIFT	GiftIn;
		GiftIn.m_eGiftType		= GIFT_TYPE_EVENT;
		GiftIn.m_TReceiverUID	= pNode->_i64UID;
		GiftIn.m_TSenderUID		= 0;
		if( g_pConfig->GetLoginUserEventGoods( pNode->m_ui8LocaleCode, pNode->_dt32EventItemDate, &GiftIn ) )
		{
			TResult = _GiftInsertDB( &GiftIn, ITEM_INSERT_REQUEST_EVENT_CONNECT );
			if( EV_SUCCESSED( TResult ) )
			{
				pNode->_ui32UpdateInfo			|= SAVE_DATABASE_LOGIN_EVENT;	// DB 저장
			}
			else
			{
				g_pLog->WriteLog( L"Error InsertEventItemOldUser / UID : %I64d / Reason : %x", pNode->_i64UID, TResult );
			}
		}
	}
	else
	{
		UINT8	ui8ItemCunt;
		ST_ITEM	stItem[ MAX_SHOP_DB_ITEM_COUNT ];

		if( g_pConfig->GetLoginUserEventItem( pNode->m_ui8LocaleCode, pNode->_dt32EventItemDate, &ui8ItemCunt, stItem ) )
		{
			TResult = _ItemInsertDB( pNode->_i64UID, ITEM_INSERT_REQUEST_EVENT_CONNECT, ui8ItemCunt, stItem );
			if( EV_SUCCESSED( TResult ) )
			{
				pNode->_ui32UpdateInfo			|= SAVE_DATABASE_LOGIN_EVENT;	// DB 저장
			}
			else
			{
				g_pLog->WriteLog( L"Error InsertEventItemOldUser / UID : %I64d / Reason : %x", pNode->_i64UID, TResult );
			}
		}
	}
}

void	CTaskProcessor::InsertEventItemNewUser( USER_NODE * pNode )
{
	UINT8		ui8ItemCount;
	ST_ITEM		stItem[ MAX_SHOP_DB_ITEM_COUNT ];

	if( g_pConfig->GetNewUserEvent( pNode->m_ui8LocaleCode, &ui8ItemCount, stItem ) )
	{
		if( EV_FAILED( _ItemInsertDB( pNode->_i64UID, ITEM_INSERT_REQUEST_EVENT_NEWUSER, ui8ItemCount, stItem ) ) )
		{
			g_pLog->WriteLog( L"Error InsertEventItemNewUser / UID : %I64d / LocaleCode:%d / ItemCount:%d ", pNode->_i64UID, pNode->m_ui8LocaleCode, ui8ItemCount );
		}
	}
}

bool	CTaskProcessor::_CheckAttendanceTarget( USER_NODE* pNode, ATTENDANCE_INFO* pEventInfo, UINT8 ui8Type, bool bChangeEvent )
{ 
	bool &bAttendanceTarget = pNode->_Attendance[ui8Type].m_bAttendanceTarget;
	// 이전에 진행한 이벤트 버전과 같은 경우, 다시 검사하지 않음
	if( false == bChangeEvent )				return bAttendanceTarget;
	
	switch( ui8Type )
	{
	case ATTENDANCE_TYPE_DORMANT:
		{
			if( 0 != pNode->_ui32TotalConnectCount )
			{
				if( pNode->m_dt32LastLogoutDate.GetDateTimeYYMMDDHHMI() <= pEventInfo->m_ui32ConditionDate )
				{
					bAttendanceTarget = true;
					break;
				}
			}
			else
			{	 // 알 수 없는 이유로 로그아웃 정보가 없는 유저는 닉네임 생성일로 검사
				if( 0 != pNode->_ui32CreateDate && pNode->_ui32CreateDate <= pEventInfo->m_ui32ConditionDate )
				{
					bAttendanceTarget = true;
					break;
				}
			} // 신규 유저는 대상이 아님
			bAttendanceTarget = false;
		}
	break;
	case ATTENDANCE_TYPE_NORMAL:
		{
			bAttendanceTarget = true;
		}
	break;
	}

	pNode->_ui32UpdateInfo |= SAVE_DATABASE_ATTENDANCE;

	return bAttendanceTarget;
}

INT32	CTaskProcessor::_CheckAttendanceMark( USER_NODE * pNode, UINT32 ui32Version, UINT8 ui8Type )
{ 
	INT32 i32ActiveIdx = g_pConfig->m_i32ActiveAttendanceIdx[ ui8Type ];
	if( -1 == i32ActiveIdx )																									return EVENT_ERROR_ATTENDANCE_PERIOD;
	
	ATTENDANCE_USER_SVR*	pUserInfo = &pNode->_Attendance[ ui8Type ];
	ATTENDANCE_INFO*		pEventInfo = &g_pConfig->m_aAttendanceInfo[ ui8Type ][ i32ActiveIdx ];

	if( ui32Version									!= pEventInfo->m_ui32Version )												return EVENT_ERROR_ATTENDANCE_WRONG_VERSION;
	if( pUserInfo->m_bAttendanceTarget				== false )																	return EVENT_ERROR_ATTENDANCE_TARGET;
	if( pUserInfo->m_dt32AttendanceTime.GetDate()	== i3ThreadTimer::LocalTime().GetDateTimeBin( TIME_ATTENDANCE_CHECK )  )	return EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED;
		
	if( 0 == pEventInfo->m_ui32GiveGoodsID[ pUserInfo->m_ui8AttendanceDay ].m_ui8GoodsCount )					// 금일 받을 아이템이 없을 경우 아이템을 받은 것으로 처리합니다
	{
		pUserInfo->m_ui8AttendanceItem++;
	}
	pUserInfo->m_ui8AttendanceDay++;																				// 출석 체크
	pUserInfo->m_dt32AttendanceTime	= i3ThreadTimer::LocalTime().GetDateTimeBin( TIME_ATTENDANCE_CHECK );		// 기간 설정
	pNode->_ui32UpdateInfo			|= SAVE_DATABASE_ATTENDANCE;

	return EVENT_ERROR_ATTENDANCE_SUCCESS;
}

void	CTaskProcessor::_SetAttendanceInfo( USER_NODE * pNode )
{
	for( UINT8 ui8Type = 0; ui8Type < ATTENDANCE_TYPE_MAX; ui8Type++ )
	{
		INT32 i32ActiveIdx				= g_pConfig->m_i32ActiveAttendanceIdx[ ui8Type ];
		if( -1 == i32ActiveIdx )
		{
			pNode->m_stAttendanceInfo[ ui8Type ].m_ui32Version = 0;
			pNode->m_stAttendanceInfo[ ui8Type ].m_ui8AttendanceType = ui8Type;
			continue;
		}
	
		ATTENDANCE_INFO* pEventInfo		= &g_pConfig->m_aAttendanceInfo[ ui8Type ][ i32ActiveIdx ];
		i3mem::Copy( &pNode->m_stAttendanceInfo[ ui8Type ], pEventInfo, sizeof(ATTENDANCE_INFO) );

		ATTENDANCE_USER_SVR* pUserInfo	= &pNode->_Attendance[ ui8Type ];
		bool bChangeEvent				= pEventInfo->m_ui32Version != pUserInfo->m_ui32Version;

	// 기존에 이용하던 버전이 아니면 초기화
		if( bChangeEvent )
		{
			pUserInfo->m_ui32Version = pEventInfo->m_ui32Version;
			pUserInfo->m_ui8AttendanceDay = 0;
			pUserInfo->m_ui8AttendanceItem = 0;
			pNode->_ui32UpdateInfo |= SAVE_DATABASE_ATTENDANCE;
		}

		if( false == _CheckAttendanceTarget( pNode, pEventInfo, ui8Type, bChangeEvent ) )			
			continue;

		pUserInfo->m_ui8EventState	= ATTENDANCE_STATE_RUNNING;

		DATE32	ui32Today = i3ThreadTimer::LocalTime().GetDateTimeBin(TIME_ATTENDANCE_CHECK);
		DATE32	ui32Yesterday = g_pConfig->m_dt32YesterDateYYMMDD;

		// 어제 출석 하지 않았을 경우
		if( ui32Yesterday > pUserInfo->m_dt32AttendanceTime )
		{
			// 리셋 플래그가 켜져있을 경우 초기화
			if( pEventInfo->m_bReset )
			{
				pUserInfo->m_ui8AttendanceDay	= 0;
				pUserInfo->m_ui8AttendanceItem	= 0;
				pNode->_ui32UpdateInfo			|= SAVE_DATABASE_ATTENDANCE;
			}
		}
		
		// 금일 출첵 도장을 찍었을 경우
		if( ui32Today == pUserInfo->m_dt32AttendanceTime )
		{
			// 금일 보상까지 받았을 경우
			if( pUserInfo->m_ui8AttendanceDay == pUserInfo->m_ui8AttendanceItem )
			{
					pUserInfo->m_ui8EventState		= ATTENDANCE_STATE_COMPLETE;
			}
		}
		else
		{	// 처음 들어왔을 경우

			// 한번의 사이클이 끝났을 경우
			if( pEventInfo->m_ui8AttendanceDay <= pUserInfo->m_ui8AttendanceItem )
			{
				// 반복이 활성 화 되어있다면 리셋
				if( pEventInfo->m_bRepeat )
				{
					pUserInfo->m_ui8AttendanceDay	= 0;
					pUserInfo->m_ui8AttendanceItem	= 0;
					pNode->_ui32UpdateInfo			|= SAVE_DATABASE_ATTENDANCE;
				}
				else
				{
					// 금일 체크 사항 없음
						pUserInfo->m_ui8EventState		= ATTENDANCE_STATE_COMPLETE;
				}
			}
			else
			{
				// 어제 보상을 받지 않았을 경우 걍 초기화 시킴
				pUserInfo->m_ui8AttendanceItem	= pUserInfo->m_ui8AttendanceDay;
				pNode->_ui32UpdateInfo			|= SAVE_DATABASE_ATTENDANCE;
			}
		}
	}
}

INT32	CTaskProcessor::_CheckAttendanceInfo( USER_NODE* pNode, UINT32 ui32Version, UINT8 ui8ItemNum, UINT8 ui8Type )
{
	INT32 i32ActiveIdx = g_pConfig->m_i32ActiveAttendanceIdx[ ui8Type ];
	if( -1 == i32ActiveIdx )																						return EVENT_ERROR_ATTENDANCE_PERIOD;

	ATTENDANCE_INFO *		pEventInfo	= &g_pConfig->m_aAttendanceInfo[ ui8Type ][ i32ActiveIdx ];
	ATTENDANCE_USER_SVR*	pUserInfo	= &pNode->_Attendance[ ui8Type ];

	// 버전이 맞는지 확인
	if( ui32Version != pEventInfo->m_ui32Version )																	return EVENT_ERROR_ATTENDANCE_WRONG_VERSION;
	// 이벤트 대상인지 확인
	if( pUserInfo->m_bAttendanceTarget == false )																	return EVENT_ERROR_ATTENDANCE_TARGET;
	// 금일 출석을 체크했는지 체크
	DATE32 dt32Today = i3ThreadTimer::LocalTime().GetDateTimeBin( TIME_ATTENDANCE_CHECK );
	if( pUserInfo->m_dt32AttendanceTime != dt32Today )																return EVENT_ERROR_ATTENDANCE_NOT_ENOUGH;

	// 이미 보상을 받았는지
	if( pUserInfo->m_ui8AttendanceItem == pUserInfo->m_ui8AttendanceDay )											return EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED;
		
	T_GoodsID TGoodsID = pEventInfo->m_ui32GiveGoodsID[ pUserInfo->m_ui8AttendanceDay - 1 ].m_ui32GoodsID[ ui8ItemNum ];
	// 있는 상품인지
	if( 0 == TGoodsID )																								return EVENT_ERROR_ATTENDANCE_REWARD_LIST;

	SEND_GIFT	GiftIn;
	GiftIn.m_eGiftType		= GIFT_TYPE_EVENT;
	GiftIn.m_TSenderUID		= 0;
	GiftIn.m_TReceiverUID	= pNode->_i64UID;
	GiftIn.m_ui8GiftCount	= 1;
	GiftIn.m_aGift[0].m_ui32GoodsID = TGoodsID;

	ITEM_INSERT_REQUEST_TYPE eType;

	switch (ui8Type)
	{
	case ATTENDANCE_TYPE_DORMANT: //휴먼 유저 보상
		eType = ITEM_INSERT_REQUEST_DORMANT;
		break;
	case ATTENDANCE_TYPE_NORMAL: //일반 출석체크
		eType = ITEM_INSERT_REQUEST_ATTENDANCE;
		break;
	default:					//예외처리로 일반 출석체크 보상으로 통일
		eType = ITEM_INSERT_REQUEST_ATTENDANCE;
		break;
	}	
	INT32 i32Rv = _GiftInsertDB( &GiftIn, eType);
	if( EV_FAILED( i32Rv) )
	{
		g_pLog->WriteLog( L"ERROR -- Attendance Clear Gift Insert / UID:%I64d, GoodsID:%d, Reason : %x ", pNode->_i64UID, TGoodsID, i32Rv );
		return i32Rv;
	}

	pUserInfo->m_ui8ItemNum			= ui8ItemNum;
	pUserInfo->m_ui8AttendanceItem	= pUserInfo->m_ui8AttendanceDay;
	pNode->_ui32UpdateInfo			|= SAVE_DATABASE_ATTENDANCE;

	return EVENT_ERROR_ATTENDANCE_SUCCESS;
}

void CTaskProcessor::SetMedal(MEDAL* pRecvMedal, USER_NODE* pNode )
{
	for( UINT16 i = 0 ; i < pNode->m_MedalAllInfoDB.m_ui16MedalCount ; i++)
	{
		// 기존에 소지 하고 있던 메달이면 업데이트
		if( pRecvMedal->m_ui16Idx == pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui16Idx )
		{
			i3mem::Copy( &pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal, pRecvMedal, sizeof( MEDAL ) );
			pNode->m_MedalAllInfoDB.m_aMedal[i].m_ui8IsSave = 1;
			return;
		}
	}

	// 기존에 소지 하지 않던 메달이면 추가.
	if( pNode->m_MedalAllInfoDB.m_ui16MedalCount < MAX_MEDAL_COUNT )
	{
		i3mem::Copy( &pNode->m_MedalAllInfoDB.m_aMedal[pNode->m_MedalAllInfoDB.m_ui16MedalCount].m_Medal, pRecvMedal, sizeof( MEDAL ) );
		pNode->m_MedalAllInfoDB.m_aMedal[pNode->m_MedalAllInfoDB.m_ui16MedalCount].m_ui8IsSave = 1;
		pNode->m_MedalAllInfoDB.m_ui16MedalCount++;
	}

	return;
}

void CTaskProcessor::_SendDataFail_Type( ENUM_TYPE_SHOP eType, UINT32 ui32ServerIdx, T_UID i64UID, INT32 i32Result, INT32 i32SessionIdx )
{
	i3NetworkPacket SendPacket;
	switch( eType )
	{
	case TYPE_SHOP_ITEM_DELETE :
		SendPacket.SetProtocol( PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK );
		break;
	case TYPE_SHOP_ITEM_AUTH :
		SendPacket.SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK );
		break;
	case TYPE_SHOP_GIFT_LIST:
		SendPacket.SetProtocol( PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK );
		break;
	case TYPE_SHOP_ITEM_LOOP_AUTH:
		SendPacket.SetProtocol( PROTOCOL_CHEAT_ITEM_AUTH_ACK );
		break;
	default:
		return;
		break;
	}

	SendPacket.WriteData( &i32SessionIdx,		sizeof( INT32 ) );
	SendPacket.WriteData( &i64UID,				sizeof( T_UID ) );
	SendPacket.WriteData( &i32Result,			sizeof( INT32 ) );
	SendGamePacket( ui32ServerIdx, &SendPacket );
}

void CTaskProcessor::_SendDataRepair(  UINT32 ui32ServerIdx,  INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv,  UINT32 ui32TotalPoint  )
{
	i3NetworkPacket	SendPacket( PROTOCOL_SHOP_REPAIR_ACK );
	SendPacket.WriteData( &i32SessionIdx,	sizeof( INT32 ) );
	SendPacket.WriteData( &i64UID,			sizeof( T_UID ) );
	SendPacket.WriteData( &i32Rv,			sizeof( INT32 ) );
	SendPacket.WriteData( &ui32TotalPoint,	sizeof( UINT32 ) );
	SendGamePacket( ui32ServerIdx, &SendPacket );
}

INT32 CTaskProcessor::_ItemInsertDB( T_UID TUID, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT8 ui8Itemcount, ST_ITEM* pDBItem, UINT32 ui32CapsuleItemID, MEDAL_REWARD_INFO* pMedalReward )
{
	if( 0 == ui8Itemcount ) return EVENT_ERROR_FAIL;

	ui8Itemcount = MIN( ui8Itemcount, MAX_SHOP_DB_ITEM_COUNT );

	DB_SHOP_ITEM DBShopItem;

	i3mem::FillZero(&DBShopItem, sizeof(DB_SHOP_ITEM));

	DBShopItem.m_TUID = TUID; 
	DBShopItem.m_stDBItem.m_ui8InsertType = (UINT8)eRequestType;
	DBShopItem.m_ui32CapsuleItemID = ui32CapsuleItemID;
	
	UINT8 ui8RealItemCount = 0;
	// ItemID Check
	for( UINT8 i = 0 ; i < ui8Itemcount; ++i )
	{
		SHOP_ITEM_DATA* pItemData = g_pShop->GetItemByID( pDBItem[i].m_TItemID );	// 해당 Item를 얻어온다.
		if( NULL == pItemData || 0 == pDBItem[i].m_TItemArg )
		{	// 해당 Item 이 없다.
			continue;
		}
		DBShopItem.m_stDBItem.m_aItem[ui8RealItemCount]._ui32ItemID		= pDBItem[i].m_TItemID;
		DBShopItem.m_stDBItem.m_aItem[ui8RealItemCount]._ui32AddItemArg	= pDBItem[i].m_TItemArg;
		if( eRequestType == ITEM_INSERT_REQUEST_MEDAL_REWARD )
		{
			i3mem::Copy( &DBShopItem.m_stMedalReward.m_aDetail[ui8RealItemCount], &pMedalReward->m_aDetail[i], sizeof( MEDAL_REWARD_DETAIL_INFO ) );
		}
		++ui8RealItemCount;
	}

	DBShopItem.m_stDBItem.m_ui8ItemCount  = ui8RealItemCount;
	DBShopItem.m_stMedalReward.m_ui8Count = ui8RealItemCount;

	if (TRUE == IsUseGiftBoxContents(DBShopItem.m_stDBItem.m_ui8InsertType)) //
	{
		TTCHAR strSendNickGM[NET_NICK_NAME_SIZE] = _TT("GM");
		TTCHAR strMessage[MAX_GIFT_MESSAGE_SIZE] = _TT("EVENT_");
		TTCHAR strMessageType[4];

		_itow(eRequestType, strMessageType, 10);

		i3String::Concat(strMessage, strMessageType);
		
		i3String::Copy(DBShopItem.m_strSenderNick, strSendNickGM, NET_NICK_NAME_SIZE);
		i3String::Copy(DBShopItem.m_strMessage, strMessage, MAX_GIFT_MESSAGE_SIZE);
	}

	if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopItem( &DBShopItem ) )
	{	// Error	 
		g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Item Insert / UID : %I64d / InsertType : %d / ItemId:%d / ItemArg : %d / CapsuleItemID : %d", 
			TUID, eRequestType, pDBItem[0].m_TItemID, pDBItem[0].m_TItemArg, ui32CapsuleItemID);
		return EVENT_ERROR_EVENT_BUFFER_FULL;
	}

	return EVENT_ERROR_SUCCESS;
}

INT32 CTaskProcessor::_GiftInsertDB( SEND_GIFT *pGift, ITEM_INSERT_REQUEST_TYPE eType )
{
	if( NULL == pGift ) return EVENT_ERROR_EVENT_CHEKC_GIFT_PARAMETER_GOODS;
	if( 0 == pGift->m_ui8GiftCount || pGift->m_ui8GiftCount > MAX_SHOP_GOODS_BUY_COUNT ) return EVENT_ERROR_EVENT_BUY_GOODS_GOODSCOUNT;

	pGift->m_eRequestType = eType;
	ST_ITEM aItem[MAX_SHOP_AUTH_GIFT_ITEM_COUNT];
	UINT8	ui8ItemCount = 0;

	for( UINT8 i = 0 ; i < pGift->m_ui8GiftCount ; i++ )
	{
		S2MO_RECV_GIFT* pRecvGift = &pGift->m_aGift[i];

		SHOP_GOODS_LIST*	pGoods = g_pShop->GetGoodsByID( pRecvGift->m_ui32GoodsID );		// GoodsID 로 Goods 를 얻어온다.
		if(	NULL == pGoods ) 
		{	// 해당 Goods가 없다
			return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;
		}

		pRecvGift->m_i32Rv		= EVENT_ERROR_FAIL;
		pRecvGift->m_TGiftDBIdx	= 0;

		 UINT8 ui8Count = g_pShop->GetItemInfo_byGoodsID( pRecvGift->m_ui32GoodsID, &aItem[ui8ItemCount] );
		 ui8ItemCount += ui8Count;
	}
	
	TTCHAR strSendNickGM[NET_NICK_NAME_SIZE ] = _TT("GM");
	TTCHAR strMessage[MAX_GIFT_MESSAGE_SIZE ] = _TT("EVENT_");
	TTCHAR strMessageType[4];

	_itow( eType, strMessageType, 10 );
	
	i3String::Concat( strMessage, strMessageType );

	switch( pGift->m_eGiftType ) 
	{
	case GIFT_TYPE_EVENT : 
	case GIFT_TYPE_GM : 
		i3String::Copy( pGift->m_strSenderNick, strSendNickGM, NET_NICK_NAME_SIZE  );
		i3String::Copy( pGift->m_strMessage, strMessage, MAX_GIFT_MESSAGE_SIZE  );
		break;
	}

	
	if( g_pConfig->IsContentsEnabled( CONTENTS_GIFT ) )		
	{	// 선물 시스템 ON - 선물함 지급
		if( !g_pModuleDBShop->InInsertGift( pGift ) )
		{	// Error
			g_pLog->WriteLog( L"[ERROR] INSERT GIFT - GIFT BOX / UID:%I64d / GoodsID:%d / Type : %d", 
				pGift->m_TReceiverUID, pGift->m_aGift[0].m_ui32GoodsID, eType );
			return EVENT_ERROR_EVENT_BUFFER_FULL;
		}
	}
	else
	{
		if (ITEM_INSERT_REQUEST_USER_GIFT == eType)
		{
			if (!g_pModuleDBShop->InInsertGift(pGift)) //선물샵만 오픈 되어 있는 경우
			{	// Error
				g_pLog->WriteLog(L"[ERROR] INSERT GIFT - GIFT BOX / UID:%I64d / GoodsID:%d / Type : %d",
					pGift->m_TReceiverUID, pGift->m_aGift[0].m_ui32GoodsID, eType);
				return EVENT_ERROR_EVENT_BUFFER_FULL;
			}

		}
		else
		{
			// 선물 시스템 OFF - 인벤토리 지급
			// 인벤토리로 지급 
			INT32 i32Rv = _ItemInsertDB(pGift->m_TReceiverUID, eType, ui8ItemCount, aItem);
			if (EV_FAILED(i32Rv))
			{
				g_pLog->WriteLog(L"[ERROR] INSERT GIFT - INVENTORY / UID:%I64d / GoodsID:%d / Type : %d / Rv : %x",
					pGift->m_TReceiverUID, pGift->m_aGift[0].m_ui32GoodsID, eType, i32Rv);
				return i32Rv;
			}
		}
	}

	return EVENT_ERROR_SUCCESS;
}

float	CTaskProcessor::_GetRepairPremium( UINT32 ui32Endurance )
{
	if( 90 < ui32Endurance )		return 0.f;
	else if( 10 < ui32Endurance )	return 0.1f;
	else							return 0.25f;
}

void CTaskProcessor::_MakeMedalInfoPacket(i3NetworkPacket *pPacket, USER_NODE *pNode )
{
	if( !g_pConfig->IsContentsEnabled( CONTENTS_MEDAL) )	return;

	// Medal Info.
	PACKET_D_GET_USER_INFO_MEDAL	PacketMedal;
	i3mem::Copy( PacketMedal.m_aNotifyMedal, pNode->m_MedalAllInfoDB.m_aNotifyMedal, sizeof(NOTIFY_MEDAL)*MAX_NOTIFY_MADAL_COUNT  );
	PacketMedal.m_ui16MedalCount = pNode->m_MedalAllInfoDB.m_ui16MedalCount;
	PacketMedal.m_ui16CurMedalSetCount = pNode->m_MedalAllInfoDB.m_ui16CurMedalSetCount;
	
	pPacket->WriteData( &PacketMedal,	sizeof(PACKET_D_GET_USER_INFO_MEDAL)  );

	// 2. Nomal Medal
	for( INT32 i = 0 ; i < PacketMedal.m_ui16MedalCount; ++i)
	{
		pPacket->WriteData( &pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal,	sizeof(MEDAL));
	}

	// 3. Current MedalSet
	pPacket->WriteData( pNode->m_MedalAllInfoDB.m_aCurMedalSet,	sizeof(CUR_MEDAL_SET) * PacketMedal.m_ui16CurMedalSetCount );
}

void CTaskProcessor::_SetUserDetailInfo( S2_USER_DETAIL_INFO& stInfo, USER_NODE* pNode )
{
	stInfo.m_TUID				= pNode->_i64UID;
	stInfo.m_ui32ClanMark		= pNode->_sClan._ui32Mark;	
	stInfo.m_ui8Rank			= (UINT8)pNode->_sBasic.m_ui32Rank;
	stInfo.m_ui32Exp			= pNode->_sBasic.m_ui32Exp;
	stInfo.m_ui32Match			= pNode->_sRecord._smatch;
	stInfo.m_ui32Win			= pNode->_sRecord._swin;
	stInfo.m_ui32Draw			= pNode->_sRecord._sdraw;
	stInfo.m_ui32Lose			= pNode->_sRecord._slose;
	stInfo.m_ui32DisConnect		= pNode->_sRecord._sdis;
	stInfo.m_ui32Kill			= pNode->_sRecord._skillcount;
	stInfo.m_ui32Death			= pNode->_sRecord._sdeath;
	stInfo.m_ui32HeadShot		= pNode->_sRecord._sheadshot;
	stInfo.m_ui32State			= GET_FRIEND_STATE(pNode->_ui32ConnectState);
	stInfo.m_ui32ServerIdx		= GET_FRIEND_SERVER(pNode->_ui32ConnectState);
	stInfo.m_ui32ChannelNum		= GET_FRIEND_CHANNEL(pNode->_ui32ConnectState);
	stInfo.m_ui32RoomIdx		= GET_FRIEND_ROOMIDX(pNode->_ui32ConnectState);
	stInfo.m_ui32Assist			= pNode->_sRecord._sAssistCount;
	//stInfo.m_i8CharCount		= (INT8)pNode->m_MultiSlot.GetCharSlotCount();

	S2MO_CHAR_BASE_INFO		BasicInfo;
	ITEM_INFO				EquipInfo[CHAR_EQUIPMENT_COUNT];

	INT8 i8CharInfoIdx = 0;

	for( INT8 i = 0 ; i < S2MO_MULTI_SLOT_COUNT; ++i )
	{
		//if (i >= S2MO_MULTI_SLOT_PACKET_COUNT) break;
		if( FALSE == pNode->m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_USING) ) continue;

		pNode->m_MultiSlot.GetCharBaseInfo( i, &BasicInfo );
		pNode->m_MultiSlot.GetCharEquipmentAll( i, EquipInfo );

		stInfo.m_aCharInfo[i8CharInfoIdx].m_ui8SlotIdx	= i;
		stInfo.m_aCharInfo[i8CharInfoIdx].m_TItemID			= EquipInfo[CHAR_EQUIPMENT_CHARA].m_TItemID;
		stInfo.m_aCharInfo[i8CharInfoIdx].m_ui32CreateDate	= BasicInfo.m_ui32CreateDate;
		stInfo.m_aCharInfo[i8CharInfoIdx].m_ui32BattleTime  = BasicInfo.m_ui32BattleTime;

		i3String::Copy( stInfo.m_aCharInfo[i8CharInfoIdx].m_strCharName, BasicInfo.m_strCharName, NET_NICK_NAME_SIZE );

		pNode->m_MultiSlot.GetCharSkillInfo( i, &stInfo.m_aCharInfo[i8CharInfoIdx].m_SkillInfo );
		
		++i8CharInfoIdx;
	}

	stInfo.m_i8CharCount = i8CharInfoIdx;
		
	i3mem::Copy( &stInfo.m_LastGame, &pNode->m_LastGame, sizeof(USER_INFO_LAST_GAME) );
	i3mem::Copy( &stInfo.m_InvenData, &pNode->_sInvenData, sizeof(USER_INFO_INVITEM_DATA) );

	i3String::Copy( stInfo.m_strNickName, pNode->_sBasic.m_strNick, NET_NICK_NAME_SIZE );
	i3String::Copy( stInfo.m_strClanName, pNode->_sClan._strClanName, NET_CLAN_NAME_SIZE );
}

void CTaskProcessor::_SendDataRepair( T_UID i64UID, S2MO_REPAIR_INFO* pstInfo )
{
	USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(i64UID);
	if( NULL == pNode ) return;

	i3NetworkPacket	SendPacket( PROTOCOL_SHOP_REPAIR_ACK );
	SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof( INT32 ) );
	SendPacket.WriteData( &pNode->_i64UID,					sizeof( T_UID ) );	
	
	char strTemp[MAX_PATH];
	UINT32 ui32TotalCash = 0;
	for( UINT8 i = 0 ; i < pstInfo->m_ui8ItemCount ; i++ )
	{
		S2MO_REPAIR_ITEM *pItem = &pstInfo->m_stRepairItem[i];

		if( EV_FAILED( pItem->m_i32Rv ))	continue;
		
		ui32TotalCash += pItem->m_ui32RepairCash;

		// 인벤토리 갱신
		if( 0 == pItem->m_TItemID ) continue;
		for( INT32 k = 0 ; k < MAX_INVEN_COUNT ; k++ )
		{
			if( pItem->m_TItemID == pNode->_Inven[k]._ui32ItemID )
			{
				pNode->_Inven[k]._ui32ItemArg = pItem->m_ui32AuthArg;
				break;
			}
		}

		//수리 성공이 떨어졌을때 남는것.
		i3String::Format(strTemp, MAX_PATH, "REPAIR,ACK, ID = %s, uid = %I64d  Point = %d Cash = %d ItemID = %d ItemArg = %d DBIdx = %d",
			pNode->_strID, pNode->_i64UID, pItem->m_ui32RepairPoint, pItem->m_ui32RepairCash, pItem->m_TItemID, pItem->m_ui32AuthArg, pItem->m_TItemDBIdx ); 
		WRITE_SERVICE_LOG_REPAIR_TASK(strTemp);	
	}

	//// cash 차감.
	//_UpdateCash( pNode, ui32TotalCash ,ui32LinkUserCash, eCashType );

	if( pNode->_ui32Cash < ui32TotalCash )	ui32TotalCash = pNode->_ui32Cash;
	pNode->_ui32Cash	-= ui32TotalCash;
	m_ui32UseCash		+= ui32TotalCash;
	m_ui32TodayUseCash	+= ui32TotalCash;

	// Send Packet
	SendPacket.WriteData( &pNode->_ui32Cash,				sizeof( UINT32 ) );
	SendPacket.WriteData( &pstInfo->m_ui8ItemCount,			sizeof( UINT8 ) );
	SendPacket.WriteData( pstInfo->m_stRepairItem,			sizeof(S2MO_REPAIR_ITEM)*pstInfo->m_ui8ItemCount );	
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );

	return;
}

void CTaskProcessor::_MakeUserInfoPacket( i3NetworkPacket* pPacket, USER_NODE *pNode, BOOL bClan   )
{
	pPacket->WriteData( &pNode->m_ui8PBVer,						sizeof(UINT8));
	pPacket->WriteData( &pNode->_ui32BirthDay,					sizeof(UINT32));
	pPacket->WriteData( &pNode->_ui8Gender,						sizeof(UINT8));
	pPacket->WriteData( &pNode->_ui32Cash,						sizeof(UINT32));
	//pPacket->WriteData( &pNode->_ui8RoleFlag,					sizeof(UINT8));
	pPacket->WriteData( &pNode->_ui8ESportIDFlag,				sizeof(UINT8));

	pPacket->WriteData( &pNode->_sBasic,						sizeof(USER_INFO_BASIC_BASE));
	pPacket->WriteData( &pNode->m_stUserInfoServer,				sizeof(USER_INFO_SERVER));

	bool bGiftAutoAuth = pNode->m_bGiftAutoAuth;	// 로그인시에만 Node에 셋팅된 값을 줍니다.
	if( bClan )
	{
		pPacket->WriteData( &pNode->_sClan,						sizeof(USER_INFO_CLAN));
		bGiftAutoAuth = false;
	}	

	ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];								// 장착 정보
	pNode->m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
	pPacket->WriteData( aCommonEquips,							sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );

	UINT8 ui8RedSlot = (UINT8)pNode->m_MultiSlot.GetTeamSlot(TEAM_RED);
	UINT8 ui8BlueSlot = (UINT8)pNode->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
	pPacket->WriteData( &ui8RedSlot,							sizeof(UINT8) );
	pPacket->WriteData( &ui8BlueSlot,							sizeof(UINT8) );

	pPacket->WriteData( &pNode->_sRecord,						sizeof(USER_INFO_RECORD));
	pPacket->WriteData( &pNode->_sInvenData,					sizeof(USER_INFO_INVITEM_DATA)		);
	/*pPacket->WriteData( &pNode->_sQuest,						sizeof(USER_INFO_QUEST));*/
 
	//
	pPacket->WriteData( &pNode->m_dt32LoginDate,				sizeof(DATE32) );
	//ZLog 에서 사용할 정보를 추가 - 서동권 2011.07.15
	pPacket->WriteData( &pNode->_ui32TotalConnectTime,			sizeof(UINT32) );
	pPacket->WriteData( &pNode->_ui32TotalConnectCount,			sizeof(UINT32) );
	pPacket->WriteData( &pNode->m_dt32LastLogInDate,			sizeof(DATE32) );
	pPacket->WriteData( &pNode->_sDaily,						sizeof(USER_INFO_DAILY) );
	pPacket->WriteData( &pNode->m_ui8Country,					sizeof(UINT8) );
	pPacket->WriteData( pNode->_Attendance,						sizeof( ATTENDANCE_USER_SVR ) * ATTENDANCE_TYPE_MAX );
	pPacket->WriteData( pNode->m_stAttendanceInfo,				sizeof( ATTENDANCE_INFO ) * ATTENDANCE_TYPE_MAX );

	pPacket->WriteData( pNode->m_aQuickJoinInfo,				sizeof( QUICKJOIN_INFO )*MAX_QUICK_JOIN_INFO_COUNT );
	_MakeMedalInfoPacket( pPacket, pNode );

	pPacket->WriteData( &pNode->m_bAASDailyUsageWay,			sizeof(bool));
	pPacket->WriteData( &pNode->m_bAASAppointedWay,				sizeof(bool));
	pPacket->WriteData( &pNode->m_ui16AASAccMin,				sizeof(UINT16));

	pPacket->WriteData( pNode->m_aui32AASBanTime,				sizeof(UINT32)*DAY_COUNT_IN_WEEK);

	pPacket->WriteData( &bGiftAutoAuth,							sizeof(bool));

#ifdef ENABLE_GACHA_SHOP
	INT32 i32GachaPurchaseRecordCnt = pNode->GetGachaPurchaseRecordCnt();
	pPacket->WriteData( &i32GachaPurchaseRecordCnt,	sizeof(INT32));

	for( INT32 i = 0; i < i32GachaPurchaseRecordCnt; ++i )
	{
		UINT16 ui16ItemKey = 0;		// 패킷 사이즈 축소를 위해 줄입니다.
		UINT16 ui16PurchaseCnt = 0; // 패킷 사이즈 축소를 위해 줄입니다.

		CPurchaseRecord* pPurchaseRecord = pNode->m_GachaPurchase.GetPurchaseRecord(i);
		if( pPurchaseRecord )
		{
			ui16ItemKey		= (UINT16)pPurchaseRecord->GetItemKey();
			ui16PurchaseCnt = (UINT16)pPurchaseRecord->GetPurchaseCount();
		}

		pPacket->WriteData( &ui16ItemKey,						sizeof(UINT16));
		pPacket->WriteData( &ui16PurchaseCnt,					sizeof(UINT16));
	}
#else
	INT32 i32GachaPurchaseRecordCnt = 0;
	pPacket->WriteData(&i32GachaPurchaseRecordCnt, sizeof(INT32));
#endif
	return;
}

void CTaskProcessor::_SendUserInvenPacket(  USER_NODE *pNode, UINT16 ui16InvenCount )
{
	i3NetworkPacket	SendPacket(PROTOCOL_BASE_GET_INVEN_INFO_ACK);
	 
	SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
	SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
	SendPacket.WriteData( &ui16InvenCount,				sizeof(UINT16) );
	UINT16 ui16Inven = 0;
	for( INT32 i = 0 ; i < MAX_INVEN_COUNT ; i++ )
	{
		if( 0 == pNode->_Inven[ i ]._ui32ItemID )		continue;

		SendPacket.WriteData( &pNode->_Inven[i],					sizeof(INVEN_BUFFER));
		ui16Inven++;
		if( ui16InvenCount == ui16Inven ) break;
	}
	SendPacket.WriteData(&pNode->_ui8RoleFlag, sizeof(UINT8));
	SendPacket.WriteData(&pNode->_sQuest, sizeof(USER_INFO_QUEST));

	// Rank를 보낸다.
	SendPacket.WriteData(&pNode->_sBasic.m_ui32Rank, sizeof(UINT32));
	
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
	 
	return;
}

void CTaskProcessor::_SendUserCharaPacket(  USER_NODE *pNode )
{
	INT32 i32Count	= pNode->m_MultiSlot.GetCharSlotCount();
	if( 0 == i32Count )
	{
		i3NetworkPacket	SendPacket(PROTOCOL_BASE_GET_CHARA_INFO_ACK);
		SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof(INT32) );
		SendPacket.WriteData( &pNode->_i64UID,				sizeof(T_UID) );
		SendPacket.WriteData( &i32Count,				                                                                                            	sizeof(INT32) );
		SendGamePacket( pNode->_i32Serveridx, &SendPacket );
		return;
	}

	UINT8					ui8SlotState;
	S2MO_CHAR_BASE_INFO		CharBaseInfo;													// 유저 기본 정보
	S2MO_CHAR_SKILL_INFO	CharSkillInfo;
	ITEM_INFO				aCharEquips[ CHAR_EQUIPMENT_COUNT ];							// 장착 정보
	UINT8					ui8CharNickSize;

	// 활성화된 슬롯 체크.
	INT32 i32ActiveSlotCount = 0;
	INT32 i32ActiveSlot[S2MO_MULTI_SLOT_COUNT] = {0,};
	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		if( FALSE == pNode->m_MultiSlot.IsActiveChar( i ) )	continue;
		i32ActiveSlot[i32ActiveSlotCount] = i;
		++i32ActiveSlotCount;
	}


	i3NetworkPacket	SendPacket(PROTOCOL_BASE_GET_CHARA_INFO_ACK);
	SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof(INT32) );
	SendPacket.WriteData( &pNode->_i64UID,				sizeof(T_UID) );
	SendPacket.WriteData( &i32Count,					sizeof(INT32) );		// 이번 패킷에서 보낼 캐릭터수
	
	INT32 i32CharaPacketSize =	( sizeof(UINT8) * 3 ) + ( sizeof(UINT32) * 4 )  + ( sizeof(TTCHAR) * NET_NICK_NAME_SIZE ) +
		sizeof(S2MO_CHAR_SKILL_INFO) + ( sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT  );

	INT32 i32MaxPacketSize = PACKETBUFFERSIZE - PACKETHEADERSIZE - ( sizeof(INT32) * 2 ) - sizeof(T_UID);

	INT32 i32WriteCharaSize = 0;
	i32Count = 0;
	for( INT32 i = 0 ; i < i32ActiveSlotCount ; i++ )
	{
		if( SendPacket.GetDataFieldSize() + i32CharaPacketSize >= i32MaxPacketSize  )
		{	
			SendPacket.WriteData( sizeof(INT32) + sizeof(T_UID) , &i32Count,						sizeof(INT32) );
			SendGamePacket( pNode->_i32Serveridx, &SendPacket );

			i32WriteCharaSize = 0;
			i32Count = 0;
			SendPacket.Clear();
			SendPacket.SetProtocol( PROTOCOL_BASE_GET_CHARA_INFO_ACK);
			SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof(INT32) );
			SendPacket.WriteData( &pNode->_i64UID,				sizeof(T_UID) );
			SendPacket.WriteData( &i32Count,					sizeof(INT32) );		// 이번 패킷에서 보낼 캐릭터수
		}

		UINT8 ui8SlotIdx = (UINT8)i32ActiveSlot[i];

		pNode->m_MultiSlot.GetCharBaseInfo( ui8SlotIdx, &CharBaseInfo );
		pNode->m_MultiSlot.GetCharSkillInfo( ui8SlotIdx, &CharSkillInfo );
		pNode->m_MultiSlot.GetCharEquipmentAll( ui8SlotIdx, aCharEquips );

		ui8CharNickSize = (UINT8)i3String::Length( CharBaseInfo.m_strCharName );

		ui8SlotState	= (UINT8)pNode->m_MultiSlot.GetActive(ui8SlotIdx);

		SendPacket.WriteData( &ui8SlotIdx,							sizeof(UINT8) );
		SendPacket.WriteData( &ui8SlotState,						sizeof(UINT8) );
		SendPacket.WriteData( &CharBaseInfo.m_i32WeaponDBIdx,		sizeof(INT32) );
		SendPacket.WriteData( &CharBaseInfo.m_ui32CreateDate,		sizeof(UINT32) );
		SendPacket.WriteData( &CharBaseInfo.m_ui32BattleTime,		sizeof(UINT32) );
		SendPacket.WriteData( &CharBaseInfo.m_ui32SkillRelTime,		sizeof(UINT32) );
		SendPacket.WriteData( &ui8CharNickSize,						sizeof(UINT8) );
		SendPacket.WriteData( CharBaseInfo.m_strCharName,			sizeof(TTCHAR) * ui8CharNickSize );
		SendPacket.WriteData( &CharSkillInfo,						sizeof(S2MO_CHAR_SKILL_INFO) );
		SendPacket.WriteData( aCharEquips,							sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT );
			
		++i32Count;
	}

	SendPacket.WriteData( sizeof(INT32) + sizeof(T_UID) , &i32Count,						sizeof(INT32) );
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );

	return;
}


INT32 CTaskProcessor::_CheckAndSet_Goods( T_UID TUID, DB_GOODS* pDBGoods, UINT8 ui8ItemInsertType )
{
	SHOP_GOODS_LIST *pGoods			= g_pShop->GetGoodsByID( pDBGoods->m_TGoodsID );
	if( NULL == pGoods )
	{	// 해당 Goods가 없다. // 말이 안되는 현상
		g_pLog->WriteLog( L"ERROR - _CheckBuyGoods No Goods / GoodsID : %d / UID : %I64d", pDBGoods->m_TGoodsID, TUID );
		return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;
	}
  
	// 결제 유형 체크. 구매일때만 검사합니다.
	switch( pDBGoods->m_ui8BuyType )
	{
	case GOODS_BUY_POINT:
		{
			if( 1 > pGoods->_pGoodsData->GetPoint() )
			{
				g_pLog->WriteLog( L"ERROR - _CheckBuyGoods No PointItem / GoodsID : %d / UID : %I64d", pDBGoods->m_TGoodsID, TUID );	
				return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;
			}
		}
		break;
	case GOODS_BUY_CASH:
		{
			if( 1 > pGoods->_pGoodsData->GetCash() )
			{
				g_pLog->WriteLog( L"ERROR - _CheckBuyGoods No CashItem / GoodsID : %d / UID : %I64d ", pDBGoods->m_TGoodsID, TUID );
				return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;
			}
		}
		break;
	default:
		break;
	}

	// Goods 에 Item 을 얻어온다.
	if( 0 == pGoods->_ui16ItemCount ) return EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;
	pDBGoods->m_stDBItem.m_ui8ItemCount		= (UINT8)pGoods->_ui16ItemCount;
	pDBGoods->m_stDBItem.m_ui8InsertType	= ui8ItemInsertType;

	for( TItemCnt i = 0 ; i < pGoods->_ui16ItemCount ; i++ )
	{
		SHOP_ITEM_DATA* pItemData = pGoods->_ppItemData[ i ];

		if( NULL == pItemData ) 
		{
			g_pLog->WriteLog( L"ERROR - _CheckBuyGoods Goods No Have Item / GoodsID : %d / UID : %I64d ", pDBGoods->m_TGoodsID, TUID );
			return EVENT_ERROR_EVENT_BUY_GOODS_NOT_ALLOWED;
		}

		pDBGoods->m_stDBItem.m_aItem[i]._ui32ItemID			= pItemData->_ui32ItemID;
		pDBGoods->m_stDBItem.m_aItem[i]._ui32AddItemArg		= pGoods->_pui32ItemArg[ i ];
	}


	return EVENT_ERROR_SUCCESS;
}

void CTaskProcessor::_SendDataShopBuy(  USER_NODE *pNode, DB_SHOP_GOODS *pShopBuy, TYPE_CURRENT_USER_CASH eCashType, UINT32 ui32LinkUserCash )
{
	char strLog[MAX_PATH];

	UINT32 ui32Cash = 0;		// 정상 처리된 상품들의 캐쉬 합.
	UINT32 ui32FailPoint = 0;	// 실패 처리된 상품들의 포인트 합.
	for( UINT8 i = 0 ; i < pShopBuy->m_ui8GoodsCount ; i++ )
	{
		SHOP_GOODS_LIST *pGoods = g_pShop->GetGoodsByID( pShopBuy->m_stDBGoods[i].m_TGoodsID );

		if (!pGoods) return;

		UINT32 ui32GoodsPoint	= pGoods->_pGoodsData->GetPoint();
		UINT32 ui32GoodsCash	= pGoods->_pGoodsData->GetCash();

		if( EV_FAILED( pShopBuy->m_stDBGoods[i].m_i32Rv )) 
		{	// 결과가 실패이면 포인트 롤백(Game서버)을 위해 계산합니다.
			switch ( pShopBuy->m_stDBGoods[i].m_ui8BuyType ) 
			{
				case GOODS_BUY_POINT :	ui32FailPoint += ui32GoodsPoint;	break;
			}
			continue; 
		}

		// 유저 인벤토리 업데이트.
		INT64 i64CharaSync = 0;		// 셋트 상품에 캐릭터가 포함된 경우를 대비해 플래그를 켜줍니다.
		for( UINT8 k = 0 ; k < pShopBuy->m_stDBGoods[i].m_stDBItem.m_ui8ItemCount ; k++ )
		{
			INVEN_BUFFER InvenBuffer;
			i3mem::Copy( &InvenBuffer, &pShopBuy->m_stDBGoods[i].m_stDBItem.m_aItem[k], sizeof( INVEN_BUFFER ) );
			UserInsertItem( pNode, &InvenBuffer, i64CharaSync);
		}

		// 상품 금액 계산. 
		switch ( pShopBuy->m_stDBGoods[i].m_ui8BuyType ) 
		{
		case GOODS_BUY_CASH :
			{
				ui32Cash += ui32GoodsCash;
			}
			break;
		}

		if( pShopBuy->m_stDBGoods[i].m_ui8BuyType != GOODS_BUY_NONE )
		{
			//구입 성공이 떨어졌을때 남는것.
			i3String::Format( strLog, MAX_PATH, "BUYGOODS,ACK,ID = %s, uid = %I64d, GoodsId = %d, BuyType = %d, Cash = %d, Point = %d ",\
				pNode->_strID, pNode->_i64UID, pShopBuy->m_stDBGoods[i].m_TGoodsID, pShopBuy->m_stDBGoods[i].m_ui8BuyType, ui32GoodsCash, ui32GoodsPoint ); 
			WRITE_SERVICE_LOG_BUY_TASK(strLog); 
			++pGoods->_pGoodsData->_ui32SaleCount;
		}
	} 

	_UpdateCash( pNode, ui32Cash,ui32LinkUserCash, eCashType );
	
	i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK) ;
	SendPacket.WriteData( &pNode->_i32Sessionidx,				sizeof(INT32)	);
	SendPacket.WriteData( &pNode->_i64UID,						sizeof(T_UID)	);
	SendPacket.WriteData( &ui32FailPoint,						sizeof(UINT32)	);	// 실패한 항목들의 Point 합( 롤백용 )
	SendPacket.WriteData( &pNode->_ui32Cash,					sizeof(UINT32)	);	// 유저의 Cash 잔액.
	SendPacket.WriteData( &pShopBuy->m_ui8JobAction,			sizeof(UINT8)	);
	SendPacket.WriteData( &pShopBuy->m_ui8GoodsCount,			sizeof(UINT8)	);
	SendPacket.WriteData( pShopBuy->m_stDBGoods,				sizeof(DB_GOODS) * pShopBuy->m_ui8GoodsCount	);
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

void CTaskProcessor::_SendGiftSender(  USER_NODE *pNode, SEND_GIFT* pGift, TYPE_CURRENT_USER_CASH eCashType, UINT32 ui32LinkUserCash )
{
	if( NULL == pNode || NULL == pGift ) return;

	char strLog[MAX_PATH];

	S2MO_SHOP_BUY_RESULT		m_aResult[MAX_SHOP_GOODS_BUY_COUNT];	// 구입 결과.

	UINT32 ui32Cash = 0;		// 정상 처리된 상품들의 캐쉬 합.
	UINT32 ui32FailPoint = 0;	// 실패 처리된 상품들의 포인트 합.

	for( UINT8 i = 0 ; i < pGift->m_ui8GiftCount ; i++ )
	{
		m_aResult[i].m_i32Rv	= pGift->m_aGift[i].m_i32Rv;
		m_aResult[i].m_TGoodsID	= pGift->m_aGift[i].m_ui32GoodsID;
		m_aResult[i].m_ui8Action = pGift->m_aGift[i].m_ui8BuyType; // 선물하기시는 이값으로 대체.

		UINT32 ui32GoodsPoint	= 0;
		UINT32 ui32GoodsCash	= 0;

		SHOP_GOODS_LIST *pGoods = g_pShop->GetGoodsByID( pGift->m_aGift[i].m_ui32GoodsID );
		if( !pGoods ) continue;

		ui32GoodsPoint	= pGoods->_pGoodsData->GetPoint(PRICE_TYPE_GIFT_REAL);
		ui32GoodsCash	= pGoods->_pGoodsData->GetCash(PRICE_TYPE_GIFT_REAL);

		if( EV_FAILED( pGift->m_aGift[i].m_i32Rv ))
		{
			switch ( pGift->m_aGift[i].m_ui8BuyType ) 
			{
				case GOODS_BUY_POINT :	ui32FailPoint += ui32GoodsPoint;	break;
			}
			continue; 
		}

		// 상품 금액 계산.
		
		switch ( pGift->m_aGift[i].m_ui8BuyType ) 
		{ 
		case GOODS_BUY_CASH :
			{
				ui32Cash += ui32GoodsCash;
			}
			break;
		}

		//선물 구입 성공이 떨어졌을때 남는것.
		i3String::Format( strLog, MAX_PATH, "GIFTGOODS,ACK,ID = BuyType =%d, SenderUID = %I64d, ReceiverUID = %I64d, GoodsId = %d, Cash = %d, Point = %d ",
			pGift->m_aGift[i].m_ui8BuyType , pGift->m_TSenderUID, pGift->m_TReceiverUID, pGift->m_aGift[i].m_ui32GoodsID, ui32GoodsCash, ui32GoodsPoint ); 
		WRITE_SERVICE_LOG_GIFT_TASK(strLog); 
		++pGoods->_pGoodsData->_ui32GiftCount;
	} 

	// 유저 캐쉬 갱신 및 일일 캐쉬판매량 누적.
	_UpdateCash( pNode, ui32Cash,ui32LinkUserCash, eCashType );

	i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK) ;
	SendPacket.WriteData( &pNode->_i32Sessionidx,	sizeof(INT32)	);
	SendPacket.WriteData( &pNode->_i64UID,			sizeof(T_UID)	);
	SendPacket.WriteData( &ui32FailPoint,			sizeof(UINT32)	);	// 실패한 항목들의 Point 합. ( 롤백용 )
	SendPacket.WriteData( &pNode->_ui32Cash,			sizeof(UINT32)	);	// 유저의 Cash 잔액.
	SendPacket.WriteData( &pGift->m_ui8GiftCount,	sizeof(UINT8)	);
	SendPacket.WriteData( m_aResult,				sizeof(S2MO_SHOP_BUY_RESULT) * pGift->m_ui8GiftCount	);
	if (!g_pConfig->IsContentsEnabled(CONTENTS_GIFT))
	{
		SendPacket.WriteData(&pGift->m_TReceiverUID, sizeof(T_UID));
		UINT8	ui8MessageSize = (UINT8)i3String::Length(pGift->m_strMessage);
		SendPacket.WriteData(&ui8MessageSize,		sizeof(UINT8));
		SendPacket.WriteData(pGift->m_strMessage,	sizeof(TTCHAR) * ui8MessageSize);
	}

	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

void CTaskProcessor::_SendFailCreateChara(USER_NODE *pNode, INT32 i32Rv )
{
	T_GoodsID TGoodsID = 0;

	i3NetworkPacket	SendPacket( PROTOCOL_CHAR_CREATE_CHARA_ACK );
	SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof( INT32 ) );
	SendPacket.WriteData( &pNode->_i64UID,					sizeof( T_UID ) );
	SendPacket.WriteData( &TGoodsID,						sizeof( T_GoodsID ) ); 
	SendPacket.WriteData( &pNode->_ui32Cash,				sizeof( UINT32));
	SendPacket.WriteData( &i32Rv,							sizeof( INT32 ) ); 
	SendGamePacket(pNode->_i32Serveridx, &SendPacket );
}


void CTaskProcessor::_UpdateCash( USER_NODE *pNode, UINT32 ui32GoodsPrice, UINT32 ui32LinkUserCash ,TYPE_CURRENT_USER_CASH eCashType)
{
	if( !pNode ) return;
	if( ui32GoodsPrice == 0 ) return;

	// 유저 캐쉬 차감. 포인트는 Game 서버에서 합니다.
	if(TYPE_CURRENT_USER_CASH_USE == eCashType)
	{
		if( ui32LinkUserCash	>=	(pNode->_ui32Cash - ui32GoodsPrice))
		{
			pNode->_ui32Cash	= ui32LinkUserCash;
		}
		else
		{
			if( pNode->_ui32Cash < ui32GoodsPrice )				ui32GoodsPrice	= pNode->_ui32Cash;
			pNode->_ui32Cash	-= ui32GoodsPrice;
		}
	}
	else
	{
		if( pNode->_ui32Cash < ui32GoodsPrice )				ui32GoodsPrice	= pNode->_ui32Cash;
		pNode->_ui32Cash	-= ui32GoodsPrice;
	}

	m_ui32UseCash		+= ui32GoodsPrice;
	m_ui32TodayUseCash	+= ui32GoodsPrice;

}

void CTaskProcessor::SetIPBlockOpenList( BOOL bUsed, INT32 i32IPCount, IPBLOCK_IP* pOpenIP, INT32 i32NationCount, UINT8* pui8OpenNation )
{
	if( NULL == m_pIPChecker )	return;
	m_pIPChecker->SetOpenList( bUsed, i32IPCount, pOpenIP, i32NationCount, pui8OpenNation );
}


GACHA_ERROR CTaskProcessor::RecvGachaPurchaseReq(USER_NODE *pUser, PACKET_GACHA_SS_PURCHASE_REQ* pReq)
{
	SHOP_GOODS_DATA* pGoods = g_pShop->GetGoodsData( pReq->m_TGoodsID ); 
	if( !pGoods )
	{
		return GACHA_ERROR_INVALID_REQUEST;
	}

	DB_GACHA	stDBGacha;
	i3mem::Copy( &stDBGacha, pReq, sizeof( GACHA_PURCHACE_BASE ) );
	stDBGacha.m_i32Rv = EVENT_ERROR_GACHA_SERVER_ERROR;
	stDBGacha.m_TUID = pUser->_i64UID;
	stDBGacha.m_ui32GachaGoodsID = pReq->m_TGoodsID;
	
	switch( pReq->m_ui8BuyType )
	{
	case GOODS_BUY_POINT :
		{
			stDBGacha.m_ui32GachaPrice = pGoods->GetPoint();
			stDBGacha.m_ui8Action = TYPE_GACHA_PROVIDE;		 
			ProvideGachaItem( pUser, &stDBGacha );
		}
		break;
	case GOODS_BUY_CASH :
		{
			stDBGacha.m_ui32GachaPrice = pGoods->GetCash();
			stDBGacha.m_ui8Action = TYPE_GACHA_PURCHASE;					 
		}
		break;
	case GOODS_BUY_COIN :
		{
			stDBGacha.m_ui32GachaPrice = pGoods->GetCoin();
			// 기획문서 나오면 재작업 필요.
		}
		break;
	default :
		return GACHA_ERROR_INVALID_REQUEST;
	}

	if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGacha( &stDBGacha ) )
	{	 
		g_pLog->WriteLog( L"BUFFER OVER - DB Gacha / Action : %d / UID : %I64d / BuyType : %d / GoodsID : %d / ItemIdx : %d ", 
			stDBGacha.m_ui8Action, pUser->_i64UID, pReq->m_ui8BuyType, pReq->m_TGoodsID, pReq->m_i32ItemIdx );
		return GACHA_ERROR_INTERNAL;
	}	

	return GACHA_ERROR_SUCCESS;
}

GACHA_ERROR CTaskProcessor::ProvideGachaItem( USER_NODE* pUser, DB_GACHA* pGacha )
{ 
#ifdef ENABLE_GACHA_SHOP
	DB_ITEM* pOut = &pGacha->m_stDBItem;

	pOut->m_ui8ItemCount = 0;
	
	CGachaShop* pGachaShop = g_pConfig->GetGachaShop();

	SHOP_MATCHING_DATA* pMatching = g_pShop->GetMatchingData( pGacha->m_TGoodsID );
	if(!pMatching )
	{
		return GACHA_ERROR_INTERNAL;
	}

	pGacha->m_i32ResultCount = pMatching->_ui32ItemArg + pMatching->_ui32BonusArg;	// ItemArg + BonusArg

	GACHA_GROUP eGroup = pGachaShop->GetGachaGroupByGoodsID(pGacha->m_TGoodsID);
	INT32 i32ItemKey = GET_GOODS_NUMBER( pGacha->m_TGoodsID ) ;
	

	// Lucky Item은 루프중 한번만 뽑힙니다.
	INT32 i32LuckyGoodsCount = 0;
	UINT32 ui32LuckyGoodsID = INVALID_GOODS_ID;
	for( INT32 i = 0; i < pGacha->m_i32ResultCount; ++i )
	{
		if( (UINT32)i < pMatching->_ui32ItemArg )
		{
			pUser->IncGachaPurchaseCnt(i32ItemKey);
		}

		UINT32 ui32IsLuckyGoodsID = pGachaShop->IsWinningLucky( eGroup, pUser->_sBasic.m_strNick);
		if( ui32IsLuckyGoodsID != INVALID_GOODS_ID )
		{
			ui32LuckyGoodsID = ui32IsLuckyGoodsID;
			++i32LuckyGoodsCount;
		}
 
		UINT32 ui32WinningGoodsID = INVALID_GOODS_ID;
		GACHA_RESULT eGachaResult = pGachaShop->IsWinningGacha( eGroup, pGacha->m_i32ItemIdx, i32ItemKey, pUser->GetGachaPurchaseCnt(i32ItemKey), ui32WinningGoodsID);
		if( eGachaResult == GACHA_RESULT_WIN || eGachaResult == GACHA_RESULT_GLORY )
			pUser->ResetGachaPurchaseCnt(i32ItemKey);

		if( ui32WinningGoodsID != INVALID_GOODS_ID )
		{
			SHOP_GOODS_LIST* pGoods = g_pShop->GetGoodsByID(ui32WinningGoodsID);
			if( !pGoods ) 
			{	// 당첨 아이템이 유효하지 않습니다. 보상 필요.
				g_pLog->WriteLog( L"[ERROR] GachaProvide Not Find Goods Need Compensation / UID : %I64d / ItemCount : %d / WinningGoodsID : %d /  GachaGoodsID : %d",
					pUser->_i64UID, pOut->m_ui8ItemCount, ui32WinningGoodsID,  pGacha->m_TGoodsID ); 
				eGachaResult = GACHA_RESULT_NONE;
				goto e_Next;
			}

			SHOP_ITEM_DATA* pItem = pGoods->_ppItemData[0];
			if( !pItem ) 
			{	// 당첨 아이템이 유효하지 않습니다. 보상 필요.
				g_pLog->WriteLog( L"[ERROR] GachaProvide Not Find Item Need Compensation / UID : %I64d / ItemCount : %d / WinningGoodsID : %d /  GachaGoodsID : %d",
					pUser->_i64UID, pOut->m_ui8ItemCount, ui32WinningGoodsID,  pGacha->m_TGoodsID ); 
				eGachaResult = GACHA_RESULT_NONE;
				goto e_Next;
			}

			pOut->m_aItem[pOut->m_ui8ItemCount]._ui32ItemID		= pItem->_ui32ItemID;
			pOut->m_aItem[pOut->m_ui8ItemCount]._ui32AddItemArg	= pGoods->_pui32ItemArg[0];
			++pOut->m_ui8ItemCount;
		}

e_Next:
		
		pGacha->stResult[i].m_ui32WinGoodsID	= ui32WinningGoodsID;
		pGacha->stResult[i].m_ui32LuckyGoodsID	= ui32IsLuckyGoodsID;
		pGacha->stResult[i].m_eGachaResult		= eGachaResult;
	}

	// Lucky Item 당첨 처리. stResult 배열과 stItem 배열 순서 맞추기 위해서 일부러 맨뒤에 추가합니다.
	for( INT32 k = 0; k < i32LuckyGoodsCount ; ++k )
	{	
		SHOP_GOODS_LIST* pGoods = g_pShop->GetGoodsByID(ui32LuckyGoodsID);
		if( !pGoods) break;

		SHOP_ITEM_DATA* pItem = pGoods->_ppItemData[0];
		if( !pItem ) break;

		pOut->m_aItem[pOut->m_ui8ItemCount]._ui32ItemID		= pItem->_ui32ItemID;
		pOut->m_aItem[pOut->m_ui8ItemCount]._ui32AddItemArg	= pGoods->_pui32ItemArg[0]; 
		++pOut->m_ui8ItemCount;
	}
#endif

	return GACHA_ERROR_SUCCESS;
}

void CTaskProcessor::SendGachaShopState(bool bAllServer, INT32 i32ServerIdx)
{
	CGachaShop* pGachaShop = g_pConfig->GetGachaShop();

	PACKET_GACHA_SHOP_STATE stGachaShopState;
	pGachaShop->MakeGachaStatePacket(&stGachaShopState);

	i3NetworkPacket stPacketGachaState(PROTOCOL_GACHA_SHOP_STATE_ACK);
	stPacketGachaState.WriteData(&stGachaShopState, sizeof(stGachaShopState));

	if( bAllServer )
		SendAllGamePacket(&stPacketGachaState);
	else
		SendGamePacket(i32ServerIdx, &stPacketGachaState);
}

void CTaskProcessor::SendGachaWinningUser(GACHA_RESULT eGachaResult, TTCHAR* pstrUserName, UINT32 ui32LuckyGoodsID, INT32 i32ServerIdx)
{
	PACKET_GACHA_WINNING_USER stWinningUser;
	stWinningUser.m_eGachaResult = eGachaResult;
	stWinningUser.m_ui32LuckyGoodsID = ui32LuckyGoodsID;
	i3String::Copy(stWinningUser.m_szLuckyUser, pstrUserName, NET_NICK_NAME_SIZE);

	i3NetworkPacket stPacket(PROTOCOL_SS_GACHA_WINNING_USER_ACK);
	stPacket.WriteData(&stWinningUser, sizeof(stWinningUser));

	if( eGachaResult == GACHA_RESULT_LUCKY )
		SendAllGamePacket(&stPacket);
	else
		SendGamePacket(i32ServerIdx, &stPacket);
}

void CTaskProcessor::DirectUserPenalty( UINT32 ui32ServerIdx, INT32 i32SessionIdx, T_UID i64UID, INT8 i8PenaltyType, INT32 i32PenaltyAmount )
{
	UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
	if(pInUID != NULL)
	{
		pInUID->_ui8Type			= TYPE_UID_DIRECT_PENALTY;
		pInUID->_ui32ServerIdx		= ui32ServerIdx;
		pInUID->_i64UID				= i64UID;
		pInUID->m_i32SessionIdx		= i32SessionIdx;
		pInUID->m_i8PenaltyType		= i8PenaltyType;
		pInUID->m_i32PenaltyAmount	= i32PenaltyAmount;

		m_pDBUID->PushUIDPTIdx();
	}
}

void CTaskProcessor::SendGachaPurchaseAck( USER_NODE *pUser, GACHA_ERROR eErrorCode, DB_GACHA* pGacha )
{
#ifdef ENABLE_GACHA_SHOP
	PACKET_GACHA_SS_RESULT_HEADER stResultHeader;
	stResultHeader.m_eErrorCode			= eErrorCode;
	stResultHeader.m_TUID				= pUser->_i64UID;

	INT32 i32ItemKey = GET_GOODS_NUMBER(pGacha->m_ui32GachaGoodsID);

	stResultHeader.m_i32ItemIdx			= pGacha->m_i32ItemIdx;
	stResultHeader.m_i32AccumPurchase	= pUser->GetGachaPurchaseCnt(i32ItemKey);
	stResultHeader.m_i32ResultCount		= pGacha->m_i32ResultCount;
	stResultHeader.m_ui32Cash			= pUser->_ui32Cash;
	stResultHeader.m_ui8BuyType			= pGacha->m_ui8BuyType;
	stResultHeader.m_TGoodsID			= pGacha->m_ui32GachaGoodsID;
	stResultHeader.m_ui32Price			= pGacha->m_ui32GachaPrice;

	i3NetworkPacket stPacket(PROTOCOL_SS_GACHA_PURCHASE_ACK);
	stPacket.WriteData( &stResultHeader, sizeof(PACKET_GACHA_SS_RESULT_HEADER) );
	
	if( GACHA_ERROR_SUCCESS == eErrorCode )
	{
		// pGacha->m_i32ResultCount 와 pDBItem->m_stDBItem.m_ui8ItemCount 는 다를수 있습니다. ( 기획의도 )
		stPacket.WriteData( pGacha->stResult,					(UINT16)(sizeof(PACKET_GACHA_RESULT_DATA) * pGacha->m_i32ResultCount) );
		stPacket.WriteData( &pGacha->m_stDBItem.m_ui8ItemCount,	sizeof(UINT8) );
		stPacket.WriteData( pGacha->m_stDBItem.m_aItem,			sizeof(INVEN_BUFFER_DB) * pGacha->m_stDBItem.m_ui8ItemCount );

		for( INT32 i = 0 ; i < pGacha->m_i32ResultCount ; ++i )
		{	// 당첨 결과에 따른 공지 메시지를 보냅니다.
			if( pGacha->stResult[i].m_eGachaResult == GACHA_RESULT_WIN )
			{	
				SendGachaWinningUser(GACHA_RESULT_WIN, pUser->_sBasic.m_strNick, pGacha->stResult[i].m_ui32WinGoodsID, pUser->_i32Serveridx);
			}

			if( pGacha->stResult[i].m_ui32LuckyGoodsID != INVALID_GOODS_ID )
			{
				SendGachaWinningUser(GACHA_RESULT_LUCKY, pUser->_sBasic.m_strNick, pGacha->stResult[i].m_ui32LuckyGoodsID);
			}
		}
	}

	SendGamePacket(pUser->_i32Serveridx, &stPacket);
#endif
}

void CTaskProcessor::SendPCCafeStatusAck( USER_NODE* pNode )
{
	i3NetworkPacket SendPacket( PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK );
	SendPacket.WriteData( &pNode->_i32Sessionidx,	sizeof(INT32) );
	SendPacket.WriteData( &pNode->_i64UID,			sizeof(T_UID) );
	SendPacket.WriteData( &pNode->_ui8RoleFlag,		sizeof(UINT8) );
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

// 주의 TASK 쓰레드에서만 호출해야함
void CTaskProcessor::WriteGiftLog( SEND_GIFT *pGift, ENUM_TYPE_SHOP eType)
{
	if( !g_pModuleLogD )  return;

	LOG_GIFT	stLog;
	stLog.m_TSenderUID		= pGift->m_TSenderUID;
	stLog.m_TReceiverUID	= pGift->m_TReceiverUID;
	stLog.m_eRequestType	= pGift->m_eRequestType;

	switch( eType )
	{
	default:
	case TYPE_SHOP_GIFT_BUY:
		stLog.m_ui32ExpireDate = g_pConfig->GetAddAuthTime( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), SECOND_PER_DAY * MAX_GET_GIFT_EXPIRE_DAY ) ;
		stLog.m_eState = TYPE_GIFT_DELIVER_NORMAL;
		break;
	case TYPE_SHOP_GIFT_AUTH:
		stLog.m_ui32ExpireDate = g_pConfig->GetAddAuthTime( i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI(), SECOND_PER_DAY * MAX_AUTH_GIFT_EXPIRE_DAY ) ;
		stLog.m_eState = TYPE_GIFT_DELIVER_RECEIVED;
		break;
	case TYPE_SHOP_GIFT_DELETE:
		stLog.m_ui32ExpireDate = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
		stLog.m_eState = TYPE_GIFT_DELIVER_DELETE;
		break;
	}

	for( UINT8 i = 0 ; i < pGift->m_ui8GiftCount ; ++i )
	{
		if( EV_FAILED(pGift->m_aGift[i].m_i32Rv) ) continue;
		stLog.m_TGoodsID = pGift->m_aGift[i].m_ui32GoodsID;
		g_pModuleLogD->SendLogMessage( LOG_MESSAGE_GIFT_TASK, LOCALE_CODE_NONE, &stLog   );
	}


	return;
}

void CTaskProcessor::_SendDeleteChara( USER_NODE *pNode, INT32 i32Rv, UINT8 ui8DeleteSlot )
{
	if( !pNode ) return;

	i3NetworkPacket	SendPacket( PROTOCOL_CHAR_DELETE_CHARA_ACK );
	SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
	SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
	SendPacket.WriteData( &i32Rv,						sizeof( INT32 ) );
	SendPacket.WriteData( &ui8DeleteSlot,				sizeof( UINT8 ) );
	SendGamePacket( pNode->_i32Serveridx, &SendPacket );

	return;
}

void CTaskProcessor::_CreateDefaultChara( USER_NODE *pNode )
{
	// 기본 캐릭터 정보가 DB에 없다면 생성합니다.
	if( !pNode ) return;

	// 닉네임 생성전.
	if( pNode->_sBasic.m_strNick[0] == '\0' )	return;
			
	
	if( pNode->m_pDesign->GetDefaultCreateCharaCount() <= 0 ) return;

	// 만들어야할 기본 캐릭터 갯수.
	INT32		pEmptySlot[TEAM_COUNT] = { -1, -1 };
	BOOL		pMakeTeam[TEAM_COUNT] = { TRUE,TRUE };

	INT32 i32CreateDefChara = pNode->m_MultiSlot.GetMakeDefCharInfo( pEmptySlot, pMakeTeam );
	if( i32CreateDefChara <= 0 ) return; 

	// 기본 캐릭터를 생성해 줍니다.
	RING_CREATE_CHAR	CreateChara;
	CreateChara.m_bAddCharaSlot = true;
	CreateChara.m_eType		=	CHARA_RING_TYPE_CREATE;
	CreateChara.m_TUID		=	pNode->_i64UID;
	CreateChara.m_ui8Class	=	CHARA_CLASS_NONE;
	CreateChara.m_ui8SlotState = MULTI_SLOT_STATE_NORMAL;

	INT32		i32MakeDefCount = 0;
	T_ItemID	MakeDefItemID[TEAM_COUNT] = { 0, };
	for( INT32 i = 0; i < TEAM_COUNT; ++i )
	{
		if( FALSE == pMakeTeam[i] ) continue;
		
		switch( i )
		{
		case TEAM_RED:
			MakeDefItemID[i32MakeDefCount++] = g_TCharEquipBasic[TEAM_RED];
			break;
		case TEAM_BLUE:
			MakeDefItemID[i32MakeDefCount++] = g_TCharEquipBasic[TEAM_BLUE];
			break;
		}
	}
 
	for( INT32 i = 0 ; i < i32CreateDefChara ; ++i )
	{	
		if( pEmptySlot[i] < 0 ) continue;

		T_ItemID TCharaItemID = MakeDefItemID[i];

		CreateChara.m_ui8CharSlot		=	(UINT8)pEmptySlot[i];
		CreateChara.m_TGoodsID			=  TCharaItemID * 100;
		g_MakeBasicItemDBIdx( CreateChara.m_Inven._TItemWareDBIdx,  TCharaItemID );
		i3String::Copy( CreateChara.m_CharBaseInfo.m_strCharName, g_TCharaName[GET_ITEM_SUBTYPE(TCharaItemID) - 1], sizeof(TTCHAR) * NET_NICK_NAME_SIZE );

		INT32 i32Rv = _CreateChar( &CreateChara );
		if( EV_FAILED(i32Rv))
		{
			g_pLog->WriteLog( L"_CreateChar Fail!!!! i32Rv : %d", i32Rv );
			_SendFailCreateChara( pNode, i32Rv ); 
		}

		++i32MakeDefCount;
	}


	 
}

void CTaskProcessor::_SendCreateChara( USER_NODE *pNode, RING_CREATE_CHAR* pChara, S2MO_CHAR_SKILL_INFO* pSkill )
{
	if( !pNode || !pChara ) return;

	i3NetworkPacket	SendPacket;
	SendPacket.SetProtocol( PROTOCOL_CHAR_CREATE_CHARA_ACK );
				
	SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof(INT32) ); 
	SendPacket.WriteData( &pNode->_i64UID,					sizeof(T_UID) ); 
	SendPacket.WriteData( &pChara->m_TGoodsID,				sizeof(T_GoodsID) );
	SendPacket.WriteData( &pNode->_ui32Cash,				sizeof(UINT32));
	SendPacket.WriteData( &pChara->m_i32Result,				sizeof(INT32) );
	if( EV_SUCCESSED( pChara->m_i32Result ) )
	{
		SendPacket.WriteData( &pChara->m_Inven,				sizeof(INVEN_BUFFER) ); 
		SendPacket.WriteData(&pChara->m_ui8SlotState,		sizeof(UINT8));
		UINT8 ui8CreateType = (UINT8)pChara->m_eType;
		SendPacket.WriteData(&ui8CreateType,				sizeof(UINT8)); 
		SendPacket.WriteData(&pChara->m_bAddCharaSlot,		sizeof(bool));
		if( pChara->m_bAddCharaSlot )
		{			
			SendPacket.WriteData( &pChara->m_ui8CharSlot,						sizeof(UINT8) );
			SendPacket.WriteData( &pChara->m_ui8Class,							sizeof(UINT8) ); 

			SendPacket.WriteData( &pChara->m_CharBaseInfo,						sizeof(S2MO_CHAR_BASE_INFO) );
			SendPacket.WriteData( pSkill,										sizeof(S2MO_CHAR_SKILL_INFO) );
		}
	}

	SendGamePacket( pNode->_i32Serveridx, &SendPacket );
}

void CTaskProcessor::_SyncChara(USER_NODE *pNode, INVEN_BUFFER* pInvenItem, INT64 &i64AddCharaSlot)
{
	if (!pNode)						return;
	if (!pInvenItem)				return;
	if ( -1 == i64AddCharaSlot )	return;
	if (ITEM_TYPE_CHARA != GET_ITEM_TYPE(pInvenItem->_ui32ItemID)) return;

	// 타입 설정이 되지 않은 캐릭터는 PASS 합니다.
	SHOP_ITEM_DATA * pItem = g_pShop->GetItemByID(pInvenItem->_ui32ItemID);
	if (!pItem) return;
	if (0 == pItem->_ui8AuthType) return;

	// 멀티 슬롯에서 소지중인지 검색
	for (INT32 k = 0; k < S2MO_MULTI_SLOT_COUNT; ++k)
	{
		if (!pNode->m_MultiSlot.IsActiveChar(k)) continue;

		ITEM_INFO CharaInfo;
		pNode->m_MultiSlot.GetCharEquipment(k, CHAR_EQUIPMENT_CHARA, &CharaInfo);

		if (CharaInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN)
		{
			if (CharaInfo.m_TItemID == pInvenItem->_ui32ItemID)
			{	// 기본 지급 캐릭터는 PASS
				return;
			}
			continue;
		}

		if (CharaInfo.m_TItemDBIdx == pInvenItem->_TItemWareDBIdx)
		{	// 밀티슬롯에 소지중이라면 PASS.
			return;
		}
	}

	MULTI_SLOT_STATE eSlotState = MULTI_SLOT_STATE_NORMAL;
	if (ITEM_TYPE_TIME == pItem->_ui8AuthType &&  ITEM_ATTR_TYPE_BUY == pInvenItem->_ui8ItemType)
	{	// 미사용 중인 캐릭터는 슬롯 대기 상태로.
		eSlotState = MULTI_SLOT_STATE_UNUSE;
	}

	RING_CREATE_CHAR	CreateChar;
	CreateChar.m_ui8CharSlot = 0xFF;
	
	for (UINT8 i = 0; i < S2MO_MULTI_SLOT_COUNT; i++)
	{
		if ((INT64)0x01 << i & i64AddCharaSlot) continue;	// 앞선 루프에서 지급된 슬롯이라면 PASS.

		if ( FALSE == pNode->m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_EXIST))
		{
			CreateChar.m_ui8CharSlot = i;
			break;
		}
	}
	
	if (0xFF == CreateChar.m_ui8CharSlot) return;	// 슬롯 인덱스 할당 실패.
	if (CreateChar.m_ui8CharSlot >= S2MO_MULTI_SLOT_COUNT - S2MO_MULTI_SLOT_PCCAFE_CHARA_COUNT) return; // PCCafe 캐릭터용 슬롯.

	CreateChar.m_TUID		= pNode->_i64UID;
	CreateChar.m_eType		= CHARA_RING_TYPE_CREATE;
	CreateChar.m_ui8Class	= (UINT8)CHARA_CLASS_NONE;
	CreateChar.m_TGoodsID	= pInvenItem->_ui32ItemID * 100;
	CreateChar.m_ui8SlotState = (UINT8)eSlotState;
	i3mem::Copy(&CreateChar.m_Inven, pInvenItem, sizeof(INVEN_BUFFER));
	CreateChar.m_bAddCharaSlot = true;

	S2MO_CHAR_BASE_INFO* pBaseInfo = &CreateChar.m_CharBaseInfo;
	pBaseInfo->Reset();
	pBaseInfo->m_ui32CreateDate = i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
	i3String::Copy(pBaseInfo->m_strCharName, _TT("Character"), NET_NICK_NAME_SIZE);

	// 실패하더라도 다음번 로그인때 멀티슬롯에 추가되므로 로그 기록 하지 않습니다.
	if ( EV_SUCCESSED(_CreateChar(&CreateChar)) )
	{
		i64AddCharaSlot |= (INT64)0x01 << (INT64)CreateChar.m_ui8CharSlot;
	}

}

INT32 CTaskProcessor::DeleteCharaDB(USER_NODE* pUserNode, T_ItemDBIdx TDBIdx)
{
	if (!pUserNode) return EVENT_ERROR_NOT_FIND_USER;
	INT32 i32SlotIdx = pUserNode->m_MultiSlot.FindSlotIdxByCharaDBIdx(TDBIdx);

	return DeleteCharaDB( pUserNode->_i64UID, i32SlotIdx, TDBIdx);
}

INT32 CTaskProcessor::DeleteCharaDB( T_UID TUID, INT32 i32SlotIdx, T_ItemDBIdx TDBIdx)
{
	if ( i32SlotIdx < 0) return EVENT_ERROR_SHOP_CHARA_DEL_FAIL_SLOT_IDX;

	RING_CREATE_CHAR	CharIn;
	CharIn.m_eType			= CHARA_RING_TYPE_DELETE;
	CharIn.m_TUID			= TUID;
	CharIn.m_ui8CharSlot	= (UINT8)i32SlotIdx;
	CharIn.m_Inven._TItemWareDBIdx = TDBIdx;

	return _CreateChar(&CharIn);
}

BOOL CTaskProcessor::IsUseGiftBoxContents(UINT8 ui8InsertType)
{
	switch (ui8InsertType)
	{
	case ITEM_INSERT_REQUEST_TS_BONUS:
	case ITEM_INSERT_REQUEST_RANKUP:
	case ITEM_INSERT_REQUEST_ATTENDANCE:
	case ITEM_INSERT_REQUEST_EVENT_CONNECT:
	case ITEM_INSERT_REQUEST_MEDAL_REWARD:
	case ITEM_INSERT_REQUEST_DOMI_REWARD:
	case ITEM_INSERT_REQUEST_WEB_GIFT:
	//case ITEM_INSERT_REQUEST_USER_GIFT:
		return true;
	default:
		return false;
	}

	return false;
}

void CTaskProcessor::SetGeneralRankUpStartHour(INT32 hour)
{
	if (m_pGeneralRankUP == NULL) return;

	m_pGeneralRankUP->SetStartHour(hour);
}

void CTaskProcessor::_SendGiftRecv_Inven(T_UID TUID, SEND_GIFT* pGift)
{
	USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pGift->m_TReceiverUID);
	if (NULL == pNode) return;

	INT32 i32Result = TRUE;
	for (INT32 i = 0; i < pGift->m_ui8GiftCount; i++)
	{
		if (EV_SUCCESSED(pGift->m_aGift[i].m_i32Rv))continue;
		i32Result = FALSE;
	}

	i3NetworkPacket	SendPacket(PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK);
	SendPacket.WriteData(&pNode->_i32Sessionidx, sizeof(INT32));
	SendPacket.WriteData(&pNode->_i64UID, sizeof(T_UID));
	SendPacket.WriteData(&pGift->m_eRequestType, sizeof(UINT8));
	SendPacket.WriteData(&i32Result, sizeof(INT32));

	if (EV_SUCCESSED(i32Result))
	{
		for (INT32 i = 0; i < pGift->m_ui8GiftCount; i++)
		{
			INT64 i64CharaSync = 0;
			// 유저 인벤토리 Update
			INVEN_BUFFER InvenBuffer;
			i3mem::Copy(&InvenBuffer, &pGift->m_InvenItem[i], sizeof(INVEN_BUFFER));
			UserInsertItem(pNode, &InvenBuffer, i64CharaSync);
		}
		
		SendPacket.WriteData(&pGift->m_ui8GiftCount, sizeof(UINT8));
		SendPacket.WriteData(pGift->m_InvenItem, sizeof(INVEN_BUFFER_DB) * pGift->m_ui8GiftCount);
		UINT32 ui32CapsuleItemID = 0;
		SendPacket.WriteData(&ui32CapsuleItemID, sizeof(UINT32));
	}

	SendGamePacket(pNode->_i32Serveridx, &SendPacket);

}



// 상점에서 판매하는 캡슐 아이템의 구성품이 없는 경우 로그만 남깁니다. 2016.09.05 어깨깡패
void	CTaskProcessor::_CheckCapsuleMatching()
{
	g_pLog->WriteLog(L"");
	g_pLog->WriteLog(L"///////////////////////////////////////////////////////");
	g_pLog->WriteLog(L"///////////////// CAPSULE CHECK START /////////////////");

	UINT32 ui32CapsuleItemID = 0;
	INT32	i32RewardCount = 0;
	INT32 i32CapsuleItemCount = 0;
	UINT32 ui32RewardItemID = 0;
	CCapsuleItem* pCapsule = NULL;

	wchar_t wstrCapsuleName[MAX_STRING_COUNT];
	
	for (INT32 i32CapsuleIdx = 0; i32CapsuleIdx < m_i32CapsuleCount; ++i32CapsuleIdx)
	{
		pCapsule = NULL;
		pCapsule = m_ppCapsule[i32CapsuleIdx];

		if (NULL == pCapsule)						continue;

		ui32CapsuleItemID = pCapsule->GetItemID();

		// 캡슐 아이템ID 오류 PASS
		SHOP_ITEM_BASE* pCapsuleItem = g_pShop->GetItemData(ui32CapsuleItemID);
		if (NULL == pCapsuleItem)
		{
			/*i3String::Format( strTemp, MAX_PATH, "[ERROR CAPSULE] CapsuleItemID / Capsule Name : %s / ItemID : %d ", pCapsule->GetName() , ui32CapsuleItemID );
			g_pModuleLogFile->Write_Log_Task( strTemp);*/
			continue;
		}

		// 아이템 타입 오류 PASS
		if (ITEM_TYPE_COUNT != pCapsuleItem->_ui8AuthType || ITEM_EXPIRE_DELETE != pCapsuleItem->_ui8AuthExpireType || ITEM_AUTH_HAND != pCapsuleItem->_ui8AutoStart)
		{
			//i3String::Format( strTemp, MAX_PATH, "[ERROR CAPSULE] Capsuletype / Capsule Name : %s / AuthType : %d / AuthExpireType : %d / AutoStart : %d ", 
			//	pCapsule->GetName(), pCapsuleItem->_ui8AuthType, pCapsuleItem->_ui8AuthExpireType, pCapsuleItem->_ui8AutoStart );
			//g_pModuleLogFile->Write_Log_Task( strTemp);
			continue;
		}

		// 상점 미판매 캡슐 아이템 PASS
		if (FALSE == g_pShop->IsSalesItem(ui32CapsuleItemID)) continue;

		i32CapsuleItemCount = pCapsule->GetItemCount();	// 캡슐 구성품 갯수
		for (INT32 i32CapsuleItemIdx = 0; i32CapsuleItemIdx < i32CapsuleItemCount; ++i32CapsuleItemIdx)
		{
			i32RewardCount = pCapsule->GetRewardCount((UINT8)i32CapsuleItemIdx);	// 각 구성품의 보상 갯수
			for (INT32 i32RewardIdx = 0; i32RewardIdx < i32RewardCount; ++i32RewardIdx)
			{
				ui32RewardItemID = pCapsule->GetRewardItemID((UINT8)i32CapsuleItemIdx, i32RewardIdx);
				if ( ITEM_TYPE_POINT == GET_ITEM_TYPE(ui32RewardItemID)) continue;	// 포인트는 그냥 계산 하므로 PASS

				SHOP_ITEM_BASE* pRewardItem = g_pShop->GetItemData(ui32RewardItemID);
				if (NULL == pRewardItem)
				{
					MultiByteToWideChar(CP_ACP, NULL, pCapsule->GetName(), -1, wstrCapsuleName, MAX_STRING_COUNT);
					g_pLog->WriteLog(L"[ERROR CAPSULE] RewardItemID / Capsule Name : %s /  Capsule ItemID : %d / Reward ItemID : %d ",
						wstrCapsuleName, ui32CapsuleItemID, ui32RewardItemID);
					continue;
				}

				if (ITEM_TYPE_ETERNITY == pRewardItem->_ui8AuthType) continue; // 영구제는 PASS

				if (0 == pRewardItem->_ui8AuthType || 0 == pRewardItem->_ui8AuthExpireType || 0 == pRewardItem->_ui8AutoStart)
				{
					MultiByteToWideChar(CP_ACP, NULL, pCapsule->GetName(), -1, wstrCapsuleName, MAX_STRING_COUNT);
					g_pLog->WriteLog(L"[ERROR CAPSULE] RewardType / Capsule Name : %s / Capsule ItemID : %d / Reward ItemID : %d / AuthType : %d / AuthExpireType : %d / AutoStart : %d ",
						wstrCapsuleName, ui32CapsuleItemID, ui32RewardItemID, pRewardItem->_ui8AuthType, pRewardItem->_ui8AuthExpireType, pRewardItem->_ui8AutoStart);
					continue;
				}

			}

		}
	}

	g_pLog->WriteLog(L"///////////////// CAPSULE CHECK END /////////////////");
	g_pLog->WriteLog(L"///////////////////////////////////////////////////////");
	g_pLog->WriteLog(L"");

	return;
}

