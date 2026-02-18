#include "i3SceneDef.h"
#include "i3NavigationPointSlerp.h"

I3_CLASS_INSTANCE( i3NavigationPointSlerp);

i3NavigationPointSlerp::~i3NavigationPointSlerp()
{
	_RemoveAllWayPoint();
}

void i3NavigationPointSlerp::_RemoveAllWayPoint()
{
	VEC3D * pVec;

	for(size_t i = 0; i < m_WayPointList.size(); i++ )
	{
		pVec = m_WayPointList[ i ];
		I3MEM_SAFE_FREE_POINTER( pVec );
	}

	m_WayPointList.clear();
}

void i3NavigationPointSlerp::Create( UINT32 nType, REAL32 fRadius, i3CollisionContext * pCollisionContext, UINT32 nWorldGroupIndex )
{
	m_nType					= nType;
	m_fBoundSphereRadius	= fRadius + 1.0f;
	
	m_pCollisionContext		= pCollisionContext;
	m_nCollisionWorldGroupIndex = nWorldGroupIndex;
}

void i3NavigationPointSlerp::Create( VEC3D * pStart, VEC3D * pTarget, UINT32 nType )
{
	
}

void i3NavigationPointSlerp::_calcDirection()
{
	VEC3D * pSt;
	VEC3D * pDt;

	if( m_nCurrentWayPoint < (INT32)m_WayPointList.size() - 1)
	{
		pSt = m_WayPointList[ m_nCurrentWayPoint ];
		pDt = m_WayPointList[ m_nCurrentWayPoint + 1 ];
			
		i3Vector::Sub( &m_vecDirection, pSt, pDt );
		//i3Vector::SetZ( &m_vecDirection, i3Vector::GetZ( &m_vecDirection ) );
		m_fLength = i3Vector::Normalize( &m_vecDirection, &m_vecDirection );
	}
}

void i3NavigationPointSlerp::_CalcCost( VEC3D * pStart, VEC3D * pTarget )
{
	VEC3D VecSub;

	i3Vector::Sub( &VecSub, pTarget, pStart );

	m_fHeuristicCost += i3Vector::Length( &VecSub );
}

void i3NavigationPointSlerp::_CalcWayPoint( i3CollideeLine * pLine, I3_WAYPOINTSLERP * pSlerpData, VEC3D * pNextPosition, INT32 nCount )
{	
	_CalcCost( getStartPos(), pNextPosition );
	nCount--;

	if( nCount == 0 )
		return;
	pSlerpData->m_nCount++;
	pSlerpData->m_fHeuristicCost += getHeuristicCost();

	if( pSlerpData->m_pPosition )
		pSlerpData->m_pPosition = (VEC3D *) i3MemReAlloc( pSlerpData->m_pPosition, sizeof(VEC3D) * pSlerpData->m_nCount );
	else
		pSlerpData->m_pPosition = (VEC3D *) i3MemAlloc( sizeof(VEC3D) );
	I3ASSERT( pSlerpData->m_pPosition );
	i3Vector::Copy( &pSlerpData->m_pPosition[ pSlerpData->m_nCount - 1 ], pNextPosition );

	pLine->SetStart( pNextPosition + 1 );

	if( m_pCollisionContext->CollisionTest( pLine, m_nCollisionWorldGroupIndex, 0 ) )
		_CalcWayPoint( pLine, pSlerpData, pNextPosition + 1, nCount-1);
}

bool i3NavigationPointSlerp::IsDone()
{
	if( m_nCurrentWayPoint + 1 >= (INT32)m_WayPointList.size() )
		return true;
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//성백 임시 코드 입니다. 
void i3NavigationPointSlerp::FindWayPoint( VEC3D * pStart, VEC3D * pTarget, REAL32 fBoundingSphere )
{
	VEC3D * pVec;

	if( m_WayPointList.size() > 0)
	{
		I3ASSERT_0; 
	}

	setStartPos( pStart );
	setTargetPos( pTarget );

	// 시작 포인트의 좌표를 넣어준다.
	pVec = (VEC3D *) i3MemAlloc( sizeof( VEC3D ) );
	i3Vector::Copy( pVec, pStart );
	m_WayPointList.push_back( pVec );

	// 마지막 목표점의 좌표를 넣어준다.
	pVec = (VEC3D *) i3MemAlloc( sizeof( VEC3D ) );
	i3Vector::Copy( pVec, pTarget );
	m_WayPointList.push_back( pVec );

	// 한번은 방향을 설정해야된다.
	setCurrentWayPoint(0);
 	_calcDirection();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 순구씨 코드 입니다. 차후 이소스로 변경합니다. 
//void i3NavigationPointSlerp::FindWayPoint( VEC3D * pStart, VEC3D * pTarget, REAL32 fBoundingSphere )
//{
//	INT32	i, j, k;
//	i3CollideeMesh * pCollideeMesh;
//	i3Collider * pCollider;
//	i3ColliderSet * pColliderSet;
//	i3CollideeLine * pLine;
//	I3_COLLISION_RESULT * pResult;
//
//	I3_WAYPOINTSLERP * pWayPoint;
//	
//	VEC3D 	vecBoundBox[4];
//	VEC3D	vecBoundBox2[4];
//	VEC3D * pVec;
//	VEC3D 	MeshCenter;
//	VEC3D	vecTemp;
//
//	REAL32	minX, minZ;
//	REAL32	maxX, maxZ;
//	REAL32	minCost = 10000.0f;
//	REAL32	maxCost = -100.0f;
//
//	INT32	nCutWay = 0;
//	UINT32	nResultCount;
//
//	setStartPos( pStart );
//	setTargetPos( pTarget );
//
//	minX = minZ = 1000.0f;
//	maxX = maxZ = -1000.0f;
//
//	// 시작 포인트의 좌표를 넣어준다.
//	pVec = (VEC3D *) i3MemAlloc( sizeof( VEC3D ) );
//	i3Vector::Copy( pVec, pStart );
//	m_WayPointList.Add( pVec );
//
//	// 컬리젼 라인 생성
//	pLine = i3CollideeLine::new_object();
//	pLine->SetStart( pVec );
//	pLine->SetEnd( pTarget );
//
//	// 해당 벡터에 컬리젼 테스트
//	I3ASSERT( m_pCollisionContext );
//	nResultCount = m_pCollisionContext->CollisionTest( pLine, m_nCollisionWorldGroupIndex, 0 );
//	pResult  = m_pCollisionContext->GetResult();
//	
//	nResultCount = 0;
//	for( i = 0; i < (INT32) nResultCount; i++ )
//	{	
//		// 컬리젼 박스의 외곽 충돌점의 최소 최대에 대한 루트를 만들어 비용을 계산한다.
//		pColliderSet = pResult[ i ].m_pDestColliderSet;
//
//		if( pColliderSet == nullptr )
//			continue;
//		
//		for( j = 0; j < pColliderSet->GetColliderCount(); j++ )
//		{
//			pCollider = pColliderSet->GetCollider( j );
//
//			//I3TRACE( "CollideeType - %d\n", pCollider->GetType() );
//
//			if( ( pCollider->GetType() & I3_COLLIDER_MESH ) )
//			{	
//				// 월드 바운드 박스
//				pCollideeMesh = (i3CollideeMesh *) pCollider->GetTransformedCollidee();
//				i3Vector::Set( &MeshCenter, i3Vector::GetX( pCollideeMesh->GetCenter() ), 0.0f, i3Vector::GetZ( pCollideeMesh->GetCenter() ) );
//				//I3TRACE( "CollideeMesh : Name %s - center ( %.2f, %.2f, %.2f )\t", pColliderSet->GetName(),
//				//	i3Vector::GetX( pCollideeMesh->GetCenter() ), i3Vector::GetY( pCollideeMesh->GetCenter() ), i3Vector::GetZ( pCollideeMesh->GetCenter() ) );
//
//				//if( pCollideeMesh->GetPositionCount() < 5 )
//				//	goto SKIP_COLLIDEE;
//
//				//I3TRACE( "Count : %d", pCollideeMesh->GetPositionCount() );
//
//				for( k = 0; k < (INT32) pCollideeMesh->GetPositionCount(); k++ )
//				{
//					if( i3Vector::GetY( pCollideeMesh->GetPosition( k ) ) > i3Vector::GetY( getStartPos() ) + 1.0f )
//						continue;
//
//					/*
//					I3TRACE( "( %.2f, %.2f, %.2f ), ", i3Vector::GetX( pCollideeMesh->GetPosition( k ) ),
//						i3Vector::GetY( pCollideeMesh->GetPosition( k ) ), i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) );
//*/
//					if( minX > i3Vector::GetX( pCollideeMesh->GetPosition( k ) ) )
//					{
//						minX = i3Vector::GetX( pCollideeMesh->GetPosition( k ) );
//						i3Vector::Set( &vecBoundBox[ 0 ], i3Vector::GetX( pCollideeMesh->GetPosition( k ) ), 1.0f,
//							i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) );
//
//						// 유닛의 바운딩스피어 만큼 떨어져야 된다.
//						i3Vector::Sub( &vecTemp, &MeshCenter, &vecBoundBox[ 0 ] );
//						i3Vector::Normalize( &vecTemp, &vecTemp );
//						i3Vector::Scale( &vecTemp, &vecTemp, m_fBoundSphereRadius );
//						i3Vector::Add( &vecBoundBox[ 0 ], &vecBoundBox[ 0 ], &vecTemp );
//
//					}
//					else if( maxX < i3Vector::GetX( pCollideeMesh->GetPosition( k ) ) )
//					{
//						maxX = i3Vector::GetX( pCollideeMesh->GetPosition( k ) );
//						i3Vector::Set( &vecBoundBox[ 1 ], i3Vector::GetX( pCollideeMesh->GetPosition( k ) ), 1.0f,
//							i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) );
//
//						// 유닛의 바운딩스피어 만큼 떨어져야 된다.
//						i3Vector::Sub( &vecTemp, &MeshCenter, &vecBoundBox[ 1 ] );
//						i3Vector::Normalize( &vecTemp, &vecTemp );
//						i3Vector::Scale( &vecTemp, &vecTemp, m_fBoundSphereRadius );
//						i3Vector::Add( &vecBoundBox[ 1 ], &vecBoundBox[ 1 ], &vecTemp );
//					}
//
//					if( minZ > i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) )
//					{
//						minZ = i3Vector::GetZ( pCollideeMesh->GetPosition( k ) );
//						i3Vector::Set( &vecBoundBox[ 2 ], i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ), 1.0f,
//							i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) );
//
//						i3Vector::Sub( &vecTemp, &MeshCenter, &vecBoundBox[ 2 ] );
//						i3Vector::Normalize( &vecTemp, &vecTemp );
//						i3Vector::Scale( &vecTemp, &vecTemp, m_fBoundSphereRadius );
//						i3Vector::Add( &vecBoundBox[ 2 ], &vecBoundBox[ 2 ], &vecTemp );
//					}
//					else if( maxZ < i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) )
//					{
//						maxZ = i3Vector::GetZ( pCollideeMesh->GetPosition( k ) );
//						i3Vector::Set( &vecBoundBox[ 3 ], i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ), 1.0f,
//							i3Vector::GetZ( pCollideeMesh->GetPosition( k ) ) );
//
//						i3Vector::Sub( &vecTemp, &MeshCenter, &vecBoundBox[ 3 ] );
//						i3Vector::Normalize( &vecTemp, &vecTemp );
//						i3Vector::Scale( &vecTemp, &vecTemp, m_fBoundSphereRadius );
//						i3Vector::Add( &vecBoundBox[ 3 ], &vecBoundBox[ 3 ], &vecTemp );
//					}
//				}
//				//I3TRACE("\n");
//			}
//			else if( pCollider->GetType() & I3_COLLIDER_SPHERE )
//			{
//				goto SKIP_COLLIDEE;
//			}
//		}
//
//		// 벽인지 체크
//		for( j = 0; j < 4; j++ )
//		{
//			for( k = 0; k < 4; k++ )
//			{
//				if( j != k )
//				{
//					if( i3Vector::IsSame( &vecBoundBox[ j ], &vecBoundBox[ k ] ) )
//					{
//						i3Vector::Set( pTarget, i3Vector::GetX( &pResult[ i ].m_Intersect ), 0.0f, i3Vector::GetZ( &pResult[ i ].m_Intersect ) );
//						goto EXIT_LAVEL;
//					}
//				}
//			}
//		}
//
//		// 컬리젼 메쉬의 외곽선을 따라 이동해본다.
//		{
//			pWayPoint = (I3_WAYPOINTSLERP *) i3MemAlloc( sizeof(I3_WAYPOINTSLERP) );
//			I3ASSERT( pWayPoint );
//			 
//			_CalcWayPoint( pLine, pWayPoint, vecBoundBox, 3 );
//			m_tempWayPoint.Add( pWayPoint );
//		}
//		
//		if( m_nType & I3_NAVPOINT_FINDTYPE_FASTMODE )
//		{
//			// 한번만 검사..
//			goto EXIT_LAVEL;
//		}
//
//		{
//			for( j = 0; j < 4; j++ )
//				i3Vector::Copy( &vecBoundBox2[j], &vecBoundBox[3 - j] );
//			pWayPoint = (I3_WAYPOINTSLERP *) i3MemAlloc( sizeof(I3_WAYPOINTSLERP) );
//			I3ASSERT( pWayPoint );
//
//			_CalcWayPoint( pLine, pWayPoint, vecBoundBox2, 3 );
//			m_tempWayPoint.Add( pWayPoint );
//		}
//		{
//			for( j = 0; j < 4; j++ )
//			{
//				if( j == 3 )
//					i3Vector::Copy( &vecBoundBox2[j], &vecBoundBox[0] );
//				else
//					i3Vector::Copy( &vecBoundBox2[j], &vecBoundBox[j+1] );
//			}
//			pWayPoint = (I3_WAYPOINTSLERP *) i3MemAlloc( sizeof(I3_WAYPOINTSLERP) );
//			I3ASSERT( pWayPoint );
//
//			_CalcWayPoint( pLine, pWayPoint, vecBoundBox2, 3 );
//			m_tempWayPoint.Add( pWayPoint );
//		}
//		{
//		}
//
//		// 코스트 비교
//		for( j = 0; j < m_tempWayPoint.GetCount(); j++ )
//		{
//			pWayPoint = (I3_WAYPOINTSLERP *) m_tempWayPoint.Items[ j ];
//			
//			if( minCost > pWayPoint->m_fHeuristicCost && (m_nType & I3_NAVPOINT_FINDTYPE_SHORTCUT) )
//			{
//				// 짧은 길을 찾는다.
//				nCutWay = j;
//				minCost = pWayPoint->m_fHeuristicCost;
//			}
//			else if( maxCost < pWayPoint->m_fHeuristicCost && (m_nType & I3_NAVPOINT_FINDTYPE_LONGAWAY) ) 
//			{
//				// 돌아가는 경우
//				nCutWay = j;
//				maxCost = pWayPoint->m_fHeuristicCost;
//			}
//		}
//
//		// 찾은 길에 대한 웨이포인트 세팅
//		pWayPoint = (I3_WAYPOINTSLERP *) m_tempWayPoint.Items[ nCutWay ];
//
//		for( j = 0; j < pWayPoint->m_nCount; j++ )
//		{
//			pVec = (VEC3D *) i3MemAlloc( sizeof(VEC3D) );
//			i3Vector::Copy( pVec, &pWayPoint->m_pPosition[ j ] );
//			m_WayPointList.Add( pVec );
//		}
//
//		for( j = 0; j < m_tempWayPoint.GetCount(); j++ )
//		{
//			pWayPoint = (I3_WAYPOINTSLERP *)m_tempWayPoint.Items[ j ];
//			I3MEM_SAFE_FREE_POINTER( pWayPoint->m_pPosition );
//		}
//		m_tempWayPoint.Clear();
//
//SKIP_COLLIDEE:
//
//		pResult++;
//	}
//
//	goto EXIT_LAVEL;
//
//EXIT_LAVEL:
//	// 마지막 목표점의 좌표를 넣어준다.
//	pVec = (VEC3D *) i3MemAlloc( sizeof( VEC3D ) );
//	i3Vector::Copy( pVec, pTarget );
//	m_WayPointList.Add( pVec );
///*
//	I3TRACE( "WayPoint\t-\n" );
//	for( i = 0; i < m_WayPointList.GetCount(); i++ )
//	{
//		pVec = (VEC3D *) m_WayPointList.Items[ i ];
//		I3TRACE( "%d - ( %.2f, %.2f, %.2f )\n", i, i3Vector::GetX( pVec ), i3Vector::GetY( pVec ), i3Vector::GetZ( pVec ) );
//	}
//*/
//	// 한번은 방향을 설정해야된다.
//	setCurrentWayPoint( 0 );
//	_calcDirection();
//
//	I3_SAFE_RELEASE( pLine );
//}

void i3NavigationPointSlerp::AddWayPoint( VEC3D * pTarget )
{
}

VEC3D * i3NavigationPointSlerp::CalcCurrentPos( REAL32 fmoveLength )
{
	m_fSlerp += fmoveLength;

	if( m_fSlerp >= m_fLength )
	{	
		m_nCurrentWayPoint++;

		if( IsDone() )
		{
			_RemoveAllWayPoint();
			m_fSlerp = 0.0f;

			return getCurrentPos();
		}

		_calcDirection();
		m_fSlerp = 0.0f;
		i3Vector::Copy( &m_vecCurrentPos, m_WayPointList[ m_nCurrentWayPoint ] );
	}
	else
	{	
		i3Vector::Lerp( getCurrentPos(), m_WayPointList[ m_nCurrentWayPoint ], m_WayPointList[ m_nCurrentWayPoint + 1 ], i3Math::div( m_fSlerp, m_fLength ) );
	}

	return getCurrentPos();
}

void i3NavigationPointSlerp::CalcMovement( VEC3D * pCurrentPos )
{	
	VEC3D	Vec;
	VEC3D * pVec;

	if( m_WayPointList.size() )
	{
		pVec = m_WayPointList[ m_nCurrentWayPoint ];

		i3Vector::Sub( &Vec, pVec, pCurrentPos );
		m_fSlerp = i3Vector::Normalize( &Vec, &Vec );

		CalcCurrentPos( 0.0f );
	}
}

void i3NavigationPointSlerp::ClearPath()
{
	_RemoveAllWayPoint();

	i3Vector::Set( &m_vecCurrentPos, 0.0f, 0.0f, 0.0f );

	m_fSlerp				= 0.0f;
	m_nCurrentWayPoint		= 0;
}
