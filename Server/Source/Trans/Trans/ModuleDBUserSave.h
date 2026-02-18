#ifndef _MODULE_DB_USER_SAVE_H__
#define _MODULE_DB_USER_SAVE_H__

#include "LocalRingBuffer.h"

class i3ODBC; 
class CADODatabase; 

class CModuleDBUserSave :	public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleDBUserSave );

	DWORD					m_dwTempTime[DATABASE_THREAD_COUNT];
	INT32					m_i32LoginCount[DATABASE_THREAD_COUNT];

protected :
	INT32					m_pi32LogIdx[ DATABASE_THREAD_COUNT ];
	HANDLE					m_phThread[DATABASE_THREAD_COUNT];
	BOOL					m_pbRunning[DATABASE_THREAD_COUNT];

	CDBConfig*				m_pSaveDBConfig[DATABASE_THREAD_COUNT];
	CADODatabase	*		m_ppSaveDB[DATABASE_THREAD_COUNT];

	CDBConfig*				m_pNickDBConfig;
	CADODatabase	*		m_ppNickDB[ DATABASE_THREAD_COUNT ];

	i3RingBuffer *			m_ppExitSaveIn[ DATABASE_THREAD_COUNT ];
	i3RingBuffer *			m_ppExitSaveOut[ DATABASE_THREAD_COUNT ];
	INT32					m_i32ExitSaveOutIdx;

	i3RingBuffer *			m_ppCreateNickIn[ DATABASE_THREAD_COUNT ];
	i3RingBuffer *			m_ppCreateNickOut[ DATABASE_THREAD_COUNT ];
	INT32					m_i32CreateNickOutIdx;

	i3RingBuffer *			m_ppCreateCharIn[ DATABASE_THREAD_COUNT ];
	i3RingBuffer *			m_ppCreateCharOut[ DATABASE_THREAD_COUNT ];
	INT32					m_i32CreateCharOutIdx;

	
    
protected :
	//-------------------------------------------------------------------------------
	//For UserInfo  Save Load 
	void					ErrorDBUserSave(INT32 i32ThreadIdx, wchar_t* wstrSQL);
	void					ErrorDBUserNick( INT32 i32ThreadIdx, wchar_t* wstrSQL);
	void					_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t* wstrSQL); 

	BOOL					_WorkingExitSave( INT32 i32ThreadIdx );
	BOOL					_WorkingCreateNick( INT32 i32ThreadIdx );
	BOOL					_WorkingCreateChar( INT32 i32ThreadIdx );

	BOOL					_SaveExitSave( INT32 i32Threadidx, RING_EXIT_SAVE_IN * pIn); 
	BOOL					_SaveCreateNick( INT32 i32Threadidx, RING_CREATE_NICK_IN* pIn, RING_CREATE_NICK_OUT* pOut );
	BOOL					_SaveCreateChar( INT32 i32Threadidx, RING_CREATE_CHAR* pIn, RING_CREATE_CHAR* pOut );
	
	
	//UID THREAD
	INT32					_GetUID_UIDTHREAD(char * strId, T_UID * pi64UID); 

	

public:
	CModuleDBUserSave(void);
	virtual ~CModuleDBUserSave(void);
	
	BOOL					OnCreate(); 
	void					OnDestroy(void);
	
	//-------------------------------------------------------------------------------
	//For UserInfo  Save Load 
	void					OnWorking(INT32 i32ThreadIdx);
	
	INT32					PushExitSave( RING_EXIT_SAVE_IN* pExitSave );
	INT32					PushCreateNick( RING_CREATE_NICK_IN* pCreateNick );
	INT32					PushCreateChar( RING_CREATE_CHAR* pCreateChar );

	INT32					GetExitSaveCount(void); 
	RING_EXIT_SAVE_OUT *	GetExitSave(void);
	void					GetExitSaveIdx(void);

	INT32					GetCreateNickCount(void); 
	RING_CREATE_NICK_OUT*	GetCreateNick(void);
	void					GetCreateNickIdx(void);

	INT32					GetCreateCharCount(void); 
	RING_CREATE_CHAR*	GetCreateChar(void);
	void					GetCreateCharIdx(void);
};

#endif 
