#ifndef __LOGIN_WORKER_TURKEY_H__
#define __LOGIN_WORKER_TURKEY_H__

class CLoginWorker_Turkey : public CLoginHTTP
{
public:
	virtual BOOL			OnCreate( INT32 i32Idx, char* strLinkHttp, REQUEST_METHOD eConnectMethod );

private: 
	i3NetworkHTTP	*		m_pHttpLogIn;					
	i3NetworkHTTP	*		m_pHttpBilling;					
	UINT32					m_nLoginConnectHTTPRetryCount;
	UINT32					m_nBillingConnectHTTPRetryCount;

protected:
	virtual bool			_WorkProcess( LOGIN_INPUT * pLoginIn, LOGIN_OUTPUT* pLoginOut );

	//Turkey
	BOOL					_ParsingLogInByTurkey( char * pszString, INT32 * iRv, UINT8 * pUID); 

public:
	CLoginWorker_Turkey();
	virtual ~CLoginWorker_Turkey();

	BOOL					ConnectionCreate();
	BOOL					ConnectionDestroy();
};

#endif // #ifndef _LOGINWORKER_TURKEY_H