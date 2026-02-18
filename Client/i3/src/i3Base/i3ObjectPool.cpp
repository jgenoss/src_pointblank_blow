#include "i3CommonType.h"
#include "i3ErrorHandler.h"
#include "i3ObjectPool.h"
#include "i3Memory.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE( i3ObjectPool);

i3ObjectPool::~i3ObjectPool(void)
{
	for(UINT32 i = 0; i < m_Count; i++)
	{
		I3_SAFE_RELEASE( m_pTable[i].m_pObj);
	}

	I3MEM_SAFE_FREE_POINTER(m_pTable);
}

void i3ObjectPool::Create( i3ClassMeta * pMeta, UINT32 initCount, UINT32 unit)
{
	I3ASSERT( unit > 0);

	m_Count = initCount;
	m_Unit = unit;
	m_pMeta = pMeta;

	if( m_Count > 0)
		Resize( m_Count);

	Reset();
}

void i3ObjectPool::Resize( UINT32 count)
{
	UINT32 Sz;
	INT32 start, end, i;
	i3ElementBase * pObj;

	I3ASSERT( m_pMeta != nullptr);
	I3ASSERT( m_Unit > 0);
	I3ASSERT( count > 0);

	Sz = (sizeof(I3_OBJECT_POOL_TABLE_INFO) * count);

	if( m_pTable != nullptr)
	{
		if( m_Count > count)
		{
			for( i = count; i < (INT32)m_Count; i++)
			{
				I3_SAFE_RELEASE( m_pTable[i].m_pObj);
			}
		}

		m_pTable = (I3_OBJECT_POOL_TABLE_INFO *)i3MemReAlign(m_pTable, Sz, 4); 
	#ifdef I3_DEBUG
		I3TRACE("WARNING : i3ObjectPool 에서 Memory ReAlloc이 발생!\n");
	#endif


		start = m_Count;
		end = count - 1;

	}
	else
	{
		m_pTable = (I3_OBJECT_POOL_TABLE_INFO *)i3MemAlign( Sz, 4);
		start = 0;
		end = count - 1;
	}

	for( i = start; i <= end; i++)
	{
		pObj = m_pMeta->create_instance();
		I3_MUST_ADDREF( pObj);
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
	m_pFreeHead = nullptr;
	AddToFree( 0, m_Count);
}

