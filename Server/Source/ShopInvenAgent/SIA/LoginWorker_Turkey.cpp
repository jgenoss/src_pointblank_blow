#include "pch.h"
#include "LoginWorker_Turkey.h"

CLoginWorker_Turkey::CLoginWorker_Turkey()
{
	m_pHttpLogIn	= NULL;
	m_pHttpBilling	= NULL; 

	m_nLoginConnectHTTPRetryCount = 0;
	m_nBillingConnectHTTPRetryCount = 0;	
}

CLoginWorker_Turkey::~CLoginWorker_Turkey()
{
	OnDestroy();
}

BOOL CLoginWorker_Turkey::ConnectionCreate()
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

	m_pHttpBilling = i3NetworkHTTP::NewObject(); 
	m_pHttpBilling->CreateMethod( 0, 0, 0, 1000, 1024 ); 
	if( FALSE == m_pHttpBilling->Connect( g_pConfig->m_BillingHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
	{
		i3String::Format( szReturnString, MAX_PATH, "Error : %s (code: %d)", m_pHttpBilling->GetLastErrorString(), m_pHttpBilling->GetLastErrorCode());
		g_pModuleLogFile->Write_Log_Main(szReturnString ); 
		return FALSE;
	}

	return TRUE;
}

BOOL CLoginWorker_Turkey::ConnectionDestroy()
{
	I3_SAFE_RELEASE( m_pHttpLogIn ); 
	I3_SAFE_RELEASE( m_pHttpBilling); 

	return TRUE;
}

bool CLoginWorker_Turkey::_WorkProcess( LOGIN_INPUT *pData, LOGIN_OUTPUT* pLoginOut )
{
	switch( pData->_CommandType )
	{
	case TYPE_LOGIN_WEB			: pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN; break; 
	case TYPE_LOGIN_ID			: 
		{
			struct in_addr	ConIp;
			pLoginOut->_i64DbIdx  = 0; 
			pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;				
			ConIp.s_addr		=	pData->_Ip;
			m_pHttpLogIn->ResetArguments();
			m_pHttpLogIn->AddArgument( "Id", pData->_Id );
			m_pHttpLogIn->AddArgument( "Pwd", pData->_Passwd );
			m_pHttpLogIn->AddArgument( "IP", inet_ntoa( ConIp ));
			m_pHttpLogIn->SetUpPostArguments();

			if( m_pHttpLogIn->SendRequestPost() )
			{
				m_pHttpLogIn->RecvRequest();

				if( _ParsingLogInByTurkey( m_pHttpLogIn->GetRequestResult(), &pLoginOut->_iResult, pLoginOut->_pLinkId ))
				{						
					pLoginOut->_birthday			= 19800101;
					pLoginOut->_ui8Gender			= 1;
					pLoginOut->_ncCoin			= 0;
					pLoginOut->m_ui8ApplingAdult	= APPLING_ADULT_YES;
					i3mem::Copy( pLoginOut->_Id,	pData->_Id, NET_ID_SIZE );

					_LoginExtension( pData, pLoginOut );
				}
				else 
				{
					char strTemp[1024];
					char strInData[512]; 
					char strOutData[512]; 
					m_pHttpLogIn->GetPostArguments(strInData,512);
					i3mem::Copy(strOutData, m_pHttpLogIn->GetRequestResult(), 512);

					i3String::Format(strTemp, 1024, "HTTP PARSING ERROR. [IN]([ID = %s],[PW = %s],[IP = %s][%s]),[Out]([ID = %s],[Rv= %d][%s])", pData->_Id, pData->_Passwd, inet_ntoa( ConIp ), strInData, pLoginOut->_pLinkId, pLoginOut->_iResult, strOutData ); 
					g_pModuleLogFile->Write_Log_Login(m_Idx,strTemp ); 

					pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_HTTP;
				}
			}
			else 
			{// 일정횟수 이상 일어나면, Error처리해야함.
				m_nLoginConnectHTTPRetryCount++;

				// 현재 로그인 유저에 대해서는 실패 처리합니다.
				pLoginOut->_iResult = EVENT_ERROR_EVENT_LOG_IN_HTTP;

				char strTemp[1024];

				// Http ReConnect.
				{
					m_pHttpLogIn->DisConnect();

					if( !m_pHttpLogIn->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
					{	
						i3String::Format( strTemp, 1024, "Error : %s (code: %d) - HTTP Connect Fail!", m_pHttpLogIn->GetLastErrorString(), m_pHttpLogIn->GetLastErrorCode());
						g_pModuleLogFile->Write_Log_Login(m_Idx, strTemp, WARNING_AUTH);							
					}
				}
				
				i3String::Format(strTemp, 1024, "ReConnect HTTP Login. [RetryCount= %d]", m_nLoginConnectHTTPRetryCount); 
				g_pModuleLogFile->Write_Log_Login( m_Idx, strTemp, WARNING_AUTH); 
			}
		}
		break; 
	case TYPE_LOGIN_LOGOUT		: 
		pLoginOut->_iResult	= EVENT_ERROR_SUCCESS; 
		pLoginOut->_i64DbIdx	= pData->_i64DbIdx;

		m_pHttpBilling->ResetArguments();
		m_pHttpBilling->AddArgument( "id", pData->_Id );
		m_pHttpBilling->SetUpPostArguments();
		if( !m_pHttpBilling->SendRequestPost() )
		{
			m_nBillingConnectHTTPRetryCount++;

			char strTemp[1024];

			// Http ReConnect.
			{
				m_pHttpBilling->DisConnect();

				if( !m_pHttpBilling->Connect( g_pConfig->m_LoginHttpAddress, i3NetworkHTTP::REQUEST_METHOD_POST ))
				{	
					i3String::Format( strTemp, 1024, "Error : %s (code: %d) - HTTP Connect Fail!", m_pHttpBilling->GetLastErrorString(), m_pHttpBilling->GetLastErrorCode());
					g_pModuleLogFile->Write_Log_Login(m_Idx, strTemp, WARNING_AUTH);							
				}
			}
			
			i3String::Format(strTemp, 1024, "ReConnect HTTP LogOut. [RetryCount= %d]", m_nBillingConnectHTTPRetryCount); 
			g_pModuleLogFile->Write_Log_Login( m_Idx, strTemp, WARNING_AUTH); 
		}

		break; 
	case TYPE_LOGIN_CASH_CHECK	: 
		pLoginOut->_iResult	= EVENT_ERROR_SUCCESS; 
		pLoginOut->_i64DbIdx		= pData->_i64DbIdx;
		break; 
	}

	_PushReult( pLoginOut ); 
	return true;
}

#define STR_RV_CODE 32

BOOL CLoginWorker_Turkey::_ParsingLogInByTurkey( char * pszString, INT32 * iRv, UINT8* pUID)
{
	INT32 iCount = 0; 
	BOOL bUID = FALSE; 
	BOOL bRv  = FALSE; 

	INT32 iUidCount = 0; 
	INT32 iRvCount	= 0; 
	char pTemp[STR_RV_CODE]; 

	if(i3String::Length( pszString ) > 1000)return FALSE; 

	while(pszString[iCount] != NULL)
	{
		iCount++; 

		if(bUID)
		{
			if( pszString[iCount] == '<' )
			{
				pUID[iUidCount] = NULL; 
				bUID = FALSE; 
			}
			else 
			{
				pUID[iUidCount] = pszString[iCount];   
				iUidCount++; 
			}

			//버그 코드 
			if(iUidCount >= TURKEY_UID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bRv)
		{
			if( pszString[iCount] == '<' )
			{
				pTemp[iRvCount] = NULL; 
				bRv = FALSE; 
			}else
			{
				pTemp[iRvCount] = pszString[iCount];
				iRvCount++; 
			}

			//버그 코드 
			if(iRvCount >= STR_RV_CODE)
			{
				return FALSE; 
			}
		}		
		
		if( pszString[iCount] != '<' )
		{
			continue;
		}

		if( pszString[iCount+1] == 'U' &&
			pszString[iCount+2] == 'I' &&
			pszString[iCount+3] == 'D' &&
			pszString[iCount+4] == '>' )
		{
			bUID = TRUE; 
			iCount = iCount + 4;
		}

		//spt_rtn
		if( pszString[iCount+1] == 's' &&
			pszString[iCount+2] == 'p' &&
			pszString[iCount+3] == 't' &&
			pszString[iCount+4] == '_' &&
			pszString[iCount+5] == 'r' &&
			pszString[iCount+6] == 't' &&
			pszString[iCount+7] == 'n' &&
			pszString[iCount+8] == '>' )
		{
			bRv = TRUE; 
			iCount = iCount + 8;
		}
	}

	if(iUidCount == 0 || iRvCount == 0) return FALSE; 
 
	//strcpy
	switch( atoi(pTemp) ) // 원본
	{
	case 0	: *iRv = EVENT_ERROR_SUCCESS;							break;  //정상 로그인 
	case 1	: *iRv = EVENT_ERROR_EVENT_LOG_IN_ALEADY_LOGIN;			break;  //중복 로그인 
	case 2	: *iRv = EVENT_ERROR_EVENT_LOG_IN_ID_FAIL;				break;  //ID Fail 
	case 3	: *iRv = EVENT_ERROR_EVENT_LOG_IN_PW_FAIL;				break;  //Password Fail
	case 4	: *iRv = EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;		break;  //블럭 계정
	case 5	: *iRv = EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT;		break;  //삭제된 계정
	case 6	: *iRv = EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR;		break;  //email 인증 실패
	case 7	: *iRv = EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;		break;  //ip 블럭 유저
	default	: *iRv = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;				break;  //에러 
	}

	return TRUE; 
}	

