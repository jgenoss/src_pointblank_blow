#include "pch.h"
#include "LoginHTTP.h"
#include "LoginWorker_Russia.h"
#include "../Innova/errors.h"

CLoginWorker_Russia::CLoginWorker_Russia()
{
}

CLoginWorker_Russia::~CLoginWorker_Russia()
{
	OnDestroy();
}

BOOL CLoginWorker_Russia::_WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	switch( pInput->m_eCommandType )
	{
	case TYPE_LOGIN_ID: 
		{
			pOutput->m_TUID	 = 0; 
			innbill_login_result2_a ret;
			struct in_addr ConIp;
			ConIp.s_addr = pInput->m_ui32IP;
			
			INT32 retunValue = g_pInte_ILogin2A(pInput->m_strID, pInput->m_strPW, 0, inet_ntoa(ConIp), 0, &ret);

			if( retunValue == innbill::INTEGRATION_API_DB_SUCCESS )
			{
				//Extra Data 처리 
				pOutput->m_i32Result		= EVENT_ERROR_SUCCESS;
				pOutput->m_ui32Birthday		= 19800101;
				pOutput->m_ui8PCCafe		= PC_NOT_CAFE;				//
				pOutput->m_i32Cash			= (INT32)ret.user_balance;	//
				pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_INTER;
				pOutput->m_i8LogOutSendType = TYPE_LOGOUT_ACK_NOT;
				pOutput->m_ui64BillingIdx	= ret.user_id;		// 러시아 Billing DB에서 사용하는 UID
				pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
				i3mem::Copy(pOutput->m_strID, pInput->m_strID, NET_ID_SIZE);
			}
			else
			{
				// 로그인 실패
				switch(retunValue)
				{
				case innbill::INTEGRATION_API_INVALID_LOGIN:	
				case innbill::INTEGRATION_API_INVALID_PASSWORD:		
				case innbill::INTEGRATION_API_INVALID_USER_ID:		
					pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;
					break;		
				case innbill::INTEGRATION_API_SERVICE_ACCOUNT_BANNED:
					pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;
					break;
				case innbill::INTEGRATION_API_DB_CONNECTION_FAILED:
				case innbill::INTEGRATION_API_DB_STATEMENT_FAILED:	
				case innbill::INTEGRATION_API_DB_EXECUTE_FAILED:	
				case innbill::INTEGRATION_API_DB_FETCH_FAILED:		
				case innbill::INTEGRATION_API_INVALID_INPUT_ARG:	
				default:
					pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
					break;
				}

				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"Login Fail Reason : %s", g_pInte_PErrorDescriptionW(retunValue) );
			}
		}
		break;
	default: 
		pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
		break; 
	}
	return true; 
}
