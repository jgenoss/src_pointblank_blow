#ifndef _TaskProcessor_h
#define _TaskProcessor_h

class CPacketLocker;
class CGateway;

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE(CTaskProcessor);
public:
	CTaskProcessor();
	virtual ~CTaskProcessor();

	BOOL Create(UINT32 nDataSize,CPacketLocker* pPacketLocker,CGateway* pGateway);
	virtual UINT32 OnRunning(void* pUserData);		
	BOOL SendPacketMessage(UINT32 idx,i3NetworkPacket* pPacket);

	void _DoTaskForGame(UINT32 idxBuffer);
	void _AccountKick(UINT8* pPacket);	

private:
	BOOL m_bIsRunning;
	UINT8* m_pRecvBuffer;
	UINT8* m_pSendBuffer;
	UINT32 m_nDataSize;
	UINT32 m_nReadedLength;	
	CPacketLocker* m_pPacketLocker;
	CGateway* m_pGateway;
};

extern CTaskProcessor* g_pTaskProcessor;

#endif //_TaskProcessor_h
