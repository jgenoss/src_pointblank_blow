#include "i3CommonType.h"
#include "i3MemoryPool.h"
#include "i3Memory.h"

i3MemoryPool::i3MemoryPool(void)
{
}

i3MemoryPool::~i3MemoryPool(void)
{
}

#if defined( I3_NO_STATIC_CLASSMETA)

#if defined( I3_WIPI_GIGA)
void *	i3MemoryPool::operator new( UINT32 sz )
#else
void *	i3MemoryPool::operator new( size_t sz )
#endif
{
	i3MemoryPool * pNew;
	INT32 hMem;

	hMem = i3MemAlloc( (UINT32) sz);

	pNew = (i3MemoryPool *) i3MemLock( hMem);

	return pNew;
}

void i3MemoryPool::operator delete( void * p)
{
	i3MemFree( p);
}
#endif
