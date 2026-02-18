#if !defined( __I3_TRANSFORM_SOURCE_H)
#define __I3_TRANSFORM_SOURCE_H

#include "i3AnimationUtil.h"
#include "i3Matrix.h"

struct I3MATRIXINFO
{
	REAL32 m_Time = 0.0f;
	MATRIX * m_pOut = nullptr;
	MATRIX * m_pIn = nullptr;
	MATRIX * m_pModelViewMatrix = nullptr;
	MATRIX * m_pViewMatrix = nullptr;
	MATRIX * m_pInvertViewMatrix = nullptr;
	MATRIX * m_pProjectioMatrix = nullptr;

	I3_ANIM_PLAY_MODE	m_PlayMode = I3_ANIM_PLAY_MODE_REPEAT;

	UINT32		m_FrameID = 0;

	VEC3D		*	m_pOutTrans = nullptr;
	QUATERNION *	m_pOutRot = nullptr;
	VEC3D *			m_pOutScale = nullptr;
	UINT32			m_KeyChannel = 0;

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
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3TransformSource, i3NamedElement);

protected:
	UINT32		m_SourceType = I3MATH_TRANSFORM_SOURCE_MATRIX;

public:
	UINT32		GetTransformSourceType(void)		{ return m_SourceType; }

	virtual REAL32	GetDuration(void) = 0;
	virtual void		GetMatrix(I3MATRIXINFO * pInfo, bool bFirst) {}

	virtual UINT32		GetInterpolatedKeyframe(I3MATRIXINFO * pInfo, bool bFirst) { return 0; }
};

#endif
