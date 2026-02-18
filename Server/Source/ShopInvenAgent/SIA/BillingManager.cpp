#include "pch.h"
#include "BillingManager.h"

#include "BillingCreator.h"

CBillingManager::CBillingManager()
{
	m_ui32PushThreadIdx	= 0;
	m_ui32PopThreadIdx	= 0;
	m_ppBilling			= NULL;
}

CBillingManager::~CBillingManager()
{
	Destroy();
}

BOOL CBillingManager::Create( UINT8 ui8MethodIdx )
{
	m_ui32WorkThreadCount		= g_pConfig->m_i32BillingThreadCount;

	m_ppBilling = new CBillingBase*[ m_ui32WorkThreadCount ];
	if( NULL == m_ppBilling )														return FALSE;
	i3mem::FillZero( m_ppBilling,	sizeof(CBillingBase*) * m_ui32WorkThreadCount ); 

	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ )
	{
		m_ppBilling[i] = CBillingCreator::GetWorker( g_pConfig->m_aeBillingMethod[ui8MethodIdx] );
		if( NULL == m_ppBilling[i] )												return FALSE;
		if( FALSE == m_ppBilling[i]->OnCreate( i, ui8MethodIdx ))					return FALSE;
	}

	return TRUE;
}

void CBillingManager::Destroy(void)
{
	if( m_ppBilling )
	{
		for(UINT32 i = 0; i < m_ui32WorkThreadCount; i++)
		{
			I3_SAFE_DELETE( m_ppBilling[i] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppBilling ); 
	}
}

INT32 CBillingManager::PushData( BILLING_INPUT* pInputData )
{
	INT32 i32Result = m_ppBilling[ m_ui32PushThreadIdx ]->Push_T( pInputData );
	
	m_ui32PushThreadIdx++;
	if( m_ui32PushThreadIdx >= m_ui32WorkThreadCount) m_ui32PushThreadIdx = 0;

	// 아래와 같은 반환값이 Return되어집니다.
	// RINGBUFFER_SUCCESS					
	// RINGBUFFER_ERROR_BUFFER_FULLIDX
	// RINGBUFFER_ERROR_BUFFER_FULL

	return i32Result;
}

INT32	CBillingManager::PopCount_T(void)
{
	INT32 i32Count = 0;
	
	// 사용버퍼의 갯수를 리턴.
	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ ) 
	{
		i32Count += m_ppBilling[i]->PopCount_T();
	}

	return i32Count;
}

BILLING_OUTPUT * CBillingManager::PopData_T()
{
	BILLING_OUTPUT* pBillingData = NULL; 

	// 순차적으로 Loop를 돌면서 Output버퍼에 담긴 값을 검색합니다.
	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ )
	{		
		m_ui32PopThreadIdx++; 
		if( m_ui32PopThreadIdx >= m_ui32WorkThreadCount )	m_ui32PopThreadIdx = 0; 

		pBillingData = m_ppBilling[ m_ui32PopThreadIdx ]->Pop_T();
		if( NULL != pBillingData )	break;
	}

	return pBillingData;
}

void CBillingManager::PopIdx_T()
{
	m_ppBilling[ m_ui32PopThreadIdx ]->PopIdx_T(); 
}