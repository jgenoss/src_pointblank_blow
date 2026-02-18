#ifndef _CONFIG_H__
#define _CONFIG_H__

#define IP_NAME_COUNT		32

#include "../../CommonServerSource/CommonServerDef.h"

class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );

public:
	
	UINT8			GetCheckLevel( DS_HACK_TYPE Type )	{	
															if( Type > HACK_TYPE_NO && Type < HACK_TYPE_MAX )
																	return m_aui8HackCheckLevel[ Type ];
															return 0;
														}
	
	void			SetCheckLevel( DS_HACK_TYPE Type, UINT8 ui8Level ) {
															if( Type > HACK_TYPE_NO && Type < HACK_TYPE_MAX )
																m_aui8HackCheckLevel[ Type ] = ui8Level;
														}
	
		
	CConfig(void);
	~CConfig(void);

public:	
	
	// ------------------
	// DEDICATION SERVER
	// ------------------
	UINT8			m_ui8NationalCode;				// National (country) code
	BOOL			m_bHackCheck;					// Hack Check Active;
	UINT8			m_ui8MultiWeaponCount;			// 다중무기슬롯 최대개수(1~10)
	// Physics
	BOOL			m_bPhysicsActive;				// is activate 
	UINT32			m_ui32PhysicsUpdateTime;		// update time (ms)
	UINT32			m_ui32PhysicsEpsilonTime;		// equivalant to zero (ms)
	UINT32			m_ui32RayCount;					// ray count
	BOOL			m_bBothDirection;				// enable both direction
	
	// Server Info
	UINT32			m_ui32UdpIp;					// UdpIp 클라이언트가 붙을 Ip
	UINT32			m_ui32UdpActiveIp;				// Proxy 존재시 실제로 클라이언트가 붙을 Ip
	UINT32			m_ui32UdpPortCount;				// # of udp ports
	UINT16*			m_pui16UdpPort;					// udp port lists
	DWORD			m_dwUpdateTime;					// task thread update time
	
	// Thread Config
	UINT32			m_i32TaskThreadCount;			// # of Task Processors
	UINT32			m_ui32IoThreadCount;			// # of IO Threads (1:1)

	// Log
	char			m_strLogFilePath[MAX_PATH];		// Log 파일 패스 
	UINT32			m_ui32LogLevel;					// Log Level (MSG, WARNING, ERROR)

	//  컨트롤 서버 
	BOOL			m_bControlActive;
	UINT32			m_ui32ControlServerIP;
	UINT16			m_ui16ControlServerPort;

	UINT32			m_ui32CastServerIP;
	UINT16			m_ui16CastServerPort;

	// Hack Check Level ((level:0~10) = (hack check ratio :0% ~ 100%))
	UINT8			m_aui8HackCheckLevel[ HACK_TYPE_MAX ];
	
	// Group
	INT32			m_i32RoomCount;

	char			m_strServerStartTime[ SMALLDATE_STRING_SIZE ];

	SS_VERSION		m_ControlVersion;
	SS_VERSION		m_Version;

	PB_VER			m_eServerVersion;

	// Map Load Check
	BOOL			m_bMapDataReceived;
	MAP_STAGE_DATA	m_aMapLoad[STAGE_UID_MAX];

public:
	INT32			LoadLogInfo(i3IniParser* pIniParser);
	BOOL			LoadConfigFile(const char * pstrFileName);

	void			SetServerStartTime()								{ i3ThreadTimer::LocalTime().GetSmallDate( m_strServerStartTime, SMALLDATE_STRING_SIZE ); }
	char*			GetServerStartTime()								{ return m_strServerStartTime; }

	UINT8			RegistPBVersion(UINT8 ui8UserPBVer);
};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__
