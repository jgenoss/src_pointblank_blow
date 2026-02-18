#ifndef _LOGINWORKER_NESIA_H
#define _LOGINWORKER_NESIA_H

typedef struct _LogInReturnValueByNesia
{	
	INT32	iReturnCode; 
	INT32	iGameIdx;
	char	szGameID[MAX_PATH];
	char	szBillingID[MAX_PATH];
	char	szName[MAX_PATH];
	UINT8	iGender;
	INT32	iBirth;
	INT32	iCash;
	UINT8	iActive;				// 1: Active		0:Deactive
}LOGIN_RETURN_VALUE_BY_NESIA;

typedef struct _BalanceReturnValueByNesia
{
	char szReturnCode[MAX_PATH];
	char szServiceCode[MAX_PATH];
	char szID[MAX_PATH];
	INT32 nGiftCash;
	INT32 nCash;
	INT32 nDate;
}BALANCE_RETURN_VALUE_BY_NESIA;

class CLoginWorker_Nesia : public CLoginWorker
{
	I3_CLASS_DEFINE( CLoginWorker_Nesia );

private: 
	i3NetworkHTTP	*	m_pHttpLogIn;					
	i3NetworkHTTP	*	m_pHttpBilling;					
	UINT32				m_nLoginConnectHTTPRetryCount;
	UINT32				m_nBillingConnectHTTPRetryCount;

protected:
	virtual bool			_WorkProcess( LOGIN_INPUT * pLoginIn, LOGIN_OUTPUT* pLoginOut );

	//Indonesia Function
	void _GetExtensionInfoNesia(LOGIN_OUTPUT * pRecvData);
	void _ParsingLogInByNesia(char* pszString,	LOGIN_RETURN_VALUE_BY_NESIA * ReturnValue);
	void _ParsingBillingByNesia(char* pszString,BALANCE_RETURN_VALUE_BY_NESIA * ReturnValue);

public:
	CLoginWorker_Nesia();
	virtual ~CLoginWorker_Nesia();

	BOOL	ConnectionCreate();
	BOOL	ConnectionDestroy();
};

#endif // #ifndef _LOGINWORKER_NESIA_H