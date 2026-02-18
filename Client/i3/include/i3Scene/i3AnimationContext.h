#if !defined( __I3_ANIMATION_CONTEXT_H)
#define __I3_ANIMATION_CONTEXT_H

class i3Skeleton;

class I3_EXPORT_SCENE i3AnimationContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AnimationContext, i3ElementBase);
protected:
	// Persistant Element

	// Volatile members
	REAL32				m_timeLocal = 0.0f;
	i3MatrixArray *		m_pCacheMatrixArray = nullptr;
	i3Skeleton *		m_pFrameSkeleton = nullptr;

	UINT32				m_lastFrameID = 0xFFFFFFFF;

	i3::vector<i3TransformSourceCombiner*> m_CombinerList;
protected:
	void				_PrepareCombiner( INT32 count);
	void				_RemoveAllCombiner(void);

public:
	virtual ~i3AnimationContext(void);

	virtual void		Create( i3Skeleton * pSkel );

	REAL32				getLocalTime(void)			{ return m_timeLocal; }
	void				setLocalTime( REAL32 tm);
	void				addLocalTime( REAL32 tm)	{ m_timeLocal += tm; }

	MATRIX *			getCacheMatrix( INT32 idx)	{ return m_pCacheMatrixArray->GetMatrix( idx); }
	void				setCacheMatrix( INT32 idx, MATRIX * pMatrix)
	{
		m_pCacheMatrixArray->SetMatrix( idx, pMatrix);
	}

	i3MatrixArray *		getCacheMatrixArray(void)	{ return m_pCacheMatrixArray; }
	void				setCacheMatrixArray( i3MatrixArray * pArray)
	{
		I3_REF_CHANGE( m_pCacheMatrixArray, pArray);
	}

	INT32						getCombinerCount(void)		{ return (INT32)m_CombinerList.size(); }
	i3TransformSourceCombiner *	getCombiner( INT32 idx)
	{
		return m_CombinerList[idx];
	}
	UINT32						getLastUpdateFrameID(void)			{ return m_lastFrameID; }
	void						setLastUpdateFrameID( UINT32 id)	{ m_lastFrameID = id; }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;


	//*******************************************************************************************//
	// Skeleton Functions
	//*******************************************************************************************//
	i3Skeleton *		getFrameSkeleton(void)		{ return m_pFrameSkeleton; }
	void				setFrameSkeleton( i3Skeleton * pSkel)	{ m_pFrameSkeleton = pSkel; }

	const char *			GetBoneName( INT32 idx) const;
	const i3::rc_string&	GetBoneNameString(INT32 idx) const;
};

#endif
