#ifndef _Gateway_h
#define _Gateway_h

class CReceiver;
class CPacketLocker;

class CGateway: public i3Thread
{
	I3_CLASS_DEFINE(CGateway);
private:
	CReceiver** m_ppReceiver;
	UINT32 m_ui32ReceiverCount;
	i3NetworkServerSocket m_ServerSocket;
	
	BOOL m_bIsRunning;
	INT32					m_i32LogIdx;

public:
	CGateway();
	virtual ~CGateway();

	BOOL Create(char* pstrIp,INT16 ui16portNum,CPacketLocker* pPacketLocker);
	BOOL SendPacket(UINT32 ui32idxReceiver,i3NetworkPacket* pPacket);
	BOOL SendIBRequestPacket(UINT8* pui8Packet,UINT32 ui32Size);
	BOOL SendIBNotifyPacket(UINT8* pui8Packet,UINT32 ui32Size);
	void RequestDisconnect(UINT32 ui32idxReceiver);

	virtual UINT32 OnRunning(void* pUserData);

	UINT32 GetConnectedGameServerCount(void);
	BOOL IsConnectedGameServer(UINT32 ui32idx);

	void AcceptGameServer(void);	

	CReceiver * GetReceiver(UINT32 ui32idx)	{ return m_ppReceiver[ui32idx]; }		//불필요하다. 쓰는곳에서 삭제합니다.
};

extern CGateway* g_pGateway;

#endif //_Gateway_h
