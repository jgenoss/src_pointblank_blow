#ifndef __PCCAFE_WORKER_SM_H__
#define __PCCAFE_WORKER_SM_H__

#include "PCCafeBase.h"

class CPCCafeWorker_SM : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_SM();
	virtual ~CPCCafeWorker_SM();
};

#endif