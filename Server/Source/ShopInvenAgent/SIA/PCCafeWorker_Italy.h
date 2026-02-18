#ifndef __PCCAFE_WORKER_ITALY_H__
#define __PCCAFE_WORKER_ITALY_H__

#include "PCCafeTCP.h"

class CPCCafeWorker_Italy : public CPCCafeTCP
{
protected:
	virtual BOOL			_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput );

public:
	CPCCafeWorker_Italy();
	virtual ~CPCCafeWorker_Italy();
};

#endif