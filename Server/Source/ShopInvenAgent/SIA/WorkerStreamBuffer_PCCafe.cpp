#include "pch.h"
#include "ModuleNet.h"
#include "WorkerStreamBuffer_PCCafe.h"
#include "HashTable.h"
#include "ModuleTask.h"

#define WORK_STREAM_BUFFER_TIMEOUT_SECOND	30

CWorkerStreamBuffer_PCCafe::CWorkerStreamBuffer_PCCafe()
{
	m_i32TotalBufferCount	= 0;
	m_i32UseBufferCount		= 0;	
	m_i32BlankIdx			= 0;
	m_i32PushIdx			= 0;

	m_pi32BufferIndex		= NULL;	
	m_pBuffer				= NULL;
}

CWorkerStreamBuffer_PCCafe::~CWorkerStreamBuffer_PCCafe()
{
	I3_SAFE_DELETE_ARRAY( m_pi32BufferIndex );
	I3_SAFE_DELETE_ARRAY( m_pBuffer );
	delete m_pUIDTable;
}

static INT32 _UID_CompareProc( void* p1, void* p2 )
{
	T_UID i64UID1 = ((WORKER_STREAM_BUFFER_PCCAFE*)p1)->m_i64UID;
	T_UID i64UID2 = ((WORKER_STREAM_BUFFER_PCCAFE*)p2)->m_i64UID;

	if ( i64UID1 > i64UID2 )			return 1;
	else if ( i64UID1 < i64UID2 )		return -1;

	return 0;
}

INT32 CWorkerStreamBuffer_PCCafe::CreateBuffer( INT32 i32Count )
{
	m_pi32BufferIndex = new INT32[ i32Count ];

	if( NULL == m_pi32BufferIndex )
		return -1;

	m_pBuffer = new WORKER_STREAM_BUFFER_PCCAFE[ i32Count ];

	if( NULL == m_pBuffer )
		return -2;

	i3mem::FillZero( m_pBuffer, sizeof(WORKER_STREAM_BUFFER_PCCAFE)*i32Count );

	for( INT32 i = 0; i < i32Count; i++ )
	{
		m_pi32BufferIndex[i]	= i;
		m_pBuffer[i].m_bEmpty	= true;
	}

	m_i32TotalBufferCount	= i32Count;
	m_i32UseBufferCount		= 0;

	// Make Hash Table 
	m_pUIDTable	= new CHashTable;
	if( NULL == m_pUIDTable )
		return -3;
	if( FALSE == m_pUIDTable->Create( MAX_CONNECT_USER_DIVISOR, _UID_CompareProc ) )
		return -4;

	return EVENT_ERROR_SUCCESS;
}

INT32 CWorkerStreamBuffer_PCCafe::Push( TYPE_PCCAFE_INPUT eType, T_UID i64UID, UINT32 ui32IP, UINT32 ui32PCGUID )
{
	if ( m_i32TotalBufferCount == m_i32UseBufferCount )
		return -1;

	INT32 i32idx = _GetNextBufferIdx();
	WORKER_STREAM_BUFFER_PCCAFE* pBuffer = &m_pBuffer[i32idx];

	if ( FALSE == m_pUIDTable->Insert( i64UID, pBuffer) )
		return -2;

	pBuffer->m_bEmpty			= false;
	pBuffer->m_i64UID			= i64UID;
	pBuffer->m_i8Type			= (INT8)eType;
	pBuffer->m_ui32IP			= ui32IP;
	pBuffer->m_ui32PCGUID		= ui32PCGUID;
	pBuffer->m_ui32ServerTime	= i3ThreadTimer::GetServerTime() + WORK_STREAM_BUFFER_TIMEOUT_SECOND;

	return i32idx;
}

// 타임아웃 체크용
void CWorkerStreamBuffer_PCCafe::UpdateBuffer()
{
	for( INT32 i = 0; i < m_i32TotalBufferCount; i++ )
	{
		if ( m_pBuffer[i].m_bEmpty )
			continue;

		// 타임아웃 검사
		if ( m_pBuffer[i].m_ui32ServerTime < i3ThreadTimer::GetServerTime() )
		{
			SetBlankBuffer( i );
		}
	}
}

INT32 CWorkerStreamBuffer_PCCafe::_GetNextBufferIdx()
{
	INT32 i32Idx = m_pi32BufferIndex[ m_i32PushIdx ];

	m_i32PushIdx++;
	m_i32PushIdx %= m_i32TotalBufferCount;

	m_i32UseBufferCount++;

	return i32Idx;
}

void CWorkerStreamBuffer_PCCafe::SetBlankBuffer( INT32 i32BufferIdx )
{
	T_UID i64UID = m_pBuffer[i32BufferIdx].m_i64UID;
	WORKER_STREAM_BUFFER_PCCAFE* pBuffer = FindByUID( i64UID );
	if( pBuffer )
		m_pUIDTable->Delete( i64UID, pBuffer );

	m_pi32BufferIndex[m_i32BlankIdx] = i32BufferIdx;
	i3mem::FillZero( &m_pBuffer[i32BufferIdx], sizeof( WORKER_STREAM_BUFFER_PCCAFE ) );
	m_pBuffer[i32BufferIdx].m_bEmpty = true;

	m_i32BlankIdx++;
	m_i32BlankIdx %= m_i32TotalBufferCount;
	m_i32UseBufferCount--;
}

void CWorkerStreamBuffer_PCCafe::SetBlankBuffer( WORKER_STREAM_BUFFER_PCCAFE* pBuffer )
{
	INT32 i32Idx = pBuffer - m_pBuffer;
	
	if ( i32Idx < 0 )						return;
	if ( i32Idx >= m_i32TotalBufferCount )	return;
	
	SetBlankBuffer( i32Idx );
}

WORKER_STREAM_BUFFER_PCCAFE* CWorkerStreamBuffer_PCCafe::FindByUID( T_UID i64UID )
{
	i3BinList* pList = m_pUIDTable->Retrieve( i64UID );
	INT32 i32Idx = pList->FindItem( &i64UID );
	if ( i32Idx < 0 ) return NULL;
	return (WORKER_STREAM_BUFFER_PCCAFE*)pList->GetItem( i32Idx );
}