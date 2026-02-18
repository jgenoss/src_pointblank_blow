#ifndef __BILLING_HTTP_H__
#define __BILLING_HTTP_H__

#include "BillingBase.h"

class CBillingHTTP	: public CBillingBase
{
	INT32					m_i32HTTPRetryCount;

	char					m_strLink[ PATH_STRING_COUNT ];
	i3NetworkHTTP::REQUEST_METHOD	m_eConnectMethod;

protected	:
	i3NetworkHTTP*			m_pHttp;

public:
	CBillingHTTP();
	virtual ~CBillingHTTP();

	virtual BOOL			OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx );
	virtual void			OnDestroy(void); 

	BOOL					OnConnect();
	BOOL					ReConnect();
};

#endif