#include "pch.h"
#include "ModuleTask.h"
#include "ModuleNet.h"

void CModuleTask::_ParshingNet(UINT32 ui32ServerIdx, NET_PACKET_BUFFER * pPacket)
{	 
	//예외 처리 3: [비정상] 패킷 사이즈가 너무 크다.
	if( pPacket->_ui16DataSize > NET_PACKET_BUFFER_SIZE )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Trans Packet size Over TotalSize = %d Datasize = %d ProtoclID = %d", pPacket->_ui16TotalSize, pPacket->_ui16DataSize, pPacket->_ui16Protocol);
		return;
	}

	switch(pPacket->_ui16Protocol)
	{
	case PROTOCOL_MASK_HEARTBIT:
		{
			NET_PACKET_BUFFER pBuffer( PROTOCOL_MASK_HEARTBIT );
			g_pModuleNet->SendPush( ui32ServerIdx, &pBuffer);
		}
		break;
	case PROTOCOL_MASK_LOGIN:
		{
			ST_PACKET_LOGIN_REQ* pLoginPacket = (LOGIN_INPUT*)pPacket->_Data;

			LOGIN_INPUT LoginData( pLoginPacket );
			LoginData.m_i32TransServerIdx	= ui32ServerIdx;
			LoginData.m_eCommandType		= TYPE_LOGIN_ID;
			
			LINK_METHOD eLogin;

			//잘못된 Login연동인 경우 첫번째로 설정한 연동으로 처리시킵니다.
			if ( LoginData.m_ui8LinkMethod >= LINK_METHOD_MAX || NULL == m_ppModuleLogin[LoginData.m_ui8LinkMethod])
			{
				eLogin = g_pConfig->m_aeLoginMethod[0];
				LoginData.m_ui8LinkMethod = static_cast<UINT8>(eLogin);
			}
			else
				eLogin = (LINK_METHOD)LoginData.m_ui8LinkMethod;

			// vv3 - check
			LoginData.m_i32MainLoginBufferIdx = m_pMainBufferLogin->Push(ui32ServerIdx, &LoginData);

			if( -1 == m_ppModuleLogin[eLogin]->Push_T(&LoginData) )
			{
				m_pMainBufferLogin->SendResultData( LoginData.m_i32MainLoginBufferIdx );
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]LOGIN ID BUFFER FULL"); 
			}
		}
		break;
	case PROTOCOL_MASK_LOGOUT:
		{
			ST_PACKET_LOGOUT_REQ* pLogoutPacket = (ST_PACKET_LOGOUT_REQ*)pPacket->_Data;

			if (g_pConfig->m_aeLoginMethod[0] == LINK_METHOD_LATINAMERICA)
			{
				PCCAFE_INPUT Input;
				Input.m_eInputType = TYPE_PCCAFE_INPUT_LOGOUT;
											
				i3mem::Copy(&Input.m_strID, pLogoutPacket->m_strID, NET_ID_SIZE);

				if (-1 == m_pModulePCCafe->PushData(&Input))
				{
					g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]PCCafe Buffer Full - PROTOCOL_MASK_PCCAFE_LOGOUT :%s", Input.m_strID);
				}

			}
			else
			{
				PCCAFE_INPUT Input;
				Input.m_eInputType = TYPE_PCCAFE_INPUT_LOGOUT;
				Input.m_ui32PCCafeBillingUID = pLogoutPacket->m_ui32PCCafeBillingUID;

				if (RING_BUFFER_SUCCESS != m_pModulePCCafe->PushData(&Input))
				{
					g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]PCCafe Buffer Full - PROTOCOL_MASK_PCCAFE_LOGOUT - WZUID:%d", Input.m_ui32PCCafeBillingUID);
				}
			}
		}
		break;

	case PROTOCOL_MASK_BUY_GOODS		: 
		{
			pPacket->Clear();

			PACKET_BUY_GOODS_REQ ShopPacket;
			
			pPacket->ReadData( &ShopPacket,					sizeof(PACKET_BUY_GOODS_BASE_REQ) ); 
			pPacket->ReadData( ShopPacket.m_stLinkBuy,		sizeof(LINK_BUY)*ShopPacket.m_ui8GoodsCount );

			BILLING_INPUT Input;
			Input.m_ui32ServerIdx		= ui32ServerIdx;
			Input.m_TUID				= ShopPacket.m_TUID;
			Input.m_ui8Action			= TYPE_SHOP_GOODS_BUY;
			Input.m_ui64UserBillingID	= ShopPacket.m_ui64UserBillingIdx;
			Input.m_ui32Rank			= ShopPacket.m_ui32Rank;
			Input.m_ui32ClientIP		= ShopPacket.m_ui32ClientIP;
			Input.m_eLinkType			= LINK_TYPE_BUY_GOODS;
			i3mem::Copy( Input.m_strBillingID, ShopPacket.m_strBillingID, BILLING_ID_SIZE );
			Input.m_strBillingID[ BILLING_ID_SIZE-1 ] = '\0';
			i3mem::Copy( Input.m_strUserNick, ShopPacket.m_strUserNick , NET_NICK_NAME_SIZE );
			Input.m_strUserNick[ NET_NICK_NAME_SIZE-1 ] = '\0';
			
			Input.m_ui8GoodsCount		= ShopPacket.m_ui8GoodsCount;
			i3mem::Copy( Input.m_stLinkBuy, ShopPacket.m_stLinkBuy, sizeof(LINK_BUY) * ShopPacket.m_ui8GoodsCount ) ;

			LINK_METHOD	eBilling;

			//잘못된 Billing연동인 경우 첫번째로 설정한 연동으로 처리시킵니다.
			if ( ShopPacket.m_ui8LinkMethod >= LINK_METHOD_MAX || NULL == m_ppModuleBilling[ShopPacket.m_ui8LinkMethod])
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"Fail Billing ");
				eBilling = g_pConfig->m_aeBillingMethod[0];
				ShopPacket.m_ui8LinkMethod = static_cast<UINT8>(eBilling);
			}
			else
				eBilling = (LINK_METHOD)ShopPacket.m_ui8LinkMethod;

			//메인버퍼에 데이터 보관 
			Input.m_ui32MainBufferIdx	= m_pMainBufferBuy->Push( ui32ServerIdx, &ShopPacket, pPacket );			
			if( RING_BUFFER_SUCCESS != m_ppModuleBilling[eBilling]->PushData( &Input ) )
			{	// Error
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Error Link Buffer OverFlow - Buy Goods / UID : %I64d / GoodsCount : %d / GoodsID : %d ",
					ShopPacket.m_TUID, ShopPacket.m_ui8GoodsCount, ShopPacket.m_stLinkBuy[0].m_TGoodsID);

				BILLING_OUTPUT Output;
				i3mem::Copy( &Output, &Input , sizeof( BILLING_BASE ) ); 
				i3mem::Fill( Output.m_ai32Rv, EVENT_ERROR_EVENT_BUFFER_FULL, sizeof( INT32 ) * ShopPacket.m_ui8GoodsCount );				
				SendDataLink( &Output, pPacket );
			}
		}
		break;

	case PROTOCOL_MASK_GET_CASH		: 
		{
			PACKET_GET_CASH_REQ* pGetCashPacket = (PACKET_GET_CASH_REQ*)pPacket->_Data;

			BILLING_INPUT Input;
			Input.m_ui32ServerIdx		= ui32ServerIdx;
			Input.m_TUID				= pGetCashPacket->m_TUID;
			Input.m_ui8Action			= TYPE_SHOP_TYPE_GET_CASH;
			Input.m_ui64UserBillingID	= pGetCashPacket->m_ui64UserBillingIdx;
			Input.m_eLinkType			= LINK_TYPE_GET_CASH;

			i3mem::Copy( Input.m_strBillingID, pGetCashPacket->m_strBillingID, BILLING_ID_SIZE );
			Input.m_strBillingID[ BILLING_ID_SIZE-1 ] = '\0';

			LINK_METHOD	eBilling;

			//잘못된 Billing연동인 경우 첫번째로 설정한 연동으로 처리시킵니다.
			if (pGetCashPacket->m_ui8LinkMethod >= LINK_METHOD_MAX || NULL == m_ppModuleBilling[pGetCashPacket->m_ui8LinkMethod])
				eBilling = g_pConfig->m_aeBillingMethod[0];			
			else
				eBilling = (LINK_METHOD)pGetCashPacket->m_ui8LinkMethod;

			if( RING_BUFFER_SUCCESS != m_ppModuleBilling[eBilling]->PushData( &Input ) )
			{	// Error
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Error Link Buffer OverFlow - Get Cash / UID : %I64d  ",  pGetCashPacket->m_TUID);

				BILLING_OUTPUT Output;
				i3mem::Copy( &Output, &Input , sizeof( BILLING_BASE ) );  
				Output.m_ai32Rv[0] = EVENT_ERROR_EVENT_BUFFER_FULL;
				Output.m_ui32CurrentUserCash	= 0;
				SendDataLink( &Output );
			}
		}
		break;

	case PROTOCOL_MASK_USE_GIFTCOUPON:
		{
			PACKET_USE_GIFTCOUPON_REQ* pCouponPacket = (PACKET_USE_GIFTCOUPON_REQ*)pPacket->_Data;

			BILLING_INPUT Input;
			Input.m_ui32ServerIdx		= ui32ServerIdx;
			Input.m_TUID				= pCouponPacket->m_TUID;
			Input.m_ui8Action			= TYPE_SHOP_USE_GIFTCOUPON;
			Input.m_ui64UserBillingID	= pCouponPacket->m_ui64UserBillingIdx;
			Input.m_eLinkType			= LINK_TYPE_USE_GIFTCOUPON;
			Input.m_ui32ClientIP		= pCouponPacket->m_ui32IP;
			Input.m_ui32BirthDay		= pCouponPacket->m_ui32BirthDay;
			Input.m_ui32CreateDate		= pCouponPacket->m_ui32CreateDate;
			Input.m_ui8PcCafe			= pCouponPacket->m_ui8PcCafe;
			i3mem::Copy( Input.m_strUserNick, pCouponPacket->m_strUserNick, NET_NICK_NAME_SIZE );
			i3mem::Copy( Input.m_strBillingID, pCouponPacket->m_strBillingID, BILLING_ID_SIZE );
			i3mem::Copy( Input.m_strID, pCouponPacket->m_strID, NET_ID_SIZE );
			Input.m_strBillingID[ BILLING_ID_SIZE-1 ] = '\0';
			
			i3mem::Copy( Input.m_strCouponCode, pCouponPacket->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );

			LINK_METHOD	eBilling;

			//잘못된 Billing연동인 경우 첫번째로 설정한 연동으로 처리시킵니다.
			if ( pCouponPacket->m_ui8LinkMethod >= LINK_METHOD_MAX || NULL == m_ppModuleBilling[pCouponPacket->m_ui8LinkMethod] )
				eBilling = g_pConfig->m_aeBillingMethod[0];
			else
				eBilling = (LINK_METHOD)pCouponPacket->m_ui8LinkMethod;

			if( RING_BUFFER_SUCCESS != m_ppModuleBilling[eBilling]->PushData( &Input ) )
			{
				wchar_t wstrCouponCode[GIFTCOUPON_LENGTH + 1];
				MultiByteToWideChar(CP_ACP, NULL, pCouponPacket->m_strCouponCode, -1, wstrCouponCode, GIFTCOUPON_LENGTH + 1);

				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"Error Link Buffer OverFlow - Use GiftCoupon /  UID : %I64d / CouponCode : %s ",
					 pCouponPacket->m_TUID, wstrCouponCode);

				BILLING_OUTPUT Output;
				i3mem::Copy( &Output, &Input , sizeof( BILLING_BASE ) );  
				Output.m_ai32Rv[0] = EVENT_ERROR_EVENT_BUFFER_FULL;
				i3mem::Copy( Output.m_strCouponCode, pCouponPacket->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );
				SendDataLink( &Output );
			}
		}
		break;

	case PROTOCOL_MASK_PCCAFE_LOGIN_REQ:
		{
			PACKET_PCCAFE_LOGIN_REQ* pLoginPacket = (PACKET_PCCAFE_LOGIN_REQ*)pPacket->_Data;

			PCCAFE_INPUT Input;
			Input.m_eInputType		= TYPE_PCCAFE_INPUT_LOGIN;
			Input.m_TUID			= pLoginPacket->m_TUID;
			Input.m_ui32IP			= pLoginPacket->m_ui32IP;
			Input.m_ui32PCCafeGUID	= pLoginPacket->m_ui32PCCafeGUID;

			if( RING_BUFFER_SUCCESS != m_pModulePCCafe->PushData( &Input ) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]PCCafe Buffer Full - PROTOCOL_MASK_PCCAFE_LOGIN_REQ - UID:%I64d, IP:%d, PCCafeGUID:%u",
					Input.m_TUID,
					Input.m_ui32IP ,
					Input.m_ui32PCCafeGUID );
			}
		}
		break;

	case PROTOCOL_MASK_PCCAFE_CHECK_AMOUNT_REQ:
		{
			PACKET_PCCAFE_CHECK_AMOUNT_REQ* pCheckAmountPacket = (PACKET_PCCAFE_CHECK_AMOUNT_REQ*)pPacket->_Data;

			PCCAFE_INPUT Input;
			Input.m_eInputType		= TYPE_PCCAFE_INPUT_CHECK_AMOUNT;
			Input.m_TUID			= pCheckAmountPacket->m_TUID;
			Input.m_ui32IP			= pCheckAmountPacket->m_ui32IP;
			Input.m_ui32PCCafeGUID	= pCheckAmountPacket->m_ui32PCCafeGUID;
			
			if( RING_BUFFER_SUCCESS != m_pModulePCCafe->PushData( &Input ) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]PCCafe Buffer Full - PROTOCOL_MASK_PCCAFE_CHECK_AMOUNT_REQ - UID:%I64d, IP:%d, PCCafeGUID:%u",
					Input.m_TUID,
					Input.m_ui32IP,
					Input.m_ui32PCCafeGUID );
			}
		}
		break;

	}
	return; 
}