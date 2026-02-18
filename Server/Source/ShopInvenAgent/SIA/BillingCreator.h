#ifndef __BILLING_CREATOR_H__
#define __BILLING_CREATOR_H__

#include "BillingBase.h"

class CBillingCreator
{

public:
	static INT32				GetWorkerSize( INT32 i32LinkMethod );
	static CBillingBase*		GetWorker( INT32 i32LinkMethod );
};

#endif // __BILLING_CREATOR_H__