#include "i3CommonType.h"
#include "i3StackMemoryPool.h"
#include "i3Macro.h"
#include "i3ElementBase.h"
#include "i3String.h"
#include "i3MetaTemplate.h"

I3_CLASS_INSTANCE( i3StackMemoryPool, i3ElementBase);

#define		I3SMEM_ALLOCED		0x80000000

typedef struct _tagStackMemInfo
{
	UINT32 m_Size;
	UINT32 m_State;
} I3_STACK_MEM_INFO;

i3StackMemoryPool::i3StackMemoryPool(void)
{
	m_pBuff = NULL;
	m_pTail = NULL;
	m_MaxSize = 0;
	m_AllocedSize = 0;
}

i3StackMemoryPool::~i3StackMemoryPool(void)
{
	I3MEM_SAFE_FREE( m_pBuff);
}

bool i3StackMemoryPool::Create( UINT32 maxSize)
{
	m_MaxSize = maxSize;

	I3MEM_SAFE_FREE( m_pBuff);

	m_pBuff = (char *) i3MemAlloc( maxSize);
	I3ASSERT( m_pBuff != NULL);

	m_pTail = m_pBuff;
	m_AllocedSize = 0;

	return true;
}

void *	i3StackMemoryPool::Alloc( UINT32 sz)
{
	UINT32 actualSize;

	actualSize = sz + sizeof(I3_STACK_MEM_INFO);

#if defined( I3_DEBUG)
	if( (m_pTail + actualSize) > (m_pBuff + m_MaxSize))
	{
		Dump();
		I3FATAL( "Memory Overflow!!! : Maximum %d bytes\n", m_MaxSize);
		return NULL;
	}
#endif

	I3_STACK_MEM_INFO * pMem = (I3_STACK_MEM_INFO *) m_pTail;

	pMem->m_Size = sz;
	pMem->m_State = I3SMEM_ALLOCED;

	m_pTail += actualSize;
	m_AllocedSize += actualSize;

	return pMem + 1;		// 관리용 Memory 부분을 뺀 만큼을 return
}

void	i3StackMemoryPool::Free( void * pBuff)
{
	I3_STACK_MEM_INFO * pMem = (I3_STACK_MEM_INFO *) pBuff;

	pMem--;		// <- 관리용 Header의 위치 검색

	I3ASSERT( pMem->m_State & I3SMEM_ALLOCED);

	m_AllocedSize -= pMem->m_Size + sizeof(I3_STACK_MEM_INFO);
	pMem->m_Size = 0;
	pMem->m_State = 0;

	if( (((char *) pMem) + pMem->m_Size + sizeof(I3_STACK_MEM_INFO)) == m_pTail)
	{
		// 가장 마지막의 것이 Free된다.
		// Tail을 앞으로 당긴다.
		m_pTail = (char *) pMem;
	}

	if( m_AllocedSize == 0)
	{
		Reset();
	}
}

void i3StackMemoryPool::Reset(void)
{
	m_pTail = m_pBuff;
	m_AllocedSize = 0;
}

void	i3StackMemoryPool::Dump(void)
{
	char szMax[64], szAlloc[64], szUsed[64];

	i3String::ValueToStr( m_MaxSize, szMax, sizeof(szMax));
	i3String::ValueToStr( m_AllocedSize, szAlloc, sizeof(szAlloc));
	i3String::ValueToStr( m_pTail - m_pBuff, szUsed, sizeof(szUsed));

	I3TRACE( "---------------------------- Stack Memory Pool dump -----------------------\n");
	I3TRACE( "Max Size     : %s bytes\n", szMax);
	I3TRACE( "Alloced Size : %s bytes\n", szAlloc);
	I3TRACE( "Used Size    : %s bytes\n", szUsed);
	I3TRACE( "---------------------------------------------------------------------------\n");
}