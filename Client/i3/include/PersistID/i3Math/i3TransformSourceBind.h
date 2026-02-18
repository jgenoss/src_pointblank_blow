#if !defined( __I3_TRANSFORM_SOURCE_BIND_H)
#define __I3_TRANSFORM_SOURCE_BIND_H

#include "i3Base.h"
#include "i3TransformSource.h"
#include "i3AnimationUtil.h"

class I3_EXPORT_MATH i3TransformSourceBind 
{
protected:
	// Persistant
	i3TransformSource *			m_pTransSource;					

	I3_ANIM_PLAY_MODE			m_PlayMode;
	I3_ANIM_BLEND_MODE			m_BlendMode;

	// Volatile
	RT_MATRIX					m_CacheMatrix;

	BOOL						m_bFirst;
	RT_REAL32					m_timeLocal;
	UINT32						m_PlayCount;
public:
	i3TransformSourceBind(void);
	virtual ~i3TransformSourceBind(void);

	void						Init( RT_REAL32 tm = __RT_0);
	void						Reset(void);

	i3TransformSource *			GetTransformSource(void)			{ return m_pTransSource; }
	void						SetTransformSource( i3TransformSource * pTrans);

	UINT32						GetTransformSourceType(void)		{ return m_pTransSource->GetTransformSourceType(); }

	REAL32						getLocalTime(void)					{ return m_timeLocal; }
	void						SetPlayCount(UINT32 count)			{ m_PlayCount = count; }
	void						SetLocalTime( RT_REAL32 tm)			{ m_timeLocal = tm; }
	void						AddLocalTime( RT_REAL32 tm)			{ m_timeLocal += tm; }
	RT_REAL32					GetDuration(void)					{ return m_pTransSource->GetDuration(); }

	I3_ANIM_PLAY_MODE			GetPlayMode(void)							{ return m_PlayMode; }
	void						SetPlayMode( I3_ANIM_PLAY_MODE mode);

	UINT32						getPlayCount(void)							{ return m_PlayCount; }

	I3_ANIM_BLEND_MODE			GetBlendMode(void)							{ return m_BlendMode; }
	void						SetBlendMode( I3_ANIM_BLEND_MODE mode)		{ m_BlendMode = mode; }

	UINT32						GetQuaternion( I3MATRIXINFO * pInfo, 
												RT_QUAT * pQ, 
												RT_VEC3D * pPos, 
												RT_VEC3D * pScale,
												UINT32 * pKey);
#ifdef I3_DEBUG
protected:
	static BOOL					m_bCheckRuntimeRelease;
public:
	static void					CheckRuntimeRelease( BOOL bTrue = TRUE );
#endif
};

#endif
