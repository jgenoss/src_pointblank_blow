#if !defined( __I3_ANIMATION_CONTEXT_H)
#define __I3_ANIMATION_CONTEXT_H

class i3Skeleton;

class i3AnimationContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AnimationContext);
protected:
	// Persistant Element

	// Volatile members
	REAL32				m_timeLocal;
	i3MatrixArray *		m_pCacheMatrixArray;
	i3Skeleton *		m_pFrameSkeleton;

	UINT32				m_lastFrameID;

	i3List				m_CombinerList;
protected:
	void				_PrepareCombiner( INT32 count);
	void				_RemoveAllCombiner(void);

public:
	i3AnimationContext(void);
	virtual ~i3AnimationContext(void);

	void				Create( INT32 boneCount);

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

	INT32						getCombinerCount(void)		{ return m_CombinerList.GetCount(); }
	i3TransformSourceCombiner *	getCombiner( INT32 idx)
	{
		return (i3TransformSourceCombiner *) m_CombinerList.Items[idx];
	}

	i3Skeleton *				getFrameSkeleton(void)		{ return m_pFrameSkeleton; }
	void						setFrameSkeleton( i3Skeleton * pSkel)	{ m_pFrameSkeleton = pSkel; }

	UINT32						getLastUpdateFrameID(void)			{ return m_lastFrameID; }
	void						setLastUpdateFrameID( UINT32 id)	{ m_lastFrameID = id; }

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
};

#endif
