#ifndef __MODULE_GATEWAY_H__
#define __MODULE_GATEWAY_H__

class CRingBuffer;
class CModuleReceiver;

class CModuleGateway : public i3Thread
{
	I3_CLASS_DEFINE( CModuleGateway );

	BOOL					m_bRunning;

	i3NetworkServerSocket*	m_pServerSocket;

	// 
	INT32					m_i32ReceiverIPCount;
	UINT32*					m_pui32ReciveIP;

	INT32					m_i32ReceiverCount;
	CModuleReceiver	**		m_ppReceiver;

	CModuleReceiver**		m_ppUseRingBuffer;
	CRingBuffer**			m_ppRingReceiver;
	
	INT32					m_i32LogThreadIdx;

protected:
	INT32					_GetAcceptIP( UINT32 ui32IP );
	INT32					_GetWaitSocket();
	INT32					_GetWaitRingBuffer();
	void					_AcceptSocket();

public:
	CModuleGateway();
	virtual ~CModuleGateway();

	BOOL					OnCreate( UINT32 ui32IP, UINT16 ui16Port, INT32 i32ServerCount, UINT32* pui32ServerIP, CRingBuffer** ppRingReceiver );
	void					OnDestroy();

	virtual UINT32			OnRunning( void* pUserData );

	BOOL					SendPacket( INT32 i32Idx, i3NetworkPacket* pPacket );

	CModuleReceiver *		GetReceiver( INT32 i32dx )			{ return m_ppReceiver[i32dx]; }		//불필요하다. 쓰는곳에서 삭제합니다. 
};

#endif //_Gateway_h
