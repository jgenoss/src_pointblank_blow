#ifndef _CONFIG_H__
#define _CONFIG_H__

#define	SERVER_NAME_COUNT	32
#define IP_NAME_COUNT		32

#include "../../CommonServerSource/DatabaseConfig.h"


class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );

private:
	UINT32					m_ui32ConnectedGameServerCount;

	CDatabaseMgr			m_DatabaseManager;

protected:
	UINT32					m_ui32ActiveGameServerCount;

	//Server List
	INT32					m_i32ServerCount;

	INT32					m_i32GameServerCount;
	SERVER_BASE*			m_pGameServer;

	INT32					m_i32BattleServerCount;
	SERVER_BATTLE*			m_pBattleServer;	

public:
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	SS_VERSION				m_Version;
	SS_VERSION				m_ControlVersion;

	wchar_t					m_wstrLogFilePath[ SERVER_STRING_COUNT ];				//Log 파일 패스 
	
	//관제 서버
	UINT32					m_ui32ControlIP;
	UINT16					m_ui16ControlServerPort;
	
	INT32					m_i32ThreadCount;

	CContentsMgr			m_ContentsManager;

	bool					m_bTestMode;

	CMapLoadManager			m_MapLoadManager;

public:
	CConfig(void);
	~CConfig(void);

	void					OnUpdate(void);

	UINT32					GetConnectedGameServerCount(void) const;

	void					AddConnectedGameServerCount(void);
	void					SubConnectedGameServerCount(void);
	void					SetClanCount(UINT32 count);

	INT32					LoadLogFile(const char * pstrFileName);
	BOOL					LoadConfigFile(const char * pstrFileName);

	INT32					GetServerCount()									{	return m_i32ServerCount;	}

	INT32					GetGameServerCount()								{	return m_i32GameServerCount;	}
	void					SetGameServerCount( INT32 i32ServerCount );	
	void					SetGameServerIP( INT32 i32Idx, UINT32 ui32IP );
	UINT32					GetGameServerIP(INT32 i32Idx);

	INT32					GetBattleServerCount()								{	return m_i32BattleServerCount;	}
	void					SetBattleServerCount( INT32 i32ServerCount );	
	void					SetBattleServerIP( INT32 i32Idx, UINT32 ui32IP );
	UINT32					GetBattleServerIP(INT32 i32Idx);

	INT32					GetIOCPThreadCount()								{	 return m_i32ThreadCount; }

	UINT32					GetActiveGameServerCount(void) const				{	 return m_ui32ActiveGameServerCount; }

	bool					InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)	{	 return m_DatabaseManager.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*				GetDatabaseConfig(char* pszDBName)					{	 return m_DatabaseManager.FindDatabaseConfig(pszDBName); }

	// Control contents
	void					InsertContents(ContentsInfo* pContents);
	bool					IsContentsEnabled(CONTENTS eContents)				{	 return m_ContentsManager.IsContensEnabled(eContents); }

	bool					IsTestMode()										{	 return m_bTestMode; }

	INT32					GetServerIdx( ASC_SERVER_TYPE eServerType, UINT32 ui32IP );

	void					SetAllowRoomCreate( INT32 i32Idx, BOOL bActive );
	INT32					GetEmptyBattleServer( STAGE_UID eStageUID );
	void					FreeBattleServer( INT32 i32Idx );
	void					SetBattleRoomCount( INT32 i32ServerIdx, INT32 i32RoomCount );

};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__