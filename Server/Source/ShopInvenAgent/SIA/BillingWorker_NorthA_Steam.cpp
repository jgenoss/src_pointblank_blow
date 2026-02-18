#include "pch.h"
#include "BillingWorker_NorthA_Steam.h"

CBillingWorker_NorthA_Steam::CBillingWorker_NorthA_Steam()
{
}

CBillingWorker_NorthA_Steam::~CBillingWorker_NorthA_Steam()
{
}

BOOL CBillingWorker_NorthA_Steam::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{

	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_GOODS_BUY:
		{
			GTX_PK_V12_PURCHASEITEM	stSAPacketBuy;
			i3mem::FillZero( &stSAPacketBuy, sizeof(GTX_PK_V12_PURCHASEITEM) );

			stSAPacketBuy.ReqLen		= sizeof(GTX_PK_V12_PURCHASEITEM); 
			stSAPacketBuy.ReqType		= GTX_PK_V12_TYPE_PURCHASEITEM; 
			stSAPacketBuy.ItemCnt		= 1;

			stSAPacketBuy.ReqKey		= pInput->m_ui32MainBufferIdx * 10 + ui8GoodsIdx;
			stSAPacketBuy.ItemID		= pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID;
			stSAPacketBuy.ChargeAmt		= pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price;
			stSAPacketBuy.ClientIP		= pInput->m_ui32ClientIP;
			stSAPacketBuy.SiteCode		= 2;// Steam

			i3String::Format( stSAPacketBuy.UserNo, BILLING_ID_SIZE, "%I64d", pInput->m_TUID );
			i3mem::Copy( stSAPacketBuy.ItemName, pInput->m_stLinkBuy[ui8GoodsIdx].m_strGoodsName, MAX_ITEM_NAME );

			stSAPacketBuy.UserNo[KOREA_UID_SIZE-1]	= '\0'; 
			stSAPacketBuy.ItemName[ MAX_ITEM_NAME-1 ] = '\0';

			if( 0 == SendMessage( (char*)&stSAPacketBuy, sizeof(GTX_PK_V12_PURCHASEITEM) ) ) return FALSE;
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			GTX_PK_V12_GETBALANCE	stSAPacketGetCash;
			i3mem::FillZero( &stSAPacketGetCash, sizeof(GTX_PK_V12_GETBALANCE) );

			stSAPacketGetCash.ReqLen		= sizeof(GTX_PK_V12_GETBALANCE);  
			stSAPacketGetCash.ReqType		= GTX_PK_V12_TYPE_GETBALANCE;

			//Set Value 
			stSAPacketGetCash.GameUID		= (unsigned __int64)pInput->m_TUID;
			stSAPacketGetCash.SiteCode		= 2;// Steam
			i3String::Format( stSAPacketGetCash.UserNo, BILLING_ID_SIZE, "%I64d", pInput->m_TUID );

			if( 0 == SendMessage( (char*)&stSAPacketGetCash, sizeof(GTX_PK_V12_GETBALANCE) ) ) return FALSE;
			
		}
		break;
	case TYPE_SHOP_USE_GIFTCOUPON:
		{
			pOutput->m_TUID = pInput->m_TUID;
			i3mem::Copy( pOutput->m_strCouponCode, pInput->m_strCouponCode, GIFTCOUPON_LENGTH );

			GTX_PK_V12_REGCOUPON	stSAPacketRegCoupon;

			i3mem::FillZero( &stSAPacketRegCoupon, sizeof(GTX_PK_V12_REGCOUPON) );

			stSAPacketRegCoupon.ReqLen		= sizeof(GTX_PK_V12_REGCOUPON);  
			stSAPacketRegCoupon.ReqType		= GTX_PK_V12_TYPE_REGCOUPON;

			stSAPacketRegCoupon.ReqKey		= pInput->m_ui32MainBufferIdx;
			stSAPacketRegCoupon.SiteCode	= 2;// Steam

			i3String::Format( stSAPacketRegCoupon.UID, BILLING_ID_SIZE, "%I64d", pInput->m_TUID );
			i3mem::Copy( stSAPacketRegCoupon.CouponNo, pInput->m_strCouponCode, GIFTCOUPON_LENGTH );

			if( 0 == SendMessage( (char*)&stSAPacketRegCoupon, sizeof(GTX_PK_V12_REGCOUPON) ) ) return FALSE;
		}
		break;
	}	

	return FALSE;
}

INT32 CBillingWorker_NorthA_Steam::RecvMessage( char* pPacket, INT32 i32Size )
{	
	if( sizeof(GTX_PK_V12_HEALTH_CHECK) > i32Size )		return 0; 

	//패킷헤더 구조체는 아니지만 ReqType 판별을 위해 사용합니다.
	GTX_PK_V12_HEALTH_CHECK* pstPacketHeader = (GTX_PK_V12_HEALTH_CHECK*)pPacket;
	BILLING_OUTPUT*	pOutput;
	
	if( i32Size < pstPacketHeader->ReqLen )		return 0; 

	pOutput = (BILLING_OUTPUT*)m_pPopBuffer->PushPointer();

	if( NULL == pOutput )
	{
		g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_NorthA_Steam::RecvMessage] Billing OutBuffer Full.");
		return pstPacketHeader->ReqLen;
	}

	//ReqType에 따라 해당하는 구조체 포인터로 캐스팅 해줍니다.
	switch(pstPacketHeader->ReqType)
	{
	//잔액조회
	case GTX_PK_V12_TYPE_GETBALANCE:
		{
			GTX_PK_V12_GETBALANCE* pstPacketGetCash = (GTX_PK_V12_GETBALANCE*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(GTX_PK_V12_GETBALANCE) != pstPacketGetCash->ReqLen )
			{ 
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_NorthA_Steam::RecvMessage] Get Cash Error Unknown Packet / Size : %d", pstPacketGetCash->ReqLen);
				return pstPacketGetCash->ReqLen;
			}

			pOutput->m_eLinkType			= LINK_TYPE_GET_CASH;
			pOutput->m_TUID					= (INT64)pstPacketGetCash->GameUID;
			
			switch( pstPacketGetCash->RetCode )
			{
			case GTX_ERR_CODE_SUCC :
				{
					pOutput->m_ai32Rv[0]			= EVENT_ERROR_SUCCESS;
					pOutput->m_ui32CurrentUserCash	= pstPacketGetCash->RealCash + pstPacketGetCash->BonusCash; 
				}
				break;
			default :
				{
					//	100	: 잔액부족
					//	200	: Non-Existing User
					//	210	: 존재하지 않는 과금번호
					//	211	: 존재하지 않는 쿠폰번호
					//	212	: 이미 사용한 쿠폰번호
					//	300	: 빌링 시스템 에러.
					//	900 : 유효하지 않은 값.				 
					pOutput->m_ai32Rv[0]			= EVENT_ERROR_BILLING_GET_CASH; 
					pOutput->m_ui32CurrentUserCash	= 0;
					g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_NorthA_Steam::RecvMessage] Cash Error Return / Error code : %d", pstPacketGetCash->RetCode);
				}
				break;
			}
			
			m_pPopBuffer->PushPointerIdx();
		}
		break;

	//아이템 구입
	case GTX_PK_V12_TYPE_PURCHASEITEM:
		{
			GTX_PK_V12_PURCHASEITEM* pstPacketBuy = (GTX_PK_V12_PURCHASEITEM*)pPacket;

			//알수 없는 패킷 입니다. 
			if( sizeof(GTX_PK_V12_PURCHASEITEM) != pstPacketBuy->ReqLen )
			{ 
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_NorthA_Steam::RecvMessage] Buy Goods Error Unknown Packet / Size : %d", pstPacketBuy->ReqLen);
				return pstPacketBuy->ReqLen;
			}

			pOutput->m_eLinkType				= LINK_TYPE_BUY_GOODS;
			pOutput->m_ui32MainBufferIdx		= pstPacketBuy->ReqKey;
			pOutput->m_ui8GoodsCount			= 1;
			pOutput->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;					
			pOutput->m_ui32CurrentUserCash		= pstPacketBuy->RealCash + pstPacketBuy->BonusCash;
			pOutput->m_ai32Rv[0]				= EVENT_ERROR_BILLING_UNKNOWN;  

			switch( pstPacketBuy->RetCode ) 
			{
			case GTX_ERR_CODE_SUCC:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_SUCCESS;  break; 
			case GTX_ERR_CODE_100:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_BILLING_NOT_ENOUGH_PRCIE; break;
			case GTX_ERR_CODE_200:		pOutput->m_ai32Rv[0]	= EVENT_ERROR_BILLING_NOT_FIND_USER; break;
			default:
				{ 		
					g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_NorthA_Steam::RecvMessage] Unknown Error / ErrorCode : %d ", pstPacketBuy->RetCode);
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