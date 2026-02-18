#if !defined( __I3_TRANSFORM_SOURCE_H)
#define __I3_TRANSFORM_SOURCE_H

#include "i3AnimationUtil.h"

typedef struct _tagMatrixEnvironmentInfo
{
	REAL32 m_Time;
	RT_MATRIX * m_pOut;
	RT_MATRIX * m_pIn;
	RT_MATRIX * m_pModelViewMatrix;
	RT_MATRIX * m_pViewMatrix;
	RT_MATRIX * m_pInvertViewMatrix;
	RT_MATRIX * m_pProjectioMatrix;

	I3_ANIM_PLAY_MODE	m_PlayMode;

	UINT32		m_FrameID;

	#if defined( I3_PSP)
		BOOL	m_bUseRegister;
		INT32	m_RegIndex;
	#endif
} I3MATRIXINFO;

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

	virtual UINT32		GetQuaternion( I3MATRIXINFO * pInfo, RT_QUAT * pQ, RT_VEC3D * pPos, RT_VEC3D * pScale, BOOL bFirst);
};

#endif
