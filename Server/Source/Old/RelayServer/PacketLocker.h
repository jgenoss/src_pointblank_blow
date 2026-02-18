#ifndef _PacketLocker_h
#define _PacketLocker_h

class CTrackBuffer;

class CPacketLocker: public i3ElementBase
{
	I3_CLASS_DEFINE(CPacketLocker);
public:
	CPacketLocker();
	virtual ~CPacketLocker();

	void Create(UINT32 nBufferCount,UINT32 nBufferSize,
				UINT32 nDataSize = PACKETBUFFERSIZE,UINT32 nMaxIdx = UINT_MAX);
	BOOL Push(UINT32 idxBuffer,UINT8* pData,UINT32 nDataSize);
	UINT32 Pop(UINT32 idxBuffer,UINT8* pData,UINT32 nDataSize);

	UINT32 getCount(void) const		{ return m_nCount;	}

private:
	CTrackBuffer** m_ppTrackBuffer;
	UINT32 m_nCount;
};

extern CPacketLocker* g_pPacketLocker;

#endif //_PacketLocker_h
