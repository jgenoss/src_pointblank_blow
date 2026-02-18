#ifndef __I3NETWORKSESSIONMANAGER_H
#define __I3NETWORKSESSIONMANAGER_H

#include "i3Mutex.h"
#include "i3List.h"

class i3NetworkSession; 

class I3_EXPORT_BASE i3NetworkSessionManager : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkSessionManager ); 

protected:
	INT32					m_ActiveSessionCount; 
	i3Mutex					m_SessionMutex;

	i3NetworkSession	**	m_pSessionList;
	INT32					m_NewSessionIdx;
	INT32					m_MaxSessionIdx;

	UINT32				*	m_pui32WorkerThreadIdList;

	i3NetworkSession	*	FindNewSession(void); 

public: 	
	i3NetworkSessionManager(); 	
	virtual ~i3NetworkSessionManager();

	virtual BOOL			OnCreate(i3List * pSessionList);	
	virtual void			OnUpdate(void); 
	virtual BOOL			OnDestroy(void);

	virtual ULONG_PTR		ConnectSession(SOCKET Socket, struct sockaddr_in * pAddr); 
	void					DisConnectSession(i3NetworkSession * pSession, BOOL bForceMain); 
	void					DisConnectAllSession(void);

	INT32					GetSessionCount(void)			{	return m_ActiveSessionCount;	}
	INT32					GetMaxSessionCount()			{	return m_MaxSessionIdx;			}

	UINT32*					GetWorkerThreadIdList()												{	return m_pui32WorkerThreadIdList;						}
	void					SetWorkerThreadIdList( UINT32	* pui32WorkerThreadIdList)			{	m_pui32WorkerThreadIdList = pui32WorkerThreadIdList;	}

};

#endif