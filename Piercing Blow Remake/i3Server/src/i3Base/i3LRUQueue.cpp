#include "i3CommonType.h"
#include "i3LRUQueue.h"

I3_CLASS_INSTANCE( i3LRUQueue, i3ElementBase);

static INT32	_SortProc( I3_LRU_SORT * p1, I3_LRU_SORT * p2)
{
	if( p1->m_pData > p2->m_pData)
		return 1;
	else if( p1->m_pData < p2->m_pData)
		return -1;

	return 0;
}

i3LRUQueue::i3LRUQueue(void)
{
	m_pEntity = NULL;
	m_pUsedHead = m_pUsedTail = m_pFree = NULL;
	m_MaxCount = 0;

	m_DataList.SetCompareProc( (COMPAREPROC) _SortProc);
	m_DataList.SetOptmizeEnableWhenClear( FALSE);
}

i3LRUQueue::~i3LRUQueue(void)
{
	DeleteAllData();

	I3MEM_SAFE_FREE( m_pEntity);
	m_pUsedHead = m_pUsedTail = m_pFree = NULL;
	m_MaxCount = 0;
}

void i3LRUQueue::SetCompareProc( COMPAREPROC pUserProc)
{
	m_DataList.SetCompareProc( pUserProc);
}

void i3LRUQueue::Create( INT32 cnt)
{
	m_MaxCount = cnt;

	m_pEntity = (I3_LRU_ENTRY *) i3MemAlloc( sizeof( I3_LRU_ENTRY) * m_MaxCount);

	m_DataList.SetAllocationUnit( cnt);

	Reset();
}

void i3LRUQueue::Reset(void)
{
	INT32 i;
	I3_LRU_ENTRY * pPrev = NULL;

	for( i = 0; i < m_MaxCount; i++)
	{
		m_pEntity[i].m_pPrev = pPrev;
		m_pEntity[i].m_pNext = NULL;

		if( pPrev != NULL)
			pPrev->m_pNext = &m_pEntity[i];

		pPrev = &m_pEntity[i];
	}

	m_pUsedHead = m_pUsedTail = NULL;
	m_pFree = m_pEntity;

	DeleteAllData();
}

void i3LRUQueue::DeleteAllData(void)
{
	INT32 i;

	for( i = 0; i < m_DataList.GetCount(); i++)
	{
		I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_DataList.GetItem( i);

		i3MemFree( pItem);
	}

	m_DataList.Clear();
}

INT32 i3LRUQueue::_FindData( void * pData)
{
	I3_LRU_SORT temp;

	temp.m_pData = pData;

	INT32 idx = m_DataList.FindItem( &temp);

	return idx;
}

void * i3LRUQueue::FindData( void * pData)
{
	INT32 idx;

	idx = _FindData( pData);

	if( idx == -1)
		return NULL;

	I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_DataList.GetItem( idx);

	return pItem->m_pData;
}

void i3LRUQueue::DeleteData( void * pData)
{
	INT32 idx;

	idx = _FindData( pData);
	I3ASSERT( idx != -1);

	I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_DataList.GetItem( idx);

	m_DataList.Delete( idx);

	i3MemFree( pItem);
}

void i3LRUQueue::UnlinkFromUsedList( I3_LRU_ENTRY * pEntry)
{
	I3_LRU_ENTRY * pPrev, * pNext;

	pPrev = pEntry->m_pPrev;
	pNext = pEntry->m_pNext;

	if( pPrev != NULL)
		pPrev->m_pNext = pNext;

	if( pNext != NULL)
		pNext->m_pPrev = pPrev;

	if( m_pUsedHead == pEntry)
		m_pUsedHead = pNext;

	if( m_pUsedTail == pEntry)
		m_pUsedTail = pPrev;

	pEntry->m_pPrev = NULL;
	pEntry->m_pNext = NULL;

	//VerifyUsed();
}

void i3LRUQueue::LinkToUsedList( I3_LRU_ENTRY * pEntry)
{
	I3ASSERT( m_pUsedHead != pEntry);

	pEntry->m_pPrev = NULL;
	pEntry->m_pNext = m_pUsedHead;

	if( m_pUsedHead != NULL)
		m_pUsedHead->m_pPrev = pEntry;

	if( m_pUsedTail == NULL)
		m_pUsedTail = pEntry;

	m_pUsedHead = pEntry;

	//VerifyUsed();
}

I3_LRU_ENTRY *	i3LRUQueue::AllocEntry( void * pData)
{
	I3_LRU_ENTRY * pEntry;

	I3ASSERT( m_pFree != NULL);

	pEntry = m_pFree;
	m_pFree = m_pFree->m_pNext;

	if( m_pFree != NULL)
		m_pFree->m_pPrev = NULL;

	pEntry->m_pPrev = NULL;
	pEntry->m_pNext = NULL;

	{
		I3_LRU_SORT * pItem = (I3_LRU_SORT *) i3MemAlloc( sizeof( I3_LRU_SORT));

		pItem->m_pData = pData;
		pItem->m_pEntity = pEntry;
		pItem->m_pEntity->m_pData = pData;

		m_DataList.Add( pItem);

		I3ASSERT( m_DataList.GetCount() <= m_MaxCount);
	}

	return pEntry;
}

void i3LRUQueue::FreeEntry( I3_LRU_ENTRY * pEntry)
{
	UnlinkFromUsedList( pEntry);

	// Free List에 추가.
	pEntry->m_pNext = m_pFree;

	if( m_pFree != NULL)
		m_pFree->m_pPrev = pEntry;

	m_pFree = pEntry;

	// Sort List에서 제거
	DeleteData( pEntry->m_pData);
}

void i3LRUQueue::PushUsed( void * pData, UINT32 tm)
{
	INT32 idx;
	I3_LRU_ENTRY * pEntry;

	// 기존의 List에 존재하는지 우선 검사
	idx = _FindData( pData);

	if( idx != -1)
	{
		I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_DataList.GetItem( idx);

		pEntry = pItem->m_pEntity;

		// Used List의 중간에 있다면, 선두로 옮긴다.
		if( pEntry != m_pUsedHead)
		{
			// Used List에서 제거
			UnlinkFromUsedList( pEntry);

			// Used List의 선두에 추가.
			LinkToUsedList( pEntry);
		}
	}
	else
	{
		pEntry = AllocEntry( pData);

		// Used List의 선두에 추가.
		LinkToUsedList( pEntry);
	}

	pEntry->m_pData = pData;
	pEntry->m_LastUsed = tm;
}

void * i3LRUQueue::PopOldest(void)
{
	I3_LRU_ENTRY * pEntry;

	I3ASSERT( m_pUsedTail != NULL);

	// Used List에서 제거
	pEntry = m_pUsedTail;

	FreeEntry( pEntry);

	return pEntry->m_pData;
}

void i3LRUQueue::Remove( void * pData)
{
	INT32 idx = _FindData( pData);

	if( idx == -1)
		return;

	I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_DataList.GetItem( idx);

	FreeEntry( pItem->m_pEntity);
}

INT32 i3LRUQueue::GetUsedData( void ** pArray, INT32 maxCount)
{
	I3_LRU_ENTRY * pTemp;
	INT32 cnt = 0;

	pTemp = m_pUsedTail;

	while( (cnt < maxCount) && (pTemp != NULL))
	{
		pArray[ cnt] = pTemp->m_pData;
		cnt++;

		pTemp = pTemp->m_pPrev;
	}

	return cnt;
}

void i3LRUQueue::DumpUsed(void)
{
	I3_LRU_ENTRY * pTemp;
	INT32 cnt = 0;

	pTemp = m_pUsedTail;

	I3TRACE( "--- LRU Dump\n");

	while( pTemp != NULL)
	{
		I3TRACE( "LRU [%d] : %p (%p)\n", cnt, pTemp, pTemp->m_pData);

		cnt++;
		pTemp = pTemp->m_pPrev;
	}
}

void i3LRUQueue::VerifyUsed(void)
{
	I3_LRU_ENTRY * pTemp, * pPrev = NULL;

	pTemp = m_pUsedHead;

	while( pTemp != NULL)
	{
		I3ASSERT( pPrev == pTemp->m_pPrev);
		I3ASSERT( pTemp->m_pPrev != pTemp);
		I3ASSERT( pTemp->m_pNext != pTemp);

		pPrev = pTemp;
		pTemp = pTemp->m_pNext;
	}

	I3ASSERT( pPrev == m_pUsedTail);

	if( pPrev != NULL)
	{
		I3ASSERT( pPrev->m_pNext == NULL);
	}
}
