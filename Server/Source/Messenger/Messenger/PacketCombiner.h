#ifndef _PacketCombiner_h
#define _PacketCombiner_h

class CPacketCombiner: public i3ElementBase
{
	I3_CLASS_DEFINE(CPacketCombiner);
public:
	CPacketCombiner();
	virtual ~CPacketCombiner();

	void	Create(UINT32 ui32Size);
	BOOL	PushStream(UINT8* pui8Data,INT32 i32Length);
	UINT32	PopPacket(UINT8* pui8Data,BOOL bI3Packet = TRUE);
	void	Clear(void);

private:
	UINT8*	m_pui8Buffer;
	INT32	m_i32Size;
	INT32	m_i32Front;
	INT32	m_i32Rear;
	INT32	m_i32PacketLength;
};

#endif //_PacketCombiner_h
