#if !defined( __I3_ACTION_EMIT_EFFECT_H)
#define __I3_ACTION_EMIT_EFFECT_H

#define I3_ACTION_EMIT_EFFECT_STYLE_ATTACH		0x00000001
#define I3_ACTION_EMIT_EFFECT_STYLE_TO_WORLD	0x00000002
#define	I3_ACTION_EMIT_EFFECT_STYLE_REPEAT		0x00000004

class I3_EXPORT_FRAMEWORK i3ActionEmitEffect : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionEmitEffect, i3Action);
protected:
	UINT32				m_nEmitStyle = 0;
	i3MemoryBuffer *	m_pEffectName = nullptr;
	i3MemoryBuffer *	m_pAttachBone = nullptr;

	VEC3D				m_Offset;
	REAL32				m_Theta = 0.0f, m_Rho = 0.0f;
	MATRIX				m_LocalTx;

	REAL32				m_Scale = 1.0f;

	REAL32				m_RepeatDuration = 1.0f;
	REAL32				m_RepeatInterval = 0.1f;

	// Volatile members
	INT32			m_idxEffect = -1;
	MATRIX *		m_pBoneMatrix = nullptr;

	REAL32				m_timeLife = 0.0f;
	REAL32				m_timeInterval = 0.0f;

protected:
	void			_updateLocalTx(void);

public:
	i3ActionEmitEffect(void);
	virtual ~i3ActionEmitEffect(void);

	char *			getEffectName(void)					
	{ 
		if( m_pEffectName == nullptr)
			return nullptr;

		return (char *) m_pEffectName->getBuffer(); 
	}

	void			SetEffectName( const char * pszName);

	bool			getAttachEnable(void)				{ return (m_nEmitStyle & I3_ACTION_EMIT_EFFECT_STYLE_ATTACH) != 0; }
	void			setAttachEnable( bool bFlag)		
	{ 
		if( bFlag)
			m_nEmitStyle |= I3_ACTION_EMIT_EFFECT_STYLE_ATTACH;
		else
			m_nEmitStyle &= ~I3_ACTION_EMIT_EFFECT_STYLE_ATTACH;
	}

	bool			getEmitToWorldState(void)			{ return (m_nEmitStyle & I3_ACTION_EMIT_EFFECT_STYLE_TO_WORLD) != 0; }
	void			setEmitToWorldState( bool bFlag)
	{
		if( bFlag)
			m_nEmitStyle |= I3_ACTION_EMIT_EFFECT_STYLE_TO_WORLD;
		else
			m_nEmitStyle &= ~I3_ACTION_EMIT_EFFECT_STYLE_TO_WORLD;
	}

	bool			isRepeat(void)						{ return (m_nEmitStyle & I3_ACTION_EMIT_EFFECT_STYLE_REPEAT) != 0; }
	void			setRepeat( bool bFlag)
	{
		if( bFlag)
			m_nEmitStyle |= I3_ACTION_EMIT_EFFECT_STYLE_REPEAT;
		else
			m_nEmitStyle &= ~I3_ACTION_EMIT_EFFECT_STYLE_REPEAT;
	}

	char *			getAttachBoneName(void)				
	{ 
		if( m_pAttachBone == nullptr)
			return nullptr;

		return (char *) m_pAttachBone->getBuffer(); 
	}

	void			SetAttachBoneName( const char * pszName);

	INT32			getEffectIndex(void)				{ return m_idxEffect; }
	void			setEffectIndex( INT32 idx)			{ m_idxEffect = idx; }

	MATRIX *		getAttachBoneMatrix(void)			{ return m_pBoneMatrix; }
	void			setAttachBoneMatrix( MATRIX * pMtx)	{ m_pBoneMatrix = pMtx; }

	REAL32			getRepeatDuration(void)				{ return m_RepeatDuration; }
	void			setRepeatDuration( REAL32 tm)		{ m_RepeatDuration = tm; }

	REAL32			getRepeatInterval(void)				{ return m_RepeatInterval; }
	void			setRepeatInterval( REAL32 tm)		{ m_RepeatInterval = tm; }

	VEC3D *			getOffset(void)						{ return &m_Offset; }
	void			setOffset( VEC3D * pOff);

	REAL32			getEmitTheta(void)					{ return m_Theta; }
	void			setEmitTheta( REAL32 th);
	REAL32			getEmitRho(void)					{ return m_Rho; }
	void			setEmitRho( REAL32 th);

	REAL32			getScale(void)						{ return m_Scale; }
	void			setScale( REAL32 scale)				{ m_Scale = scale; }

	MATRIX*			getLocalOffsetMatrix(void)			{ return &m_LocalTx; }

	void			reset(void);

	REAL32			getCurrentLifeTime(void)			{ return m_timeLife; }
	REAL32			addLifeTime( REAL32 tm)				{ return m_timeLife += tm; }

	REAL32			getCurrentInterval(void)			{ return m_timeInterval; }
	void			setCurrentInterval( REAL32 tm)		{ m_timeInterval = tm; }
	REAL32			addIntervalTime( REAL32 tm)			{ return m_timeInterval += tm; }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
