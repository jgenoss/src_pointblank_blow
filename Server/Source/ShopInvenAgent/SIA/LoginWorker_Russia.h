#ifndef _LOGINWORKER_RUSSIA_H
#define _LOGINWORKER_RUSSIA_H

class CLoginWorker_Russia : public CLoginBase
{

protected:
	virtual BOOL _WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput );
	
public:
	CLoginWorker_Russia();
	virtual ~CLoginWorker_Russia();
};

#endif // #ifndef _LOGINWORKER_RUSSIA_H