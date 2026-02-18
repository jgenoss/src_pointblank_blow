#include "pch.h"
#include "ModuleDBUserSave.h"

struct Thread_Paramater
{
	void*	m_pClass;
	INT32	m_i32Idx;
};

I3_CLASS_INSTANCE( CModuleDBUserSave, i3ElementBase);

static UINT32 _DefThreadProc( void * pPointer )
{
	Thread_Paramater* pParameter	= (Thread_Paramater*)pPointer;
	CModuleDBUserSave* pClass		= (CModuleDBUserSave*)pParameter->m_pClass;
	INT32 i32ThreadIdx				= pParameter->m_i32Idx;
	I3_SAFE_DELETE( pParameter );

	pClass->OnWorking( i32ThreadIdx ); 
	return 0; 
}

CModuleDBUserSave::CModuleDBUserSave(void)
{
	m_i32ExitSaveOutIdx		= 0;
	m_i32CreateNickOutIdx	= 0;
	m_i32CreateCharOutIdx	= 0;

	//m_ppNickDB				= NULL;
	
	i3mem::FillZero( m_ppNickDB,			sizeof(CADODatabase	*)		* DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppSaveDB,			sizeof(CADODatabase	*)		* DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_phThread,			sizeof(HANDLE)				* DATABASE_THREAD_COUNT);	
	i3mem::FillZero( m_pbRunning,			sizeof(BOOL)				* DATABASE_THREAD_COUNT);

	i3mem::FillZero( m_ppExitSaveIn,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppExitSaveOut,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppCreateNickIn,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppCreateNickOut,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppCreateCharIn,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
	i3mem::FillZero( m_ppCreateCharOut,		sizeof(i3RingBuffer *) * DATABASE_THREAD_COUNT);
}


CModuleDBUserSave::~CModuleDBUserSave(void)
{
	OnDestroy(); 
}

BOOL CModuleDBUserSave::OnCreate()
{
	DWORD dwThreadID; 
		
	//--------------------------------------------------------------------------------------------------------
	//For User Info Save 
	m_pNickDBConfig = g_pConfig->GetDatabaseConfig("PBUser");
	if( !m_pNickDBConfig )
		return FALSE;

	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{	

		m_ppNickDB[i]		= CADODatabase::NewObject();
		if( NULL == m_ppNickDB[i] )																		return FALSE;
		if( FALSE == m_ppNickDB[i]->Connect(m_pNickDBConfig->GetDataSource(), m_pNickDBConfig->GetCatalog(), m_pNickDBConfig->GetAccount(), m_pNickDBConfig->GetPassword()) )
			return FALSE;

		char szDBName[DB_STRING_LENGTH];
		i3String::Format(szDBName, DB_STRING_LENGTH, "PBUser%02d", i);
		m_pSaveDBConfig[i] = g_pConfig->GetDatabaseConfig(szDBName);

		//Set DataBase
		m_ppSaveDB[i]	= CADODatabase::NewObject();
		if( NULL == m_ppSaveDB[i] )																		return FALSE;
		if( FALSE == m_ppSaveDB[i]->Connect(m_pSaveDBConfig[i]->GetDataSource(), m_pSaveDBConfig[i]->GetCatalog(), m_pSaveDBConfig[i]->GetAccount(), m_pSaveDBConfig[i]->GetPassword()) )
			return FALSE;		

		m_ppExitSaveIn[i]	= i3RingBuffer::NewObject();
		if( NULL == m_ppExitSaveIn[i] )																	return FALSE;
		if( FALSE == m_ppExitSaveIn[i]->Create( sizeof(RING_EXIT_SAVE_IN), DB_SAVEINFO_COUNT ) )		return FALSE;

		m_ppExitSaveOut[i]	= i3RingBuffer::NewObject(); 
		if( NULL == m_ppExitSaveOut[i] )																return FALSE;
		if( FALSE == m_ppExitSaveOut[i]->Create( sizeof(RING_EXIT_SAVE_OUT), DB_SAVEINFO_COUNT ) )		return FALSE;  

		m_ppCreateNickIn[i] = i3RingBuffer::NewObject();
		if( NULL == m_ppCreateNickIn[i] )																return FALSE;
		if( FALSE == m_ppCreateNickIn[i]->Create( sizeof(RING_CREATE_NICK_IN), DB_SAVEINFO_COUNT ) )	return FALSE; 

		m_ppCreateNickOut[i] = i3RingBuffer::NewObject();
		if( NULL == m_ppCreateNickOut[i] )																return FALSE;
		if( FALSE == m_ppCreateNickOut[i]->Create( sizeof(RING_CREATE_NICK_OUT), DB_SAVEINFO_COUNT ) )	return FALSE; 

		m_ppCreateCharIn[i] = i3RingBuffer::NewObject();
		if( NULL == m_ppCreateCharIn[i] )																return FALSE;
		if( FALSE == m_ppCreateCharIn[i]->Create( sizeof(RING_CREATE_CHAR), DB_SAVEINFO_COUNT ) )	return FALSE; 

		m_ppCreateCharOut[i] = i3RingBuffer::NewObject(); 
		if( NULL == m_ppCreateCharOut[i] )																return FALSE;
		if( FALSE == m_ppCreateCharOut[i]->Create( sizeof(RING_CREATE_CHAR), DB_SAVEINFO_COUNT ) )	return FALSE; 

		Thread_Paramater* pParameter	= new Thread_Paramater;
		pParameter->m_pClass			= this;
		pParameter->m_i32Idx			= i;

		//Make Thread 		
		m_phThread[i] = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , pParameter , 0, &dwThreadID);
		if(m_phThread[i] == NULL)return FALSE; 
		
		//Set Thread Priority
		if( !SetThreadPriority( m_phThread[i], THREAD_PRIORITY_NORMAL) )return FALSE;

		m_i32LoginCount[i] = 0;
		m_dwTempTime[i] = GetTickCount();
	}
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE WORKER USERINFO"  );


	return TRUE; 
}

void CModuleDBUserSave::OnDestroy(void)
{
	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++ )
	{
		m_pbRunning[i]		= FALSE;
	}

	::WaitForMultipleObjects( DATABASE_THREAD_COUNT, m_phThread, TRUE, INFINITE );

	for( INT32 i = 0; i < DATABASE_THREAD_COUNT; i++ )
	{
		I3_SAFE_RELEASE( m_ppCreateCharIn[i] );
		I3_SAFE_RELEASE( m_ppCreateCharOut[i] );
		I3_SAFE_RELEASE( m_ppCreateNickIn[i] );
		I3_SAFE_RELEASE( m_ppCreateNickOut[i] );
		I3_SAFE_RELEASE( m_ppExitSaveIn[i] );
		I3_SAFE_RELEASE( m_ppExitSaveOut[i] );
		I3_SAFE_RELEASE( m_ppSaveDB[i] );
		I3_SAFE_RELEASE( m_ppNickDB[i] );
	}

	return; 
}

void CModuleDBUserSave::OnWorking( INT32 i32ThreadIdx )
{
	BOOL bWorked;

	wchar_t wstrLog[MAX_PATH];
	i3String::Format(wstrLog, MAX_PATH, L"SV%02d", i32ThreadIdx);
	m_pi32LogIdx[i32ThreadIdx] = g_pLog->AddLogThread(wstrLog, 0, GetCurrentThreadId());

	m_pbRunning[i32ThreadIdx] = TRUE;

	while( m_pbRunning[i32ThreadIdx] )
	{
		bWorked = FALSE;

		if( _WorkingExitSave( i32ThreadIdx ) )	bWorked = TRUE;
		if( _WorkingCreateNick( i32ThreadIdx ) )	bWorked = TRUE;
		if( _WorkingCreateChar( i32ThreadIdx ) )	bWorked = TRUE;

		if( FALSE == bWorked ) Sleep(1); 
	}

	return; 
}

void CModuleDBUserSave::ErrorDBUserSave( INT32 i32ThreadIdx, wchar_t* wstrSQL )
{
	g_pLog->WriteLog( L"[ErrorDBUserSave] %s, Error Code : %08x", wstrSQL, m_ppSaveDB[ i32ThreadIdx ]->GethrLastError() );
	
	HRESULT EventType = m_ppSaveDB[ i32ThreadIdx ]->ReConnect(
																	m_pSaveDBConfig[i32ThreadIdx]->GetDataSource(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetCatalog(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetAccount(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUserSave] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUserSave] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBUserSave::ErrorDBUserNick( INT32 i32ThreadIdx, wchar_t * wstrSQL )
{
	g_pLog->WriteLog( L"[ErrorDBUserNick] %s, Error Code : %08x", wstrSQL, m_ppNickDB[ i32ThreadIdx ]->GethrLastError() );
	
	HRESULT EventType = m_ppNickDB[ i32ThreadIdx ]->ReConnect(
																	m_pSaveDBConfig[i32ThreadIdx]->GetDataSource(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetCatalog(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetAccount(),
																	m_pSaveDBConfig[i32ThreadIdx]->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBUserNick] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBUserNick] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CModuleDBUserSave::_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t* wstrSQL)
{	
	g_pLog->WriteLog( L"[DB_ERROR] %s", wstrSQL  ); 
}