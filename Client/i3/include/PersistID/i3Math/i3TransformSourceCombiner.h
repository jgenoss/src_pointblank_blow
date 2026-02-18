#if !defined( __I3_TRANSFORM_SOURCE_COMBINER_H)
#define __I3_TRANSFORM_SOURCE_COMBINER_H

#include "i3TransformSource.h"
#include "i3TransformSourceBind.h"
#include "i3TransformModifier.h"
#include "i3AnimationUtil.h"

class I3_EXPORT_MATH i3TransformSourceCombiner : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3TransformSourceCombiner);
protected:
	i3TransformSourceBind		m_Binds[2];
	INT32						m_BindCount;
	RT_REAL32					m_timeBlend;
	RT_REAL32					m_timeCur;
	RT_REAL32					m_timeScale;
	i3TransformModifier *		m_pModifier;

	MATRIX						m_CacheMatrix;
	UINT32						m_lastFrameID;

protected:
	void		ShiftBind(void);

public:
	i3TransformSourceCombiner(void);
	virtual ~i3TransformSourceCombiner(void); 

	BOOL		IsEmpty(void)						{ return (m_BindCount == 0); }

	INT32				GetTransformSourceCount(void)		{ return m_BindCount; }
	i3TransformSource *	GetTransformSource( INT32 idx);

	i3TransformModifier *	getModifier(void)				{ return m_pModifier; }
	void					setModifier( i3TransformModifier * pMod)
	{
		I3_REF_CHANGE( m_pModifier, pMod);
	}

	void		AddTransformSource( i3TransformSource * pTransSource, 
								RT_REAL32 rStartTime = __RT_0,
								RT_REAL32 interval = __RT_0, 
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP								 
								);
	void		RemoveTransformSource( INT32 idx);
	void		RemoveAllTransformSource(void);
	void		SetPlayCount(UINT32 count);
	void		SetPlayTime( RT_REAL32 tm);
	void		AddPlayTime( RT_REAL32 tm);
	void		SetTimeScale( RT_REAL32 tm = __RT_1)		{ m_timeScale = tm;		}

	MATRIX *	getCacheMatrix(void)						{ return &m_CacheMatrix; }

	UINT32		getPlayCount(void)
	{
		if( m_BindCount <= 0)
			return 0;

		return m_Binds[0].getPlayCount();
	}

	REAL32		getAnimPlayTime(void)
	{
		if( m_BindCount <= 0)
			return 0.0f;

		return m_Binds[0].getLocalTime();
	}

	RT_REAL32	getPlayTime(void)					{ return m_timeCur; }
	RT_REAL32	GetDuration(void);
	RT_REAL32	GetActualDuration( void);

	void		SetPlayMode( I3_ANIM_PLAY_MODE mode);

	void		GetMatrix( I3MATRIXINFO * pInfo);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
