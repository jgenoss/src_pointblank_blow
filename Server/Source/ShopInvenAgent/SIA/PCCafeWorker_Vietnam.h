#ifndef __PCCAFE_WORKER_VIETNAM_H__
#define __PCCAFE_WORKER_VIETNAM_H__

#include "PCCafeBase.h"

class CPCCafeWorker_Vietnam : public CPCCafeBase
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_Vietnam();
	virtual ~CPCCafeWorker_Vietnam();
};

#endif