#include "pch.h"
#include "BillingDB.h"

CBillingDB::CBillingDB()
: m_pDB(NULL)
, m_pBillingDBConfig(NULL)
{
}

CBillingDB::~CBillingDB()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CBillingDB::OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx )
{
	m_pDB = CADODatabase::NewObject();

	m_pBillingDBConfig = g_pConfig->GetDatabaseConfig("Billing");
	if( !m_pBillingDBConfig )
		return FALSE;

	if( !m_pDB->Connect(m_pBillingDBConfig->GetDataSource(), m_pBillingDBConfig->GetCatalog(), m_pBillingDBConfig->GetAccount(), m_pBillingDBConfig->GetPassword()) )
		return FALSE;

	return CBillingBase::OnCreate( i32Idx, ui8MethodIdx );
}

void CBillingDB::OnDestroy()
{
	I3_SAFE_RELEASE( m_pDB );
}

BOOL CBillingDB::ReConnect( wchar_t* wstrQuery )
{
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"BillingDB SP Fail / SP : %s ", wstrQuery);

	if( m_pDB->IsConnected() )	 g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"ReConnectDataBase CONNECTED" );
	else						 g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"ReConnectDataBase CONNECTE" ); 

	m_pDB->Close();
	if( FALSE == m_pDB->Connect( m_pBillingDBConfig->GetDataSource(), m_pBillingDBConfig->GetCatalog(), m_pBillingDBConfig->GetAccount(), m_pBillingDBConfig->GetPassword() ) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"ReConnect Fail" ); 
		
		return FALSE;
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"ReConnect Success" ); 
	
	return TRUE;
}