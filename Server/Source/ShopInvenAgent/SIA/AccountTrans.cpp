#include "pch.h"
#include "AccountTrans.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((CAccountTrans*)pPointer)->Running();
	return 0;
}

CAccountTrans::CAccountTrans()
{
	m_bRunning			= FALSE;
	m_hThread			= NULL;
	m_pDB				= NULL;

	m_ui32ResultTime	= 0;
}

CAccountTrans::~CAccountTrans()
{
	Destroy();
}

BOOL CAccountTrans::Create()
{	
	if( FALSE == g_pConfig->IsContentsEnabled( CONTENTS_ACCOUNT_TRANS ) )				return TRUE;

	m_pDB = CADODatabase::NewObject();

	m_pDBConfig = g_pConfig->GetDatabaseConfig("AccountTrans");
	if( NULL == m_pDBConfig )																return FALSE;

	if( !m_pDB->Connect(m_pDBConfig->GetDataSource(), m_pDBConfig->GetCatalog(), m_pDBConfig->GetAccount(), m_pDBConfig->GetPassword()) )
		return FALSE;

	m_hThread  = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&m_ui32ThreadID );
	if( NULL == m_hThread )																	return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_ACCOUNTTRANS) )				return FALSE;

	return TRUE;
}

void CAccountTrans::Destroy()
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	I3_SAFE_RELEASE( m_pDB );
}

void CAccountTrans::Running()
{
	g_pLog->AddLogThread( L"ACCT", 0, m_ui32ThreadID );

	wchar_t	wstrSP[SERVER_STRING_COUNT];

	T_UID	TGarenaUID;
	T_UID	TPBUID;
	
	INT32	i32WaitingTime;

	INT32	i32ResultCount = 0;

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		i32WaitingTime = 100;		// 100ms À¯ÈÞ½Ã°£
		
		i3String::Format(wstrSP, SERVER_STRING_COUNT, L"EXECUTE usp_AccountTransfer_sel" );
		if( FALSE == m_pDB->ExecuteSP(wstrSP) )
		{
			ReConnect(wstrSP);
			goto e_End;
		}

		if( 0 < m_pDB->GetRecordCount() )
		{		
			TGarenaUID	= m_pDB->GetINT64Data( 0 );
			TPBUID		= m_pDB->GetINT64Data( 1 );

			i3String::Format(wstrSP, SERVER_STRING_COUNT, L"EXECUTE usp_AccountTransfer_upd %I64d, %I64d", TGarenaUID, TPBUID );
			if( FALSE == m_pDB->ExecuteSP(wstrSP) )
			{
				ReConnect(wstrSP);
				goto e_End;
			}
			i32WaitingTime = 1;
			i32ResultCount++;
		}

		if( 0 < i32ResultCount )
		{
			if( m_ui32ResultTime < i3ThreadTimer::GetServerTime() )
			{
				m_ui32ResultTime = i3ThreadTimer::GetServerTime();

				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Account Trans Success Count:%d (/1s)", i32ResultCount);
				i32ResultCount = 0;
			}
		}
e_End:
		WaitForSingleObject( m_hThread, i32WaitingTime );
	}
}

BOOL CAccountTrans::ReConnect( wchar_t* wstrQuery )
{
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"CAccountTrans SP Fail / SP : %s ", wstrQuery);

	if( m_pDB->IsConnected() )	 g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"ReConnectDataBase CONNECTED" );
	else						 g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"ReConnectDataBase CONNECTE" ); 

	m_pDB->Close();
	if( FALSE == m_pDB->Connect( m_pDBConfig->GetDataSource(), m_pDBConfig->GetCatalog(), m_pDBConfig->GetAccount(), m_pDBConfig->GetPassword() ) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"ReConnect Fail" ); 
		
		return FALSE;
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"ReConnect Success" ); 
	
	return TRUE;
}