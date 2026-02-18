#ifndef __BILLING_WORKER_SM_H__
#define __BILLING_WORKER_SM_H__

#include "BillingDB.h"

class CBillingWorker_SM : public CBillingDB
{
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

	void					_ParsingBuyItem( BILLING_INPUT * pInput, BILLING_OUTPUT * pOut, UINT8 ui8GoodsIdx  );

public:
	CBillingWorker_SM();
	~CBillingWorker_SM();

};

#endif // __BILLING_WORKER_SM_H__