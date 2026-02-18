#include "pch.h"
#include "CircularIndex.h"

I3_CLASS_INSTANCE(CCircularIndex, i3ElementBase);

CCircularIndex::CCircularIndex()
{
	m_pui32Buffer	= NULL;
	m_i32idxFront	= 0;
	m_i32idxRear	= 0;
	m_ui32Count		= 0;
}

CCircularIndex::~CCircularIndex()
{
	Destroy();
}

BOOL CCircularIndex::Create(UINT32 ui32Size)
{
	I3ASSERT(NULL == m_pui32Buffer);
	I3ASSERT(0 < ui32Size);

	m_pui32Buffer = (UINT32*)i3MemAlloc(sizeof(UINT32) * ui32Size);
	
	if (NULL == m_pui32Buffer)
	{
		return FALSE;
	}

	m_ui32Size = ui32Size;

	return TRUE;
}

void CCircularIndex::Destroy(void)
{
	I3MEM_SAFE_FREE(m_pui32Buffer);
	m_i32idxFront	= 0;
	m_i32idxRear	= 0;
	m_ui32Count		= 0;
}

BOOL CCircularIndex::Push(UINT32 ui32Value,BOOL bCheckUnique)
{
	I3ASSERT(ui32Value < m_ui32Size);

	// Buffer Full
	if (m_ui32Count == m_ui32Size)
	{
		return FALSE;
	}

	// Check Unique
	if (bCheckUnique)
	{
		for(UINT32 i = 0; i < m_ui32Count; i++)
		{
			INT32 ui32idx = (m_i32idxFront + i) % m_ui32Size;
			if (ui32Value == m_pui32Buffer[ui32idx])
			{
				return FALSE;
			}
		}
	}

	m_pui32Buffer[m_i32idxRear] = ui32Value;
	m_i32idxRear = (m_i32idxRear + 1) % m_ui32Size;
	m_ui32Count++;

	return TRUE;
}

BOOL CCircularIndex::Pop(UINT32* pui32Value)
{
	I3ASSERT(pui32Value);

	// Buffer Empty
	if (0 == m_ui32Count)
	{
		return FALSE;
	}

	*pui32Value = m_pui32Buffer[m_i32idxFront];
	m_i32idxFront = (m_i32idxFront + 1) % m_ui32Size;
	m_ui32Count--;

	return TRUE;
}

UINT32 CCircularIndex::Pop(void)
{
	I3ASSERT(0 != m_ui32Count);

	UINT32 ui32RetValue = m_pui32Buffer[m_i32idxFront];
	m_i32idxFront = (m_i32idxFront + 1) % m_ui32Size;
	m_ui32Count--;

	return ui32RetValue;
}

void CCircularIndex::Clear(void)
{
	m_ui32Count		= 0;
	m_i32idxFront	= 0;
	m_i32idxRear	= 0;
}
