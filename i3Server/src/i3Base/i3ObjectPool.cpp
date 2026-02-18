#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3ObjectPool.h"
#include "i3Memory.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE( i3ObjectPool, i3ElementBase);

i3ObjectPool::i3ObjectPool(void)
{
	m_pMeta		= NULL;
	m_pTable	= NULL;
//	m_TableIdx	= -1; 
	m_Count		= 0;
	m_Unit		= 0;
	m_pFreeHead = NULL;
}

i3ObjectPool::~i3ObjectPool(void)
{
	UINT32 i;

	for( i = 0; i < m_Count; i++)
	{
		m_pTable[i].m_pObj->Release();
	}

	if( m_pTable != NULL)
	{
		//i3MemFreeByHandle( m_TableIdx );
		//m_pTable = NULL;
		I3MEM_SAFE_FREE_POINTER(m_pTable); 
	}
}

BOOL i3ObjectPool::Create( i3ClassMeta * pMeta, UINT32 initCount, UINT32 unit)
{
	I3ASSERT( unit > 0);

	m_Count = initCount;
	m_Unit = unit;
	m_pMeta = pMeta;

	if( m_Count > 0)
		Resize( m_Count);

	Reset();

	return TRUE;
}

void i3ObjectPool::Resize( UINT32 count)
{
	UINT32 Sz;
	INT32 start, end, i;
	i3ElementBase * pObj;

	I3ASSERT( m_pMeta != NULL);
	I3ASSERT( m_Unit > 0);
	I3ASSERT( count > 0);

	Sz = (sizeof(I3_OBJECT_POOL_TABLE_INFO) * count);

	if( m_pTable != NULL)
	{
		if( m_Count > count)
		{
			for( i = count; i < (INT32)m_Count; i++)
			{
				m_pTable[i].m_pObj->Release();
			}
		}

		//m_TableIdx = i3MemReAlignByHandle(m_TableIdx, Sz, 4);		
		m_pTable = (I3_OBJECT_POOL_TABLE_INFO *)i3MemReAlign(m_pTable, Sz, 4); 
	#ifdef I3_DEBUG
		I3TRACE("WARNING : i3ObjectPool 에서 Memory ReAlloc이 발생!\n");
	#endif


		start = m_Count;
		end = count - 1;

	}
	else
	{
		//m_TableIdx = i3MemAlign( Sz, 4);
		m_pTable = (I3_OBJECT_POOL_TABLE_INFO *)i3MemAlign( Sz, 4);
		start = 0;
		end = count - 1;
	}

	//m_pTable = (I3_OBJECT_POOL_TABLE_INFO *) i3MemLock( m_TableIdx );

	for( i = start; i <= end; i++)
	{
		pObj = CREATEINSTANCE( m_pMeta);
		pObj->AddRef();

		m_pTable[i].m_pObj = pObj;
	}

	if( count > m_Count)
	{
		AddToFree( m_Count, count - m_Count);
	}

	m_Count = count;
}

void i3ObjectPool::AddToFree( UINT32 idx, UINT32 count)
{
	I3_OBJECT_POOL_TABLE_INFO * pInfo;
	UINT32 i;

	pInfo = m_pTable + idx;

	for( i = 0; i < count; i++)
	{
		pInfo->m_pNext = m_pFreeHead;

		m_pFreeHead = pInfo;

		pInfo++;
	}
}

void i3ObjectPool::Reset(void)
{
	m_pFreeHead = NULL;
	AddToFree( 0, m_Count);
}

