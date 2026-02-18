#if !defined( __I3_ACTION_USER_EVENT_H)
#define __I3_ACTION_USER_EVENT_H

class I3_EXPORT_FRAMEWORK i3ActionUserEvent : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionUserEvent, i3Action);
protected:
	INT32			m_EventID = 0;
	INT32			m_Param1 = 0;
	INT32			m_Param2 = 0;

public:
	i3ActionUserEvent(void);

	INT32			getEventID(void)				{ return m_EventID; }
	void			setEventID( INT32 id)			{ m_EventID = id; }

	INT32			getParam1(void)					{ return m_Param1; }
	void			setParam1( INT32 param)			{ m_Param1 = param; }

	INT32			getParam2(void)					{ return m_Param2; }
	void			setParam2( INT32 param)			{ m_Param2 = param; }
	
	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
