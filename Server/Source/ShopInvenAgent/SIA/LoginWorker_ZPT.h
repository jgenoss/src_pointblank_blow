#ifndef __LOGIN_WORKER_ZPT_H__
#define __LOGIN_WORKER_ZPT_H__

#include "LoginDB.h"

class CLoginWorker_ZPT : public CLoginDB
{

protected:
	virtual BOOL			_WorkProcess( LOGIN_INPUT* pLoginIn, LOGIN_OUTPUT* pLoginOut );
	
public:
	CLoginWorker_ZPT();
	virtual ~CLoginWorker_ZPT();
};

#endif //__LOGIN_WORKER_ZPT_H__