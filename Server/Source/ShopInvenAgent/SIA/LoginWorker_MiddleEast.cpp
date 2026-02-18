//////////////////////////////////
#include "pch.h"
#include "LoginWorker.h"
#include "LoginWorker_MiddleEast.h"

// cocerned with billing system 
#import "bxIPGClient.dll" no_namespace

#define BILL_SUCCESS	"0000"
#define BUF_SIZE		512
#define WTOM( wstr, mstr ) WideCharToMultiByte( CP_ACP, 0, (wstr), -1, (mstr), BUF_SIZE, NULL, NULL )
//////////////////////////////////

I3_CLASS_INSTANCE( LoginWorker_MiddleEast, CLoginWorker);


LoginWorker_MiddleEast::LoginWorker_MiddleEast()
{
	m_pHttpLogIn = NULL;

	m_ui32LoginConnectHTTPRetryCount = 0; 

	m_HtmlParser.addTag( "result" );
}

LoginWorker_MiddleEast::~LoginWorker_MiddleEast()
{

	OnDestroy();
}

BOOL LoginWorker_MiddleEast::ConnectionCreate()
{
	char szReturnString[MAX_PATH];

	m_pHttpLogIn = i3NetworkHTTP::NewObject();
	m_pHttpLogIn->CreateMethod( 0, 0, 0, 1000, 1024 ); 

	if( FALSE == m_pHttpLogIn->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
	{	
		i3String::Format( szReturnString, MAX_PATH, "Error : %s (code: %d)", m_pHttpLogIn->GetLastErrorString(), m_pHttpLogIn->GetLastErrorCode());
		g_pModuleLogFile->Write_Log_Main(szReturnString );
		return FALSE;
	}

	return TRUE;
}

BOOL LoginWorker_MiddleEast::ConnectionDestroy()
{
	I3_SAFE_RELEASE( m_pHttpLogIn ); 

	return TRUE;
}

bool LoginWorker_MiddleEast::_WorkProcess( LOGIN_INPUT *pData, LOGIN_OUTPUT* pLoginOut )
{
	switch( pData->_CommandType )
	{
	case TYPE_LOGIN_WEB: pLoginOut->_iResult	= EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break;
	case TYPE_LOGIN_ID:
		{
			pLoginOut->_i64DbIdx	= 0;
			pLoginOut->_iResult	= EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			
			m_pHttpLogIn->ResetArguments();
			m_pHttpLogIn->AddArgument( "username", pData->_Id );
			m_pHttpLogIn->AddArgument( "password", pData->_Passwd);  
			m_pHttpLogIn->SetUpPostArguments();

			if ( m_pHttpLogIn->SendRequestPost() )
			{
				m_pHttpLogIn->RecvRequest();

				BOOL bSuccess	= _ParsingLogInByMiddleEast( m_pHttpLogIn->GetRequestResult() );

				// Check Balance
				BalanceCheck	balanceCheck;
				i3String::Format(balanceCheck.m_strUserNo, 40, "%d", pData->_i64DbIdx);
				strncpy(balanceCheck.m_strUserID, pData->_Id, sizeof(balanceCheck.m_strUserID));  
				strncpy(balanceCheck.m_strUserNo, pData->_Id, sizeof(balanceCheck.m_strUserNo)); 
				strncpy(balanceCheck.m_strCompanyCd, "TAHD", sizeof(balanceCheck.m_strCompanyCd)); 

				// Get billing balance data
				GetCash(balanceCheck);

				if ( bSuccess )
				{
					pLoginOut->_iResult			= EVENT_ERROR_SUCCESS;
					pLoginOut->_i64DbIdx			= 0;
					pLoginOut->_birthday			= 19800101;
					pLoginOut->_ui8Gender			= 1;
					pLoginOut->_pcCafe			= PC_NOT_CAFE;
					pLoginOut->m_ui8ApplingAdult	= APPLING_ADULT_YES;
					pLoginOut->_ncCoin			= (INT32)balanceCheck.m_lBalnace;

					i3mem::Copy( pLoginOut->_Id, pData->_Id, NET_ID_SIZE );
				}
				else
				{
				 	pLoginOut->_iResult		= EVENT_ERROR_EVENT_LOG_IN_ID_FAIL;
				}
			}
			else
			{
				m_ui32LoginConnectHTTPRetryCount++;

				pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_HTTP;

				char strTemp[1024];

				m_pHttpLogIn->DisConnect();
				if ( !m_pHttpLogIn->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
				{
					i3String::Format( strTemp, 1024, "Error : %s (code: %d) - HTTP Connect Fail!", m_pHttpLogIn->GetLastErrorString(), m_pHttpLogIn->GetLastErrorCode());
					g_pModuleLogFile->Write_Log_Login(m_Idx, strTemp, WARNING_AUTH);
				}

				i3String::Format(strTemp, 1024, "ReConnect HTTP Login. [RetryCount= %d]", m_ui32LoginConnectHTTPRetryCount); 
				g_pModuleLogFile->Write_Log_Login( m_Idx, strTemp, WARNING_AUTH);
			}
		}
		break;
	case TYPE_LOGIN_LOGOUT:
		{
			pLoginOut->_iResult	= EVENT_ERROR_SUCCESS; 
			pLoginOut->_i64DbIdx	= pData->_i64DbIdx;			
		} break;
	default: 
		{
			pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; 
		} break; 
	}

	_PushReult( pLoginOut );
	return true;
}

BOOL LoginWorker_MiddleEast::_ParsingLogInByMiddleEast( char* pszString )
{
	if ( i3String::Length( pszString ) == 0 || pszString == NULL || i3String::Length( pszString ) > 512 )
	{
		g_pModuleLogFile->Write_Log_Login( m_Idx, "Middle East LoginHTTP Return Parameter is wrong!!");
		return FALSE;
	}
		
	m_HtmlParser.RunParse( pszString );

	if ( m_HtmlParser.CompareByIdx( 0, "0" ) )  // login fail
	{
		return false;
	}
	else if ( m_HtmlParser.CompareByIdx( 0, "1" ) ) // login success
	{
		return true;
	}

	return FALSE;
}


bool LoginWorker_MiddleEast::OutputIPG(Withdrawal& withdrawal)
{	
	IIPGClientPtr ptrClient;
	ptrClient.CreateInstance(__uuidof(IPGClient));
	
	_variant_t vsUserNo, vsUserId, vsCompanyCd, vsNickName, vsUserIP, vlGoodsNo, vlPayAmt;
	_variant_t vsAgencyNo, vsEtc2, vsEtc3, vsEtc4, vsBxaid, vsRetCode;
		
	//vsUserNo = (LPSTR)"1234";
	//vsUserId = (LPSTR)"testid";
	vsCompanyCd = (LPSTR)"BILL";
	//vsNickName = (LPSTR)"nick";
	//vsUserIP = (LPSTR)"1.1.1.1";
	vlGoodsNo = (LONG)1;
	vlPayAmt = (LONG)100;
	vsAgencyNo = (LPSTR)"1234";
	vsEtc2 = (LPSTR)"test1";
	vsEtc3 = (LPSTR)"test2";
	vsEtc4 = (LPSTR)"test3";

	vsRetCode = ptrClient->Output(&vsUserNo, &vsUserId, &vsCompanyCd, &vsNickName, &vsUserIP, 
								&vlGoodsNo, &vlPayAmt, &vsAgencyNo, &vsEtc2, &vsEtc3, &vsEtc4, &vsBxaid); 
	
	// Output return code
	char szRetCode[5] = {0};
	WTOM(vsRetCode.bstrVal, szRetCode);
	I3TRACE("RetCode : %s\n", szRetCode); 

	if(strncmp(szRetCode, BILL_SUCCESS, 4) == 0)
	{
		char szTemp[BUF_SIZE] = {0};
		i3mem::FillZero(szTemp, BUF_SIZE);
		WTOM(vsBxaid.bstrVal, szTemp);
		I3TRACE("Bxaid : %s\n", szTemp);
	}
	else
	{
		I3TRACE( "unknown error\n" );
		CoUninitialize();
		return false;
	}
	
	return true;
}

bool LoginWorker_MiddleEast::GetCash(BalanceCheck& balanceCheck)
{
	IIPGClientPtr ptrClient;
	ptrClient.CreateInstance(__uuidof(IPGClient));
	
	_variant_t vsUserNo, vsUserId, vsCompanyCd, vsNickName, vlRemainCashShop, vlRemainCashContents;
	_variant_t vlRemainMileageShop, vlRemainMileageContents, vlRemainCashBonus, vlRemainCashEtc;
	_variant_t vsRetCode;

	_variant_t vsUserIP, vlGoodsNo, vlPayAmt;
	_variant_t vsAgencyNo, vsEtc2, vsEtc3, vsEtc4, vsBxaid;
	
	vsUserNo = (LPSTR)balanceCheck.m_strUserNo;
	vsUserId = (LPSTR)balanceCheck.m_strUserID;
	vsCompanyCd = (LPSTR)"TAHD";

	vsRetCode = ptrClient->GetCash(&vsUserNo, &vsUserId, &vsCompanyCd, &vsNickName, &vlRemainCashShop, 
							&vlRemainCashContents, &vlRemainCashBonus, &vlRemainCashEtc, 
							&vlRemainMileageShop, &vlRemainMileageContents);


	// Print Result
	balanceCheck.m_lBalnace = vlRemainCashShop.lVal + vlRemainCashContents.lVal + vlRemainCashBonus.lVal + vlRemainCashEtc.lVal + vlRemainMileageShop.lVal + vlRemainMileageContents.lVal;
	WTOM(vsRetCode.bstrVal, balanceCheck.m_strRetCode);

	if(strncmp(balanceCheck.m_strRetCode, BILL_SUCCESS, 4) == 0)
	{
		I3TRACE("Success GetCash() \n");
	}
	else
	{
		I3TRACE( "unknown error\n" );
		CoUninitialize();
		return false;
	}
	
	I3TRACE("RetCode               : %s\n", balanceCheck.m_strRetCode);
	I3TRACE("RemainCashShop        : %ld\n", vlRemainCashShop.lVal);
	I3TRACE("RemainCashContents    : %ld\n", vlRemainCashContents.lVal);
	I3TRACE("RemainCashBonus       : %ld\n", vlRemainCashBonus.lVal);
	I3TRACE("RemainCashEtc         : %ld\n", vlRemainCashEtc.lVal);
	I3TRACE("RemainMileageShop     : %ld\n", vlRemainMileageShop.lVal);
	I3TRACE("RemainMileageContents : %ld\n", vlRemainMileageContents.lVal);

	return true;
}


