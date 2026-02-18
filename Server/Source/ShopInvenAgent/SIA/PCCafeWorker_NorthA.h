#ifndef __PCCAFE_WORKER_NORTHA_H__
#define __PCCAFE_WORKER_NORTHA_H__

#include "PCCafeBase.h"

class CPCCafeWorker_NorthA : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_NorthA();
	virtual ~CPCCafeWorker_NorthA();
};

#endif