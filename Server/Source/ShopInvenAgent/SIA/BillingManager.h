#ifndef __BILLING_MANAGER_H__
#define __BILLING_MANAGER_H__

#include "BillingBase.h"

class CBillingManager
{
	UINT32					m_ui32WorkThreadCount;

	UINT32					m_ui32PushThreadIdx;
	UINT32					m_ui32PopThreadIdx;

	CBillingBase**			m_ppBilling;

public:
	CBillingManager();
	~CBillingManager();

	BOOL					Create( UINT8 ui8MethodIdx );
	void					Destroy();

	INT32					PushData( BILLING_INPUT* pInputData );

	INT32					PopCount_T();
	BILLING_OUTPUT*			PopData_T();
	void					PopIdx_T();
};

#endif