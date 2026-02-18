#ifndef __BILLING_WORKER_VIETNAM_H__
#define __BILLING_WORKER_VIETNAM_H__

#include "BillingDB.h"

class CBillingWorker_Vietnam : public CBillingDB
{
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

	void					_ParsingBuyItem( BILLING_INPUT * pInput, BILLING_OUTPUT * pOut, UINT8 ui8GoodsIdx  );

public:
	CBillingWorker_Vietnam();
	~CBillingWorker_Vietnam();

};

#endif // __BILLING_WORKER_VIETNAM_H__