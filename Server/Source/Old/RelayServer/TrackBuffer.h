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

	void Create(UINT32 nBufferSize,UINT32 nDataSize,UINT32 nMaxIdx);
	BOOL Push(UINT8* pData,UINT32 nDataSize);
	UINT32 Pop(UINT8* pData,UINT32 nDataSize);	

private:
	UINT8* m_pBuffer;
	UINT32 m_nBufferSize;
	UINT32 m_nDataSize;
	UINT32 m_idxWrite;
	UINT32 m_idxRead;
	UINT32 m_nMaxIdx;
};

#endif //_TrackBuffer_h
