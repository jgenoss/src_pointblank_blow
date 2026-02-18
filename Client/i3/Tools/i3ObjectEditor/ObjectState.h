#if !defined( __OBJECT_STATE_H)
#define __OBJECT_STATE_H

class CObjectTemplate;

class CObjectState : public i3EventReceiver
{
	I3_CLASS_DEFINE( CObjectState, i3EventReceiver);
protected:
	REAL32					m_HPRatio;
	i3SceneGraphInfo *		m_pSg;
	i3AttrSet*				m_pCollideRootForClear;		// 소거용으로 반드시 필요..(2012.09.18.수빈)
														// i3Viewer2Framework.cpp(897)을 참고함...
	bool					m_bContainConvex;
	
	// Context
	BoneListPtr				m_pBoneList;
	i3Body *				m_pBody;

	i3TimeEventGen *		m_pTimeEvent;
	CObjectTemplate *		m_pOwner;
	bool					m_bModified;

protected:
	INT32					_FindTimeEventInsertPos( REAL32 tm);
	void					_CheckConvex(void);

	void					RemoveVolatileNodes( i3Node * pNode = NULL);

public:
	CObjectState(void);
	virtual ~CObjectState(void);

	void					setOwner( CObjectTemplate * pObj)	
	{ 
		m_pOwner = pObj; 
		m_pTimeEvent->setEventReceiver( (i3EventReceiver *) m_pOwner);
	}

	const BoneListPtr&		getBoneList(void) const	{ return m_pBoneList; }
	i3Body *				getBody(void)			{ return m_pBody; }

	REAL32					getHPRatio( void)			{ return m_HPRatio; }
	void					setHPRatio( REAL32 val)		{ m_HPRatio = val; }

	bool					getShadowCastEnable(void)	{ return m_pSg->getInstanceSg()->IsFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER) == FALSE; }
	void					setShadowCastEnable( bool bFlag)
	{
		if( m_pSg == NULL)
			return;

		if( bFlag)
			m_pSg->getInstanceSg()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		else
			m_pSg->getInstanceSg()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);


		m_bModified = true;
	}

	bool					isContainConvex(void)		{ return m_bContainConvex; }

	i3TimeEventGen *		getTimeEvent(void)		{ return m_pTimeEvent; }
	void					setTimeEvent( i3TimeEventGen * pObj)
	{
		I3_REF_CHANGE( m_pTimeEvent, pObj);
	}

	void					Enable(void);
	void					Disable(void);
	bool					Serialize(void);

	INT32					getEventCount(void)		{ return m_pTimeEvent->getEventCount(); }
	i3TimeEventInfo *		getEvent( INT32 idx)	{ return m_pTimeEvent->getEvent( idx); }
	INT32					AddTimeEvent( REAL32 tm, i3Action * pAction, UINT32 param);
	void					RemoveTimeEvent( INT32 idx);
	void					RemoveAllTimeEvent(void);

	i3SceneGraphInfo *	getSgResource(void)		{ return m_pSg; }
	void					setSgResource( i3SceneGraphInfo * pRes);

	void					BuildHull(void);

	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;

	void					OnDestroy(void);
	void					OnRevive(void);
};

#endif
