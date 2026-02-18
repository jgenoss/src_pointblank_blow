#if !defined( __I3_PHYSIX_MEM_POOL_H)
#define __I3_PHYSIX_MEM_POOL_H

typedef struct _tagi3PhysixMemInfo
{
	UINT32		m_Size;
} I3_PHYSIX_MEMINFO;

class I3_EXPORT_SCENE i3PhysixMemoryPool : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysixMemoryPool);
protected:

	i3BinList		m_FreeList;
	i3BinList		m_AllocList;

	UINT32			m_Size;
	UINT32			m_TotalAllocReq;
	UINT32			m_HitAllocReq;

public:
	i3PhysixMemoryPool(void);
	virtual ~i3PhysixMemoryPool(void);

	INT32		getAllocCount(void)					{ return m_AllocList.GetCount(); }
	INT32		getFreeCount(void)					{ return m_FreeList.GetCount(); }
	INT32		getTotalCount(void)					{ return m_AllocList.GetCount() + m_FreeList.GetCount(); }

	UINT32		getTotalSize(void)					{ return m_Size; }

	UINT32		getTotalAllocRequest(void)			{ return m_TotalAllocReq; }
	UINT32		getHitAllocRequest(void)			{ return m_HitAllocReq; }

	void *		Alloc( UINT32 size);
	void *		Realloc( void * p, UINT32 size);
	bool		Free( void * pBuff); 
	bool		IsAlloced( char * p);
	void		Contact(void);
};

#endif
