#if !defined( __I3_EFFECT_TYPE_INFO_H)
#define __I3_EFFECT_TYPE_INFO_H

class i3GameResSceneGraph;

class I3_EXPORT_FRAMEWORK i3EffectTypeInfo : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3EffectTypeInfo, i3ElementBase);
protected:
	typedef void	(* EffectCancelProc)( i3Node * pNode);

	i3Node *		m_pTypeRoot;		//SceneType Root [initialize at constructor]
	i3::rc_string	m_Name;				//ÀÌ¸§
	UINT32			m_HashCode = 0xFFFFFFFF;

	i3TimeStamp *	m_pUsedHead = nullptr;
	i3TimeStamp *	m_pUsedTail = nullptr;
	i3TimeStamp *	m_pFreeHead = nullptr;

	bool			m_bDecal = false;
	bool			m_bCommonRes = false;

	void(*m_pCallbackFuncCancelEffect)() = nullptr;

protected:
	void			_InitSoundNode( i3Node * pNode);

	void			_UnlinkFromUsedList( i3TimeStamp * pTemp);
	void			_LinkToUsedList( i3TimeStamp * pTemp);
	void			_UnlinkFromFreeList( i3TimeStamp * pTemp);
	void			_LinkToFreeList( i3TimeStamp * pTemp);

	i3TimeStamp *	FindFree( REAL32 timeStamp, bool bForce, EffectCancelProc pProc );

public:
	i3EffectTypeInfo(void);
	virtual ~i3EffectTypeInfo(void);

	bool			isDecal(void)					{ return m_bDecal; }
	void			setDecal( bool bFlag)			{ m_bDecal = bFlag; }

	void			Reset(void);

	void			Create( i3GameResSceneGraph * pRes, UINT32 count, bool bDecal = false );
	void			DeleteAll(void);

	i3Node *		getRoot(void)					{ return m_pTypeRoot; }

	const char*		getName(void) const				{ return m_Name.c_str(); }
	void			SetName( const char * pszName);

	UINT32			getHashCode(void) const			{ return m_HashCode; }

	INT32			getInstanceCount(void)			{ return m_pTypeRoot->GetChildCount(); }

	bool			isCommonRes(void)				{ return m_bCommonRes; }
	void			setCommonRes( bool bFlag)		{ m_bCommonRes = bFlag; }

	i3TimeStamp *	Emit( REAL32 timeStamp, bool bForce, EffectCancelProc pProc );

	INT32			OnUpdate( REAL32 tm);
};


#endif
