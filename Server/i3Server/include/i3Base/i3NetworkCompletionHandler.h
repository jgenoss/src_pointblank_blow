#ifndef __I3NETWORKCOMPLETIONHANDLER_H
#define __I3NETWORKCOMPLETIONHANDLER_H

#include "i3Mutex.h"

class i3NetworkIocpWorker; 
class i3NetworkSession; 
class i3NetworkSessionManager; 

class I3_EXPORT_BASE i3NetworkCompletionHandler : public i3ElementBase
{	
	I3_CLASS_DEFINE(i3NetworkCompletionHandler); 
private:
	
protected: 
	i3NetworkIocpWorker	**	m_pWorker;
	i3NetworkSession	**	m_pWorkSession; 
	i3Mutex					m_WorkSessionMutex;

	HANDLE					m_hIOCP;
	INT32					m_WorkerCount;

	// RECV
	UINT64*					m_pui64PacketCount;
	UINT64*					m_pui64PacketSize;

	// SEND
	UINT64*					m_pui64SendPacketCount;
	UINT64*					m_pui64SendPacketSize;

	i3NetworkSessionManager * m_pSessionManager; 

public:
	i3NetworkCompletionHandler(); 
	virtual ~i3NetworkCompletionHandler(); 

	BOOL OnCreate(INT32 WorkCount, i3NetworkSessionManager * pSessionManager); 
	BOOL OnDestroy(void);    
	
	void	OnAddIOCP(HANDLE handle, ULONG_PTR keyValue); 	
	HANDLE	getWorkerIOCPHandle(void) { return m_hIOCP; }

	//Add만 필요함 
	BOOL AddWorkingSession(i3NetworkSession *	pSession, INT32 Idx)
	{
		BOOL Rv = TRUE; 
		m_WorkSessionMutex.Enter();
		{
			for(INT32 i= 0; i < m_WorkerCount; i++)
			{
				if( m_pWorkSession[i] == pSession)
				{
					Rv = FALSE; 
					break; 
				}
			}
			if(Rv) m_pWorkSession[Idx] = pSession; 
		}
		m_WorkSessionMutex.Leave(); 

		return Rv; 
	}

	void AddPacketInfo( INT32 Idx, INT32 i32PacketSize )
	{
		m_pui64PacketCount[Idx]++;
		m_pui64PacketSize[Idx]	+= i32PacketSize;
	}

	void AddSendPacketInfo(INT32 Idx, INT32 i32PacketSize)
	{
		m_pui64SendPacketCount[Idx]++;
		m_pui64SendPacketSize[Idx] += i32PacketSize;
	}

	void RemoveWorkingSession(INT32 Idx)
	{
		m_pWorkSession[Idx] = NULL; 		
	}

	void					GetPacketInfo(UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize);
	void					GetPacketInfoThread(INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize);

	inline	i3NetworkIocpWorker*	GetIocpWorker( UINT32 idx ) { return m_pWorker[idx]; }
};

#endif