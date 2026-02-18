#if !defined( __I3_EFFECT_TYPE_INFO_H)
#define __I3_EFFECT_TYPE_INFO_H

class i3GameResSceneGraph;

class I3_EXPORT_FRAMEWORK i3EffectTypeInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( i3EffectTypeInfo);
protected:

	i3Node *		m_pTypeRoot;		//SceneType Root
	char			m_szName[128];		//ÀÌ¸§
	UINT32			m_HashCode;

	i3TimeStamp *	m_pUsedHead;
	i3TimeStamp *	m_pUsedTail;

	i3TimeStamp *	m_pFreeHead;

	bool			m_bDecal;

	bool			m_bCommonRes;

protected:
	void			_InitSoundNode( i3Node * pNode);

	void			_UnlinkFromUsedList( i3TimeStamp * pTemp);
	void			_LinkToUsedList( i3TimeStamp * pTemp);
	void			_UnlinkFromFreeList( i3TimeStamp * pTemp);
	void			_LinkToFreeList( i3TimeStamp * pTemp);

	i3TimeStamp *	FindFree( REAL32 timeStamp, bool bForce);

public:
	i3EffectTypeInfo(void);
	virtual ~i3EffectTypeInfo(void);

	bool			isDecal(void)					{ return m_bDecal; }
	void			setDecal( bool bFlag)			{ m_bDecal = bFlag; }

	void			Reset(void);

	void			Create( i3GameResSceneGraph * pRes, UINT32 count, BOOL bDecal = FALSE );
	void			DeleteAll(void);

	i3Node *		getRoot(void)					{ return m_pTypeRoot; }

	char *			getName(void)					{ return m_szName; }
	void			SetName( const char * pszName);

	UINT32			getHashCode(void)				{ return m_HashCode; }

	INT32			getInstanceCount(void)			{ return m_pTypeRoot->GetChildCount(); }

	bool			isCommonRes(void)				{ return m_bCommonRes; }
	void			setCommonRes( bool bFlag)		{ m_bCommonRes = bFlag; }

	i3TimeStamp *	Emit( REAL32 timeStamp, bool bForce);

	INT32			OnUpdate( REAL32 tm);
};


#endif
