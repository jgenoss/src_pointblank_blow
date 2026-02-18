#include "pch.h"
#include "ListIndexingLink.h"

CListMainStreamBuffer::CListMainStreamBuffer()
{
	m_pBuffer = NULL;
}

CListMainStreamBuffer::~CListMainStreamBuffer()
{
	I3MEM_SAFE_FREE( m_pBuffer );
}

void CListMainStreamBuffer::CreateBuffer( UINT32 ui32Count )
{
	Create( ui32Count );
	m_pBuffer = ( MAIN_STREAM_BUFFER* )i3MemAlloc( sizeof(MAIN_STREAM_BUFFER) * ui32Count );
	i3mem::FillZero( m_pBuffer, sizeof( MAIN_STREAM_BUFFER ) * ui32Count );
}

UINT32 CListMainStreamBuffer::GetBlankIdx()
{
	UINT32 ui32Idx = CreateIdx();
	if( 0 == m_pBuffer[ ui32Idx ]._ui32ServerST )
	{
		m_pBuffer[ ui32Idx ]._ui32ServerST = i3ThreadTimer::GetServerTime();
		return ui32Idx;
	}
	return 0xFFFFFFFF;
}

void CListMainStreamBuffer::SetBlankBuffer( UINT32 ui32Idx )
{
	if( GetIdxCount() < ui32Idx )return;
	m_pBuffer[ ui32Idx ]._ui32ServerST = 0; 
	DeleteIdx( ui32Idx );
}

MAIN_STREAM_BUFFER * CListMainStreamBuffer::GetBuffer( UINT32 ui32Idx )
{
	if( GetIdxCount() < ui32Idx )	return	NULL;

	return &m_pBuffer[ ui32Idx ];
}
