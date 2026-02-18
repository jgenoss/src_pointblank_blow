#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include "S2CriticalSection.h"

// max
#define DS_LOG_BUFFER_COUNT		2048	// # of circular queue items
#define DS_LOG_STRING_LENGTH	1024	// 최대 문자열 길이

// error level
#define DS_LOG_ERROR_LEVEL_MSG			0
#define DS_LOG_ERROR_LEVEL_WARNING		1
#define DS_LOG_ERROR_LEVEL_ERROR		2

struct DS_LOG_ITEM
{
	SYSTEMTIME	m_time;
	UINT32		m_ui32SerialNo;						// serial no.
	UINT16		m_ui16Level;						// 에러 수준
	UINT16		m_ui16MsgID;						// 메시지 ID (reserved)
	UINT16		m_ui16LangID;						// 언어 ID   (rerserved)
	UINT16		m_ui16MsgLen;						// 문자열 메시지 길이
	char		m_szMsg[ DS_LOG_STRING_LENGTH ];	// 문자열 메시지

	DS_LOG_ITEM()
	{
		memset( &m_time, 0, sizeof(m_time) );
		
		m_ui32SerialNo  = 0;						
		m_ui16Level		= 0;						
		m_ui16MsgID		= 0;						
		m_ui16LangID	= 0;						
		m_ui16MsgLen	= 0;						
		m_szMsg[0]		= 0;						
	}
};

struct DS_LOG_QUEUE
{
	DS_LOG_ITEM	m_logList[ DS_LOG_BUFFER_COUNT ]; 
	
	INT32		m_i32WriteIdx;	
	INT32		m_i32ReadIdx;	
	UINT32		m_ui32Count;	
	
	DS_LOG_QUEUE()
	{
		m_i32WriteIdx	=  0;
		m_i32ReadIdx	=  0;
		m_ui32Count		=  0;
	};
};


class CLogFile : public i3ElementBase
{
	I3_CLASS_DEFINE( CLogFile ); 

public:
	// ----------------------------------------------------------------------
	// WriteLog : wrapper를 이용하여 접근한다
	INT32 WriteLog( UINT32 ui32Level, char* szMsg, UINT32 ui32MsgLen ); // 버퍼에 메시지 write
	LPSTR GetErrorString( DWORD dwErrorCode );							// 시스템 에러 메시지 스트링
																		// 리턴된 문자열은 반드시 LocalFree() 시켜야 한다.
	// ----------------------------------------------------------------------
	// service callback functions
	BOOL OnCreate();							// g_InitServer()
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
	
	S2CriticalSection	m_cs;
};


// 글로벌 변수 선언
extern CLogFile * g_pLogFile;

#define DS_MAKE_VAR_STRING( s, l, f )	{						\
							va_list		argptr;					\
							va_start (argptr, f);				\
							l = vsprintf_s( s, l, f, argptr);	\
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

inline INT32 DSSYSERR( DWORD dwErrorCode )
{
	char	szMsg[DS_LOG_STRING_LENGTH];
	INT32	i32MsgLen	=  0;
	LPSTR	pMessage	= NULL;
	

	if ( g_pLogFile )
	{
		pMessage = g_pLogFile->GetErrorString( dwErrorCode );
		if ( pMessage )
		{
			i32MsgLen = sprintf_s(szMsg, sizeof(szMsg), "[SYSTEM ERROR] Error Code %d: %s\n", dwErrorCode, pMessage);
			i32MsgLen = g_pLogFile->WriteLog( DS_LOG_ERROR_LEVEL_ERROR, szMsg, i32MsgLen );
			LocalFree( pMessage );
		}
	}

	return i32MsgLen;
}

#endif
