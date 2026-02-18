#pragma once

#include "HtmlParser.h"

// Concerned with Middle east Billing System////////////////
// Essential input parameter data to use builling interface
struct Withdrawal
{
	// In Parameter
	char m_strUserNo[40];	// User Number
	char m_strUserID[40];	// User Id	
	char m_strCompanyCd[5];	// Company code
	char m_strUserNick[30];	// User Nickname
	char m_strUserIP[16];	
	char m_strGoodsNo[6];	
	char m_strPayAmt[12];   // Product Price

	// Output Parameter		
	char m_strRetCode[5];	// Return code
};

// To check Balance
struct BalanceCheck
{
	// In Parameter
	char m_strUserNo[40];	// User Number
	char m_strUserID[40];	// User Id	
	char m_strCompanyCd[5];	// Company code
	char m_strUserNick[30];	// User Nickname

	// Output Parameter
	long m_lBalnace;		// Balance
	char m_strRetCode[5];	// Return code
};
///////////////////////////////////////////////////////////


// concerned with Middleeast Login ////////////////////////
typedef struct _LogInReturnValueMiddleEast
{	
	UINT32	ui32Result;		// Success:1, Fail:0

}LOGIN_RETURN_VALUE_BY_MIDDLEEAST;
////////////////////////////////////////////////////////////


class LoginWorker_MiddleEast : public CLoginWorker
{
	I3_CLASS_DEFINE( LoginWorker_MiddleEast );

private: 
	i3NetworkHTTP*	m_pHttpLogIn;					
	UINT32			m_ui32LoginConnectHTTPRetryCount;

	CHtmlParser		m_HtmlParser;
	
protected:
	virtual bool			_WorkProcess( LOGIN_INPUT * pLoginIn, LOGIN_OUTPUT* pLoginOut );
	BOOL _ParsingLogInByMiddleEast( char* strString );

public:

	BalanceCheck m_balanceCheck;


	// constructor/ destructor
	LoginWorker_MiddleEast();
	virtual ~LoginWorker_MiddleEast();

	// External interface
	BOOL ConnectionCreate();
	BOOL ConnectionDestroy();

public:
	// Billing External Interface
	bool OutputIPG(Withdrawal& withdrawal);
	bool GetCash(BalanceCheck& balanceCheck);
};

