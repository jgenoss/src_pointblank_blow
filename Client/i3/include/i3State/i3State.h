#if !defined( __I3_STATE_H)
#define __I3_STATE_H

#include "i3Base.h"
#include "i3Event.h"

typedef void		(*STATECALLBACKPROC)( void * pUserData);

class i3StateMachine;

class i3State : public i3NamedElement
{
	I3_CLASS_DEFINE( i3State);
protected:
	i3List		m_EventList;
	INT32		m_Priority;

public:
	i3State(void);
	virtual ~i3State(void);

	void		AddEvent( i3Event * pEvent);
	void		RemoveEvent( i3Event * pEvent);
	void		RemoveAllEvent(void);
	UINT32		GetEventCount(void)					{ return m_EventList.GetCount(); }
	i3Event *	GetEvent( UINT32 idx)				{ return (i3Event *) m_EventList.Items[ idx]; }

	INT32		GetPriority(void)					{ return m_Priority; }
	void		SetPriority( INT32 prio)			{ m_Priority = prio; }

	virtual		void	OnInit( i3StateMachine * pStateMachine);
	virtual		void	OnRun( i3StateMachine * pStateMachine);
	virtual		void	OnFinish( i3StateMachine * pStateMachine);

	virtual		void	OnBuildObjectList( i3List * pList);
	virtual		UINT32	OnSave( i3ResourceFile * pResFile);
	virtual		UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif