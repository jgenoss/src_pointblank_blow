#include "pch.h"
#include "BillingWorker_Italy.h"

//#define TEST_LOG_ACTIVE

CBillingWorker_Italy::CBillingWorker_Italy()
{
}

CBillingWorker_Italy::~CBillingWorker_Italy()
{
}

BOOL CBillingWorker_Italy::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{pOutput;

	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_GOODS_BUY:
		{
			GTX_PK_V11_PURCHASEITEM	stSAPacketBuy;
			i3mem::FillZero( &stSAPacketBuy, sizeof(GTX_PK_V11_PURCHASEITEM) );

			stSAPacketBuy.ReqKey		= pInput->m_ui32MainBufferIdx * 10 + ui8GoodsIdx;
			stSAPacketBuy.UserNo		= (DWORD)pInput->m_TUID;
			stSAPacketBuy.ClientIP		= pInput->m_ui32ClientIP;
			stSAPacketBuy.ReqLen		= sizeof(GTX_PK_V11_PURCHASEITEM); 
			stSAPacketBuy.ReqType		= GTX_PK_V09_TYPE_PURCHASEITEM; 

			i3String::Format( stSAPacketBuy.ItemID, 256, "%d", pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID );
			i3String::Format( stSAPacketBuy.ItemCnt, 128, "1" );
			i3String::Format( stSAPacketBuy.ItemUnitPrice, 256, "%d", pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price );
			i3String::Format( stSAPacketBuy.CouponDCPrice, 256, "0" );
			i3String::Format( stSAPacketBuy.ChargePrice, 256, "%d", pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price ); 
			i3String::Format( stSAPacketBuy.UserID, 50, "%s", pInput->m_strBillingID );
			i3String::Format( stSAPacketBuy.CharacterID, 50, "%s", pInput->m_strUserNick );

#ifdef TEST_LOG_ACTIVE 
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[CBillingWorker_Italy::_WorkProcess] MainIdx : %d / UID : %I64d / Ip : %d / GoodsId : %d / Price : %d ", 
			stSAPacketBuy.ReqKey, pInput->m_TUID, pInput->m_ui32ClientIP, pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID, pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price ); 
#endif

			INT32 i32SendSize = SendMessage( (char*)&stSAPacketBuy, sizeof(GTX_PK_V11_PURCHASEITEM) );
			if( i32SendSize <= 0 )  
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NETWORK, L"[CBillingWorker_Italy::_WorkProcess] MainIdx : %d / UID : %I64d / Ip : %d / GoodsId : %d / Price : %d ", 
				stSAPacketBuy.ReqKey, pInput->m_TUID, pInput->m_ui32ClientIP, pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID, pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price ); 
				return FALSE;
			}
			 
#ifdef TEST_LOG_ACTIVE
			if( i32SendSize > 0 ) 
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[CBillingWorker_Italy::_WorkProcess] Succ Send Buy Packet / Size : %d ", i32SendSize ); 
			}
#endif
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			//Set Value 
			GTX_PK_V11_GETBALANCE sGetBalanceBuffer;
			i3mem::FillZero( &sGetBalanceBuffer, sizeof(GTX_PK_V11_GETBALANCE) );
			sGetBalanceBuffer.ReqLen		= sizeof(GTX_PK_V11_GETBALANCE);
			sGetBalanceBuffer.ReqType		= GTX_PK_V11_TYPE_GETBALANCE;
			sGetBalanceBuffer.ReqKey		= (DWORD)pInput->m_TUID;
			sGetBalanceBuffer.UserNo		= (DWORD)pInput->m_TUID;
			i3String::Format( sGetBalanceBuffer.UserID, 50, "%s", pInput->m_strBillingID);

			if( 0 == SendMessage( (char*)&sGetBalanceBuffer, sizeof(GTX_PK_V11_GETBALANCE) ) ) return FALSE;
			
		}
		break;
	case TYPE_SHOP_USE_GIFTCOUPON:
		{
		
			/*pOutput->m_TUID = pInput->m_TUID;
			i3mem::Copy( pOutput->m_strCouponCode, pInput->m_strCouponCode, GIFTCOUPON_LENGTH );
			 
			GTX_PK_REGCOUPON	stSAPacketRegCoupon;

			i3mem::FillZero( &stSAPacketRegCoupon, sizeof(GTX_PK_REGCOUPON) );

			stSAPacketRegCoupon.ReqLen		= sizeof(GTX_PK_REGCOUPON);  
			stSAPacketRegCoupon.ReqType		= GTX_PK_V09_TYPE_REGCOUPON;

			stSAPacketRegCoupon.ReqKey		= pInput->m_ui32MainBufferIdx;

			i3mem::Copy( stSAPacketRegCoupon.UID, pInput->m_strBillingID, BILLING_ID_SIZE );
			i3mem::Copy( stSAPacketRegCoupon.CouponNo, pInput->m_strCouponCode, GIFTCOUPON_LENGTH );

			if( 0 == SendMessage( (char*)&stSAPacketRegCoupon, sizeof(GTX_PK_REGCOUPON) ) ) return FALSE;*/
		}
		break;
	}	

	return FALSE;
}

INT32 CBillingWorker_Italy::RecvMessage( char* pPacket, INT32 i32Size )
{	
	if( sizeof(BOQGTX_HEALTH_CHECK) > i32Size )		return 0; 

	//패킷헤더 구조체는 아니지만 ReqType 판별을 위해 사용합니다.
	BOQGTX_HEALTH_CHECK* pstPacketHeader = (BOQGTX_HEALTH_CHECK*)pPacket;
	BILLING_OUTPUT*	pOutput;

	if( i32Size < pstPacketHeader->ReqLen )		return 0; 

	pOutput = (BILLING_OUTPUT*)m_pPopBuffer->PushPointer();
	if( NULL == pOutput )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_TASK, L"[CBillingWorker_Italy::RecvMessage] Billing OutBuffer Full." ); 
		return pstPacketHeader->ReqLen;
	}

	// 구매결과
	switch(pstPacketHeader->ReqType)
	{
	case GTX_PK_V09_TYPE_PURCHASEITEM :
		{	// 구매 결과
#ifdef TEST_LOG_ACTIVE
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Italy::RecvMessage] Recv Buy Goods Packet / Size : %d", pstPacketHeader->ReqLen );
#endif
			GTX_PK_V11_PURCHASEITEM* pstSAPacketBuy = (GTX_PK_V11_PURCHASEITEM*)pPacket;
			
			//알수 없는 패킷 입니다. 
			if( sizeof(GTX_PK_V11_PURCHASEITEM) != pstSAPacketBuy->ReqLen )
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Italy::RecvMessage] Error Buy Goods Unknown Packet / Size : %d", pstSAPacketBuy->ReqLen );
				return pstSAPacketBuy->ReqLen; 
			}

			pOutput->m_eLinkType				= LINK_TYPE_BUY_GOODS;
			pOutput->m_ui32MainBufferIdx		= pstSAPacketBuy->ReqKey; 
			pOutput->m_ui8GoodsCount			= 1;
			pOutput->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;
			pOutput->m_ui32CurrentUserCash		= (pstSAPacketBuy->RealCash + pstSAPacketBuy->BonusCash);

			switch( pstSAPacketBuy->RetCode )
			{
			case 0 :
#ifdef TEST_LOG_ACTIVE
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[CBillingWorker_Italy::RecvMessage] Succ Buy Goods Result : %d / MainBufferIdx : %d ",  pstSAPacketBuy->RetCode, pstSAPacketBuy->ReqKey );
#endif

				pOutput->m_ai32Rv[0]			= EVENT_ERROR_SUCCESS;
				break;
			default : 
				{
					char strLinkError[MAX_PATH];
					_GetLinkErrorMessage( pstSAPacketBuy->RetCode, strLinkError, MAX_PATH );

					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Italy::RecvMessage] Error Buy Goods / %s / ErorCode : %d / MainBufferIdx : %d ",
						strLinkError, pstSAPacketBuy->RetCode, pstSAPacketBuy->ReqKey ); 

					pOutput->m_ai32Rv[0] = EVENT_ERROR_SHOP_BUY_FAIL_LINK;
				}
				break;
			}			 

			m_pPopBuffer->PushPointerIdx();
		}
		break;
	case GTX_PK_V09_TYPE_GETBALANCE :
		{	// 잔액조회 결과
			GTX_PK_V11_GETBALANCE* pstPacketGetCash = (GTX_PK_V11_GETBALANCE*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(GTX_PK_V11_GETBALANCE) != pstPacketGetCash->ReqLen )
			{ 
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Italy::RecvMessage] Get Cash Error Unknown Packet / Size : %d", pstPacketGetCash->ReqLen );
				return pstPacketGetCash->ReqLen; 
			}

			pOutput->m_eLinkType				= LINK_TYPE_GET_CASH;
			pOutput->m_TUID						= (INT64)pstPacketGetCash->ReqKey;
			pOutput->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;
			
			switch( pstPacketGetCash->RetCode )
			{
			case 0 :
				pOutput->m_ui32CurrentUserCash		= (pstPacketGetCash->RealCash + pstPacketGetCash->BonusCash);
				pOutput->m_ai32Rv[0]				= EVENT_ERROR_SUCCESS;
				break;
			default : 
				{
					char strLinkError[MAX_PATH];
					_GetLinkErrorMessage( pstPacketGetCash->RetCode, strLinkError, MAX_PATH );

					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Italy::RecvMessage] Error GetCash / %s / ErrorCode : %d / UID : %I64d ",
						strLinkError, pstPacketGetCash->RetCode, pOutput->m_TUID ); 

					pOutput->m_ai32Rv[0] = EVENT_ERROR_SHOP_BUY_FAIL_LINK;
					pOutput->m_ui32CurrentUserCash		= 0;
				}
				break;
			}			 
			
			m_pPopBuffer->PushPointerIdx();
		}
		break;
	default:
		break;
	}

	return pstPacketHeader->ReqLen;
}

void	CBillingWorker_Italy::_GetLinkErrorMessage( INT32 i32ErrorCode, char* strErrorMessage, INT32 i32Size )
{
	switch( i32ErrorCode )
	{
		case 450 :	i3String::Copy( strErrorMessage, "EXEC SP", i32Size );				break;
		case 452 :	i3String::Copy( strErrorMessage, "DB CONNECT", i32Size );			break;
		case 1001 :	i3String::Copy( strErrorMessage, "PARAMETER", i32Size );			break;
		case 1003 :	
		case 1013 :	
					i3String::Copy( strErrorMessage, "REQ TYPE", i32Size );				break;
		case 1004 :	i3String::Copy( strErrorMessage, "SOCKET TIMEOUT", i32Size );		break;
		case 1006 :	
		case 1010 :
					i3String::Copy( strErrorMessage, "STOP GTX SERVER", i32Size );		break;
		case 1007 :	i3String::Copy( strErrorMessage, "BLOCK SERVER IP", i32Size );		break; 
		case 1011 :	i3String::Copy( strErrorMessage, "REQ OVERFLOW", i32Size );			break;
		case 1012 :	i3String::Copy( strErrorMessage, "GTX SERVER EXCEPTION", i32Size );	break;					
		default :	i3String::Copy( strErrorMessage, "UNKNOWN ERROR", i32Size );		break;
	}
	return;
}