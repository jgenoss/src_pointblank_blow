#if !defined( __I3_TRANSFORM_H)
#define __I3_TRANSFORM_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Node.h"
#include "i3ModelViewMatrixAttr.h"

class I3_EXPORT_SCENE i3Transform : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Transform);

protected:
	MATRIX					m_CacheMatrix;
	i3MatrixObject *		m_pMatrixObject;
	UINT32					m_UpdateFrameID;

	i3TransformSourceCombiner *		m_pTransCombiner;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3Transform(void);
	virtual ~i3Transform(void);

	//////////////////////////////////////
	// Internal Use Only
	UINT32			GetUpdateFrameID(void)				{ return m_UpdateFrameID; }
	void			SetUpdateFrameID( UINT32 id)		{ m_UpdateFrameID = id; }
	
	// Internal Use Only
	//////////////////////////////////////

	void			SetMatrixObject( i3MatrixObject * pObj);
	i3MatrixObject *GetMatrixObject(void)								{ return m_pMatrixObject; }

	void			SetMatrix( RT_MATRIX * pMatrix);
	RT_MATRIX *		GetMatrix(void)							{ return m_pMatrixObject->GetMatrix(); }

	RT_MATRIX *		GetCacheMatrix(void)					{ return &m_CacheMatrix;}

	BOOL			IsAnimated(void)									
	{ 
		return (m_pTransCombiner != NULL) && (m_pTransCombiner->IsEmpty() == FALSE); 
	}

	void			GetAnimatedMatrix( I3MATRIXINFO * pEnv);
	void			SetTransformSource( i3TransformSource * pTransSource,
							RT_REAL32 rStartTime = __RT_0,
							RT_REAL32 Interval = __RT_0,
							RT_REAL32 timeScale = __RT_1,
							I3_ANIM_BLEND_MODE BlendMode = I3_ANIM_BLEND_MODE_REPLACE,
							I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
							I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP
							);

	REAL32			getPlayTime(void)
	{
		if( m_pTransCombiner == NULL)
			return 0.0f;

		return m_pTransCombiner->getAnimPlayTime();
	}
	
	void			SetPlayCount(UINT32 count);
	void			SetPlayTime( RT_REAL32 tm);
	void			AddPlayTime( RT_REAL32 tm);
	void			SetPlayMode( I3_ANIM_PLAY_MODE mode)				{ m_pTransCombiner->SetPlayMode( mode); }
	RT_REAL32			GetDuration(void)
	{
		if( m_pTransCombiner != NULL)
			return m_pTransCombiner->GetDuration();

		return __RT_ZERO;
	}

	void			RemoveAllTransformSource(void)	
	{
		m_pTransCombiner->RemoveAllTransformSource();
	}

	UINT32			getPlayCount(void)									
	{ 
		if( m_pTransCombiner == NULL)
			return 0;

		return m_pTransCombiner->getPlayCount(); 
	}

	INT32			GetTransformSourceCount(void)						{ return m_pTransCombiner->GetTransformSourceCount(); }
	i3TransformSource * GetTransformSource( INT32 idx)					{ return m_pTransCombiner->GetTransformSource( idx); }

	virtual void	OnUpdate( i3SceneTracer * pTracer);
	virtual void	OnChangeTime( REAL32 tm);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
