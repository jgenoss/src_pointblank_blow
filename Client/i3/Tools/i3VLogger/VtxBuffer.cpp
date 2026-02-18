#include "stdafx.h"
#include "VtxBuffer.h"

I3_CLASS_INSTANCE( VtxBuffer); //, i3ElementBase);

#define		ALLOC_UNIT			65536

VtxBuffer::VtxBuffer(void)
{
	m_pBuff = NULL;
	m_MaxCount = 0;
	m_Count = 0;
}

VtxBuffer::~VtxBuffer(void)
{
	I3MEM_SAFE_FREE( m_pBuff);
}

_VTX *	VtxBuffer::alloc( INT32 cnt)
{
	if( (m_Count + cnt) > m_MaxCount)
	{
		m_MaxCount = ((m_Count + cnt) + (ALLOC_UNIT - 1)) / ALLOC_UNIT;
		m_MaxCount *= ALLOC_UNIT;

		// 추가 할당
		if( m_pBuff == NULL)
			m_pBuff = (_VTX *) i3MemAlloc( sizeof(_VTX) * m_MaxCount);
		else
			m_pBuff = (_VTX *) i3MemReAlloc( m_pBuff, sizeof(_VTX) * m_MaxCount);
	}

	INT32 oldCnt = m_Count;

	m_Count += cnt;

	return & m_pBuff[ oldCnt];
}
	
void	VtxBuffer::reset(void)
{
	m_Count = 0;
}
