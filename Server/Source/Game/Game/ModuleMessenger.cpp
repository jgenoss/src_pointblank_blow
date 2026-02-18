#include "pch.h"
#include "ModuleMessenger.h"
#include "ModuleMessengerSocket.h"
// #include "ModuleLogFile.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CModuleMessenger, i3Thread );

CModuleMessenger * g_pModuleMessenger = NULL; 

/////////////////////////////////////////////////////////////
//MESSENGER
#define MESSENGER_MAX_SEND_IDX_COUNT			2000000000
#define MESSENGER_RECONNECT_TIME				10
#define MESSENGER_SERVER_HEART_BIT				60
#define MESSENGER_SERVER_HEART_BIT_REQ_TIME		120
#ifdef I3_DEBUG
#define MESSENGER_SEND_BUFFER					30
#else
#define MESSENGER_SEND_BUFFER					100
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MESSENGER_CHECKVALID_IDX		if( !g_pContextMain->m_bMessengerActive)return TRUE; \
	if( !m_pMessengerSocket->IsActive())return FALSE; \
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer; \
	switch( WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sMessengerMainMessage;			break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sMessengerAuthMessage;			break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sMessengerClanMessage;			break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sMessengerMessMessage;			break; \
	default								: pNetBuffer = &m_pMessengerUserMessage[WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > MESSENGER_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleMessenger WriteBuffer FULL" );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= MESSENGER_SEND_BUFFER )\
{\
	g_pLog->WriteLog( L"ModuleMessenger WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % MESSENGER_SEND_BUFFER;

CModuleMessenger::CModuleMessenger(void)
{
	m_bRunning					= FALSE;
	m_ui32ThreadCount			= 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Messenger 
	m_pMessengerSocket			= NULL;
	m_ui32MessengerConnectTime	= 0;
	m_pMessengerUserMessage		= NULL;

	i3mem::FillZero( &m_sMessengerMainMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerAuthMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerClanMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerMessMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
}

CModuleMessenger::~CModuleMessenger(void)
{
	OnDestroy();
}

INT32 CModuleMessenger::OnCreate(UINT32 ui32ThreadCount)
{
	if( FALSE == g_pContextMain->m_bMessengerActive)	return EVENT_ERROR_SUCCESS;

	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sMessengerMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * MESSENGER_SEND_BUFFER); 
	m_sMessengerAuthMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * MESSENGER_SEND_BUFFER); 
	m_sMessengerClanMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * MESSENGER_SEND_BUFFER); 
	m_sMessengerMessMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * MESSENGER_SEND_BUFFER); 

	for( UINT32 i = 0; i < MESSENGER_SEND_BUFFER ; i++ )
	{
		m_sMessengerMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sMessengerAuthMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sMessengerClanMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sMessengerMessMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
	}

	m_pMessengerUserMessage = (CONNECT_SERVER_NET_BUFFER*) i3MemAlloc(sizeof(CONNECT_SERVER_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0; i < m_ui32ThreadCount ; i++ )
	{
		m_pMessengerUserMessage[i]._ReadPos			= 0; 
		m_pMessengerUserMessage[i]._WritePos		= 0; 
		m_pMessengerUserMessage[i]._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * MESSENGER_SEND_BUFFER); 
		for(INT32 j = 0; j < MESSENGER_SEND_BUFFER; j++)
		{
			m_pMessengerUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Create Socket 	
	m_pMessengerSocket = CModuleMessengerSocket::NewObject();
	if( FALSE == m_pMessengerSocket->OnCreate(g_pContextMain->m_ui32MessengerServerIp, g_pContextMain->m_ui16MessengerServerPort) )		return SERVER_ERROR_MESSENGER;
		
	m_ui32MessengerConnectTime = i3ThreadTimer::GetServerTime();

	//Create Thread
	if( !i3Thread::Create("ModuleContextA_Messenger", 0, 4096, NULL, PRIORITY_HIGH) )											return SERVER_ERROR_MESSENGER;
	
	return EVENT_ERROR_SUCCESS;
}

void CModuleMessenger::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	if( FALSE == g_pContextMain->m_bMessengerActive )	return;

	I3_SAFE_RELEASE( m_pMessengerSocket );

	if( m_sMessengerMainMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < MESSENGER_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sMessengerMainMessage._ppNetMessage[i] ); 
		}
		I3MEM_SAFE_FREE(m_sMessengerMainMessage._ppNetMessage); 
	}
	if( m_sMessengerAuthMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < MESSENGER_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sMessengerAuthMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sMessengerAuthMessage._ppNetMessage); 
	}
	if( m_sMessengerClanMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < MESSENGER_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sMessengerClanMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sMessengerClanMessage._ppNetMessage); 
	}
	if( m_sMessengerMessMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < MESSENGER_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sMessengerMessMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sMessengerMessMessage._ppNetMessage); 
	}

	if( m_pMessengerUserMessage )
	{
		for( UINT32 i = 0; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pMessengerUserMessage[i]._ppNetMessage )
			{
				for( INT32 j = 0;j < MESSENGER_SEND_BUFFER; j++)
				{
					I3_SAFE_RELEASE(m_pMessengerUserMessage[i]._ppNetMessage[j] ); 			
				}
				I3MEM_SAFE_FREE(m_pMessengerUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pMessengerUserMessage );
	}

	return; 
}

UINT32 CModuleMessenger::OnRunning( void * pUserData)
{	
	g_pLog->AddLogThread( L"MDCTAMSS", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bRunning		= TRUE;

	while( m_bRunning )
	{
		if( 0 < _MessengerUpdate() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0;
}

INT32 CModuleMessenger::_MessengerConnect()
{
	if( m_ui32MessengerConnectTime + MESSENGER_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32MessengerConnectTime = i3ThreadTimer::GetServerTime();

	m_pMessengerSocket->OnDestroy(); 
	if( m_pMessengerSocket->OnCreate( g_pContextMain->m_ui32MessengerServerIp, g_pContextMain->m_ui16MessengerServerPort ) )
	{
		m_ui32MessengerConnectTime		= i3ThreadTimer::GetServerTime();
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pMessengerSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleMessenger::_MessengerUpdate(void)
{
	UINT32	i; 
	INT32	i32SendCount; 
	INT32	i32ReadIdx;
	INT32	i32SendCompleteCount = 0;

	if( FALSE == g_pContextMain->m_bMessengerActive )	return i32SendCompleteCount;

	//-------------------------------------------------------------------------------
	//1. Connect Check 
	if( FALSE == m_pMessengerSocket->IsConnect() )
	{
		INT32 i32Rv = _MessengerConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect MessengerServer 1" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect MessengerServer 1" );
		}
		return i32SendCompleteCount;
	}	

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pMessengerSocket->IsActive() )
	{
		//2-1 Main Thread 
		i32SendCount = m_sMessengerMainMessage._WritePos - m_sMessengerMainMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sMessengerMainMessage._ReadPos % MESSENGER_SEND_BUFFER; 
			m_pMessengerSocket->SendPacketMessage( m_sMessengerMainMessage._ppNetMessage[i32ReadIdx] ); 
			m_sMessengerMainMessage._ReadPos++; 
			i32SendCompleteCount++;
		}
		//2-2 Trans Thread 
		i32SendCount = m_sMessengerAuthMessage._WritePos - m_sMessengerAuthMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sMessengerAuthMessage._ReadPos % MESSENGER_SEND_BUFFER; 
			m_pMessengerSocket->SendPacketMessage( m_sMessengerAuthMessage._ppNetMessage[i32ReadIdx] );
			m_sMessengerAuthMessage._ReadPos++; 
			i32SendCompleteCount++;
		}

		//2-3 Clan Thread 
		i32SendCount = m_sMessengerClanMessage._WritePos - m_sMessengerClanMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sMessengerClanMessage._ReadPos % MESSENGER_SEND_BUFFER; 
			m_pMessengerSocket->SendPacketMessage( m_sMessengerClanMessage._ppNetMessage[i32ReadIdx] );
			m_sMessengerClanMessage._ReadPos++; 
			i32SendCompleteCount++;
		}

		//2-4 Messenger Thread 
		i32SendCount = m_sMessengerMessMessage._WritePos - m_sMessengerMessMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sMessengerMessMessage._ReadPos % MESSENGER_SEND_BUFFER; 
			m_pMessengerSocket->SendPacketMessage( m_sMessengerMessMessage._ppNetMessage[i32ReadIdx] );
			m_sMessengerMessMessage._ReadPos++; 
			i32SendCompleteCount++;
		}
		//2-5 User Thread
		for( i = 0; i < m_ui32ThreadCount; i++ )
		{
			CONNECT_SERVER_NET_BUFFER	* pUserMessage = &m_pMessengerUserMessage[i]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % MESSENGER_SEND_BUFFER; 
				m_pMessengerSocket->SendPacketMessage( pUserMessage->_ppNetMessage[i32ReadIdx]); 
				pUserMessage->_ReadPos++; 
				i32SendCompleteCount++; 
			}
		}

		//2-4 Check Send 패킷			
		if( (m_pMessengerSocket->m_ui32LastSendTime + MESSENGER_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
			m_pMessengerSocket->SendPacketMessage(&SendPacket);
			m_pMessengerSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pMessengerSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State	
	if( (m_pMessengerSocket->m_ui32LastRecvTime + MESSENGER_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		INT32 i32Rv = _MessengerConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect MessengerServer 2");
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect MessengerServer 2" );
		}
		return i32SendCompleteCount;
	}

	return i32SendCompleteCount; 
}

BOOL CModuleMessenger::MessengerSendNote(INT32 WorkIdx, CUserSession * pSession, UINT8 ui8nicknameSize, UINT8 ui8messageSize, TTCHAR * nickname, TTCHAR * message)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8nicknameSize,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(nickname,				sizeof(TTCHAR) * ui8nicknameSize);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,					sizeof(TTCHAR) * ui8messageSize);
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendNoteByUID(INT32 WorkIdx, CUserSession * pSession, T_UID i64UID, UINT8 ui8messageSize, TTCHAR * message)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64UID,					sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,					sizeof(TTCHAR) * ui8messageSize);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerCheckReadedNote(INT32 WorkIdx, CUserSession * pSession, UINT8 ui8readCount, INT32* pi32noteIdList)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8readCount,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pi32noteIdList,				sizeof(INT32) * ui8readCount);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerDeleteNote(INT32 WorkIdx, CUserSession * pSession, UINT8 ui8deleteCount, INT32* pi32noteIdList)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_DELETE_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8deleteCount,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pi32noteIdList,			sizeof(INT32) * ui8deleteCount);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerLogin(INT32 WorkIdx, CUserSession * pSession )
{	
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LOGIN_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->m_UserInfoBasic.m_strNick,		NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32ViewRank,				sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerLogout(INT32 WorkIdx, T_UID i64DbIdx)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LOGOUT_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64DbIdx, sizeof(T_UID));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendServerEnter(INT32 WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_ENTER_USER_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerLeaveUser(INT32 WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LEAVE_USER_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerChangeNickname(	INT32 WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();

		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,						sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,							sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoBasic.m_strNick,		NET_NICK_NAME_SIZE);
		pNetBuffer->_WritePos++;    
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerChangeRank(	INT32 WorkIdx, CUserSession * pSession )
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_RANK_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32ViewRank,				sizeof(UINT32));
		pNetBuffer->_WritePos++;    
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerAliveUser(INT32 WorkIdx, INT32 i32SessionIdx, T_UID i64UID, TTCHAR* NickName)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_ALIVE_USER_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32SessionIdx,	sizeof(INT32));	
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64UID,			sizeof(T_UID));	
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(NickName,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);

		pNetBuffer->_WritePos++;
	}

	return TRUE; 
}

BOOL CModuleMessenger::MessengerChangeColorNick(	INT32 WorkIdx, CUserSession * pSession )
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,						sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,							sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
		pNetBuffer->_WritePos++;    
	}
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendClanNote(INT32 WorkIdx, CUserSession * pSession, T_UID i64receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, TTCHAR * message, TTCHAR* strClanName )
{
	//INT32	ECode = EVENT_ERROR_SUCCESS;

	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		UINT8 clanNameSize = (UINT8)(i3String::Length(strClanName) + 1);
		clanNameSize = MIN(clanNameSize, NET_CLAN_NAME_SIZE);

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64receiverDbIdx,			sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(UINT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8isAskNote,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&clanNameSize,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strClanName,					sizeof(TTCHAR) * clanNameSize);

		if( 0 < ui8messageSize )
		{
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,					sizeof(TTCHAR) * ui8messageSize);
		}
		else
		{
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&message[0],				sizeof(TTCHAR));
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&message[1],				sizeof(TTCHAR) * message[0] );
		}

		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendClanGroupNote(INT32 WorkIdx, CUserSession * pSession, UINT32 ui32Count, T_UID* pi64receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, TTCHAR * message, TTCHAR* strClanName )
{
	//INT32	ECode = EVENT_ERROR_SUCCESS;

	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		ui32Count = MIN(ui32Count, MAX_MEMBER_COUNT);

		UINT8 clanNameSize = (UINT8)(i3String::Length(strClanName) + 1);
		clanNameSize = MIN(clanNameSize, NET_CLAN_NAME_SIZE);


		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Count		,		sizeof(UINT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pi64receiverDbIdx,		(UINT16)(sizeof(T_UID) * ui32Count));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,			sizeof(UINT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8isAskNote,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&clanNameSize,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strClanName,				sizeof(TTCHAR) * clanNameSize);		

		if( 0 < ui8messageSize )
		{
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,				sizeof(TTCHAR) * ui8messageSize);
		}
		else
		{
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&message[0],			sizeof(TTCHAR));
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&message[1],			sizeof(TTCHAR) * message[0] );
		}

		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

#ifdef USE_MESS_FIND
BOOL CModuleMessenger::MessengerFindUser(INT32 WorkIdx, CUserSession * pSession, TTCHAR* findNick)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_FIND_USER_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(findNick,				sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE;
}
#endif

BOOL CModuleMessenger::MessengerFriendAccept(INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Idx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendInsert(	INT32 WorkIdx, CUserSession * pSession, TTCHAR* strNickName )
{
	MESSENGER_CHECKVALID_IDX;

	UINT8 ui8Rank = (UINT8)pSession->m_ui32ViewRank;

	UINT8 ui8NickSize = (UINT8)i3String::Length( strNickName )+1;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_INSERT_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Rank,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui8FindUserRank,	sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui8FindUserNickColor,	sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NickSize,			sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strNickName,				sizeof( TTCHAR ) * ui8NickSize);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendInviteUser(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8FriendIdx)
{ 
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_INVITED_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID)); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8FriendIdx,			sizeof(UINT8)); 
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendDelete(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Idx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_DELETE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}
BOOL CModuleMessenger::MessengerSendFriendRoomEnter(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChIdx, UINT16 ui16RoomIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ChIdx,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui16RoomIdx,			sizeof( UINT16 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendRoomLeave(	INT32 WorkIdx, CUserSession * pSession )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));		
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendResetFriendInfo(INT32 WorkIdx, T_UID iDbIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&iDbIdx, sizeof(T_UID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendChangeNickname(INT32 WorkIdx, CUserSession * pSession, TTCHAR* nickname)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(nickname,				sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendKickUser(INT32 WorkIdx, CUserSession * pSession)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_KICKUSER_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(T_UID));
	pNetBuffer->_WritePos++;    

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendWhisper(INT32 WorkIdx, CUserSession * pSession, TTCHAR* receiverNick, INT16 i16messageSize, TTCHAR* message, bool bIsGMUser )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_SEND_WHISPER_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->GetNickName(), sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(receiverNick,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bIsGMUser,				sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i16messageSize,			sizeof(INT16));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,					sizeof(TTCHAR) * i16messageSize);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,		sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendWhisperByUID(INT32 WorkIdx, CUserSession * pSession, T_UID i64receiverUID, TTCHAR* receiverNick, INT16 i16messageSize, TTCHAR* message, bool bIsGMUser )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64receiverUID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->GetNickName(), sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(receiverNick,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bIsGMUser,				sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i16messageSize,			sizeof(INT16));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message,					sizeof(TTCHAR) * i16messageSize);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,		sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendChannelEnter(INT32 WorkIdx, CUserSession * pSession, T_UID iUID, INT32 nChannelIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_FRIEND_LOBBY_ENTER_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&nChannelIdx,			sizeof(INT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendChannelLeave(INT32 WorkIdx, CUserSession * pSession, T_UID iUID)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_FRIEND_LOBBY_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendBlockInsert(	INT32 WorkIdx, CUserSession * pSession,  UINT8 ui8Rank, char* strNickName, char* strComment )
{
	MESSENGER_CHECKVALID_IDX;

	UINT8 ui8NickSize	 = (UINT8)i3String::Length( strNickName )+1;
	UINT8 ui8CommentSize = (UINT8)i3String::Length( strComment )+1;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_BLOCK_INSERT_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID ));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Rank,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NickSize,			sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strNickName,				sizeof( char ) * ui8NickSize);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8CommentSize,			sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strComment,				sizeof( char ) * ui8CommentSize);
	
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendBlockDelete( INT32 WorkIdx, CUserSession * pSession, INT32 i32Idx )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_BLOCK_DELETE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32Idx,					sizeof( INT32 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendChangeState(	INT32 WorkIdx, CUserSession * pSession, FRIEND_STATE eState )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_CHANGE_STATE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&eState,					sizeof( FRIEND_STATE));
	pNetBuffer->_WritePos++;
	return TRUE; 
}
BOOL CModuleMessenger::MessengerSendNoteGiftNotice(INT32 WorkIdx, T_UID UID, UINT8 ui8messageSize, TTCHAR * pstrMessage, UINT8 ui8NickSize, TTCHAR *pSenderNick)
{
	if (g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_GIFT_NOTICE_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&UID, sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NickSize, sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSenderNick, sizeof(TTCHAR) * ui8NickSize);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize, sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrMessage, sizeof(TTCHAR) * ui8messageSize);
		pNetBuffer->_WritePos++;
	}
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendNoteUserGiftNotice(INT32 WorkIdx, CUserSession * pSession, T_UID i64UID, UINT8 ui8messageSize, TTCHAR * message)
{
	if (g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_USERGIFT_NOTICE_REQ);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID, sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64UID, sizeof(T_UID));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8messageSize, sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(message, sizeof(TTCHAR) * ui8messageSize);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}