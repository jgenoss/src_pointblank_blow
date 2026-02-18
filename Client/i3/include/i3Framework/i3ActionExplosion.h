#if !defined( __I3_ACTION_EXPLOSION_H)
#define __I3_ACTION_EXPLOSION_H

class I3_EXPORT_FRAMEWORK i3ActionExplosion : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionExplosion, i3Action);
protected:
	i3MemoryBuffer *	m_pAttachBone = nullptr;

	REAL32			m_Range = 10.0f;
	REAL32			m_Damage = 100.0f;
	REAL32			m_Force = 10.0f;
	REAL32			m_Slope = 0.5f;

	bool			m_bAttachedBone = false;

public:
	i3ActionExplosion(void);
	virtual ~i3ActionExplosion(void);

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;

	REAL32			getRange(void)			{ return m_Range; }
	void			setRange( REAL32 val)	{ m_Range = val; }

	REAL32			getDamage(void)			{ return m_Damage; }
	void			setDamage( REAL32 val)	{ m_Damage = val; }

	REAL32			getForce(void)			{ return m_Force; }
	void			setForce( REAL32 val)	{ m_Force = val; }
	
	REAL32			getSlope(void)			{ return m_Slope; }
	void			setSlope( REAL32 val)	{ m_Slope = val; }

	bool			getAttachEnable(void)				{ return m_bAttachedBone; }
	void			setAttachEnable( bool bFlag)		{ m_bAttachedBone = bFlag;}
	char *			getAttachBoneName(void)				
	{ 
		if( m_pAttachBone == nullptr)
			return nullptr;

		return (char *) m_pAttachBone->getBuffer(); 
	}

	void			SetAttachBoneName( const char * pszName);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void	OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
