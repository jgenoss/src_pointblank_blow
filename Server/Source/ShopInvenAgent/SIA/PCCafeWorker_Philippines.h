#ifndef __PCCAFE_WORKER_PHILIPPINES_H__
#define __PCCAFE_WORKER_PHILIPPINES_H__

#include "PCCafeBase.h"

class CPCCafeWorker_Philippines : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_Philippines();
	virtual ~CPCCafeWorker_Philippines();
};

#endif