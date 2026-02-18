#include "pch.h"
#include "PCCafeDB.h"

CPCCafeDB::CPCCafeDB()
: m_pDB(NULL)
, m_pDBConfig(NULL)
{
}

CPCCafeDB::~CPCCafeDB()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CPCCafeDB::OnCreate( INT32 i32Idx )
{
	m_pDB = CADODatabase::NewObject();

	m_pDBConfig = g_pConfig->GetDatabaseConfig("PBLink");
	if( !m_pDBConfig )
		return FALSE;

	if( !m_pDB->Connect(m_pDBConfig->GetDataSource(), m_pDBConfig->GetCatalog(), m_pDBConfig->GetAccount(), m_pDBConfig->GetPassword()) )
		return FALSE;

	return CPCCafeDB::OnCreate( i32Idx );
}

void CPCCafeDB::OnDestroy()
{
	I3_SAFE_RELEASE( m_pDB );
}

BOOL CPCCafeDB::ReConnect( wchar_t* wstrQuery )
{
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"PCCafe SP Fail / SP : %s ", wstrQuery);

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