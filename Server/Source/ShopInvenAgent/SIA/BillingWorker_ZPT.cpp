#include "pch.h"
#include "BillingWorker_ZPT.h"

CBillingWorker_ZPT::CBillingWorker_ZPT()
{
}

CBillingWorker_ZPT::~CBillingWorker_ZPT()
{
}

BOOL CBillingWorker_ZPT::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{
	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_GOODS_BUY:
		{
			char strID[ MAX_PATH ];
			S2DB::ConvertString( strID, MAX_PATH, pInput->m_strBillingID );

			char strQuery[ MAX_PATH ];
			i3String::Format( strQuery, MAX_PATH, "EXECUTE usp_GameCash_mer N'%s', -%d", strID, pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price);
			if( FALSE == m_pDB->ExecuteSP( strQuery ) )
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_FAIL_ERROR;
				return FALSE;
			}

			INT32 i32Count = m_pDB->GetRecordCount();
			if( 0 >= i32Count )
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_FAIL_ERROR;
				return FALSE;
			}
			
			INT32 i32Result = m_pDB->GetINT16Data( 0 );
			if( 0 > i32Result )
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_FAIL_ERROR;
				return FALSE;
			}

			pOutput->m_ai32Rv[ui8GoodsIdx]			= EVENT_ERROR_SUCCESS;
			pOutput->m_ui32CurrentUserCash			= m_pDB->GetINT32Data( 1 );
			pOutput->m_ui32CurrentUserCashType		= TYPE_CURRENT_USER_CASH_NONE;
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			char strID[ MAX_PATH ];
			S2DB::ConvertString( strID, MAX_PATH, pInput->m_strBillingID );

			char strQuery[ MAX_PATH ];
			i3String::Format( strQuery, MAX_PATH, "EXECUTE usp_GameCash_sel N'%s'", strID );
			if( FALSE == m_pDB->ExecuteSP( strQuery ) )
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_FAIL_ERROR;
				pOutput->m_ui32CurrentUserCash	= 0;
				return FALSE;
			}

			INT32 i32Count = m_pDB->GetRecordCount();
			if( 0 >= i32Count )
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_FAIL_ERROR;
				pOutput->m_ui32CurrentUserCash	= 0;
				return FALSE;
			}

			pOutput->m_ai32Rv[ui8GoodsIdx]		= EVENT_ERROR_SUCCESS;	
			pOutput->m_ui32CurrentUserCash		= m_pDB->GetINT32Data( 0 );
			pOutput->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_NONE;
		}
		break;
	case TYPE_SHOP_USE_GIFTCOUPON:
		{
			char strCouponCode[GIFTCOUPON_LENGTH + 1];
			i3mem::Copy( strCouponCode, pInput->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );
			i3mem::Copy( pOutput->m_strCouponCode, pInput->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );

			pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_WRONG_NUMBER;
			switch( strCouponCode[0] )
			{
			case '0':	// 미사용된 올바른 쿠폰 - '0'으로 시작하고 'a'가 중간에 있는 쿠폰번호가 올바른 쿠폰번호이다. 2번째문자부터 a나오기 전까지의 숫자를 GoodsID로 지급한다.
				{
					INT32 i32Len = i3String::Length( strCouponCode );
					bool bFoundKey = false;
					for( INT32 i = 1; i < i32Len; i++ )
					{
						if ( 'a' == strCouponCode[i] || 'A' == strCouponCode[i] )
						{
							strCouponCode[i] = '\0';
							bFoundKey = true;
							break;
						}
					}
					if ( bFoundKey )
					{
						INT32 i32GoodsID = atoi( strCouponCode + 1 );
						pOutput->m_ai32Rv[0] = EVENT_ERROR_SUCCESS;
						pOutput->m_ai32Rv[1] = i32GoodsID;
						pOutput->m_ui8GoodsCount = 1;
					}
				}
				break;
			case '1':	// 이미 사용한 쿠폰 - '1'로 시작하면 이미 사용한 쿠폰
				pOutput->m_ai32Rv[0] = EVENT_ERROR_COUPON_ALREADY_USED;
				break;
			}
		}
		break;
	}

	return TRUE;
}