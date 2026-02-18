#include "i3CommonType.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3FixedMemoryPool.h"
#include "i3Memory.h"
#include "i3Macro.h"

#if defined( I3_DEBUG)
static i3Mutex2 *			s_pPoolMutex = nullptr;
static i3FixedMemoryPool * s_pPoolHead = nullptr;

namespace
{
	void cb_atexit_destroy()
	{
		I3_SAFE_DELETE( s_pPoolMutex);
	}
}
#endif

i3FixedMemoryPool::i3FixedMemoryPool(void)
{
	#if defined( I3_DEBUG)
		if( s_pPoolMutex == nullptr)
		{
			s_pPoolMutex = new i3Mutex2;
			NetworkDump_Malloc( s_pPoolMutex, sizeof( i3Mutex2), __FILE__, __LINE__);
			::atexit(cb_atexit_destroy);
		}
		m_pNext = s_pPoolHead;
		s_pPoolHead = this;
	#endif

	m_pMutex = new i3Mutex2;
	NetworkDump_Malloc( m_pMutex, sizeof( i3Mutex2), __FILE__, __LINE__);
}

i3FixedMemoryPool::~i3FixedMemoryPool(void)
{
	// 할당되었던 모든 Page들을 해제한다.
	m_pMutex->Enter();
	{
		PAGE * pTempPage = m_pPage;
		PAGE * pNext;

		while( pTempPage != nullptr)
		{
			pNext = pTempPage->m_pNext;

			i3MemFree( pTempPage);

			pTempPage = pNext;
		}
		m_pPage = nullptr;
	}
	m_pMutex->Leave();

	I3_SAFE_DELETE( m_pMutex);

	#if defined( I3_DEBUG)
	{
		// Memory Pool List에서 제거한다.
		if (s_pPoolMutex)
			s_pPoolMutex->Enter();
		{
			i3FixedMemoryPool * pTemp = s_pPoolHead;

			while((pTemp != nullptr) && (pTemp->m_pNext != this))
				pTemp = pTemp->m_pNext;

			if( pTemp != nullptr)
				pTemp->m_pNext = m_pNext;

			if( s_pPoolHead == this)
				s_pPoolHead = m_pNext;
		}
		if (s_pPoolMutex)
			s_pPoolMutex->Leave();
	}
	#endif
}

i3FixedMemoryPool::PAGE *	i3FixedMemoryPool::NewPage( const char * pszFile, INT32 line)
{
	PAGE * pNew;

	pNew = (PAGE *) i3mem::Alloc( m_PageUnit, 0, m_Align, pszFile, line);//i3MemAlign( m_PageUnit, m_Align, pszFile, line);

	pNew->m_pNext = nullptr;

	InitPage( pNew);

	return pNew;
}

void	i3FixedMemoryPool::InitPage( PAGE * pPage)
{
	UINT32 i;
	BLOCK * pElm;

	pPage->m_UsedCount = 0;

	pElm = (BLOCK *)(((char *) pPage) + m_AlignedPageTagSize);

	for( i = 0; i < m_CountPerPage; i++)
	{
		pElm->m_bUsed = false;
		pElm->m_pNextFree = m_pFree;			// Free List에 추가

		m_pFree = pElm;							// 새로운 Free Element로 등록.

		pElm = (BLOCK *) (((char *) pElm) + (m_AlignedElementTagSize + m_AlignedElementSize));
	}
}

void	i3FixedMemoryPool::RemovePage( PAGE * pPage)
{
	PAGE * pPrev;

	I3ASSERT( pPage != nullptr);
	I3ASSERT( pPage->m_UsedCount == 0);

	// 해당 Page에 존재하는 모든 Free Block을 제거한다.
	{
		char * pPageStart, * pPageEnd;
		BLOCK * pTemp, * pNext, * pPrev;

		pPageStart = (char *) pPage;
		pPageEnd = (char *) pPage + m_PageUnit;
		pPrev = nullptr;

		for( pTemp = m_pFree; pTemp != nullptr;)
		{
			pNext = pTemp->m_pNextFree;

			if( (((char *) pTemp) >= pPageStart) && (((char *) pTemp) <= pPageEnd))
			{
				// 해당 Page 영역 안에 존재하는 Element이다.
				if( pPrev != nullptr)
					pPrev->m_pNextFree = pNext;

				if( m_pFree == pTemp)
					m_pFree = pNext;
			}
			else
			{
				pPrev = pTemp;
			}

			pTemp = pNext;
		}
	}

	// 앞 Page를 찾는다.
	pPrev = m_pPage;
	while((pPrev != nullptr) && (pPrev->m_pNext != pPage))
		pPrev = pPrev->m_pNext;

	if( pPrev != nullptr)
		pPrev->m_pNext = pPage->m_pNext;

	if( m_pPage == pPage)
		m_pPage = pPage->m_pNext;

	i3MemFree( pPage);
}

bool i3FixedMemoryPool::Create( const char * pszName, UINT32 UnitSz, UINT32 PageUnit, UINT32 Align, bool bAutoExtend, bool bAutoCompact)
{
	I3ASSERT( UnitSz > 0);
	I3ASSERT( Align > 0);
	I3ASSERT( PageUnit > Align);

	#if defined( I3_DEBUG)
	m_Name = pszName;
	#endif

	m_UnitSize = UnitSz;

	m_Align = Align;
	m_PageUnit = PageUnit;

	m_bAutoExtend = bAutoExtend;
	m_bAutoCompact = bAutoCompact;

	m_AlignedPageTagSize	= ((sizeof(PAGE) + Align - 1) / Align) * Align;
	m_AlignedElementTagSize	= ((sizeof(BLOCK) + Align - 1) / Align) * Align;
	m_AlignedElementSize	= ((UnitSz + Align - 1) / Align) * Align;

	m_CountPerPage = (PageUnit - m_AlignedPageTagSize) / (m_AlignedElementTagSize + m_AlignedElementSize);

	m_pPage = nullptr;

	return true;
}

void i3FixedMemoryPool::Reset(void)
{
	m_pMutex->Enter();
	{
		PAGE * pTemp = m_pPage;

		m_pFree = nullptr;

		while( pTemp != nullptr)
		{
			InitPage( pTemp);

			pTemp = pTemp->m_pNext;
		}
	}
	m_pMutex->Leave();
}

void	i3FixedMemoryPool::Compact(void)
{
	m_pMutex->Enter();
	{
		PAGE * pTemp = m_pPage;
		PAGE * pNext;

		while( pTemp != nullptr)
		{
			pNext = pTemp->m_pNext;

			if( pTemp->m_UsedCount == 0)
			{
				RemovePage( pTemp);
			}

			pTemp = pNext;
		}
	}
	m_pMutex->Leave();
}

i3FixedMemoryPool::PAGE *	i3FixedMemoryPool::FindPage( char * p)
{
	m_pMutex->Enter();
	{
		PAGE * pTemp = m_pPage;

		while( pTemp != nullptr)
		{
			if( (p >= (char *)pTemp) && (p <= (((char *)pTemp) + m_PageUnit)))
			{
				m_pMutex->Leave();
				return pTemp;
			}

			pTemp = pTemp->m_pNext;
		}
	}
	m_pMutex->Leave();

	return nullptr;
}

void * i3FixedMemoryPool::Alloc( UINT32 Sz, const char * pszFile, INT32 line)
{
	char * pBlock;

	m_pMutex->Enter();

	if(m_pFree == nullptr)
	{
		if( m_bAutoExtend)
		{
			// 추가로 Page를 늘린다.
			PAGE * pNewPage = NewPage( pszFile, line);

			pNewPage->m_pNext = m_pPage;

			m_pPage = pNewPage;
		}
		else
		{
			m_pMutex->Leave();
			return nullptr;
		}

		I3ASSERT_RETURN(m_pFree != nullptr, nullptr);
	}
	
	pBlock = (((char *) m_pFree) + m_AlignedElementTagSize);

	{
		PAGE * pUsedPage = FindPage( (char *) m_pFree);

		I3ASSERT( pUsedPage != nullptr);

		pUsedPage->m_UsedCount++;
	}

	m_pFree->m_bUsed = true;
	m_pFree = m_pFree->m_pNextFree;			// Free Head를 이동한다.

	m_pMutex->Leave();

	return pBlock;
}

void i3FixedMemoryPool::Free( void * pObj)
{
	PAGE * pPage;
	BLOCK * pBlock;

	m_pMutex->Enter();

	pBlock = (BLOCK *) (((char *) pObj) - m_AlignedElementTagSize);	
	pBlock->m_bUsed = false;

	// Free List에 추가.
	pBlock->m_pNextFree = m_pFree;
	m_pFree = pBlock;

	// 해당 Page의 설정
	pPage = FindPage( (char *) pBlock);
	I3ASSERT( pPage != nullptr);

	pPage->m_UsedCount--;

	if( m_bAutoCompact)
	{
		if( pPage->m_UsedCount <= 0)
		{
			RemovePage( pPage);
		}
	}

	m_pMutex->Leave();
}

INT32 i3FixedMemoryPool::GetPageCount(void)
{
	m_pMutex->Enter();

	PAGE * pPage = m_pPage;
	INT32 cnt = 0;

	while( pPage != nullptr)
	{
		cnt++;
		pPage = pPage->m_pNext;
	}

	m_pMutex->Leave();

	return cnt;
}

UINT32	i3FixedMemoryPool::GetTotalBlockCount(void)
{
	return m_CountPerPage * m_UnitSize * GetPageCount();
}

UINT32	i3FixedMemoryPool::GetAllocatedBlockCount(void)
{
	m_pMutex->Enter();
	PAGE * pPage = m_pPage;
	INT32 cnt = 0;

	while( pPage != nullptr)
	{
		cnt += pPage->m_UsedCount;
		pPage = pPage->m_pNext;
	}
	m_pMutex->Leave();

	return cnt;
}

#if defined( I3_DEBUG)

void i3FixedMemoryPool::DumpUsage(void)
{
	INT32 TotalSize, AllocCount, UsedSize, PageCount;
	REAL32 Percent;
	PAGE * pTempPage;

	TotalSize = 0;
	AllocCount = 0;
	UsedSize = 0;
	PageCount = 0;
	Percent = 0.0f;

	m_pMutex->Enter();

	pTempPage = m_pPage;
	while( pTempPage != nullptr)
	{
		TotalSize += m_PageUnit;
		AllocCount += pTempPage->m_UsedCount;
		UsedSize += (pTempPage->m_UsedCount * m_AlignedElementSize);
		PageCount++;

		pTempPage = pTempPage->m_pNext;
	}
	m_pMutex->Leave();

	if( TotalSize > 0)
	{
		Percent = (REAL32) UsedSize / TotalSize * 100.0f;
	}

	I3TRACE( "%s\t%d\t%d\t%d\t%.1f%%\n",
			m_Name.c_str(), TotalSize, AllocCount, UsedSize, Percent);
}

void i3FixedMemoryPool::Dump(void)
{
	s_pPoolMutex->Enter();

	{
		i3FixedMemoryPool * pTemp = s_pPoolHead;

		I3TRACE( "---------------------- i3FixedMemoryPool usage state ---------------------- \n");
		I3TRACE( "Name\tTotal_allocated_size\tTotal_allocated_count\tUsed_size\tPercent\n");

		while( pTemp != nullptr)
		{
			pTemp->DumpUsage();

			pTemp = pTemp->m_pNext;
		}
	}
	s_pPoolMutex->Leave();
}

void i3FixedMemoryPool::Enum( DUMPCALLBACK pUserProc, void * pUserData)
{
	s_pPoolMutex->Enter();

	i3FixedMemoryPool * pTemp = s_pPoolHead;

	while( pTemp != nullptr)
	{
		if( pUserProc( pTemp, pUserData) == false)
			break;

		pTemp = pTemp->m_pNext;
	}

	s_pPoolMutex->Leave();
}

#endif
