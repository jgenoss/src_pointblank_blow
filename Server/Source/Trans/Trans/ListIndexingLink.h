#ifndef __LISTINDEXING_LINK_H__
#define __LISTINDEXING_LINK_H__

#include "ListIndexing.h"

class CListMainStreamBuffer : public CListIndexing
{
private	: 
	MAIN_STREAM_BUFFER * m_pBuffer;
public	:
	CListMainStreamBuffer();
	virtual ~CListMainStreamBuffer();

	void	CreateBuffer( UINT32 ui32Count);		
	UINT32	GetBlankIdx();
	void	SetBlankBuffer( UINT32 ui32Idx );
	MAIN_STREAM_BUFFER * GetBuffer( UINT32 ui32Idx );
};

#endif