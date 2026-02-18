#if !defined( __I3_ACTION_VISIBLE_H)
#define __I3_ACTION_VISIBLE_H

class I3_EXPORT_FRAMEWORK i3ActionVisible : public i3Action
{
	I3_CLASS_DEFINE( i3ActionVisible);
protected:
	bool		m_bShow;
	REAL32		m_rAlphaTime;

public:
	i3ActionVisible(void);
	virtual ~i3ActionVisible(void);

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);

	bool			getEnable(void)				{ return m_bShow; }
	void			setEnable( bool bFlag)		{ m_bShow = bFlag; }

	REAL32			getAlphaTime(void)			{ return m_rAlphaTime; }
	void			setAlphaTime( REAL32 rTime )	{ m_rAlphaTime = rTime; }
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
