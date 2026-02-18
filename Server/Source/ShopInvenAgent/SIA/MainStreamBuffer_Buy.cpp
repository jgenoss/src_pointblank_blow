#include "pch.h"
#include "ModuleNet.h"
#include "MainStreamBuffer_Buy.h"

CMainStreamBuffer_Buy::CMainStreamBuffer_Buy()
{
	m_ui16TotalBufferCount	= 0;
	m_ui16UseBufferCount	= 0;	
	m_ui16BlankIdx			= 0;
	m_ui16PushIdx			= 0;

	m_pBufferIndex			= NULL;	
	m_pBuffer				= NULL;
}

CMainStreamBuffer_Buy::~CMainStreamBuffer_Buy()
{
	I3_SAFE_DELETE_ARRAY( m_pBufferIndex );
	I3_SAFE_DELETE_ARRAY( m_pBuffer );
}

BOOL	CMainStreamBuffer_Buy::CreateBuffer( UINT32 ui32Count )
{
	m_pBufferIndex	= new UINT16[ ui32Count ];

	if( NULL == m_pBufferIndex )
		return FALSE;

	m_pBuffer		= new MAIN_STREAM_BUFFER_BUY[ ui32Count ];

	if( NULL == m_pBuffer )
		return FALSE;

	for( UINT16 i = 0 ; i < ui32Count ; ++i )
	{
		m_pBufferIndex[i] = i;
		m_pBuffer[i].m_bIsEmpty = TRUE;
		m_pBuffer[i].m_ui8RecvCount = 0;

		i3mem::Fill( m_pBuffer[i].m_stBuyGoodsAck.m_i32Rv, EVENT_FAIL_ERROR, sizeof( INT32 ) * MAX_SHOP_GOODS_BUY_COUNT );
	}

	m_ui16TotalBufferCount	= (UINT16)ui32Count;
	m_ui16UseBufferCount	= 0;

	return TRUE;
}

INT32	CMainStreamBuffer_Buy::Push( UINT32 ui32ServerIdx, PACKET_BUY_GOODS_REQ* pBuyReq, NET_PACKET_BUFFER *pPacket )
{
	if ( m_ui16TotalBufferCount == m_ui16UseBufferCount )
		return EVENT_FAIL_MAINSTREAMBUFFER_FULL;

	UINT16	ui16idx = _GetNextBufferIdx();

	MAIN_STREAM_BUFFER_BUY* pBuy = &m_pBuffer[ui16idx];
	pBuy->m_ui32ServerIdx					= ui32ServerIdx;
	pBuy->m_ui32ServerST					= i3ThreadTimer::GetServerTime();
	pBuy->m_bIsEmpty						= FALSE;
	pBuy->m_stBuyGoodsAck.m_ui8Action		= pBuyReq->m_ui8Action;
	pBuy->m_stBuyGoodsAck.m_ui8GoodsCount	= pBuyReq->m_ui8GoodsCount;
	pBuy->m_stBuyGoodsAck.m_TUID			= pBuyReq->m_TUID;

	//void*	pBuffer = (void*)&pPacket->_Data[ sizeof(PACKET_BUY_GOODS_REQ) ];

	for( UINT8 i = 0 ; i < pBuyReq->m_ui8GoodsCount; ++i )
	{
		pBuy->m_stBuyGoodsAck.m_aGoodsID[i] = pBuyReq->m_stLinkBuy[i].m_TGoodsID;
	}
	
	switch( pBuyReq->m_ui8Action )
	{
	case TYPE_GACHA_PURCHASE:
		{
			pPacket->ReadData(  &pBuy->m_stGachaBuffer,					sizeof(GACHA_PURCHACE_BASE) ); 
			pPacket->ReadData(  &pBuy->m_stGachaBuffer.m_TTicketDBIdx,	sizeof(T_ItemDBIdx));
		}
		break;
	case TYPE_SHOP_CHARA_BUY:
		{
			pPacket->ReadData(  &pBuy->m_stCharaBuffer, sizeof(BUFFER_BUY_CHARA) ); 
		}
		break;
	case TYPE_SHOP_REPAIR:
		{
			pPacket->ReadData(  pBuy->m_stRepairBuffer.m_stRepairItem, sizeof(REPAIR_ITEM) * pBuyReq->m_ui8GoodsCount ); 
			pBuy->m_stRepairBuffer.m_ui8ItemCount = pBuyReq->m_ui8GoodsCount;
		}
		break;
	case TYPE_SHOP_GIFT_BUY:
		{
			pPacket->ReadData( &pBuy->m_stGiftBuffer.m_TReceiverUID, sizeof(T_UID) );

			for( UINT8 i = 0 ; i < pBuyReq->m_ui8GoodsCount; ++i )
			{
				pPacket->ReadData( &pBuy->m_stGiftBuffer.m_aGiftDBIdx[i], sizeof(T_GiftDBIdx) );
			}

			pPacket->ReadData( &pBuy->m_stGiftBuffer.m_ui8MessageSize, sizeof(UINT8) );
			pPacket->ReadData( pBuy->m_stGiftBuffer.m_strMessage, sizeof(TTCHAR) * pBuy->m_stGiftBuffer.m_ui8MessageSize );
			pBuy->m_stGiftBuffer.m_strMessage[pBuy->m_stGiftBuffer.m_ui8MessageSize] = 0;
		}
		break;
	case TYPE_SHOP_GOODS_BUY:
	case TYPE_SHOP_ITEM_EXTEND:
		{
			pPacket->ReadData( &pBuy->m_stShopBuffer.m_TItemDBIdx, sizeof(T_ItemDBIdx) );
			for( UINT8 i = 0 ; i < pBuyReq->m_ui8GoodsCount; ++i )
			{
				BUY_GOODS *pGoods = &pBuy->m_stShopBuffer.m_stDBGoods[i];
				pPacket->ReadData(  &pGoods->m_ui8ItemCount,		sizeof(UINT8) );  
				pPacket->ReadData(  pGoods->m_aItemDBIdx,			sizeof(T_ItemDBIdx) * pGoods->m_ui8ItemCount ); 
			}
		}
		break;
	}

	return ui16idx;
}
	
//외부 연동 결과 버퍼에 반영
BOOL	CMainStreamBuffer_Buy::SetRecvResult( BILLING_OUTPUT* pBilling_Out, BOOL bIsAsyncOutput )
{
	UINT32 ui32BufferIdx;

	//비동기 결과인 경우 마지막 자리를 제외한 실제 인덱스 값을 구한다.
	if( bIsAsyncOutput )
	{
		ui32BufferIdx = pBilling_Out->m_ui32MainBufferIdx / 10;		
	}
	else
	{
		ui32BufferIdx = pBilling_Out->m_ui32MainBufferIdx;
	}
	
	if(m_pBuffer[ui32BufferIdx].m_bIsEmpty)
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[MainBuffer::SetRecvResult]Invalid BufferIdx. Idx : %d, UID : %d, Rv : %x ", pBilling_Out->m_ui32MainBufferIdx, pBilling_Out->m_TUID, pBilling_Out->m_ai32Rv[0]);
		return FALSE;
	}

	m_pBuffer[ui32BufferIdx].m_stBuyGoodsAck.m_ui32CurrentUserCash		= pBilling_Out->m_ui32CurrentUserCash;
	m_pBuffer[ui32BufferIdx].m_stBuyGoodsAck.m_ui32CurrentUserCashType	= pBilling_Out->m_ui32CurrentUserCashType;

	if( bIsAsyncOutput )
	{
		m_pBuffer[ui32BufferIdx].m_ui8RecvCount++;
		m_pBuffer[ui32BufferIdx].m_stBuyGoodsAck.m_i32Rv[ pBilling_Out->m_ui32MainBufferIdx % 10 ] = pBilling_Out->m_ai32Rv[0];
	}
	else
	{
		m_pBuffer[ui32BufferIdx].m_ui8RecvCount		= pBilling_Out->m_ui8GoodsCount;
		i3mem::Copy( m_pBuffer[ui32BufferIdx].m_stBuyGoodsAck.m_i32Rv, pBilling_Out->m_ai32Rv, sizeof(INT32) * pBilling_Out->m_ui8GoodsCount) ;
	}	
	return TRUE;
}

//처리 가능한 Buffer를 Trans로 전송, Timeout 선별, etc......
BOOL	CMainStreamBuffer_Buy::UpdateBuffer()
{
	BOOL bWorking = FALSE;

	for( UINT16 i = 0 ; i < m_ui16TotalBufferCount ; ++i )
	{
		if( m_pBuffer[i].m_bIsEmpty )
			continue;
		
		//구입 요청 별 처리 완료 여부 확인
		if( m_pBuffer[i].m_stBuyGoodsAck.m_ui8GoodsCount == m_pBuffer[i].m_ui8RecvCount )
				bWorking = _SendResultData(i);
		

		//타임 오버된 버퍼 초기화
		if( m_pBuffer[i].m_ui32ServerST + 600 < i3ThreadTimer::GetServerTime() )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[MainBuffer::UpdateBuffer]Time out of Process. Idx : %d, UID : %d, ActionType : %d", i, m_pBuffer[i].m_stBuyGoodsAck.m_TUID, m_pBuffer[i].m_stBuyGoodsAck.m_ui8Action);
			_SendResultData(i);
		}

	}

	return bWorking;
}

BOOL	CMainStreamBuffer_Buy::_SendResultData( UINT16 ui16BufferIdx )
{
	MAIN_STREAM_BUFFER_BUY * pBuy = &m_pBuffer[ui16BufferIdx];
	if( !pBuy ) return FALSE;

	NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_BUY_GOODS );

	SendPacket.WriteData( &pBuy->m_stBuyGoodsAck,			sizeof(PACKET_BUY_GOODS_BASE_ACK) );
	SendPacket.WriteData( pBuy->m_stBuyGoodsAck.m_i32Rv,	sizeof(INT32) * pBuy->m_stBuyGoodsAck.m_ui8GoodsCount  );
	SendPacket.WriteData( pBuy->m_stBuyGoodsAck.m_aGoodsID,	sizeof(T_GoodsID) * pBuy->m_stBuyGoodsAck.m_ui8GoodsCount  );
	
	switch( pBuy->m_stBuyGoodsAck.m_ui8Action )
	{
	case TYPE_GACHA_PURCHASE:
		{
			SendPacket.WriteData( &pBuy->m_stGachaBuffer,					sizeof(GACHA_PURCHACE_BASE) ); 
			SendPacket.WriteData( &pBuy->m_stGachaBuffer.m_TTicketDBIdx,	sizeof(T_ItemDBIdx) );
		}
		break;
	case TYPE_SHOP_CHARA_BUY:
		{
			SendPacket.WriteData( &pBuy->m_stCharaBuffer, sizeof(BUFFER_BUY_CHARA) );
		}
		break;
	case TYPE_SHOP_REPAIR:
		{
			SendPacket.WriteData( m_pBuffer[ui16BufferIdx].m_stRepairBuffer.m_stRepairItem, sizeof(REPAIR_ITEM) * pBuy->m_stBuyGoodsAck.m_ui8GoodsCount );
		}
		break;
	case TYPE_SHOP_GIFT_BUY:
		{
			SendPacket.WriteData( &pBuy->m_stGiftBuffer.m_TReceiverUID, sizeof(T_UID) );
			for( UINT8 i = 0 ; i < pBuy->m_stBuyGoodsAck.m_ui8GoodsCount ; ++i )
			{
				SendPacket.WriteData( &pBuy->m_stGiftBuffer.m_aGiftDBIdx[i], sizeof(T_GiftDBIdx) );
			}
			SendPacket.WriteData( &pBuy->m_stGiftBuffer.m_ui8MessageSize, sizeof(UINT8) );
			SendPacket.WriteData( pBuy->m_stGiftBuffer.m_strMessage, sizeof(TTCHAR) * pBuy->m_stGiftBuffer.m_ui8MessageSize );
		}
		break;
	case TYPE_SHOP_ITEM_EXTEND:
	case TYPE_SHOP_GOODS_BUY:
		{ 
			SendPacket.WriteData( &pBuy->m_stShopBuffer.m_TItemDBIdx,	sizeof(T_ItemDBIdx)); 
			for( UINT8 i = 0 ; i < pBuy->m_stBuyGoodsAck.m_ui8GoodsCount ; ++i )
			{
				BUY_GOODS* pGoods = &pBuy->m_stShopBuffer.m_stDBGoods[i];
				SendPacket.WriteData( &pGoods->m_ui8ItemCount,	sizeof(UINT8)); 
				SendPacket.WriteData( pGoods->m_aItemDBIdx,		sizeof(T_ItemDBIdx) * pGoods->m_ui8ItemCount );
			}
		}
		break;
	}
	
	g_pModuleNet->SendPush( pBuy->m_ui32ServerIdx, &SendPacket );

	_SetBlankBuffer( ui16BufferIdx );

	return TRUE;
}


UINT16	CMainStreamBuffer_Buy::_GetNextBufferIdx()
{
	UINT16 ui16Idx = m_pBufferIndex[ m_ui16PushIdx ];

	++m_ui16PushIdx;
	m_ui16PushIdx %= m_ui16TotalBufferCount;

	++m_ui16UseBufferCount;

	return ui16Idx;
}


void	CMainStreamBuffer_Buy::_SetBlankBuffer( UINT16 ui16BufferIdx)
{
	m_pBufferIndex[m_ui16BlankIdx]			= ui16BufferIdx;
	m_pBuffer[ui16BufferIdx].m_bIsEmpty		= TRUE;	
	m_pBuffer[ui16BufferIdx].m_ui8RecvCount = 0;

	i3mem::Fill( m_pBuffer[ui16BufferIdx].m_stBuyGoodsAck.m_i32Rv, EVENT_FAIL_ERROR, sizeof( INT32 ) * MAX_SHOP_GOODS_BUY_COUNT );

	++m_ui16BlankIdx;
	m_ui16BlankIdx %= m_ui16TotalBufferCount;
	--m_ui16UseBufferCount;
}

