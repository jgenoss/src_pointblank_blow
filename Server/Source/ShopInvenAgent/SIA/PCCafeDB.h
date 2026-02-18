#ifndef __PCCAFE_DB_H__
#define __PCCAFE_DB_H__

#include "PCCafeBase.h"

class CPCCafeDB	: public CPCCafeBase
{
	CDBConfig*				m_pDBConfig;

protected	:
	CADODatabase	*		m_pDB;

public:
	CPCCafeDB();
	virtual ~CPCCafeDB();

	virtual BOOL			OnCreate( INT32 i32Idx );
	virtual void			OnDestroy(void); 

	BOOL					ReConnect( wchar_t* wstrQuery );

};

#endif