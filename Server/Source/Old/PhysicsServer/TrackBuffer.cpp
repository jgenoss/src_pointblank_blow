// TrackBuffer.cpp
//
// YouJong Ha
//	Last update : 2011-07-11 (yyyy:mm:dd)
//	
// Description:
//	TrackBuffer 구현 파일
//	
#include "pch.h"
#include "TrackBuffer.h"

I3_CLASS_INSTANCE( CTrackBuffer, i3ElementBase );

//CTrackBuffer*	g_pRecvQueue = NULL;
CTrackBuffer*	g_pSendQueue = NULL;

///////////////////////////////////////////////////////////////////////////////
//
// Function: CTrackBuffer
//
// Description:
//	- constructor
//		 
// Arguments:
// Return:
//	 
CTrackBuffer::CTrackBuffer()
{
	m_i32Count		= 0;	// # of items
	m_i32Size		= 0;	// buffer size 
	m_i32Front		= 0;	// front index
	m_ppRing		= NULL; // ptrs array
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CTrackBuffer
//
// Description:
//	- destructor
//		
// Arguments:
// Return:
//
CTrackBuffer::~CTrackBuffer()
{
	for( INT32 i = 0; i < m_i32Count; i++)
	{
		I3MEM_SAFE_FREE ( m_ppRing[i]);
	}
	
	I3MEM_SAFE_FREE( m_ppRing );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Create
//
// Description:
//	- 메모리 할당 함수
//		
// Arguments:
//	- nItemsPerRing:	큐 아이템의 개수
//		 
// Return:
//	- TRUE/FALSE
//
BOOL CTrackBuffer::Create( INT32 i32MaxItems )
{
	I3ASSERT( NULL == m_ppRing );
	I3ASSERT( i32MaxItems > 0 );
	
	m_i32Size	= i32MaxItems;
	m_ppRing	= (i3ElementBase**) i3MemAlloc( sizeof(i3ElementBase*) * m_i32Size );
	if( m_ppRing == NULL) return FALSE;
	
	memset( m_ppRing, 0, sizeof(i3ElementBase*) * m_i32Size );
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Push
//
// Description:
//	- 큐에 아이템 삽입
//		- 
// Arguments:
//	- pInput : 아이템
//		- 
// Return:
//	- TRUE/FALSE
//
BOOL CTrackBuffer::Push( i3ElementBase*  pInput)
{
	I3ASSERT( m_ppRing);
	
	m_cs.Lock();

	if ( m_i32Size != m_i32Count )
	{
		m_ppRing[ ( m_i32Count + m_i32Front) % m_i32Size ] = pInput;
		m_i32Count++;
		
		m_cs.Unlock();
		return TRUE;
	}
	
	m_cs.Unlock();
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Pop
//
// Description:
//	- 큐에서 아이템 추출
//		- 
// Arguments:
//	- ppOutput : 출력용 포인터
//		- 
// Return:
//	- TRUE/FALSE
//
BOOL CTrackBuffer::Pop( i3ElementBase** ppOutput)
{
	I3ASSERT( m_ppRing);

	*ppOutput = NULL;

	m_cs.Lock();

	if( m_i32Count > 0 )
	{
		*ppOutput = m_ppRing[ m_i32Front ];
		m_i32Count--;
		m_i32Front++;
		if( m_i32Front == m_i32Size)
			m_i32Front = 0;
		
		m_cs.Unlock();
		return TRUE;
	}

	m_cs.Unlock();
	return FALSE;
}
