#ifndef __MUTEX_H__
#define	__MUTEX_H__

typedef CRITICAL_SECTION NSM_CRITICAL_SECTION;

class EXPORT_BASE NSM_cCriticalSection
{	
protected	:
	NSM_CRITICAL_SECTION	m_CriticalSection; 

public		: 
	NSM_cCriticalSection(); 
	virtual ~NSM_cCriticalSection();

	void Enter(void); 	
	void Leave(void); 
}; 

#endif