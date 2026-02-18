#if !defined( __I3_LRU_QUEUE_H)
#define __I3_LRU_QUEUE_H

#include "i3ElementBase.h"
#include "i3Base/itl/vector_set.h"

class i3FixedMemoryPool;

struct	I3_LRU_ENTRY
{
	void *			m_pData = nullptr;
	UINT32			m_LastUsed = 0;
	I3_LRU_ENTRY *	m_pPrev = nullptr;
	I3_LRU_ENTRY *	m_pNext = nullptr;
};

struct I3_LRU_SORT
{
	void *			m_pData = nullptr;
	I3_LRU_ENTRY *	m_pEntity = nullptr;
};

bool	operator<(const I3_LRU_SORT& a1, const I3_LRU_SORT& a2);
template<> struct i3::less<I3_LRU_SORT*> : i3::less_ptr<I3_LRU_SORT*> {};


class I3_EXPORT_BASE i3LRUQueue : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3LRUQueue, i3ElementBase);
protected:
	i3::vector_set<I3_LRU_SORT*>	m_DataList;
	I3_LRU_ENTRY *		m_pEntity = nullptr;
	I3_LRU_ENTRY *		m_pUsedHead = nullptr;
	I3_LRU_ENTRY *		m_pUsedTail = nullptr;
	I3_LRU_ENTRY *		m_pFree = nullptr;
	INT32				m_MaxCount = 0;

	i3FixedMemoryPool *	m_pMemPool;				// [initialize at constructor]

protected:
	void		DeleteAllData(void);
	INT32		_FindData( void * pData);
	void		DeleteData( void * pData);

	void		UnlinkFromUsedList( I3_LRU_ENTRY * pEntry);
	void		LinkToUsedList( I3_LRU_ENTRY * pEntry);
	I3_LRU_ENTRY *	AllocEntry( void * pData);
	void			FreeEntry( I3_LRU_ENTRY * pEntry);

public:
	i3LRUQueue(void);
	virtual ~i3LRUQueue(void);

	void		Create( INT32 cnt);

	void		Reset(void);

	bool		IsThereRoom(void)			{ return m_pFree != nullptr; }
	void		PushUsed( void * pData, UINT32 tm);
	void *		PopOldest(void);

	void *		FindData( void * pData);
	void		Remove( void * pData);

	INT32		GetUsedData( void ** pArray, INT32 maxCount);

//	void		SetCompareProc( COMPAREPROC pUserProc);

	void		DumpUsed(void);
	void		VerifyUsed(void);
};


#endif
