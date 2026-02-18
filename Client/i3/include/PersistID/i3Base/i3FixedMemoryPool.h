#if !defined( __I3_ELEMENT_POOL_H)
#define __I3_ELEMENT_POOL_H

#include "i3MemoryPool.h"

class I3_EXPORT_BASE i3FixedMemoryPool : public i3MemoryPool
{
	//I3_CLASS_DEFINE( i3FixedMemoryPool);

public:
	typedef struct _tagPageHeader
	{
		UINT32					m_UsedCount;
		struct _tagPageHeader *	m_pNext;
	} PAGE;

	typedef struct _tagBlockHeader
	{
		BOOL						m_bUsed;
		struct _tagBlockHeader *	m_pNextFree;
	} BLOCK;

protected:
#if defined( I3_DEBUG)
	char					m_szName[32];

	i3FixedMemoryPool *		m_pNext;
#endif

	PAGE	*				m_pPage;
	BLOCK *					m_pFree;
	
	UINT32					m_UnitSize;
	UINT32					m_PageUnit;
	UINT32					m_Align;
	UINT32					m_CountPerPage;
	UINT32					m_AlignedPageTagSize;
	UINT32					m_AlignedElementTagSize;
	UINT32					m_AlignedElementSize;

	BOOL					m_bAutoExtend;
	BOOL					m_bAutoCompact;

protected:
	PAGE *				NewPage(void);
	void				InitPage( PAGE * pPage);
	void				RemovePage( PAGE * pPage);
	PAGE *				FindPage( char * p);

public:
	i3FixedMemoryPool(void) NOTHROW;
	virtual ~i3FixedMemoryPool(void) NOTHROW;

	BOOL	Create( const char * pszName, UINT32 UnitSize, UINT32 PageUnit = 4096, UINT32 Align = 4, BOOL bAutoExtend = TRUE, BOOL bAutoCompact = TRUE);

	BOOL	GetAutoCompactEnable(void)				{ return m_bAutoCompact; }
	void	SetAutoCompactEnable( BOOL bFlag)		{ m_bAutoCompact = bFlag; }

	void	Reset(void);
	void	Compact(void);

	virtual	void *		Alloc( UINT32 Sz, const char * pszFile, INT32 line);
	virtual	void		Free( void * p);

#if defined( I3_DEBUG)
	void				DumpUsage(void);
	static	void		Dump(void);
#endif
};

#endif
