#ifndef __BILLING_WORKER_LATIN_H__
#define __BILLING_WORKER_LATIN_H__

#include "Payletter.h"
#include "BillingTCP.h"

class CBillingWorker_Latin : public CBillingTCP
{
protected:
	virtual BOOL			_WorkProcess(BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx);

public:
	CBillingWorker_Latin();
	~CBillingWorker_Latin();

	virtual INT32			RecvMessage(char* pPacket, INT32 i32Size);
};

#endif
