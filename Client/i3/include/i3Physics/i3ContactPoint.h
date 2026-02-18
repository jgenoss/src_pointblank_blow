#if !defined( __I3_CONTACTPOINT_H )
#define __I3_CONTACTPOINT_H

#include "i3Rigidbody.h"

class I3_EXPORT_PHYSICS i3ContactPoint : public i3ElementBase
{
	I3_CLASS_DEFINE( i3ContactPoint );
protected:
	UINT32			m_Style;
	i3Rigidbody *	m_pA;		// 정점을 포함
	i3Rigidbody *	m_pB;		// 면을 포함
	RT_VEC3D		m_P;		// 접점
	RT_VEC3D		m_N;		// 노말
	RT_VEC3D		m_EA;		// A 변
	RT_VEC3D		m_EB;		// B 변
public:

public:
	i3ContactPoint();
	virtual ~i3ContactPoint();

	void Create();
};

#endif // __I3_CONTACTPOINT_H
