#ifndef __LOGIN_WORKER_KR_H__
#define __LOGIN_WORKER_KR_H__

#include "LoginHTTP.h"

class CLoginWorker_Korea : public CLoginHTTP
{

protected:
	BOOL _WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput );
	BOOL _ParsingLogInByKorea( char * pstrString, LOGIN_OUTPUT * pReadData );
	

public:
	CLoginWorker_Korea();
	virtual ~CLoginWorker_Korea();

};

#endif #ifndef __LOGIN_WORKER_KR_H__