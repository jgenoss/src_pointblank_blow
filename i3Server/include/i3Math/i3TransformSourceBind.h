#if !defined( __I3_TRANSFORM_SOURCE_BIND_H)
#define __I3_TRANSFORM_SOURCE_BIND_H

#include "i3Base.h"
#include "i3Animation.h"
#include "i3AnimationUtil.h"

class I3_EXPORT_MATH i3TransformSourceBind 
{
protected:
	// Persistant
	i3Animation *				m_pAnim;
	INT32						m_idxBone;

	I3_ANIM_PLAY_MODE			m_PlayMode;
	I3_ANIM_BLEND_MODE			m_BlendMode;

	// Volatile
	RT_MATRIX					m_CacheMatrix;

	BOOL						m_bFirst;
	REAL32					m_timeLocal;
	UINT32						m_PlayCount;
public:
	i3TransformSourceBind(void);
	virtual ~i3TransformSourceBind(void);

	void						Init( REAL32 tm = __RT_0);
	void						Reset(void);

	i3Animation *				getAnimation(void)					{ return m_pAnim; }
	INT32						getBoneIndex(void)					{ return m_idxBone; }
	void						setAnimation( i3Animation * pAnim, INT32 idxBone)
	{
		I3_REF_CHANGE( m_pAnim, pAnim);
		m_idxBone = idxBone;
		m_PlayCount = 0;
		m_bFirst = TRUE;
	}

	REAL32						getLocalTime(void)					{ return m_timeLocal; }
	void						SetPlayCount(UINT32 count)			{ m_PlayCount = count; }
	void						SetLocalTime( REAL32 tm)			{ m_timeLocal = tm; }
	void						AddLocalTime( REAL32 tm)			{ m_timeLocal += tm; }
	REAL32						GetDuration(void)					{ return m_pAnim->GetDuration(); }

	I3_ANIM_PLAY_MODE			GetPlayMode(void)							{ return m_PlayMode; }
	void						SetPlayMode( I3_ANIM_PLAY_MODE mode);

	UINT32						getPlayCount(void)							{ return m_PlayCount; }

	I3_ANIM_BLEND_MODE			GetBlendMode(void)							{ return m_BlendMode; }
	void						SetBlendMode( I3_ANIM_BLEND_MODE mode)		{ m_BlendMode = mode; }

	void						GetInterpolatedKeyframe( I3MATRIXINFO * pInfo);

#ifdef I3_DEBUG
protected:
	static BOOL					m_bCheckRuntimeRelease;
public:
	static void					CheckRuntimeRelease( BOOL bTrue = TRUE );
#endif
};

#endif
