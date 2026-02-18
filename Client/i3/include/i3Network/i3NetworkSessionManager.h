#ifndef __I3NETWORKSESSIONMANAGER_H
#define __I3NETWORKSESSIONMANAGER_H

class i3NetworkSession; 

class I3_EXPORT_NETWORK i3NetworkSessionManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkSessionManager, i3ElementBase ); 
private:	
	INT32					m_ActiveSessionCount = 0; 
	i3Mutex					m_SessionMutex;

protected: 
	i3NetworkSession	**	m_pSessionList = nullptr;
	INT32					m_NewSessionIdx = 0;
	INT32					m_MaxSessionIdx = 0; 

	i3NetworkSession	*	FindNewSession(void); 

public:
	virtual bool OnCreate(const i3::vector<i3NetworkSession*>& SessionList);	
	virtual bool OnCreate(i3NetworkSession** ppArray, INT32 ElemSize);
	virtual void OnUpdate(void) {}
	virtual bool OnDestroy(void);

	DWORD	ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr); 
	void	DisConnectSession(i3NetworkSession * pSession, bool bForceMain); 

	INT32	GetSessionCount(void)			{ return m_ActiveSessionCount; }	
	void	DisConnectAllSession(void);
};

#endif