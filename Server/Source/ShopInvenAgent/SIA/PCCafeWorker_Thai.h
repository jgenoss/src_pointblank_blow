#ifndef __PCCAFE_WORKER_THAI_H__
#define __PCCAFE_WORKER_THAI_H__

#include "PCCafeBase.h"

class CPCCafeWorker_Thai : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_Thai();
	virtual ~CPCCafeWorker_Thai();
};

#endif