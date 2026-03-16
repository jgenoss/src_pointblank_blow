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
	REAL32						m_timeBlend;
	REAL32						m_timeCur;
	REAL32						m_timeScale;
	i3TransformModifier *		m_pModifier;

	MATRIX						m_CacheMatrix;
	UINT32						m_lastFrameID;

protected:
	void		ShiftBind(void);

public:
	i3TransformSourceCombiner(void);
	virtual ~i3TransformSourceCombiner(void); 

	BOOL		IsEmpty(void)						{ return (m_BindCount == 0); }

	INT32				getAnimationCount(void)		{ return m_BindCount; }
	i3Animation *		getAnimation( INT32 idx);
	INT32				getAnimationBoneIndex( INT32 idx);

	i3TransformModifier *	getModifier(void)				{ return m_pModifier; }
	void					setModifier( i3TransformModifier * pMod)
	{
		I3_REF_CHANGE( m_pModifier, pMod);
	}

	void		AddAnimation( i3Animation * pAnim, INT32 idxBone,
								REAL32 rStartTime = __RT_0,
								REAL32 interval = __RT_0, 
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP								 
								);
	void		RemoveAnimation( INT32 idx);
	void		RemoveAllAnimation(void);
	void		SetPlayCount(UINT32 count);
	void		SetPlayTime( REAL32 tm);
	void		AddPlayTime( REAL32 tm);
	void		SetTimeScale( REAL32 tm = 1.0f)				{ m_timeScale = tm;		}

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

	REAL32	getPlayTime(void)					{ return m_timeCur; }
	REAL32	GetDuration(void);
	REAL32	GetActualDuration( void);

	void		SetPlayMode( I3_ANIM_PLAY_MODE mode);

	void		GetMatrix( I3MATRIXINFO * pInfo);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
