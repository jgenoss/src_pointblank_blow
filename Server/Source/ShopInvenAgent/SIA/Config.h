#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "../../CommonServerSource/DatabaseConfig.h"

class CConfig
{
	CDatabaseMgr			m_DatabaseConfig;
	CContentsMgr			m_ContentsManager;

public:
	wchar_t					m_wstrLogFilePath[ PATH_STRING_COUNT ];
	
	// Version
	SS_VERSION				m_Version;
	SS_VERSION				m_ControlVersion;

	UINT32					m_ui32Ip;										//Server IP
	UINT16					m_ui16Port;										//Server PORT
	UINT8					m_ui8Active;									//Server Active

	UINT32					m_ui32ControlIP;
	UINT16					m_ui16ControlPort;

	INT32					m_i32LoginThreadCount;								// Login Thread Count
	INT32					m_i32LogInBufferCount;
	UINT8					m_ui8LoginMethodCount;								// 설정할 로그인 연동 갯수
	LINK_METHOD				m_aeLoginMethod[LINK_METHOD_MAX];					// Login Type
	char					m_astrLoginHttp[LINK_METHOD_MAX][PATH_STRING_COUNT];	//Link Http Address
	UINT8					m_aui8LoginHttpMethod[LINK_METHOD_MAX];
	UINT32					m_aui32LoginIp[LINK_METHOD_MAX];
	UINT16					m_aui16LoginPort[LINK_METHOD_MAX];

	INT32					m_i32BillingThreadCount;
	INT32					m_i32BillingBufferCount;
	UINT8					m_ui8BillingMethodCount;						// 설정할 빌링 연동 갯수
	LINK_METHOD				m_aeBillingMethod[LINK_METHOD_MAX];				// Billing Type
	char					m_astrBillingHttp[LINK_METHOD_MAX][PATH_STRING_COUNT];
	UINT32					m_aui32BillingIp[LINK_METHOD_MAX];
	UINT16					m_aui16BillingPort[LINK_METHOD_MAX];

	INT32					m_i32PCCafeThreadCount;							// PCCafe Thread Count
	INT32					m_i32PCCafeBufferCount;							// PCCafe In/Out Buffer Count
	LINK_METHOD				m_ePCCafeMethod;								// PCCafe Type
	UINT32					m_ui32PCCafeIp; 
	UINT16					m_ui16PCCafePort; 

	INT32					m_i32ServerCount;								//Trans Count
	UINT32*					m_pServerIP;									//Trans IP

	INT32					m_i32DataBaseThreadCount;						//INVENTORY_TABLE_COUNT 이 값과 동일합니다. 
	UINT64					m_ServiceIdForRussia;							// use for Russia servive
	
	UINT8					m_ui8ConBuildVerMonth;							// 컨트롤 서버 빌드 버전 Month
	UINT8					m_ui8ConBuildVerDay;							// 컨트롤 서버 빌드 버전 Day

public:
	CConfig();
	~CConfig();

	INT32					LoadLogInfo( const char * strFileName );
	BOOL					LoadConfigFile( const char * strFileName );

	CDBConfig*				GetDatabaseConfig(char* pstrDBName)					{ return m_DatabaseConfig.FindDatabaseConfig(pstrDBName); }
	bool					InsertDatabaseConfig(DATABASE_CONFIG* pDBConfig)	{ return m_DatabaseConfig.InsertDatabaseConfig(pDBConfig); }

	void					InsertContents(ContentsInfo* pContents);
	bool					IsContentsEnabled(CONTENTS eContents)						{ return m_ContentsManager.IsContensEnabled(eContents); }
};

extern CConfig * g_pConfig;

#endif