#pragma once

#include "S2CriticalSection.h"
// 사용자 포지션을 체크하기 위한 큐.
// multiple circular quque이다.
// 
// Task Thread가 데이터를 추가하고, Cheating Checker가 검사한다.
// Cheating Checker는 한꺼번에 데이터를 꺼내간다.
// Note. TrackBuffer와의 차이점: 
//		1. push가 pop보다 매우 큰 경우에 오버라이트 된다.
//		2. point가 아닌 구조체 복사를 수행한다

struct DS_CHECK_POSITION;

class CPositionQueue
{
public:
	// create
	BOOL Create( INT32 i32Tracks, INT32 i32MaxItemsPerTrack);	// n개의 ring buffer(CTrackBuffer)를 생성한다
	
	// count
	INT32 GetTrackCount()  const				{ return (INT32) m_i32TrackCount;		}		
	INT32 GetItemCount( INT32 i32Track ) const	{ 
													if( i32Track >= 0 && i32Track < m_i32TrackCount ) 
														return (INT32)m_ai32ItemCountPerTrack[ i32Track ];
													else 
														return 0;
												}	
	// push / pop
	BOOL Push( INT32 i32Track, DS_CHECK_POSITION*  pItem);			// i번째 track에 item 삽입
	DS_CHECK_POSITION* Pop( INT32 i32Track );						// i번째 track에서 item 얻기

	// conster / destor
	CPositionQueue(void);
	~CPositionQueue(void);

private:
	INT32 m_i32TrackCount;											// # of track buffer
	INT32 m_i32MaxItemsPerTrack;
	INT32 m_ai32ItemCountPerTrack[ DS_MAX_HACK_CHECKER_COUNT ];	// # of entries in the track
	INT32 m_ai32ReadIdx [ DS_MAX_HACK_CHECKER_COUNT ];
	INT32 m_ai32WriteIdx[ DS_MAX_HACK_CHECKER_COUNT ];
	
	DS_CHECK_POSITION** m_ppTrackBuffer;								// pointer array

	S2CriticalSection m_cs;

};

extern CPositionQueue*	g_pPositionQueue;