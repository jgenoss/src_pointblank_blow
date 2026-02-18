#include "pch.h"
#include "ModuleSIA.h"
#include "ModuleSIASocket.h"
#include "TaskProcessor.h"

CModuleSIA * g_pModuleSIA		= NULL; 

#define MODULE_CONTEXT_SEND_COUNT			10
#define MODULE_CONTEXT_MAX_SEND_IDX_COUNT	2000000000		
#define SIA_SEND_BUFFER_COUNT				500
#define SIA_RECONNECT_TIME					1000 *	30	//MS
#define SIA_SERVER_HEART_BIT				60
#define SIA_CONNECT_TIMEOUT					10
#define SIA_SERVER_CHECK_REQ_TIME			SIA_SERVER_HEART_BIT * 2
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define SIA_CHECKVALID_IDX_T		if( FALSE == g_pConfig->m_bSIAActive )return FALSE; \
if( !m_pSIASocket->IsActive())return FALSE; \
	SIA_NET_BUFFER	* pNetBuffer = &m_SendBufferT; \
	if( pNetBuffer->_i32ReadPos > (MODULE_CONTEXT_MAX_SEND_IDX_COUNT - 1000) )\
{\
	if( pNetBuffer->_i32WritePos == pNetBuffer->_i32ReadPos )\
{\
	pNetBuffer->_i32WritePos	= 0;\
	pNetBuffer->_i32ReadPos	= 0;\
}\
}\
	if( pNetBuffer->_i32ReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT )\
{\
	return FALSE; \
}\
	if( (pNetBuffer->_i32WritePos - pNetBuffer->_i32ReadPos) >= SIA_SEND_BUFFER_COUNT )\
{\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_i32WritePos % SIA_SEND_BUFFER_COUNT;


static UINT32 _DefThreadProc(void * pPointer)
{
	((CModuleSIA*)pPointer)->OnRunning(); 
	return 0; 
}

CModuleSIA::CModuleSIA(void)
{
	m_bIsRunning			= FALSE;
	m_hThread				= NULL;
	m_pSIASocket			= NULL;
	m_ui32LastSendTime		= 0;
	m_ui32ConnectTime		= 0;
	
	// 2010.03.29 노준영
	// ModuleSIA 소멸자에서 초기화 되어있지 않아 크래쉬가 발생하여 초기화 추가. 
	memset( &m_SendBufferT, 0, sizeof(SIA_NET_BUFFER));
}

CModuleSIA::~CModuleSIA(void)
{
	OnDestroy(); 
}

BOOL CModuleSIA::OnCreate()
{
	if(!g_pConfig->m_bSIAActive)	return TRUE; 
	
	m_SendBufferT._i32ReadPos	= 0;
	m_SendBufferT._i32WritePos	= 0;
    m_SendBufferT._ppNetMessage	= (SIA_SEND_MESSAGE **)i3MemAlloc( sizeof(SIA_SEND_MESSAGE *) * SIA_SEND_BUFFER_COUNT);

	for(INT32 i = 0; i < SIA_SEND_BUFFER_COUNT; i++)
	{
		m_SendBufferT._ppNetMessage[i] = (SIA_SEND_MESSAGE *)i3MemAlloc( sizeof(SIA_SEND_MESSAGE ) );
	}

	//Create Socket 
	m_pSIASocket = CModuleSIASocket::NewObject(); 
	if( FALSE == m_pSIASocket->OnCreate(g_pConfig->m_ui32SIAServerIP, g_pConfig->m_ui16SIAServerPort, m_i32LogIdx )) return FALSE;
	m_ui32ConnectTime = i3ThreadTimer::GetServerTime();

	//----------------------------------------------------
	m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
	m_pSIASocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;

	//Create Thread
	DWORD dwThreadID;
	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , this, 0, (DWORD *)&dwThreadID );
	if( NULL == m_hThread )	return FALSE;
	
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )	return FALSE;

	return TRUE; 
}

void CModuleSIA::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	WaitForSingleObject( m_hThread, INFINITE );	

	I3_SAFE_RELEASE( m_pSIASocket );

	if( m_SendBufferT._ppNetMessage )
	{
		for(INT32 i = 0; i < SIA_SEND_BUFFER_COUNT; i++)
		{
			I3MEM_SAFE_FREE( m_SendBufferT._ppNetMessage[i] );
		}
		I3MEM_SAFE_FREE( m_SendBufferT._ppNetMessage );
	}
	return; 
}

//MODULE_SIA_THREAD_IDX
UINT32 CModuleSIA::OnRunning()
{
	m_i32LogIdx = g_pLog->AddLogThread(L"MSIA", 0, GetCurrentThreadId());

	BOOL	bIsSend;

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		bIsSend = FALSE;

		//-------------------------------------------------------------------------------
		//1. Connect Check 
		if(!m_pSIASocket->IsConnected())
		{
			m_pSIASocket->OnDestroy(); 
			if( m_pSIASocket->OnCreate(g_pConfig->m_ui32SIAServerIP, g_pConfig->m_ui16SIAServerPort, m_i32LogIdx ) )
			{
				m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect SIAServer 1" );

				S2RingBuffer* pRing = g_pTaskProcessor->GetRingSIAPop();
				TASK_INVEN_BUFFER* pBuffer = (TASK_INVEN_BUFFER*)pRing->PushPointer();
				if( NULL != pBuffer )
				{
					// SIA서버와 재연결 성공
					pBuffer->m_ui8Type			= TASK_INVEN_TYPE_PCCAFE;
					pBuffer->m_ui8Action		= TYPE_PCCAFE_CONNECT;

					pRing->PushPointerIdx();
				}
			}
			else
			{
				m_pSIASocket->OnDestroy(); 

				g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect SIAServer 1" );
				::Sleep( SIA_RECONNECT_TIME );
			}

			m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
			m_pSIASocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;
			continue; 
		}

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pSIASocket->IsActive() )
		{
			INT32 i;
			INT32 i32SendCount;
			INT32 i32ReadIdx;
			i32SendCount = m_SendBufferT._i32WritePos - m_SendBufferT._i32ReadPos;

			if(i32SendCount > 0)
			{
				if(i32SendCount > MODULE_CONTEXT_SEND_COUNT)i32SendCount = MODULE_CONTEXT_SEND_COUNT; 

				for( i = 0; i < i32SendCount; i++)
				{
					i32ReadIdx = m_SendBufferT._i32ReadPos % SIA_SEND_BUFFER_COUNT; 
 					m_pSIASocket->SendPacketMessage( (char*)m_SendBufferT._ppNetMessage[i32ReadIdx], m_SendBufferT._ppNetMessage[i32ReadIdx]->_ui16TotalSize ); 
					m_SendBufferT._i32ReadPos++;
				}
				bIsSend = TRUE;
			}

			//2-4 Check Send 패킷
			if( (m_ui32LastSendTime + SIA_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
			{
				SIA_SEND_MESSAGE PacketHeart; 
				PacketHeart.Clear(); 
				PacketHeart.SetProtocol( PROTOCOL_MASK_HEARTBIT );
				m_pSIASocket->SendPacketMessage( (char*)&PacketHeart, PacketHeart._ui16TotalSize ); 
				m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
				bIsSend = TRUE;
			}
		}
		else 
		{
			//Connect Req Fail
			if( m_ui32ConnectTime + SIA_CONNECT_TIMEOUT < i3ThreadTimer::GetServerTime())
			{
				m_pSIASocket->OnDestroy(); 
				if( m_pSIASocket->OnCreate(g_pConfig->m_ui32SIAServerIP, g_pConfig->m_ui16SIAServerPort, m_i32LogIdx ) )
				{
					m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect SIAServer 3" );
				}
				else
				{
					m_pSIASocket->OnDestroy(); 
					g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect SIAServer3" );
					::Sleep(SIA_RECONNECT_TIME);
				}
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pSIASocket->SelectEvent();

		//-------------------------------------------------------------------------------
		
		//4. Check Connect State
		if( (m_pSIASocket->m_ui32LastRecvTime + SIA_SERVER_CHECK_REQ_TIME) < i3ThreadTimer::GetServerTime() )
		{
			m_pSIASocket->OnDestroy(); 
			if( m_pSIASocket->OnCreate(g_pConfig->m_ui32SIAServerIP, g_pConfig->m_ui16SIAServerPort, m_i32LogIdx ) )
			{
				m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect SIAServer 2" );
			}
			else
			{
				m_pSIASocket->OnDestroy(); 
				g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect SIAServer2" );
				::Sleep(SIA_RECONNECT_TIME);
			}

			m_ui32LastSendTime					= i3ThreadTimer::GetServerTime();
			m_pSIASocket->m_ui32LastRecvTime	= m_ui32LastSendTime + 1;
			continue; 
		}

		if( FALSE == bIsSend ) ::WaitForSingleObject( m_hThread, 1 );					//거의 들어오지 않는다. 1초 동안 돌지 않아도 된다.

	}
	return 0; 
}

BOOL	CModuleSIA::IsActive()
{
	if( NULL	== m_pSIASocket )					return FALSE;
	if( FALSE	== m_pSIASocket->IsConnected() )	return FALSE;
	if( FALSE	== m_pSIASocket->IsActive() )		return FALSE;

	return TRUE;;	
}

BOOL	CModuleSIA::BuyGoods_T( PACKET_BUY_GOODS_REQ *pBuyReq, void* pBuffer )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_BUY_GOODS );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pBuyReq,						sizeof(PACKET_BUY_GOODS_BASE_REQ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pBuyReq->m_stLinkBuy,		sizeof(LINK_BUY)*pBuyReq->m_ui8GoodsCount );

	switch(pBuyReq->m_ui8Action)
	{
	case TYPE_GACHA_PURCHASE:
		{
			DB_GACHA* pGacha = (DB_GACHA*)pBuffer;

			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pGacha, sizeof(GACHA_PURCHACE_BASE) );
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pGacha->m_TTicketDBIdx, sizeof(T_ItemDBIdx));		
		}
		break;
	case TYPE_SHOP_CHARA_BUY:
		{
			RING_CREATE_CHAR*	pCharaBuy	=	(RING_CREATE_CHAR*)pBuffer;

			BUFFER_BUY_CHARA	stBuyChara;
			stBuyChara.m_TGoodsID		= pCharaBuy->m_TGoodsID;
			stBuyChara.m_TItemDBIdx		= pCharaBuy->m_Inven._TItemWareDBIdx;
			stBuyChara.m_ui8Class		= pCharaBuy->m_ui8Class;
			stBuyChara.m_ui8CharSlot	= pCharaBuy->m_ui8CharSlot;
			i3String::Copy( stBuyChara.m_strCharName, pCharaBuy->m_CharBaseInfo.m_strCharName, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);

			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &stBuyChara,							sizeof(BUFFER_BUY_CHARA) );		
		}
		break;
	case TYPE_SHOP_REPAIR:
		{
			BUFFER_BUY_REPAIR*	pRepair		=	(BUFFER_BUY_REPAIR*)pBuffer;
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRepair->m_stRepairItem,	sizeof(REPAIR_ITEM) * pRepair->m_ui8ItemCount );
		}
		break;
	case TYPE_SHOP_GIFT_BUY:
		{
			SEND_GIFT*	pGift		=	(SEND_GIFT*)pBuffer;

			UINT8 ui8MessageSize = (UINT8)i3String::Length( pGift->m_strMessage );
			
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pGift->m_TReceiverUID,	sizeof(T_UID) );	
			for( UINT8 i = 0 ; i < pBuyReq->m_ui8GoodsCount; ++i )
			{
				pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pGift->m_aGift[i].m_TGiftDBIdx, sizeof(T_GiftDBIdx) );			 
			}
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8MessageSize,			sizeof(UINT8) );	
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pGift->m_strMessage,		sizeof(TTCHAR) * ui8MessageSize );

		}
		break;
	case TYPE_SHOP_GOODS_BUY:
	case TYPE_SHOP_ITEM_EXTEND:
		{
			DB_SHOP_GOODS*		pDBShopBuy	=	(DB_SHOP_GOODS*)pBuffer;
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pDBShopBuy->m_TItemDBIdx, sizeof(T_ItemDBIdx) );
			for( UINT8 i = 0 ; i < pBuyReq->m_ui8GoodsCount ; ++i )
			{
				DB_ITEM_IN_GOODSID* pItem = &pDBShopBuy->m_stDBGoods[i].m_stDBItem;

				pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pItem->m_ui8ItemCount, sizeof(UINT8) );

				for( UINT8 k = 0 ; k < pItem->m_ui8ItemCount ; ++k )
				{
					pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pItem->m_aItem[k]._TItemWareDBIdx, sizeof(T_ItemDBIdx) );
				}
			}			
		}
 		break;
	case TYPE_SHOP_USE_GIFTCOUPON:
		{
			DB_COUPON*			pCoupon		=	(DB_COUPON*)pBuffer;
			BUFFER_BUY_COUPON	BufferCoupon;
			i3mem::Copy( &BufferCoupon, pCoupon->m_strCoupon, GIFTCOUPON_LENGTH + 1);
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &BufferCoupon, sizeof(BUFFER_BUY_COUPON) );
		}
		break;
	}

	pNetBuffer->_i32WritePos++;
	return TRUE;
}

BOOL	CModuleSIA::GetCash_T(	PACKET_GET_CASH_REQ *pGetCashReq )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_GET_CASH );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pGetCashReq,		sizeof(PACKET_GET_CASH_REQ));
	pNetBuffer->_i32WritePos++;
	return TRUE;
}

BOOL	CModuleSIA::SendLogin( ST_PACKET_LOGIN_REQ* pLoginData )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_LOGIN );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pLoginData,		sizeof(ST_PACKET_LOGIN_REQ) );
	pNetBuffer->_i32WritePos++;
	return TRUE;
}


BOOL	CModuleSIA::SendLogout( ST_PACKET_LOGOUT_REQ* pLogoutData )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_LOGOUT );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pLogoutData,	sizeof(ST_PACKET_LOGOUT_REQ) );
	pNetBuffer->_i32WritePos++;
	return TRUE;
}

BOOL	CModuleSIA::UseGiftCoupon_T( PACKET_USE_GIFTCOUPON_REQ* pUseCouponReq )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_USE_GIFTCOUPON );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pUseCouponReq,	sizeof(PACKET_USE_GIFTCOUPON_REQ));
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL	CModuleSIA::SendPCCafeLogin( PACKET_PCCAFE_LOGIN_REQ* pPCCafeLoginReq )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_PCCAFE_LOGIN_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pPCCafeLoginReq,	sizeof(PACKET_PCCAFE_LOGIN_REQ) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL	CModuleSIA::SendPCCafeCheckAmount( PACKET_PCCAFE_CHECK_AMOUNT_REQ* pPCCafeCheckAmountReq )
{
	SIA_CHECKVALID_IDX_T;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_MASK_PCCAFE_CHECK_AMOUNT_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pPCCafeCheckAmountReq, sizeof(PACKET_PCCAFE_CHECK_AMOUNT_REQ) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}