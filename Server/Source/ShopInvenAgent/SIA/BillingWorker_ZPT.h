#ifndef __BILLING_WORKER_ZPT_H__
#define __BILLING_WORKER_ZPT_H__

#include "BillingDB.h"

class CBillingWorker_ZPT : public CBillingDB
{
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

public:
	CBillingWorker_ZPT();
	~CBillingWorker_ZPT();
};

#endif // __BILLING_WORKER_ZPT_H__