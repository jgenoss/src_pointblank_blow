#include "pch.h"
#include "ModuleDBStats.h"
#include "SystemTimer.h"
#include "TaskProcessor.h"

static UINT32 _DefThreadProc(void * pPointer)
{
	((CModuleDBStats*)pPointer)->Working(); 
	return 0; 
}

CModuleDBStats::CModuleDBStats()
{
	m_hThread			= NULL;
	m_bIsRunning		= false;

	m_pSystemTimer		= NULL;
	m_pADO				= NULL;
	m_pRingPush			= NULL;
}

CModuleDBStats::~CModuleDBStats()
{
	Destroy();
}

bool CModuleDBStats::Create()
{
	//--------------------------------------------------------------------------------------------------------
	m_pSystemTimer	= new CSystemTimer;
	if( NULL == m_pSystemTimer )													return false;
	if( false == m_pSystemTimer->Create() )											return false;
	
	m_pZPLogDBConfig = g_pConfig->GetDatabaseConfig("ZLog");
	if( !m_pZPLogDBConfig )
		return false;
	
	//For NickName 
	m_pADO = CADODatabase::NewObject(); 
	if( !m_pADO->Connect(m_pZPLogDBConfig->GetDataSource(), m_pZPLogDBConfig->GetCatalog(), m_pZPLogDBConfig->GetAccount(), m_pZPLogDBConfig->GetPassword()) )
		return false; 

	m_pRingPush	= new S2RingBuffer;
	if( NULL == m_pRingPush )														return false;
	if( false == m_pRingPush->Create( sizeof(STATS_PUSH), 10 ) )					return false;

	DWORD dwThreadID;
	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , this, 0, &dwThreadID );
	if( NULL == m_hThread )															return false;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_BELOW_NORMAL) )		return false;
	
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER STATS"  ); 	

	return true;
}

void CModuleDBStats::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	
	I3_SAFE_RELEASE( m_pADO );
	I3_SAFE_DELETE( m_pRingPush );
	I3_SAFE_DELETE( m_pSystemTimer );
}

void CModuleDBStats::Working(void)
{
	bool		bWorked;
	STATS_PUSH*		pPush; 

	m_i32LogIdx = g_pLog->AddLogThread(L"STAT", 0, GetCurrentThreadId());

	m_bIsRunning = true;

	while( m_bIsRunning )
	{
		// Thread Timer
		m_pSystemTimer->RenewElapsedTime();

		bWorked = false;

		if( 0 == m_pRingPush->GetBufferCount() )
		{
			// 처리할 일이 없습니다. Continue;
			goto e_Exit;
		}

		pPush	= (STATS_PUSH *)m_pRingPush->Pop();
		
		if( NULL == pPush )
		{	// Buffer Full
			g_pLog->WriteLog( L"[ModuleDBNick] Ring In Buffer Full" );
			goto e_Exit; 
		}

		switch( pPush->i8Type )
		{
		case STATS_TYPE_CCU_MIN:
			{
				_SetDBCCU( pPush->ui32Date, (INT32)pPush->ui64Value );
			}
			break;
		}

		m_pRingPush->PopIdx();
		bWorked = true;

e_Exit:
		REAL32 r32Elapsed = m_pSystemTimer->RenewElapsedTime();
		if(CHECK_TIMEOUT < r32Elapsed )
		{
			g_pLog->WriteLog( L"[ModuleDBNick] DB Time Over : %d(ms)", (UINT32)(100*r32Elapsed) );
		}

		if( false == bWorked )			::WaitForSingleObject( m_hThread, 1 );
	}

	return; 
}

void CModuleDBStats::_SetDBCCU( UINT32 ui32Date, UINT32 ui32Value )
{
	wchar_t		wstrSQL[SERVER_STRING_COUNT];
	wchar_t		wstrSmallDataTime[SERVER_STRING_COUNT];

	S2DB::ConvertUINT32ToSmallDate(wstrSmallDataTime, SERVER_STRING_COUNT, ui32Date);

	S2DB::MakeQueryW( wstrSQL, L"usp_Set_CCU",
		DB_DATA_TYPE_SMALLDATETIME,		wstrSmallDataTime,
		DB_DATA_TYPE_INT,				ui32Value,
		DB_DATA_TYPE_END);
	if( FALSE == m_pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBStats( wstrSQL );
		return;
	}
	if( 1 != m_pADO->GetRecordCount() )
	{
		_DataBaseResultFailLog( wstrSQL );		
		return;
	}
	if( 1 != m_pADO->GetINT32Data(0))
	{
		_DataBaseResultFailLog( wstrSQL );
		return;
	}
}

INT32 CModuleDBStats::Push( INT8 i8Type, UINT32 ui32Date, UINT64 ui64Value )
{
	STATS_PUSH * pPush = (STATS_PUSH *)m_pRingPush->PushPointer();
	if( NULL == pPush )			return EVENT_RETURN_ERROR_BUFFER;

	// BU_UID에 관련된 uid값이 푸쉬되었을 경우에는 i8Type = STATS_TYPE_BU_UID, ui32Date = 0, ui64Value = 구매자UID의 값이 들어갑니다.
	// BU에 관련된 값이 푸쉬되었을 경우에는 i8Type = STATS_TYPE_BU, ui32Date = 날짜, ui64Value = 0이 들어갑니다.
	pPush->i8Type		= i8Type; 
	pPush->ui32Date		= ui32Date; 
	pPush->ui64Value	= ui64Value;

	m_pRingPush->PushPointerIdx();

	return EVENT_RETURN_OK;  
}

void CModuleDBStats::ErrorDBStats( wchar_t * wstrSQL)
{
	g_pLog->WriteLog( L"[ErrorDBStats] %s, Error Code : %08x", wstrSQL, m_pADO->GethrLastError() );
	
	HRESULT EventType = m_pADO->ReConnect(
																	m_pZPLogDBConfig->GetDataSource(),
																	m_pZPLogDBConfig->GetCatalog(),
																	m_pZPLogDBConfig->GetAccount(),
																	m_pZPLogDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBStats] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBStats] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBStats::_DataBaseResultFailLog( wchar_t * wstrSQL)
{
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSQL ); 
}