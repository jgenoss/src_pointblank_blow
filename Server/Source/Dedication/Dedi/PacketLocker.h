// PacketLocker.h
//
// YouJong Ha
//	Last update : 2010-09-30 (yyyy:mm:dd)
//	
// Description:
//	복수의 Track buffer을 이용한 큐	
//
// Dependency: 
//	#include "spinLock.h"		
//	#include "trackBuffer.h"

#ifndef _PacketLocker_h
#define _PacketLocker_h

#include "S2CriticalSection.h"

class CTrackBuffer;


///////////////////////////////////////////////////////////////////////////////
//
// calss: CPacketLocker
//
// Description:
//	- Multiple Queue의 구현 클래스로서 SendQueue, Data Archive에 사용된다
//
class CPacketLocker: public i3ElementBase
{
	I3_CLASS_DEFINE(CPacketLocker);

public:
	
	BOOL Create( INT32 i32MaxTrack, INT32 i32MaxItemsPerTrack );	// n개의 track buffer(CTrackBuffer)를 생성한다
	
	UINT32 GetSize()  const { return m_i32Size;		}				// get # of track buffer
	
	INT32 GetCount( INT32 i32Track);								// i번째 track에 포함된 item의 수
	
	BOOL Push( INT32 i32Track, i3ElementBase*  pInput);				// i번째 track에 item 삽입
	BOOL Pop ( INT32 i32Track, i3ElementBase** ppOutput);			// i번째 track에서 item 얻기
//	BOOL Pop ( INT32 i32Track, i3ElementBase** ppOutput, INT32* pi32ElementCount ); // i번째 track에서 복수 item

	CPacketLocker();												// constructor
	virtual ~CPacketLocker();										// destructor

private:
	INT32 m_i32Size;												// # of track buffer
	CTrackBuffer** m_ppTrackBuffer;									// pointer array
};

extern CPacketLocker* g_pDataArchive;


#endif //_PacketLocker_h
