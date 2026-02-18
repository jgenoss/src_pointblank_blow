#include "pch.h"
#include "DatabaseConfig.h"


//////////////////////////////////////////////////////////////////////////
// CDBConfig
CDBConfig::CDBConfig()
{
}

CDBConfig::~CDBConfig()
{
}

bool CDBConfig::IsEqualDBName(char* pszDBName)
{
	return strcmp(m_szDBName, pszDBName) == 0;
}

void CDBConfig::SetDatabaseConfig(DATABASE_CONFIG* pDBConfig)
{
	i3String::Copy( m_szDBName,		pDBConfig->m_szDBName,		DB_STRING_LENGTH);
	i3String::Copy( m_szDataSource, pDBConfig->m_szDataSource,	DB_STRING_LENGTH);
	i3String::Copy( m_szCatalog,	pDBConfig->m_szCatalog,		DB_STRING_LENGTH);
	i3String::Copy( m_szAccount,	pDBConfig->m_szAccount,		DB_STRING_LENGTH);
	i3String::Copy( m_szPassword,	pDBConfig->m_szPassword,	DB_STRING_LENGTH);
}

void CDBConfig::MakePacketDBConfig(DATABASE_CONFIG* pDBConfig)
{
	i3String::Copy( pDBConfig->m_szDBName,		m_szDBName,		DB_STRING_LENGTH);
	i3String::Copy( pDBConfig->m_szDataSource,	m_szDataSource, DB_STRING_LENGTH);
	i3String::Copy( pDBConfig->m_szCatalog,		m_szCatalog,	DB_STRING_LENGTH);
	i3String::Copy( pDBConfig->m_szAccount,		m_szAccount,	DB_STRING_LENGTH);
	i3String::Copy( pDBConfig->m_szPassword,	m_szPassword,	DB_STRING_LENGTH);
}

CDatabaseMgr::CDatabaseMgr()
: m_i32DBCount(0)
{
}

CDatabaseMgr::~CDatabaseMgr()
{
}

CDBConfig* CDatabaseMgr::FindDatabaseConfig(char* pszDBName)
{
	for( INT32 i = 0; i < m_i32DBCount; ++i )
	{
		if( m_DBConfig[i].IsEqualDBName(pszDBName) )
			return &m_DBConfig[i];
	}

	return NULL;
}

CDBConfig* CDatabaseMgr::FindDatabaseConfig(char* pszDBName, INT32 i32DBNumber)
{
	char szDBName[DB_STRING_LENGTH] = {0, };
	sprintf_s(szDBName, DB_STRING_LENGTH, "%s%02d", pszDBName, i32DBNumber);

	for( INT32 i = 0; i < m_i32DBCount; ++i )
	{
		if( m_DBConfig[i].IsEqualDBName(szDBName) )
			return &m_DBConfig[i];
	}

	return NULL;
}

bool CDatabaseMgr::InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)
{
	if( MAX_DATABASE_COUNT <= m_i32DBCount )
		return false;
	
	m_DBConfig[m_i32DBCount++].SetDatabaseConfig(pDBConfig);

	return true;
}



//////////////////////////////////////////////////////////////////////////
// CDBConfigMgr
CDBConfigMgr::CDBConfigMgr()
{
	for( INT32 i = 0; i < ASC_SERVER_COUNT; ++i )
		m_ai32DBCount[i] = 0;
}

CDBConfigMgr::~CDBConfigMgr()
{
}

bool CDBConfigMgr::InsertDatabaseConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig)
{
	if( MAX_DATABASE_COUNT <= m_ai32DBCount[eServer] )
		return false;

	m_DBConfig[eServer][m_ai32DBCount[eServer]++].SetDatabaseConfig(pDBConfig);

	return true;
}

void CDBConfigMgr::MakePacketDBConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig)
{
	for( INT32 i = 0; i < m_ai32DBCount[eServer]; ++i )
		m_DBConfig[eServer][i].MakePacketDBConfig(&pDBConfig[i]);
}
