#ifndef __BILLING_WORKER_RUSSIA_H__
#define __BILLING_WORKER_RUSSIA_H__

class CBillingWorker_Russia : public CBillingBase
{

protected:
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

public:
	CBillingWorker_Russia();
	~CBillingWorker_Russia();
	
};

#endif