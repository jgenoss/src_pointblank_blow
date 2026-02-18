#ifndef __I3_MUTEX_H__
#define	__I3_MUTEX_H__

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#if defined( I3_WINDOWS ) 
	typedef CRITICAL_SECTION I3MUTEX;
#else
	//만들어야 합니다. 
	typedef INT32 I3MUTEX;
#endif

class I3_EXPORT_BASE i3Mutex : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Mutex );
protected:
	I3MUTEX	m_Mutex; 

public	: 
	i3Mutex(); 
	virtual ~i3Mutex(); 

	void Enter(void); 
	BOOL TryEnter(void); 
	void Leave(void); 
}; 


class I3_EXPORT_BASE i3Mutex2
{	
protected:
	I3MUTEX	m_Mutex; 

public	: 
	i3Mutex2(); 
	virtual ~i3Mutex2(); 

	void Enter(void); 
	BOOL TryEnter(void); 
	void Leave(void); 
}; 

#endif