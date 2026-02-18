#include "pch.h"
#include "BillingWorker_NO.h"

CBillingWorker_NO::CBillingWorker_NO()
{
}

CBillingWorker_NO::~CBillingWorker_NO()
{
}

BOOL CBillingWorker_NO::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{	pInput;

	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_USE_GIFTCOUPON:
		pOutput->m_ai32Rv[ui8GoodsIdx]			= EVENT_ERROR_COUPON_WRONG_NUMBER;
		break;
	default:
		pOutput->m_ai32Rv[ui8GoodsIdx]			= EVENT_ERROR_SUCCESS;
		break;
	}

	pOutput->m_ui32CurrentUserCash			= 999999999;
	pOutput->m_ui32CurrentUserCashType		= TYPE_CURRENT_USER_CASH_NONE;

	return TRUE;
}