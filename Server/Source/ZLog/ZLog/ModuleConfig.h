#ifndef __MODULE_CONFIG__
#define __MODULE_CONFIG__

class CModuleConfig
{
public:
	UINT32				m_ui32Ip;
	UINT16				m_ui16Port;

	wchar_t				m_wstrLogFilePath[SERVER_STRING_COUNT];
	char				m_strServiceLogFilePath[MAX_PATH];

	UINT32				m_ui32ControlIP;
	UINT16				m_ui16ControlPort;

	UINT32				m_ui32ServerCount;
	UINT32	*			m_pui32ServerIP;

	UINT8				m_ui8WriteDBThreadCount;

	CDatabaseMgr		m_DatabaseManager;

	SS_VERSION			m_Version;
	SS_VERSION			m_ControlVersion;

public:
	CModuleConfig();
	virtual ~CModuleConfig();

	INT32				LoadLogInfo( const char * strFileName );
	BOOL				LoadConfigFile( char* pstrCurrentPath );
	void				OnDestroy();

	bool				InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)			{ return m_DatabaseManager.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*			GetDatabaseConfig(char* pstrDBName)							{ return m_DatabaseManager.FindDatabaseConfig(pstrDBName); }
};

extern CModuleConfig * g_pConfig;

#endif