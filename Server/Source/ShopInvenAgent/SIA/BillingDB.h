#ifndef __BILLING_DB_H__
#define __BILLING_DB_H__

#include "BillingBase.h"

class CBillingDB	: public CBillingBase
{
	CDBConfig*				m_pBillingDBConfig;

protected	:
	CADODatabase*			m_pDB;

public:
	CBillingDB();
	virtual ~CBillingDB();

	virtual BOOL			OnCreate(INT32 i32Idx, UINT8 ui8MethodIdx);
	virtual void			OnDestroy(void); 

	BOOL					ReConnect( wchar_t* wstrQuery );

};

#endif