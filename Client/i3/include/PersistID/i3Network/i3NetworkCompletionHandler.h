#ifndef __I3NETWORKCOMPLETIONHANDLER_H
#define __I3NETWORKCOMPLETIONHANDLER_H

class i3NetworkIocpWorker; 
class i3NetworkSession; 
class i3NetworkSessionManager; 

class I3_EXPORT_NETWORK i3NetworkCompletionHandler : public i3ElementBase
{	
	I3_CLASS_DEFINE(i3NetworkCompletionHandler); 
private:
	
protected: 
	i3NetworkIocpWorker	**	m_pWorker;
	i3NetworkSession	**	m_pWorkSession; 
	i3Mutex					m_WorkSessionMutex;

	HANDLE					m_hIOCP;
	INT32					m_WorkerCount;

	i3NetworkSessionManager * m_pSessionManager; 

public:
	i3NetworkCompletionHandler(); 
	virtual ~i3NetworkCompletionHandler(); 

	BOOL OnCreate(INT32 WorkCount, i3NetworkSessionManager * pSessionManager); 
	BOOL OnDestroy(void);    
	
	void	OnAddIOCP(HANDLE handle, DWORD keyValue); 	
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

	void RemoveWorkingSession(INT32 Idx)
	{
		m_pWorkSession[Idx] = NULL; 		
	}
};

#endif