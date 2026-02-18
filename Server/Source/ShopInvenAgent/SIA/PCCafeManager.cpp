#include "pch.h"
#include "PCCafeManager.h"

#include "PCCafeCreator.h"

CPCCafeManager::CPCCafeManager()
{
	m_ui32PushThreadIdx	= 0;
	m_ui32PopThreadIdx	= 0;
	m_ppPCCafe			= NULL;
}

CPCCafeManager::~CPCCafeManager()
{
	Destroy();
}

BOOL CPCCafeManager::Create()
{
	m_ui32WorkThreadCount		= g_pConfig->m_i32PCCafeThreadCount;

	m_ppPCCafe = new CPCCafeBase*[ m_ui32WorkThreadCount ];
	if( NULL == m_ppPCCafe )														return FALSE;
	i3mem::FillZero( m_ppPCCafe,	sizeof(CPCCafeBase*) * m_ui32WorkThreadCount ); 

	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ )
	{
		m_ppPCCafe[i] = CPCCafeCreator::GetWorker( g_pConfig->m_ePCCafeMethod );
		if( NULL == m_ppPCCafe[i] )													return FALSE;
		if( FALSE == m_ppPCCafe[i]->OnCreate( i ))									return FALSE;
	}

	return TRUE;
}

void CPCCafeManager::Destroy(void)
{
	if( m_ppPCCafe )
	{
		for(UINT32 i = 0; i < m_ui32WorkThreadCount; i++)
		{
			I3_SAFE_DELETE( m_ppPCCafe[i] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppPCCafe ); 
	}
}

INT32 CPCCafeManager::PushData( PCCAFE_INPUT* pInputData )
{
	INT32 i32Result = m_ppPCCafe[ m_ui32PushThreadIdx ]->Push_T( pInputData );
	
	m_ui32PushThreadIdx++;
	if( m_ui32PushThreadIdx >= m_ui32WorkThreadCount) m_ui32PushThreadIdx = 0;

	// 아래와 같은 반환값이 Return되어집니다.
	// RINGBUFFER_SUCCESS					
	// RINGBUFFER_ERROR_BUFFER_FULLIDX
	// RINGBUFFER_ERROR_BUFFER_FULL

	return i32Result;
}

INT32	CPCCafeManager::PopCount_T(void)
{
	INT32 i32Count = 0;
	
	// 사용버퍼의 갯수를 리턴.
	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ ) 
	{
		i32Count += m_ppPCCafe[i]->PopCount_T();
	}

	return i32Count;
}

PCCAFE_OUTPUT * CPCCafeManager::PopData_T()
{
	PCCAFE_OUTPUT* pBillingData = NULL; 

	// 순차적으로 Loop를 돌면서 Output버퍼에 담긴 값을 검색합니다.
	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ )
	{		
		m_ui32PopThreadIdx++; 
		if( m_ui32PopThreadIdx >= m_ui32WorkThreadCount )	m_ui32PopThreadIdx = 0; 

		pBillingData = m_ppPCCafe[ m_ui32PopThreadIdx ]->Pop_T();
		if( NULL != pBillingData )	break;
	}

	return pBillingData;
}

void CPCCafeManager::PopIdx_T()
{
	m_ppPCCafe[ m_ui32PopThreadIdx ]->PopIdx_T(); 
}