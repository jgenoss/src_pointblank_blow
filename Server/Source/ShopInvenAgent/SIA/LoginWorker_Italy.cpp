#include "pch.h"
#include "LoginWorker_Italy.h"

CLoginWorker_Italy::CLoginWorker_Italy()
{
} 
 
CLoginWorker_Italy::~CLoginWorker_Italy()
{
	 
} 

BOOL CLoginWorker_Italy::_WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	switch( pInput->m_eCommandType )
	{
	case TYPE_LOGIN_ID			: 
		{
			char strID[CONVERT_STRING_COUNT];
			char strPW[CONVERT_STRING_COUNT];
			S2DB::ConvertString(strID, CONVERT_STRING_COUNT, pInput->m_strID);		// 아이디에 ' 체크후 변환
			S2DB::ConvertString(strPW, CONVERT_STRING_COUNT, pInput->m_strPW);		// 패스워드에 ' 체크후 변환

			char* strTempIP;
			struct in_addr ip_addr;
			char strIP[CONVERT_STRING_COUNT];

			ip_addr.s_addr = htonl(pInput->m_ui32IP);
			strTempIP = inet_ntoa(ip_addr);
			S2DB::ConvertString(strIP, CONVERT_STRING_COUNT, strTempIP );		// IP 주소 변환

			
			char	strQuery[MAX_PATH];
			i3String::Format( strQuery, MAX_PATH, "EXECUTE usp_Account_check_Login '%s', '%s', 2, '%s' ", strID, strPW, strIP );

			wchar_t wstrQuery[MAX_PATH];
			MultiByteToWideChar(CP_ACP, NULL, strQuery, -1, wstrQuery, MAX_PATH);

			if( FALSE == m_pDB->ExecuteSP(wstrQuery) )
			{
				ReConnect(wstrQuery);
				pOutput->m_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
				return TRUE;
			}

			
			INT32 i32Count = m_pDB->GetRecordCount();
			if( i32Count < 1)
			{
				// 확인용 추후 제거.
				//g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Login Fail Italy Record Count is 0 / ID : %s / PW : %s ", strID, strPW);

				pOutput->m_i32Result	= EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;
				return TRUE;
			}
			 
			LOGIN_RETURN_VALUE_BY_ITALY ReturnValue;
			_ParsingLogInByItaly( &ReturnValue);
			pOutput->m_i32Result	= ReturnValue.i32ReturnCode; 

			 
			if( EV_SUCCESSED( pOutput->m_i32Result ) )
			{
				pOutput->m_TUID				= (INT64)ReturnValue.ui32UniqueID; // ReturnValue.iGameIdx;
				pOutput->m_ui32Birthday		= 19800101;
				pOutput->m_i32Cash			= 0;
				pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_EXTER;
				pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
				pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE; 
				pOutput->m_ui8PCCafe		= PC_NOT_CAFE; 
				
				i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE ); 
				i3String::Copy( pOutput->m_strID, pInput->m_strID, NET_ID_SIZE );

				CLoginBase::_LoginExtension( pInput, pOutput );
			}			 			
		}
		return TRUE;
	}
	

	return FALSE;  
}

void CLoginWorker_Italy::_ParsingLogInByItaly( LOGIN_RETURN_VALUE_BY_ITALY * ReturnValue )
{
	INT16 i16Result;
	i16Result						= m_pDB->GetINT16Data(0); 
	ReturnValue->ui32UniqueID		= m_pDB->GetUINT32Data(1); 
	//ReturnValue->ui32Gender			= m_pDB->GetUINT8Data(2); 

	switch( i16Result )
	{
	case 0 : // 정상
		ReturnValue->i32ReturnCode = EVENT_ERROR_SUCCESS;
		break;
	case -1 : // 계정 존재 안함
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_ID_FAIL;
		break;
	case -2 : // 암호 불일치
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_PW_FAIL;
		break;
	case -3 : // 탈퇴/탈퇴대기 유저
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_DELETE_ACCOUNT;
		break;
	case -4 : // 이메일 미인증 유저
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_EMAIL_AUTH_ERROR;
		break;
	case -5 : // 제재 유저
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_BLOCK_ACCOUNT;
		break;
	default : //
		ReturnValue->i32ReturnCode = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
		break;
	}
}	
 
 