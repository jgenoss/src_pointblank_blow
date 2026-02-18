#include "pch.h"
#include "LoginWorker_NorthA.h"

CLoginWorker_NorthA::CLoginWorker_NorthA()
{
}

CLoginWorker_NorthA::~CLoginWorker_NorthA()
{
	OnDestroy();
}

BOOL CLoginWorker_NorthA::_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput )
{
	pOutput->m_eCommandType			= pInput->m_eCommandType;		// LogIn	LogOut
	pOutput->m_i32TransServerIdx	= pInput->m_i32TransServerIdx;	// LogIn	LogOut
	pOutput->m_ui8PBVer				= pInput->m_ui8PBVer;			// LogIn	LogOut	
	pOutput->m_i32SessionIdx		= pInput->m_i32SessionIdx;		// LogIn	LogOut	
	pOutput->m_ui32AuthKey			= pInput->m_ui32AuthKey;		// LogIn	LogOut
	pOutput->m_ui32LoginTime		= GetTickCount();
	pOutput->m_ui8LinkThreadIdx		= pInput->m_ui8LinkThreadIdx;
	pOutput->m_strBillingID[0]		= '\0';
	pOutput->m_ui8Country			= pInput->m_ui8Country;

	switch( pInput->m_eCommandType )
	{	
	case TYPE_LOGIN_ID			: 
		{
			struct in_addr	ConIp;
			pOutput->m_i32Result	= EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			ConIp.s_addr			= pInput->m_ui32IP;
			m_pHttp->ResetArguments();
			m_pHttp->AddArgument( "Id",		pInput->m_strID );
			m_pHttp->AddArgument( "Pwd",	pInput->m_strPW );
			m_pHttp->AddArgument( "IP",		inet_ntoa( ConIp ));
			m_pHttp->SetUpPostArguments();

			if( m_pHttp->SendRequestPost() )
			{
				m_pHttp->RecvRequest();

				if( _ParsingLogInByNorth( m_pHttp->GetRequestResult(), pOutput ) )
				{
					pOutput->m_ui32Birthday		= 19800101;
					pOutput->m_i32Cash			= 0;
					pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_EXTER;
					pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
					pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
					i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE ); 
					i3String::Copy( pOutput->m_strID, pInput->m_strID, NET_ID_SIZE );

					_LoginExtension( pInput, pOutput );
				}
				else
				{
					char strTemp[SERVER_STRING_COUNT];
					i3String::Format(strTemp, SERVER_STRING_COUNT, "HTTP PARSING ERROR. [IN]([ID = %s],[IP = %s]),[Out]([Rv= %x])", pInput->m_strID, inet_ntoa(ConIp), pOutput->m_i32Result);

					wchar_t wstrTemp[SERVER_STRING_COUNT];
					MultiByteToWideChar(CP_ACP, NULL, strTemp, -1, wstrTemp, SERVER_STRING_COUNT);

					g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, wstrTemp);

					pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;
				}
			}
			else 
			{// 일정횟수 이상 일어나면, Error처리해야함.
				ReConnect();
			}
		}
		break;
	}

	return true; 
}

#define STR_RV_CODE 32

BOOL CLoginWorker_NorthA::_ParsingLogInByNorth( char * pszString, LOGIN_OUTPUT * pRecvData )
{
	INT32 iCount = 0; 
	BOOL bUID = FALSE; 
	BOOL bRv  = FALSE; 
	BOOL bTempKey = FALSE; 
	
	INT32 iUidCount = 0; 
	INT32 iTempKey = 0; 
	INT32 iRvCount	= 0; 
	char pTemp[STR_RV_CODE]; 

	char LinkId[50];

	while(pszString[iCount] != NULL)
	{
		iCount++; 

		if(bUID)
		{
			if( pszString[iCount] == '<' )
			{
				LinkId[iUidCount] = NULL; 
				bUID = FALSE; 
			}
			else 
			{
				LinkId[iUidCount] = pszString[iCount];   
				iUidCount++; 
			}

			//버그 코드 
			if(iUidCount >= TURKEY_UID_SIZE)
			{
				return FALSE; 
			}
		}

		if(bTempKey)
		{
			if( pszString[iCount] == '<' )
			{
				pRecvData->m_strBillingID[iTempKey] = NULL;
				bTempKey = FALSE; 
			}
			else 
			{
				pRecvData->m_strBillingID[iTempKey] = pszString[iCount];
				iTempKey++; 
			}

			if(iTempKey >= BILLING_ID_SIZE)
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

		if( pszString[iCount+1] == 't' &&
			pszString[iCount+2] == 'e' &&
			pszString[iCount+3] == 'm' &&
			pszString[iCount+4] == 'p' &&
			pszString[iCount+5] == 'K' &&
			pszString[iCount+6] == 'e' &&
			pszString[iCount+7] == 'y' &&
			pszString[iCount+8] == '>' )
		{
			bTempKey = TRUE; 
			iCount = iCount + 8;
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

	if(iRvCount == 0) return FALSE; 
 
	//i3String::Copy
	switch( atoi(pTemp) ) // 원본
	{
	case 0		: pRecvData->m_i32Result	 = EVENT_ERROR_SUCCESS;							break;  //정상 로그인 
	case -1		: pRecvData->m_i32Result	 = EVENT_ERROR_EVENT_LOG_IN_ID_FAIL;			break;  //존재하지않는 ID
	case -2		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_PW_FAIL;				break;  //비밀번호 불일치 
	case -3		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT;		break;  //삭제된 계정
	case -99		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;				break;  //로그인 페이지 오류	
	default		: pRecvData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;				break;  //에러 
	}

	i3String::Copy( pRecvData->m_strBillingID, LinkId, BILLING_ID_SIZE );
	pRecvData->m_TUID				= _atoi64(LinkId);

	return TRUE; 
}