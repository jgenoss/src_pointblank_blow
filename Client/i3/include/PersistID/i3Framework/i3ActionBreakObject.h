#if !defined( __I3_ACTION_BREAK_OBJECT_H)
#define __I3_ACTION_BREAK_OBJECT_H

class I3_EXPORT_FRAMEWORK i3ActionBreakObject : public i3Action
{
	I3_CLASS_DEFINE( i3ActionBreakObject);
protected:
	REAL32		m_Damage;

public:
	i3ActionBreakObject(void);
	virtual ~i3ActionBreakObject(void);

	REAL32			getDamage(void)				{ return m_Damage; }
	void			setDamage( REAL32 dmg)		{ m_Damage = dmg; }

	virtual void	OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
