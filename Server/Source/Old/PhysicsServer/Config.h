#ifndef _CONFIG_H__
#define _CONFIG_H__

#define IP_NAME_COUNT		32

class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );

protected :
	i3Timer			m_Timer;							// dlg version에서 사용하는 타이머
	REAL32			m_LocalTime; 
	UINT32			m_iServerTime; 
	SYSTEMTIME		m_SystemTime;	

public:
	CConfig(void);
	~CConfig(void);

public:	
	// ------------------
	// PHYSICS SERVER
	// ------------------
	
	// number of raycast 
	UINT32			m_ui32RayCount;						// 1, 3, 5
	// Server Info
	UINT32			m_ui32UdpIp;						// UdpIp 클라이언트가 붙을 Ip
	UINT16			m_ui16UdpPort;						// udp port 
	
	// Dedicated Server List
	UINT32			m_ui32BattleServerCount;			// dedicated server's ip list
	UINT32			m_ui32BattleServerIpList[MAX_SERVER_COUNT];
	
	// Thread Config
	INT32			m_i32TaskThreadCount;				// # of Task Processors

	// Timing
	UINT32			m_ui32UpdateTime;					// task thread update 시간
	UINT32			m_ui32EpsilonTime;					// 미소 시간 (이 시간보다 작으면 0으로 간주)

	// Log
	char			m_szLogFilePath[MAX_PATH];			//Log 파일 패스 
	UINT32			m_ui32LogLevel;
	
	//Time
	UINT32			m_Date; 
	INT32			m_Day; 
	INT32			m_Month; 
	INT32			m_Year; 
	INT32			m_Hour; 
	INT32			m_Minute; 
	INT32			m_Second;

public:
	BOOL			LoadFromFile(const char * szFileName);		
	void			RenewTime(void)				
	{ 
		//Cal Server Time 
		REAL32 rDeltaTime = m_Timer.GetDeltaSec(); 
		m_LocalTime = m_LocalTime + rDeltaTime;		
		if( m_LocalTime > 1.0f)
		{
			//정수부 더하기 
			UINT32 iTime	= (UINT32)m_LocalTime; 
			m_iServerTime	+= iTime;

			//정수부 삭제 
			REAL32	rTime	= (REAL32)iTime; 
			m_LocalTime		-= rTime;
		}
        
		//Cal Local Time
		::GetLocalTime( &m_SystemTime );	
		m_Month		=	m_SystemTime.wMonth; 
		m_Day		=	m_SystemTime.wDay; 	
		m_Hour		=	m_SystemTime.wHour; 
		m_Minute	=	m_SystemTime.wMinute; 
		m_Year		=	m_SystemTime.wYear;
		m_Second	=	m_SystemTime.wSecond;
	}

	UINT32	GetServerTime(void)			{ return m_iServerTime;	}
	BOOL	IsValidBattleServer( UINT32 ui32IP );
};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__
