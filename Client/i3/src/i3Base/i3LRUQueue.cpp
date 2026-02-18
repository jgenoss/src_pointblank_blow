#include "i3CommonType.h"
#include "i3LRUQueue.h"
#include "i3FixedMemoryPool.h"

I3_CLASS_INSTANCE( i3LRUQueue);

bool	operator<(const I3_LRU_SORT& a1, const I3_LRU_SORT& a2)
{
	return a1.m_pData < a2.m_pData;	
}

i3LRUQueue::i3LRUQueue(void)
{
	m_pMemPool = new i3FixedMemoryPool;
	m_pMemPool->Create( "LRUQueuePool", sizeof( I3_LRU_SORT), 1024);
}

i3LRUQueue::~i3LRUQueue(void)
{
	DeleteAllData();

	I3MEM_SAFE_FREE( m_pEntity);
	m_pUsedHead = m_pUsedTail = m_pFree = nullptr;
	m_MaxCount = 0;

	delete m_pMemPool;
}

void i3LRUQueue::Create( INT32 cnt)
{
	m_MaxCount = cnt;

	m_pEntity = (I3_LRU_ENTRY *) i3MemAlloc( sizeof( I3_LRU_ENTRY) * m_MaxCount);

	m_DataList.reserve( cnt);

	Reset();
}

void i3LRUQueue::Reset(void)
{
	INT32 i;
	I3_LRU_ENTRY * pPrev = nullptr;

	for( i = 0; i < m_MaxCount; i++)
	{
		m_pEntity[i].m_pPrev = pPrev;
		m_pEntity[i].m_pNext = nullptr;

		if( pPrev != nullptr)
			pPrev->m_pNext = &m_pEntity[i];

		pPrev = &m_pEntity[i];
	}

	m_pUsedHead = m_pUsedTail = nullptr;
	m_pFree = m_pEntity;

	DeleteAllData();
}

void i3LRUQueue::DeleteAllData(void)
{
	
	for(size_t i = 0; i < m_DataList.size(); i++)
	{
		I3_LRU_SORT * pItem = m_DataList.get_vector()[i];

		m_pMemPool->Free( pItem);
	}

	m_DataList.clear();
}

INT32 i3LRUQueue::_FindData( void * pData)
{
	I3_LRU_SORT temp;

	temp.m_pData = pData;

//	INT32 idx = m_DataList.FindItem( &temp);
	i3::vector_set<I3_LRU_SORT*>::iterator it = m_DataList.find(&temp);
	size_t idx = i3::distance(m_DataList.begin(), it);
	return  ( idx != m_DataList.size() ) ? INT32(idx) : -1;
}

void * i3LRUQueue::FindData( void * pData)
{
	INT32 idx;

	idx = _FindData( pData);

	if( idx == -1)
		return nullptr;

	I3_LRU_SORT * pItem = m_DataList.get_vector()[idx];

	return pItem->m_pData;
}

void i3LRUQueue::DeleteData( void * pData)
{
	INT32 idx;

	idx = _FindData( pData);
	I3ASSERT( idx != -1);

	I3_LRU_SORT * pItem = m_DataList.get_vector()[idx];

	m_DataList.erase(m_DataList.begin() + idx);

	m_pMemPool->Free( pItem);
}

void i3LRUQueue::UnlinkFromUsedList( I3_LRU_ENTRY * pEntry)
{
	I3_LRU_ENTRY * pPrev, * pNext;

	pPrev = pEntry->m_pPrev;
	pNext = pEntry->m_pNext;

	if( pPrev != nullptr)
		pPrev->m_pNext = pNext;

	if( pNext != nullptr)
		pNext->m_pPrev = pPrev;

	if( m_pUsedHead == pEntry)
		m_pUsedHead = pNext;

	if( m_pUsedTail == pEntry)
		m_pUsedTail = pPrev;

	pEntry->m_pPrev = nullptr;
	pEntry->m_pNext = nullptr;

	//VerifyUsed();
}

void i3LRUQueue::LinkToUsedList( I3_LRU_ENTRY * pEntry)
{
	I3ASSERT( m_pUsedHead != pEntry);

	pEntry->m_pPrev = nullptr;
	pEntry->m_pNext = m_pUsedHead;

	if( m_pUsedHead != nullptr)
		m_pUsedHead->m_pPrev = pEntry;

	if( m_pUsedTail == nullptr)
		m_pUsedTail = pEntry;

	m_pUsedHead = pEntry;

	//VerifyUsed();
}

I3_LRU_ENTRY *	i3LRUQueue::AllocEntry( void * pData)
{
	I3_LRU_ENTRY * pEntry;

	I3ASSERT( m_pFree != nullptr);

	pEntry = m_pFree;
	m_pFree = m_pFree->m_pNext;

	if( m_pFree != nullptr)
		m_pFree->m_pPrev = nullptr;

	pEntry->m_pPrev = nullptr;
	pEntry->m_pNext = nullptr;

	{
		I3_LRU_SORT * pItem = (I3_LRU_SORT *) m_pMemPool->Alloc( sizeof(I3_LRU_SORT),__FILE__,__LINE__);

		pItem->m_pData = pData;
		pItem->m_pEntity = pEntry;
		pItem->m_pEntity->m_pData = pData;

		m_DataList.insert( pItem);

		I3ASSERT( (INT32)m_DataList.size() <= m_MaxCount);
	}

	return pEntry;
}

void i3LRUQueue::FreeEntry( I3_LRU_ENTRY * pEntry)
{
	UnlinkFromUsedList( pEntry);

	// Free List에 추가.
	pEntry->m_pNext = m_pFree;

	if( m_pFree != nullptr)
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
		I3_LRU_SORT * pItem = m_DataList.get_vector()[idx];

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

	I3ASSERT( m_pUsedTail != nullptr);

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

	I3_LRU_SORT * pItem = m_DataList.get_vector()[idx];

	FreeEntry( pItem->m_pEntity);
}

INT32 i3LRUQueue::GetUsedData( void ** pArray, INT32 maxCount)
{
	I3_LRU_ENTRY * pTemp;
	INT32 cnt = 0;

	pTemp = m_pUsedTail;

	while( (cnt < maxCount) && (pTemp != nullptr))
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

	while( pTemp != nullptr)
	{
		I3TRACE( "LRU [%d] : %p (%p)\n", cnt, pTemp, pTemp->m_pData);

		cnt++;
		pTemp = pTemp->m_pPrev;
	}
}

void i3LRUQueue::VerifyUsed(void)
{
	I3_LRU_ENTRY * pTemp, * pPrev = nullptr;

	pTemp = m_pUsedHead;

	while( pTemp != nullptr)
	{
		I3ASSERT( pPrev == pTemp->m_pPrev);
		I3ASSERT( pTemp->m_pPrev != pTemp);
		I3ASSERT( pTemp->m_pNext != pTemp);

		pPrev = pTemp;
		pTemp = pTemp->m_pNext;
	}

	I3ASSERT( pPrev == m_pUsedTail);

	if( pPrev != nullptr)
	{
		I3ASSERT( pPrev->m_pNext == nullptr);
	}
}
