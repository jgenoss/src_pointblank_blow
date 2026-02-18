#ifndef __LOGIN_DB_H__
#define __LOGIN_DB_H__

#include "LoginBase.h"


class CLoginDB : public CLoginBase
{
protected :	 
	CDBConfig*				m_pLoginDBConfig;
	CADODatabase*			m_pDB;

public:
	CLoginDB(void);
	virtual ~CLoginDB(void);
	
	virtual BOOL			OnCreate( INT32 i32dx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx );
	void					OnDestroy(void);

	BOOL					ReConnect( wchar_t* pwstrQuery );
};

#endif // __LOGIN_DB_H__