#ifndef _CONFIG_H__
#define _CONFIG_H__

#define	SERVER_NAME_COUNT	32
#define IP_NAME_COUNT		32
#define FILE_NAME			128

#include "../../CommonServerSource/DatabaseConfig.h"


class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );
public:
	UINT32			GetConnectedGameServerCount(void) const;
	
	void			AddConnectedGameServer(void);
	void			SubConnectedGameServer(void);
	void			SetConnectedUserCount(UINT32 ui32count);
	void			OnUpdate(void);

private:
	UINT32			m_ui32connectedGameServerCount;

	CDatabaseMgr	m_DatabaseConfig;

public:
	INT32			m_i32MacAddressListCount;
	UINT64	*		m_pui64MacAddressList;
		
	SS_VERSION		m_ControlVersion;
	SS_VERSION		m_Version;

	CConfig(void);
	~CConfig(void);

public:	
	//ServerInfo
	char			m_strIpAddress[IP_NAME_COUNT];
	UINT16			m_ui16Port;

	wchar_t			m_wstrLogFilePath[ SERVER_STRING_COUNT ];			// Log 파일 패스 
	
	//관제툴 서버 
	BOOL			m_bControlActive;
	char			m_strControlServerIp[IP_NAME_COUNT];
	UINT16			m_ui16ControlServerPort;

	INT32			m_i32UserMaxCount;				// 개발용.

	//Server List
	UINT32			m_ui32ServerCount;
	UINT32	*		m_pui32ServerIp;

	char			m_strServerStartTime[ SMALLDATE_STRING_SIZE ];

public:
	INT32			LoadLogFile(const char * pstrFileName);
	BOOL			LoadConfigFile(const char * pstrFileName);

	void			SetGameServerCount( UINT32 ui32ServerCount )		{ m_ui32ServerCount = ui32ServerCount; };

	bool			InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)	{ return m_DatabaseConfig.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*		GetDatabaseConfig(char* pstrDBName)					{ return m_DatabaseConfig.FindDatabaseConfig(pstrDBName); }

};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__
