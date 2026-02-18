#include "pch.h"
#include "ModuleEmail.h"
#include "LocalRingBuffer.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CModuleEmail, CSmtpMail );

CModuleEmail * g_pModuleEmail			= NULL; 

#define EMAIL_SEND_BUFFER_COUNT	500 // 메일 버퍼 갯수.

#define MAX_STRING_MAIL_INFO	50 // 메일 정보 최대값 

//------------------------------------------------------------------------------------------
CModuleEmail::CModuleEmail(void)
{
	m_pEMailSendBufferMain		= NULL;
}

CModuleEmail::~CModuleEmail(void)
{
	OnDestroy(); 
}

BOOL	CModuleEmail::OnCreate(void) 
{
	// 메일 전송 시스템 Off 상태면 강제 리턴 TRUE.
	if(  1 != g_pControl->m_stEmailSendSystem.m_ui32Active ) return TRUE;
	
	m_pEMailSendBufferMain = new Local_CRingBuffer;
	if(NULL == m_pEMailSendBufferMain ) 
	{
		g_pLog->WriteLog( L"[CModuleEmail::OnCreate] Create Fail EMailSendBufferMain NULL");
		return FALSE; 
	}
	if(FALSE == m_pEMailSendBufferMain->OnCreate( sizeof(EMAIL_SEND_BUFFER), EMAIL_SEND_BUFFER_COUNT ) )
	{
		g_pLog->WriteLog( L"[CModuleEmail::OnCreate] Create Fail EMailSendBufferMain");
		return FALSE; 
	}
	
	INT32 i32Rv =  CSmtpMail::OnCreate( &g_pControl->m_stEmailSendSystem.m_BaseEmailInfo);
	if( CREATE_SMTPEMAIL_SUCCESS != i32Rv)
	{
		switch ( i32Rv )
		{
		case CREATE_SMTPEMAIL_FAIL_BASE_INFO :				g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Smtp Base Info");		break;
		case CREATE_SMTPEMAIL_FAIL_SOCKET_CREATE :			g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Socket Create");			break;
		case CREATE_SMTPEMAIL_FAIL_SOCKET_CONNECT :			g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Socket Connect");		break;
		case CREATE_SMTPEMAIL_FAIL_SOCKET_SET_OPTION :		g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Socket SetOption");		break;
		case CREATE_SMTPEMAIL_FAIL_SEND_BUFFER_CREATE :		g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Send Buffer Create");	break;
		case CREATE_SMTPEMAIL_FAIL_THREAD_CREATE :			g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Thread Create");			break;
		default:											g_pLog->WriteLog( L"[CSmtpMail::OnCreate] Create Fail Unknown Reason");		break;
		}
		
		return FALSE;
	}

	m_i32SmtpMailIdx = g_pLog->AddLogThread( L"MAIL", 0, m_ThreadID );

	return TRUE;
}

void CModuleEmail::OnDestroy(void)
{
	if( NULL != m_pEMailSendBufferMain ) delete m_pEMailSendBufferMain;
}


bool CModuleEmail::OnRunningEx()
{
	EMAIL_SEND_BUFFER *	pEmailSendBuffer;

	bool	bWorked = false; 

	//-------------------------------------------------------------------------------
	// Main 쓰레드용 버퍼.
	if( m_pEMailSendBufferMain->GetBufferCount() > 0   && FALSE == m_bIsSendingEmail && i3ThreadTimer::GetServerTime() > ( m_ui32LastMailFinishTime + m_BaseEmailInfo.m_ui32EmailSendDelayTime )	)
	{
		// 버퍼에 보낼 데이터가 있고, 가장 최근 메일 종료시간 + m_ui32EmailSendDelayTime 이 지나고 난이후에 들어옵니다.
		pEmailSendBuffer = (EMAIL_SEND_BUFFER *)m_pEMailSendBufferMain->Pop(); 

		if( NULL == pEmailSendBuffer )
		{
			g_pLog->WriteLog( L"[CModuleEmail::OnRunningEx()] NULL == pEmailSendBuffer");
		}
		else
		{
			i3mem::Copy( &m_CurrenEmailSend, pEmailSendBuffer, sizeof( EMAIL_SEND_BUFFER ) );

			if( FALSE == SendEmailStart() )
			{
				g_pLog->WriteLog( L"[CModuleEmail::OnRunningEx] Fail SendEmailStart " );
			}
			bWorked = true;

		}

		m_pEMailSendBufferMain->PopIdx();
	}

	return bWorked;
}


void CModuleEmail::PushEmail( INT32 i32ThreadIdx,  SEND_REASON eSendReason, void* pEmilInfo1 )
{
	// 이메일 전송 시스템 Off이면 리턴.
	if(  1 != g_pControl->m_stEmailSendSystem.m_ui32Active ) return;

	char strSubject[MAX_EMAIL_SUBJECT];
	char strBody[MAX_EMAIL_BODY];

	// 메일 전송 사유에 따라 다른 방식으로 메일제목 및 내용을 만든다.
	switch( eSendReason ) 
	{
	case SEND_REASON_SERVER_UNNORMAL_DOWN :
	case SEND_REASON_SERVER_TIMEOUT :
		{
			if( NULL == pEmilInfo1 ) return;

			CSession* pSession = (CSession*)pEmilInfo1;

			ASC_CONNECT_TYPE eConnectType =  (ASC_CONNECT_TYPE)pSession->m_i32ConnectType ;
			ASC_SERVER_TYPE eServerType =  (ASC_SERVER_TYPE)pSession->m_i32ServerType;

			// 관제툴, 업데이트서버, 빌링서버면 리턴.
			if( ASC_CONNECT_TYPE_ADMIN == eConnectType || ASC_SERVER_UPDATER == eServerType ) return;

			// 전달받은 pEmailInfo 를 가지고 메일 제목,내용을 만든다.
			char strSendReason[MAX_STRING_MAIL_INFO];
			char strServerType[MAX_STRING_MAIL_INFO];
			

			// 메일 전송 사유
			switch( eSendReason ) 
			{
			case SEND_REASON_SERVER_UNNORMAL_DOWN : i3String::Copy( strSendReason, "DOWN", MAX_STRING_MAIL_INFO ) ; break;
			case SEND_REASON_SERVER_TIMEOUT :		i3String::Copy( strSendReason, "TIMEOUT", MAX_STRING_MAIL_INFO ) ; break;
			}

			// 서버 종류
			switch( eServerType ) 
			{
			case ASC_SERVER_TRANS :					i3String::Copy( strServerType, "TRANS", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_CLAN :					i3String::Copy( strServerType, "CLAN", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_MESSENGER :				i3String::Copy( strServerType, "MESSENGER", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_BATTLE :				i3String::Copy( strServerType, "BATTLE", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_UPDATER :				i3String::Copy( strServerType, "UPDATER", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_CONTROL :				i3String::Copy( strServerType, "CONTROL", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_ZLOG :					i3String::Copy( strServerType, "ZLOG", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_GATEWAY :				i3String::Copy( strServerType, "GATEWAY", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_SIA :					i3String::Copy( strServerType, "SIA", MAX_STRING_MAIL_INFO ) ; break;
			case ASC_SERVER_GAME :					i3String::Copy( strServerType, "GAME", MAX_STRING_MAIL_INFO ) ; break;
			default:								i3String::Copy( strServerType, "UNKNOWN", MAX_STRING_MAIL_INFO ) ; break;
			}

			// 메일 제목 생성
			i3String::Format( strSubject, MAX_EMAIL_SUBJECT, "PB_%s_SERVER_%s %s", strServerType, strSendReason, pSession->m_strIpString );

			// 메일 내용 생성
			i3String::Format( strBody, MAX_EMAIL_BODY, "ServerType : %s \r\n Issue : %s \r\n Server Ip : %s ", strServerType, strSendReason,pSession->m_strIpString );
		}
		break;
	default:	return;
	}


	EMAIL_SEND_BUFFER * pEmailSendBuffer = NULL; 
	Local_CRingBuffer * pRingBuffer; 

	/*switch( i32ThreadIdx )
	{
	case THREAD_IDX_MAIN :  pRingBuffer = m_pEMailSendBufferMain; break;
	default: return;
	}
*/
	if(i32ThreadIdx == g_i32MainLogIdx)
	{
		pRingBuffer = m_pEMailSendBufferMain;
	}
	else
	{
		return;
	}

	pEmailSendBuffer = (EMAIL_SEND_BUFFER*)pRingBuffer->PushPointer();
	if( NULL == pEmailSendBuffer ) return;

	i3String::Copy( pEmailSendBuffer->m_strSubject, strSubject, MAX_EMAIL_SUBJECT ) ;
	i3String::Copy( pEmailSendBuffer->m_strBody, strBody, MAX_EMAIL_BODY ) ;

	pRingBuffer->PushPointerIdx();

#ifdef I3_DEBUG
	g_pLog->WriteLog( L"[CModuleEmail::PushEmail()] PushEmail()"); 
#endif
	return; 
}