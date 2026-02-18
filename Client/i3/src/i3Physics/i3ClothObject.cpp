#include "i3PhysicsType.h"
#include "i3ClothObject.h"

I3_CLASS_INSTANCE( i3ClothObject, i3NamedElement );

i3ClothObject::i3ClothObject(void)
{
	m_pGeoAttr = NULL;
	m_pCollisionContext = NULL;

	i3Vector::Set( &m_WindFactor, __RT(-3.0f), __RT_0, __RT_0 );
	m_Mass = __RT_0;
	i3Vector::Set( &m_Gravity, __RT_0, __RT_0, __RT_0 );
	m_DragFactor = __RT(0.4f);
	m_Time = __RT_0;

	m_ParticleGroupCount = 2;

	m_pIndexTable = NULL;

	m_Style |= I3_PHYSICS_INTEGRATE_EULER;
}

i3ClothObject::~i3ClothObject(void)
{
	RemoveAllParticles();
	RemoveAllSprings();
	//_RemoveAllAcumParticles();

	I3_SAFE_RELEASE( m_pIndexTable);
}

void i3ClothObject::_RemoveAllAcumParticles(void)
{
	INT32 i;
	i3PhysicsParticle * pObj;

	for( i = 0; i < m_CurParticleList.GetCount(); i++ )
	{
		pObj = (i3PhysicsParticle *)m_CurParticleList.Items[i];

		pObj->Release();
	}
	m_CurParticleList.Clear();

/*
	for( i = 0; i < m_AcumParticleList0.GetCount(); i++ )
	{
		pObj = (i3PhysicsParticle *)m_AcumParticleList0.Items[i];

		pObj->Release();
	}
	for( i = 0; i < m_AcumParticleList1.GetCount(); i++ )
	{
		pObj = (i3PhysicsParticle *) m_AcumParticleList1.Items[i];
		pObj->Release();
	}
	for( i = 0; i < m_AcumParticleList2.GetCount(); i++ )
	{
		pObj = (i3PhysicsParticle *) m_AcumParticleList2.Items[i];
		pObj->Release();
	}
	for( i = 0; i < m_AcumParticleList3.GetCount(); i++ )
	{
		pObj = (i3PhysicsParticle *) m_AcumParticleList3.Items[i];
		pObj->Release();
	}

	m_AcumParticleList0.Clear();
	m_AcumParticleList1.Clear();
	m_AcumParticleList2.Clear();
	m_AcumParticleList3.Clear();
	*/
}

void i3ClothObject::RemoveAllParticles(void)
{
	INT32 i;
	i3PhysicsParticle * pObj;

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getParticle( i);

		pObj->Release();
	}

	m_ParticleList.Clear();
}

void i3ClothObject::RemoveAllSprings(void)
{
	INT32 i;
	i3Spring * pObj;

	for( i = 0; i < getSpringCount(); i++)
	{
		pObj = getSpring( i);

		pObj->Release();
	}

	m_SpringList.Clear();
}

i3PhysicsParticle * i3ClothObject::_FindParticleByVertexIndex( INT32 idx)
{
	INT32 i, j;
	i3PhysicsParticle * pObj;

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getParticle( i);

		// Table Index에 대한 순환 
		for( j = pObj->getTableStartIndex(); j <= pObj->getTableEndIndex(); j++)
		{
			// Index Table에서 실제 Vertex Index를 구해 온다.
			if( m_pIndexTable->getIndex( j) == idx)
				return pObj;
		}
	}

	return NULL;
}

// Geometry Setting
void i3ClothObject::SetGeometryAttr( i3GeometryAttr * pGeoAttr )
{
	if( pGeoAttr != NULL)
		pGeoAttr->AddRef();

	I3_SAFE_RELEASE( m_pGeoAttr);

	m_pGeoAttr = pGeoAttr;
}

void i3ClothObject::SetIndexTable( i3PHSParticleIdxTable * pTable)
{
	if( pTable != NULL)
		pTable->AddRef();

	I3_SAFE_RELEASE( m_pIndexTable);

	m_pIndexTable = pTable;
}

void i3ClothObject::AddAcumParticle( INT32 num, i3PhysicsParticle * pParticle )
{
	/*
	pParticle->AddRef();
	switch( num )
	{
	case 0:
		m_AcumParticleList0.Add( pParticle );
		break;
	case 1:
		m_AcumParticleList1.Add( pParticle );
		break;
	case 2:
		m_AcumParticleList2.Add( pParticle );
		break;
	case 3:
		m_AcumParticleList3.Add( pParticle );
		break;
	default:		break;
	}*/
}

void i3ClothObject::AddParticle( i3PhysicsParticle * pParticle)
{
	i3PhysicsParticle * pObj;

	pParticle->AddRef();

	m_ParticleList.Add( pParticle);

	{	
		pObj = i3PhysicsParticle::NewObjectRef();
		pObj->AddRef();
		m_CurParticleList.Add( pObj );
/*
		pObj = i3PhysicsParticle::NewObjectRef();
		AddAcumParticle( 0, pObj );
		pObj = i3PhysicsParticle::NewObjectRef();
		AddAcumParticle( 1, pObj );
		pObj = i3PhysicsParticle::NewObjectRef();
		AddAcumParticle( 2, pObj );
		pObj = i3PhysicsParticle::NewObjectRef();
		AddAcumParticle( 3, pObj );
		*/
	}
}

void i3ClothObject::AddSpring( i3Spring * pSpring)
{
	pSpring->AddRef();

	m_SpringList.Add( pSpring);
}

void i3ClothObject::BuildParticle( I3PSQ_PERSIST_CLOTH_PARTICLE * pParticle, INT32 Count )
{
	INT32 i;
	i3PhysicsParticle * pObj;

	RemoveAllParticles();
	_RemoveAllAcumParticles();

	setStyle( I3_PHYSICS_INTEGRATE_EULER );

	// Particle Object Creation
	for( i = 0; i < Count; i++)
	{
		{
			pObj = i3PhysicsParticle::NewObjectRef();

			pObj->setStyle( pParticle[i].m_Style );

			pObj->setTableStartIndex( pParticle[i].m_VIndexStart );
			pObj->setTableEndIndex( pParticle[i].m_VIndexEnd );

			pObj->setMass( pParticle[i].m_Mass );
			pObj->setRestitution( pParticle[i].m_Restitution );
			pObj->setFriction( pParticle[i].m_Friction );

			AddParticle( pObj);
		}
	}
}

void i3ClothObject::BuildSpring( I3PSQ_PERSIST_CLOTH_SPRING * pInfo, INT32 Count )
{
	INT32 i;
	i3Spring * pSpring;
	i3PhysicsParticle * pObj1, *pObj2;

	RemoveAllSprings();

	for( i = 0; i < Count; i++)
	{
		pSpring = i3Spring::NewObjectRef();

		pSpring->setType( pInfo[i].m_Type );

		pSpring->setDamp( pInfo[i].m_DampConstant );
		pSpring->setConstant( pInfo[i].m_Constant );
		pSpring->setLength( pInfo[i].m_RestLength );

		pObj1 = _FindParticleByVertexIndex( pInfo[i].m_Idx.m_Index0);
		I3ASSERT( pObj1 != NULL);

		pObj2 = _FindParticleByVertexIndex( pInfo[i].m_Idx.m_Index1);
		I3ASSERT( pObj2 != NULL);

		pSpring->setParticles( pObj1, pObj2);

		AddSpring( pSpring);
	}
}

void i3ClothObject::ResetParticle(void)
{
	INT32 i, vidx;
	i3VertexArray * pVA;
	i3PhysicsParticle * pObj;
	RT_VEC3D	pos;
	RT_REAL32 m = __RT_0;

	I3ASSERT( m_pGeoAttr != NULL);

	pVA = m_pGeoAttr->GetVertexArray();
	I3ASSERT( pVA != NULL);

	pVA->Lock();

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getParticle( i);

		vidx = m_pIndexTable->getIndex( pObj->getTableStartIndex());
		pVA->GetPosition( vidx, &pos);

		pObj->setPos( &pos);
	}

	pVA->Unlock();

	if( m_Style & I3_PHYSICS_UNIFORMED_MASS )
		m = i3Math::div( m_Mass, __RT( (REAL32)getParticleCount() ) );

	// 물리량 세팅
	for( i = 0; i < getParticleCount(); i++ )
	{
		pObj = getParticle( i );

		pObj->addStyle( I3_PHYSICS_PARAM_GRAVITY );
		pObj->addStyle( I3_PHYSICS_DRAG_AIR );
		pObj->addStyle( I3_PHYSICS_PARAM_WINDFACTOR );

		pObj->setGravity( &m_Gravity );
		pObj->setWind( &m_WindFactor );

		if( m_Style & I3_PHYSICS_UNIFORMED_MASS )
			pObj->setMass( m );

		pObj->setDragFactor( m_DragFactor );
	}
}

i3PhysicsParticle * i3ClothObject::_FindParticleByGroupIndex( INT32 idx )
{
	INT32 i;
	i3PhysicsParticle * pParticle = NULL;


	for( i = 0; i < getParticleCount(); i++)
	{
		pParticle = getParticle( i );

		if( pParticle->getGroupIndex() == idx )
			return pParticle;
	}

	return NULL;
}

// 모든 힘을 계산한다.
void i3ClothObject::_CalcForces( )
{
	INT32 i, j;
	i3PhysicsParticle * pParticle;
	i3PhysicsParticle * pGroup;

	// 힘 계산을 그룹으로 관리한다.
	for( i = 0; i < getGroupCount(); i++ )
	{
		pGroup = _FindParticleByGroupIndex( i );

		pGroup->CalcForce();
		for( j = 0; j < getParticleCount(); j++ )
		{
			pParticle = getParticle( j );

			if( pParticle->getGroupIndex() == i )
				pParticle->setForce( pGroup->getForce() );
		}
	}

	// 변화된 파티클에 대한 스프링 힘 처리
	for( i = 0; i < getSpringCount(); i++)
	{	
		i3Spring *pSpring = getSpring(i);
		
		pSpring->CalcSimulation();
	}
}

void i3ClothObject::StepSimulation( RT_REAL32 delta )
{
	INT32 i;
	RT_REAL32 halfDT;//, sixthDT;
	//RT_REAL32 f;
	//RT_VEC3D Vec0, Vec1, Vec2;
	i3PhysicsParticle * pParticle;
	i3PhysicsParticle * pCurrentParticle;
	//i3PhysicsParticle * pAccumParticle0;
	//i3PhysicsParticle * pAccumParticle1;
	//i3PhysicsParticle * pAccumParticle2;
	//i3PhysicsParticle * pAccumParticle3;
	
	m_Time = delta;

	// 힘을 계산한다.
	_CalcForces( );
	
	// 적분한다.
	if( m_Style & I3_PHYSICS_INTEGRATE_EULER )
	{
		// 율러 계산
		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );

			i3PhysicsParticle_CalcIntegrateOverTime( pParticle, pParticle, pParticle, m_Time );
		}
	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_MIDP )
	{
		// 중간점 계산
		halfDT = i3Math::Mul( delta, __RT( 0.5f ) );

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle(i);
			{	
				pCurrentParticle->setVelocity( pParticle->getVelocity() );
				pCurrentParticle->setPos( pParticle->getPos() );
				pCurrentParticle->setInvMass( pParticle->getInvMass() );
			}
			i3PhysicsParticle_CalcIntegrateOverTime( pParticle, pParticle, pParticle, halfDT );
		}

		_CalcForces( );
		
		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle( i );
			i3PhysicsParticle_CalcIntegrateOverTime( pCurrentParticle, pParticle, pParticle, m_Time );
		}
	}
	else if( m_Style & I3_PHYSICS_INTEGRATE_RK4 )
	{
		/*
		halfDT = i3Math::Mul( delta, __RT( 0.5f ) );
		sixthDT = i3Math::div( delta, __RT( 6.0f ) );

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle(i);
			pAccumParticle0 = (i3PhysicsParticle*) m_AcumParticleList0.Items[i];
			{
				pCurrentParticle->setForce( pParticle->getForce() );
				pCurrentParticle->setVelocity( pParticle->getVelocity() );
				pCurrentParticle->setPos( pParticle->getPos() );
				pCurrentParticle->setInvMass( pParticle->getInvMass() );
			}
			i3PhysicsParticle_CalcRungeKutta_G( pCurrentParticle, pParticle, pParticle, pAccumParticle0, halfDT );
		}

		_CalcForces();

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle( i );
			pAccumParticle1 = (i3PhysicsParticle*) m_AcumParticleList1.Items[i];
			i3PhysicsParticle_CalcRungeKutta_G( pCurrentParticle, pParticle, pParticle, pAccumParticle1, halfDT );
		}

		_CalcForces();

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle( i );
			pAccumParticle2 = (i3PhysicsParticle*) m_AcumParticleList2.Items[i];
			i3PhysicsParticle_CalcRungeKutta_G( pCurrentParticle, pParticle, pParticle, pAccumParticle2, m_Time );
		}

		_CalcForces();

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle( i );
			pAccumParticle3 = (i3PhysicsParticle*) m_AcumParticleList3.Items[i];

			f = i3Math::Mul( m_Time, pParticle->getInvMass() );
			i3Vector::Scale( pAccumParticle3->getForce(), pParticle->getForce(), f );

			i3Vector::Scale( pAccumParticle3->getVelocity(), pParticle->getVelocity(), m_Time );
		}

		for( i = 0; i < getParticleCount(); i++ )
		{
			pParticle = getParticle( i );
			pCurrentParticle = getCurParticle( i );
			pAccumParticle0 = (i3PhysicsParticle*) m_AcumParticleList0.Items[i];
			pAccumParticle1 = (i3PhysicsParticle*) m_AcumParticleList1.Items[i];
			pAccumParticle2 = (i3PhysicsParticle*) m_AcumParticleList2.Items[i];
			pAccumParticle3 = (i3PhysicsParticle*) m_AcumParticleList3.Items[i];

			i3Vector::Add( &Vec0, pAccumParticle1->getForce(), pAccumParticle2->getForce() );
			i3Vector::Scale( &Vec0, &Vec0, __RT(2.0f) );
			i3Vector::Add( &Vec1, pAccumParticle0->getForce(), pAccumParticle3->getForce() );
			i3Vector::Add( &Vec1, &Vec0, &Vec1);
			i3Vector::Scale( &Vec2, &Vec1, sixthDT );
			i3Vector::Add( pParticle->getVelocity(), pCurrentParticle->getVelocity(), &Vec2 );
			
			// DETERMINE THE NEW POSITION FOR THE PARTICLE USING RK4 FORMULA
			i3Vector::Add( &Vec0, pAccumParticle1->getVelocity(), pAccumParticle2->getVelocity() );
			i3Vector::Scale( &Vec0, &Vec0, __RT(2.0f) );
			i3Vector::Add( &Vec1, pAccumParticle0->getVelocity(), pAccumParticle3->getVelocity() );
			i3Vector::Add( &Vec1, &Vec0, &Vec1 );
			i3Vector::Scale( &Vec2, &Vec1, sixthDT );
			i3Vector::Add( pParticle->getPos(), pCurrentParticle->getPos(), &Vec2 );
		}
		*/
	}
	
	// 충돌 처리
	for( i = 0; i < getParticleCount(); i++)
	{	
		pParticle = getParticle( i);
		pParticle->CheckCollision( );
	}
	
	// 버텍스 업데이트
	UpdateClothGeometry( );
}

void i3ClothObject::UpdateClothGeometry( void )
{
	INT32 i;
	i3PhysicsParticle * pParticle;
	RT_VEC3D Vec;

	for( i = 0; i < getParticleCount(); i++)
	{
		pParticle = getParticle( i);

		i3Vector::Scale( &Vec, pParticle->getVelocity(), m_Time );

		i3Vector::Add( pParticle->getPos(), pParticle->getPos(), &Vec );
	}
}

void i3ClothObject::OnBuildObjectList( i3List * pList)
{
	if( m_pGeoAttr != NULL)
	{
		m_pGeoAttr->OnBuildObjectList( pList);
	}

	if( m_pIndexTable != NULL)
		m_pIndexTable->OnBuildObjectList( pList);

	i3NamedElement::OnBuildObjectList( pList);
}

UINT32 i3ClothObject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	i3Stream * pStream = pResFile->GetStream();

	// 클로스 오브젝트의 정보를 저장합니다.
	{
		I3PSQ_PERSIST_CLOTH_OBJECT Info;

		memset( &Info, 0, sizeof(Info));

		memcpy( Info.m_ID, "CTH1", 4);
		Info.m_Style				= getStyle();
		Info.m_ParticleCount		= getParticleCount();
		Info.m_SpringCount			= getSpringCount();
		Info.m_pGeoAttr				= (OBJREF) m_pGeoAttr;
		Info.m_pIndexTable			= (OBJREF) m_pIndexTable;
		Info.m_Mass					= getMass();
		Info.m_WindFactor.x			= i3Vector::GetX( getWind());
		Info.m_WindFactor.y			= i3Vector::GetY( getWind());
		Info.m_WindFactor.z			= i3Vector::GetZ( getWind());
		Info.m_Gravity.x			= i3Vector::GetX( getGravity());
		Info.m_Gravity.y			= i3Vector::GetY( getGravity());
		Info.m_Gravity.z			= i3Vector::GetZ( getGravity());
		Info.m_DragFactor			= getDrag();

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write Cloth Object Info.\n");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Particle에 대한 정보를 저장합니다.
	{
		i3PhysicsParticle * pParticle;
		I3PSQ_PERSIST_CLOTH_PARTICLE Particle;
		
		for( i = 0; i < getParticleCount(); i++)
		{
			pParticle = getParticle( i );

			memset( &Particle, 0, sizeof( Particle ) );
			
			Particle.m_Style			= pParticle->getStyle();

			Particle.m_VIndexStart		= pParticle->getTableStartIndex();
			Particle.m_VIndexEnd		= pParticle->getTableEndIndex();
			
			Particle.m_Mass				= pParticle->getMass();
			Particle.m_Restitution		= pParticle->getRestitution();
			Particle.m_Friction			= pParticle->getFriction();
			
			Rc = pStream->Write( &Particle, sizeof(Particle));
			if( Rc == STREAM_ERR )
			{
				I3FATAL( "Could not write Cloth Particle data.\n" );
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Spring에 대한 정보를 저장합니다.
	{	
		I3PSQ_PERSIST_CLOTH_SPRING Spring;
		i3Spring * pSpring;

		for( i = 0; i < getSpringCount(); i++)
		{
			pSpring = getSpring( i);

			//
			memset( &Spring, 0, sizeof( Spring ) );

			Spring.m_Type					= pSpring->getType();
			
			// Index Table에서 실제 Vertex Index를 구해 온다.
			Spring.m_Idx.m_Index0 = m_pIndexTable->getIndex( pSpring->getParticle1()->getTableStartIndex() );
			Spring.m_Idx.m_Index1 = m_pIndexTable->getIndex( pSpring->getParticle2()->getTableStartIndex() );
			
			Spring.m_DampConstant			= pSpring->getDamp();
			Spring.m_Constant				= pSpring->getConstant();
			Spring.m_RestLength				= pSpring->getLength();

			Rc = pStream->Write( &Spring, sizeof(Spring));
			if( Rc == STREAM_ERR )
			{
				I3FATAL( "Could not write Cloth Spring data.\n" );
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3ClothObject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	//INT32 hMem;
	i3Stream * pStream = pResFile->GetStream();
	I3PSQ_PERSIST_CLOTH_OBJECT Info;
	i3GeometryAttr * pGeoAttr = NULL;
	RT_VEC3D vec;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not read Cloth Object Info.\n");
		return STREAM_ERR;
	}
	Result += Rc;

	// Geometry
	if( Info.m_pGeoAttr != 0)
	{
		// 함께 저장된 Geometry Attribte를 검색한다.
		pGeoAttr = (i3GeometryAttr *) pResFile->FindObjectByID( Info.m_pGeoAttr);
		I3ASSERT( pGeoAttr != NULL);

		SetGeometryAttr( pGeoAttr );
	}

	// Index Table
	if( Info.m_pIndexTable != 0)
	{
		i3PHSParticleIdxTable * pIndexTable;

		pIndexTable = (i3PHSParticleIdxTable *) pResFile->FindObjectByID( Info.m_pIndexTable);
		I3ASSERT( pIndexTable);

		SetIndexTable( pIndexTable);
	}

	// Style
	setStyle( Info.m_Style );

	// Mass
	setMass( __RT( Info.m_Mass));
	// Wind Factor
	{
		i3Vector::Set( &vec, __RT(Info.m_WindFactor.x), __RT(Info.m_WindFactor.y), __RT(Info.m_WindFactor.z));
		setWind( &vec);
	}
	// Gravity
	{
		i3Vector::Set( &vec, __RT( Info.m_Gravity.x), __RT( Info.m_Gravity.y), __RT( Info.m_Gravity.z));
		setGravity( &vec);
	}
	// Drag Factor
	setDrag( __RT( Info.m_DragFactor));

	// Particle에 대한 기본 정보를 우선 읽어 들인다.
	{	
		I3PSQ_PERSIST_CLOTH_PARTICLE * pParticle;

		//hMem = i3MemAlloc( Info.m_ParticleCount * sizeof(I3PSQ_PERSIST_CLOTH_PARTICLE) );
		pParticle = (I3PSQ_PERSIST_CLOTH_PARTICLE *)i3MemAlloc( Info.m_ParticleCount * sizeof(I3PSQ_PERSIST_CLOTH_PARTICLE) );

		Rc = pStream->Read( pParticle, Info.m_ParticleCount * sizeof(I3PSQ_PERSIST_CLOTH_PARTICLE) );
		if( Rc == STREAM_ERR )
		{
			I3FATAL( "Could not read cloth particle data.\n" );
			return STREAM_ERR;
		}

		BuildParticle( pParticle, Info.m_ParticleCount );

		//i3MemFreeByHandle( hMem );
		i3MemFree( pParticle ); 

		Result += Rc;
	}
	
	// Spring에 대한 정보를 세팅한다.
	{
		I3PSQ_PERSIST_CLOTH_SPRING * pSpring;

		//hMem = i3MemAlloc( Info.m_SpringCount * sizeof(I3PSQ_PERSIST_CLOTH_SPRING) );
		pSpring = (I3PSQ_PERSIST_CLOTH_SPRING *)i3MemAlloc( Info.m_SpringCount * sizeof(I3PSQ_PERSIST_CLOTH_SPRING) );

		Rc = pStream->Read( pSpring, Info.m_SpringCount * sizeof(I3PSQ_PERSIST_CLOTH_SPRING) );
		if( Rc == STREAM_ERR )
		{
			I3FATAL( "Could not read cloth spring data.\n" );
			return STREAM_ERR;
		}

		BuildSpring( pSpring, Info.m_SpringCount );

		//i3MemFreeByHandle( hMem );
		i3MemFree(pSpring); 

		Result += Rc;
	}

	return Result;
}
