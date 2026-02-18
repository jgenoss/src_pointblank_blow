#include "pch.h"
#include "LoginWorker_Brazil.h"

CLoginWorker_Brazil::CLoginWorker_Brazil()
{
}

CLoginWorker_Brazil::~CLoginWorker_Brazil()
{
	OnDestroy();
}

BOOL CLoginWorker_Brazil::_WorkProcess(LOGIN_INPUT * pInput, LOGIN_OUTPUT * pOutput)
{
	pOutput->m_eCommandType			= pInput->m_eCommandType;		// LogIn	LogOut
	pOutput->m_i32TransServerIdx	= pInput->m_i32TransServerIdx;	// LogIn	LogOut
	pOutput->m_ui8PBVer				= pInput->m_ui8PBVer;			// LogIn	LogOut	
	pOutput->m_i32SessionIdx		= pInput->m_i32SessionIdx;		// LogIn	LogOut	
	pOutput->m_ui32AuthKey			= pInput->m_ui32AuthKey;		// LogIn	LogOut
	pOutput->m_ui32LoginTime		= GetTickCount();
	pOutput->m_ui8LinkThreadIdx		= pInput->m_ui8LinkThreadIdx;
	pOutput->m_strBillingID[0]		= '\0';
	//pOutput->m_ui8Country			= pInput->m_ui8Country;

	switch (pInput->m_eCommandType)
	{
		case TYPE_LOGIN_ID:
		{
			pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
			char strLogin[MAX_PATH];
			char szPWMD5[MAX_PATH];
			
			// MD5 Convert
			m_pHttp->ConvertMD5(szPWMD5, pInput->m_strPW);

			i3String::Format(strLogin, MAX_PATH, "%s?ID=%s&PW=%s", g_pConfig->m_astrLoginHttp[0], pInput->m_strID, szPWMD5);
			
			if (m_pHttp->SendRequestGet(strLogin))
			{
				m_pHttp->RecvRequest();

				if (_ParsingLogInByBrazil(m_pHttp->GetRequestResult(), pOutput))
				{
					pOutput->m_ui32Birthday		= 19800101;
					pOutput->m_i32Cash			= 0;
					pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_INTER;
					pOutput->m_i8LogOutSendType = TYPE_LOGOUT_ACK_NOT;
					pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
					pOutput->m_ui8PCCafe		= PC_NOT_CAFE;
					pOutput->m_ui8Country = pInput->m_ui8Country;

					i3mem::FillZero(pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE);
					i3String::Copy(pOutput->m_strID, pInput->m_strID, NET_ID_SIZE);

					_LoginExtension(pInput, pOutput);
				}
				else
				{
					// ID, Password 틀린 내용은 로그로 기록하지 않습니다.
					// 너무 많아요.. Jinsik.Kim
					if (pOutput->m_i32Result != EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT)
					{
						char strTemp[SERVER_STRING_COUNT];
						i3String::Format(strTemp, SERVER_STRING_COUNT, "HTTP PARSING ERROR. [IN]([ID = %s]),[Out]([Rv= %x])",
							pInput->m_strID, pOutput->m_i32Result);

						wchar_t wstrTemp[SERVER_STRING_COUNT];
						MultiByteToWideChar(CP_ACP, NULL, strTemp, -1, wstrTemp, SERVER_STRING_COUNT);

						g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, wstrTemp);

						//pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_HTTP;
					}
				}
			}
			else
			{// 일정횟수 이상 일어나면, Error처리해야함.
				ReConnect();
			}
		}
		break;
	}

	return TRUE;
}

BOOL CLoginWorker_Brazil::_ParsingLogInByBrazil(char * pstrString, LOGIN_OUTPUT * pReadData)
{
	if (pstrString == NULL || i3String::Length(pstrString) == 0 || i3String::Length(pstrString) > 512)
	{
		//에러 리턴 
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[_ParsingLogInByBrazil] Parameter is wrong");
		pReadData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_PACKET;
		return FALSE;
	}

	if (0 == i3String::CompareCase(pstrString, "true"))
	{
		pReadData->m_i32Result = EVENT_ERROR_SUCCESS;						// 로그인 성공
		return TRUE;
	}
	else if (0 == i3String::CompareCase(pstrString, "false"))
	{
		pReadData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;	// 로그인 실패
		return FALSE;
	}
	//else if (0 == i3String::CompareCase(pstrString, "flash"))
	//{
	//	pReadData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;	// 로그인 실패
	//	return FALSE;
	//}
	else
	{
		pReadData->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;			// 에러
		return FALSE;
	}
}