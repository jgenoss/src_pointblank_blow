#ifndef __PCCAFE_WORKER_NO_H__
#define __PCCAFE_WORKER_NO_H__

#include "PCCafeBase.h"

class CPCCafeWorker_NO : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_NO();
	virtual ~CPCCafeWorker_NO();
};

#endif