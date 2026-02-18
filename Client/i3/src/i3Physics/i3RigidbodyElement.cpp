#include "i3PhysicsType.h"
#include "i3RigidbodyElement.h"

I3_CLASS_INSTANCE( i3RigidbodyElement, i3ElementBase );

i3RigidbodyElement::i3RigidbodyElement()
{
	m_Mass = 0.0f;
	m_InvMass = 0.0f;
	i3Vector::Set( getCenterPosition(), 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &m_LocalInertia, 0.0f, 0.0f, 0.0f );

	m_pGeoAttr = NULL;
}

i3RigidbodyElement::~i3RigidbodyElement()
{
}

void i3RigidbodyElement::SetGeometryAttr( i3GeometryAttr * pObj )
{
	if( pObj != NULL)
		pObj->AddRef();

	I3_SAFE_RELEASE( pObj);

	m_pGeoAttr = pObj;
}

void i3RigidbodyElement::CalcCenterPos( void )
{
	UINT32 i;
	i3VertexArray * pVA;
	VEC3D	vPos;
	VEC3D	addPos;
	REAL32 invCount;

	I3ASSERT( m_pGeoAttr != NULL);

	pVA = m_pGeoAttr->GetVertexArray();
	I3ASSERT( pVA != NULL);

	i3Vector::Set( &addPos, 0.0f, 0.0f, 0.0f );

	pVA->Lock();

	for( i = 0; i < pVA->GetCount(); i++)
	{	
		pVA->GetPosition( i, &vPos);

		i3Vector::Add( &addPos, &addPos, &vPos);
	}

	invCount = i3Math::div( 1.0f, (REAL32)pVA->GetCount() );

	i3Vector::Scale( getCenterPosition(), &addPos, invCount );

	pVA->Unlock();
}

void i3RigidbodyElement::CalcInertia( UINT32 nType )
{
	UINT32 i;
	i3VertexArray * pVA;
	VEC3D vPos;
	REAL32 rx, ry, rz;
	REAL32 lx, ly, lz;
	REAL32 addRx, addRy, addRz;

	rx = ry = rz = 0.0f;
	lx = ly = lz = 0.0f;
	addRx = addRy = addRz = 0.0f;

	I3ASSERT( m_pGeoAttr != NULL );

	pVA = m_pGeoAttr->GetVertexArray();
	I3ASSERT( pVA != NULL );

	pVA->Lock();

	for( i = 0; i < pVA->GetCount(); i++ )
	{
		pVA->GetPosition( i, &vPos );

		rx = getX( &vPos ) - getX( getCenterPosition() );
		ry = getY( &vPos ) - getY( getCenterPosition() );
		rz = getZ( &vPos ) - getZ( getCenterPosition() );

		addRx += i3Math::Sqrt( i3Math::Mul( rx, rx ) );
		addRy += i3Math::Sqrt( i3Math::Mul( ry, ry ) );
		addRz += i3Math::Sqrt( i3Math::Mul( rz, rz ) );
	}

	addRx = i3Math::div( addRx, (REAL32)pVA->GetCount() );
	addRy = i3Math::div( addRy, (REAL32)pVA->GetCount() );
	addRz = i3Math::div( addRz, (REAL32)pVA->GetCount() );

	pVA->Unlock();

	if( nType == I3_INERTIA_SPHERE )
	{
		REAL32 r;
		r = i3Math::div( addRx + addRy + addRz, 3.0f );
		r = i3Math::Mul( r, r );

		lx = ly = lz = i3Math::Mul( r, 0.4f );
	}
	else
	{
		lx = i3Math::Mul( addRy, addRy ) + i3Math::Mul( addRz, addRz );
		ly = i3Math::Mul( addRx, addRx ) + i3Math::Mul( addRz, addRz );
		lz = i3Math::Mul( addRx, addRx ) + i3Math::Mul( addRy, addRy );
	}

	i3Vector::Set( getInertia(), lx, ly, lz );
	i3Vector::Scale( getInertia(), getInertia(), getMass() );
}

