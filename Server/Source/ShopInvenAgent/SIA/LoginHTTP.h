#ifndef __LOGIN_HTTP_H__
#define __LOGIN_HTTP_H__

#include "LoginBase.h"

 class CLoginHTTP : public CLoginBase
{
	INT32					m_i32HTTPRetryCount;

	char					m_strLink[ PATH_STRING_COUNT ];
	i3NetworkHTTP::REQUEST_METHOD	m_eConnectMethod;

protected	:
	i3NetworkHTTP*			m_pHttp;

public:
	CLoginHTTP(void);
	virtual ~CLoginHTTP(void);
	
	virtual BOOL			OnCreate( INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx );
	void					OnDestroy(void);

	BOOL					OnConnect();
	BOOL					ReConnect();
	//BOOL					ReConnect( char* strQuery );
};

#endif // __LOGIN_DB_H__