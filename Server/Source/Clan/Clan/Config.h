#ifndef _CONFIG_H__
#define _CONFIG_H__

#define	SERVER_NAME_COUNT	32
#define IP_NAME_COUNT		32

#include "../../CommonServerSource/DatabaseConfig.h"


class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );

private:
	UINT32			m_ui32ConnectedGameServerCount;
	UINT32			m_ui32ClanCount;

	CDatabaseMgr	m_DatabaseManager;

protected:
	UINT32			m_ui32ActiveGameServerCount;


public:
	UINT32			m_ui32IPAddress;
	UINT16			m_ui16Port;

	SS_VERSION		m_ClanVersion;
	SS_VERSION		m_ControlVersion;

	wchar_t			m_wstrLogFilePath[ SERVER_STRING_COUNT ];			//Log 파일 패스 
	
	UINT32			m_ui32ClanMax;										// Clan 최대 갯수
	UINT8			m_ui8CreateMemberCount;								// 최초 클랜 생성 시 지급되는 맴버 수

	//NC Azit
	BOOL			m_bAzitActive;

	UINT8			m_ui8ClanCreateRank;
	INT32			m_i32ClanCreatePoint;

	//관제 서버
	UINT32			m_ui32ControlIP;
	UINT16			m_ui16ControlServerPort;

	//Server List
	UINT32			m_ui32ServerCount;
	UINT32	*		m_pui32ServerIp;

	CContentsMgr	m_ContentsManager;

	bool			m_bTestMode;


	CClanMatchSeason	m_ClanMatchSeason[SEASON_TYPE_COUNT];

public:
	CConfig(void);
	~CConfig(void);

	UINT32			GetConnectedGameServerCount(void) const;
	UINT32			GetClanCount(void) const;

	void			AddConnectedGameServerCount(void);
	void			SubConnectedGameServerCount(void);
	void			SetClanCount(UINT32 count);

	INT32			LoadLogFile(const char * pstrFileName);
	BOOL			LoadConfigFile(const char * pstrFileName);

	UINT32			GetServerCount()							{ return m_ui32ServerCount; }
	void			SetGameServerCount(UINT32 ui32ServerCount)	{ m_ui32ServerCount = ui32ServerCount; }

	void			OnUpdate(void);

	UINT32			GetActiveGameServerCount(void) const		{ return m_ui32ActiveGameServerCount; }

	bool			InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)	{ return m_DatabaseManager.InsertDatabaseConfig(pDBConfig); }
	CDBConfig*		GetDatabaseConfig(char* pszDBName)			{ return m_DatabaseManager.FindDatabaseConfig(pszDBName); }

	
	void			InsertClanMatchSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pClanMatchSeason);

	INT32			GetSeasonIndex(SEASON_TYPE eSeason)			{ return m_ClanMatchSeason[eSeason].GetSeasonIndex(); }
	bool			IsOngoingSeason(SEASON_TYPE eSeason = SEASON_TYPE_ONGOING)		
	{
		if( !IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) ) return false;
		return m_ClanMatchSeason[eSeason].GetSeasonIndex() != 0; 
	}

	// Control contents
	void			InsertContents(ContentsInfo* pContents);
	bool			IsContentsEnabled(CONTENTS eContents)		{ return m_ContentsManager.IsContensEnabled(eContents); }

	bool			IsTestMode()								{ return m_bTestMode; }

	bool			CheckGameServerIdx(UINT32 ui32ServerIdx);

	UINT64			MakeStateMatch(UINT64 ui64State, T_TeamID TTeamID);

};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__