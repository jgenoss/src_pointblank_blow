#ifndef __LOGIN_WORKER_PHILIPPINES_H__
#define __LOGIN_WORKER_PHILIPPINES_H__

#include "LoginBase.h"

#include "z_OpensslRSA.h"
#include "z_base64.h"

#pragma pack (1)
struct UserTokInfoPhilippines_t
{
	UINT32					ui32Uid;
	char					strUsername[32];
	UINT32					ui32Timestamp;
	UINT16					ui16GcaType;
	UINT8					ui8GoldMemberType;
	char					acReserved[57];
};
#pragma pack ()

class CLoginWorker_Philippines : public CLoginBase
{
	COpensslRSA				m_OpensslRSA;

protected:
	virtual BOOL			_Create();
	virtual void			_Destroy();
	virtual BOOL			_WorkProcess( LOGIN_INPUT* pLoginIn, LOGIN_OUTPUT* pLoginOut );


	BOOL					_DecryptSessionKey( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput );
	
public:
	CLoginWorker_Philippines();
	virtual ~CLoginWorker_Philippines();
};

#endif // __LOGIN_WORKER_PHILIPPINES_H__