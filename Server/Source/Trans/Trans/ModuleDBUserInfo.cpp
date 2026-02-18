#include "pch.h"
#include "ModuleDBUserInfo.h"

UINT32 _DefThreadProcNick(void * pPointer)
{
	((CModuleDBUserInfo*)pPointer)->Working(); 
	return 0; 
}

CModuleDBUserInfo::CModuleDBUserInfo()
{
	m_hThread			= NULL;
	m_bRunning			= FALSE;
	m_pDB				= NULL;
	m_pRingIn			= NULL;
	m_pRingOut			= NULL;
}

CModuleDBUserInfo::~CModuleDBUserInfo()
{
	Destroy();
}

BOOL CModuleDBUserInfo::Create()
{

	//For NickName 
	m_pUserDBConfig = g_pConfig->GetDatabaseConfig("PBUser");
	if( !m_pUserDBConfig )
		return FALSE;

	m_pDB = CADODatabase::NewObject();
	if( !m_pDB || !m_pDB->Connect(m_pUserDBConfig->GetDataSource(), m_pUserDBConfig->GetCatalog(), m_pUserDBConfig->GetAccount(), m_pUserDBConfig->GetPassword()) )
		return FALSE;

	m_pRingIn	= i3RingBuffer::NewObject();
	if( NULL == m_pRingIn )																	return FALSE;
	if( false == m_pRingIn->Create( sizeof(RING_USER_INFO_IN), DB_USER_INFO_COUNT ) )		return FALSE;

	m_pRingOut = i3RingBuffer::NewObject();
	if( NULL == m_pRingOut )																return FALSE;
	if( false == m_pRingOut->Create( sizeof(RING_USER_INFO_OUT), DB_USER_INFO_COUNT ) )		return FALSE;

	DWORD dwThreadID;
	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProcNick , this, 0, &dwThreadID );
	if( NULL == m_hThread )																	return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )					return FALSE;
	
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER UserInfo"  ); 	

	return TRUE;
}

void CModuleDBUserInfo::Destroy()
{
	m_bRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );

	I3_SAFE_RELEASE( m_pDB );
	I3_SAFE_RELEASE( m_pRingIn );
	I3_SAFE_RELEASE( m_pRingOut );
}

void CModuleDBUserInfo::Working()
{
	BOOL bWorked;
	RING_USER_INFO_IN*	pRingIn;
	RING_USER_INFO_OUT*	pRingOut;

	m_i32LogIdx = g_pLog->AddLogThread(L"USIF", 0, GetCurrentThreadId());

	m_bRunning = TRUE;

	while( m_bRunning )
	{
		bWorked = FALSE;

		// 처리할 일이 없습니다. Continue;
		if( 0 == m_pRingIn->GetBufferCount() )	goto THREAD_CONTINUE;

		pRingIn	= (RING_USER_INFO_IN*)m_pRingIn->Pop();
		pRingOut= (RING_USER_INFO_OUT*)m_pRingOut->PushPointer(); 
		if( NULL == pRingIn )
		{
			g_pLog->WriteLog( L"[WARRING] User Info Ring Buffer In Full" ); 
			goto THREAD_CONTINUE;
		}
		if( NULL == pRingOut )
		{
			g_pLog->WriteLog( L"[WARRING] User Info Ring Buffer Out Full" ); 
			goto THREAD_CONTINUE;
		}

		bWorked = TRUE;

		pRingOut->m_i32ServerIdx	= pRingIn->m_i32ServerIdx;
		pRingOut->m_i32SessionIdx	= pRingIn->m_i32SessionIdx;
		pRingOut->m_TUID			= pRingIn->m_TUID;
		
		_GetUserInfo( pRingIn, pRingOut );

		m_pRingIn->PopIdx();
		m_pRingOut->PushPointerIdx();

THREAD_CONTINUE:

		if( FALSE == bWorked )			WaitForSingleObject( m_hThread, 1 );
	}
}

void CModuleDBUserInfo::_GetUserInfo( RING_USER_INFO_IN* pRingIn, RING_USER_INFO_OUT* pRingOut )
{
	wchar_t wstrSQL[SERVER_STRING_COUNT];

	switch( pRingIn->m_eFindState )
	{
	case USER_INFO_FIND_STATE_UID:
		{ 
			S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_ByUID", 
				DB_DATA_TYPE_BIGINT,		pRingIn->m_TFindUID,							//    @i64UserUID_                BIGINT
				DB_DATA_TYPE_END );
			if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserInfo( wstrSQL );
				pRingOut->m_TResult = EVENT_ERROR_FAIL;
				return;
			}
			
			if( 0 == m_pDB->GetRecordCount() )
			{
				pRingOut->m_TResult = EVENT_ERROR_FAIL;
				return;
			}

			pRingOut->m_DetailInfo.m_TUID	= pRingIn->m_TFindUID;

			m_pDB->GetTextData( 0, pRingOut->m_DetailInfo.m_strNickName, NET_NICK_NAME_SIZE );
			pRingOut->m_DetailInfo.m_ui8Rank		= (UINT8)m_pDB->GetINT32Data(1);
			pRingOut->m_DetailInfo.m_ui32Exp		= m_pDB->GetINT32Data(2);
			pRingOut->m_DetailInfo.m_ui32Match		= m_pDB->GetINT32Data(3);
			pRingOut->m_DetailInfo.m_ui32Win		= m_pDB->GetINT32Data(4);
			pRingOut->m_DetailInfo.m_ui32Draw		= m_pDB->GetINT32Data(5);
			pRingOut->m_DetailInfo.m_ui32Lose		= m_pDB->GetINT32Data(6);
			pRingOut->m_DetailInfo.m_ui32Kill		= m_pDB->GetINT32Data(7);
			pRingOut->m_DetailInfo.m_ui32Death		= m_pDB->GetINT32Data(8);
			pRingOut->m_DetailInfo.m_ui32HeadShot	= m_pDB->GetINT32Data(9);
			pRingOut->m_DetailInfo.m_ui32DisConnect	= m_pDB->GetINT32Data(10);
			m_pDB->GetTextData(11, pRingOut->m_DetailInfo.m_strClanName, NET_CLAN_NAME_SIZE);
			pRingOut->m_DetailInfo.m_ui32ClanMark = m_pDB->GetINT32Data(12);
			pRingOut->m_DetailInfo.m_ui32Assist = m_pDB->GetINT32Data(13);
		}
		break;
	case USER_INFO_FIND_STATE_NICK:
		{
			S2DB::MakeQueryW( wstrSQL, L"usp_Basic_sel_ByNick", 
				DB_DATA_TYPE_TTCHAR,			pRingIn->m_strNickName,							//    @strUNickname_				NVARCHAR(100)
				DB_DATA_TYPE_END );
			if( FALSE == m_pDB->ExecuteSP(wstrSQL) )
			{
				ErrorDBUserInfo( wstrSQL );
				pRingOut->m_TResult = EVENT_ERROR_FAIL;
				return;
			}
			
			if( 0 == m_pDB->GetRecordCount() )
			{
				pRingOut->m_TResult = EVENT_ERROR_FAIL;
				return;
			}

			i3String::Copy( pRingOut->m_DetailInfo.m_strNickName, pRingIn->m_strNickName, NET_NICK_NAME_SIZE );

			pRingOut->m_DetailInfo.m_TUID			= m_pDB->GetINT64Data(0);
			pRingOut->m_DetailInfo.m_ui8Rank		= (UINT8)m_pDB->GetINT32Data(1);
			pRingOut->m_DetailInfo.m_ui32Exp		= m_pDB->GetINT32Data(2);
			pRingOut->m_DetailInfo.m_ui32Match		= m_pDB->GetINT32Data(3);
			pRingOut->m_DetailInfo.m_ui32Win		= m_pDB->GetINT32Data(4);
			pRingOut->m_DetailInfo.m_ui32Draw		= m_pDB->GetINT32Data(5);
			pRingOut->m_DetailInfo.m_ui32Lose		= m_pDB->GetINT32Data(6);
			pRingOut->m_DetailInfo.m_ui32Kill		= m_pDB->GetINT32Data(7);
			pRingOut->m_DetailInfo.m_ui32Death		= m_pDB->GetINT32Data(8);
			pRingOut->m_DetailInfo.m_ui32HeadShot	= m_pDB->GetINT32Data(9);
			pRingOut->m_DetailInfo.m_ui32DisConnect	= m_pDB->GetINT32Data(10);
			m_pDB->GetTextData(11, pRingOut->m_DetailInfo.m_strClanName, NET_CLAN_NAME_SIZE);
			pRingOut->m_DetailInfo.m_ui32ClanMark = m_pDB->GetINT32Data(12);
			pRingOut->m_DetailInfo.m_ui32Assist = m_pDB->GetINT32Data(13);
		}
		break;
	}

	S2DB::MakeQueryW( wstrSQL, L"usp_Character_sel_AllCharaDetail", 
		DB_DATA_TYPE_BIGINT,			pRingOut->m_DetailInfo.m_TUID,							//    @iUSN_          BIGINT
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserInfo( wstrSQL );
		pRingOut->m_TResult = EVENT_ERROR_FAIL;
		return;
	}
	
	pRingOut->m_DetailInfo.m_i8CharCount = (INT8)m_pDB->GetRecordCount();
	//pRingOut->m_DetailInfo.m_i8CharCount = MIN(pRingOut->m_DetailInfo.m_i8CharCount, S2MO_MULTI_SLOT_PACKET_COUNT);

	S2MO_CHAR_DETAIL_INFO* pInfo;
	for( INT32 i = 0 ; i < pRingOut->m_DetailInfo.m_i8CharCount ; i++ )
	{
		pInfo = &pRingOut->m_DetailInfo.m_aCharInfo[i];
		pInfo->m_ui8SlotIdx						= m_pDB->GetINT8Data(0);
		m_pDB->GetTextData( 1, pInfo->m_strCharName, NET_NICK_NAME_SIZE );
		pInfo->m_TItemID						= m_pDB->GetINT32Data(2);
		pInfo->m_SkillInfo.m_ui8Class			= m_pDB->GetINT8Data(3);
		pInfo->m_ui32BattleTime					= m_pDB->GetINT32Data(4);
		pInfo->m_ui32SkillRelTime				= m_pDB->GetINT32Data(5);
		
		GetMainSkillBit( pInfo->m_SkillInfo.m_ui8MainSkill, pInfo->m_SkillInfo.m_ui8MainSkillLv, m_pDB->GetINT32Data(8) );
		GetAssistSkillBit(pInfo->m_SkillInfo.m_ui8AssistSkill, pInfo->m_SkillInfo.m_ui8AssistSkillLv, m_pDB->GetINT32Data(9));
		GetCommonSkillBit( pInfo->m_SkillInfo.m_ui8CommonSkill, pInfo->m_SkillInfo.m_ui8CommonSkillLv, m_pDB->GetINT32Data(10) );
		
					
		pInfo->m_ui32CreateDate					= S2DB::ConvertSmallDateToUINT32( m_pDB->GetDateData(12) );


		m_pDB->MoveNext();
	}

	S2DB::MakeQueryW( wstrSQL, L"usp_AccountItemData_sel", 
		DB_DATA_TYPE_BIGINT,	pRingOut->m_DetailInfo.m_TUID,							//    @iUSN_          BIGINT
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserInfo( wstrSQL );
		pRingOut->m_TResult = EVENT_ERROR_FAIL;
		return;
	}

	pRingOut->m_DetailInfo.m_InvenData.Reset();

	if( 0 < m_pDB->GetRecordCount() )
	{
		pRingOut->m_DetailInfo.m_InvenData._ui8NickColor		= m_pDB->GetUINT8Data(0);
		pRingOut->m_DetailInfo.m_InvenData._wRank				= m_pDB->GetUINT32Data(1);
		pRingOut->m_DetailInfo.m_InvenData._wAprilFoolRank		= m_pDB->GetUINT32Data(2);
		m_pDB->GetTextData(3, pRingOut->m_DetailInfo.m_InvenData._cNick, NET_NICK_NAME_SIZE);;
		pRingOut->m_DetailInfo.m_InvenData._ui8CrossHairColor	= m_pDB->GetUINT8Data(4);
		pRingOut->m_DetailInfo.m_InvenData._ui8ChattingColor	= m_pDB->GetUINT8Data(5);
	}

	pRingOut->m_TResult		= EVENT_ERROR_SUCCESS;
}

void CModuleDBUserInfo::ErrorDBUserInfo( wchar_t * wstrSQL )
{
	g_pLog->WriteLog( L"[ErrorDBUserInfo] %s, Error Code : %08x", wstrSQL, m_pDB->GethrLastError() );
	
	HRESULT EventType = m_pDB->ReConnect(
																	m_pUserDBConfig->GetDataSource(),
																	m_pUserDBConfig->GetCatalog(),
																	m_pUserDBConfig->GetAccount(),
																	m_pUserDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUserInfo] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUserInfo] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}