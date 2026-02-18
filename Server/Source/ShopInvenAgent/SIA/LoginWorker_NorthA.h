#ifndef __LOGINWORKER_NORTHA_H__
#define __LOGINWORKER_NORTHA_H__

#include "LoginHTTP.h"

class CLoginWorker_NorthA : public CLoginHTTP
{
protected:
	BOOL					_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput );
	BOOL					_ParsingLogInByNorth( char * pstrString, LOGIN_OUTPUT * pReadData );

public:
	CLoginWorker_NorthA();
	virtual ~CLoginWorker_NorthA();
};

#endif	// __LOGINWORKER_NORTHA_H__