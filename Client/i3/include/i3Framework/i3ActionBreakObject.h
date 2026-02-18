#if !defined( __I3_ACTION_BREAK_OBJECT_H)
#define __I3_ACTION_BREAK_OBJECT_H

class I3_EXPORT_FRAMEWORK i3ActionBreakObject : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionBreakObject, i3Action);
private:
	REAL32		m_Damage = 0.0f;

public:
	REAL32			getDamage(void)				{ return m_Damage; }
	void			setDamage( REAL32 dmg)		{ m_Damage = dmg; }

	virtual void	OnEvent(i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override {}
	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
