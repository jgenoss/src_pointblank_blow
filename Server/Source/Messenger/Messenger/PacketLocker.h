#ifndef _PacketLocker_h
#define _PacketLocker_h

class CTrackBuffer;

class CPacketLocker: public i3ElementBase
{
	I3_CLASS_DEFINE(CPacketLocker);
public:
	CPacketLocker();
	virtual ~CPacketLocker();

	BOOL			Create(UINT32 ui32BufferCount,UINT32 ui32BufferSize, UINT32 ui32DataSize = PACKETBUFFERSIZE,UINT32 ui32MaxIdx = UINT_MAX);
	BOOL			Push(UINT32 ui32idxBuffer,UINT8* pui8Data,UINT32 ui32DataSize);
	UINT32			Pop(UINT32 ui32idxBuffer,UINT8* pui8Data,UINT32 ui32DataSize);

	UINT32			getCount(void) const		{ return m_ui32Count;	}

private:
	CTrackBuffer**	m_ppTrackBuffer;
	UINT32			m_ui32Count;
};

extern CPacketLocker* g_pPacketLocker;

#endif //_PacketLocker_h
