#ifndef _MODULE_DATABASE_USER_LOAD_H__
#define _MODULE_DATABASE_USER_LOAD_H__

#include "LocalRingBuffer.h"

class i3ODBC; 
class CADODatabase; 

class CModuleDBUserLoad :	public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleDBUserLoad );

	DWORD					m_dwTempTime[DATABASE_THREAD_COUNT];
	INT32					m_i32LoginCount[DATABASE_THREAD_COUNT];

protected :
	CDBConfig*				m_pUserDBConfig[DATABASE_THREAD_COUNT];

	//-------------------------------------------------------------------------------
	//For UserInfo  Save Load 
	INT32					m_pi32LogIdx[ DATABASE_THREAD_COUNT ];
	HANDLE					m_phThread[DATABASE_THREAD_COUNT];
	CADODatabase	*		m_ppADO[DATABASE_THREAD_COUNT];

	Local_CRingBuffer *		m_ppCommandIn_LoadInfo[DATABASE_THREAD_COUNT];
	Local_CRingBuffer *		m_ppCommandOut_LoadInfo[DATABASE_THREAD_COUNT]; 

	BOOL					m_pbIsRunning[DATABASE_THREAD_COUNT];

	INT32					m_i32LoadInfoOutIdx; 
	//INT32					m_i32NickOutIdx;
    
	INT32					m_i32LastUpdateTime;
protected :
	//-------------------------------------------------------------------------------
	//For UserInfo  Save Load 
	void					ErrorDBUserLoad( INT32 i32ThreadIdx, wchar_t* wstrSQL ); 
	void					_DataBaseResultFailLog( INT32 i32ThreadIdx, wchar_t* wstrSQL ); 

	BOOL					_LoadUserInfo( INT32 i32Threadidx, LOAD_USER_INFO_OUT * pOut ); 
	
	//UID THREAD
	INT32					_GetUID_UIDTHREAD(char * strId, T_UID * pi64UID); 

public:
	CModuleDBUserLoad(void);
	virtual ~CModuleDBUserLoad(void);
	
	BOOL					OnCreate(); 
	void					OnDestroy(void);
	
	//-------------------------------------------------------------------------------
	//For UserInfo  Save Load 
	void					OnWorking(INT32 i32ThreadIdx);

	INT32					PushLoadInfo( USER_NODE * pNode );

	INT32					GetBufferCount(void); 
	LOAD_USER_INFO_OUT *	OutGetLoadInfo(void);
	void					OutGetLoadInfoIdx(void); 

	// 사용안함.
	//INT32					OutGetNickChangeCount(void);
	//NICK_CHANGE_OUT*		OutGetNickChange(void);
	//void					OutGetNickChangeIdx(void);
};

#endif 
