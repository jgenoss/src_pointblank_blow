#include "i3SceneDef.h"
#include "i3PhysixMemoryPool.h"

I3_CLASS_INSTANCE( i3PhysixMemoryPool);

i3PhysixMemoryPool::i3PhysixMemoryPool(void)
{
	m_FreeList.reserve( 1024);
}

void * i3PhysixMemoryPool::Alloc( UINT32 sz)
{
	INT32 si, ei, idx = -1, idxFind = -1;
	I3_PHYSIX_MEMINFO * pInfo = nullptr;

	m_TotalAllocReq ++; 

	// Free List를 Binary Search하면서 적당한 위치를 찾는다.
	{
		si = 0;
		ei = (INT32)m_FreeList.size() - 1;

		while( si <= ei)
		{
			idx = (si + ei) >> 1;
			pInfo = m_FreeList.get_vector()[ idx].second;

			if( pInfo->m_Size == sz)
			{
				idxFind = idx;
				break;
			}
			else if( pInfo->m_Size > sz)
				ei = idx - 1;
			else
				si = idx + 1;
		}
	}

	if( pInfo != nullptr)
	{
		while( (idx < (INT32)m_FreeList.size()) && (pInfo->m_Size < sz))
		{
			idx++;
			pInfo = (I3_PHYSIX_MEMINFO *) m_FreeList.get_vector()[ idx].second;
		}

		if( idx < (INT32)m_FreeList.size())
		{
			// 적당한 Free Block이 있다.

			// 혹, 크기의 비율이 2배 가까이 난다면 적절하지 않은 Block으로 간주한다.
			// 이것은 심한 메모리의 낭비를 막기 위함.
			REAL32 fact = (REAL32) pInfo->m_Size / sz;

			if( fact < 1.5f)
			{
				m_FreeList.erase(m_FreeList.begin() + idx);

				m_AllocList.insert(    pInfo);

				m_HitAllocReq ++;

				return (void *) (pInfo + 1);
			}
		}
	}

	// 적당한 Block이 없다.
	// 새롭게 할당...
	pInfo = (I3_PHYSIX_MEMINFO *) i3MemAlloc( sizeof(I3_PHYSIX_MEMINFO) + sz);

	m_Size += sizeof(I3_PHYSIX_MEMINFO) + sz;

	pInfo->m_Size = sz;

	m_AllocList.insert( pInfo);		// 해시 셋..으로 순서는 없다..

	return (void *)( pInfo + 1);
}

void *	i3PhysixMemoryPool::Realloc( void * p, UINT32 size)
{
	I3_PHYSIX_MEMINFO * pInfo; // = (I3_PHYSIX_MEMINFO *) p;
	
	I3ASSERT( p != nullptr);

//	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocFindProc);

//	idx = m_AllocList.FindItem( p);				
	// 원 포인터에서 헤더를 더한것을 실 포인터로 했기 때문에 헤더를 빼고 넣는다..
	i3::unordered_set<I3_PHYSIX_MEMINFO*>::iterator it = m_AllocList.find( static_cast<I3_PHYSIX_MEMINFO*>(p) - 1);

//	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocCompareProc);
	
	if( it == m_AllocList.end() )
		return nullptr;

	I3ASSERT( (static_cast<I3_PHYSIX_MEMINFO*>(p) - 1) == *it );

	pInfo = *it;		// find에 넣은 값과 *it는 같은 값이다.(I3ASSERT로 확인필요)		//	(I3_PHYSIX_MEMINFO *) m_AllocList.Items[idx];


	if( pInfo->m_Size >= size)
	{
		// 이미 충분한 크기를 가지고 있기 때문에 그냥 반환
		return p;
	}

	m_Size  -= pInfo->m_Size + sizeof(I3_PHYSIX_MEMINFO);

	pInfo = (I3_PHYSIX_MEMINFO *) i3MemReAlloc( pInfo, size + sizeof(I3_PHYSIX_MEMINFO));

	pInfo->m_Size = size;

	m_Size  += size + sizeof(I3_PHYSIX_MEMINFO);

	// m_AllocList.Items[idx] = pInfo;					// 이건 루틴에 하자가 있다...원소스코드는 정렬을 기준으로 했기 때문에 아무렇게나 포인터를 변경한 것 그대로..
													// 대입해 놔두는건 좋지 않다..제대로 하려면 예전pInfo를 떼고 새 pInfo를 다시 넣어야한다..
	m_AllocList.erase(it);
	m_AllocList.insert(pInfo);

	return (void *) (pInfo + 1);
}

bool i3PhysixMemoryPool::Free( void * pBuff)
{
	I3_PHYSIX_MEMINFO * pInfo;

//	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocFindProc);
//
//	idx = m_AllocList.FindItem( pBuff);
//
//	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocCompareProc);
	i3::unordered_set<I3_PHYSIX_MEMINFO*>::iterator it = m_AllocList.find( static_cast<I3_PHYSIX_MEMINFO*>(pBuff) - 1);

	if( it == m_AllocList.end() )
		return false;

	pInfo = (*it); // (I3_PHYSIX_MEMINFO *) m_AllocList.Items[idx];

	m_AllocList.erase( it);
	m_FreeList.insert( i3::vector_multimap<UINT32, I3_PHYSIX_MEMINFO*>::value_type(pInfo->m_Size, pInfo) );

	return true;
}

bool i3PhysixMemoryPool::IsAlloced( char * p)
{
	/*
	INT32 idx;

	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocFindProc);

	idx = m_AllocList.FindItem( p);

	m_AllocList.SetCompareProc( (COMPAREPROC) _AllocCompareProc);

	return idx != -1;
	*/
	i3::unordered_set<I3_PHYSIX_MEMINFO*>::iterator it = m_AllocList.find( reinterpret_cast<I3_PHYSIX_MEMINFO*>(p) - 1);

	return ( it != m_AllocList.end() );

}

void	i3PhysixMemoryPool::ReleaseAllocList(void)
{
	I3_PHYSIX_MEMINFO * pInfo;

	Contact();
/*
	for( i = 0; i < m_AllocList.GetCount(); i++)
	{
		pInfo = (I3_PHYSIX_MEMINFO *) m_AllocList.Items[i];

		i3MemFree( pInfo);
	}
*/
	i3::unordered_set<I3_PHYSIX_MEMINFO*>::iterator it_end = m_AllocList.end();
	for (i3::unordered_set<I3_PHYSIX_MEMINFO*>::iterator it = m_AllocList.begin() ; it != it_end ; ++it )
	{
		pInfo = *it;
		i3MemFree(pInfo);
	}

	m_AllocList.clear();
	m_FreeList.clear();					//Contact보다 먼저 호출되면 이 함수가 문제가 된다..(2012.05.23.수빈)
											
}											

void	i3PhysixMemoryPool::Contact(void)
{
	I3_PHYSIX_MEMINFO * pInfo;

	for(size_t i = 0; i < m_FreeList.size(); i++)
	{
		pInfo = m_FreeList.get_vector()[i].second;
		i3MemFree( pInfo);
	}

	m_FreeList.clear();
}
