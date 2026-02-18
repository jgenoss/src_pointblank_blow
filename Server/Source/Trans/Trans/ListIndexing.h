#ifndef __LISTINDEXING_H__
#define __LISTINDEXING_H__

class CListIndexing
{
	UINT32				m_ui32IdxCount;
	UINT32				m_ui32CreateIdx;
	UINT32				m_ui32DeleteIdx;
	UINT32*				m_pui32Indexing;

protected:
	void				Create( UINT32 ui32Count );
	UINT32				CreateIdx();
	void				DeleteIdx( UINT32 ui32Idx );

	UINT32				GetIdxCount()						{	return m_ui32IdxCount;	};

public:
	CListIndexing();
	virtual ~CListIndexing();
};

#endif