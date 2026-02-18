#ifndef _Gateway_h
#define _Gateway_h

class CReceiver;
class CPacketLocker;

class CGateway: public i3Thread
{
	I3_CLASS_DEFINE(CGateway);
public:
	CGateway();
	virtual ~CGateway();

	BOOL Create(char* strIp,INT16 portNum,CPacketLocker* pPacketLocker);
	BOOL SendPacket(UINT32 idxReceiver,i3NetworkPacket* pPacket);
	BOOL SendIBRequestPacket(UINT8* pPacket,UINT32 nSize);
	BOOL SendIBNotifyPacket(UINT8* pPacket,UINT32 nSize);
	void RequestDisconnect(UINT32 idxReceiver);

	virtual UINT32 OnRunning(void* pUserData);

	UINT32 GetConnectedGameServerCount(void);
	BOOL IsConnectedGameServer(UINT32 idx);

	void AcceptGameServer(void);	

private:
	CReceiver** m_ppReceiver;
	UINT32 m_nReceiverCount;
	i3NetworkServerSocket m_ServerSocket;		
	
	BOOL m_IsRunning;
};

extern CGateway* g_pGateway;

#endif //_Gateway_h
