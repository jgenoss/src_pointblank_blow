#ifndef __CONFIG_H__
#define __CONFIG_H__

#define IP_NAME_COUNT		32

class CConfig :	public i3Thread
{
	I3_CLASS_DEFINE( CConfig );

	BOOL					m_bRunning;
	INT32					m_i32LogThreadIdx;

	HANDLE					m_hChangeEvnet;

	char					m_strReFileName[ SERVER_STRING_COUNT ];
	char					m_strReDirName[ SERVER_STRING_COUNT ];

public:	
	//ServerInfo
	char					m_strIP[ IP_NAME_COUNT ];
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	char					m_strLogFilePath[ SERVER_STRING_COUNT ];			//Log 파일 패스 

	LINK_METHOD				m_eLinkMethod;

	// Control Server
	UINT32					m_ui32ControlServerIP;
	UINT16					m_ui16ControlServerPort;

	//Server List
	INT32					m_i32ServerCount;
	UINT32*					m_pui32ServerIp;

	//Update Delay Time
	INT32					m_i32UpdateMain; 

public:

	

	//// ZLog
	//BOOL			m_bLogDActive;							//엔씨 로그D 모듈연결
	//char			m_ui32ZLogServerIP;						//엔씨 로그D 서버 아이피 
	//UINT16			m_ui16ZLogServerPort;					//엔씨 로그D 서버 포트 

public:
	CConfig(void);
	~CConfig(void);

	BOOL					OnCreate( char* strFileName, char* strReLoadFileName );
	BOOL					OnCreateThread();

	void					OnDestroy();
	virtual UINT32			OnRunning( void* pUserData );

	BOOL					_LoadFromFile( const char* strFileName );
	BOOL					_LoadReFromFile( const char* strFileName );

	void					_CorrectPath( char* strIn, INT32 i32Size, char* strOut );
};

extern CConfig * g_pConfig;

#endif //_CONFIG_H__