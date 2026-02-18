#pragma once


#define MAX_DATABASE_COUNT			50
#define DB_STRING_LENGTH			32

#pragma pack(push, 1)

struct DATABASE_CONFIG
{
	char			m_szDBName[DB_STRING_LENGTH];
	char			m_szDataSource[DB_STRING_LENGTH];
	char			m_szCatalog[DB_STRING_LENGTH];
	char			m_szAccount[DB_STRING_LENGTH];
	char			m_szPassword[DB_STRING_LENGTH];
};

#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////
// CDBConfig
class CDBConfig
{
public:
	bool			IsEqualDBName(char* pszDBName);

	char*			GetDataSource()										{ return m_szDataSource; }
	char*			GetCatalog()										{ return m_szCatalog; }
	char*			GetAccount()										{ return m_szAccount; }
	char*			GetPassword()										{ return m_szPassword; }

	void			SetDatabaseConfig(DATABASE_CONFIG* pDBConfig);
	void			MakePacketDBConfig(DATABASE_CONFIG* pDBConfig);


public:
	CDBConfig();
	virtual ~CDBConfig();


private:
	char			m_szDBName[DB_STRING_LENGTH];
	char			m_szDataSource[DB_STRING_LENGTH];
	char			m_szCatalog[DB_STRING_LENGTH];
	char			m_szAccount[DB_STRING_LENGTH];
	char			m_szPassword[DB_STRING_LENGTH];
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseMgr
class CDatabaseMgr
{
public:
	bool			InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig);

	CDBConfig*		FindDatabaseConfig(char* pszDBName);
	CDBConfig*		FindDatabaseConfig(char* pszDBName, INT32 i32DBNumber);
	

public:
	CDatabaseMgr();
	virtual ~CDatabaseMgr();


private:
	INT32			m_i32DBCount;
	CDBConfig		m_DBConfig[MAX_DATABASE_COUNT];
};



//////////////////////////////////////////////////////////////////////////
// CDBConfigMgr
class CDBConfigMgr
{
public:
	INT32			GetDatabaseConfigCount(ASC_SERVER_TYPE eServe)									{ return m_ai32DBCount[eServe]; }
	bool			InsertDatabaseConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig);
	void			MakePacketDBConfig(ASC_SERVER_TYPE eServer, DATABASE_CONFIG* pDBConfig);

public:
	CDBConfigMgr();
	virtual ~CDBConfigMgr();


private:
	INT32			m_ai32DBCount[ASC_SERVER_COUNT];
	CDBConfig		m_DBConfig[ASC_SERVER_COUNT][MAX_DATABASE_COUNT];
};
