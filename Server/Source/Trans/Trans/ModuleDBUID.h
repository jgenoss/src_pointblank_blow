#ifndef __MODULE_DATABASE_UID_H__
#define __MODULE_DATABASE_UID_H__

#include "S2RingBuffer.h"

class i3ODBC; 
class CADODatabase; 

class CModuleDBUID
{
	//-------------------------------------------------------------------------------
	//For NickName
	CADODatabase*			m_pDB; 
	S2RingBuffer*			m_ppCommandIn_UID;
	S2RingBuffer*			m_ppCommandOut_UID;
	INT32					m_i32LogIdx;
	HANDLE					m_hThread;

	CDBConfig*				m_pUserDBConfig;
	BOOL					m_bIsRunning;

protected:

	void					ErrorDBUID( wchar_t* wstrSQL );
	void					_DataBaseResultFailLog( wchar_t *wstrSQL );

	INT32					_GetUID_UIDTHREAD( LOGIN_IN_DB* pLoginInDB );

public:

	CModuleDBUID();
	virtual ~CModuleDBUID();

	BOOL					OnCreate();
	void					OnDestroy();
	void					OnWorking(void);
	
	UID_IN	*				PushUIDPT(void)						{ return (UID_IN*)m_ppCommandIn_UID->PushPointer(); }
	void					PushUIDPTIdx(void)					{ m_ppCommandIn_UID->PushPointerIdx(); }
	
	S2RingBuffer*			GetRingOut()						{	return m_ppCommandOut_UID;		}

	INT32					PushBirthDaySet(UINT32 iServerIdx, INT32 iSessionIdx, T_UID i64DbIdx, UINT32 ui32BirthDay);
};

#endif