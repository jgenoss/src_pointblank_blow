#ifndef _TrackBuffer_h
#define _TrackBuffer_h

class CTrackBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CTrackBuffer);
public:
	typedef struct SHeader
	{
		UINT32 _nLength;
	} HEADER;

public:
	CTrackBuffer();
	virtual ~CTrackBuffer();

	void Create(UINT32 ui32BufferSize,UINT32 ui32DataSize,UINT32 ui32MaxIdx);
	BOOL Push(UINT8* pui8Data,UINT32 ui32DataSize);
	UINT32 Pop(UINT8* pui8Data,UINT32 ui32DataSize);

private:
	UINT8* m_pui8Buffer;
	UINT32 m_ui32BufferSize;
	UINT32 m_ui32DataSize;
	UINT32 m_ui32idxWrite;
	UINT32 m_ui32idxRead;
	UINT32 m_ui32MaxIdx;
};

#endif //_TrackBuffer_h
