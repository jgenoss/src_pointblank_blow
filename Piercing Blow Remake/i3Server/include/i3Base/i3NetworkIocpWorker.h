#ifndef __i3NetworkIocpWorker_H
#define __i3NetworkIocpWorker_H

#include "i3Thread.h"

class i3NetworkSessionManager; 
class i3NetworkSession; 
class i3NetworkCompletionHandler; 

class I3_EXPORT_BASE i3NetworkIocpWorker : public i3Thread
{	
	I3_CLASS_DEFINE(i3NetworkIocpWorker); 
private:
	HANDLE							m_hIOCP; 
protected: 
	INT32							m_Idx; 
	i3NetworkSessionManager		*	m_pSessionManager; 
	BOOL							m_bIsRun; 
	i3NetworkSession	**			m_pWorkSession; 
	i3NetworkCompletionHandler *	m_pCompletionHandler; 
	
public:
	i3NetworkIocpWorker(); 
	virtual ~i3NetworkIocpWorker(); 

	virtual void	BeforeRunning( void * pUserData);
	virtual UINT32	OnRunning( void * pUserData);		
	virtual void	AfterRunning( void * pUserData);

	void SetIocpHandle(HANDLE IocpHandle, i3NetworkSessionManager * pSessionManager, INT32 ThreadIdx, i3NetworkCompletionHandler *	pCompletionHandler)	
	{ 
		m_hIOCP				 = IocpHandle; 
		m_pSessionManager	 = pSessionManager; 
		m_Idx				 = ThreadIdx; 
		m_pCompletionHandler = pCompletionHandler; 
	}
};

#endif