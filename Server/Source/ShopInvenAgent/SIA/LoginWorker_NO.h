#ifndef __LOGIN_WORKER_NO_H__
#define __LOGIN_WORKER_NO_H__

#include "LoginBase.h"

class CLoginWorker_NO : public CLoginBase
{

protected:
	virtual BOOL			_WorkProcess( LOGIN_INPUT* pLoginIn, LOGIN_OUTPUT* pLoginOut );
	
public:
	CLoginWorker_NO();
	virtual ~CLoginWorker_NO();
};

#endif // __LOGIN_WORKER_NO_H__