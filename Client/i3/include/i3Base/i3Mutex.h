#ifndef __I3_MUTEX_H__
#define	__I3_MUTEX_H__

#include "i3ElementBase.h"

#if defined( I3_WINDOWS ) 
	typedef CRITICAL_SECTION I3MUTEX;
#else
	//만들어야 합니다. 
	typedef INT32 I3MUTEX;
#endif

class I3_EXPORT_BASE i3Mutex : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Mutex, i3ElementBase );
protected:
	I3MUTEX	m_Mutex; 

public	: 
	i3Mutex(); 
	virtual ~i3Mutex(); 

	void Enter(void); 
	bool TryEnter(void); 
	void Leave(void); 
}; 


class I3_EXPORT_BASE i3Mutex2
{	
protected:
	I3MUTEX	m_Mutex; 

public	: 
	i3Mutex2(); 
	~i3Mutex2(); 

	void Enter(void); 
	bool TryEnter(void); 
	void Leave(void); 
};

class I3_EXPORT_BASE i3Monitor2
{
protected:
	i3Mutex2 *	m_pMutex;

public:
	i3Monitor2( i3Mutex2 * pMutex);
	~i3Monitor2(void);
};

#endif