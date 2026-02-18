#include "i3GfxType.h"
#include "i3IndexArrayMem.h"


I3_CLASS_INSTANCE( i3IndexArrayMem);


i3IndexArrayMem::~i3IndexArrayMem(void)
{
	I3MEM_SAFE_FREE( m_pIdxBuffer);
}

bool i3IndexArrayMem::Create( UINT32 count, I3G_USAGE usage, UINT32 vaCount)
{
	UINT32 unitSz;

	m_Count = count;
	SetUsageFlag( usage);

	if( vaCount > 0xFFFF)
	{
		m_DataType = I3G_IDX_32BITS;
	}
	else
	{
		m_DataType = I3G_IDX_16BITS;
	}

	if( m_DataType == I3G_IDX_16BITS)
		unitSz = sizeof(UINT16);
	else
		unitSz = sizeof( UINT32);

	m_pIdxBuffer = (char *) i3MemAlloc( count * unitSz);
	I3ASSERT( m_pIdxBuffer != nullptr);

	return true;
}

bool i3IndexArrayMem::Create( UINT32 count, I3G_USAGE usage, I3G_IDX_TYPE type)
{
	UINT32 unitSz;

	m_Count = count;
	SetUsageFlag( usage);

	m_DataType = type;

	if( m_DataType == I3G_IDX_16BITS)
		unitSz = sizeof(UINT16);
	else
		unitSz = sizeof( UINT32);

	m_pIdxBuffer = (char *) i3MemAlloc( count * unitSz);
	I3ASSERT( m_pIdxBuffer != nullptr);

	return true;
}

bool i3IndexArrayMem::Lock( UINT32 StartIdx, UINT32 Count, I3G_LOCKMETHOD lock)
{
	if( (m_State & I3G_INDEXARRAY_STATE_LOCKED) == I3G_INDEXARRAY_STATE_LOCKED)
		return false;

	if( Count == 0)
		Count = m_Count;

	m_LockCount = Count;

	m_LockStartIdx = StartIdx;

	I3ASSERT( m_pLockedData == nullptr);

	m_pLockedData = m_pIdxBuffer;
	m_State |= I3G_INDEXARRAY_STATE_LOCKED;

	return true;
}

void i3IndexArrayMem::Unlock(void)
{
	m_pLockedData = nullptr;
	m_State &= ~I3G_INDEXARRAY_STATE_LOCKED;
	m_LockCount = 0;
}

bool i3IndexArrayMem::SafeLock( UINT32 StartIdx /*= 0*/, UINT32 Count /*= 0*/, I3G_LOCKMETHOD lock /*= 0*/)
{
	m_Mutex.Enter();

	return Lock( StartIdx, Count, lock);
}

bool i3IndexArrayMem::SafeUnlock(void)
{
	Unlock();
	m_Mutex.Leave();
	return true;
}

