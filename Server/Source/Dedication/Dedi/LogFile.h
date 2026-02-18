#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include "S2CriticalSection.h"

#ifdef NEW_LOG

// max
#define DS_LOG_BUFFER_COUNT		500		// # of circular queue items
#define DS_LOG_USER_THREAD_MAX	256		// max thread for logging
#define DS_LOG_STRING_LENGTH	1024	// 최대 문자열 길이

// error level
#define DS_LOG_ERROR_LEVEL_MSG			0
#define DS_LOG_ERROR_LEVEL_WARNING		1
#define DS_LOG_ERROR_LEVEL_ERROR		2

struct DS_LOG_ITEM
{
	UINT16		m_ui16MsgLen;
	UINT8		m_ui8Level;							// error level { DS_LOG_ERROR_LEVEL_MSG | DS_LOG_ERROR_LEVEL_WARNING | DS_LOG_ERROR_LEVEL_ERROR }
	UINT8		m_ui8Minute;
	UINT8		m_ui8Second;
	char		m_strMsg[ DS_LOG_STRING_LENGTH ];

	DS_LOG_ITEM()
	{
		Reset();
	}

	void Reset()
	{
		m_ui16MsgLen	= 0;
		m_ui8Level		= DS_LOG_ERROR_LEVEL_MSG;
		m_ui8Minute		= 0;
		m_ui8Second		= 0;
		m_strMsg[0]		= '\0';
	}
};

struct DS_LOG_MESSAGE_BUFFER	// per thread
{
	DS_LOG_ITEM	m_alogList[ DS_LOG_BUFFER_COUNT ];
	
	INT32		m_i32WriteIdx;
	INT32		m_i32ReadIdx;
	DWORD		m_dwThreadID;	// windows thread id
	
	DS_LOG_MESSAGE_BUFFER()
	{
		m_i32WriteIdx	=  0;
		m_i32ReadIdx	=  0;
		m_dwThreadID	= (DWORD)-1;
	};
};

enum LOG_TYPE{LOG =0,HACKLOG};

class CLogFile
{
public:
	//Interface
	DS_LOG_MESSAGE_BUFFER*	AllocBuffer( DWORD dwThreadID );			// 스레드 생성시 호출, 생성된 스레드는 이버퍼를 사용해야 한다.
	BOOL WriteLog( UINT32 ui32Level, char* szMsg, UINT32 ui32MsgLen );	// 로컬 버퍼에 메시지 write
	
	//Callback functions
	BOOL OnCreate(LOG_TYPE eType);							// g_InitServer()
	void OnUpdate(void);						// g_UpdateServer()
	void OnDestroy(void);						// g_DestroyServer()

	CLogFile(void); 
	virtual ~CLogFile(void); 

private:
	BOOL FileOpen();							
	void Flush();								// write contents into the disk
	DS_LOG_MESSAGE_BUFFER*	FindLocalBuffer();

	//Buffer
	DS_LOG_MESSAGE_BUFFER**	m_ppBuffers;		// pointer array to the thread buffers
	UINT32					m_ui32BufferCount;	// buffer count = total user thread 
	
	//Thread
	HANDLE				m_hFileThread;			// handle to logFile Thread
	
	//File
	HANDLE				m_hFile;				// handle to logFile
	INT32				m_i32Hour;				// g_pConfig->m_Day에서 읽음 (날짜 변경 인식하기 위함)
	
	S2CriticalSection	m_cs;					// initialize 때만 사용.

	UINT8				m_ui8LogType;
};


// 글로벌 변수 선언
extern CLogFile * g_pLogFile;
extern CLogFile * g_pHackLogFile;

#define DS_MAKE_VAR_STRING( s, l, f )	{						\
							va_list		argptr;					\
							va_start (argptr, f);				\
							l = vsprintf_s( s, l, f, argptr);	\
							if( l >= sizeof(s) ) {				\
								s[sizeof(s) - 1] = '\0';		\
								l = sizeof(s) - 1; }			\
							va_end(argptr);	 }

// inline functions
inline INT32 DSMSG( const char *fmt, ... )	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);

	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_MSG, szMsg, i32MsgLen);		
	}
	return 0;
}

inline INT32 DSWARNING( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_WARNING, szMsg, i32MsgLen);		
	}
	return 0;
}

inline INT32 DSERROR ( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);		
	}
	return 0;
}


inline INT32 DSHACK ( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);

	if ( g_pHackLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pHackLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);		
	}
	return 0;
}


inline INT32 DSMUST ( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);		
	}
	return 0;
}

#else

// max
#define DS_LOG_BUFFER_COUNT		2048	// # of circular queue items
#define DS_LOG_STRING_LENGTH	1024	// 최대 문자열 길이

// error level
#define DS_LOG_ERROR_LEVEL_MSG			0
#define DS_LOG_ERROR_LEVEL_WARNING		1
#define DS_LOG_ERROR_LEVEL_ERROR		2

enum LOG_TYPE{LOG =0,HACKLOG};

struct DS_LOG_ITEM
{
	SYSTEMTIME	m_time;
	UINT32		m_ui32SerialNo;						// serial no.
	UINT16		m_ui16Level;						// 에러 수준
	UINT16		m_ui16MsgLen;						// 문자열 메시지 길이
	char		m_szMsg[ DS_LOG_STRING_LENGTH ];	// 문자열 메시지

	DS_LOG_ITEM()
	{
		i3mem::FillZero( &m_time, sizeof(m_time) );
		
		m_ui32SerialNo  = 0;
		m_ui16Level		= 0;
		m_ui16MsgLen	= 0;
		m_szMsg[0]		= 0;
	}
};

struct DS_LOG_QUEUE
{
	DS_LOG_ITEM	m_logList[ DS_LOG_BUFFER_COUNT ]; 
	
	INT32		m_i32WriteIdx;	
	INT32		m_i32ReadIdx;	
	INT32		m_i32Count;	
	
	DS_LOG_QUEUE()
	{
		m_i32WriteIdx	=  0;
		m_i32ReadIdx	=  0;
		m_i32Count		=  0;
	};
};


class CLogFile// : public i3ElementBase
{
	//I3_EXPORT_CLASS_DEFINE( CLogFile, i3ElementBase ); 

public:
	// ----------------------------------------------------------------------
	// WriteLog : wrapper를 이용하여 접근한다
	INT32 WriteLog( UINT32 ui32Level, char* szMsg, UINT32 ui32MsgLen ); // 버퍼에 메시지 write
	LPSTR GetErrorString( DWORD dwErrorCode );							// 시스템 에러 메시지 스트링
																		// 리턴된 문자열은 반드시 LocalFree() 시켜야 한다.
	// ----------------------------------------------------------------------
	HANDLE				m_hFileThread;			// handle to logFile Thread
	BOOL				m_bIsRunning;
	

	// service callback functions
	BOOL OnCreate( LOG_TYPE eType );							// g_InitServer()
	BOOL OnServerCreate(UINT32 i32LogCount );	// g_StartServer()
	void OnUpdate(void);						// g_UpdateServer()
	void OnDestroy(void);						// g_DestroyServer()

	CLogFile(void); 
	virtual ~CLogFile(void); 

private:
	BOOL FileOpen();							// 날짜에 가반하여 파일명을 생성하고 Open
	void Flush();								// 버퍼에 있는 내용 전부를 강제로 파일에 기록한다

	DS_LOG_QUEUE*		m_pBuffer;				// handle to queue
	HANDLE				m_hFile;				// handle to logFile
	INT32				m_i32Hour;				// g_pConfig->m_Day에서 읽음 (날짜 변경 인식하기 위함)
	UINT32				m_ui32SerialNo;		
	UINT8				m_ui8LogType;	

	INT32				m_i32Average_Min;		// 분 카운트
	INT32				m_i32AverageCCU_Min;	// 1시간동안 평균 유저 카운트. 1분마다.
	INT32				m_i32AverageRoom_Min;	// 1시간동안 평균 룸 카운트. 1분마다.
	
	S2CriticalSection	m_cs;
};


// 글로벌 변수 선언
extern CLogFile * g_pLogFile;
extern CLogFile * g_pHackLogFile;

#define DS_MAKE_VAR_STRING( s, l, f )	{						\
							va_list		argptr;					\
							va_start (argptr, f);				\
							l = vsprintf_s( s, l, f, argptr);	\
							if( l >= sizeof(s) ) {				\
								s[sizeof(s) - 1] = '\0';		\
								l = sizeof(s) - 1; }			\
							va_end(argptr);	 }

// inline functions
inline INT32 DSMSG( const char *fmt, ... )	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);

	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_MSG, szMsg, i32MsgLen);
	}
	return 0;
}

inline INT32 DSWARNING( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_WARNING, szMsg, i32MsgLen);
	}
	return 0;
}

inline INT32 DSERROR ( const char *fmt, ...)
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);
	}
	return 0;
}


inline INT32 DSHACK ( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);

	if ( g_pHackLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pHackLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);		
	}
	return 0;
}


inline INT32 DSMUST ( const char *fmt, ...)	
{	
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen = sizeof(szMsg);
	
	if ( g_pLogFile )
	{
		DS_MAKE_VAR_STRING( szMsg, i32MsgLen, fmt );
		return g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen);		
	}
	return 0;
}
#endif // NEW_LOG
#endif
