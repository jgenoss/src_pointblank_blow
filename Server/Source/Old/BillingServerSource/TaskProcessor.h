#ifndef __TASK_PROCESSOR_H__
#define __TASK_PROCESSOR_H__

class CRingBuffer;
class CModuleGateway;
class CModuleDataBase;
class CLinkManager;

class CTaskProcessor: public i3Thread
{
	I3_CLASS_DEFINE( CTaskProcessor );

	BOOL					m_bIsRunning;
	
	INT32					m_i32LogThreadIdx;

	INT32					m_i32RingReceiver;
	CRingBuffer**			m_ppRingReceiver;
	
	CRingBuffer*			m_pRingDBUserCashGetIn;
	CRingBuffer*			m_pRingDBUserCashGetOut;
	CRingBuffer*			m_pRingDBUserCashBuyIn;
	CRingBuffer*			m_pRingDBUserCashBuyOut;

	CRingBuffer*			m_pRingDBLinkIn;
	CRingBuffer*			m_pRingDBLinkOut;

	CModuleDataBase*		m_pDBUserCash;
	CLinkManager*			m_pLinkManager;
	CModuleGateway*			m_pGateway;

private:
	void					_ParsingServer( INT32 i32Idx, void* pBuffer );

public:	
	CTaskProcessor();
	virtual ~CTaskProcessor();

	BOOL					OnCreate();
	void					OnDestroy();

	virtual UINT32			OnRunning( void* pUserData );


	BOOL					ServerSendPacket( INT32 i32Idx, i3NetworkPacket* pPacket );
};

extern CTaskProcessor* g_pTaskProcessor;

#endif
