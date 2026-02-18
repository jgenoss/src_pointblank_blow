#include "i3GfxType.h"

#if defined( I3G_PSP)
#include "i3IndexArray.h"

i3IndexArray::i3IndexArray(void)
{
	m_pData = NULL;

	m_Count  = 0;
	m_LockStartIdx = 0;
	m_pLockedData = NULL;
}

i3IndexArray::~i3IndexArray(void)
{
	if( m_pData != NULL)
	{
		i3mem::Free( m_pData);
		m_pData = NULL;
	}
}

BOOL i3IndexArray::Create( UINT32 count, I3G_USAGE usage)
{
	UINT32 Sz;

	m_Count = count;
	m_Usage = usage;

	Sz = sizeof(UINT16) * count;

	INT32 idx;
	idx = i3MemAlloc( Sz);
	m_pData = (UINT16 *) i3mem::Lock(idx); 
	I3ASSERT( m_pData != NULL);

	return TRUE;
}
	
void i3IndexArray::Lock( UINT32 StartIdx, UINT32 count, I3G_LOCKMETHOD lock)
{
	m_pLockedData = m_pData;
}

void i3IndexArray::Unlock(void)
{
	m_pLockedData = NULL;
}

void i3IndexArray::SetIndex16( UINT32 idx, UINT16 val)
{
	I3ASSERT( m_pData != NULL);

	m_pData[idx] = val;
}

void i3IndexArray::SetIndices16( UINT32 idx, UINT32 Count, UINT16 * pIndices)
{
	UINT32 i;
	UINT16 * pDest;

	pDest = m_pData + idx;

	for( i = 0; i < Count; i++)
	{
		*pDest = *pIndices;

		pDest++;
		pIndices++;
	}
}

UINT16 i3IndexArray::GetIndex16( UINT32 idx)
{
	I3ASSERT( m_pData != NULL);

	return m_pData[idx];
}


#endif
