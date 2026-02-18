#include "stdafx.h"
#include "Link_Innova.h"

cLink_Innova::cLink_Innova()
{
}
 
cLink_Innova::~cLink_Innova()
{
	ReleaseThread();
	OnDestroy();
}

BOOL	cLink_Innova::OnCreate( INT32 i32Idx, NSM_CRingBuffer* pInputBuffer, NSM_CRingBuffer* pOutputBuffer )
{
	return CLinkBase::OnCreate( i32Idx, pInputBuffer, pOutputBuffer ); 
}

void	cLink_Innova::OnDestroy(void)
{
}

BOOL	cLink_Innova::InputProcess( LINK_INPUT * pInput )
{
#if defined( LINK_INTE )
	
	UINT64	ui64BillingTransactionID = 0;
	double	dCurrentUserBalance = 0;
	INT32	item_price_type = 0;
	
	if( pInput->ui8Action == TYPE_SHOP_ITEM_REPAIR ) item_price_type = 1;

	// Link Start Time
	#ifdef DEF_LINK_TIME_CHECK
		DWORD dwOldTime = GetTickCount();
	#endif
	//
	INT32	i32Rv = innbill_buy_item_iia( pInput->ui64UserBillingID, pInput->ui32ItemID, 0, pInput->ui32Price, item_price_type, pInput->ui8ItemCount,  
		   						pInput->_ui32MainBufferIdx, "PB_Server", pInput->strUserID, &ui64BillingTransactionID, &dCurrentUserBalance);

	if( 0 != i32Rv )
	{	// 실패
		char strLog[ MAX_PATH ]; //정확한 사이즈를 몰라서 이렇게 넣습니다. 
		NSMString::Format( strLog, MAX_PATH, "[RUSSIA]Buy Failed [UID: %I64d][Goods ID: %d][ErrorCode: %d][Reason:( %s )]", 
							pInput->_i64UID, pInput->ui32ItemID, i32Rv, innbill_error_description_a( i32Rv ) );
		g_pModuleLogFile->Write_Log_Link( m_i32Idx, strLog);

		return FALSE;
	}

	// Buy Success
	LINK_OUTPUT * pOut = (LINK_OUTPUT*)m_pOutputBuffer->PushPointer(); 
	if( pOut )
	{
		pOut->i32ReturnCode			= EVENT_SUCCESS;
		pOut->_ui32MainBufferIdx	= pInput->_ui32MainBufferIdx;
		m_pOutputBuffer->PushPointerIdx(); 
	}
	else
	{
		g_pModuleLogFile->Write_Log_Link( m_i32Idx, "[ERROR] Link Output Buffer Full 2", WARNING_DATABASE_TIMEOUT );
	}
	//Link End Time
	#ifdef DEF_LINK_TIME_CHECK
		DWORD dwNewTime = GetTickCount() - dwOldTime;
		if( dwNewTime > DEF_LINK_LIMIT_TIME )
		{
			char strLog[ 1024 ];
			NSMString::Format( strLog, 1024, "---[%d]Link_Innova Time Over : %d", m_i32Idx, dwNewTime );		
			g_pModuleLogFile->Write_Log_Link( m_i32Idx, strLog, WARNING_AUTH );
		}
	#endif

#endif

	return TRUE;
}
