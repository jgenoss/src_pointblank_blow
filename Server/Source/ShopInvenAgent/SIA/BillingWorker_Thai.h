#ifndef __BILLING_WORKER_THAI_H__
#define __BILLING_WORKER_THAI_H__

#include "BillingDB.h"

class CBillingWorker_Thai : public CBillingDB
{
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

	void					_ParsingBuyItem( BILLING_INPUT * pInput, BILLING_OUTPUT * pOut, UINT8 ui8GoodsIdx   );

public:
	CBillingWorker_Thai();
	~CBillingWorker_Thai();

};

#endif // __BILLING_WORKER_THAI_H__