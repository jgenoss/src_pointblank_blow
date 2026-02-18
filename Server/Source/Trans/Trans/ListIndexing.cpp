#include "pch.h"
#include "ListIndexing.h"

CListIndexing::CListIndexing()
{
	m_ui32IdxCount		= 0;
	m_ui32CreateIdx		= 0;
	m_ui32DeleteIdx		= 0;
	m_pui32Indexing		= NULL;
}

CListIndexing::~CListIndexing()
{
	I3MEM_SAFE_FREE( m_pui32Indexing );
}

void	CListIndexing::Create( UINT32 ui32Count )
{
	m_ui32IdxCount	= ui32Count;
	m_ui32CreateIdx = 0;
	m_ui32DeleteIdx	= 0;
	m_pui32Indexing = (UINT32*)i3MemAlloc( sizeof(UINT32) * m_ui32IdxCount );
	for( UINT32 i = 0 ; i < m_ui32IdxCount ; i++ )
	{
		m_pui32Indexing[ i ] = i;
	}
}

UINT32	CListIndexing::CreateIdx()
{
	UINT32 ui32Idx = m_pui32Indexing[ m_ui32CreateIdx ];

	m_ui32CreateIdx++;
	m_ui32CreateIdx %= m_ui32IdxCount;

	return ui32Idx;
}

void	CListIndexing::DeleteIdx( UINT32 ui32Idx )
{
	m_pui32Indexing[ m_ui32DeleteIdx ] = ui32Idx;

	m_ui32DeleteIdx++;
	m_ui32DeleteIdx %= m_ui32IdxCount;
}