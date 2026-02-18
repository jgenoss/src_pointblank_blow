#if !defined( __I3_EVENT_H)
#define __I3_EVENT_H

#include "i3Base.h"

class i3State;

class i3Event : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Event);
protected:
	i3State *		m_pFrom;
	i3State *		m_pTo;
	BOOL			m_bSignaled;
	INT32			m_Priority;

public:
	i3Event(void);
	virtual ~i3Event(void);

	void		SetFromState( i3State * pState);
	i3State	*	GetFromState(void)							{ return m_pFrom; }

	void		SetToState( i3State * pState);
	i3State *	GetToState(void)							{ return m_pTo; }

	void		SetSignaled( BOOL bState);
	BOOL		IsSignaled(void)							{ return m_bSignaled; }

	void		SetPriority( INT32 prio)					{ m_Priority = prio; }
	INT32		GetPriority(void)							{ return m_Priority; }

	virtual void OnBuildObjectList( i3List * pList);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

#endif