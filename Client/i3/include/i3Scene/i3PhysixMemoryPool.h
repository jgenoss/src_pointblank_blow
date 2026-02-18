#if !defined( __I3_PHYSIX_MEM_POOL_H)
#define __I3_PHYSIX_MEM_POOL_H

// 이 메모리 풀은 2012.7~8월 중 폐기 예정...(2012.05.23.수빈)
#include "i3Base/itl/vector_map.h"

struct I3_PHYSIX_MEMINFO
{
	UINT32		m_Size = 0;
} ;

class I3_EXPORT_SCENE i3PhysixMemoryPool : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PhysixMemoryPool, i3ElementBase);
protected:

	i3::vector_multimap<UINT32, I3_PHYSIX_MEMINFO*>		m_FreeList;				// 사이즈는 여러개가 허용되며, 내부에서 2진검색됨..
	i3::unordered_set<I3_PHYSIX_MEMINFO*>				m_AllocList;			// 포인터는 유일성 보장되서 이렇게 처리.

	UINT32			m_Size = 0;
	UINT32			m_TotalAllocReq = 0;
	UINT32			m_HitAllocReq = 0;

public:
	i3PhysixMemoryPool(void);

	INT32		getAllocCount(void)					{ return (INT32)m_AllocList.size(); }
	INT32		getFreeCount(void)					{ return (INT32)m_FreeList.size(); }
	INT32		getTotalCount(void)					{ return (INT32)(m_AllocList.size() + m_FreeList.size()); }

	UINT32		getTotalSize(void)					{ return m_Size; }

	UINT32		getTotalAllocRequest(void)			{ return m_TotalAllocReq; }
	UINT32		getHitAllocRequest(void)			{ return m_HitAllocReq; }

	void *		Alloc( UINT32 size);
	void *		Realloc( void * p, UINT32 size);
	bool		Free( void * pBuff); 
	bool		IsAlloced( char * p);
	void		Contact(void);
	void		ReleaseAllocList(void);
};

#endif
