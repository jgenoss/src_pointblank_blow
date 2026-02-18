#ifndef _TrackBuffer_h
#define _TrackBuffer_h

class CTrackBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CTrackBuffer);
public:
	struct HEADER
	{
		UINT32 _ui32Length;
	};

public:
	CTrackBuffer();
	virtual ~CTrackBuffer();

	BOOL	Create(UINT32 ui32BufferSize,UINT32 ui32DataSize,UINT32 ui32MaxIdx);
	BOOL	Push(UINT8* pui8Data,UINT32 ui32DataSize);
	UINT32	Pop(UINT8* pui8Data,UINT32 ui32DataSize);

private:
	UINT8*	m_pui8Buffer;
	UINT32	m_ui32BufferSize;
	UINT32	m_ui32DataSize;
	UINT32	m_ui32idxWrite;
	UINT32	m_ui32idxRead;
	UINT32	m_ui32MaxIdx;
};

#endif //_TrackBuffer_h
