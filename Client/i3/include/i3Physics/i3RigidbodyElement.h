#if !defined( __I3_RIGIDBODY_ELEMENT_H )
#define __I3_RIGIDBODY_ELEMENT_H

enum _I3_INERTIATYPE
{
	I3_INERTIA_SPHERE,
	I3_INERTIA_CUBE,
	I3_INERTIA_ROUND
};

//////////////////////////////////////////////////////////////////////////////////
// 강체의 구성 요소 클래스 2006. 1. 19
// 하나의 Mesh가 이루어질 수 있다.
// 하나의 Object는 여러 Mesh를 가질 수 있다.
class I3_EXPORT_PHYSICS i3RigidbodyElement : public i3ElementBase
{
	I3_CLASS_DEFINE( i3RigidbodyElement );
protected:
	REAL32			m_Mass;
	REAL32			m_InvMass;
	VEC3D			m_CenterPosition;
	VEC3D			m_LocalInertia;

	i3GeometryAttr *	m_pGeoAttr;

public:
	REAL32		getMass( void )				{ return m_Mass; }
	void		setMass( REAL32 m )			{ m_Mass = m; m_InvMass = i3Math::div( 1.0f, m_Mass ); }

	REAL32		getInvMass( void )			{ return m_InvMass; }
	void		setInvMass( REAL32 m )		{ m_InvMass = m ; }

	VEC3D *		getCenterPosition( void )	{ return &m_CenterPosition; }
	void		setCenterPosition( VEC3D * pVec )	{ i3Vector::Copy( &m_CenterPosition, pVec ); }

	VEC3D *		getInertia( void )			{ return &m_LocalInertia; }
	void		setInertia( VEC3D * pVec )	{ i3Vector::Copy( &m_LocalInertia, pVec ); }

	i3GeometryAttr * getGeometryAttr( void )	{ return m_pGeoAttr; }

public:
	i3RigidbodyElement();
	virtual ~i3RigidbodyElement();

	void		SetGeometryAttr( i3GeometryAttr * pObj );

	// 질량 중심점 산출
	void		CalcCenterPos( void );

	// 관성 모멘트를 산출한다. 2006. 1. 20. by dfly79
	// desc : 질량 중심이 계산되어져 있어야 합니다.
	void		CalcInertia( UINT32 nType );

};

#endif // __I3_RIGIDBODY_ELEMENT_H
