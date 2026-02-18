#ifndef __PCCAFE_WORKER_RUSSIA_H__
#define __PCCAFE_WORKER_RUSSIA_H__

#include "PCCafeBase.h"

class CPCCafeWorker_Russia : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_Russia();
	virtual ~CPCCafeWorker_Russia();
};

#endif