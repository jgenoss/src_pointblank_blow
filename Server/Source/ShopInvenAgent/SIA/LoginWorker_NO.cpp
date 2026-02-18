#include "pch.h"
#include "LoginWorker_NO.h"

CLoginWorker_NO::CLoginWorker_NO()
{
}

CLoginWorker_NO::~CLoginWorker_NO()
{
	OnDestroy();
}

BOOL CLoginWorker_NO::_WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	// 테스트용
	//Sleep((rand() % 1000) * 10);

	switch( pInput->m_eCommandType )
	{
	case TYPE_LOGIN_ID		:
		{
			pOutput->m_i32Result		= EVENT_ERROR_SUCCESS;
			pOutput->m_i32Cash			= 0;
			pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_INTER;
			pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
			pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
			i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE );

			// 2010년생(미성년)과 80년생(성인) 둘중에 하나 랜덤.
			if((UINT8)(rand()%2)) 		pOutput->m_ui32Birthday	= 20130101;
			else						pOutput->m_ui32Birthday	= 19800101;

			i3String::Copy( pOutput->m_strID,			pInput->m_strID, NET_ID_SIZE );
			i3String::Copy( pOutput->m_strBillingID,	pInput->m_strID, BILLING_ID_SIZE );
			pOutput->m_strID[ NET_ID_SIZE-1 ]			= '\0';
			pOutput->m_strBillingID[ BILLING_ID_SIZE-1 ]	= '\0';
			pOutput->m_TUID					= 0;

			CLoginBase::_LoginExtension( pInput, pOutput );
		}
		return TRUE;
	}

	return FALSE;
}