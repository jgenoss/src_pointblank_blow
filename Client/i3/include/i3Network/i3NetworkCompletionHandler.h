#ifndef __I3NETWORKCOMPLETIONHANDLER_H
#define __I3NETWORKCOMPLETIONHANDLER_H

class i3NetworkIocpWorker; 
class i3NetworkSession; 
class i3NetworkSessionManager; 

class I3_EXPORT_NETWORK i3NetworkCompletionHandler : public i3ElementBase
{	
	I3_EXPORT_CLASS_DEFINE(i3NetworkCompletionHandler, i3ElementBase); 
private:
	
protected: 
	i3Mutex					m_WorkSessionMutex;
	i3NetworkIocpWorker	**	m_pWorker = nullptr;
	i3NetworkSession	**	m_pWorkSession = nullptr; 
	i3NetworkSessionManager * m_pSessionManager = nullptr;
	HANDLE					m_hIOCP = 0;
	INT32					m_WorkerCount = 0;

public:
	virtual ~i3NetworkCompletionHandler(); 

	bool OnCreate(INT32 WorkCount, i3NetworkSessionManager * pSessionManager); 
	bool OnDestroy(void);    
	
	void	OnAddIOCP(HANDLE handle, DWORD keyValue); 	
	HANDLE	getWorkerIOCPHandle(void) { return m_hIOCP; }

	//Add만 필요함 
	bool AddWorkingSession(i3NetworkSession *	pSession, INT32 Idx)
	{
		bool Rv = true;
		m_WorkSessionMutex.Enter();
		{
			for(INT32 i= 0; i < m_WorkerCount; i++)
			{
				if( m_pWorkSession[i] == pSession)
				{
					Rv = false;
					break; 
				}
			}
			if(Rv) m_pWorkSession[Idx] = pSession; 
		}
		m_WorkSessionMutex.Leave(); 

		return Rv; 
	}

	void RemoveWorkingSession(INT32 Idx)
	{
		m_pWorkSession[Idx] = nullptr; 		
	}
};

#endif