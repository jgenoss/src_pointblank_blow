#include "pch.h"
#include "LoginDB.h"

CLoginDB::CLoginDB()
: m_pDB(NULL)
, m_pLoginDBConfig(NULL)
{
}

CLoginDB::~CLoginDB()
{
	OnDestroy();
}

void CLoginDB::OnDestroy(void)
{
	I3_SAFE_RELEASE( m_pDB ); 
}

BOOL CLoginDB::OnCreate( INT32 i32dx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx )
{
	m_pDB = CADODatabase::NewObject();
	if( NULL == m_pDB )
		return FALSE;

	m_pLoginDBConfig = g_pConfig->GetDatabaseConfig("Login");
	if( !m_pLoginDBConfig )
		return FALSE;

	if( FALSE == m_pDB->Connect(m_pLoginDBConfig->GetDataSource(), m_pLoginDBConfig->GetCatalog(), m_pLoginDBConfig->GetAccount(), m_pLoginDBConfig->GetPassword()) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CLoginDB::OnCreate] Connect Fail LoginDB" ); 
		return FALSE; 
	}
	return CLoginBase::OnCreate( i32dx, ui32BufferMaxCount, ui8MethodIdx );
}

BOOL CLoginDB::ReConnect(wchar_t* wstrQuery )
{
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"LoginDB SP Fail / SP : %s ", wstrQuery);
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_DATABASE_RECONNECT, L"ReConnectDataBase LoginDB RECONNECTED" ); 

	m_pDB->Close(); 
	m_pDB->Connect(m_pLoginDBConfig->GetDataSource(), m_pLoginDBConfig->GetCatalog(), m_pLoginDBConfig->GetAccount(), m_pLoginDBConfig->GetPassword()); 

	return TRUE; 
}
 