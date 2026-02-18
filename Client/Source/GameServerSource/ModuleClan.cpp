#include "pch.h"
#include "ModuleClan.h"
#include "ModuleClanSocket.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CModuleClan, i3Thread );

CModuleClan * g_pModuleClan = NULL; 

//////////////////////////////////////////////////////////////
//CLAN
#define CLAN_MAX_SEND_IDX_COUNT					2000000000
#define CLAN_RECONNECT_TIME						10
#define CLAN_SERVER_HEART_BIT					60
#define CLAN_SERVER_HEART_BIT_REQ_TIME			120
#define CLAN_SEND_BUFFER						100
#define CLAN_CONTEXT_LIST_TIMEOUT				5 * 60
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLAN_CHECKVALID_IDX		if( !g_pContextMain->m_bClanActive)return FALSE; \
	if( !m_pClanSocket->IsActive())return FALSE; \
	if( 0 == pSession->m_UserInfoBasic._clanidx )	return TRUE; \
	SERVER_NET_BUFFER	* pNetBuffer; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sClanMainMessage;				break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sClanAuthMessage;				break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sClanClanMessage;				break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sClanMessMessage;				break; \
	default								: pNetBuffer = &m_pClanUserMessage[i32WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CLAN_MAX_SEND_IDX_COUNT )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleClan Buffer FULL", WARNING_NETWORK );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CLAN_SEND_BUFFER )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleClan WriteBuffer Overflow", WARNING_NETWORK );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % CLAN_SEND_BUFFER;


#define CLAN_CHECKVALID_IDX_NOCLAN	if( !g_pContextMain->m_bClanActive)return FALSE; \
	if( !m_pClanSocket->IsActive())return FALSE; \
	SERVER_NET_BUFFER	* pNetBuffer; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sClanMainMessage;				break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sClanAuthMessage;				break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sClanClanMessage;				break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sClanMessMessage;				break; \
	default								: pNetBuffer = &m_pClanUserMessage[i32WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CLAN_MAX_SEND_IDX_COUNT )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleClan Buffer FULL", WARNING_NETWORK);\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CLAN_SEND_BUFFER )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleClan WriteBuffer Overflow", WARNING_NETWORK );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % CLAN_SEND_BUFFER;

CModuleClan::CModuleClan(void)
{
	m_bRunning					= FALSE;
	m_ui32ThreadCount			= 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Clan
	m_pClanSocket				= NULL;
	m_ui32ClanConnectTime		= 0; 
	m_pClanUserMessage			= NULL;

	i3mem::FillZero( &m_sClanMainMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanAuthMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanClanMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanMessMessage,	sizeof( SERVER_NET_BUFFER ) );
}

CModuleClan::~CModuleClan(void)
{
	OnDestroy();
}

INT32 CModuleClan::OnCreate(UINT32 ui32ThreadCount)
{
	if( FALSE == g_pContextMain->m_bClanActive )	return EVENT_ERROR_SUCCESS;

	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer

	// Buffer Create
	// Main
	m_sClanMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CLAN_SEND_BUFFER); 
	m_sClanAuthMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CLAN_SEND_BUFFER); 
	m_sClanClanMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CLAN_SEND_BUFFER); 
	m_sClanMessMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CLAN_SEND_BUFFER); 

	for( UINT32 i = 0 ; i < CLAN_SEND_BUFFER ; i++ )
	{
		m_sClanMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sClanAuthMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sClanClanMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_sClanMessMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
	}

	m_pClanUserMessage = (SERVER_NET_BUFFER*) i3MemAlloc(sizeof(SERVER_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pClanUserMessage[i]._ReadPos	= 0; 
		m_pClanUserMessage[i]._WritePos	= 0; 
		m_pClanUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CLAN_SEND_BUFFER); 
		for(INT32 j = 0; j < CLAN_SEND_BUFFER; j++)
		{
			m_pClanUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Create Socket 
	m_pClanSocket = CModuleClanSocket::NewObject(); 	
	if( FALSE == m_pClanSocket->OnCreate(g_pContextMain->m_strClanServerIp, g_pContextMain->m_ui16ClanServerPort) )	return SERVER_ERROR_CLAN;
		
	m_ui32ClanConnectTime			= g_pContextMain->GetServerTime();
	m_ui32ClanContextSendTime		= g_pContextMain->GetServerTime();

	//Create Thread
	if( !i3Thread::Create("ModuleContextA_Clan", 0, 4096, NULL, PRIORITY_HIGH) )								return SERVER_ERROR_CLAN;

	return EVENT_ERROR_SUCCESS; 
}

void CModuleClan::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	if( FALSE == g_pContextMain->m_bClanActive )	return;

	I3_SAFE_RELEASE( m_pClanSocket );

	if( m_sClanMainMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < CLAN_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sClanMainMessage._ppNetMessage[i] ); 
		}
		I3MEM_SAFE_FREE(m_sClanMainMessage._ppNetMessage); 
	}
	if( m_sClanAuthMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < CLAN_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sClanAuthMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sClanAuthMessage._ppNetMessage); 
	}
	if( m_sClanClanMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < CLAN_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sClanClanMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sClanClanMessage._ppNetMessage); 
	}
	if( m_sClanMessMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < CLAN_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sClanMessMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sClanMessMessage._ppNetMessage); 
	}

	if( m_pClanUserMessage )
	{
		for( UINT32 i = 0; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pClanUserMessage[i]._ppNetMessage )
			{
				for( INT32 j = 0;j < CLAN_SEND_BUFFER; j++)
				{
					I3_SAFE_RELEASE(m_pClanUserMessage[i]._ppNetMessage[j] ); 			
				}
				I3MEM_SAFE_FREE(m_pClanUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pClanUserMessage );
	}

	return; 
}

UINT32 CModuleClan::OnRunning( void * pUserData)
{	
	m_bRunning		= TRUE;

	while( m_bRunning )
	{
		if( 0 < _ClanUpdate() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0;
}

INT32 CModuleClan::_ClanConnect()
{
	if( m_ui32ClanConnectTime + CLAN_RECONNECT_TIME > g_pContextMain->GetServerTime() )	return 1;
	m_ui32ClanConnectTime = g_pContextMain->GetServerTime();

	m_pClanSocket->OnDestroy(); 
	if( m_pClanSocket->OnCreate(g_pContextMain->m_strClanServerIp, g_pContextMain->m_ui16ClanServerPort) )
	{
		m_ui32ClanContextSendTime		= g_pContextMain->GetServerTime();
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pClanSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleClan::_ClanUpdate(void)
{
	UINT32	i; 
	INT32	i32SendCount; 
	INT32	i32ReadIdx;
	INT32	i32SendCompleteCount = 0; 

	if( FALSE == g_pContextMain->m_bClanActive )	return i32SendCompleteCount;

	//-------------------------------------------------------------------------------
	//1-1. Connect Check
	if( FALSE == m_pClanSocket->IsConnect() )
	{	
		INT32 i32Rv = _ClanConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL,"Success ReConnect ClanServer 1", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL,"Fail ReConnect ClanServer 1", WARNING_NETWORK );
		}
		return i32SendCompleteCount;
	}	

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pClanSocket->IsActive() )
	{
		//2-1 Main Thread 
		i32SendCount = m_sClanMainMessage._WritePos - m_sClanMainMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sClanMainMessage._ReadPos % CLAN_SEND_BUFFER; 
			m_pClanSocket->SendPacketMessage( m_sClanMainMessage._ppNetMessage[i32ReadIdx] ); 
			m_sClanMainMessage._ReadPos++; 
			i32SendCompleteCount++;
		}

		//2-2 Trans Thread 
		i32SendCount = m_sClanAuthMessage._WritePos - m_sClanAuthMessage._ReadPos;
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sClanAuthMessage._ReadPos % CLAN_SEND_BUFFER;
			m_pClanSocket->SendPacketMessage( m_sClanAuthMessage._ppNetMessage[i32ReadIdx] );
			m_sClanAuthMessage._ReadPos++;
			i32SendCompleteCount++;
		}

		//2-3 Clan Thread 
		i32SendCount = m_sClanClanMessage._WritePos - m_sClanClanMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sClanClanMessage._ReadPos % CLAN_SEND_BUFFER; 
			m_pClanSocket->SendPacketMessage( m_sClanClanMessage._ppNetMessage[i32ReadIdx] );
			m_sClanClanMessage._ReadPos++;
			i32SendCompleteCount++;
		}

		//2-4 Messenger Thread 
		i32SendCount = m_sClanMessMessage._WritePos - m_sClanMessMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sClanMessMessage._ReadPos % CLAN_SEND_BUFFER; 
			m_pClanSocket->SendPacketMessage( m_sClanMessMessage._ppNetMessage[i32ReadIdx] );
			m_sClanMessMessage._ReadPos++; 
			i32SendCompleteCount++;
		}
		
		//2-5 User Thread
		for( i = 0 ; i < m_ui32ThreadCount ; i++ )
		{
			SERVER_NET_BUFFER	* pUserMessage = &m_pClanUserMessage[i]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % CLAN_SEND_BUFFER; 
				m_pClanSocket->SendPacketMessage( pUserMessage->_ppNetMessage[i32ReadIdx]); 
				pUserMessage->_ReadPos++; 
				i32SendCompleteCount++; 
			}
		}

		//2-4 Check Send 패킷			
		if( (m_pClanSocket->m_ui32LastSendTime + CLAN_SERVER_HEART_BIT) < g_pContextMain->GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
			m_pClanSocket->SendPacketMessage(&SendPacket);
			m_pClanSocket->m_ui32LastSendTime = g_pContextMain->GetServerTime();
		}

		if( m_pClanSocket->m_bNewClanCreate )
		{
			m_pClanSocket->m_bNewClanCreate = FALSE;
			i3NetworkPacket SendPacket(PROTOCOL_CS_CLAN_CONTEXT_REQ);
			m_pClanSocket->SendPacketMessage(&SendPacket);

			m_ui32ClanContextSendTime = g_pContextMain->GetServerTime();
		}

		//2_5 Chack Clan Version 
		if( (m_ui32ClanContextSendTime + CLAN_CONTEXT_LIST_TIMEOUT) < g_pContextMain->GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_CONTEXT_REQ );
			m_pClanSocket->SendPacketMessage(&SendPacket);

			m_ui32ClanContextSendTime = g_pContextMain->GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pClanSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pClanSocket->m_ui32LastRecvTime + CLAN_SERVER_HEART_BIT_REQ_TIME) < g_pContextMain->GetServerTime() )
	{
		INT32 i32Rv = _ClanConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL,"Success ReConnect ClanServer 2", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CLAN_THREAD_IDX,NULL,"Fail ReConnect ClanServer 2", WARNING_NETWORK );
		}
	}

	return i32SendCompleteCount; 
}

BOOL CModuleClan::ClanSendTestNCQA( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Message, UINT32 ui32Value, UINT32 ui32Value1, UINT32 ui32Value2 )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_TEST_NC_QA);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Message,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Value,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Value1,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Value2,				sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendLogIn( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_LOGIN_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));	
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendLogOut( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;

	UINT8 ui8Rank = (UINT8)pSession->m_UserInfoBasic._rank;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_LOGOUT_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Rank,								sizeof(UINT8));
	pNetBuffer->_WritePos++; 

	return TRUE; 
}

BOOL CModuleClan::ClanSendServerEnter( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_SERVER_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendServerLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_SERVER_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendChannelEnter( INT32 i32WorkIdx, CUserSession * pSession, INT32 nChannelIdx )
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_CHANEL_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&nChannelIdx,							sizeof(INT32));
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendChannelLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_CHANEL_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendRoomEnter( INT32 i32WorkIdx, CUserSession * pSession, INT32 nRoomIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_ROOM_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&nRoomIdx,							sizeof(INT32));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendRoomLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_ROOM_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendPageEnter( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_PAGE_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendPageLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_USER_PAGE_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendCreate( INT32 i32WorkIdx, CUserSession * pSession, char * pClanName, char * pClanIntro, char * pClanAzit, UINT32 ui32Mark )	//생성 
{
	UINT8 ui8UserNick	= (UINT8)i3String::Length( pSession->m_UserInfoBasic._nick )+1;
	UINT8 ui8ClanName	= (UINT8)i3String::Length( pClanName )+1;
	UINT8 ui8ClanIntro	= (UINT8)i3String::Length( pClanIntro )+1;
	UINT8 ui8ClanAzit	= (UINT8)i3String::Length( pClanAzit )+1;

	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CREATE_CLAN_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,				sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i32ChannelNum,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i32RoomIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Mark,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._rank,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8UserNick,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ClanName,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ClanIntro,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ClanAzit,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_UserInfoBasic._nick,	ui8UserNick);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pClanName,						ui8ClanName);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pClanIntro,						ui8ClanIntro);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pClanAzit,						ui8ClanAzit);
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendJoinRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanDBIdx, char * strContents, UINT8 ui8Age, UINT8 ui8Rank )//조인
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8Size		= (UINT8)i3String::Length( strContents )+1;
	UINT8 ui8NickSize	= (UINT8)i3String::Length( pSession->m_UserInfoBasic._nick )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_JOIN_REQUEST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,						sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32ClanDBIdx,							sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i32ChannelNum,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i32RoomIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Size,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( strContents,								ui8Size);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Age,									sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Rank,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8NickSize,							sizeof(UINT8));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pSession->m_UserInfoBasic._nick,			ui8NickSize);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord._killcount,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord._death,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord._match,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord._win,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord._lose,		sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendCancelRequest( INT32 i32WorkIdx, CUserSession * pSession )				//가입취소	
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_CS_CANCEL_REQUEST_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendDetailInfo(INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Type )						//클랜 상세정보 요청
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_DETAIL_INFO_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Type,					sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberContext(INT32 i32WorkIdx, CUserSession * pSession )						//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_CONTEXT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberList( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Section)		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_LIST_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Section,							sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberDetailInfo( INT32 i32WorkIdx, CUserSession * pSession, TUID ClanMemberUID)		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ClanMemberUID,						sizeof(TUID));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestContext(	INT32 i32WorkIdx, CUserSession * pSession )						//가입신청서 요청
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_CS_REQUEST_CONTEXT_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestList(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Section )		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REQUEST_LIST_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Section,							sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestInfo(	INT32 i32WorkIdx, CUserSession * pSession, TUID i64RequestIdx )		//유저 상세정보 요청
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REQUEST_INFO_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64RequestIdx ,						sizeof(TUID));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendAcceptRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID * pList) //스탭만 권한이 있음
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_ACCEPT_REQUEST_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pList,								sizeof(TUID) * ui8Count );
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendDenialRequest(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID * pList) //스탭만 권한이 있음
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_DENIAL_REQUEST_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pList,								sizeof(TUID) * ui8Count );
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendInviteAcceptRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT32 ui32State, UINT8 ui8Accept )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_INVITE_ACCEPT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32State,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Accept,				sizeof(UINT8));
	if( 1 == ui8Accept )
	{
		UINT8 ui8NickSize = (UINT8)i3String::Length( pSession->m_UserInfoBasic._nick ) + 1;
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._rank,	sizeof(UINT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,						sizeof(UINT8));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_UserInfoBasic._nick,	ui8NickSize);
	}
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendChatingRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CHATTING_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ChatSize,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrChating,							ui8ChatSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendPageChatingRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_PAGE_CHATTING_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ChatSize,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrChating,							ui8ChatSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendSecessionRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_SECESSION_CLAN_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCloseRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CLOSE_CLAN_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionMasterRequest( INT32 i32WorkIdx, CUserSession * pSession, TUID i64UserIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_MASTER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64UserIdx,							sizeof(TUID));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionStaffRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_STAFF_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pIdx,									sizeof(TUID) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionRegularRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_REGULAR_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pIdx,									sizeof(INT64) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendDuplicateName( INT32 i32WorkIdx, CUserSession * pSession, char* pstrClanName )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT32 iClanIdx = 0;		// 보낼 필요 없습니다. 클랜 서버와 함께 수정합니다.....

	UINT8 ui8Size = (UINT8)i3String::Length( pstrClanName ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CLIENT_CHECK_DUPLICATE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&iClanIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrClanName,				ui8Size);
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendDuplicateAzit( INT32 i32WorkIdx, CUserSession * pSession, char* pstrAzit )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT32 iClanIdx = 0;		// 보낼 필요 없습니다. 클랜 서버와 함께 수정합니다.....

	UINT8 ui8Size = (UINT8)i3String::Length( pstrAzit ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&iClanIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrAzit,					ui8Size );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendDeportationRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count, TUID* pIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_DEPORTATION_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pIdx,									sizeof(TUID) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendNoticeRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8NoticeSize, char* pstrNotice )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_NOTICE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NoticeSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNotice,							ui8NoticeSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendIntroRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IntroSize, char* pstrIntro )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_INTRO_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8IntroSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrIntro,							ui8IntroSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendNoteRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Type, UINT8 ui8NoteSize, char* pstrNote )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_NOTE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Type,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NoteSize,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNote,								ui8NoteSize );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendNameRequest( INT32 i32WorkIdx, CUserSession * pSession, char* pstrName )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrName,								NET_CLAN_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCheckMarkRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Mark )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_MARK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Mark,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendMarkRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Mark )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MARK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Mark,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendPersonMaxRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_PERSONMAX_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendManagementRequest( INT32 i32WorkIdx, CUserSession * pSession, CLAN_MANAGEMENT* pManagement )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MANAGEMENT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pManagement,							sizeof(CLAN_MANAGEMENT));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendRoomInvitedRequest( INT32 i32WorkIdx, CUserSession * pSession, TUID wUserIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_ROOM_INVITED_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&wUserIdx,							sizeof(TUID));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendCheckJoinAuthorityRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Age, UINT8 ui8Rank )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ClanIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Age,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Rank,					sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendRecordResetRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_RECORD_RESET_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendPointResetRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_POINT_RESET_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendClanColorRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date, UINT8 ui8Color )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_COLOR_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Date,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Color,								sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendExpUpRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_EXPUP_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Date,								sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamContextRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchAllTeamContextRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Channel )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Channel,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamListRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Section )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_LIST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Section,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchAllTeamListRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Channel, UINT16 ui16Section )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Channel,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui16Section,							sizeof(UINT16));	
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamDetailRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32TeamIdx,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamCreateRequest( INT32 i32WorkIdx, CUserSession * pSession, INT8 i8Channel, INT8 i8PerCount )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_CREATE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i8Channel,							sizeof(INT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i8PerCount,							sizeof(INT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamJoinRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx, UINT8 ui8Type )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_JOIN_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32TeamIdx,							sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Type,								sizeof(UINT8));	
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamLeaveRequest( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamChangePerRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8PerMax )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8PerMax,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchFightQuickRequestRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Count )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchFightRequestRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32RequsetTeamIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32RequsetTeamIdx,					sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchFightAccectRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32SpIdx, UINT32 ui32RequsetTeamIdx, UINT8 ui8Accept )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32SpIdx,							sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32RequsetTeamIdx,					sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Accept,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchChattingRequest( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, char* pstrChating )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_CHATING_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ChatSize,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrChating,							ui8ChatSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendChangeNickname( INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_NICK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,							NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendChangeRank( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Rank )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_RANK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Rank,								sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendChangeColorNick( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Color,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendClanInfoRankUpdate( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8Rank = (UINT8)pSession->m_UserInfoBasic._rank;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_INO_RANK_UPDATE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Rank,								sizeof(UINT8));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLAN BATTLE SERVER
BOOL CModuleClan::ClanSendBattleResult( INT32 i32WorkIdx, UINT8 ui8Match, UINT8 ui8ClanCount, CLAN_RESULT* pClanResult, UINT8 ui8UserCount, CLAN_RESULT_USER* pClanResultUser )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_BATTLE_RESULT);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Match,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ClanCount,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8UserCount,		sizeof(UINT8));
	if( ui8ClanCount > 0 )
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pClanResult,		sizeof(CLAN_RESULT) * ui8ClanCount);
	if( ui8UserCount > 0 )
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pClanResultUser,	sizeof(CLAN_RESULT_USER) * ui8UserCount);
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendRoomLeaveResult( INT32 i32WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendBattleRoomJoin( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx, UINT32 ui32ReqTeamIdx, INT32 i32RoomIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32TeamIdx,							sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ReqTeamIdx,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32RoomIdx,							sizeof(INT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendInvite( INT32 i32WorkIdx, CUserSession * pSession, TUID i64UID )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_INVITE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64UID,								sizeof(TUID));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendItemDelete( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32ItemID )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_ITEM_DELETE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_UserInfoBasic._clanidx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

UINT8 CModuleClan::GetClanPerRank( INT32 i32Curper )
{
	if (250 <= i32Curper)		return CLAN_UNIT_CORPS;
	else if (200 <= i32Curper)	return CLAN_UNIT_DIVISION;
	else if (150 <= i32Curper)	return CLAN_UNIT_BRIGADE;
	else if (100 <= i32Curper)	return CLAN_UNIT_REGIMENT;
	else if (50 <= i32Curper)	return CLAN_UNIT_BATTALION;
	else if (30 <= i32Curper)	return CLAN_UNIT_COMPANY;
	else if (10 <= i32Curper)	return CLAN_UNIT_PLATOON;

	return CLAN_UNIT_SQUARD;
}