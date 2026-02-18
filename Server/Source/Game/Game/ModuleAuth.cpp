#include "pch.h"
#include "ModuleAuth.h"
#include "ModuleAuthSocket.h"
#include "LogFile.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CModuleAuth, i3Thread );

//CModuleAuth * g_pModuleAuth		= NULL; 

#define AUTH_SEND_BUFFER				10
#define AUTH_SERVER_TIMEOUT				25			//AuthServer는 내부적으로 30초 입니다. 그래서 25초마다 한번씩 패킷을 보낸다. 
#define AUTH_SERVER_HEART_BIT_REQ_TIME	10			//10까지 기다린다.
#define AUTH_SHOP_CHECK_TIME			60 * 60		//5분 마다 한번씩 체크한다. 
#define AUTH_CONNECT_TIMEOUT			50

#define CHECKVALID_IDX		if( !m_pAuthSocket->IsActive())return FALSE; \
							AUTH_NET_BUFFER	* pNetBuffer; \
							switch( WorkIdx )\
							{\
							case	MAIN_THREAD_IDX	: pNetBuffer = &m_MainMessage;			break; \
							case	AUTH_THREAD_IDX	: pNetBuffer = &m_AuthMessage;			break; \
							default					: pNetBuffer = &m_UserMessage[WorkIdx];	break; \
							}\
							if(!_CheckBufferIdx(pNetBuffer))\
							{\
								WRITE_LOG(NULL,"CModuleAuth::WriteBuffer FULL");\
								return FALSE; \
							}\
							INT32 WriteIdx = pNetBuffer->_WritePos % AUTH_SEND_BUFFER; \
							if(WriteIdx >= AUTH_SEND_BUFFER)\
							{\
								WRITE_LOG(NULL,"CModuleAuth::WriteBuffer Overflow");\
								return FALSE; \
							}\

CModuleAuth::CModuleAuth(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);	
	m_UserMessage		= 0; 
	m_iShopCheckTime	= 0; 
}

CModuleAuth::~CModuleAuth(void)
{
	::SetEvent( m_hKillEvent ); 
	::Sleep(1000);
	::CloseHandle( m_hKillEvent ); 
	OnDestroy(); 
}

BOOL CModuleAuth::OnCreate(INT32 ThreadCount)
{
	BOOL Rv = TRUE; 
#ifdef USE_AUTHSERVER
	//Create Socket 
	m_pAuthSocket = CModuleAuthSocket::NewObject(); 
	Rv = m_pAuthSocket->OnCreate(g_pContextMain->m_AuthServerIp, g_pContextMain->m_AuthServerPort); 	
	if(Rv)
	{
		m_iConnectTime = g_pContextMain->GetServerTime(); 		
	}
	else
	{
		m_pAuthSocket->OnDestroy();		
	}

	//----------------------------------------------------
	m_ThreadCount = ThreadCount; 

	m_MainMessage._ReadPos		= 0; 
	m_MainMessage._WritePos		= 0; 
	m_MainMessage._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * AUTH_SEND_BUFFER); 

	m_AuthMessage._ReadPos		= 0; 
	m_AuthMessage._WritePos		= 0; 
	m_AuthMessage._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * AUTH_SEND_BUFFER); 

	for(INT32 i = 0; i < AUTH_SEND_BUFFER; i++)
	{
		m_MainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
		m_AuthMessage._ppNetMessage[i] = i3NetworkPacket::NewObject(); 
	}

	m_UserMessage = (AUTH_NET_BUFFER*) i3MemAlloc(sizeof(AUTH_NET_BUFFER) * m_ThreadCount); 
	for(INT32 i = 0; i< m_ThreadCount; i++)
	{
		m_UserMessage[i]._ReadPos	= 0; 
		m_UserMessage[i]._WritePos	= 0; 
		m_UserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * AUTH_SEND_BUFFER); 
		for(INT32 j = 0; j < AUTH_SEND_BUFFER; j++)
		{
			m_UserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//----------------------------------------------------
	m_iLastSendTime = g_pContextMain->GetServerTime();
	m_pAuthSocket->m_iLastRecvTime = m_iLastSendTime + 1;

	m_iShopCheckTime = g_pContextMain->GetServerTime();
	//Create Thread
	Rv = i3Thread::Create("ModuleAuth", 0, 4096, NULL, PRIORITY_HIGH);		
	if(!Rv)return FALSE; 
	WRITE_LOG(NULL, "[CModuleAuth::OnCreate] AUTH CONNECT SUCCESS"); 
#endif
	return Rv; 
}

void CModuleAuth::OnDestroy(void)
{
#ifdef USE_AUTHSERVER
	//----------------------------------------------------
	for(INT32 i = 0; i < AUTH_SEND_BUFFER; i++)
	{
		I3_SAFE_RELEASE( m_MainMessage._ppNetMessage[i] ); 
		I3_SAFE_RELEASE( m_AuthMessage._ppNetMessage[i] ); 		
	}
	I3MEM_SAFE_FREE(m_MainMessage._ppNetMessage); 
	I3MEM_SAFE_FREE(m_AuthMessage._ppNetMessage); 

	for(INT32 i = 0; i< m_ThreadCount; i++)
	{
		for(INT32 j = 0;j < AUTH_SEND_BUFFER; j++)
		{
			I3_SAFE_RELEASE(m_UserMessage[i]._ppNetMessage[j] ); 			
		}
		I3MEM_SAFE_FREE(m_UserMessage[i]._ppNetMessage); 
	}

	I3_SAFE_RELEASE( m_pAuthSocket );
#endif
	return; 
}

UINT32 CModuleAuth::OnRunning( void * pUserData)
{
	INT32	i; 
	INT32	SendCount; 
	INT32	ReadIdx;
	BOOL	IsSend;
	while(1)
	{
		m_SendAuthCount = 0; 
		IsSend = FALSE; 
		if( ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 )break;	

		//-------------------------------------------------------------------------------
		//1. Connect Check 
		if(!m_pAuthSocket->IsConnect())
		{
			m_pAuthSocket->OnDestroy(); 
			if( m_pAuthSocket->OnCreate(g_pContextMain->m_AuthServerIp, g_pContextMain->m_AuthServerPort) )
			{
				m_iConnectTime = g_pContextMain->GetServerTime(); 		
				I3TRACE("Success ReConnect AuthServer Check 1\n");
				WRITE_LOG(NULL,"Success ReConnect AuthServer 1");
			}
			else
			{
				m_pAuthSocket->OnDestroy(); 
				I3TRACE("Fail ReConnect AuthServer Check 1\n");
				WRITE_LOG(NULL,"Fail ReConnect AuthServer 1");
				::Sleep(1000);
			}

			m_iLastSendTime	= g_pContextMain->GetServerTime();
			m_pAuthSocket->m_iLastRecvTime = m_iLastSendTime + 1;
			continue; 
		}

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pAuthSocket->IsActive() )
		{
			//2-1 Main Thread 
			SendCount = m_MainMessage._WritePos - m_MainMessage._ReadPos; 
			if(SendCount > 0)
			{
				for(i = 0; i < SendCount; i++)
				{
					ReadIdx = m_MainMessage._ReadPos % AUTH_SEND_BUFFER; 
					m_pAuthSocket->SendPacketMessage( m_MainMessage._ppNetMessage[ReadIdx] );
					m_MainMessage._ReadPos++; 
					m_SendAuthCount++; 
				}
				IsSend = TRUE; 
			}
		}

		if( m_pAuthSocket->IsActive() )
		{
			//2-2 Auth Thread 
			SendCount = m_AuthMessage._WritePos - m_AuthMessage._ReadPos; 
			if(SendCount > 0)
			{
				for(i = 0; i < SendCount; i++)
				{
					ReadIdx = m_AuthMessage._ReadPos % AUTH_SEND_BUFFER; 
					m_pAuthSocket->SendPacketMessage( m_AuthMessage._ppNetMessage[ReadIdx]);
					m_AuthMessage._ReadPos++; 
					m_SendAuthCount++; 
				}
				IsSend = TRUE; 
			}
		}


		if( m_pAuthSocket->IsActive() )
		{
			//2-3 User Thread
			for(INT32 j = 0; j < m_ThreadCount; j++)
			{
				AUTH_NET_BUFFER	* pUserMessage = &m_UserMessage[j]; 
				SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
				if(SendCount > 0)
				{				
					for(i = 0; i < SendCount; i++)
					{				
						ReadIdx = pUserMessage->_ReadPos % AUTH_SEND_BUFFER; 
						m_pAuthSocket->SendPacketMessage( pUserMessage->_ppNetMessage[ReadIdx] );
						pUserMessage->_ReadPos++; 
						m_SendAuthCount++; 
					}
					IsSend = TRUE;
				}
			}
		}

		if( m_pAuthSocket->IsActive() )
		{
			//2-4 Check Send 패킷
			if( (m_iLastSendTime + AUTH_SERVER_TIMEOUT) < g_pContextMain->GetServerTime() )
			{
				i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
				m_pAuthSocket->SendPacketMessage(&SendPacket);
				m_iLastSendTime = g_pContextMain->GetServerTime();
			}

			if( (m_iShopCheckTime + AUTH_SHOP_CHECK_TIME) < g_pContextMain->GetServerTime() )
			{
				i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_GET_SAILLIST_VERSION_REQ); 
				m_pAuthSocket->SendPacketMessage(&SendPacket);
				m_iShopCheckTime = g_pContextMain->GetServerTime();
			}

			if( m_pAuthSocket->m_bShopListReq )
			{
				i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_GET_SAILLIST_REQ); 
				m_pAuthSocket->SendPacketMessage(&SendPacket);
				m_pAuthSocket->m_bShopListReq = FALSE; 
			}
		}
		else 
		{
			//Connect Req Fail
			if( m_iConnectTime + AUTH_CONNECT_TIMEOUT < g_pContextMain->GetServerTime())
			{
				m_pAuthSocket->OnDestroy(); 
				if( m_pAuthSocket->OnCreate(g_pContextMain->m_AuthServerIp, g_pContextMain->m_AuthServerPort) )
				{
					m_iConnectTime = g_pContextMain->GetServerTime(); 		
					I3TRACE("Success ReConnect AuthServer 3\n");
					WRITE_LOG(NULL,"Success ReConnect AuthServer 3");
				}
				else
				{
					m_pAuthSocket->OnDestroy(); 
					I3TRACE("Fail ReConnect AuthServer 3\n");
					WRITE_LOG(NULL,"Fail ReConnect AuthServer3");
					::Sleep(1000);
				}
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pAuthSocket->SelectEvent(); 		

		//-------------------------------------------------------------------------------
		//4. Check Connect State 
		//if(  m_pAuthSocket->IsActive() && (m_pAuthSocket->GetLastRecvTime() + AUTH_SERVER_TIMEOUT + 5) < g_pContextMain->GetServerTime() )//Send 보다는 5초를 더 해준다.

		if( m_pAuthSocket->IsActive() && (m_iLastSendTime > m_pAuthSocket->m_iLastRecvTime) )
		{//ACK를 못받을때 들어온다. 

			if( (m_iLastSendTime + AUTH_SERVER_HEART_BIT_REQ_TIME) < g_pContextMain->GetServerTime() )
			{
				m_pAuthSocket->OnDestroy(); 
				if( m_pAuthSocket->OnCreate(g_pContextMain->m_AuthServerIp, g_pContextMain->m_AuthServerPort) )
				{
					m_iConnectTime = g_pContextMain->GetServerTime(); 		
					I3TRACE("Success ReConnect AuthServer 2\n");
					WRITE_LOG(NULL,"Success ReConnect AuthServer 2");
				}
				else
				{
					m_pAuthSocket->OnDestroy(); 
					I3TRACE("Fail ReConnect AuthServer2\n");
					WRITE_LOG(NULL,"Fail ReConnect AuthServer2");
					::Sleep(1000);
				}

				m_iLastSendTime					= g_pContextMain->GetServerTime();
				m_pAuthSocket->m_iLastRecvTime	= m_iLastSendTime + 1;
				continue; 
			}			
		}
	}
	return 0; 
}

BOOL CModuleAuth::SendLogIn(INT32 WorkIdx, INT32 * pSessionIdx, char * pId, char * pPass, UINT64 * pMacAddress, UINT32 * pNetIp)
{ 	
#ifdef USE_AUTHSERVER	
	CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_LOGIN_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pId,			NET_ID_SIZE	);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pPass,		NET_PW_SIZE	);
	UINT32 IpAdd;			//NC요청 
	IpAdd = htonl(*pNetIp); //NC요청 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&IpAdd,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pMacAddress,	sizeof(INT64));

	pNetBuffer->_WritePos++; 
#endif	
	return TRUE; 
} 

BOOL CModuleAuth::SendLogInWEB(	INT32 WorkIdx, INT32 * pSessionIdx, char * pSessionKey, UINT64 * pMacAddress, UINT32 * pNetIp)
{ 
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_LOGIN_WEBKEY_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSessionIdx,	sizeof(INT32));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pSessionKey,	16);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pNetIp,		sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pMacAddress,	sizeof(UINT64));	

	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendServerEnter(INT32 WorkIdx, CUserSession * pSession)
{ 
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_USER_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof(INT32));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));

	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendServerLeave(  INT32 WorkIdx, CUserSession * pSession)
{ 
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_USER_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx, sizeof(INT32));		

	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendLogOut( INT32 WorkIdx, unsigned char * pNcGusId, INT32 DbIdx)
{ 
#ifdef USE_AUTHSERVER	
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_LOGOUT_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pNcGusId, NET_NCGUSID_SIZE);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&DbIdx,	sizeof(INT32));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendBuyGoods( INT32 WorkIdx, CUserSession * pSession, INT32 GoodsIdx )
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;	
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_BUY_ITEM_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&GoodsIdx,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendInsertItem( INT32 WorkIdx, CUserSession * pSession, INT32 ItemIdx, INT32 authDay )
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;	
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ItemIdx,				sizeof(UINT32));	
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&authDay,				sizeof(UINT32));	
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendAuthItem(	INT32 WorkIdx, CUserSession * pSession, INT32 InventoryIdx, UINT64 ItemId )
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_AUTH_START_REQ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ItemId,				sizeof(INT64));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&InventoryIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));	
	pNetBuffer->_WritePos++;
#endif
	return TRUE; 
}

BOOL CModuleAuth::SendUserNowInfo( INT32 WorkIdx, UINT32 SessionIdx, INT32 ChannelIdx, INT32 RoomIdx, INT32 SlotIdx )
{ 
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_AUTH_USER_NOW_INFO_ACK); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ChannelIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&RoomIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&SlotIdx,		sizeof(INT32));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendFriendInviteUser(	INT32 WorkIdx, CUserSession * pSession, UINT8 FriendIdx)
{ 
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_INVITED_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof(INT32)); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&FriendIdx,				sizeof(UINT8)); 
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendFriendAccept(	INT32 WorkIdx, CUserSession * pSession, UINT8 Idx)
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_ACCEPT_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof( INT32));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}
BOOL CModuleAuth::SendFriendInsert(	INT32 WorkIdx, CUserSession * pSession, UINT32 DbIdx)
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_INSERT_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof( INT32 ));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&DbIdx,					sizeof( UINT32 ));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}
BOOL CModuleAuth::SendFriendDelete(	INT32 WorkIdx, CUserSession * pSession, UINT8 Idx)
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_DELETE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof( INT32));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Idx,					sizeof( UINT8 ));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}
BOOL CModuleAuth::SendFriendRoomEnter(	INT32 WorkIdx, CUserSession * pSession, UINT8 ChIdx, UINT16 RoomIdx)
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_ROOM_ENTER_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof( INT32));		
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ChIdx,					sizeof( UINT8 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&RoomIdx,				sizeof( UINT16 ));
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}

BOOL CModuleAuth::SendFriendRoomLeave(	INT32 WorkIdx, CUserSession * pSession )
{
#ifdef USE_AUTHSERVER
	CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_AUTH_FRIEND_ROOM_LEAVE_REQ ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&pSession->m_DBIdx,		sizeof( INT32));		
	pNetBuffer->_WritePos++;
#endif	
	return TRUE; 
}
