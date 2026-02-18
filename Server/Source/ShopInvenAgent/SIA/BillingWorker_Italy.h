#ifndef __BILLING_WORKER_ITALY_H__
#define __BILLING_WORKER_ITALY_H__

#include "_GameProtocol.h"
#include "Payletter.h"
#include "BillingTCP.h"

#define ITALY_BILLING_PACKET_HEADER_SIZE			4

class CBillingWorker_Italy : public CBillingTCP
{

protected:
	virtual BOOL			_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx );
	void					_GetLinkErrorMessage( INT32 i32ErrorCode, char* strErrorMessage, INT32 i32Size );

public:
	CBillingWorker_Italy();
	~CBillingWorker_Italy();	

	virtual INT32			RecvMessage( char* pPacket, INT32 i32Size );
};

#endif // __BILLING_WORKER_ITALY_H__