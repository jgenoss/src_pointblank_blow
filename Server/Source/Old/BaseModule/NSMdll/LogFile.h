#ifndef __LOGFILE_H_
#define __LOGFILE_H_

class NSM_CRingBuffer; 
class NSM_cCriticalSection; 
#define LOG_BUFFER_MAX_COUNTER		100

#define RV_ADD_FAIL		0x80000001

enum FILE_SAVE_INTERVAL
{
	FILE_SAVE_INTERVAL_DAY =1,
	FILE_SAVE_INTERVAL_HOUR,
};

class EXPORT_BASE CLogFile
{
private:
	//For Thread 
	HANDLE	m_hThread;
	bool				m_bIsRunning;

	//For File
	HANDLE	m_hFile;
	INT32	m_iSaveType; 
	UINT32	m_ui32FileHour; 
	UINT32	m_ui32FileDay; 
	char	*	m_strFileName; 

	//For File Save Buffer 
	NSM_CRingBuffer	*	m_ppLogBuffer[LOG_BUFFER_MAX_COUNTER];
	char			*	m_ppInitString[LOG_BUFFER_MAX_COUNTER]; 
	UINT32				m_iWorkCounter; 
	INT32				m_iBufferCount; 

	//For Add Log Buffer 
	NSM_cCriticalSection * m_pAddCs; 

	//For Save Time
	SYSTEMTIME	m_SystemTime;
	UINT32		m_iMonth;
	UINT32		m_iDay;
	UINT32		m_iHour;
	UINT32		m_iMinute;
	UINT32		m_iSecond;

protected:	

protected:
	BOOL _CreateFile(void); 
	void _UpdateTime(void)
	{
		::GetLocalTime( &m_SystemTime );	
		m_iMonth		=	m_SystemTime.wMonth; 
		m_iDay			=	m_SystemTime.wDay; 	
		m_iHour			=	m_SystemTime.wHour; 
		m_iMinute		=	m_SystemTime.wMinute; 
		m_iSecond		=	m_SystemTime.wSecond;
		return; 
	}
	void _WriteLogFile(char * pLog);

public: 	
	CLogFile(void); 
	virtual ~CLogFile(void); 
	BOOL	OnCreate(char * strFileName, INT32 iSaveType = FILE_SAVE_INTERVAL_HOUR, INT32 iLogBufferCount = 100, INT32 iThreadPri = THREAD_PRIORITY_NORMAL);
	INT32	AddLogThread(char * strPreString, INT32 iType); 
	BOOL	OnDestroy(void);
	void	OnWorking(void); 
	void	Write_LogFile(INT32 iIdx, char * pLogString);
};

#endif
