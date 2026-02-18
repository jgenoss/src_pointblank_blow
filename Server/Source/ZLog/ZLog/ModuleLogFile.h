#ifndef __MODULE_LOGFILE_H__
#define __MODULE_LOGFILE_H__

struct LOG_FILE_BUFFER
{
	char	_szMessage[1024];
};

class CModuleLogFile
{
private:
	//For Thread 
	HANDLE				m_hThread;
	bool				m_bIsRunning;

	//For File
	HANDLE				m_hFile;
	UINT32				m_ui32FileHour; 

	//For Buffer 
	i3RingBuffer	*	m_pMain;
	i3RingBuffer	*	m_pControl;
	i3RingBuffer	*	m_pFileManager;
	i3RingBuffer	*	m_pNetwork;
	INT32				m_i32DBUploader;
	i3RingBuffer **		m_ppDBUploader;
	INT32				m_i32NetServer;
	i3RingBuffer	**	m_ppNetServer;

	// Warning
	UINT32				m_ui32WriteTime[SIA_WARNING_TYPE_COUNT][SIA_WARNING_LEVEL_COUNT];
	UINT32				m_ui32ReadTime[SIA_WARNING_TYPE_COUNT][SIA_WARNING_LEVEL_COUNT];

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

	bool					OnCreate(void);
	bool					OnCreate2th(void);
	void					OnDestroy(void);
	void					OnWorking(void); 
	void					Write_LogFile(INT32 i32ThreadIdx, char * pstrLogString, INT32 i32WarningType = -1, INT32 i32WarningLevel = -1);
	HANDLE					GetFileHandle() { return m_hFile; }

	UINT32					GetFileHour(void)	{ return m_ui32FileHour; }



	UINT32	GetWarningInfo( INT32 i32WarningType );
	BOOL	SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel );
	UINT32	GetFlagWarningLevel( INT32 i32WarningLevel );

};

extern CModuleLogFile * g_pModuleLogFile; 


#define WRITE_LOG_MAIN(b)							g_pModuleLogFile->Write_LogFile(THREAD_MAIN_IDX, b)
#define WRITE_LOG_CONTROL(b)						g_pModuleLogFile->Write_LogFile(THREAD_CONTROL_IDX, b)
#define WRITE_LOG_FILE_MANAGER(b)					g_pModuleLogFile->Write_LogFile(THREAD_FILE_MANAGER_IDX, b)
#define WRITE_LOG_NETWORK(b)						g_pModuleLogFile->Write_LogFile(THREAD_NETWORK_IDX, b)
#define WRITE_LOG_NET_SERVER(a,b)					g_pModuleLogFile->Write_LogFile(THREAD_NET_SERVER_IDX+a, b)
#define WRITE_LOG_DB_UPLOADER(a,b)					g_pModuleLogFile->Write_LogFile(THREAD_DB_UPLOADER_IDX+a, b)

#define WRITE_WARNING_LOG_MAIN(b,c,d)				g_pModuleLogFile->Write_LogFile(THREAD_MAIN_IDX, b, c, d)
#define WRITE_WARNING_LOG_CONTROL(b,c,d)			g_pModuleLogFile->Write_LogFile(THREAD_CONTROL_IDX, b, c, d)
#define WRITE_WARNING_LOG_FILE_MANAGER(b,c,d)		g_pModuleLogFile->Write_LogFile(THREAD_FILE_MANAGER_IDX, b)
#define WRITE_WARNING_LOG_NETWORK(b,c,d)			g_pModuleLogFile->Write_LogFile(THREAD_NETWORK_IDX, b, c, d)
#define WRITE_WARNING_LOG_NET_SERVER(a,b,c,d)		g_pModuleLogFile->Write_LogFile(THREAD_NET_SERVER_IDX+a, b, c, d)
#define WRITE_WARNING_LOG_DB_UPLOADER(a,b,c,d)		g_pModuleLogFile->Write_LogFile(THREAD_DB_UPLOADER_IDX+a, b, c, d)

#endif
