#include "pch.h"
#include "LoginWorker_ZPT.h"

CLoginWorker_ZPT::CLoginWorker_ZPT()
{
}

CLoginWorker_ZPT::~CLoginWorker_ZPT()
{
}

BOOL CLoginWorker_ZPT::_WorkProcess( LOGIN_INPUT* pInput, LOGIN_OUTPUT* pOutput )
{
	switch( pInput->m_eCommandType )
	{
	case TYPE_LOGIN_ID		:
		{
			char strID[ MAX_PATH ];
			char strPW[ MAX_PATH ];

			S2DB::ConvertString( strID, MAX_PATH, pInput->m_strID );
			S2DB::ConvertString( strPW, MAX_PATH, pInput->m_strPW );

			char strQuery[ MAX_PATH ];
			i3String::Format( strQuery, MAX_PATH, "EXECUTE usp_Account_sel N'%s', N'%s'", strID, strPW );
			if( FALSE == m_pDB->ExecuteSP( strQuery ) )
			{
				pOutput->m_i32Result	= EVENT_FAIL_ERROR;
				return TRUE;
			}

			INT32 i32Count = m_pDB->GetRecordCount();
			if( 0 >= i32Count )
			{
				pOutput->m_i32Result	= EVENT_ERROR_EVENT_LOG_IN_INVALID_ACCOUNT;
				return TRUE;
			}

			pOutput->m_i32Cash			= m_pDB->GetINT32Data( 0 );
			pOutput->m_i32Result		= EVENT_ERROR_SUCCESS;
			pOutput->m_ui8PCCafe		= PC_NOT_CAFE;
			pOutput->m_ui32Birthday		= 19800101;
			
			i3String::Copy( pOutput->m_strID,			pInput->m_strID, NET_ID_SIZE );
			i3String::Copy( pOutput->m_strBillingID,	pInput->m_strID, BILLING_ID_SIZE );
			pOutput->m_strID[ NET_ID_SIZE-1 ]			= '\0';
			pOutput->m_strBillingID[ BILLING_ID_SIZE-1 ]	= '\0';
			
			pOutput->m_i8UIDCreateType	= TYPE_UID_CREATE_INTER;
			pOutput->m_i8LogOutSendType	= TYPE_LOGOUT_ACK_NOT;
			pOutput->m_ui8LocaleCode	= LOCALE_CODE_NONE;
			i3mem::FillZero( pOutput->m_aui8ShutDownTime, SHUTDOWN_TIME_SIZE ); 

			CLoginBase::_LoginExtension( pInput, pOutput );
		}
		return TRUE;
	}

	return FALSE;
}