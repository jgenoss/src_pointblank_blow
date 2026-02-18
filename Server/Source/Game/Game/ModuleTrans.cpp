#include "pch.h"
#include "ModuleTrans.h"
#include "ModuleTransSocket.h"
// #include "ModuleLogFile.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CModuleTrans, i3Thread );

CModuleTrans * g_pModuleTrans = NULL; 

//////////////////////////////////////////////////////////////
//Trans
#define TRANS_MAX_SEND_IDX_COUNT				2000000000
#define TRANS_RECONNECT_TIME					10
#define TRANS_SERVER_HEART_BIT					25
#define TRANS_SERVER_HEART_BIT_REQ_TIME			60
#ifdef I3_DEBUG
#define TRANS_SEND_BUFFER						50
#else
#define TRANS_SEND_BUFFER						500
#endif
#define TRANS_SERVER_SHOP_VERSION_CHECK_TIME	30
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TRANS_CHECKVALID_IDX		if( !m_pTransSocket->IsActive())return FALSE; \
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer = NULL; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sTransMainMessage;			break; \
	case	MODULE_CONTROL_THREAD_IDX	: I3ASSERT_0;									break; \
	case	MODULE_CAST_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_TRANS_THREAD_IDX		: pNetBuffer = &m_sTransTransMessage;			break; \
	case	MODULE_CLAN_THREAD_IDX		: pNetBuffer = &m_sTransClanMessage;			break; \
	case	MODULE_MESSENGER_THREAD_IDX	: pNetBuffer = &m_sTransMessMessage;			break; \
	default								: pNetBuffer = &m_pTransUserMessage[i32WorkIdx];break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > TRANS_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleTrans WriteBuffer FULL" );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= TRANS_SEND_BUFFER )\
{\
	g_pLog->WriteLog( L"ModuleTrans WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % TRANS_SEND_BUFFER;



CModuleTrans::CModuleTrans(void)
{
	m_bRunning					= FALSE;
	m_ui32ThreadCount			= 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Trans	
	m_pTransSocket				= NULL;
	m_ui32AuthConnectTime		= 0;
	m_pTransUserMessage			= NULL;

	i3mem::FillZero( &m_sTransMainMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransTransMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransClanMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransMessMessage,	sizeof( CONNECT_SERVER_NET_BUFFER ) );
}

CModuleTrans::~CModuleTrans(void)
{
	OnDestroy();
}

INT32 CModuleTrans::OnCreate(UINT32 ui32ThreadCount)
{
	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sTransMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * TRANS_SEND_BUFFER); 
	m_sTransTransMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * TRANS_SEND_BUFFER);
	m_sTransClanMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * TRANS_SEND_BUFFER);
	m_sTransMessMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * TRANS_SEND_BUFFER);

	for( UINT32 i = 0; i < TRANS_SEND_BUFFER ; i++ )
	{
		m_sTransMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
		m_sTransTransMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
		m_sTransClanMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
		m_sTransMessMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	// User
	m_pTransUserMessage = (CONNECT_SERVER_NET_BUFFER*) i3MemAlloc(sizeof(CONNECT_SERVER_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pTransUserMessage[i]._ReadPos	= 0; 
		m_pTransUserMessage[i]._WritePos	= 0; 
		m_pTransUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * TRANS_SEND_BUFFER); 
		for( UINT32 j = 0 ; j < TRANS_SEND_BUFFER ; j++ )
		{
			m_pTransUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Socket
	m_pTransSocket = CModuleTransSocket::NewObject(); 
	if( FALSE == m_pTransSocket->OnCreate(g_pContextMain->m_ui32TransServerIp, g_pContextMain->m_ui16TransServerPort) ) 	return SERVER_ERROR_AUTH;
	
	m_ui32AuthConnectTime		= i3ThreadTimer::GetServerTime();

	//Create Thread
	if( !i3Thread::Create("ModuleContextA_Trans", 0, 4096, NULL, PRIORITY_HIGH) )								return SERVER_ERROR_AUTH;
	
	return EVENT_ERROR_SUCCESS; 
}

void CModuleTrans::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pTransSocket );

	if( m_sTransMainMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < TRANS_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sTransMainMessage._ppNetMessage[i] ); 
		}
		I3MEM_SAFE_FREE(m_sTransMainMessage._ppNetMessage); 
	}
	if( m_sTransTransMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < TRANS_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sTransTransMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sTransTransMessage._ppNetMessage); 
	}
	if( m_sTransClanMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < TRANS_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sTransClanMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sTransClanMessage._ppNetMessage); 
	}
	if( m_sTransMessMessage._ppNetMessage )
	{
		for( UINT32 i = 0; i < TRANS_SEND_BUFFER; i++)
		{
			I3_SAFE_RELEASE( m_sTransMessMessage._ppNetMessage[i] ); 		
		}
		I3MEM_SAFE_FREE(m_sTransMessMessage._ppNetMessage); 
	}

	if( m_pTransUserMessage )
	{
		for( UINT32 i = 0; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pTransUserMessage[i]._ppNetMessage )
			{
				for( INT32 j = 0;j < TRANS_SEND_BUFFER; j++)
				{
					I3_SAFE_RELEASE(m_pTransUserMessage[i]._ppNetMessage[j] ); 			
				}
				I3MEM_SAFE_FREE(m_pTransUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pTransUserMessage );
	}

	return; 
}

UINT32 CModuleTrans::OnRunning( void * pUserData)
{	
	g_pLog->AddLogThread( L"MDCTATR", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bRunning		= TRUE;	

	while( m_bRunning )
	{
		if( 0 < _TransUpdate() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0; 
}

INT32 CModuleTrans::_TransConnect()
{
	if( m_ui32AuthConnectTime + TRANS_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32AuthConnectTime = i3ThreadTimer::GetServerTime();

	m_pTransSocket->OnDestroy(); 
	if( m_pTransSocket->OnCreate(g_pContextMain->m_ui32TransServerIp, g_pContextMain->m_ui16TransServerPort) )
	{
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pTransSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleTrans::_TransUpdate(void)
{
	UINT32	i; 
	INT32	i32SendCount; 
	INT32	i32ReadIdx;
	UINT32	ui32SendCompleteCount = 0;

	//1. Connect Check 
	if( FALSE == m_pTransSocket->IsConnect() )
	{
		INT32 i32Rv = _TransConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect TransServer 1 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect TransServer 1 Fail" );
		}
		return ui32SendCompleteCount;
	}

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pTransSocket->IsActive() )
	{
		//2-1 Main Thread 
		i32SendCount = m_sTransMainMessage._WritePos - m_sTransMainMessage._ReadPos; 
		if( 0 < i32SendCount )
		{			
			i32ReadIdx = m_sTransMainMessage._ReadPos % TRANS_SEND_BUFFER; 
			m_pTransSocket->SendPacketMessage( m_sTransMainMessage._ppNetMessage[i32ReadIdx]); 
			m_sTransMainMessage._ReadPos++; 
			ui32SendCompleteCount++;
		}

		//2-2 Trans Thread 
		i32SendCount = m_sTransTransMessage._WritePos - m_sTransTransMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sTransTransMessage._ReadPos % TRANS_SEND_BUFFER; 
			m_pTransSocket->SendPacketMessage( m_sTransTransMessage._ppNetMessage[i32ReadIdx]); 
			m_sTransTransMessage._ReadPos++; 
			ui32SendCompleteCount++;
		}

		//2-3 Clan Thread 
		i32SendCount = m_sTransClanMessage._WritePos - m_sTransClanMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sTransClanMessage._ReadPos % TRANS_SEND_BUFFER; 
			m_pTransSocket->SendPacketMessage( m_sTransClanMessage._ppNetMessage[i32ReadIdx]); 
			m_sTransClanMessage._ReadPos++; 
			ui32SendCompleteCount++;
		}

		//2-4 Messenger Thread 
		i32SendCount = m_sTransMessMessage._WritePos - m_sTransMessMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sTransMessMessage._ReadPos % TRANS_SEND_BUFFER; 
			m_pTransSocket->SendPacketMessage( m_sTransMessMessage._ppNetMessage[i32ReadIdx]); 
			m_sTransMessMessage._ReadPos++; 
			ui32SendCompleteCount++;
		}

		//2-5 User Thread
		for( i = 0 ; i < m_ui32ThreadCount ; i++ )
		{
			CONNECT_SERVER_NET_BUFFER	* pUserMessage = &m_pTransUserMessage[i]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % TRANS_SEND_BUFFER;
				m_pTransSocket->SendPacketMessage( pUserMessage->_ppNetMessage[i32ReadIdx]);
				pUserMessage->_ReadPos++;
				ui32SendCompleteCount++;
			}
		}


		// 상점 버전 체크
		if( (m_pTransSocket->m_ui32ShopVersionCheckTime + TRANS_SERVER_SHOP_VERSION_CHECK_TIME) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_VERSION_REQ); 
			m_pTransSocket->SendPacketMessage(&SendPacket);
			m_pTransSocket->m_ui32ShopVersionCheckTime = i3ThreadTimer::GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pTransSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State 
	if( (m_pTransSocket->m_ui32LastRecvTime + TRANS_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		INT32 i32Rv = _TransConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect TransServer 2 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect TransServer 2 Fail" );
		}
	}
	return ui32SendCompleteCount; 
}

BOOL CModuleTrans::AuthSendLogIn(INT32 i32WorkIdx, CUserSession * pSession, char* strID, char * strPass, UINT16 ui16SessionSize, char* pui8SessionKey, UINT8 ui8LinkMethod)
{ 	
	TRANS_CHECKVALID_IDX;

	UINT8 ui8IDSize		= (UINT8)(i3String::Length( strID ) + 1);
	UINT8 ui8PassSize	= (UINT8)(i3String::Length( strPass ) + 1);
	UINT32 ui32IP		= htonl( pSession->m_ConIp ); //NC요청

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOGIN_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_ui8PBVer,				sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,				sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_ui32LoginKey,			sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32IP,								sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_MacAddress,				sizeof(UINT64) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_ui8LocaleCode,			sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8IDSize,							sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8PassSize,						sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( strID,								ui8IDSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( strPass,								ui8PassSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui16SessionSize,					sizeof(UINT16) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pui8SessionKey,						ui16SessionSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8LinkMethod, sizeof(UINT8) );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendLogOut( INT32 i32WorkIdx, CUserSession * pSession )
{ 
	_TransSendCharaInfo(i32WorkIdx, pSession);

	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_LOGOUT_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,				sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui32LoginKey,		sizeof(UINT32));
	
	pSession->UpdateAASAccMin();

	if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], pSession ) )
	{
		return FALSE; 
	}
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendGetUserInfo( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_GET_USER_INFO_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,		sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,			sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_ConIp,			sizeof(UINT32) );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendCreateChar( INT32 i32WorkIdx, CUserSession * pSession, PACKET_CHAR_CREATE_CHARA_REQ* pCreateChar, T_ItemDBIdx TCharaDBIdx )
{
	TRANS_CHECKVALID_IDX;

	UINT8 ui8NickSize = (UINT8)(pCreateChar->m_strCharaName.GetCount()+1);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_CHAR_CREATE_CHARA_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,			sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,				sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pCreateChar->m_ui8CreateType,	sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pCreateChar->m_TGoodsID,		sizeof(T_GoodsID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &TCharaDBIdx,					sizeof(T_ItemDBIdx) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pCreateChar->m_ui8Class,		sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8NickSize,					sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pCreateChar->m_strCharaName,	sizeof(TTCHAR) * ui8NickSize );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendServerEnter( INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_USER_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendServerLeave(  INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_USER_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui8LocaleCode,	sizeof(UINT8));

	pSession->UpdateAASAccMin();

	if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], pSession ) )
	{
		return FALSE; 
	}
	pNetBuffer->_WritePos++;

	_TransSendCharaInfo(i32WriteIdx, pSession);
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendChannelEnter( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_ENTER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID, sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i32ChannelNum, sizeof(INT32) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendChannelLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID, sizeof(T_UID) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendRoomEnter( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ROOM_JOIN_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID, sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i32RoomIdx, sizeof(INT32) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendRoomLeave( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ROOM_LEAVE_ROOM_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID, sizeof(T_UID) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendCreateNickName( INT32 i32WorkIdx, CUserSession * pSession, TTCHAR* pstrNickName )
{
	TRANS_CHECKVALID_IDX;
	
	UINT8 ui8NickSize = (UINT8)i3String::Length( pstrNickName ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_CREATE_NICK_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,					sizeof(TTCHAR) * ui8NickSize);
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendShopVersion( INT32 i32WorkIdx )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_VERSION_REQ );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendShopList( INT32 i32WorkIdx )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_LIST_REQ );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendUserItem( INT32 i32WorkIdx, CUserSession * pSession )
{
	//TRANS_CHECKVALID_IDX;
	
	//pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	//pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_GET_ITEMLIST_REQ );
	//pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	//pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	//pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendBuyGoodsBasket( INT32 i32WorkIdx, CUserSession * pSession, BUY_BASKET *stGoodsData, UINT8 ui8GoodsCount )
{
	TRANS_CHECKVALID_IDX;	
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8GoodsCount,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(stGoodsData,				sizeof(BUY_BASKET) * ui8GoodsCount);
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

// Goods 지급이면 AuthSendRewardItem() 함수 호출해주세요!!. 
BOOL CModuleTrans::AuthSendInsertItem( INT32 i32WorkIdx, CUserSession * pSession, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT8 ui8ItemCount, ST_ITEM* pItem )
{
	if( 0 == ui8ItemCount ) return TRUE;
	ui8ItemCount = MIN( ui8ItemCount, MAX_SHOP_DB_ITEM_COUNT );

	TRANS_CHECKVALID_IDX;

	INT8 i8RequestType = (INT8)eRequestType;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i8RequestType,			sizeof(INT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8ItemCount,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pItem,					sizeof(ST_ITEM)*ui8ItemCount);
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendRewardItem( INT32 i32WorkIdx, CUserSession * pSession, REWARD_ITEM *pReward )
{
	if( 0 == pReward->m_ui8GoodsCount ) return TRUE;

	pReward->m_ui8GoodsCount = MIN( pReward->m_ui8GoodsCount, MAX_SHOP_GOODS_BUY_COUNT );
	
	TRANS_CHECKVALID_IDX;
 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_REWARD_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pReward->m_ui8RequestType,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pReward->m_ui8GoodsCount,	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pReward->m_aTGoodsID,			sizeof(T_GoodsID) * pReward->m_ui8GoodsCount);
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendDeleteItem( INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&TItemWareIdx,			sizeof( T_ItemDBIdx));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendRepair( INT32 i32WorkIdx, CUserSession * pSession, S2MO_REPAIR_INFO *pstInfo )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_SHOP_REPAIR_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof( T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pstInfo->m_ui8ItemCount,		sizeof( UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstInfo->m_stRepairItem,		sizeof( S2MO_REPAIR_ITEM) * pstInfo->m_ui8ItemCount);
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendUserNowInfo( INT32 i32WorkIdx, INT32 i32SessionIdx, INT32 i32ChannelIdx, INT32 i32RoomIdx, INT32 i32SlotIdx )
{ 
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_USER_NOW_INFO_ACK);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32ChannelIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32RoomIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32SlotIdx,		sizeof(INT32));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

//BOOL CModuleTrans::AuthSendAliveUser(INT32 i32WorkIdx, INT32 i32SessionIdx, T_UID i64UID)
//{
//	TRANS_CHECKVALID_IDX;
//
//	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
//	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ALIVE_USER_REQ);
//	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32SessionIdx,	sizeof(INT32));	
//	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64UID,			sizeof(T_UID));	
//	pNetBuffer->_WritePos++;
//
//	return TRUE; 
//}

BOOL CModuleTrans::AuthSendAliveUser(INT32 i32WorkIdx, UINT8 ui8Count, CUserSession ** ppSession )
{
	for( INT32 i = 0; i < ui8Count; i++ )
	{
		_TransSendCharaInfo(i32WorkIdx, ppSession[i] );
	}

	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ALIVE_USER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Count, sizeof(UINT8) );
	for( INT32 i = 0; i < ui8Count; i++ )
	{
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ppSession[i]->m_SessionIdx,	sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ppSession[i]->m_i64UID,		sizeof(T_UID));

		if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], ppSession[i] ) )
		{
			return FALSE; 
		}
	}
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendUserRecord(INT32 i32WorkIdx, CUserSession * pSession, T_UID i64FUID )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64FUID,					sizeof(T_UID));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendAuthItem( INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INT32 i32ItemArg, UINT8 ui8CharaSlotIdx, UINT32 ui32ItemID)
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID) );	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&TItemWareIdx,			sizeof( T_ItemDBIdx ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32ItemArg,				sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8CharaSlotIdx,			sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,				sizeof( UINT32 ) );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendAuthItemData( INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, UINT32 ui32ItemID, UINT8 ui8Ability, void* pAbility )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&TItemWareIdx,			sizeof( T_ItemDBIdx) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,				sizeof( UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Ability,				sizeof( UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pAbility,					ui8Ability );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendAuthCountItem(INT32 i32WorkIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INT32 i32ItemArg, TLoopCnt ui16ItemCount, UINT8 ui8CharaSlotIdx, UINT32 ui32ItemID)
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_CHEAT_ITEM_AUTH_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&TItemWareIdx,			sizeof(T_ItemDBIdx));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32ItemArg,				sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8CharaSlotIdx,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui16ItemCount,			sizeof(TLoopCnt));

	pNetBuffer->_WritePos++;

	return TRUE;
}
 
BOOL CModuleTrans::AuthSendGiftGoods(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8GiftCount, BUY_BASKET *pGift, TTCHAR* strReceiverNick,  TTCHAR* pstrMessage )
{
	TRANS_CHECKVALID_IDX;
 
	UINT8 ui8MessageLength = (UINT8)(i3String::Length( pstrMessage )+1);

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof( T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->GetNickName(),		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(strReceiverNick,				sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8MessageLength,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrMessage,					sizeof(TTCHAR) * ui8MessageLength);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8GiftCount,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pGift,						sizeof(BUY_BASKET)*ui8GiftCount);
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendUserGift(INT32 i32WorkIdx, CUserSession * pSession)
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendAuthGift(INT32 i32WorkIdx, CUserSession * pSession, PACKET_AUTH_SHOP_AUTH_GIFT_REQ *pPacket )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,				sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pPacket->m_ui8Type,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pPacket->m_aGiftIdx.GetCount(),	sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pPacket->m_aGiftIdx,				(UINT16)(sizeof(T_GiftDBIdx)*pPacket->m_aGiftIdx.GetCount()));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendGetRemainMoney(INT32 i32WorkIdx, CUserSession* pSession)
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_GET_POINT_CASH_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendChangeColorNick(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_CHANGE_COLOR_NICK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Color,				sizeof(UINT8));
	pNetBuffer->_WritePos++;
	
	return TRUE;
}

BOOL CModuleTrans::AuthSendUsedWeapon( INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, USED_ITEM* pUsedItemList )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_USED_WEAPON_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,					sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,						sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoBasic.m_ui32Exp,	sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoBasic.m_ui32Point,	sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoRecord,				sizeof(USER_INFO_SEASON_RECORD) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_UserInfoLastGame,			sizeof(USER_INFO_LAST_GAME) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui16ItemCount,							sizeof(UINT16) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pUsedItemList,							sizeof(USED_ITEM)*ui16ItemCount );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendCheckNickName( INT32 i32WorkIdx, CUserSession * pSession, TTCHAR* pstrNickName )
{
	TRANS_CHECKVALID_IDX;
	
	UINT8 ui8Size = (UINT8)i3String::Length( pstrNickName )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_CHECK_NICK_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,					sizeof( UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				sizeof(TTCHAR) * ui8Size);
	pNetBuffer->_WritePos++;
	
	return TRUE;
}

BOOL CModuleTrans::TransSendAccountKick( INT32 i32WorkIdx, T_UID i64UID )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_GSM_ACCOUNT_KICK_ACK);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID, sizeof(T_UID) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::TransSendGMBlockUser( INT32 i32WorkIdx, T_UID i64UID, const TTCHAR *pstrComment)
{
	TRANS_CHECKVALID_IDX;

	UINT8 ui8CommentLength = (UINT8)i3String::Length(pstrComment);

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ROOM_GM_BLOCK_USER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,				sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8CommentLength,	sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pstrComment,			sizeof(TTCHAR) * ui8CommentLength );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::_TransSendUserInfo( i3NetworkPacket* pNetMessage, CUserSession* pSe )
{
	INT32 i32ChangeFlag = pSe->m_i32ChangeDBFlage;

	pNetMessage->WriteData( &i32ChangeFlag,									sizeof( UINT32) );

	if( SAVE_DATABASE_BASEINFO & i32ChangeFlag )
	{	// 기본 정보
		if( '\0' == pSe->m_UserInfoBasic.m_strNick[0] ) 
		{
			return FALSE;
		}
		pNetMessage->WriteData( &pSe->m_UserInfoBasic,						sizeof( USER_INFO_BASIC_BASE ) );
		pNetMessage->WriteData( &pSe->m_UserInfoServer,						sizeof( USER_INFO_SERVER ) );
	}
	if( SAVE_DATABASE_CLAN & i32ChangeFlag )
	{ 
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_ui32Clanidx,			sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_ui32ClanState,			sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_ui8ClanExpRank,			sizeof( UINT8 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_ui8ClanPerRank,			sizeof( UINT8 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_ui32ClanMark,			sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic.m_strClanName,			sizeof( TTCHAR ) * NET_CLAN_NAME_SIZE );
		pNetMessage->WriteData( &pSe->m_ui32ClanRankingDate,	sizeof( UINT32 ) );
	}
	/*if( SAVE_DATABASE_CHARA_INFO_ALL & i32ChangeFlag )
	{
		S2MO_CHAR_BASE_INFO		CharBaseInfo;
		S2MO_CHAR_SKILL_INFO	CharSkillInfo;
		ITEM_INFO				aCharEquipment[CHAR_EQUIPMENT_COUNT];

		for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
		{
			if( 0 == (i32ChangeFlag & (SAVE_DATABASE_CHARA_INFO_1<<i )) )	continue;

			pSe->m_MultiSlot.GetCharBaseInfo( i, &CharBaseInfo );
			pSe->m_MultiSlot.GetCharSkillInfo( i, &CharSkillInfo);
			pSe->m_MultiSlot.GetCharEquipmentAll( i, aCharEquipment );
			MULTI_SLOT_STATE eState = pSe->m_MultiSlot.GetActive(i);
			
			pNetMessage->WriteData( &eState,								sizeof(UINT8) );
			pNetMessage->WriteData( &CharBaseInfo,							sizeof(S2MO_CHAR_BASE_INFO) );
			pNetMessage->WriteData( &CharSkillInfo,							sizeof(S2MO_CHAR_SKILL_INFO) );
			pNetMessage->WriteData(aCharEquipment,							sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT);
		}
	}*/
	if( SAVE_DATABASE_CHARA_COMMON & i32ChangeFlag )
	{
		ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
		pSe->m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
		pNetMessage->WriteData( aCommonEquips,								sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COMMON_COUNT );
		UINT8 ui8RedTeamSlot	= (UINT8)pSe->m_MultiSlot.GetTeamSlot(TEAM_RED);
		UINT8 ui8BlueTeamSlot	= (UINT8)pSe->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
		pNetMessage->WriteData( &ui8RedTeamSlot,							sizeof(UINT8) );
		pNetMessage->WriteData( &ui8BlueTeamSlot,							sizeof(UINT8) );
	}
	if(	SAVE_DATABASE_RECORD & i32ChangeFlag )
	{	// 전적 정보
		pNetMessage->WriteData( &pSe->m_UserInfoRecord,						sizeof(USER_INFO_SEASON_RECORD) );
		// 당일 전적 전송
		pNetMessage->WriteData( &pSe->m_UserInfoDaily,						sizeof( USER_INFO_DAILY) );
	}
	if(	SAVE_DATABASE_QUEST & i32ChangeFlag )
	{	// 임무
		pNetMessage->WriteData( &pSe->m_ui32QuestEventDate,					sizeof( UINT32 ) );
		pNetMessage->WriteData( pSe->m_aaui8QuestCtxActive,					sizeof( UINT8 )*MAX_QUEST_ALL_USE_NEW );
		pNetMessage->WriteData( &pSe->m_QuestIngInfo,						sizeof( QUESTING_INFO ) );
		pNetMessage->WriteData( &pSe->m_ui32Miniature,						sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Insignia,						sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Order,							sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Master,							sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui64GainUserTitle,					sizeof( UINT64 ) );
		pNetMessage->WriteData( pSe->m_aui8EquipUserTitle,					sizeof( UINT8 )*MAX_EQUIP_USER_TITLE );
	}


	// 1. Medal Basic Info Save
	pSe->UpdateNotufyMedalCount(); // 알리미 등록 메달의 미션 달성 횟수 업데이트.
	if( SAVE_DATABASE_MEDAL_BASIC & i32ChangeFlag )
	{	
		pNetMessage->WriteData( pSe->m_MedalAllInfo.m_aNotifyMedal,			sizeof( NOTIFY_MEDAL ) * MAX_NOTIFY_MADAL_COUNT );
		//*pui32PacketSize += DB_SIZE_MEDAL_BASIC ;
	}


	// 2. Normal Medal Save
	if( SAVE_DATABASE_MEDAL & i32ChangeFlag )
	{
		MEDAL	SaveMedal[MAX_MEDAL_COUNT];
		i3mem::FillZero( SaveMedal, sizeof( MEDAL ) * MAX_MEDAL_COUNT );
		UINT16 ui16SaveMedalCount = 0;	// 전송할 일반 메달 갯수

		for( INT32 Type = MEDAL_MC_CHECK_POSITION_START ; Type < MEDAL_MC_CHECK_POSITION_MAX ; Type++ )
		{
			for( INT32 MedalArrIdx = 0 ; MedalArrIdx < pSe->m_MedalAllInfo.m_aui16MedalCount[Type] ; MedalArrIdx++ )
			{
				if( 1 == pSe->m_MedalAllInfo.m_aMedal[Type][MedalArrIdx].m_ui8IsSave ) 
				{
					SaveMedal[ui16SaveMedalCount]	= pSe->m_MedalAllInfo.m_aMedal[Type][MedalArrIdx].m_Medal;
					ui16SaveMedalCount++;
				}
			}
		}

		pNetMessage->WriteData( &ui16SaveMedalCount,						sizeof( UINT16 ));
		//*pui32PacketSize += DB_SIZE_MEDAL ;

		pNetMessage->WriteData( SaveMedal,									sizeof( MEDAL ) * ui16SaveMedalCount );
		//*pui32PacketSize += DB_SIZE_MEDAL_INFO * ui16SaveMedalCount;
	}


	// 3. Current Medal Set Save
	if( SAVE_DATABASE_CUR_MEDAL_SET & i32ChangeFlag )
	{
		pNetMessage->WriteData( pSe->m_MedalAllInfo.m_aCurMedalSet,			sizeof( CUR_MEDAL_SET ) * MAX_CUR_MEDAL_SET_COUNT);
		//*pui32PacketSize += DB_SIZE_CUR_MEDAL_SET ;
	}

	// QuickJoin
	if( SAVE_DATABASE_QUICK_JOIN & i32ChangeFlag )
	{
		pNetMessage->WriteData( pSe->m_aQuickJoinInfo,						sizeof( QUICKJOIN_INFO ) * MAX_QUICK_JOIN_INFO_COUNT );	
		//*pui32PacketSize += PACKET_SIZE_SAVE_DATABASE_QUICKJOIN ;
	}

	// AAS
	if( SAVE_DATABASE_AAS & i32ChangeFlag )
	{
		pNetMessage->WriteData( &pSe->m_ui16AASAccMin,						sizeof( UINT16 ) );
	}

	if(	SAVE_DATABASE_ITEM & i32ChangeFlag )
	{	//계정 아이템 사용 정보
		pNetMessage->WriteData( &pSe->m_UserInfoInvItemData,				sizeof( USER_INFO_INVITEM_DATA ) );
	}

	UINT32 ui32CheckNum = 0x123ABCDE;

	pNetMessage->WriteData( &ui32CheckNum,									sizeof( UINT32 ));

	return TRUE; 
}

BOOL CModuleTrans::_TransSendCharaInfo( INT32 i32WorkIdx, CUserSession* pSe )
{
	UINT64 ui64ChangeFlag = pSe->m_ui64ChangeCharaDBFlage & 0x0FFFFFFFFFFFFFFF;	// 최상위 비트에서 S2MO_MULTI_SLOT_PCCAFE_CHARA_COUNT	4 개 만큼은 DB 저장 제외 시킴.
	if( ui64ChangeFlag == 0 ) return FALSE;

	if( '\0' == pSe->m_UserInfoBasic.m_strNick[0] )  return FALSE;
	
	UINT8	ui8Count = 0;		// 한 패킷에서 보낼 캐릭터 수
	INT32	i32CharaSLot[S2MO_MULTI_SLOT_PACKET_COUNT];
	i3mem::Fill( i32CharaSLot, -1, sizeof( i32CharaSLot ) );

	for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
	{
		UINT64 ui64CheckSlot = (UINT64)0x01 << i; 
		if( 0 == (ui64ChangeFlag & ui64CheckSlot ) )	continue;

		if( ui8Count > 0 && 0 == ui8Count % S2MO_MULTI_SLOT_PACKET_COUNT )
		{
			_TransSendCharaPacket( ui8Count, i32CharaSLot, i32WorkIdx, pSe );

			ui8Count = 0;
			i3mem::Fill( i32CharaSLot, -1, sizeof( i32CharaSLot ) );
		}

		i32CharaSLot[ui8Count] = (INT32)i;
		++ui8Count;
	}

	if( ui8Count > 0 )
	{
		_TransSendCharaPacket( ui8Count, i32CharaSLot, i32WorkIdx, pSe );
	}
 
	return TRUE; 
}

BOOL CModuleTrans::_TransSendCharaPacket( UINT8 ui8Count, INT32* pi32CharaSlot, INT32 i32WorkIdx, CUserSession* pSe )
{ 
	S2MO_CHAR_BASE_INFO		CharBaseInfo;
	S2MO_CHAR_SKILL_INFO	CharSkillInfo;
	ITEM_INFO				aCharEquipment[CHAR_EQUIPMENT_COUNT];

	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_CHARA_DB_SAVE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSe->m_i64UID,		sizeof(T_UID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Count, sizeof(UINT8) );

	for( INT32 i = 0 ; i < ui8Count ; i++ )
	{
		INT32 i32CharaSlot = pi32CharaSlot[i];
 
		pSe->m_MultiSlot.GetCharBaseInfo( i32CharaSlot, &CharBaseInfo );
		pSe->m_MultiSlot.GetCharSkillInfo( i32CharaSlot, &CharSkillInfo);
		pSe->m_MultiSlot.GetCharEquipmentAll( i32CharaSlot, aCharEquipment );

		MULTI_SLOT_STATE eState = pSe->m_MultiSlot.GetActive(i32CharaSlot);

		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32CharaSlot,							sizeof(UINT8) );
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &eState,									sizeof(UINT8) );
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &CharBaseInfo,							sizeof(S2MO_CHAR_BASE_INFO) );
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &CharSkillInfo,							sizeof(S2MO_CHAR_SKILL_INFO) );
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( aCharEquipment,							sizeof(ITEM_INFO)*CHAR_EQUIPMENT_COUNT);
	}

	UINT32 ui32CheckNum = 0x123ABCDE;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32CheckNum, sizeof(UINT32) ); 
	pNetBuffer->_WritePos++;
	

	return TRUE; 
}

#ifdef USE_TRANS_FIND
// 유저 찾기.
BOOL CModuleTrans::AuthSendFindNickName( INT32 WorkIdx, CUserSession * pSession, char* pstrNickName )
{
	TRANS_CHECKVALID_IDX;
	
	UINT8 ui8Size = (UINT8)i3String::Length( strNickName )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_FIND_NICK_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,					sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				ui8Size);
	pNetBuffer->_WritePos++;
	
	return TRUE;
}
#endif

//---------------------------------------------------------------------------------
// 출석체크 이벤트 - 서동권 2012.01.26
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendAttendanceReq( INT32 i32WorkIdx, T_UID i64UID, UINT32 ui8AttendanceNum, UINT8 ui8AttendanceType )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_ATTENDANCE_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,					sizeof( T_UID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceNum,		sizeof( UINT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceType,		sizeof( UINT8 ) );
	pNetBuffer->_WritePos++;
	
	return TRUE;
}

//---------------------------------------------------------------------------------
// 출석체크 이벤트 - 서동권 2011.07.07
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendAttendanceItemReq( INT32 i32WorkIdx, T_UID i64UID, UINT32 ui8AttendanceNum, UINT8 ui8AttendanceItemNum, UINT8 ui8AttendanceType )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,					sizeof( T_UID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceNum,		sizeof( UINT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceItemNum,	sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceType,		sizeof( UINT8 ) );
	pNetBuffer->_WritePos++;
	
	return TRUE;
}

BOOL CModuleTrans::AuthSendSetBirthDay(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32BirthDay )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_LOBBY_SET_BIRTHDAY_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( T_UID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32BirthDay,			sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}


BOOL CModuleTrans::AuthSendExtendItem( INT32 i32WorkIdx, CUserSession * pSession, EXTEND_BASKET* stExtendItemData )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(stExtendItemData,			sizeof(EXTEND_BASKET));
	pNetBuffer->_WritePos++;

	return TRUE; 

}

BOOL CModuleTrans::AuthSendCharaDelete( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8CharaSlotIdx, T_ItemDBIdx TItemDBIdx )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_CHAR_DELETE_CHARA_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8CharaSlotIdx,			sizeof( UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&TItemDBIdx,				sizeof( T_ItemDBIdx));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendBasicUserInfoNick( INT32 i32WorkIdx, CUserSession * pSession, TTCHAR * strNickName )
{
	TRANS_CHECKVALID_IDX;
	
	UINT8 ui8NickSize = (UINT8)i3String::Length( strNickName ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_GET_USER_INFO_NICK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,			sizeof( T_UID ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8NickSize,				sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( strNickName,					sizeof(TTCHAR) * ui8NickSize );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendBasicUserInfoUID( INT32 i32WorkIdx, CUserSession * pSession, T_UID i64UID )
{
	TRANS_CHECKVALID_IDX
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_GET_USER_INFO_UID_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,			sizeof( T_UID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,						sizeof(T_UID) );
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendGetUserInfoDetailInfo( INT32 i32WorkIdx, CUserSession * pSession, T_UID T_UID )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_FIND_UID_GET_USER_DETAIL_INFO_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,			sizeof( T_UID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &T_UID,						sizeof( T_UID ) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendChangeClanMark( INT32 i32WorkIdx, UINT32 ui32ClanMark, UINT8 ui8SendUserCount, T_UID* pTUIDList )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_CHANGE_CLAN_MARK_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32ClanMark,				sizeof( UINT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8SendUserCount,			sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pTUIDList,					sizeof( T_UID )*ui8SendUserCount );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendChangeClanName( INT32 i32WorkIdx, TTCHAR* strClanName, UINT8 ui8SendUserCount, T_UID* pTUIDList )
{
	TRANS_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_CHANGE_CLAN_NAME_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( strClanName,					sizeof(TTCHAR) * NET_CLAN_NAME_SIZE );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8SendUserCount,			sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pTUIDList,					sizeof( T_UID )*ui8SendUserCount );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::CommunitySendInviteUser(	INT32 i32WorkIdx, PACKET_COMMUNITY_USER_INVITED_REQ* pPacketReq)
{ 
	TRANS_CHECKVALID_IDX;

	PACKET_TRANS_COMMUNITY_USER_INVITED_REQ	MessPacketReq;
	MessPacketReq.m_i32SessionIdx	= pPacketReq->m_SessionIdx;
	MessPacketReq.m_i64UID			= pPacketReq->m_i64UID;
	MessPacketReq.m_i64InviteUser	= pPacketReq->m_i64InviteUser;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_COMMUNITY_USER_INVITED_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&MessPacketReq,	sizeof(PACKET_TRANS_COMMUNITY_USER_INVITED_REQ));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::SendGachaPurchase( INT32 i32WorkIdx, INT64 i64UID, UINT8 ui8BuyType, UINT32 TGoodsID, INT32 i32ItemIdx )
{
	TRANS_CHECKVALID_IDX;

	PACKET_GACHA_SS_PURCHASE_REQ stSSPurchase;
	stSSPurchase.m_TUID				= i64UID;
	stSSPurchase.m_ui8BuyType		= ui8BuyType;	
	stSSPurchase.m_TGoodsID			= TGoodsID;	
	stSSPurchase.m_i32ItemIdx		= i32ItemIdx;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_SS_GACHA_PURCHASE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&stSSPurchase, sizeof(PACKET_GACHA_SS_PURCHASE_REQ));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendMedalReward( INT32 i32WorkIdx, CUserSession * pSession, MEDAL_REWARD_INFO* pMedalRewadInfo )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_GET_MEDAL_REWARD_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,				sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pMedalRewadInfo->m_ui8Count,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pMedalRewadInfo->m_aDetail,		sizeof(MEDAL_REWARD_DETAIL_INFO) * pMedalRewadInfo->m_ui8Count );
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendApplyPenalty( INT32 i32WorkIdx, CUserSession * pSession, PACKET_GMCHAT_APPLY_PENALTY_REQ* pApplyPenaltyReq )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_GMCHAT_APPLY_PENALTY_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,					sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pApplyPenaltyReq->m_i64UserUID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pApplyPenaltyReq->m_ui8PenaltyType,		sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pApplyPenaltyReq->m_i32PenaltyAmount,	sizeof(INT32));
	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

BOOL CModuleTrans::AuthSendUseCoupon( INT32 i32WorkIdx, CUserSession * pSession, char* pstrCoupon )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_USE_GIFTCOUPON_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrCoupon,					GIFTCOUPON_LENGTH + 1);
	pNetBuffer->_WritePos++;
	
	return TRUE;
}

BOOL CModuleTrans::AuthSendExpireDeleteItem(INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, T_ItemDBIdx  * pTItemDBIdx)
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_EXPIRE_DELETE_ITEM_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,	sizeof(T_UID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui16ItemCount,		sizeof(UINT16));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pTItemDBIdx,			sizeof(T_ItemDBIdx)*ui16ItemCount);
	pNetBuffer->_WritePos++;
	return TRUE;
}