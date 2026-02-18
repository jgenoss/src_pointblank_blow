#if !defined( __I3_OBJECT_POOL_H)
#define __I3_OBJECT_POOL_H

#include "i3ElementBase.h"


#define I3_OBJECT_POOL_STATE_USED		0x00000001

struct I3_OBJECT_POOL_TABLE_INFO
{
	struct I3_OBJECT_POOL_TABLE_INFO *	m_pNext = nullptr;
	UINT32							m_State = 0;
	i3ElementBase *					m_pObj = nullptr;
} ;

class I3_EXPORT_BASE i3ObjectPool : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3ObjectPool, i3ElementBase);
protected:
	i3ClassMeta *				m_pMeta = nullptr;
	I3_OBJECT_POOL_TABLE_INFO *	m_pTable = nullptr;
	UINT32						m_Count = 0;
	UINT32						m_Unit = 0;
	I3_OBJECT_POOL_TABLE_INFO *	m_pFreeHead = nullptr;

protected:
	void	Resize( UINT32 newCount);
	void	AddToFree( UINT32 idx, UINT32 count);

public:
	virtual ~i3ObjectPool(void);

	void	Create( i3ClassMeta * pMeta, UINT32	initCount = 0, UINT32 unit = 16);

	void	Reset(void);

	i3ElementBase *	Alloc(void)
	{
		I3_OBJECT_POOL_TABLE_INFO * pNew;

		if( m_pFreeHead == nullptr)
		{
			Resize( m_Count + m_Unit);
		}

		I3ASSERT_RETURN( m_pFreeHead != nullptr, nullptr);

		pNew = m_pFreeHead;

		pNew->m_State |= I3_OBJECT_POOL_STATE_USED;

		m_pFreeHead = pNew->m_pNext;

		return pNew->m_pObj;
	}

	INT32			GetIndex( i3ElementBase * pObj)
	{
		UINT32 i;
		
		for( i = 0; i < m_Count; i++)
		{
			if( m_pTable[i].m_pObj == pObj)
				return i;
		}

		return -1;
	}

	void			Free( i3ElementBase * pObj)
	{
		I3_OBJECT_POOL_TABLE_INFO * pInfo;
		INT32 idx;

		idx = GetIndex( pObj);
		
		pInfo = m_pTable + idx;

		pInfo->m_pNext = m_pFreeHead;
		pInfo->m_State &= ~I3_OBJECT_POOL_STATE_USED;

		m_pFreeHead = pInfo;
	}
};

#endif






