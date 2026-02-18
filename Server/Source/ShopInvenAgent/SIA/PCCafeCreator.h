#ifndef __PCCAFE_CREATOR_H__
#define __PCCAFE_CREATOR_H__

#include "PCCafeBase.h"

class CPCCafeCreator
{

public:
	static INT32				GetWorkerSize( INT32 i32LinkMethod );
	static CPCCafeBase*			GetWorker( INT32 i32LinkMethod );
};

#endif // __PCCAFE_CREATOR_H__