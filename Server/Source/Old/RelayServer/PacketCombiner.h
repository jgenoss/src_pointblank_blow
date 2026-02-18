#ifndef _PacketCombiner_h
#define _PacketCombiner_h

class CPacketCombiner: public i3ElementBase
{
	I3_CLASS_DEFINE(CPacketCombiner);
public:
	CPacketCombiner();
	virtual ~CPacketCombiner();

	void Create(UINT32 nSize);
	BOOL PushStream(UINT8* pData,INT32 nLength);
	UINT32 PopPacket(UINT8* pData,BOOL bI3Packet = TRUE);
	void Clear(void);

private:
	UINT8* m_pBuffer;
	INT32 m_nSize;
	INT32 m_nFront;
	INT32 m_nRear;
	INT32 m_nPacketLength;
};

#endif //_PacketCombiner_h
