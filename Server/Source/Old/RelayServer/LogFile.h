#ifndef __LOGFILE_H__
#define __LOGFILE_H__

class CGsSession; 

#define LOGBUFFERCOUNT		1000
#define LOGTHREADIDX_MAIN	-200
#define LOGTHREADIDX_CON	-100
#define LOGTHREADIDX_RELAY	0

typedef struct _tagLogBuffer
{
	char		_szLog[ LOGBUFFERCOUNT ][1024];
	INT32		_i32WritePos;
	INT32		_i32ReadPos;
	_tagLogBuffer::_tagLogBuffer()
	{
		i3mem::FillZero( _szLog, LOGBUFFERCOUNT*1024 );
		_i32WritePos	= 0;
		_i32ReadPos		= 0;
	};
}ST_LOGBUFFER;

class CLogFile : public i3ElementBase
{
	I3_CLASS_DEFINE( CLogFile ); 
private:
	ST_LOGBUFFER*		m_pLogBufferMain;
	ST_LOGBUFFER*		m_pLogBufferControl;
	ST_LOGBUFFER**		m_ppLogBufferRelay;

	UINT32				m_ui32RelayCount;
	HANDLE				m_hFile;
	INT32				m_i32Day; 

protected:
	BOOL FileOpen();
public: 
	CLogFile(void); 
	virtual ~CLogFile(void); 

	BOOL OnCreate( ); 
	BOOL OnServerCreate( UINT32 i32LogCount );
	void OnUpdate(void);			
	void OnDestroy(void); 
	void WriteLog( INT32 iThreadIdx, char* pLogString);
};

extern CLogFile * g_pLogFile;

//#define WRITE_LOG(a, b)			g_pLogFile->WriteLog(a, b)
#define WRITE_LOG_MAIN(a)		if( g_pLogFile )	g_pLogFile->WriteLog(LOGTHREADIDX_MAIN, a)
#define WRITE_LOG_CONTROL(a)	if( g_pLogFile )	g_pLogFile->WriteLog(LOGTHREADIDX_CON, a)
#define WRITE_LOG_RELAY(a, b)	if( g_pLogFile )	g_pLogFile->WriteLog(LOGTHREADIDX_RELAY+a, b)

#endif
