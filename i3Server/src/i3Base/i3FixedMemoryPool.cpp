#include "i3CommonType.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3FixedMemoryPool.h"
#include "i3Memory.h"
#include "i3Macro.h"

#if defined( I3_DEBUG)
static i3Mutex2 *			s_pPoolMutex = NULL;
static i3FixedMemoryPool * s_pPoolHead = NULL;

namespace
{
	void cb_atexit_destroy()
	{
		delete s_pPoolMutex;	s_pPoolMutex = NULL;
	}
}
#endif

i3FixedMemoryPool::i3FixedMemoryPool(void)
{
	#if defined( I3_DEBUG)
	if( s_pPoolMutex == NULL)
	{
		s_pPoolMutex = new i3Mutex2;
		::atexit(cb_atexit_destroy);
	}

	m_szName[0] = 0;

	m_pNext = s_pPoolHead;
	s_pPoolHead = this;
	#endif

	m_PageUnit = 0;
	m_Align = 0;

	m_UnitSize = 0;

	m_pPage = NULL;
	m_pFree = NULL;

	m_pMutex = new i3Mutex2;

	m_bAutoExtend = TRUE;
	m_bAutoCompact = TRUE;

	m_AlignedElementSize = 0;
	m_AlignedElementTagSize = 0;
	m_AlignedPageTagSize = 0;
	m_CountPerPage = 0;
}

i3FixedMemoryPool::~i3FixedMemoryPool(void)
{
	// 할당되었던 모든 Page들을 해제한다.
	m_pMutex->Enter();
	{
		PAGE * pTempPage = m_pPage;
		PAGE * pNext;

		while( pTempPage != NULL)
		{
			pNext = pTempPage->m_pNext;

			i3MemFree( pTempPage);

			pTempPage = pNext;
		}
		m_pPage = NULL;
	}
	m_pMutex->Leave();

	delete m_pMutex;
	m_pMutex = NULL;

	#if defined( I3_DEBUG)
	{
		// Memory Pool List에서 제거한다.
		if (s_pPoolMutex)
			s_pPoolMutex->Enter();

		{
			i3FixedMemoryPool * pTemp = s_pPoolHead;

			while((pTemp != NULL) && (pTemp->m_pNext != this))
				pTemp = pTemp->m_pNext;

			if( pTemp != NULL)
				pTemp->m_pNext = m_pNext;

			if( s_pPoolHead == this)
				s_pPoolHead = m_pNext;
		}

		if (s_pPoolMutex)
			s_pPoolMutex->Leave();
	}
	#endif
}

i3FixedMemoryPool::PAGE *	i3FixedMemoryPool::NewPage(void)
{
	PAGE * pNew;

	pNew = (PAGE *) i3MemAlign( m_PageUnit, m_Align);

	pNew->m_pNext = NULL;

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
		pElm->m_bUsed = FALSE;
		pElm->m_pNextFree = m_pFree;			// Free List에 추가

		m_pFree = pElm;							// 새로운 Free Element로 등록.

		pElm = (BLOCK *) (((char *) pElm) + (m_AlignedElementTagSize + m_AlignedElementSize));
	}
}

void	i3FixedMemoryPool::RemovePage( PAGE * pPage)
{
	PAGE * pPrev;

	I3ASSERT( pPage != NULL);
	I3ASSERT( pPage->m_UsedCount == 0);

	// 해당 Page에 존재하는 모든 Free Block을 제거한다.
	{
		char * pPageStart, * pPageEnd;
		BLOCK * pTempB, * pNextB, * pPrevB;

		pPageStart = (char *) pPage;
		pPageEnd = (char *) pPage + m_PageUnit;
		pPrevB = NULL;

		for( pTempB = m_pFree; pTempB != NULL;)
		{
			pNextB = pTempB->m_pNextFree;

			if( (((char *)pTempB ) >= pPageStart) && (((char *)pTempB ) <= pPageEnd))
			{
				// 해당 Page 영역 안에 존재하는 Element이다.
				if( pPrevB != NULL)
					pPrevB->m_pNextFree = pNextB;

				if( m_pFree == pTempB )
					m_pFree = pNextB;
			}
			else
			{
				pPrevB = pTempB;
			}

			pTempB = pNextB;
		}
	}

	// 앞 Page를 찾는다.
	pPrev = m_pPage;
	while((pPrev != NULL) && (pPrev->m_pNext != pPage))
		pPrev = pPrev->m_pNext;

	if( pPrev != NULL)
		pPrev->m_pNext = pPage->m_pNext;

	if( m_pPage == pPage)
		m_pPage = pPage->m_pNext;

	i3MemFree( pPage);
}

BOOL i3FixedMemoryPool::Create( const char * pszName, UINT32 UnitSz, UINT32 PageUnit, UINT32 Align, BOOL bAutoExtend, BOOL bAutoCompact)
{
	I3ASSERT( UnitSz > 0);
	I3ASSERT( Align > 0);
	I3ASSERT( PageUnit > Align);

	#if defined( I3_DEBUG)
	strcpy( m_szName, pszName);
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

	m_pPage = NULL;

	return TRUE;
}

void i3FixedMemoryPool::Reset(void)
{
	m_pMutex->Enter();
	{
		PAGE * pTemp = m_pPage;

		m_pFree = NULL;

		while( pTemp != NULL)
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

		while( pTemp != NULL)
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

		while( pTemp != NULL)
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

	return NULL;
}

void * i3FixedMemoryPool::Alloc( UINT32 /* Sz */, const char * /* pszFile */, INT32 /* line */)
{
	char * pBlock;

	m_pMutex->Enter();

	if(m_pFree == NULL)
	{
		if( m_bAutoExtend)
		{
			// 추가로 Page를 늘린다.
			PAGE * pNewPage = NewPage();

			pNewPage->m_pNext = m_pPage;

			m_pPage = pNewPage;
		}
		else
		{
			m_pMutex->Leave();
			return NULL;
		}
	}

	I3ASSERT( m_pFree != NULL);

	pBlock = (((char *) m_pFree) + m_AlignedElementTagSize);

	{
		PAGE * pUsedPage = FindPage( (char *) m_pFree);

		I3ASSERT( pUsedPage != NULL);

		pUsedPage->m_UsedCount++;
	}

	m_pFree->m_bUsed = TRUE;	
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
	pBlock->m_bUsed = FALSE;

	// Free List에 추가.
	pBlock->m_pNextFree = m_pFree;
	m_pFree = pBlock;

	// 해당 Page의 설정
	pPage = FindPage( (char *) pBlock);
	I3ASSERT( pPage != NULL);

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

	while( pPage != NULL)
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

	while( pPage != NULL)
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
	while( pTempPage != NULL)
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
			m_szName, TotalSize, AllocCount, UsedSize, Percent);
}

void i3FixedMemoryPool::Dump(void)
{
	s_pPoolMutex->Enter();

	{
		i3FixedMemoryPool * pTemp = s_pPoolHead;

		I3TRACE( "---------------------- i3FixedMemoryPool usage state ---------------------- \n");
		I3TRACE( "Name\tTotal_allocated_size\tTotal_allocated_count\tUsed_size\tPercent\n");

		while( pTemp != NULL)
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

	while( pTemp != NULL)
	{
		if( pUserProc( pTemp, pUserData) == false)
			break;

		pTemp = pTemp->m_pNext;
	}

	s_pPoolMutex->Leave();
}

#endif
