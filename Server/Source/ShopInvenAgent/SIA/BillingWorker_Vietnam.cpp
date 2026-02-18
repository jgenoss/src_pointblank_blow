#include "pch.h"
#include "BillingWorker_Vietnam.h"

CBillingWorker_Vietnam::CBillingWorker_Vietnam()
{
}

CBillingWorker_Vietnam::~CBillingWorker_Vietnam()
{
}

BOOL CBillingWorker_Vietnam::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{
	wchar_t wstrSP[SERVER_STRING_COUNT];

	switch( pInput->m_ui8Action )
	{
	case TYPE_SHOP_GOODS_BUY:
		{
			UINT8 ui8ItemCount = 1;

			// Stored Procidure를 이용해서 Insert 한다.
			// DB와 반드시 일치해야 하는 값 : 유저 아이디, 아이템 코드, 아이템 금액
			i3String::Format(wstrSP, SERVER_STRING_COUNT, USP_SM_BUY_GOODS,
				pInput->m_TUID,			// UID
				pInput->m_stLinkBuy[ui8GoodsIdx].m_ui32Price,	// 아이템 금액
				pInput->m_stLinkBuy[ui8GoodsIdx].m_TGoodsID,	// 상품 ID
				ui8ItemCount,			// 아이템 갯수.
				pInput->m_ui32Rank		// 구매자 레벨
				);

			if( FALSE == m_pDB->ExecuteSP(wstrSP) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Vietnam::InputProcess] BUY SP Fail : %s ", wstrSP);
				CBillingDB::ReConnect(wstrSP);
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_ERROR_DATABASE_EXECUTE_SP;
				return FALSE;
			}
			_ParsingBuyItem( pInput, pOutput, ui8GoodsIdx );
		}
		break;
	case TYPE_SHOP_TYPE_GET_CASH:
		{
			pOutput->m_ui32CurrentUserCash = 0;
		
			i3String::Format(wstrSP, SERVER_STRING_COUNT, USP_SM_GET_CASH, pInput->m_TUID);

			if( FALSE == m_pDB->ExecuteSP(wstrSP) )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Vietnam::InputProcess] GET CASH SP Fail : %s ", wstrSP);
				CBillingDB::ReConnect(wstrSP);
				pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_ERROR_DATABASE_EXECUTE_SP;
				return FALSE;
			}

			INT32 i32Count = m_pDB->GetRecordCount();
			if( 1 != i32Count ) 
			{
				pOutput->m_ai32Rv[ui8GoodsIdx]		= EVENT_ERROR_DATABASE_SP_RESULT;

				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[CBillingWorker_Vietnam::InputProcess] GET CASH SP Fail / Record Count / UID : %I64d / Count : %d", pInput->m_TUID, i32Count);
				return FALSE;
			}

			INT32 i32Cash 	= m_pDB->GetINT32Data( 0 ); 

			// 음수이면 0으로.
			if( i32Cash < 0 ) pOutput->m_ui32CurrentUserCash = 0;
			else pOutput->m_ui32CurrentUserCash = i32Cash;

			pOutput->m_ai32Rv[ui8GoodsIdx]	= EVENT_ERROR_SUCCESS;
		}
		break;
	}	

	return TRUE;
}

void CBillingWorker_Vietnam::_ParsingBuyItem( BILLING_INPUT * pInput, BILLING_OUTPUT * pOut, UINT8 ui8GoodsIdx  )
{
	INT32 i32Count = m_pDB->GetRecordCount();

	if( 1 != i32Count )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Vietnam::InputProcess] RecordCount is Wrong / RecordCount :  %d", i32Count);
		return;
	}

	INT16 i16Result		= m_pDB->GetINT16Data( 0 );
	UINT64 ui64CurCash	= m_pDB->GetUINT64Data( 1 );

	switch( i16Result )
	{
	case 0:
		{
			pOut->m_ui32CurrentUserCash		= (UINT32)ui64CurCash;
			pOut->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;
			pOut->m_ai32Rv[ui8GoodsIdx]		= EVENT_ERROR_SUCCESS;
		}
		break;
	case -1:	// 존재하지 않는 PB 유저
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Vietnam::InputProcess] no Exist User(%I64d)'s request ", pInput->m_TUID);
			pOut->m_ui32CurrentUserCash		= (UINT32)0;
			pOut->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_NONE;
			pOut->m_ai32Rv[ui8GoodsIdx]		= EVENT_FAIL_BUY_FAIL_UNKNOWN;
		}
		break;
	case -2:	// 캐쉬를 충전한 기록이 없는 유저
	case -3:	// 보유 캐쉬가 Goods 가격보다 적음
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[CBillingWorker_Vietnam::InputProcess] Buy Fail / Not Enough Cash / User : %I64d, Cash : %I64d ", pInput->m_TUID, ui64CurCash);
			pOut->m_ui32CurrentUserCash		= (UINT32)ui64CurCash;
			pOut->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_USE;
			pOut->m_ai32Rv[ui8GoodsIdx]		= EVENT_FAIL_BUY_FAIL_NOT_ENOUGH_CASH;
		}
		break;
	case -9999:
	default:
		{
			pOut->m_ui32CurrentUserCash		= (UINT32)0;
			pOut->m_ui32CurrentUserCashType	= TYPE_CURRENT_USER_CASH_NONE;
			pOut->m_ai32Rv[ui8GoodsIdx]		= EVENT_FAIL_ERROR;
		}
		break;
	}
}