#ifndef __I3NETWORKSESSIONMANAGER_H
#define __I3NETWORKSESSIONMANAGER_H

class i3NetworkSession; 

class I3_EXPORT_NETWORK i3NetworkSessionManager : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkSessionManager ); 
private:	
	INT32					m_ActiveSessionCount; 
	i3Mutex					m_SessionMutex;

protected: 
	i3NetworkSession	**	m_pSessionList;
	INT32					m_NewSessionIdx;
	INT32					m_MaxSessionIdx; 

	i3NetworkSession	*	FindNewSession(void); 

public: 	
	i3NetworkSessionManager(); 	
	virtual ~i3NetworkSessionManager();

	virtual BOOL OnCreate(i3List * pSessionList);	
	virtual void OnUpdate(void); 
	virtual BOOL OnDestroy(void);

	DWORD	ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr); 
	void	DisConnectSession(i3NetworkSession * pSession, BOOL bForceMain); 

	INT32	GetSessionCount(void)			{ return m_ActiveSessionCount; }	
	void	DisConnectAllSession(void);
};

#endif