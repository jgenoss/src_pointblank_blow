#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#define LOG_FILE_NAME		128
#define LOG_STRING_LEN		512

struct LOG_FILE_MESSAGE
{	
	INT32	i32Length; 
	char	strLog[LOG_STRING_LEN];

	// SWS
	INT32	_i32WarningLevel;
}; 

class CLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CLogFile ); 
private:
	BOOL					m_bRunning;
	HANDLE					m_hFile;
	HANDLE					m_hChangeNickFile;
	HANDLE					m_hChangeRankFile;
	INT32					m_i32Hour;

	UINT32					m_ui32WorkingCounter;
	Local_CRingBuffer *		m_pWorkingBuffer[500];				//Thread 가 500개 이상이면 더 만들어야 합니다. 

	UINT32					m_ui32ChangeNickWorkingCounter;
	Local_CRingBuffer *		m_pChangeNickWorkingBuffer[200];	//Thread 가 200개 이상이면 더 만들어야 합니다. 
	UINT32					m_ui32ChangeRankWorkingCounter;
	Local_CRingBuffer *		m_pChangeRankWorkingBuffer[200];	//Thread 가 200개 이상이면 더 만들어야 합니다. 


	Local_CRingBuffer **	m_ppReceiverBuffer;
	Local_CRingBuffer **	m_ppDataBaseBuffer;
	Local_CRingBuffer *		m_pMainBuffer;
	Local_CRingBuffer *		m_pTaskBuffer;
	Local_CRingBuffer *		m_pGateWay; 
	Local_CRingBuffer *		m_pLogDBuffer; 
	Local_CRingBuffer *		m_pControlBuffer;

	// Change Nick, Rank ErrorLog
	Local_CRingBuffer *		m_pChangeNickBuffer;
	Local_CRingBuffer *		m_pChangeRankBuffer;

	Local_CRingBuffer **	m_ppChangeNickErrorDBBuffer;
	Local_CRingBuffer **	m_ppChangeRankErrorDBBuffer;

protected:
	BOOL FileOpen(void);
	BOOL ChangeNickErrorLogFileOpen();
	BOOL ChangeRankErrorLogFileOpen();
	BOOL ChangeNickErrorLogFileClose();
	BOOL ChangeRankErrorLogFileClose();

	void AddRingBuffer(Local_CRingBuffer * pRingBuffer)
	{
		m_pWorkingBuffer[m_ui32WorkingCounter] = pRingBuffer;
		m_ui32WorkingCounter++; 
	}

	void AddChangeNickRingBuffer(Local_CRingBuffer * pRingBuffer)
	{
		m_pChangeNickWorkingBuffer[m_ui32ChangeNickWorkingCounter] = pRingBuffer;
		m_ui32ChangeNickWorkingCounter++; 
	}

	void AddChangeRankRingBuffer(Local_CRingBuffer * pRingBuffer)
	{
		m_pChangeRankWorkingBuffer[m_ui32ChangeRankWorkingCounter] = pRingBuffer;
		m_ui32ChangeRankWorkingCounter++; 
	}

public: 
	CLogFile(void); 
	virtual ~CLogFile(void); 

	BOOL OnCreate(void); 
	BOOL OnCreateGame(void);
	void OnDestroy(void); 
	virtual UINT32	OnRunning( void * pUserData);

	//SWS
	void		WriteLog(INT32 i32idx, char* pstrLogString, INT32 i32WarningLevel);
	void		WriteWarningLevel( INT32 i32WarningLevel );

	void		Write_Log_Main(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_MAIN, pstrLogString, i32WarningLevel); }
	void		Write_Log_Task(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_TASK, pstrLogString, i32WarningLevel); }
	void		Write_Log_GateWay(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_GATEWAY, pstrLogString, i32WarningLevel); }
	void		Write_Log_LogD(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_LOGD, pstrLogString, i32WarningLevel); }
	void		Write_Log_Control(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_CONTROL, pstrLogString, i32WarningLevel); }
	void		Write_Log_Nick(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_CHANGE_NICK, pstrLogString, i32WarningLevel); }
	void		Write_Log_Rank(char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ){ WriteLog( THREAD_IDX_CHANGE_RANK, pstrLogString, i32WarningLevel); }
	
	 
	void		Write_Log_DB_Nick( INT32 i32ThreadIdx,  char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ) { WriteLog( i32ThreadIdx + THREAD_IDX_CHANGE_NICK_DB_ST, pstrLogString, i32WarningLevel); }
	void		Write_Log_DB_Rank( INT32 i32ThreadIdx,  char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ) { WriteLog( i32ThreadIdx + THREAD_IDX_CHANGE_RANK_DB_ST, pstrLogString, i32WarningLevel); }
	void		Write_Log_Recv( INT32 i32ThreadIdx,  char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ) { WriteLog( i32ThreadIdx, pstrLogString, i32WarningLevel); }
	void		Write_Log_DataBase( INT32 i32ThreadIdx,  char * pstrLogString, INT32 i32WarningLevel = WARNING_LEVEL_NO ) { WriteLog( i32ThreadIdx + THREAD_IDX_DATABASE_ST, pstrLogString, i32WarningLevel); }
	 
};

extern CLogFile * g_pLogFile;

#endif

