#pragma once

#include "LoginHTTP.h"

class CLoginWorker_Brazil : public CLoginHTTP
{
protected:
	BOOL					_WorkProcess(LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput);
	BOOL					_ParsingLogInByBrazil(char * pstrString, LOGIN_OUTPUT * pReadData);

public:
	CLoginWorker_Brazil();
	virtual ~CLoginWorker_Brazil();
};