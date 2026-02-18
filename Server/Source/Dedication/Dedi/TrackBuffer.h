// TrackBuffer.h
//
// YouJong Ha
//	Last update : 2010-09-30 (yyyy:mm:dd)
//	
// Description:
//	cirular queue class
//
#ifndef _TRACKBUFFER_H_
#define _TRACKBUFFER_H_

class CTrackBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CTrackBuffer);

public:
	BOOL	Create( INT32 i32MaxItems );	// create
	BOOL	Push(i3ElementBase*  pInput);		// push element
	BOOL	Pop (i3ElementBase** ppOutput);		// pop element
	
	INT32	GetCount() const					{ return m_i32Count; }	

	CTrackBuffer();								// constructor
	virtual ~CTrackBuffer();					// destructor

private:
	
	INT32	m_i32Count;					// # of items (current)
	INT32	m_i32Size;				// ring buffer size 
	INT32	m_i32Front;				// read index
	
	i3ElementBase**	m_ppRing;			// ptrs array

	S2CriticalSection	  m_cs;
};

#endif //_TrackBuffer_h
