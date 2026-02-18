#ifndef _CONFIG_H__
#define _CONFIG_H__

#define	SERVER_NAME_COUNT	32
#define IP_NAME_COUNT		32

class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig );
public:
	UINT32 			GetConnectedGameServerCount(void) const;
	UINT32 			GetConnectedUserCount(void) const;
	
	void 			AddConnectedGameServer(void);
	void 			SubConnectedGameServer(void);
	void 			SetConnectedUserCount(UINT32 count);

private:
	UINT32			m_connectedGameServerCount;
	UINT32			m_connectedUserCount;

protected :
	i3Timer			m_Timer; 
	REAL32			m_LocalTime; 
	UINT32			m_iServerTime; 
	SYSTEMTIME		m_SystemTime;	


public:
	INT32			m_MacAddressListCount; 
	UINT64	*		m_pMacAddressList; 

	CConfig(void);
	~CConfig(void);

public:	
	//ServerInfo
	UINT32			m_UdpIp;
	UINT32			m_UdpActiveIp;
	UINT8			m_cUdpPortCount;
	UINT16*			m_pUdpPort;

	char			m_LogFilePath[256];					//Log 파일 패스 
	UINT16			m_ncGameId;

	//NC 로그D
	BOOL			m_bLogDActive;						//엔씨 로그D 모듈연결
	char			m_LogDServerIp[IP_NAME_COUNT];		//엔씨 로그D 서버 아이피 
	UINT16			m_LogDServerPort;					//엔씨 로그D 서버 포트 

	//관제툴 서버 
	BOOL			m_bControlActive;
	char			m_ControlServerIp[IP_NAME_COUNT];		
	UINT16			m_ControlServerPort;

	//Server List
	UINT32			m_ServerCount;
	UINT32	*		m_pServerIp; 

	//Update 
	INT32			m_UpdateLogFile; 

	UINT32			m_wGroupCount;

	//Time
	UINT32			m_Date; 
	INT32			m_Day; 
	INT32			m_Month; 
	INT32			m_Year; 
	INT32			m_Hour; 
	INT32			m_Minute; 
	INT32			m_Second;

	INT32			m_iCpuQuery; 
	INT32			m_iHandleQuery; 

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
			m_iServerTime	= m_iServerTime + iTime;

			//정수부 삭제 
			REAL32	rTime	= (REAL32)iTime; 
			m_LocalTime		= m_LocalTime - rTime;
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
};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__
