#ifndef __i3NetworkIocpWorker_H
#define __i3NetworkIocpWorker_H

class i3NetworkSessionManager; 
class i3NetworkSession; 
class i3NetworkCompletionHandler; 

class I3_EXPORT_NETWORK i3NetworkIocpWorker : public i3Thread
{	
	I3_EXPORT_CLASS_DEFINE(i3NetworkIocpWorker, i3Thread); 
private:
	HANDLE							m_hIOCP = nullptr; 
protected: 
	INT32							m_Idx = -1;
	i3NetworkSessionManager		*	m_pSessionManager = nullptr;
	bool							m_bIsRun = false;
	i3NetworkSession	**			m_pWorkSession = nullptr; 
	i3NetworkCompletionHandler *	m_pCompletionHandler = nullptr;
	
public:
	virtual ~i3NetworkIocpWorker(); 

	virtual void	BeforeRunning(void * pUserData) override {}
	virtual UINT32	OnRunning( void * pUserData) override;
	virtual void	AfterRunning( void * pUserData) override;

	void SetIocpHandle(HANDLE IocpHandle, i3NetworkSessionManager * pSessionManager, INT32 ThreadIdx, i3NetworkCompletionHandler *	pCompletionHandler)	
	{ 
		m_hIOCP				 = IocpHandle; 
		m_pSessionManager	 = pSessionManager; 
		m_Idx				 = ThreadIdx; 
		m_pCompletionHandler = pCompletionHandler; 
	}
};

#endif