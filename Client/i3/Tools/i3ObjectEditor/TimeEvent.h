#if !defined( __TIME_EVENT_H)
#define __TIME_EVENT_H

class CTimeEvent : public i3PersistantElement
{
	I3_CLASS_DEFINE( CTimeEvent);
protected:
	I3_TIME_EVENT	m_Event;

public:
	CTimeEvent(void);
	virtual ~CTimeEvent(void);

	REAL32			getTime(void)				{ return m_Event.m_Time; }
	void			setTime( REAL32 tm)			{ m_Event.m_Time = tm; }

	i3Action *		getAction(void)				{ return m_Event.m_pAction; }
	void			setAction( i3Action * pAction)
	{
		I3_REF_CHANGE( m_Event.m_pAction, pAction);
	}

	UINT32			getParam(void)				{ return m_Event.m_Param2; }
	void			setParam( UINT32 param)		{ m_Event.m_Param2 = param; }

	I3_TIME_EVENT *	getEvent(void)				{ return &m_Event; }
	void			setEvent( REAL32 tm, i3Action * pAction, UINT32 param)
	{
		m_Event.m_Time = tm;
		I3_REF_CHANGE( m_Event.m_pAction, pAction);
		m_Event.m_Param2 = param;
	}

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
