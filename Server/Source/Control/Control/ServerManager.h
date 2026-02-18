#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

class CServerControl; 

class CServerControlManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CServerControlManager ); 
protected:
	CServerControl * m_pServerControl; 

public:
	CServerControlManager(); 
	virtual ~CServerControlManager();
	virtual BOOL OnCreate(void);	
	virtual void OnUpdate(void); 
	virtual BOOL OnDestroy(void);
}; 

extern CServerControlManager * g_pServerControlManager;

#endif