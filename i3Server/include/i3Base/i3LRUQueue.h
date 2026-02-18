#if !defined( __I3_LRU_QUEUE_H)
#define __I3_LRU_QUEUE_H

#include "i3ElementBase.h"
#include "i3BinList.h"

struct	I3_LRU_ENTRY
{
	void *			m_pData;
	UINT32			m_LastUsed;
	I3_LRU_ENTRY *	m_pPrev;
	I3_LRU_ENTRY *	m_pNext;
};

struct I3_LRU_SORT
{
	void *			m_pData;
	I3_LRU_ENTRY *	m_pEntity;
};

class I3_EXPORT_BASE i3LRUQueue : public i3ElementBase
{
	I3_CLASS_DEFINE( i3LRUQueue);
protected:
	i3BinList			m_DataList;
	I3_LRU_ENTRY *		m_pEntity;
	I3_LRU_ENTRY *		m_pUsedHead;
	I3_LRU_ENTRY *		m_pUsedTail;
	I3_LRU_ENTRY *		m_pFree;
	INT32				m_MaxCount;

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

	bool		IsThereRoom(void)			{ return m_pFree != NULL; }
	void		PushUsed( void * pData, UINT32 tm);
	void *		PopOldest(void);

	void *		FindData( void * pData);
	void		Remove( void * pData);

	INT32		GetUsedData( void ** pArray, INT32 maxCount);

	void		SetCompareProc( COMPAREPROC pUserProc);

	void		DumpUsed(void);
	void		VerifyUsed(void);
};


#endif
