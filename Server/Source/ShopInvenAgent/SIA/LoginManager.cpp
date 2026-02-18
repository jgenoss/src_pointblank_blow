#include "pch.h"
#include "LoginManager.h"
#include "LoginCreator.h"

CLoginManager::CLoginManager(void)
{
	m_ui32PushThreadIdx		= 0;
	m_ui32PopThreadIdx		= 0;
}

CLoginManager::~CLoginManager(void)
{
	OnDestroy(); 
}

BOOL CLoginManager::OnCreate( UINT8 ui8MethodIdx )
{
	m_ui32WorkThreadCount		= g_pConfig->m_i32LoginThreadCount;

	//Create Work Thread
	m_ppLogInModule = new CLoginBase*[ m_ui32WorkThreadCount ];
	if( NULL == m_ppLogInModule )																return FALSE;
	for(UINT32 i = 0; i < m_ui32WorkThreadCount; i++)
	{
		m_ppLogInModule[i] = CLoginCreator::GetWorker( g_pConfig->m_aeLoginMethod[ui8MethodIdx] );
		if( NULL == m_ppLogInModule[i] )														return FALSE;
		if( FALSE == m_ppLogInModule[i]->OnCreate( i, g_pConfig->m_i32LogInBufferCount, ui8MethodIdx ) )		return FALSE;
	}

	return TRUE;
}

void CLoginManager::OnDestroy(void)
{
	//Release Work Thread 
	if( m_ppLogInModule )
	{
		for( UINT32 i = 0; i < m_ui32WorkThreadCount ; i++ )
		{
			I3_SAFE_DELETE( m_ppLogInModule[i] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppLogInModule ); 
	}
}

INT32 CLoginManager::Push_T( LOGIN_INPUT* pLogInData )
{
	INT32 i32Count;

	for(UINT32 i = 0; i < m_ui32WorkThreadCount; i++)
	{
		m_ui32PushThreadIdx++; 
		if( m_ui32PushThreadIdx >= m_ui32WorkThreadCount )	m_ui32PushThreadIdx = 0; 

		pLogInData->m_ui8LinkThreadIdx = (UINT8)m_ui32PushThreadIdx;
		i32Count = m_ppLogInModule[m_ui32PushThreadIdx]->Push_T( pLogInData );
		if( -1 != i32Count )	return i32Count;
	}

	return -1;
}

INT32 CLoginManager::PopCount_T(void)
{
	INT32 i32Count = 0; 
	for( UINT32 i = 0 ; i < m_ui32WorkThreadCount ; i++ )
	{
		i32Count += m_ppLogInModule[ i ]->PopCount_T();
	}
	return i32Count; 
}

LOGIN_OUTPUT * CLoginManager::Pop_T(void)
{
	LOGIN_OUTPUT* pLogInData;

	for( UINT32 i = 0; i < m_ui32WorkThreadCount ; i++ )
	{
		m_ui32PopThreadIdx++; 
		if( m_ui32PopThreadIdx >= m_ui32WorkThreadCount )	m_ui32PopThreadIdx = 0; 
		
		pLogInData = m_ppLogInModule[ m_ui32PopThreadIdx ]->Pop_T();
		if( NULL != pLogInData )	return pLogInData;
	}
	
	return NULL; 
}
