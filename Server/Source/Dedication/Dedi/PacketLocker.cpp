// PacketLocker.cpp
//
// YouJong Ha
//	Last update : 2011-07-11 (yyyy:mm:dd)
//	
// Description:
//	packetLocker 구현 파일
//
// Dependency: 
//	#include "spinLock.h"		
//	#include "trackBuffer.h"
//
#include "pch.h"
#include "TrackBuffer.h"
#include "PacketLocker.h"

I3_CLASS_INSTANCE( CPacketLocker, i3ElementBase);

// global variables
CPacketLocker* g_pDataArchive = NULL;	// 데이터 저장소. recv queue.

///////////////////////////////////////////////////////////////////////////////
//
// Function: CPacketLocker
//
// Description:
//	- constructor 
//
// Arguments:
// Return:
//
CPacketLocker::CPacketLocker()
{
	m_ppTrackBuffer		= NULL;
	m_i32Size			= 0;	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CPacketLocker
//
// Description:
//	- destructor
//
// Arguments:
// Return:
//
CPacketLocker::~CPacketLocker()
{
	if( m_ppTrackBuffer )
	{
		for( INT32 i = 0; i < m_i32Size; i++)
		{
			I3_SAFE_RELEASE( m_ppTrackBuffer[i] );
		}

		I3MEM_SAFE_FREE(m_ppTrackBuffer);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: Create
//
// Description:
//	- CTrackBuffer*를 위한 배열을 할당한다
//		
// Arguments:
//	- ui32RingBuffers : # of ring buffers (default : I/O thread count)
//	- ui32ItemsPerRing: # of items of the ring buffer
//
// Return:
//	- TRUE/FALSE
//
BOOL CPacketLocker::Create( INT32 i32MaxTrack, INT32 i32MaxItemsPerTrack )				   
{
	I3ASSERT( i32MaxTrack > 0);
	I3ASSERT( i32MaxItemsPerTrack > 0);
	I3ASSERT( m_ppTrackBuffer == NULL );
	
	m_i32Size			= i32MaxTrack;
	
	m_ppTrackBuffer = (CTrackBuffer**) i3MemAlloc(sizeof(CTrackBuffer*) * m_i32Size );
	
	if( m_ppTrackBuffer == NULL ) return FALSE;
	
	for( INT32 i = 0; i < m_i32Size; i++ )
	{
		m_ppTrackBuffer[i] = CTrackBuffer::NewObject();
		if ( m_ppTrackBuffer[i] == NULL ) return FALSE;
		if ( m_ppTrackBuffer[i]->Create( i32MaxItemsPerTrack ) == FALSE ) return FALSE;
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Push
//
// Description:
//	- Insert an item into the TrackBuffer
//
// Arguments:
//	- idxRing	: ring index
//	- pInput	: input data
//	
// Return:
//	- TRUE/FALSE
//
BOOL CPacketLocker::Push( INT32 i32Track, i3ElementBase* pInput)
{
	I3ASSERT( m_ppTrackBuffer);
	
	if( i32Track < 0 || i32Track >= m_i32Size )
	{
		DSERROR("[CPacketLocker::Push] Index\n");
		return FALSE;
	}

	//I3TRACE("[CPacketLocker::Push] thread id=%d, index=%d, before count=%d\n", ::GetCurrentThreadId(), i32Track, m_ppTrackBuffer[ i32Track ]->GetCount() );
	
	I3ASSERT( m_ppTrackBuffer[ i32Track ] );
	return m_ppTrackBuffer[ i32Track ]->Push( pInput );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Pop
//
// Description:
//	- Get an item from the TrackBuffer
//
// Arguments:
//	- idxRing	: ring index
//	- ppOutput: output data
//		
// Return:
//	- TRUE/FALSE
//
BOOL CPacketLocker::Pop( INT32 i32Track, i3ElementBase** ppOutput)
{
	I3ASSERT(m_ppTrackBuffer);

	if( i32Track < 0 || i32Track >= m_i32Size )
	{
		DSERROR("[CPacketLocker::Pop] Index\n");
		return FALSE;
	}

	//I3TRACE("[CPacketLocker::Pop] thraed id=%d, %d, before count = %d \n", ::GetCurrentThreadId(), i32Track, m_ppTrackBuffer[ i32Track ]->GetCount() );

	I3ASSERT( m_ppTrackBuffer[ i32Track ] );	
	return m_ppTrackBuffer[ i32Track ]->Pop( ppOutput );
}


//BOOL CPacketLocker::Pop (INT32 i32Track, i3ElementBase** ppOutput, INT32* pi32ElementCount )
//{
//	I3ASSERT( ppOutput != NULL );
//	I3ASSERT( pi32ElementCount != NULL );
//	I3ASSERT( m_ppTrackBuffer != NULL );
//	
//	if ( i32Track < 0 || i32Track >= m_i32Size )
//	{
//		DSERROR("[CPacketLocker::Pop] Index\n");
//		return FALSE;
//	}
//	
//	I3ASSERT( m_ppTrackBuffer[ i32Track ] != NULL );
//	return m_ppTrackBuffer[i32Track]->Pop( ppOutput, pi32ElementCount );
//}

///////////////////////////////////////////////////////////////////////////////
//
// Function: GetCount
//
// Description:
//	- i번째 track에 포함된 아이템의 수를 리턴한다
//		
// Arguments:
//	- idxRing : track 인덱스
//		
// Return:
//	- 아이템의 수
//
INT32 CPacketLocker::GetCount( INT32 i32Track )
{
	I3ASSERT( m_ppTrackBuffer );
	
	if ( i32Track < 0 || i32Track >= m_i32Size )
	{
		DSERROR("[CPacketLocker::GetCount] Index\n");
		return 0;
	}
	
	I3ASSERT( m_ppTrackBuffer[ i32Track ] );	// check track
	return m_ppTrackBuffer[ i32Track ]->GetCount();
} 

