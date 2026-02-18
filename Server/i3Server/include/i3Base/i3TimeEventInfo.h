#if !defined( __I3_TIME_EVENT_INFO_H)
#define __I3_TIME_EVENT_INFO_H

#include "i3NamedElement.h"
#include "i3Action.h"

#define I3_TIMEEVENT_STATE_SELECTED		0x00000001
#define	I3_TIMEEVENT_STATE_DISABLE		0x00000002

class I3_EXPORT_BASE i3TimeEventInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( i3TimeEventInfo);
protected:
	UINT32		m_State;
	REAL32		m_Time;
	i3Action *	m_pAction;
	UINT32		m_Param;

	REAL32		m_Duration;

public:
	i3TimeEventInfo(void);
	virtual ~i3TimeEventInfo(void);

	UINT32			getState(void)						{ return m_State; }
	void			setState( UINT32 state)				{ m_State = state; }
	void			addState( UINT32 state)				{ m_State |= state; }
	void			removeState( UINT32 state)			{ m_State &= ~state; }

	REAL32			getStartTime(void)					{ return m_Time; }
	void			setStartTime( REAL32 tm)			{ m_Time = tm; }

	i3Action *		getAction(void)						{ return m_pAction; }
	void			setAction( i3Action * pAction)		{ I3_REF_CHANGE( m_pAction, pAction); }

	UINT32			getParam(void)						{ return m_Param; }
	void			setParam( UINT32 param)				{ m_Param = param; }

	REAL32			getDuration(void)					{ return m_Duration;}
	void			setDuration( REAL32 duration)		{ m_Duration = duration;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
