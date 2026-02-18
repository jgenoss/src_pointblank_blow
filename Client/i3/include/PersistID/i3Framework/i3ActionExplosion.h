#if !defined( __I3_ACTION_EXPLOSION_H)
#define __I3_ACTION_EXPLOSION_H

class I3_EXPORT_FRAMEWORK i3ActionExplosion : public i3Action
{
	I3_CLASS_DEFINE( i3ActionExplosion);
protected:
	i3MemoryBuffer *	m_pAttachBone;

	REAL32			m_Range;
	REAL32			m_Damage;
	REAL32			m_Force;
	REAL32			m_Slope;

	bool			m_bAttachedBone;

public:
	i3ActionExplosion(void);
	virtual ~i3ActionExplosion(void);

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);

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
		if( m_pAttachBone == NULL)
			return NULL;

		return (char *) m_pAttachBone->getBuffer(); 
	}

	void			SetAttachBoneName( const char * pszName);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual void	OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
