#include "pch.h"
#include "CircularIndex.h"

I3_CLASS_INSTANCE(CCircularIndex, i3ElementBase);

CCircularIndex::CCircularIndex()
{
	m_pBuffer	= NULL;
}

CCircularIndex::~CCircularIndex()
{
	Destroy();
}

bool CCircularIndex::Create( UINT32 iMaxCount )
{
	//Set Buffer Setting
	m_idxFront	= 0;
	m_idxRear	= 0;
	m_nCount	= 0;
	m_iMaxCount = iMaxCount;

	//Create Buffer 
	m_pBuffer	= (UINT32*)i3MemAlloc(sizeof(UINT32) * m_iMaxCount);
	if( NULL == m_pBuffer )				return false;
	
	//Push All Buffers 
	for(UINT32 i = 0; i < m_iMaxCount; i++)
	{
		if( false == Push(i, FALSE) )	return false;
	}

	return true;
}

void CCircularIndex::Destroy(void)
{
	//Free Buffer 
	I3MEM_SAFE_FREE( m_pBuffer );
}

bool CCircularIndex::Push(UINT32 nValue,BOOL bCheckUnique)
{
	I3ASSERT(nValue < m_iMaxCount);

	// Buffer Full
	if (m_nCount == m_iMaxCount)return false;

	// Check Unique	
	if(bCheckUnique)
	{// 이건좀..그런데....쩝. 
		for(UINT32 i = 0; i < m_nCount; i++)
		{
			INT32 idx = (m_idxFront + i) % m_iMaxCount;
			if (nValue == m_pBuffer[idx])
			{
				return false;
			}
		}
	}

	m_pBuffer[m_idxRear] = nValue;
	m_idxRear = (m_idxRear + 1) % m_iMaxCount;
	m_nCount++;

	return true;
}

BOOL CCircularIndex::Pop(UINT32 * pValue)
{
	I3ASSERT(pValue);

	// Buffer Empty
	if (0 == m_nCount)return FALSE;

	*pValue = m_pBuffer[m_idxFront];
	m_idxFront = (m_idxFront + 1) % m_iMaxCount;
	m_nCount--;

	return TRUE;
}
