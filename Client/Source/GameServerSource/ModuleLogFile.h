#ifndef __MODULE_LOGFILE_H__
#define __MODULE_LOGFILE_H__

#define LOG_FILE_STRING_COUNT		1024
struct LOG_FILE_BUFFER
{		
	char	_szMessage[LOG_FILE_STRING_COUNT];

	// SWS
	INT32	_i32WarningLevel;
};

class CUserSession; 

class CModuleLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CModuleLogFile );
private:		
	//For File 
	BOOL					m_bRunning;
	INT32					m_i32ThreadCount; 

	HANDLE					m_hFile;
	UINT32					m_ui32DateHour;

	LOG_FILE_BUFFER		*	m_pMainLogFileBuffer;
	INT32					m_i32Main_WritePos;	
	INT32					m_i32Main_ReadPos;	

	LOG_FILE_BUFFER		*	m_pModuleNCLogFileBuffer;
	INT32					m_i32ModuleNc_WritePos;	
	INT32					m_i32ModuleNc_ReadPos;	

	LOG_FILE_BUFFER		*	m_pModuleControlLogFileBuffer;
	INT32					m_i32ModuleControl_WritePos;
	INT32					m_i32ModuleControl_ReadPos;

	LOG_FILE_BUFFER		*	m_pModuleTransLogFileBuffer;
	INT32					m_i32ModuleTrans_WritePos;
	INT32					m_i32ModuleTrans_ReadPos;

	LOG_FILE_BUFFER		*	m_pModuleClanLogFileBuffer;
	INT32					m_i32ModuleClan_WritePos;
	INT32					m_i32ModuleClan_ReadPos;

	LOG_FILE_BUFFER		*	m_pModuleMessengerLogFileBuffer;
	INT32					m_i32ModuleMessenger_WritePos;
	INT32					m_i32ModuleMessenger_ReadPos;

	LOG_FILE_BUFFER		*	m_pModuleDBLogFileBuffer;
	INT32					m_i32ModuleDB_WritePos; 
	INT32					m_i32ModuleDB_ReadPos; 

	LOG_FILE_BUFFER		**	m_ppUserLogFileBuffer;
	INT32				*	m_pUser_WritePos;	
	INT32				*	m_pUser_ReadPos;	

	LOG_FILE_BUFFER		*	m_pModuleXTrapLogFileBuffer;
	INT32					m_i32ModuleXTrap_WritePos;
	INT32					m_i32ModuleXTrap_ReadPos;

protected:
	UINT32					m_ui32DateTime; 

protected:
	BOOL _CreateFile(void); 
	void _WriteLogFile(char * pstrLog)
	{
		INT32 i32Len = i3String::Length(pstrLog); 
		::WriteFile( m_hFile, pstrLog, i32Len, (LPDWORD)&i32Len, NULL ); 
	}

public: 	
	CModuleLogFile(void); 
	virtual ~CModuleLogFile(void); 

	BOOL			OnCreate(INT32 i32ThreadCount);
	void			OnDestroy(void);
	virtual UINT32	OnRunning( void * pUserData);

	// SWS 
	void			Write_LogFile(INT32 i32ThreadIdx, CUserSession * pSession, char * pstrLogString, /*SWS*/ INT32 i32WarningLevel );
	void			WriteWarningLevel( INT32 i32WarningLevel );

	void			Write_M_Log( INT32 i32ThreadIdx, CUserSession * pSession, char * pstrLogString, INT32 i32WarningLevel = WARNING_NO ) { Write_LogFile( i32ThreadIdx, pSession, pstrLogString, i32WarningLevel); }	
};

extern CModuleLogFile * g_pModuleLogFile; 

//#define g_pModuleLogFile->Write_M_Log(a, b, c)	g_pModuleLogFile->Write_LogFile(a, b, c)

#endif
