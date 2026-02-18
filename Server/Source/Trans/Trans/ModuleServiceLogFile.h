#ifndef __MODULE_SERVICE_LOG_H__
#define __MODULE_SERVICE_LOG_H__

#define SERVICE_LOG_FILE_BUFFER_MAX		200
#define SERVICE_LOG_MESSAGE_SIZE		1024

struct SERVICE_LOG_FILE_BUFFER
{		
	char	m_strMessage[SERVICE_LOG_MESSAGE_SIZE];
};

enum SERVICE_LOG_TYPE
{
	SERVICE_LOG_TYPE_DB = 0,
	SERVICE_LOG_TYPE_BUY,
	SERVICE_LOG_TYPE_GIFT,
	SERVICE_LOG_TYPE_REPAIR,
	SERVICE_LOG_TYPE_MAX,
};

class CModuleServiceLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CModuleServiceLogFile );
private:
	BOOL					m_bIsRunning;

	//For File 
	HANDLE					m_hFileDB;
	HANDLE					m_hFileBuy;
	HANDLE					m_hFileGift;
	HANDLE					m_hFileRepair; 

	UINT32					m_ui32DateHour;

	INT32					m_i32LogIdx;

	// DB
	INT32						m_i32ServiceLog_DB_ThreadCount;
	SERVICE_LOG_FILE_BUFFER	**	m_ppServiceLog_DB_FileBuffer;
	INT32*						m_pi32ServiceLog_DB_WritePos;
	INT32*						m_pi32ServiceLog_DB_ReadPos;

	// Buy
	SERVICE_LOG_FILE_BUFFER	*	m_pServiceLog_Buy_FileBuffer;
	INT32						m_i32ServiceLog_Buy_WritePos;
	INT32						m_i32ServiceLog_Buy_ReadPos;

	// Gift
	SERVICE_LOG_FILE_BUFFER	*	m_pServiceLog_Gift_FileBuffer;
	INT32						m_i32ServiceLog_Gift_WritePos;
	INT32						m_i32ServiceLog_Gift_ReadPos;

	// Repair
	SERVICE_LOG_FILE_BUFFER	*	m_pServiceLog_Repair_FileBuffer;
	INT32						m_i32ServiceLog_Repair_WritePos;
	INT32						m_i32ServiceLog_Repair_ReadPos;

protected:
	UINT32						m_ui32DateTime;

protected:
	BOOL _CreateFile(SERVICE_LOG_TYPE eType); 
	void _WriteLogDBFile(SERVICE_LOG_TYPE eType, char * pstrLog);

public:
	CModuleServiceLogFile(void); 
	virtual ~CModuleServiceLogFile(void); 

	BOOL			OnCreate();
	BOOL			OnCreateGame( INT32 i32ThreadCount );
	void			OnDestroy(void);
	virtual UINT32	OnRunning( void * pUserData);

	void			WriteLog(SERVICE_LOG_TYPE eLogType, INT32 i32ThreadIdx, char * pstrLogString);

	void			_WriteLogFile(SERVICE_LOG_TYPE eType, char * pstrLog);

	UINT32			GetFileHour(void) { return m_ui32DateHour; }
};

extern CModuleServiceLogFile * g_pModuleServiceLogFile; 

// *** 주의 *** : ModuleDBShop 쓰레드 고정입니다. 다른 쓰레드에서는 호출 금지.
#define WRITE_SERVICE_LOG_DB(DBThreadIdx, b)	g_pModuleServiceLogFile->WriteLog(SERVICE_LOG_TYPE_DB,		DBThreadIdx + THREAD_S_DATABASE_IDX, b)

// *** 주의 *** : TaskProcessor 쓰레드 고정입니다. 다른 쓰레드에서는 호출 금지.
#define WRITE_SERVICE_LOG_BUY_TASK(b)			g_pModuleServiceLogFile->WriteLog(SERVICE_LOG_TYPE_BUY,		MODULE_TASK_THREAD_IDX, b)
#define WRITE_SERVICE_LOG_GIFT_TASK(b)			g_pModuleServiceLogFile->WriteLog(SERVICE_LOG_TYPE_GIFT,	MODULE_TASK_THREAD_IDX, b)
#define WRITE_SERVICE_LOG_REPAIR_TASK(b)		g_pModuleServiceLogFile->WriteLog(SERVICE_LOG_TYPE_REPAIR,	MODULE_TASK_THREAD_IDX, b)


#endif
