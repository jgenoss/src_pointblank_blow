#ifndef __LOGIN_CREATOR_H__
#define __LOGIN_CREATOR_H__

#include "LoginBase.h"

class CLoginCreator
{

public:
	static INT32				GetWorkerSize( INT32 i32TransMethod );
	static CLoginBase*			GetWorker( INT32 i32TransMethod );
};

#endif // __LOGIN_CREATOR_H__