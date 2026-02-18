#include "pch.h"
#include "ModuleTrans.h"
#include "ModuleTransSocket.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CModuleTrans, i3Thread );

CModuleTrans * g_pModuleTrans = NULL; 

//////////////////////////////////////////////////////////////
//Trans
#define TRANS_MAX_SEND_IDX_COUNT				2000000000
#define TRANS_RECONNECT_TIME					10
#define TRANS_SERVER_HEART_BIT					25
#define TRANS_SERVER_HEART_BIT_REQ_TIME			60
#define TRANS_SEND_BUFFER						500
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TRANS_CHECKVALID_IDX		if( !m_pTransSocket->IsActive())return FALSE; \
	SERVER_NET_BUFFER	* pNetBuffer = NULL; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sTransMainMessage;			break; \
	case	MODULE_CONTROL_THREAD_IDX	: I3ASSERT_0;									break; \
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
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleTrans WriteBuffer FULL", WARNING_NETWORK );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= TRANS_SEND_BUFFER )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleTrans WriteBuffer Overflow", WARNING_NETWORK );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % TRANS_SEND_BUFFER;



CModuleTrans::CModuleTrans(void)
{
	m_ui8RSEnable				= 0;
	m_bRunning					= FALSE;
	m_ui32ThreadCount			= 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Trans	
	m_pTransSocket				= NULL;
	m_ui32AuthConnectTime		= 0;
	m_pTransUserMessage			= NULL;

	i3mem::FillZero( &m_sTransMainMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransTransMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransClanMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_sTransMessMessage,	sizeof( SERVER_NET_BUFFER ) );
	i3mem::FillZero( &m_stRSItemInfo,		sizeof( RS_ALL_ITEM_INFO ) );
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
	m_pTransUserMessage = (SERVER_NET_BUFFER*) i3MemAlloc(sizeof(SERVER_NET_BUFFER) * m_ui32ThreadCount); 
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
	if( FALSE == m_pTransSocket->OnCreate(g_pContextMain->m_strTransServerIp, g_pContextMain->m_ui16AuthServerPort) ) 	return SERVER_ERROR_AUTH;
	
	m_ui32AuthConnectTime		= g_pContextMain->GetServerTime();

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
	if( m_ui32AuthConnectTime + TRANS_RECONNECT_TIME > g_pContextMain->GetServerTime() )	return 1;
	m_ui32AuthConnectTime = g_pContextMain->GetServerTime();

	m_pTransSocket->OnDestroy(); 
	if( m_pTransSocket->OnCreate(g_pContextMain->m_strTransServerIp, g_pContextMain->m_ui16AuthServerPort) )
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
			g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL,"ReConnect TransServer 1 Success", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL,"ReConnect TransServer 1 Fail", WARNING_NETWORK );
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
			SERVER_NET_BUFFER	* pUserMessage = &m_pTransUserMessage[i]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % TRANS_SEND_BUFFER;
				m_pTransSocket->SendPacketMessage( pUserMessage->_ppNetMessage[i32ReadIdx]);
				pUserMessage->_ReadPos++;
				ui32SendCompleteCount++;
			}
		}

		//하트 비트를 보낸다. 
		if( (m_pTransSocket->m_ui32LastSendTime + TRANS_SERVER_HEART_BIT) < g_pContextMain->GetServerTime() )
		{
			i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
			m_pTransSocket->SendPacketMessage(&SendPacket);
			m_pTransSocket->m_ui32LastSendTime = g_pContextMain->GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pTransSocket->SelectEvent(); 		

	//-------------------------------------------------------------------------------
	//4. Check Connect State 
	if( (m_pTransSocket->m_ui32LastRecvTime + TRANS_SERVER_HEART_BIT_REQ_TIME) < g_pContextMain->GetServerTime() )
	{
		INT32 i32Rv = _TransConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL,"ReConnect TransServer 2 Success", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL,"ReConnect TransServer 2 Fail", WARNING_NETWORK );
		}
	}
	return ui32SendCompleteCount; 
}

BOOL CModuleTrans::AuthSendLogIn(INT32 i32WorkIdx, CUserSession * pSession, char * pPass )
{ 	
	TRANS_CHECKVALID_IDX;

	UINT8 ui8IDSize		= (UINT8)(i3String::Length( pSession->m_strId ) + 1);
	UINT8 ui8PassSize	= (UINT8)(i3String::Length( pPass ) + 1);

	UINT32 ui32IpAdd;			//NC요청 
	ui32IpAdd = htonl( pSession->m_ConIp ); //NC요청

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_LOGIN_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui32LoginKey,	sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8IDSize,					sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8PassSize,					sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_strId,			ui8IDSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pPass,						ui8PassSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32IpAdd,					sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui64MacAddress,		sizeof(TUID) );
	
#if defined( USE_INDONESIA_PCBANG )
	// 인도네시아에서 사용하는 GWarnet 레지스트리 값 ( PC방 )
	if( pSession->m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
	{
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_strNesiaPCbangKey,	MAX_INDONESIA_PCBANG_KEY		);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_ui8NesiaPCbangHWInfo,	sizeof(UINT8)*DATASIZE_NO_CHECK	);
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_dwNesiaPCbangHWKey,	sizeof(DWORD)					);
	}
#endif

	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleTrans::AuthSendLogInWEB(	INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_LOGIN_WEBKEY_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui32LoginKey,	sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_strWebKey,			NET_SESSION_KEY_SIZE);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ConIp,			sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui64MacAddress,		sizeof(UINT64));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendLogInRussia( INT32 i32WorkIdx, CUserSession * pSession, char * pPass )
{ 
	TRANS_CHECKVALID_IDX;

	UINT32 ui32IpAdd;
	ui32IpAdd = htonl( pSession->m_ConIp );

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_LOGIN_WEBKEY_RUSSIA_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui32LoginKey,		sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pSession->m_strId,				MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pPass,							MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32IpAdd,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui64MacAddress,		sizeof(UINT64));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendLogInThai(INT32 i32WorkIdx, CUserSession * pSession, INT16 i16TokenSize, char * pToken)
{
	TRANS_CHECKVALID_IDX;

	UINT32 ui32IpAdd;
	ui32IpAdd = htonl( pSession->m_ConIp );

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_LOGIN_THAI_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,			sizeof(INT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui32LoginKey,		sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i16TokenSize,					sizeof(INT16) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pToken,							i16TokenSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32IpAdd,						sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui64MacAddress,		sizeof(UINT64));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendLogOut( INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_LOGOUT_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], pSession ) )
	{
		return FALSE; 
	}
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendGetMyInfo( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_GET_MYINFO_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ConIp,		sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendServerEnter( INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_USER_ENTER_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendServerLeave(  INT32 i32WorkIdx, CUserSession * pSession )
{ 
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_USER_LEAVE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));		
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_ui8LocaleCode,	sizeof(UINT8));
	if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], pSession ) )
	{
		return FALSE; 
	}

	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendCreateNickName( INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName )
{
	TRANS_CHECKVALID_IDX;
	UINT8 ui8NickSize = (UINT8)i3String::Length( pstrNickName ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_CREATE_NICK_NAME_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				ui8NickSize);
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
	//pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	//pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendBuyGoods( INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT32 ui32CouponID, INT64 i64CouponIdx )
{
	TRANS_CHECKVALID_IDX;
	UINT8 ui8GoodsCount = 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8GoodsCount,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32GoodsID,				sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8BuyType,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32CouponID,			sizeof(UINT32)); // 할인쿠폰 아이디를 넘겨서 SIA 에서 다시 검사할수 있도록
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64CouponIdx,			sizeof(INT64));  // 할인쿠폰 인덱스를 넘김
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendBuyGoodsBasket( INT32 i32WorkIdx, CUserSession * pSession, BUY_BASKET *pGoodsData, UINT8 ui8GoodsCount )
{
// 장바구니
	TRANS_CHECKVALID_IDX;	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8GoodsCount,			sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pGoodsData,				sizeof(BUY_BASKET) * ui8GoodsCount);
	pNetBuffer->_WritePos++;
	return TRUE; 
}


BOOL CModuleTrans::AuthSendInsertItem( INT32 i32WorkIdx, CUserSession * pSession, ITEM_INSERT_REQUEST_TYPE eRequestType, UINT32 ui32ItemID, UINT32 ui32AuthDay )
{
	// 2011.04.13 기존 AuthType 을 받아서 날짜일 경우 86400 을 곱해주던 코드는 제거하였습니다. - 서동권
	// 퀘스트 보상으로 지급되는 아이템의 경우 기간제일 경우에는 맴버변수인 Arg에 자동으로 곱해진 값이 들어가 있으므로 그대로 사용하시면 되고
	// 코드로 직접 기입하여 쓰실 경우에는 기간제인 경우를 잘 고려하셔서 * 86400 만 적어주시면 되겠습니다.

	TRANS_CHECKVALID_IDX;

	INT8 i8RequestType = (INT8)eRequestType;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_INSERT_ITEM_REQ ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i8RequestType,			sizeof(INT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,				sizeof(UINT32));	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32AuthDay,				sizeof(UINT32));
	
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendDeleteItem( INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64ItemWareIdx,			sizeof( TUID));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendRepairItem( INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, UINT32 ui32Point )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_REPAIR_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64ItemWareIdx,			sizeof( TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Point,				sizeof( UINT32));
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

BOOL CModuleTrans::AuthSendAliveUser(INT32 i32WorkIdx, UINT8 ui8Count, CUserSession ** ppSession )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ALIVE_USER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8Count, sizeof(UINT8) );
	for( INT32 i = 0; i < ui8Count; i++ )
	{
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ppSession[i]->m_SessionIdx, sizeof(INT32));
		pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ppSession[i]->m_i64UID,		sizeof(TUID));

		if( !_TransSendUserInfo( pNetBuffer->_ppNetMessage[i32WriteIdx], ppSession[i] ) )
		{
			return FALSE; 
		}
	}

	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendUserRecord(INT32 i32WorkIdx, CUserSession * pSession, TUID i64FUID )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64FUID,					sizeof(TUID));
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendAuthItem( INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, INT32 i32ItemArg )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( TUID) );	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64ItemWareIdx,			sizeof( TUID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32ItemArg,				sizeof( INT32) );
	pNetBuffer->_WritePos++;
	return TRUE; 
}

BOOL CModuleTrans::AuthSendAuthItemData( INT32 i32WorkIdx, CUserSession * pSession, TUID i64ItemWareIdx, UINT32 ui32ItemID, UINT8 ui8Ability, void* pAbility )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof( TUID) );	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i64ItemWareIdx,			sizeof( TUID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32ItemID,				sizeof( UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Ability,				sizeof( UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pAbility,					ui8Ability );
	pNetBuffer->_WritePos++;
	return TRUE; 
}
 
BOOL CModuleTrans::AuthSendGiftGoods(INT32 i32WorkIdx, CUserSession * pSession, char* pstrReceiverNick, UINT32 ui32GoodsID, char* pstrMessage )
{
	TRANS_CHECKVALID_IDX;
 
	UINT8 ui8NickLength = (UINT8)(i3String::Length( pstrReceiverNick )+1);
	UINT8 ui8MessageLength = (UINT8)(i3String::Length( pstrMessage )+1);


	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,		sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,			sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32GoodsID,					sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickLength,				sizeof(INT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrReceiverNick,				ui8NickLength);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8MessageLength,			sizeof(INT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrMessage,					ui8MessageLength);
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendUserGift(INT32 i32WorkIdx, CUserSession * pSession)
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendAuthGift(INT32 i32WorkIdx, CUserSession * pSession, TGiftDxIdx i32GiftWareIdx )
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&i32GiftWareIdx,			sizeof(TGiftDxIdx));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendGetRemainMoney(INT32 i32WorkIdx, CUserSession* pSession)
{
	TRANS_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_GET_POINT_CASH_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendChangeRank(INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Rank )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_CHANGE_RANK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32Rank,				sizeof(UINT32));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendChangeColorNick(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Color )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_CHANGE_COLOR_NICK_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Color,				sizeof(UINT8));
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendUsedWeapon( INT32 i32WorkIdx, CUserSession * pSession, UINT16 ui16ItemCount, USED_ITEM* pUsedItemList )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_USED_WEAPON_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui16ItemCount,			sizeof(UINT16));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pUsedItemList,			sizeof(USED_ITEM)*ui16ItemCount);

	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendCheckNickName( INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName )
{
	TRANS_CHECKVALID_IDX;
	UINT8 ui8Size = (UINT8)i3String::Length( pstrNickName )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_CHECK_NICK_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,					sizeof( UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrNickName,				ui8Size);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::TransSendAccountKick( INT32 i32WorkIdx, TUID i64UID )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_GSM_ACCOUNT_KICK_ACK);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID, sizeof(TUID) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::TransSendGMBlockUser( INT32 i32WorkIdx, TUID i64UID, const char *pstrComment)
{
	TRANS_CHECKVALID_IDX;

	UINT8 ui8CommentLength = (UINT8)i3String::Length(pstrComment);

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_ROOM_GM_BLOCK_USER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,				sizeof(TUID) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8CommentLength,	sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pstrComment,			ui8CommentLength );

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::_TransSendUserInfo( i3NetworkPacket* pNetMessage, CUserSession* pSe )
{
	UINT8 ui8ChangeFlag = pSe->m_ui8ChangeDBFlage;

	pNetMessage->WriteData( &ui8ChangeFlag,				sizeof( UINT8) );

	if( SAVE_DATABASE_BASEINFO & ui8ChangeFlag )
	{	// 기본 정보
		pNetMessage->WriteData( pSe->m_UserInfoBasic._nick,			NET_NICK_NAME_SIZE );
		if( '\0' == pSe->m_UserInfoBasic._nick[0] ) 
		{
			return FALSE;
		}
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._rank,				sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._point,				sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._exp,					sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._keep_rank,			sizeof( UINT32 ) );		
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._ui32AbusingCount,	sizeof( UINT32 ) );	
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._nickcolor,			sizeof( UINT8 ) );	
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._ui8TutorialIng,		sizeof( UINT8 ) );
		pNetMessage->WriteData( &pSe->m_ui32LoginBattleTime,				sizeof( UINT32 ) );	
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._ui32TotalBattleTime,	sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._ui32LastBuyCash,		sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui8ReportCount,						sizeof( UINT8 ) );	
	}
	if( SAVE_DATABASE_CLAN & ui8ChangeFlag )
	{
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanidx,		sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanstate,	sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanExpRank,	sizeof( UINT8 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanPerRank,	sizeof( UINT8 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanMark,	sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_UserInfoBasic._clanName,	NET_CLAN_NAME_SIZE );
	}
	if( SAVE_DATABASE_EQUIPMENT & ui8ChangeFlag )
	{	// 전적
		pNetMessage->WriteData( &pSe->m_UserInfoEquipmentChara,		sizeof( USER_INFO_EQUIPMENT_CHARA ) );
		pNetMessage->WriteData( &pSe->m_UserInfoEquipmentWeapon,	sizeof( USER_INFO_EQUIPMENT_WEAPON ) );
	}
	if(	SAVE_DATABASE_RECORD & ui8ChangeFlag )
	{	// 장착
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._smatch,		sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._swin,		sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._slose,		sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sdraw,		sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._skillcount,	sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sheadshot,	sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sdeath,		sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sdmatch,	sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sdkillcount,sizeof(UINT32) );
		pNetMessage->WriteData( &pSe->m_UserInfoRecord._sdis,		sizeof(UINT32) );
	}
	if(	SAVE_DATABASE_MEDAL & ui8ChangeFlag )
	{	// 임무

		pNetMessage->WriteData( &pSe->m_ui32QuestEventDate,			sizeof( UINT32 ) );
		pNetMessage->WriteData( pSe->m_aaui8QuestCtxActive,			sizeof( pSe->m_aaui8QuestCtxActive ) );
		pNetMessage->WriteData( &pSe->m_QuestIngInfo,				sizeof( QUESTING_INFO ) );
		pNetMessage->WriteData( &pSe->m_ui32Miniature,				sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Insignia,				sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Order,					sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui32Master,					sizeof( UINT32 ) );
		pNetMessage->WriteData( &pSe->m_ui64GainUserTitle,			sizeof( UINT64 ) );
		pNetMessage->WriteData( pSe->m_aui8EquipUserTitle,			sizeof( pSe->m_aui8EquipUserTitle ) );
	}

	// 당일 전적 전송
	pNetMessage->WriteData( &pSe->m_UserInfoDaily,					sizeof( USER_INFO_DAILY) );

	UINT32 ui32CheckNum = 0x123ABCDE;

	pNetMessage->WriteData( &ui32CheckNum,							sizeof( UINT32 ));

	return TRUE; 
}

#ifdef USE_TRANS_FIND
// 유저 찾기.
BOOL CModuleTrans::AuthSendFindNickName( INT32 i32WorkIdx, CUserSession * pSession, char* strNickName )
{
	TRANS_CHECKVALID_IDX;
	UINT8 ui8Size = (UINT8)i3String::Length( strNickName )+1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_LOBBY_FIND_NICK_NAME_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx, sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8Size,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(strNickName,				ui8Size);
	pNetBuffer->_WritePos++;
	return TRUE;
}
#endif

//---------------------------------------------------------------------------------
// 보급상자
//---------------------------------------------------------------------------------
BOOL CModuleTrans::BattleSendSupplyBoxResultReq(INT32 i32WorkIdx, INT32 i32ChannelNum, INT32 i32RoomIdx, UINT32 ui32RoomStateCheckTime, INT8 i8BattleMemberCount, USER_RANK_INFO *pUserRankInfo )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SUPPLY_BOX_RESULT_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32ChannelNum,			sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32RoomIdx,				sizeof(INT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32RoomStateCheckTime,	sizeof(UINT32) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i8BattleMemberCount,	sizeof(INT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pUserRankInfo,			sizeof(USER_RANK_INFO)*i8BattleMemberCount );
	pNetBuffer->_WritePos++;
	return TRUE;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Award Item : 보상 내려받기
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendCouponAwardItemReq(INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8CouponNumSize, char* pstrCouponNum )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_AUTH_COUPON_AWARD_ITEM_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&pSession->m_i64UID,		sizeof(TUID));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8CouponNumSize,		sizeof( UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pstrCouponNum,			ui8CouponNumSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
// 출석체크 이벤트 - 서동권 2012.01.26
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendAttendanceReq( INT32 i32WorkIdx, INT64 i64UID, UINT32 ui32AttendanceNum )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_ATTENDANCE_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,				sizeof( INT64 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32AttendanceNum,	sizeof( UINT32 ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

//---------------------------------------------------------------------------------
// 출석체크 이벤트 - 서동권 2012.01.26
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendAttendanceInfoReq( INT32 i32WorkIdx, INT64 i64UID )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_ATTENDANCE_INFO_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,				sizeof( INT64 ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

//---------------------------------------------------------------------------------
// 출석체크 이벤트 - 서동권 2011.07.07
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendAttendanceItemReq( INT32 i32WorkIdx, INT64 i64UID, UINT32 ui32AttendanceNum, UINT8 ui8AttendanceItemNum )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,					sizeof( INT64 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32AttendanceNum,		sizeof( UINT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8AttendanceItemNum,	sizeof( UINT8 ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

//---------------------------------------------------------------------------------
// 뽑기 상점 
//---------------------------------------------------------------------------------
BOOL CModuleTrans::AuthSendRSStartReq( INT32 i32WorkIdx, CUserSession * pSession, RS_ROULETTE_INFO* pRSRouletteInfo, BOOL bIsHidden )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_RS_START_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &bIsHidden,				sizeof( BOOL ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRSRouletteInfo,			sizeof( RS_ROULETTE_INFO ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendRSEnterReq( INT32 i32WorkIdx, CUserSession * pSession  )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_RS_ENTER_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendRSItemInfoReq( INT32 i32WorkIdx, CUserSession * pSession )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_RS_ITEM_INFO_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendRSJackPotNotifyReq( INT32 i32WorkIdx, CUserSession * pSession , char * strNickName )
{
	TRANS_CHECKVALID_IDX;
	UINT8 ui8NickSize = (UINT8)i3String::Length( strNickName ) + 1;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_RS_JACKPOT_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8NickSize,				sizeof(UINT8));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(strNickName,				ui8NickSize);
	pNetBuffer->_WritePos++;
	return TRUE;
}

BOOL CModuleTrans::AuthSendSetBirthDay(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32BirthDay )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_LOBBY_SET_BIRTHDAY_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof(INT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32BirthDay,			sizeof(UINT32));
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendCheckSID( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IDSize, char* pstrSecurityID )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SECURITYID_DUPLICATE_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8IDSize,				sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pstrSecurityID,			ui8IDSize );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendCreateSID( INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8IDSize, char* pstrSecurityID, UINT8 ui8PWSize, char* pstrSecurityPW )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SECURITYID_CREATE_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_SessionIdx,	sizeof( INT32 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pSession->m_i64UID,		sizeof( TUID ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8IDSize,				sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pstrSecurityID,			ui8IDSize );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8PWSize,				sizeof( UINT8 ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pstrSecurityPW,			ui8PWSize );
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleTrans::AuthSendTestCreateSID( INT32 i32WorkIdx )
{
	TRANS_CHECKVALID_IDX;
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_AUTH_SECURITYID_TEST_CREATE_REQ );
	pNetBuffer->_WritePos++;
	return TRUE;
}