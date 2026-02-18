#ifndef __MODULE_LOGFILE_H__
#define __MODULE_LOGFILE_H__

#define LOG_THREAD_INVALID		-1
#define LOG_STRING_BASIC		500
#define LOG_STRING_MAX			512
#define	LOG_FILE_BUFFER_COUNT	100

typedef struct _tagLogFileBuffer
{		
	INT32	_i32ReadPos;
	INT32	_i32WritePos;
	char	_szMessage[ LOG_FILE_BUFFER_COUNT ][ LOG_STRING_MAX ];
}LOG_FILE_BUFFER;

class CModuleLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CModuleLogFile );

	BOOL					m_bIsRunning;

	//For File 
	HANDLE					m_hFile;
	char					m_strLogFilePath[ MAX_PATH ];
	INT32					m_i32DateHour;

	INT32					m_i32LogBufferCount;
	i3List*					m_pLogList;
	
	INT32					m_i32LogThreadIdx;

protected:
	BOOL					_CreateFile(void);
	void					_WriteLogFile( char * strLog );
	
public: 	
	CModuleLogFile(void); 
	virtual ~CModuleLogFile(void); 

	BOOL					OnCreate( char* strLogFilePath );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData );

	INT32					InsertLogBuffer();

	void					InsertBuffer( INT32 i32ThreadIdx, char* strLog );	
};

extern CModuleLogFile * g_pLog;

extern void WriteLog( INT32 i32ThreadIdx, const char * strLog, ... );
#endif
