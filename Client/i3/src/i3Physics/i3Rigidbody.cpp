#include "i3PhysicsType.h"
#include "i3RigidbodyElement.h"
#include "i3Rigidbody.h"

I3_CLASS_INSTANCE( i3Rigidbody, i3ElementBase );

i3Rigidbody::i3Rigidbody()
{
	m_Type			= 0;				// 강체 타입
	m_Style			= 0;				// 강체 속성

	// 강체 속성
	m_Mass			= 0.0f;				// body mass
	m_InvMass		= 0.0f;				// inverse mass
	i3Matrix::Identity( &m_Inertia );		// moment of inertia
	i3Matrix::Identity( &m_InvInertia );	// inverse inertia

	// 상태 변수
	i3Vector::Set( &m_Position, 0.0f, 0.0f, 0.0f );			// 월드 좌표
	i3Quat::Identity( &m_Orientation );						// 월드 좌표의 방향
	i3Vector::Set( &m_Velocity, 0.0f, 0.0f, 0.0f );			// 물체 선속도
	i3Vector::Set( &m_AngularVelocity, 0.0f, 0.0f, 0.0f );	// 물체 각속도
		
	i3Vector::Set( &m_Force, 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &m_Moment, 0.0f, 0.0f, 0.0f );

	i3Vector::Set( &m_externalForce, 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &m_externalMoment, 0.0f, 0.0f, 0.0f );

	m_pGeoAttr = NULL;
}

i3Rigidbody::~i3Rigidbody()
{
	_RemoveAllElement();
}

void i3Rigidbody::_AddElement( i3RigidbodyElement * pObj )
{
	pObj->AddRef();

	m_ElementList.Add( pObj );
}

void i3Rigidbody::_RemoveAllElement( void )
{
	INT32 i;
	i3RigidbodyElement * pObj;

	for( i = 0; i < getElementCount(); i++ )
	{
		pObj = getElement( i );
		pObj->Release();
	}

	m_ElementList.Clear();
}

void i3Rigidbody::_CalcMassProperties( void )
{
	INT32 i;
	REAL32 Ixx, Iyy, Izz;
	REAL32 Ixy, Ixz, Iyz;
	REAL32 xx, yy, zz;
	REAL32 mass = 0.0f;
	VEC3D *pVec;
	i3RigidbodyElement * pObj;

	Ixx = Iyy = Izz = Ixy = Ixz = Iyz = 0.0f;

	for( i = 0; i < getElementCount(); i++ )
	{
		pObj = getElement( i );

		// 각 요소의 중심점 계산
		pObj->CalcCenterPos();

		// 각 요소의 관성값 계산
		pObj->CalcInertia( 1 );

		mass += pObj->getMass();

		pVec = pObj->getCenterPosition();
		// 
		xx = i3Math::Mul( getX( pVec ), getX( pVec ) );
		yy = i3Math::Mul( getY( pVec ), getY( pVec ) );
		zz = i3Math::Mul( getZ( pVec ), getZ( pVec ) );
		
		Ixx += getX( pObj->getInertia() ) + i3Math::Mul( pObj->getMass(), yy + zz );
		Iyy += getY( pObj->getInertia() ) + i3Math::Mul( pObj->getMass(), zz + xx );
		Izz += getZ( pObj->getInertia() ) + i3Math::Mul( pObj->getMass(), xx + yy );

		Ixy += i3Math::Mul( pObj->getMass(), i3Math::Mul( getX( pVec ), getY( pVec ) ) );
		Ixz += i3Math::Mul( pObj->getMass(), i3Math::Mul( getX( pVec ), getZ( pVec ) ) );
		Iyz += i3Math::Mul( pObj->getMass(), i3Math::Mul( getY( pVec ), getZ( pVec ) ) );
	}

	i3Matrix::Identity( &m_Inertia );
	i3Matrix::SetRight( &m_Inertia, Ixx, -Ixy, -Ixz, 0.0f );
	i3Matrix::SetUp( &m_Inertia, -Ixy, Iyy, -Iyz, 0.0f );
	i3Matrix::SetAt( &m_Inertia, -Ixz, -Iyz, Izz, 0.0f );

	i3Matrix::Inverse( &m_InvInertia, NULL, &m_Inertia );
}

void i3Rigidbody::Create( void )
{

}

void i3Rigidbody::BuildElement( I3PSQ_PERSIST_RIGIDBODYELEMENT * pElement, INT32 nCount )
{
	INT32 i;
	VEC3D Vec;
	i3RigidbodyElement * pObj;

	_RemoveAllElement();

	for( i = 0; i < nCount; i++ )
	{
		pObj = i3RigidbodyElement::NewObjectRef();

		pObj->setMass( pElement[i].m_Mass );
		i3Vector::Set( &Vec, pElement[i].m_CenterPos.x, pElement[i].m_CenterPos.y, pElement[i].m_CenterPos.z );
		pObj->setCenterPosition( &Vec );
		i3Vector::Set( &Vec, pElement[i].m_Inertia.x, pElement[i].m_Inertia.y, pElement[i].m_Inertia.z );
		pObj->setInertia( &Vec );

		_AddElement( pObj );
	}
}


void i3Rigidbody::CalcForce(void)
{
	VEC3D Vec;

	i3Vector::Set( &m_Force, 0.0f, 0.0f, 0.0f );
	i3Vector::Set( &m_Moment, 0.0f, 0.0f, 0.0f );

	if( m_Style & I3_PHYSICS_PARAM_LOCKED )
		return ;

	if( m_Style & I3_PHYSICS_PARAM_GRAVITY )
	{
		i3Vector::Scale( &Vec, &m_Gravity, m_Mass );
		i3Vector::Add( &m_Force, &m_Force, &Vec );
	}

	if( m_Style & I3_PHYSICS_DRAG_AIR )
	{	
		i3Vector::Scale( &Vec, &m_Velocity, m_DragFactor );
		i3Vector::Sub( &m_Force, &m_Force, &Vec );

		i3Vector::Scale( &Vec, &m_AngularVelocity, m_DragFactor );
		i3Vector::Sub( &m_Moment, &m_Moment, &Vec );
	}

	if( m_Style & I3_PHYSICS_PARAM_WINDFACTOR )
		i3Vector::Add( &m_Force, &m_Force, &m_Wind );


}

// 2006. 1. 20. by dfly79
void i3Rigidbody::CalcIntegrate(void)
{	
	REAL32 f;
	VEC3D Vec;

	if( m_Style & I3_PHYSICS_INTEGRATE_EULER )
	{
		// 이동에 관한 적분
		i3Physics::CalcIntegrateOverTime( &m_Velocity, &m_Position, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, m_dTime, m_InvMass );
		
		// 회전에 관한 적분
		f = i3Vector::Normalize( &Vec, &m_AngularVelocity );
		i3Quat::RotateAxis( &m_Orientation, &Vec, i3Math::Mul( f, m_dTime ) );
	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_MIDP )
	{
		REAL32 halfdt = i3Math::Mul( m_dTime, 0.5f );

		// 이동에 관한 적분
		i3Physics::CalcIntegrateOverTime( &m_Velocity, &m_Position, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, halfdt, m_InvMass );
		
		// 회전에 관한 적분
		f = i3Vector::Normalize( &Vec, &m_AngularVelocity );
		i3Quat::RotateAxis( &m_Orientation, &Vec, i3Math::Mul( f, halfdt ) );

		CalcForce();

		i3Physics::CalcIntegrateOverTime( &m_Velocity, &m_Position, &m_Force, &m_Velocity,
			&m_Velocity, &m_Position, m_dTime, m_InvMass );
		
		// 회전에 관한 적분
		f = i3Vector::Normalize( &Vec, &m_AngularVelocity );
		i3Quat::RotateAxis( &m_Orientation, &Vec, i3Math::Mul( f, m_dTime ) );

	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_RK4 )
	{
	}

}

void i3Rigidbody::StepSimulation( REAL32 dTime )
{
	setDelta( dTime );

	// 힘 계산
	CalcForce();

	// 적분
	CalcIntegrate();

	// 충돌 처리

	// Geometry Update

}

void i3Rigidbody::OnBuildObjectList( i3List * pList)
{
	if( m_pGeoAttr != NULL)
	{
		m_pGeoAttr->OnBuildObjectList( pList);
	}
	
	i3NamedElement::OnBuildObjectList( pList);
}

UINT32 i3Rigidbody::OnSave( i3ResourceFile * pResFile )
{
	INT32 i;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	{
		I3PSQ_PERSIST_RIGIDBODY Info;
		
		memcpy( Info.m_ID, "PRE1", 4 );
		Info.m_Type		= getType();
		Info.m_Style	= getStyle();

		Info.m_ElementCount = getElementCount();
		Info.m_pGeoAttr		= (OBJREF) m_pGeoAttr;
		
		Info.m_Inertia.mat[0].x = i3Matrix::Get11( getInertia() );
		Info.m_Inertia.mat[0].y = i3Matrix::Get12( getInertia() );
		Info.m_Inertia.mat[0].z = i3Matrix::Get13( getInertia() );
		Info.m_Inertia.mat[0].w = i3Matrix::Get14( getInertia() );

		Info.m_Inertia.mat[1].x = i3Matrix::Get21( getInertia() );
		Info.m_Inertia.mat[1].y = i3Matrix::Get22( getInertia() );
		Info.m_Inertia.mat[1].z = i3Matrix::Get23( getInertia() );
		Info.m_Inertia.mat[1].w = i3Matrix::Get24( getInertia() );

		Info.m_Inertia.mat[2].x = i3Matrix::Get31( getInertia() );
		Info.m_Inertia.mat[2].y = i3Matrix::Get32( getInertia() );
		Info.m_Inertia.mat[2].z = i3Matrix::Get33( getInertia() );
		Info.m_Inertia.mat[2].w = i3Matrix::Get34( getInertia() );

		Info.m_Inertia.mat[3].x = i3Matrix::Get41( getInertia() );
		Info.m_Inertia.mat[3].y = i3Matrix::Get42( getInertia() );
		Info.m_Inertia.mat[3].z = i3Matrix::Get43( getInertia() );
		Info.m_Inertia.mat[3].w = i3Matrix::Get44( getInertia() );

		Info.m_Gravity.x = i3Vector::GetX( getGravity() );
		Info.m_Gravity.y = i3Vector::GetY( getGravity() );
		Info.m_Gravity.z = i3Vector::GetZ( getGravity() );

		Info.m_Wind.x = i3Vector::GetX( getWind() );
		Info.m_Wind.y = i3Vector::GetY( getWind() );
		Info.m_Wind.z = i3Vector::GetZ( getWind() );

		Info.m_Mass = getMass();
		Info.m_Drag = getDrag();

		Rc = pStream->Write( &Info, sizeof( Info ) );
		if( Rc == STREAM_ERR )
		{
			I3FATAL( "Could not write rigidbody info.\n" );
			return STREAM_ERR;
		}
		Result += Rc;
	}

	{
		I3PSQ_PERSIST_RIGIDBODYELEMENT Info;
		i3RigidbodyElement * pElement;

		for( i = 0; i < getElementCount(); i++ )
		{
			pElement = getElement( i );

			Info.m_Mass = pElement->getMass();
			Info.m_CenterPos.x = i3Vector::GetX( pElement->getCenterPosition() );
			Info.m_CenterPos.y = i3Vector::GetY( pElement->getCenterPosition() );
			Info.m_CenterPos.z = i3Vector::GetZ( pElement->getCenterPosition() );
			Info.m_Inertia.x = i3Vector::GetX( pElement->getInertia() );
			Info.m_Inertia.y = i3Vector::GetY( pElement->getInertia() );
			Info.m_Inertia.z = i3Vector::GetZ( pElement->getInertia() );

			Rc = pStream->Write( &Info, sizeof( Info ) );
			if( Rc == STREAM_ERR )
			{
				I3FATAL( "Could not write rigidbody element info.\n" );
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3Rigidbody::OnLoad( i3ResourceFile * pResFile )
{	
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	{
		I3PSQ_PERSIST_RIGIDBODY Info;

		Rc = pStream->Read( &Info, sizeof( Info ) );
		if( Rc == STREAM_ERR )
		{
			I3FATAL( "Could not read rigidbody info.\n" );
			return STREAM_ERR;
		}
		Result += Rc;

		setType( Info.m_Type );
		setStyle( Info.m_Style );
		setMass( Info.m_Mass );

		i3Matrix::SetRight( &m_Inertia, Info.m_Inertia.mat[0].x, Info.m_Inertia.mat[0].y,
			Info.m_Inertia.mat[0].z, Info.m_Inertia.mat[0].w );
		i3Matrix::SetUp( &m_Inertia, Info.m_Inertia.mat[1].x, Info.m_Inertia.mat[1].y,
			Info.m_Inertia.mat[1].z, Info.m_Inertia.mat[1].w );
		i3Matrix::SetAt( &m_Inertia, Info.m_Inertia.mat[2].x, Info.m_Inertia.mat[2].y,
			Info.m_Inertia.mat[2].z, Info.m_Inertia.mat[2].w );
		i3Matrix::SetPos( &m_Inertia, Info.m_Inertia.mat[3].x, Info.m_Inertia.mat[3].y,
			Info.m_Inertia.mat[3].z, Info.m_Inertia.mat[3].w );

		i3Vector::Set( &m_Gravity, Info.m_Gravity.x, Info.m_Gravity.y, Info.m_Gravity.z );
		setDrag( m_DragFactor );
		i3Vector::Set( &m_Wind, Info.m_Wind.x, Info.m_Wind.y, Info.m_Wind.z );

		{
			I3PSQ_PERSIST_RIGIDBODYELEMENT * pElement;
			
			pElement = (I3PSQ_PERSIST_RIGIDBODYELEMENT *)i3MemAlloc( Info.m_ElementCount * sizeof(I3PSQ_PERSIST_RIGIDBODYELEMENT) );
		
			Rc = pStream->Read( pElement, Info.m_ElementCount * sizeof(I3PSQ_PERSIST_RIGIDBODYELEMENT) );
			if( Rc == STREAM_ERR )
			{
				I3FATAL( "Could not read rigidbody element info. \n" );
				return STREAM_ERR;
			}

			BuildElement( pElement, Info.m_ElementCount );

			Result += Rc;

			i3MemFree( pElement );
		}
	}

	return Result;
}

// Collision test
void i3Rigidbody_CollisionSolve( i3Rigidbody * pBodyA, i3Rigidbody * pBodyB )
{
	
}

// 2006. 1. 19. by dfly79
// Colliding factor
// pCollPoint : 접점
// pNor : 접점에서의 법선 벡터
// elast : 반발력
void i3Rigidbody_ProcessCollidingContact( i3Rigidbody * pBodyA, i3Rigidbody * pBodyB , RT_VEC3D * pCollPoint, RT_VEC3D * pNor, RT_REAL32 elast )
{
	REAL32 fNum, fDenom, f;
	VEC3D rA, rB;
	VEC3D kA, kB;
	VEC3D uA, uB; // Tension
	VEC3D Vec;
	VEC3D impulse;

	I3ASSERT( pBodyA != NULL );
	I3ASSERT( pBodyB != NULL );
	I3ASSERT( pCollPoint != NULL );
	I3ASSERT( pNor != NULL );

	// 충격량 계산
	i3Vector::Sub( &rA, pCollPoint, pBodyA->getPos() );
	i3Vector::Sub( &rB, pCollPoint, pBodyB->getPos() );

	// 모멘트 축 산출
	i3Vector::Cross( &kA, &rA, pNor );
	i3Vector::Cross( &kB, &rB, pNor );

	i3Vector::TransformCoord( &uA, &kA, pBodyA->getInvInertia() );
	i3Vector::TransformCoord( &uB, &kB, pBodyB->getInvInertia() );

	{
		VEC3D vS;
		REAL32 Do1, Do2, Do3;

		i3Vector::Sub( &vS, pBodyA->getVelocity(), pBodyB->getVelocity() );
		Do1 = i3Vector::Dot( pNor, &vS );
		Do2 = i3Vector::Dot( pBodyA->getAngular(), &kA );
		Do3 = i3Vector::Dot( pBodyB->getAngular(), &kB );
		fNum = -i3Math::Mul( (__RT(1.0f) + elast), Do1 ) + Do2 - Do3;

		Do1 = i3Vector::Dot( &kA, &uA );
		Do2 = i3Vector::Dot( &kB, &uB );
		fDenom = pBodyA->getInvMass() + pBodyB->getInvMass() + Do1 + Do2;
		f = i3Math::div( fNum, fDenom );		// 
		i3Vector::Scale( &impulse, pNor, f);	// 텐서에 대한 반사힘 계산
	}

	// 물리량 계산
	// 반발력을 추가한다.
	i3Vector::Add( pBodyA->getForce(), pBodyA->getForce(), &impulse );
	i3Vector::Sub( pBodyB->getForce(), pBodyB->getForce(), &impulse );

	// 모멘트를 갱신한다.
	i3Vector::Scale( &Vec, &kA, f);
	i3Vector::Add( pBodyA->getMoment(), pBodyA->getMoment(), &Vec );
	i3Vector::Scale( &Vec, &kB, f);
	i3Vector::Sub( pBodyB->getMoment(), pBodyB->getMoment(), &Vec );

	// 선/각속도 갱신
	i3Vector::Scale( pBodyA->getVelocity(), pBodyA->getForce(), pBodyA->getInvMass());
	i3Vector::Scale( &Vec, &uA, f);
	i3Vector::Add( pBodyA->getAngular(), pBodyA->getAngular(), &Vec );

	i3Vector::Scale( pBodyB->getVelocity(), pBodyB->getForce(), pBodyB->getInvMass());
	i3Vector::Scale( &Vec, &uB, f);
	i3Vector::Add( pBodyB->getAngular(), pBodyB->getAngular(), &Vec );
}
