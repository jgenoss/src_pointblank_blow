#ifndef __BILLING_WORKER_NO_H__
#define __BILLING_WORKER_NO_H__

#include "BillingDB.h"

class CBillingWorker_NO : public CBillingBase
{
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

public:
	CBillingWorker_NO();
	~CBillingWorker_NO();
};

#endif // __BILLING_WORKER_NO_H__