#if !defined( __I3_ELEMENT_POOL_H)
#define __I3_ELEMENT_POOL_H

#include "i3MemoryPool.h"
#include "i3Mutex.h"

class I3_EXPORT_BASE i3FixedMemoryPool : public i3MemoryPool
{
//	I3_CLASS_DEFINE( i3FixedMemoryPool, i3MemoryPool);

public:
	struct PAGE
	{
		UINT32					m_UsedCount;
		struct PAGE *			m_pNext;
	};

	struct BLOCK
	{
		bool					m_bUsed;
		struct BLOCK *			m_pNextFree;
	};

	typedef bool (*DUMPCALLBACK)( i3FixedMemoryPool * pPool, void * pUserData);

protected:
#if defined( I3_DEBUG)
	i3::rc_string			m_Name;
	i3FixedMemoryPool *		m_pNext;
#endif

	i3Mutex2 *				m_pMutex;

	PAGE	*				m_pPage = nullptr;
	BLOCK *					m_pFree = nullptr;
	
	UINT32					m_UnitSize = 0;
	UINT32					m_PageUnit = 0;
	UINT32					m_Align = 0;
	UINT32					m_CountPerPage = 0;
	UINT32					m_AlignedPageTagSize = 0;
	UINT32					m_AlignedElementTagSize = 0;
	UINT32					m_AlignedElementSize = 0;

	bool					m_bAutoExtend = true;
	bool					m_bAutoCompact = true;

protected:
	PAGE *				NewPage( const char * pszFile, INT32 line);
	void				InitPage( PAGE * pPage);
	void				RemovePage( PAGE * pPage);
	PAGE *				FindPage( char * p);

public:
	i3FixedMemoryPool(void) NOTHROW;
	virtual ~i3FixedMemoryPool(void) NOTHROW;

	bool	Create( const char * pszName, UINT32 UnitSize, UINT32 PageUnit = 4096, UINT32 Align = 4, bool bAutoExtend = true, bool bAutoCompact = true);
#if defined( I3_DEBUG)
	const char *	getName(void)	const				{ return m_Name.c_str(); }
#endif
	bool	GetAutoCompactEnable(void)				{ return m_bAutoCompact; }
	void	SetAutoCompactEnable( bool bFlag)		{ m_bAutoCompact = bFlag; }

	UINT32	getPageSize(void)						{ return m_PageUnit; }
	INT32	GetPageCount(void);

	UINT32	getUnitSize(void)						{ return m_UnitSize; }

	UINT32	GetTotalBlockCount(void);
	UINT32	GetAllocatedBlockCount(void);

	void	Reset(void);
	void	Compact(void);

	virtual	void *		Alloc( UINT32 Sz, const char * pszFile, INT32 line);
	virtual	void		Free( void * p);

#if defined( I3_DEBUG)
	void				DumpUsage(void);
	static	void		Dump(void);
	static void			Enum( DUMPCALLBACK pUserProc, void * pUserData);
#endif
};

#endif
