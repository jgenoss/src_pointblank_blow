#include "pch.h"
#include "ModuleMessenger.h"
#include "ModuleMessengerSocket.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CModuleMessenger, i3Thread );

CModuleMessenger * g_pModuleMessenger = NULL; 

/////////////////////////////////////////////////////////////
//MESSENGER
#define MESSENGER_MAX_SEND_IDX_COUNT			2000000000
#define MESSENGER_RECONNECT_TIME				10
#define MESSENGER_SERVER_HEART_BIT				60
#define MESSENGER_SERVER_HEART_BIT_REQ_TIME		120
#define MESSENGER_SEND_BUFFER					100
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MESSENGER_CHECKVALID_IDX		if( !g_pContextMain->m_bMessengerActive)return TRUE; \
	if( !m_pMessengerSocket->IsActive())return FALSE; \
	SERVER_NET_BUFFER	* pNetBuffer; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sMessengerMainMessage;			break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sMessengerAuthMessage;			break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sMessengerClanMessage;			break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sMessengerMessMessage;			break; \
	default								: pNetBuffer = &m_pMessengerUserMessage[i32WorkIdx];break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > MESSENGER_MAX_SEND_IDX_COUNT )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL, "ModuleMessenger WriteBuffer FULL", WARNING_NETWORK );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= MESSENGER_SEND_BUFFER )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleMessenger WriteBuffer Overflow", WARNING_NETWORK );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % MESSENGER_SEND_BUFFER;

CModuleMessenger::CModuleMessenger(void)
{
	m_bRunning					= FALSE;
	m_ui32ThreadCount				= 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Messenger 
	m_pMessengerSocket			= NULL;
	m_ui32MessengerConnectTime		= 0;
	m_pMessengerUserMessage		= NULL;

	i3mem::FillZero( &m_sMessengerMainMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerAuthMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerClanMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sMessengerMessMessage,	sizeof( SERVER_NET_BUFFER ) );
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

	m_pMessengerUserMessage = (SERVER_NET_BUFFER*) i3MemAlloc(sizeof(SERVER_NET_BUFFER) * m_ui32ThreadCount); 
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
	if( FALSE == m_pMessengerSocket->OnCreate(g_pContextMain->m_strMessengerServerIp, g_pContextMain->m_ui16MessengerServerPort) )		return SERVER_ERROR_MESSENGER;
		
	m_ui32MessengerConnectTime				= g_pContextMain->GetServerTime();

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
	if( m_ui32MessengerConnectTime + MESSENGER_RECONNECT_TIME > g_pContextMain->GetServerTime() )	return 1;
	m_ui32MessengerConnectTime = g_pContextMain->GetServerTime();

	m_pMessengerSocket->OnDestroy(); 
	if( m_pMessengerSocket->OnCreate( g_pContextMain->m_strMessengerServerIp, g_pContextMain->m_ui16MessengerServerPort ) )
	{
		m_ui32MessengerConnectTime		= g_pContextMain->GetServerTime();
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
			g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX,NULL,"Success ReConnect MessengerServer 1", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX,NULL,"Fail ReConnect MessengerServer 1", WARNING_NETWORK );
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
			SERVER_NET_BUFFER	* pUserMessage = &m_pMessengerUserMessage[i]; 
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
		if( (m_pMessengerSocket->m_ui32LastSendTime + MESSENGER_SERVER_HEART_BIT) < g_pContextMain->GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
			m_pMessengerSocket->SendPacketMessage(&SendPacket);
			m_pMessengerSocket->m_ui32LastSendTime = g_pContextMain->GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pMessengerSocket->SelectEvent(); 		

	//-------------------------------------------------------------------------------
	//4. Check Connect State	
	if( (m_pMessengerSocket->m_ui32LastRecvTime + MESSENGER_SERVER_HEART_BIT_REQ_TIME) < g_pContextMain->GetServerTime() )
	{
		INT32 i32Rv = _MessengerConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX,NULL,"Success ReConnect MessengerServer 2", WARNING_NETWORK);
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_MESSENGER_THREAD_IDX,NULL,"Fail ReConnect MessengerServer 2", WARNING_NETWORK );
		}
		return i32SendCompleteCount;
	}

	return i32SendCompleteCount; 
}

BOOL CModuleMessenger::MessengerSendNote(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8nicknameSize, UINT8 ui8messageSize, char * pstrnickname, char * pstrmessage)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8nicknameSize,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrnickname,				ui8nicknameSize);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,				ui8messageSize);
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendNoteByUID(INT32 i32WorkIdx, CUserSession * pSession, INT64 i64UID, UINT8 ui8messageSize, char * pstrmessage)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64UID,					sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));		
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,				ui8messageSize);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerCheckReadedNote(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8readCount, INT32* pi32noteIdList)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8readCount,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pi32noteIdList,			sizeof(INT32) * ui8readCount);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerDeleteNote(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8deleteCount, INT32* pi32noteIdList)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_NOTE_DELETE_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8deleteCount,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pi32noteIdList,			sizeof(INT32) * ui8deleteCount);
		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerLogin(INT32 i32WorkIdx, CUserSession * pSession )
{	
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LOGIN_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_UserInfoBasic._nick,		NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._rank,		sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerLogout(INT32 i32WorkIdx, INT64 i64DbIdx)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LOGOUT_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64DbIdx, sizeof(INT64));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendServerEnter(INT32 i32WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_ENTER_USER_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerLeaveUser(INT32 i32WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_LEAVE_USER_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_WritePos++;
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerChangeNickname(	INT32 i32WorkIdx, CUserSession * pSession)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,				sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._nick, NET_NICK_NAME_SIZE);
		pNetBuffer->_WritePos++;    
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerChangeRank(	INT32 i32WorkIdx, CUserSession * pSession )
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_RANK_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._rank,		sizeof(UINT32));
		pNetBuffer->_WritePos++;    
	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerAliveUser(INT32 i32WorkIdx, INT32 i32SessionIdx, INT64 i64UID, char* pstrNickName)
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ALIVE_USER_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32SessionIdx,	sizeof(INT32));	
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64UID,			sizeof(INT64));	
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,		NET_NICK_NAME_SIZE);

		pNetBuffer->_WritePos++;
	}

	return TRUE; 
}

BOOL CModuleMessenger::MessengerChangeColorNick(	INT32 i32WorkIdx, CUserSession * pSession )
{
	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._nickcolor,	sizeof(UINT8));
		pNetBuffer->_WritePos++;    
	}
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendClanNote(INT32 i32WorkIdx, CUserSession * pSession, INT64 i64receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, char * pstrmessage, char* pstrClanName )
{
	//INT32	ECode = EVENT_ERROR_SUCCESS;

	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		UINT8 clanNameSize = (UINT8)(i3String::Length(pstrClanName) + 1);
		clanNameSize = MIN(clanNameSize, NET_CLAN_NAME_SIZE);

		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64receiverDbIdx,			sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,					sizeof(UINT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8isAskNote,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&clanNameSize,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8messageSize,				sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrClanName,					clanNameSize);

		if( 0 < ui8messageSize )
		{
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,					ui8messageSize);
		}
		else
		{
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pstrmessage[0],				sizeof(char));
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pstrmessage[1],				pstrmessage[0] );
		}

		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendClanGroupNote(INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Count, INT64* pi64receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, char * pstrmessage, char* pstrClanName )
{
	//INT32	ECode = EVENT_ERROR_SUCCESS;

	if(g_pContextMain->m_bMessengerActive)
	{
		MESSENGER_CHECKVALID_IDX;

		ui32Count = MIN(ui32Count, MAX_MEMBER_COUNT);

		UINT8 ui8clanNameSize = (UINT8)(i3String::Length(pstrClanName) + 1);
		ui8clanNameSize = MIN(ui8clanNameSize, NET_CLAN_NAME_SIZE);


		pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
		pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Count,				sizeof(UINT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pi64receiverDbIdx,		(UINT16)(sizeof(INT64) * ui32Count));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(UINT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8isAskNote,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8clanNameSize,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8messageSize,			sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrClanName,				ui8clanNameSize);		

		if( 0 < ui8messageSize )
		{
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,				ui8messageSize);
		}
		else
		{
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pstrmessage[0],			sizeof(char));
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pstrmessage[1],			pstrmessage[0] );
		}

		pNetBuffer->_WritePos++;

	}

	return TRUE;
}

#ifdef USE_MESS_FIND
BOOL CModuleMessenger::MessengerFindUser(INT32 i32WorkIdx, CUserSession * pSession, char* pstrfindNick)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_FIND_USER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrfindNick,				NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE;
}
#endif

BOOL CModuleMessenger::MessengerFriendAccept(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Idx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendInsert(	INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName )
{
	MESSENGER_CHECKVALID_IDX;

	UINT8 ui8NickSize = (UINT8)i3String::Length( pstrNickName )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_INSERT_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64 ));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				ui8NickSize);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendInviteUser(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8FriendIdx)
{ 
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_INVITED_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64)); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8FriendIdx,			sizeof(UINT8)); 
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendDelete(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Idx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_DELETE_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}
BOOL CModuleMessenger::MessengerSendFriendRoomEnter(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChIdx, UINT16 ui16RoomIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ChIdx,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui16RoomIdx,				sizeof( UINT16 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendFriendRoomLeave(	INT32 i32WorkIdx, CUserSession * pSession )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64));		
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendResetFriendInfo(INT32 i32WorkIdx, INT64 i64DbIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64DbIdx,	sizeof(INT64));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendChangeNickname(INT32 i32WorkIdx, CUserSession * pSession, char* pstrnickname)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrnickname,				NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendKickUser(INT32 i32WorkIdx, CUserSession * pSession)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_KICKUSER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_WritePos++;    

	return TRUE;
}

BOOL CModuleMessenger::MessengerSendWhisper(INT32 i32WorkIdx, CUserSession * pSession, char* pstrreceiverNick, INT16 i16messageSize, char* pstrmessage)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_SEND_WHISPER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->GetNickName(),	NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrreceiverNick,			NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i16messageSize,			sizeof(INT16));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,				i16messageSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleMessenger::MessengerSendWhisperByUID(INT32 i32WorkIdx, CUserSession * pSession, INT64 i64receiverUID, char* preceiverNick, INT16 i16messageSize, char* pstrmessage)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64receiverUID,			sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->GetNickName(),	NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(preceiverNick,			NET_NICK_NAME_SIZE);	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i16messageSize,			sizeof(INT16));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrmessage,				i16messageSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

#ifdef USE_UNDECIDED_DESIGN_TEST
BOOL CModuleMessenger::MessengerSendChannelEnter(INT32 i32WorkIdx, CUserSession * pSession, INT64 iUID, INT32 nChannelIdx)
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_MESSENGER_SEND_CHANNEL_NUM_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(INT64));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&nChannelIdx,				sizeof(INT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}
#endif

BOOL CModuleMessenger::MessengerSendBlockInsert(	INT32 i32WorkIdx, CUserSession * pSession,  UINT8 ui8Rank, char* pstrNickName, char* pstrComment )
{
	MESSENGER_CHECKVALID_IDX;

	UINT8 ui8NickSize	 = (UINT8)i3String::Length( pstrNickName )+1;
	UINT8 ui8CommentSize = (UINT8)i3String::Length( pstrComment )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_BLOCK_INSERT_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64 ));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Rank,					sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,				sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				sizeof( char ) * ui8NickSize);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8CommentSize,			sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrComment,				sizeof( char ) * ui8CommentSize);
	
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleMessenger::MessengerSendBlockDelete( INT32 i32WorkIdx, CUserSession * pSession, INT32 i32Idx )
{
	MESSENGER_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MESSENGER_BLOCK_DELETE_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( INT64));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32Idx,					sizeof( INT32 ));
	pNetBuffer->_WritePos++;
	return TRUE; 
}
