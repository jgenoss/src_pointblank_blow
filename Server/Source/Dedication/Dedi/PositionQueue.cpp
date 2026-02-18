#include "pch.h"
#include "LogFile.h"
#include "PositionQueue.h"

CPositionQueue*	g_pPositionQueue = NULL;

CPositionQueue::CPositionQueue(void)
{
	m_i32TrackCount = 0;
}

CPositionQueue::~CPositionQueue(void)
{
	for( INT32 i = 0; i < m_i32TrackCount; i++)
	{
		if( m_ppTrackBuffer[i] != NULL)
		{
			delete[] m_ppTrackBuffer[i];
			m_ppTrackBuffer[i] = NULL;
		}
	}
	if( m_ppTrackBuffer != NULL )
	{
		delete[] m_ppTrackBuffer;
	}
}


// n개의 ring buffer(CTrackBuffer)를 생성한다
BOOL CPositionQueue::Create( INT32 i32Tracks, INT32 i32MaxItemsPerTrack)	
{
	I3ASSERT( i32Tracks > 0);
	I3ASSERT( i32MaxItemsPerTrack > 0);
	
	m_ppTrackBuffer = (DS_CHECK_POSITION**) new DS_CHECK_POSITION*[ i32Tracks ] ;
	if( m_ppTrackBuffer == NULL) return FALSE;

	for ( INT32 i = 0; i < i32Tracks; i ++)
	{
		// alloc buffer
		m_ppTrackBuffer[i] = new DS_CHECK_POSITION[ i32MaxItemsPerTrack ];
		if( m_ppTrackBuffer[i] == NULL ) return FALSE;
		// reset counter
		m_ai32ItemCountPerTrack[i]	= 0;
		m_ai32ReadIdx[i]			= 0;
		m_ai32WriteIdx[i]			= 0;
	}

	m_i32TrackCount			=	i32Tracks;
	m_i32MaxItemsPerTrack	=	i32MaxItemsPerTrack;

	return TRUE;
}
	
// push / pop
// i번째 track에 item 삽입
BOOL CPositionQueue::Push( INT32 i32Track, DS_CHECK_POSITION*  pItem)
{
	I3ASSERT( pItem != NULL);

	INT32 i32WriteIdx = -1;
	if( i32Track >= 0 && i32Track < m_i32TrackCount )
	{
		m_cs.Lock();
		
		if ( m_ai32ItemCountPerTrack[ i32Track ] >= m_i32MaxItemsPerTrack )
		{
			I3TRACE("[CPositionQueue::Push] Buffer Full : Push Failed!\n");
			return FALSE;
		}
		
		// push
		i32WriteIdx =  m_ai32WriteIdx[ i32Track ];
		memcpy_s( &m_ppTrackBuffer[ i32Track ][ i32WriteIdx ], sizeof( DS_CHECK_POSITION ), pItem, sizeof( DS_CHECK_POSITION ) );
		
		// circular
		if( ++i32WriteIdx >= m_i32MaxItemsPerTrack ) i32WriteIdx = 0;
		
		// set write idx
		m_ai32WriteIdx[ i32Track ] = i32WriteIdx;
		
		// increase counter
		if( ++m_ai32ItemCountPerTrack[ i32Track ] > m_i32MaxItemsPerTrack )
			m_ai32ItemCountPerTrack[ i32Track ] = m_i32MaxItemsPerTrack;

		m_cs.Unlock();
		return TRUE;
	}
	
	return FALSE;
}
// i번째 track에서 item 얻기
DS_CHECK_POSITION* CPositionQueue::Pop( INT32 i32Track )
{
	INT32 i32ReadIdx = -1;
	DS_CHECK_POSITION* pItem = NULL;

	if( i32Track >= 0 && i32Track < m_i32TrackCount )
	{
		if ( m_ai32ItemCountPerTrack[ i32Track ] > 0 )
		{
			m_cs.Lock();
			i32ReadIdx =  m_ai32ReadIdx[ i32Track ];
			pItem = &m_ppTrackBuffer[ i32Track ][i32ReadIdx];
			
			// circular
			if( ++i32ReadIdx >= m_i32MaxItemsPerTrack ) 
				i32ReadIdx = 0;
			
			// set read index
			m_ai32ReadIdx[ i32Track ] = i32ReadIdx;
			
			// set counter
			if( --m_ai32ItemCountPerTrack[ i32Track ] < 0) 
				m_ai32ItemCountPerTrack[ i32Track ] = 0;

			m_cs.Unlock();
			return pItem;
		}
	}
	
	return NULL;
}
