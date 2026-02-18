#pragma once

#include "BillingDB.h"

class CBillingWorker_Brazil : public CBillingDB
{
protected:
	virtual BOOL			_WorkProcess(BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx);

	void					_ParsingBuyItem(BILLING_INPUT * pInput, BILLING_OUTPUT * pOut, UINT8 ui8GoodsIdx);

public:
	CBillingWorker_Brazil();
	~CBillingWorker_Brazil();
};