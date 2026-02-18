#ifndef __MODULE_LOGFILE_H__
#define __MODULE_LOGFILE_H__

#define LOG_BUFFER_MAX_COUNTER		100
#define MAX_LOG_PATH				1024
#define LOG_INIT_STRING				10

struct LOG_FILE_BUFFER
{
	char	_szMessage[MAX_LOG_PATH];

	// SWS
	INT32	_i32WarningLevel;
};

class CModuleLogFile
{
private:
	//For Thread 
	HANDLE					m_hThread;
	BOOL					m_bIsRunning;

	INT32					m_i32LogIdx;

	//For File
	HANDLE					m_hFile;
	UINT32					m_ui32FileHour; 

	i3Mutex					m_pAddCs;
	//For Buffer 
	INT32					m_ui32WorkCounter;
	i3RingBuffer*			m_ppLogBuffer[ LOG_BUFFER_MAX_COUNTER	];
	TCHAR					m_astrLogInit[ LOG_BUFFER_MAX_COUNTER ][ LOG_INIT_STRING ]; 

	i3RingBuffer*			m_pLogBuffer_Main;
	i3RingBuffer*			m_pLogBuffer_Task;
	i3RingBuffer*			m_pLogBuffer_Network;
	i3RingBuffer*			m_pLogBuffer_WebShop; 
	i3RingBuffer**			m_ppLogBuffer_Link;
	i3RingBuffer*			m_pLogBuffer_Control; 
	i3RingBuffer*			m_pLogBuffer_WebGift; 

	i3RingBuffer*			m_pLogBuffer_DataBase_Main;
	i3RingBuffer**			m_ppLogBuffer_DataBase;
	i3RingBuffer**			m_ppLogBuffer_Award;

	// Warning
	UINT32					m_ui32WriteTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];
	UINT32					m_ui32ReadTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];

protected:
	UINT32					m_ui32DateTime; 
	WARNING_LEVEL			m_aWarningInfo[ WARNING_LEVEL_COUNT ];

protected:
	bool _CreateFile(void); 

	void _WriteLogFile(char * pstrLog)
	{
		INT32 i32Len = i3String::Length(pstrLog); 
		::WriteFile( m_hFile, pstrLog, i32Len, (LPDWORD)&i32Len, NULL ); 
	}

public:

	CModuleLogFile(void); 
	virtual ~CModuleLogFile(void); 
	bool 					OnCreate(void);
	bool 					OnDestroy(void);
	void 					OnWorking(void); 

	UINT32					GetFileHour(void)	{ return m_ui32FileHour; }
	INT32					AddLogThread( TCHAR* strPreString );

	void					Write_LogFile( INT32 i32WorkIdx, INT32 i32WarningLevel, TCHAR * pstrLogString, ... );
	void					Write_WarningLogFile( INT32 i32WorkIdx, INT32 i32WarningType, INT32 i32WarningLevel, TCHAR * pstrLogString, ... ); // 위에 함수에 인자를 추가해서 사용할수도 있었지만 이미 사용되는 함수라 따로 분리시켜서 만들었습니다.

	UINT32	GetWarningInfo( INT32 i32WarningType );
	BOOL	SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel );
	UINT32	GetFlagWarningLevel( INT32 i32WarningLevel );
};

extern CModuleLogFile * g_pModuleLogFile; 

#endif
