#include "pch.h"
#include "LoginWorker_Vietnam.h"
#include "LoginBase.h"

#pragma comment(lib,"./Garena/libeay32MD.lib")

#define TOKEN_RSA_PUB_KEY_FILE_VIETNAM			"publickey.pem"

CLoginWorker_Vietnam::CLoginWorker_Vietnam()
{
}

CLoginWorker_Vietnam::~CLoginWorker_Vietnam()
{
	_Destroy();
}

BOOL CLoginWorker_Vietnam::_Create()
{
	char strKeyFile[MAX_PATH];
	i3String::Format( strKeyFile, MAX_PATH, "%s\\%s", g_strCCurrentPath, TOKEN_RSA_PUB_KEY_FILE_VIETNAM );

	if( false == m_OpensslRSA.Init( strKeyFile ) )					return FALSE;

	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[LOAD] CLoginWorker_Vietnam_Garena Create Success - Check Key File" ); 

	return TRUE;
}

void CLoginWorker_Vietnam::_Destroy()
{
}

BOOL CLoginWorker_Vietnam::_WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	switch( pInput->m_eCommandType )
	{
	case TYPE_LOGIN_ID		:
		{
			if( FALSE == _DecryptSessionKey( pInput, pOutput ) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Login Fail 2 Result : %x", pOutput->m_i32Result);

			}

			CLoginBase::_LoginExtension( pInput, pOutput );
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CLoginWorker_Vietnam::_DecryptSessionKey( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	const char* szToken = (const char *)pInput->m_pSessionKey;
	char bufToken[NET_ENCRYPTED_TOKEN_SIZE+1] = {0,};
	INT32 i32Len = base64_decode(szToken, bufToken);
	if(i32Len == 0)
	{
		//Invalid token format
		pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_PACKET;
		return FALSE;
	}

	char bufDecrypt[NET_ENCRYPTED_TOKEN_SIZE+1] = {0,};
	bool bRet = m_OpensslRSA.Decrypt(bufToken, i32Len, bufDecrypt);
	if(!bRet)
	{
		//Decrypt token failed!
		pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_PACKET;
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	UserTokInfoVietnam_t tokInfo;
	i3mem::Copy(&tokInfo, &bufDecrypt[0], sizeof(tokInfo));
	// big endian convert to little endian
	tokInfo.ui32Uid				= ntohl(tokInfo.ui32Uid);
	tokInfo.ui32Timestamp		= ntohl(tokInfo.ui32Timestamp);

	// 타임아웃 체크 tokInfo.Timestamp
	time_t UTCTime;
	time_t tokenTime;
	time(&UTCTime);
	tokenTime = tokInfo.ui32Timestamp;
	if( tokenTime + 30*60 <  UTCTime )		// 한번 생성된 토큰은 30분간 유효
	{
		pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;
		return FALSE;
	}

	pOutput->m_TUID = tokInfo.ui32Uid;
	if( 0 == pOutput->m_TUID )
	{
		pOutput->m_i32Result = EVENT_ERROR_EVENT_LOG_IN_UNKNOWN;
		return FALSE;
	}

	pOutput->m_ui16GcaType		= tokInfo.ui16GcaType;

	pOutput->m_i32Result		= EVENT_ERROR_SUCCESS;
	pOutput->m_ui32Birthday		= 19800101;
	i3String::Copy( pOutput->m_strID,			tokInfo.strUsername, NET_ID_SIZE );
	i3String::Copy( pOutput->m_strBillingID,	tokInfo.strUsername, BILLING_ID_SIZE );
	pOutput->m_strID[ NET_ID_SIZE-1 ]			= '\0';
	pOutput->m_strBillingID[ BILLING_ID_SIZE-1 ]	= '\0';
	pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_EXTER;
	pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
	pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
	i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE ); 

	return TRUE;
}