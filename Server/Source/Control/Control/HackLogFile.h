#ifndef __HACK_LOGFILE_H__
#define __HACK_LOGFILE_H__

class CSession; 

#define HACK_STRING_SIZE	128

struct HACK_LOG_FILE_MESSAGE
{	
	INT32	i32Length;
	char	strLog[HACK_STRING_SIZE];
}; 

class CHackLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CHackLogFile );
private:
	i3Mutex				m_csWrite;
	HANDLE				m_hFile;
	INT32				m_i32Hour;
	INT32				m_i32Day;


	BOOL				m_bIsRunning;

	UINT32				m_ui32WorkingCounter;
	Local_CRingBuffer *	m_pWorkingBuffer[500];//Thread 가 10개 이상이면 더 만들어야 합니다. 
	Local_CRingBuffer *	m_pMainBuffer;

protected:
	BOOL FileOpen(void);

public: 
	CHackLogFile(void);
	virtual ~CHackLogFile(void);

	BOOL				OnCreate(void);
	void				OnUpdate(void);
	void				OnDestroy(void);

	virtual UINT32		OnRunning( void * pUserData ); 	
	void				WriteLog( INT64 i64Uid, UINT32 ui32Type);
	void				AddRingBuffer(Local_CRingBuffer * pRingBuffer)
	{
		m_pWorkingBuffer[m_ui32WorkingCounter] = pRingBuffer;
		m_ui32WorkingCounter++;
	}
};

extern CHackLogFile * g_pHackLogFile;

#endif
