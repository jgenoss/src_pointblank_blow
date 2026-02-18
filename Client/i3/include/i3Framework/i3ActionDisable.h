#if !defined( _I3_ACTION_DISABLE_H__)
#define _I3_ACTION_DISABLE_H__

class I3_EXPORT_FRAMEWORK i3ActionDisable : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionDisable, i3Action);
private:
	bool			m_bEnable = false;
	REAL32			m_timeFade = 0.0f;

public:
	bool			getEnableState(void)					{ return m_bEnable; }
	void			setEnableState( bool bState)			{ m_bEnable = bState; }

	REAL32			getFadeTime(void)						{ return m_timeFade; }
	void			setFadeTime( REAL32 tm)					{ m_timeFade = tm; }

	virtual void	GetInfoString(char * pszStr, bool bShort, INT32 len) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};


#endif //#define _I3_ACTION_DISABLE_H__