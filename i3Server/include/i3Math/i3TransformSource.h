#if !defined( __I3_TRANSFORM_SOURCE_H)
#define __I3_TRANSFORM_SOURCE_H

#include "i3AnimationUtil.h"

struct I3MATRIXINFO
{
	REAL32 m_Time;
	MATRIX * m_pOut;
	MATRIX * m_pIn;
	MATRIX * m_pModelViewMatrix;
	MATRIX * m_pViewMatrix;
	MATRIX * m_pInvertViewMatrix;
	MATRIX * m_pProjectioMatrix;

	I3_ANIM_PLAY_MODE	m_PlayMode;

	UINT32		m_FrameID;

	VEC3D		*	m_pOutTrans;
	QUATERNION *	m_pOutRot;
	VEC3D *			m_pOutScale;
	UINT32			m_KeyChannel;

	inline void		setOutVectors( VEC3D * pPos, QUATERNION * pRot, VEC3D * pScale)
	{
		m_pOutTrans = pPos;
		m_pOutRot	= pRot;
		m_pOutScale	= pScale;
	}
};

#define		I3MATH_TRANSFORM_SOURCE_MATRIX			0x00000001
#define		I3MATH_TRANSFORM_SOURCE_QUATERNION		0x00000002

#define		I3MATH_KEY_TRANSLATE					0x00000001
#define		I3MATH_KEY_ROTATE						0x00000002
#define		I3MATH_KEY_SCALE						0x00000004

class I3_EXPORT_MATH i3TransformSource : public i3NamedElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3TransformSource);

protected:
	UINT32		m_SourceType;

public:
	i3TransformSource(void);
	virtual ~i3TransformSource(void);

	UINT32		GetTransformSourceType(void)		{ return m_SourceType; }

	virtual RT_REAL32	GetDuration(void) = 0;
	virtual void		GetMatrix( I3MATRIXINFO * pInfo, BOOL bFirst);

	virtual UINT32		GetInterpolatedKeyframe( I3MATRIXINFO * pInfo, BOOL bFirst);
};

#endif
