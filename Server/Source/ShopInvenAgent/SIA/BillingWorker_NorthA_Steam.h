#ifndef __BILLING_WORKER_NORTHA_STEAM_H__
#define __BILLING_WORKER_NORTHA_STEAM_H__

#include "Payletter.h"
#include "BillingTCP.h"

class CBillingWorker_NorthA_Steam : public CBillingTCP
{

protected:
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );

public:
	CBillingWorker_NorthA_Steam();
	~CBillingWorker_NorthA_Steam();	

	virtual INT32			RecvMessage( char* pPacket, INT32 i32Size );
	
};

#endif // __BILLING_WORKER_NORTHA_H__