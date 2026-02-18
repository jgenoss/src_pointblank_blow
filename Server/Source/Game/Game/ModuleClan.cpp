#include "pch.h"
#include "ModuleClan.h"
#include "ModuleClanSocket.h"
// #include "ModuleLogFile.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CModuleClan, i3Thread );

CModuleClan * g_pModuleClan = NULL; 

//////////////////////////////////////////////////////////////
//CLAN
#define CLAN_MAX_SEND_IDX_COUNT					2000000000
#define CLAN_RECONNECT_TIME						10
#define CLAN_SERVER_HEART_BIT					60
#define CLAN_SERVER_HEART_BIT_REQ_TIME			120
#ifdef I3_DEBUG
#define CLAN_SEND_BUFFER						30
#else
#define CLAN_SEND_BUFFER						100
#endif
#define CLAN_CONTEXT_LIST_TIMEOUT				5 * 60
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CLAN_CHECKVALID_IDX		if( !g_pContextMain->m_bClanActive)return FALSE; \
	if( !m_pClanSocket->IsActive())return FALSE; \
	if( 0 == pSession->m_ui32VisitCUID )\
{\
	if( 0 == pSession->m_UserInfoBasic.m_ui32Clanidx )return TRUE; \
}\
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer; \
	switch( WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sClanMainMessage;				break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sClanAuthMessage;				break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sClanClanMessage;				break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sClanMessMessage;				break; \
	default								: pNetBuffer = &m_pClanUserMessage[WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CLAN_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleClan Buffer FULL" );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CLAN_SEND_BUFFER )\
{\
	g_pLog->WriteLog( L"ModuleClan WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % CLAN_SEND_BUFFER;


#define CLAN_CHECKVALID_IDX_NOCLAN	if( !g_pContextMain->m_bClanActive)return FALSE; \
	if( !m_pClanSocket->IsActive())return FALSE; \
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer; \
	switch( WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sClanMainMessage;				break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sClanAuthMessage;				break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sClanClanMessage;				break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sClanMessMessage;				break; \
	default								: pNetBuffer = &m_pClanUserMessage[WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CLAN_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleClan Buffer FULL");\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CLAN_SEND_BUFFER )\
{\
	g_pLog->WriteLog( L"ModuleClan WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % CLAN_SEND_BUFFER;


#define CLAN_MAKE_PACKET_CLAN_MEMBER	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32)); \
pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,							sizeof(T_UID)); \
pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoBasic.m_ui32Clanidx,	sizeof(UINT32)); \

#define CLAN_MAKE_PACKET_CLAN_WAR	CLAN_MAKE_PACKET_CLAN_MEMBER \
pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,						sizeof(T_TeamID)); \
pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TMerID,							sizeof(T_MerID)); \


CModuleClan::CModuleClan(void)
{
	m_bRunning					= FALSE;
	m_ui32ThreadCount			= 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Clan
	m_pClanSocket				= NULL;
	m_ui32ClanConnectTime		= 0; 
	m_pClanUserMessage			= NULL;

	i3mem::FillZero( &m_sClanMainMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanAuthMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanClanMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sClanMessMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
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

	m_pClanUserMessage = (CONNECT_SERVER_NET_BUFFER*) i3MemAlloc(sizeof(CONNECT_SERVER_NET_BUFFER) * m_ui32ThreadCount); 
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
	if( FALSE == m_pClanSocket->OnCreate(g_pContextMain->m_ui32ClanServerIp, g_pContextMain->m_ui16ClanServerPort) )	return SERVER_ERROR_CLAN;
		
	m_ui32ClanConnectTime			= i3ThreadTimer::GetServerTime();
	m_ui32ClanContextSendTime		= i3ThreadTimer::GetServerTime();

	//Create Thread
	if( !i3Thread::Create("ModuleContextA_Clan", 0, 4096, NULL, PRIORITY_HIGH) )								return SERVER_ERROR_CLAN;
	
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_START_GAME_SERVER_REQ);
	INT32	i32SessionIdx = 0;
	T_UID	TUID = 0;
	UINT32	ui32ClanDBIdx = 0;
	SendPacket.WriteData(&i32SessionIdx,	sizeof(INT32));
	SendPacket.WriteData(&TUID,				sizeof(T_UID));
	SendPacket.WriteData(&ui32ClanDBIdx,	sizeof(UINT32));
	
	m_pClanSocket->SendPacketMessage(&SendPacket);
	
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
	g_pLog->AddLogThread( L"MDCTCL", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

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
	if( m_ui32ClanConnectTime + CLAN_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32ClanConnectTime = i3ThreadTimer::GetServerTime();

	m_pClanSocket->OnDestroy(); 
	if( m_pClanSocket->OnCreate(g_pContextMain->m_ui32ClanServerIp, g_pContextMain->m_ui16ClanServerPort) )
	{
		m_ui32ClanContextSendTime		= i3ThreadTimer::GetServerTime();
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
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect ClanServer 1" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect ClanServer 1" );
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
			CONNECT_SERVER_NET_BUFFER	* pUserMessage = &m_pClanUserMessage[i]; 
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
		if( (m_pClanSocket->m_ui32LastSendTime + CLAN_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
			m_pClanSocket->SendPacketMessage(&SendPacket);
			m_pClanSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
		}

		if( m_pClanSocket->m_bNewClanCreate )
		{
			m_pClanSocket->m_bNewClanCreate = FALSE;
			i3NetworkPacket SendPacket(PROTOCOL_CS_CLAN_CONTEXT_REQ);
			m_pClanSocket->SendPacketMessage(&SendPacket);

			m_ui32ClanContextSendTime = i3ThreadTimer::GetServerTime();
		}

		//2_5 Chack Clan Version 
		if( (m_ui32ClanContextSendTime + CLAN_CONTEXT_LIST_TIMEOUT) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_CS_CLAN_CONTEXT_REQ );
			m_pClanSocket->SendPacketMessage(&SendPacket);

			m_ui32ClanContextSendTime = i3ThreadTimer::GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pClanSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pClanSocket->m_ui32LastRecvTime + CLAN_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		INT32 i32Rv = _ClanConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect ClanServer 2" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect ClanServer 2" );
		}
	}

	return i32SendCompleteCount; 
}

BOOL CModuleClan::ClanSendLogIn( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_LOGIN_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendLogOut( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
		
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_LOGOUT_REQ); 
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_WritePos++; 

	return TRUE; 
}

BOOL CModuleClan::ClanSendServerEnter( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_SERVER_ENTER_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendServerLeave( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_SERVER_LEAVE_REQ); 
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendChannelEnter( INT32 WorkIdx, CUserSession * pSession, INT32 i32ChannelIdx )
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_CHANEL_ENTER_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32ChannelIdx,						sizeof(INT32));
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendChannelLeave( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_CHANEL_LEAVE_REQ); 
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_WritePos++; 

	return TRUE;
}

BOOL CModuleClan::ClanSendRoomEnter( INT32 WorkIdx, CUserSession * pSession, INT32 i32RoomIdx, BOOL bMatchMaking )
{
	CLAN_CHECKVALID_IDX;

	bool IsMatchMaking = bMatchMaking?true:false;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_ROOM_ENTER_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32RoomIdx,							sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&IsMatchMaking,						sizeof(bool));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendRoomLeave( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_ROOM_LEAVE_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendPageEnter( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_PAGE_ENTER_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendPageLeave( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_PAGE_LEAVE_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::ClanSendCreate( INT32 WorkIdx, CUserSession * pSession, TTCHAR * pClanName, TTCHAR * pClanIntro, TTCHAR * pClanAzit, UINT32 ui32Mark )	//생성 
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8UserNick	= (UINT8)i3String::Length( pSession->m_UserInfoBasic.m_strNick )+1;
	UINT8 ui8ClanName	= (UINT8)i3String::Length( pClanName )+1;
	UINT8 ui8ClanIntro	= (UINT8)i3String::Length( pClanIntro )+1;
	UINT8 ui8ClanAzit	= (UINT8)i3String::Length( pClanAzit )+1;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CREATE_CLAN_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i32ChannelNum,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i32RoomIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Mark,							sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32ViewRank,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8UserNick,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ClanName,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ClanIntro,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ClanAzit,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->m_UserInfoBasic.m_strNick,	sizeof(TTCHAR) * ui8UserNick);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanName,							sizeof(TTCHAR) * ui8ClanName);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanIntro,							sizeof(TTCHAR) * ui8ClanIntro);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanAzit,							sizeof(TTCHAR) * ui8ClanAzit);
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendJoinRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ClanDBIdx, TTCHAR * strContents )//조인
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8Size		= (UINT8)i3String::Length( strContents )+1;
	UINT8 ui8NickSize	= (UINT8)i3String::Length( pSession->m_UserInfoBasic.m_strNick )+1;
	UINT8 ui8Rank		= (UINT8)pSession->m_UserInfoBasic.m_ui32Rank;
	UINT8 ui8ViewRank	= (UINT8)pSession->m_ui32ViewRank;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_JOIN_REQUEST_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32)); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,						sizeof(T_UID)); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanDBIdx,							sizeof(UINT32)); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_i32ChannelNum,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_i32RoomIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &ui8Size,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( strContents,							sizeof(TTCHAR) * ui8Size);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_ui8Age,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &ui8Rank,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &ui8ViewRank,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &ui8NickSize,							sizeof(UINT8));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( pSession->m_UserInfoBasic.m_strNick,	sizeof(TTCHAR) * ui8NickSize);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoRecord._killcount,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoRecord._death,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoRecord._match,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoRecord._win,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pSession->m_UserInfoRecord._lose,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoRecord._AssistCount, sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendCancelRequest( INT32 WorkIdx, CUserSession * pSession )				//가입취소	
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_CS_CANCEL_REQUEST_REQ ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendClanListDetailInfo(INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Type)		//클랜 리스트에서 특정 클랜 선택시 해당 클랜의 상세정보 요청
{
	CLAN_CHECKVALID_IDX_NOCLAN;	
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_LIST_DETAIL_INFO_REQ); 	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Type,				sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendDetailInfo(INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Type )						//클랜 상세정보 요청
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_DETAIL_INFO_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Type,				sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberContext(INT32 WorkIdx, CUserSession * pSession )						//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_CONTEXT_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(T_UID));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32VisitCUID,			sizeof(UINT32));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberList( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Section)		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_LIST_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(T_UID));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32VisitCUID,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Section,							sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMemberDetailInfo( INT32 WorkIdx, CUserSession * pSession, T_UID ClanMemberUID)		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MEMBER_DETAIL_INFO_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,					sizeof(T_UID));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32VisitCUID,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ClanMemberUID,						sizeof(T_UID));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestContext(	INT32 WorkIdx, CUserSession * pSession )						//가입신청서 요청
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_CS_REQUEST_CONTEXT_REQ ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestList(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Section )		//유저 정보 요청 
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REQUEST_LIST_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Section,							sizeof(UINT8));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendRequestInfo(	INT32 WorkIdx, CUserSession * pSession, T_UID i64RequestIdx )		//유저 상세정보 요청
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REQUEST_INFO_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64RequestIdx ,						sizeof(T_UID));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendAcceptRequest(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count, T_UID * pList) //스탭만 권한이 있음
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_ACCEPT_REQUEST_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pList,								sizeof(T_UID) * ui8Count );
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendDenialRequest(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count, T_UID * pList) //스탭만 권한이 있음
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_DENIAL_REQUEST_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pList,								sizeof(T_UID) * ui8Count );
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendInviteAcceptRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT32 ui32State, UINT8 ui8Accept )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_INVITE_ACCEPT_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32State,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Accept,				sizeof(UINT8)); 
	if( 1 == ui8Accept )
	{
		UINT8 ui8NickSize = (UINT8)i3String::Length( pSession->m_UserInfoBasic.m_strNick ) + 1;
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_ui32ViewRank,	sizeof(UINT32));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NickSize,							sizeof(UINT8));
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->m_UserInfoBasic.m_strNick,		sizeof(TTCHAR) * ui8NickSize);
	}
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendChatingRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CHATTING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bIsGMUser,							sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ChatSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrChating,							sizeof(TTCHAR) * ui8ChatSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendPageChatingRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_PAGE_CHATTING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bIsGMUser,							sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ChatSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrChating,							sizeof(TTCHAR) * ui8ChatSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendSecessionRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_SECESSION_CLAN_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCloseRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLOSE_CLAN_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionMasterRequest( INT32 WorkIdx, CUserSession * pSession, T_UID i64UserIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_MASTER_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64UserIdx,							sizeof(T_UID));

	// 브라질용 클랜 마스터 위임 랭크제한
	if (LOCALE_CODE_BRAZIL == pSession->m_ui8LocaleCode)
	{
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&g_pContextMain->m_ui8ClanCreateRank, sizeof(UINT8));
	}
	else
	{
		UINT8 temp = 0;
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&temp, sizeof(UINT8));
	}

	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionStaffRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count, T_UID* Idx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_STAFF_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(Idx,									sizeof(T_UID) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCommissionRegularRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count, T_UID* Idx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_COMMISSION_REGULAR_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(Idx,									sizeof(T_UID) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendDuplicateName( INT32 WorkIdx, CUserSession * pSession, TTCHAR* strClanName )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8Size = (UINT8)i3String::Length( strClanName ) + 1;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_DUPLICATE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Size,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strClanName,				sizeof(TTCHAR) * ui8Size);
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendDuplicateAzit( INT32 WorkIdx, CUserSession * pSession, TTCHAR* strAzit )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	UINT8 ui8Size = (UINT8)i3String::Length( strAzit ) + 1;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_DUPLICATE_AZIT_URL_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER; 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Size,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strAzit,					sizeof(TTCHAR) * ui8Size );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendDeportationRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count, T_UID* Idx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_DEPORTATION_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(Idx,									sizeof(T_UID) * ui8Count );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendNoticeRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8NoticeSize, TTCHAR* pstrNotice )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_NOTICE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NoticeSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrNotice,							sizeof(TTCHAR) * ui8NoticeSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendIntroRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8IntroSize, TTCHAR* pstrIntro )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_INTRO_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8IntroSize,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrIntro,									sizeof(TTCHAR) * ui8IntroSize );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendNoteRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Type, UINT8 ui8NoteSize, TTCHAR* pstrNote )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_NOTE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Type,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8NoteSize,						sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrNote,							sizeof(TTCHAR) * ui8NoteSize );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::ClanSendNameRequest( INT32 WorkIdx, CUserSession * pSession, TTCHAR* pstrName )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_NAME_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrName,							sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendCheckMarkRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32Mark )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_MARK_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Mark,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendMarkRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32Mark )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MARK_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Mark,							sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendPersonMaxRequest( INT32 WorkIdx, CUserSession * pSession, UINT16 ui16AddPersonCount )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_PERSONMAX_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui16AddPersonCount,					sizeof(UINT16));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendManagementRequest( INT32 WorkIdx, CUserSession * pSession, CLAN_MANAGEMENT* pManagement )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MANAGEMENT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pManagement,							sizeof(CLAN_MANAGEMENT));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendRoomInvitedRequest( INT32 WorkIdx, CUserSession * pSession, T_UID wUserIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_ROOM_INVITED_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&wUserIdx,							sizeof(T_UID));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendCheckJoinAuthorityRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ClanIdx, UINT8 ui8Age, UINT8 ui8Rank )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CHECK_JOIN_AUTHORITY_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ClanIdx,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Age,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Rank,				sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendRecordResetRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_RECORD_RESET_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendPointResetRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_POINT_RESET_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendClanColorRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date, UINT8 ui8Color )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_COLOR_NAME_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ItemID,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Date,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Color,								sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendExpUpRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32Date )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_EXPUP_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ItemID,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32Date,								sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamContextRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchGetClanRoomInfo( INT32 WorkIdx, CUserSession * pSession, UINT8 cChannel )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&cChannel,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchAllTeamContextRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Channel )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Channel,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamListRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchAllTeamListRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Channel, UINT16 ui16Section )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Channel,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui16Section,						sizeof(UINT16));	
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamDetailRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32TeamIdx,						sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamCreateRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8MaxPer )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8MaxPer,							sizeof(UINT8));

	INT32 i32KDWeight = pSession->GetKillDeathRate();
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32KDWeight, sizeof(INT32));
	
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamJoinRequest(INT32 WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx, UINT8 ui8Type)
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_JOIN_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32TeamIdx,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Type,							sizeof(UINT8));	
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamLeaveRequest( INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchTeamChangePerRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8PerMax )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8PerMax,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

//BOOL CModuleClan::ClanSendMatchFightQuickRequestRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Count )
//{
//	CLAN_CHECKVALID_IDX;
//	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
//	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_MATCH_MATCHMAKING_REQ);
//	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,						sizeof(INT32));
//	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i64UID,							sizeof(T_UID));
//	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoBasic.m_ui32Clanidx,	sizeof(UINT32));
//	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Count,									sizeof(UINT8));
//	pNetBuffer->_WritePos++;
//	return TRUE;
//}

BOOL CModuleClan::ClanSendMatchFightRequestRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32RequsetTeamIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32RequsetTeamIdx,					sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchFightAccectRequest( INT32 WorkIdx, CUserSession * pSession, UINT32 wSpIdx, UINT32 ui32RequsetTeamIdx, UINT8 ui8Accept )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&wSpIdx,								sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32RequsetTeamIdx,					sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Accept,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendMatchChattingRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, TTCHAR* pstrChating, bool bIsGMUser )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_CHATING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bIsGMUser,									sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ChatSize,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pstrChating,									sizeof(TTCHAR) * ui8ChatSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendChangeNickname( INT32 WorkIdx, CUserSession * pSession, TTCHAR* strNickName )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_NICK_REQ);
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(strNickName,							sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendChangeRank( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Rank )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_RANK_REQ);
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Rank,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleClan::ClanSendChangeColorNick( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Color )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_REPLACE_MEMBER_COLOR_NICK_REQ);
	CLAN_MAKE_PACKET_CLAN_WAR;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Color,							sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLAN CS

BOOL CModuleClan::ClanSendMatchResultContext(INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;

	PACKET_GC_CS_CLAN_MATCH_RESULT_CONTEXT_REQ	PacketReqGC;
	PacketReqGC.m_i32SessionIdx		= pSession->m_SessionIdx;
	PacketReqGC.m_i64UID			= pSession->m_i64UID;
	if(pSession->m_bVisitClanPageEnter) { PacketReqGC.m_ui32CUID = pSession->m_ui32VisitCUID; }
	else								{ PacketReqGC.m_ui32CUID = pSession->m_UserInfoBasic.m_ui32Clanidx; }

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_CONTEXT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&PacketReqGC, sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_CONTEXT_REQ));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMatchResultList( INT32 WorkIdx, CUserSession * pSession, UINT8 iSection)
{
	CLAN_CHECKVALID_IDX;

	PACKET_GC_CS_CLAN_MATCH_RESULT_LIST_REQ		PacketReqGC;
	PacketReqGC.m_i32SessionIdx		= pSession->m_SessionIdx;
	PacketReqGC.m_i64UID			= pSession->m_i64UID;
	if(pSession->m_bVisitClanPageEnter) { PacketReqGC.m_ui32CUID = pSession->m_ui32VisitCUID; }
	else								{ PacketReqGC.m_ui32CUID = pSession->m_UserInfoBasic.m_ui32Clanidx; }	
	PacketReqGC.m_ui8iSection		= iSection;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_LIST_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&PacketReqGC, sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_LIST_REQ));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMatchResultEmotionSave( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8MatchCount, UINT8 ui8Emotion, TTCHAR* strNickName )
{
	CLAN_CHECKVALID_IDX;

	PACKET_GC_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ		PacketReqGC;
	PacketReqGC.m_i32SessionIdx		= pSession->m_SessionIdx;
	PacketReqGC.m_i64UID			= pSession->m_i64UID;
	PacketReqGC.m_ui32CUID			= pSession->m_UserInfoBasic.m_ui32Clanidx;
	PacketReqGC.m_ui8MatchCount		= ui8MatchCount;
	PacketReqGC.m_ui8Emotion		= ui8Emotion;
	i3String::Copy(PacketReqGC.m_strUserNick, strNickName, NET_NICK_NAME_SIZE);
	PacketReqGC.m_strUserNick[NET_NICK_NAME_SIZE-1] = '\0';

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ);  
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&PacketReqGC, sizeof(PACKET_GC_CS_CLAN_MATCH_RESULT_EMOTION_SAVE_REQ));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

BOOL CModuleClan::ClanSendMatchResultAutoAcceptOption( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32AutoAcceptOption )
{
	CLAN_CHECKVALID_IDX;

	PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ	PacketReqGC;
	PacketReqGC.m_i32SessionIdx			= pSession->m_SessionIdx;
	PacketReqGC.m_i64UID					= pSession->m_i64UID;
	PacketReqGC.m_ui32CUID				= pSession->m_UserInfoBasic.m_ui32Clanidx;
	PacketReqGC.m_ui32AutoAcceptOption	= ui32AutoAcceptOption;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&PacketReqGC, sizeof(PACKET_GC_CS_CLAN_REQUEST_AUTO_ACCEPT_OPTION_REQ));
	pNetBuffer->_WritePos++; 
	return TRUE; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CLAN BATTLE SERVER
BOOL CModuleClan::ClanSendBattleResult(	INT32 WorkIdx, UINT8 ui8ClanCount, CLAN_RESULT* pClanResult, CLAN_DETAIL_RESULT* pClanDetailResult, UINT8 ui8UserCount, CLAN_RESULT_USER* pClanResultUser ) // 클랜 배틀 결과
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_BATTLE_RESULT);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ClanCount,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8UserCount,		sizeof(UINT8));
	if( ui8ClanCount > 0 )
	{
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanResult,		sizeof(CLAN_RESULT) * ui8ClanCount);
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanDetailResult,	sizeof(CLAN_DETAIL_RESULT));
	}
	if( ui8UserCount > 0 )
		pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pClanResultUser, sizeof(CLAN_RESULT_USER) * ui8UserCount);
	pNetBuffer->_WritePos++;   

	return TRUE;
}
 
BOOL CModuleClan::ClanSendTeamChattingRequest( INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChatSize, TTCHAR* pChat )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,						sizeof(T_TeamID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8ChatSize,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pChat,										sizeof(TTCHAR) * ui8ChatSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendRoomLeaveResult(INT32 WorkIdx, CUserSession * pSession)
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendBattleRoomJoin( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32TeamIdx, UINT32 ui32ReqTeamIdx, INT32 i32RoomIdx )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32TeamIdx,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ReqTeamIdx,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32RoomIdx,							sizeof(INT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendInvite( INT32 WorkIdx, CUserSession * pSession, T_UID i64UID )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_INVITE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i64UID,								sizeof(T_UID));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::ClanSendItemDelete( INT32 WorkIdx, CUserSession * pSession, UINT32 ui32ItemID )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_CLAN_ITEM_DELETE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32ItemID,							sizeof(UINT32));
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

BOOL CModuleClan::ClanSendFindUserClanInfo(	INT32 WorkIdx, CUserSession * pSession, T_UID ui64FindUID )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_GC_CLAN_USER_INFO_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	PACKET_GC_NEW_CLAN_USER_INFO_REQ	PacketReqGC;
	PacketReqGC.m_i32SessionIdx			= pSession-> GetSessionIndex();
	PacketReqGC.m_i64UID				= pSession->GetUID();
	PacketReqGC.m_i64FindUID			= ui64FindUID;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&PacketReqGC, sizeof(PACKET_GC_NEW_CLAN_USER_INFO_REQ));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleClan::SendClanMatchJoinTeamRequest(INT32 WorkIdx, CUserSession * pSession, T_TeamID TTeamID)
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TTeamID,				sizeof(T_TeamID));

	INT32 i32KillDeathRate = pSession->GetKillDeathRate();
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32KillDeathRate,		sizeof(INT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SendClanMatchLeaveTeamRequest(INT32 WorkIdx, CUserSession * pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,	sizeof(T_TeamID));

	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SendClanMatchChangeTeamOperation(INT32 WorkIdx, CUserSession * pSession, const TTCHAR* pOperation)
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,	sizeof(T_TeamID));

	UINT8 ui8Length = ( UINT8)MIN( i3String::Length( pOperation ), NET_CLAN_OPERATION_SIZE - 1 );
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8Length,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pOperation,				sizeof(TTCHAR)*ui8Length);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SendClanMatchChangeMaxPerCount(INT32 WorkIdx, CUserSession * pSession, UINT8 ui8MaxPerCount)
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,	sizeof(T_TeamID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8MaxPerCount,			sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SendClanMatchMaking(INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MATCHMAKING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,	sizeof(T_TeamID));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SendCancelClanMatchMaking(INT32 WorkIdx, CUserSession * pSession )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,	sizeof(T_TeamID));
	pNetBuffer->_WritePos++;
	return TRUE;
}
 
BOOL CModuleClan::SendCreateClanMatchRoomAck(INT32 WorkIdx, CUserSession * pSession, T_MatchingID TMatchingID, INT32 i32MatchRoomIdx)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMatchingID,								sizeof(T_MatchingID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_i32ChannelNum,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32MatchRoomIdx,							sizeof(INT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendJoinClanMatchRoomAck(INT32 WorkIdx, CUserSession * pSession, T_RESULT TResult, T_MatchingID TMatchingID )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TResult,									sizeof(T_RESULT));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMatchingID,								sizeof(T_MatchingID));  
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,						sizeof(T_TeamID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchLeaveRoom(INT32 WorkIdx, CUserSession* pSession, T_MatchingID TMatchingID, bool bRoomDestroy, UINT8 ui8RoomState)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	T_TeamID TTeamID = pSession->GetMatchTeamId();
	bool	bForceLeave = pSession->GetForceLeaveMatchRoom();

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_LEAVE_ROOM_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMatchingID,								sizeof(T_MatchingID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bRoomDestroy,								sizeof(bool));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TTeamID, sizeof(T_TeamID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8RoomState,								sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&bForceLeave,								sizeof(bool));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchRegistMercenary(INT32 WorkIdx, CUserSession* pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;

	// 현재 장착중인 주무기 타입을 보냅니다.
	S2MO_CHAR_EQUIP_INFO	CharEquipInfo;
	pSession->m_pDesign->GetMainCharEquipInfo( &pSession->m_MultiSlot, &CharEquipInfo);	// 팀 알수 없음.
	ITEM_INFO ItemInfo;
	CharEquipInfo.GetCharEquipmentInfo( CHAR_EQUIPMENT_PRIMARY, &ItemInfo );
	
	UINT8 ui8CurEquipClass = (UINT8)GET_ITEM_SUBTYPE( ItemInfo.m_TItemID );

	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8CurEquipClass,							sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSession->m_UserInfoBasic.m_strNick,			sizeof(TTCHAR)* NET_NICK_NAME_SIZE); 

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchRemoveMercenary(INT32 WorkIdx, CUserSession* pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TMerID,							sizeof(T_MerID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchInviteMercenaryReq(INT32 WorkIdx, CUserSession* pSession, T_MerID TMerID)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMerID,										sizeof(T_MerID));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,						sizeof(T_TeamID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchInviteAcceptReq(INT32 WorkIdx, CUserSession* pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TMerID,		sizeof(T_MerID));
	INT32 i32KDWeight = pSession->GetKillDeathRate();
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&i32KDWeight,			sizeof(INT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchInviteDenialReq(INT32 WorkIdx, CUserSession* pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TMerID,		sizeof(T_MerID));
	pNetBuffer->_WritePos++;

	return TRUE;
}


BOOL CModuleClan::SendClanWarReleaseInviteTeam(INT32 WorkIdx, CUserSession* pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_RELEASE_INVITE_TEAM_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TMerID, sizeof(T_MerID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanMatchMercenaryListReq(INT32 WorkIdx, CUserSession* pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::ClanSendChangeState( INT32 WorkIdx, CUserSession * pSession, FRIEND_STATE eState )
{
	CLAN_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CS_USER_CHANGE_STATE_REQ); 
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&eState,	sizeof(FRIEND_STATE));
	pNetBuffer->_WritePos++; 
	return TRUE;
}

BOOL CModuleClan::SendClanMatcMercenaryDetailInfo(INT32 WorkIdx, CUserSession* pSession, T_MerID TMerID )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMerID,	sizeof(T_MerID)); 
	pNetBuffer->_WritePos++;

	return TRUE;
}


BOOL CModuleClan::SendClanCommandMatchTeamPoint(INT32 WorkIdx, CUserSession * pSession)
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_TTeamID,		sizeof(T_TeamID));
	pNetBuffer->_WritePos++;

	return TRUE;
}


BOOL CModuleClan::SendClanCommandMatchResult(INT32 WorkIdx, CUserSession * pSession, TEAM_TYPE eWinTeam, T_MatchingID TMatchingID )
{
	CLAN_CHECKVALID_IDX_NOCLAN;
	
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&eWinTeam,			sizeof(TEAM_TYPE));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&TMatchingID,		sizeof(T_MatchingID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::SendClanWarResult(INT32 WorkIdx, CUserSession* pSession)
{
	CLAN_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_RESULT_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleClan::LoadMerPenalty( INT32 WorkIdx, CUserSession* pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleClan::SaveMerPenalty( INT32 WorkIdx, CUserSession* pSession )
{
	CLAN_CHECKVALID_IDX_NOCLAN;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_SAVE_REQ);
	CLAN_MAKE_PACKET_CLAN_MEMBER;
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_dt32MerPenaltyTime,		sizeof(DATE32));
	pNetBuffer->_WritePos++;

	return TRUE;
}