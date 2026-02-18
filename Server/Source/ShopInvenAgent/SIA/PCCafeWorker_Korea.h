#ifndef __PCCAFE_WORKER_KOREA_H__
#define __PCCAFE_WORKER_KOREA_H__

#include "PCCafeBase.h"

class CWorkerStreamBuffer_PCCafe;

class CPCCafeWorker_Korea : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess(PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput);

public:
	CPCCafeWorker_Korea();
	virtual ~CPCCafeWorker_Korea();
};

#endif