#include "pch.h"
#include "Payletter.h"
#include "BillingBase.h"
#include "BillingWorker_Russia.h"
#include "../Innova/errors.h"

CBillingWorker_Russia::CBillingWorker_Russia()
{
}

CBillingWorker_Russia::~CBillingWorker_Russia()
{
}

BOOL CBillingWorker_Russia::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{
	INT32 i32PriceType = 0;
	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_REPAIR:
		{
			i32PriceType = 1;
		}
		// break; 없는 게 맞습니다.
	case TYPE_SHOP_GOODS_BUY:
		{
			UINT64 ui64BillingTransactionID = 0;
			double	dCurrentUserCash = 0;
			char strItemID[ITEM_ID_SIZE];
			struct in_addr ConIp;

			ConIp.s_addr = pInput->m_ui32ClientIP;
			sprintf_s( strItemID, ITEM_ID_SIZE, "%d", pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID );

			INT32 i32Rv = g_pInte_IBytItem2A( pInput->m_ui64UserBillingID, pInput->m_strID, strItemID, pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price, i32PriceType, pInput->m_ui8GoodsCount,
				pInput->m_ui32MainBufferIdx, "PB_Server", pInput->m_strID, inet_ntoa(ConIp), &ui64BillingTransactionID, &dCurrentUserCash );

			if( i32Rv != innbill::INTEGRATION_API_DB_SUCCESS )
			{	// 실패
				g_pLog->WriteLog( L"[RU] Buy Failed [UID : %I64d][GoodsID : %d][ErrorCode : %d][Reason : (%s)]",
									pInput->m_TUID, pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID, i32Rv, g_pInte_PErrorDescriptionW(i32Rv) );
				return FALSE;
			}
			// 성공
			pOutput->m_ai32Rv[ui8GoodsIdx] = EVENT_ERROR_SUCCESS;
			pOutput->m_ui32MainBufferIdx = pInput->m_ui32MainBufferIdx;
			pOutput->m_ui32CurrentUserCash = (UINT32)dCurrentUserCash;
			pOutput->m_ui32CurrentUserCashType = TYPE_CURRENT_USER_CASH_USE;
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			innbill_check_user_status_result userStatus;
			g_pInte_ICheckUserStatus2A( pInput->m_ui64UserBillingID, pInput->m_strID, &userStatus );

			pOutput->m_ai32Rv[ui8GoodsIdx] = EVENT_ERROR_SUCCESS;
			pOutput->m_TUID = pInput->m_TUID;
			pOutput->m_ui32CurrentUserCash = (UINT32)userStatus.user_balance;
			pOutput->m_ui32CurrentUserCashType = TYPE_CURRENT_USER_CASH_USE;			
		}
		break;
	}
	return TRUE;
}