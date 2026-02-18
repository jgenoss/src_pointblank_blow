#include "pch.h"
#include "ModuleTask.h"
#include "ModuleNet.h"

void CModuleTask::_ParshingBilling( BILLING_OUTPUT * pOutBuffer, UINT8 ui8BillingMethod )
{
	switch ( pOutBuffer->m_eLinkType ) 
	{
	case LINK_TYPE_BUY_GOODS:
		{
			BOOL	bIsAsyncBilling = FALSE;

			switch( ui8BillingMethod )
			{		
			// 브라질, 태국, 러시아는 구매시 잔액을 덮어쓴다.
			case LINK_METHOD_BRAZIL		:
			//case LINK_METHOD_THAI		:
			case LINK_METHOD_RUSSIA:
				break;
			case LINK_METHOD_NORTHAMERICA	:
			case LINK_METHOD_NORTHA_STEAM	:
			case LINK_METHOD_ITALY	:
			case LINK_METHOD_KOREA			:	//TCP 빌링이기 때문에 Async를 체크합니다.
			case LINK_METHOD_LATINAMERICA:
				bIsAsyncBilling = TRUE;
			default:
				pOutBuffer->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_NONE;
				break;
			}

			m_pMainBufferBuy->SetRecvResult( pOutBuffer, bIsAsyncBilling );		
		}
		break;

	case LINK_TYPE_GET_CASH:
	case LINK_TYPE_USE_GIFTCOUPON:
		{
			SendDataLink( pOutBuffer );
		}
		break;
	}

	return; 
}

void CModuleTask::SendDataLink( BILLING_OUTPUT* pLinkOut, void* pData )
{
	switch ( pLinkOut->m_eLinkType )
	{
	case LINK_TYPE_BUY_GOODS : 
		{
			if( NULL == pData ) return;
 
			DB_GOODS aDBGoods[MAX_SHOP_GOODS_BUY_COUNT];

			NET_PACKET_BUFFER* pPacket = (NET_PACKET_BUFFER*)pData;
			pPacket->ReadData(  aDBGoods,	sizeof(DB_GOODS) * pLinkOut->m_ui8GoodsCount ); 			

			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_BUY_GOODS );

			PACKET_BUY_GOODS_ACK BuyAck;
			BuyAck.m_TUID						= pLinkOut->m_TUID; 			
			BuyAck.m_ui32CurrentUserCash		= pLinkOut->m_ui32CurrentUserCash;
			BuyAck.m_ui32CurrentUserCashType	= pLinkOut->m_ui32CurrentUserCashType;
			BuyAck.m_ui8GoodsCount				= pLinkOut->m_ui8GoodsCount;
			i3mem::Copy( BuyAck.m_i32Rv, pLinkOut->m_ai32Rv, sizeof( INT32 ) * pLinkOut->m_ui8GoodsCount );

			SendPacket.WriteData( &BuyAck,					sizeof(PACKET_BUY_GOODS_BASE_ACK));
			SendPacket.WriteData( BuyAck.m_i32Rv,			sizeof(INT32) * BuyAck.m_ui8GoodsCount);
			SendPacket.WriteData( aDBGoods,					sizeof(DB_GOODS) * BuyAck.m_ui8GoodsCount);
			g_pModuleNet->SendPush( pLinkOut->m_ui32ServerIdx, &SendPacket );
		}
		break;
	case LINK_TYPE_GET_CASH:
		{
			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_GET_CASH );

			PACKET_GET_CASH_ACK GetCashAck;
			GetCashAck.m_i32Result	= pLinkOut->m_ai32Rv[0];
			GetCashAck.m_TUID		= pLinkOut->m_TUID;
			GetCashAck.m_ui32Cash	= pLinkOut->m_ui32CurrentUserCash;

			SendPacket.WriteData( &GetCashAck,	sizeof(PACKET_GET_CASH_ACK));
			g_pModuleNet->SendPush( pLinkOut->m_ui32ServerIdx, &SendPacket );
		}
		break;
	case LINK_TYPE_USE_GIFTCOUPON:
		{
			NET_PACKET_BUFFER SendPacket( PROTOCOL_MASK_USE_GIFTCOUPON );

			PACKET_USE_GIFTCOUPON_ACK CouponAck;

			CouponAck.m_TUID = pLinkOut->m_TUID;
			CouponAck.m_i32Result = pLinkOut->m_ai32Rv[0];
			i3mem::Copy( CouponAck.m_strCouponCode, pLinkOut->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );

			if ( EV_SUCCESSED( CouponAck.m_i32Result ) )
			{
				CouponAck.m_ui32IncCash		= pLinkOut->m_ui32CurrentUserCash;
				CouponAck.m_i8GoodsCount	= pLinkOut->m_ui8GoodsCount;
				for( INT8 i = 0; i < CouponAck.m_i8GoodsCount; i++ )
					CouponAck.m_ai32GoodsID[i] = pLinkOut->m_ai32Rv[i + 1];
			}

			SendPacket.WriteData( &CouponAck,	sizeof(PACKET_USE_GIFTCOUPON_ACK) );

			g_pModuleNet->SendPush( pLinkOut->m_ui32ServerIdx, &SendPacket );
		}
		break;
	default : 
		return;
		break;
	}

	
}
