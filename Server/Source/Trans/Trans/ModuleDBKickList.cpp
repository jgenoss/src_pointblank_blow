#include "pch.h"
#include "ModuleDBKickList.h"

static UINT32 _DefThreadProc(void * pPointer)
{
	((CModuleDBKickList*)pPointer)->Working(); 
	return 0; 
}

CModuleDBKickList::CModuleDBKickList(void)
{
	m_hThread			= NULL;
	m_bIsRunning		= FALSE;

	m_pADO				= NULL;
	m_pRingIN			= NULL;
	m_pRingOut			= NULL;

	m_pLinkDBConfig		= NULL;
}

CModuleDBKickList::~CModuleDBKickList(void)
{
	Destroy();
}

BOOL CModuleDBKickList::Create()
{
	m_pLinkDBConfig = g_pConfig->GetDatabaseConfig("PBLink");
	if( !m_pLinkDBConfig )		return FALSE;

	m_pADO = CADODatabase::NewObject(); 
	if( !m_pADO->Connect(m_pLinkDBConfig->GetDataSource(), m_pLinkDBConfig->GetCatalog(), m_pLinkDBConfig->GetAccount(), m_pLinkDBConfig->GetPassword()) )
		return FALSE;

	m_pRingIN	= new S2RingBuffer;
	if( NULL == m_pRingIN )																return FALSE;
	if( false == m_pRingIN->Create( sizeof(DB_USER_KICK_IN), DB_USER_KICK_COUNT ) )		return FALSE;

	m_pRingOut	= new S2RingBuffer;
	if( NULL == m_pRingOut )															return FALSE;
	if( false == m_pRingOut->Create( sizeof(DB_USER_KICK_OUT), DB_USER_KICK_COUNT ) )	return FALSE;

	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , this, 0, &m_dwThreadID );
	if( NULL == m_hThread )																return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_BELOW_NORMAL) )			return FALSE;
	
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER KICK LIST"  ); 	

	return TRUE;
}

void CModuleDBKickList::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	
	I3_SAFE_RELEASE( m_pADO );
	I3_SAFE_DELETE( m_pRingIN );
	I3_SAFE_DELETE( m_pRingOut );
}

void CModuleDBKickList::Working(void)
{
	g_pLog->AddLogThread( L"KICKLIST", 0, m_dwThreadID );

	wchar_t wstrSQL[SERVER_STRING_COUNT]; 

	INT32				i32Count;
	BOOL				bWorking;

	DB_USER_KICK_IN*	pDBUserKickIn;
	DB_USER_KICK_OUT*	pDBUserKickOut;

	T_UserKickIdx		i32UserKickIdx;
	T_UID				i64UID;
	UINT8				ui8IsInsert;
	BOOL				bSPResult;

	m_bIsRunning = TRUE;

	while( m_bIsRunning )
	{
		bWorking = FALSE;

		// 1.  결과를 DB에 UPDATE 합니다.
		i32Count	= m_pRingOut->GetBufferCount();

		for( INT32 i = 0; i < i32Count; i++ )
		{
			pDBUserKickOut	= (DB_USER_KICK_OUT*)m_pRingOut->Pop();

			if( EVENT_ERROR_SUCCESS == pDBUserKickOut->i32Rv )		ui8IsInsert = KICKLIST_INSERT_STATE_COMPLETE;
				else												ui8IsInsert = KICKLIST_INSERT_STATE_FAIL;			// 실패지만 유저가 이미 로그아웃 한 경우만 존재합니다.

			S2DB::MakeQueryW( wstrSQL, L"usp_KickList_upd",
				DB_DATA_TYPE_INT,			pDBUserKickOut->i32UserKickIdx,
				DB_DATA_TYPE_TINYINT,		ui8IsInsert,
				DB_DATA_TYPE_END);

			bSPResult	= TRUE;
			if( !m_pADO->ExecuteSP( wstrSQL ) )
			{
				bSPResult = FALSE;
				ErrorDBKickList( wstrSQL, TRUE );
			}
			if( 1 != m_pADO->GetRecordCount() )
			{
				bSPResult = FALSE;
				ErrorDBKickList( wstrSQL );
			}
			if( 1 != m_pADO->GetINT32Data(0) )
			{
				bSPResult = FALSE;
				ErrorDBKickList( wstrSQL );
			}

			if( FALSE == bSPResult )
				{	 // Update 실패시 기록을 남겨줍니다.
					g_pLog->WriteLog( L"USERKICK UPDATE ERROR : IDX : %d / UID : %I64d / Rv : %x", 
						pDBUserKickOut->i32UserKickIdx, pDBUserKickOut->i64UID, pDBUserKickOut->i32Rv );
				}

				m_pRingOut->PopIdx();

		}

		UINT64 ui64Result = 0;	// 비트플래그(0:실패, 1:성공). USER_KICK_MAX 가 64개 이상이 되면 수정되야 합니다.
		T_UserKickIdx i32ResultIdx[ USER_KICK_MAX ];

		S2DB::MakeQueryW( wstrSQL, L"usp_KickList_sel",
			DB_DATA_TYPE_INT,		USER_KICK_MAX,
			DB_DATA_TYPE_END);

		if( !m_pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBKickList( wstrSQL, TRUE);

			// 테이블 또는 SP가 없는경우. 10초 쉽니다.
			::WaitForSingleObject( m_hThread, 10000 );
			continue;
		}

		// 2. 유저 킥을 위해 Push 합니다.
		i32Count = m_pADO->GetRecordCount();

		for( INT32 i = 0; i < i32Count; i++ )
		{
			i32UserKickIdx	= m_pADO->GetINT32Data(0);
			i64UID			= m_pADO->GetINT64Data(1);

			i32ResultIdx[i] = i32UserKickIdx;
			bWorking = TRUE;

			pDBUserKickIn	= (DB_USER_KICK_IN *)m_pRingIN->PushPointer();

			if( pDBUserKickIn != NULL)
			{
				pDBUserKickIn->i32UserKickIdx	= i32UserKickIdx;
				pDBUserKickIn->i64UID			= i64UID;

				m_pRingIN->PushPointerIdx();
				ui64Result |= (UINT64)0x01 << i;
			}
			else   
			{
				g_pLog->WriteLog( L"KICKLIST BUFFER FULL / DB IDX = %d / UID = %I64d ", i32UserKickIdx, i64UID );  
			}

			m_pADO->MoveNext();
		}

		// 3. 2의 결과를 DB에 UPDATE합니다.
		for(INT32 i = 0; i < i32Count; i++)
		{
			UINT8 ui8Result = KICKLIST_INSERT_STATE_FAIL;
			if( ui64Result & (UINT64)0x01 << i )
			{
				ui8Result =  KICKLIST_INSERT_STATE_ING;
			}

			S2DB::MakeQueryW( wstrSQL, L"usp_KickList_upd",
					DB_DATA_TYPE_INT,			i32ResultIdx[ i ],
					DB_DATA_TYPE_TINYINT,		ui8Result,
					DB_DATA_TYPE_END);

			if( !m_pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBKickList( wstrSQL, TRUE);
				continue;
			}
			if( 1 != m_pADO->GetRecordCount() )
			{
				ErrorDBKickList( wstrSQL );
			}
			if( 1 != m_pADO->GetINT32Data(0) )
			{
				ErrorDBKickList( wstrSQL );
			}
		}

		if( FALSE == bWorking ) 
		{	// 한일이 없다면 1초간 휴식합니다.
			::WaitForSingleObject( m_hThread, 1000 );
		}
		else 
		{
			::WaitForSingleObject( m_hThread, 1 );
		}

	}

	return;
}

void CModuleDBKickList::ErrorDBKickList( wchar_t * wstrSQL, BOOL bReconnect)
{
	g_pLog->WriteLog( L"[ErrorDBKickList] %s, Error Code : %08x / Reconnect : %d", wstrSQL, m_pADO->GethrLastError(), bReconnect);
	
	if (bReconnect)
	{
		HRESULT EventType = m_pADO->ReConnect(
			m_pLinkDBConfig->GetDataSource(),


			m_pLinkDBConfig->GetCatalog(),
			m_pLinkDBConfig->GetAccount(),
			m_pLinkDBConfig->GetPassword()
		);

		switch (EventType)
		{
		case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
			g_pLog->WriteLog(WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBKickList] RECONNECT FAILE");
			break;
		case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBKickList] RECONNECT SUCCEED");
			break;
		case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우

			break;
		}
	}
}

UINT32 CModuleDBKickList::GetBufferCount()
{
	return m_pRingIN->GetBufferCount();
}

DB_USER_KICK_IN* CModuleDBKickList::Pop()
{
	return (DB_USER_KICK_IN*)m_pRingIN->Pop();
}

INT32 CModuleDBKickList::Push( DB_USER_KICK_OUT *pDBUserKickOut )
{
	return m_pRingOut->Push( pDBUserKickOut );
}

void CModuleDBKickList::PopIdx()
{
	m_pRingIN->PopIdx();
}
