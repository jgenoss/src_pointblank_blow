#include "i3OptPCH.h"
#include "i3GeometrySplitter.h"

I3_CLASS_INSTANCE( i3GeometrySplitter);

//#define				VLOG

i3GeometrySplitter::~i3GeometrySplitter(void)
{
	I3_SAFE_RELEASE( m_pPoolVA);
	I3_SAFE_RELEASE( m_pIncVA);
	I3_SAFE_RELEASE( m_pExcVA);
}

////////////////////////////////////////////////
// Planes
void i3GeometrySplitter::addPlane( VEC4D * pPlane)
{
	VEC4D * pNew;

	pNew = new VEC4D;
	NetworkDump_Malloc( pNew, sizeof(VEC4D), __FILE__, __LINE__);

	i3Vector::Copy( pNew, pPlane);

	m_PlaneList.push_back( pNew);
}

void i3GeometrySplitter::addPlane( i3BoundBox * pBox)
{
	VEC4D plane;
	VEC3D n;

	// Negative X plane
	i3Vector::Set( &n, 1.0f, 0.0f, 0.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMin(), &n);
	addPlane( &plane);

	// Positive X plane
	i3Vector::Set( &n, -1.0f, 0.0f, 0.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMax(), &n);
	addPlane( &plane);

	// Negative Y plane
	i3Vector::Set( &n, 0.0f, 1.0f, 0.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMin(), &n);
	addPlane( &plane);

	// Positive Y plane
	i3Vector::Set( &n, 0.0f, -1.0f, 0.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMax(), &n);
	addPlane( &plane);

	// Negative Z plane
	i3Vector::Set( &n, 0.0f, 0.0f, 1.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMin(), &n);
	addPlane( &plane);

	// Positive Z plane
	i3Vector::Set( &n, 0.0f, 0.0f, -1.0f);			// BoundBox의 안쪽으로 Normal이 설정되어야 한다.
	i3Plane::Set( &plane, pBox->GetMax(), &n);
	addPlane( &plane);
}

void i3GeometrySplitter::removeAllPlane(void)
{
	INT32 i;
	VEC4D * pPlane;

	for( i = 0; i < getPlaneCount(); i++)
	{
		pPlane = (VEC4D *) getPlane( i);

		delete pPlane;
	}

	m_PlaneList.clear();
}

////////////////////////////////////////////////
// Pools
INT32 i3GeometrySplitter::addPool( bool bAdd)
{
	INT32 idx = (m_PoolCount * 3);

	if( bAdd)
		m_PoolCount++;

	return idx;
}

void i3GeometrySplitter::setPoolVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1, REAL32 t)
{
	if( idx1 == -1)
		m_pPoolVA->CopyVertexFrom( idxDest, pVA, idx0);
	else
		m_pPoolVA->InterpolateVertex( idxDest, pVA, idx0, idx1, t);
}

void i3GeometrySplitter::getPool( INT32 idx, VEC3D * p)
{
	m_pPoolVA->GetPosition( idx + 0, p + 0);
	m_pPoolVA->GetPosition( idx + 1, p + 1);
	m_pPoolVA->GetPosition( idx + 2, p + 2);
}

////////////////////////////////////////////////
// Included
INT32 i3GeometrySplitter::addInc(void)
{
	INT32 idx = (m_IncCount * 3);

	m_IncCount++;

	return idx;
}

void i3GeometrySplitter::setIncVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1, REAL32 t)
{
	if( idx1 == -1)
		m_pIncVA->CopyVertexFrom( idxDest, pVA, idx0);
	else
		m_pIncVA->InterpolateVertex( idxDest, pVA, idx0, idx1, t);
}

////////////////////////////////////////////////
// Excluded
INT32 i3GeometrySplitter::addExc(void)
{
	INT32 idx = (m_ExcCount * 3);

	m_ExcCount++;

	return idx;
}

void i3GeometrySplitter::setExcVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1, REAL32 t)
{
	if( idx1 == -1)
		m_pExcVA->CopyVertexFrom( idxDest, pVA, idx0);
	else
		m_pExcVA->InterpolateVertex( idxDest, pVA, idx0, idx1, t);
}

inline void _getNormal( VEC3D * p1, VEC3D * p2, VEC3D * p3, VEC3D * pNormal)
{
	VEC3D d1, d2;

	i3Vector::Sub( &d1, p2, p1);
	i3Vector::Sub( &d2, p3, p2);
	i3Vector::Cross( pNormal, &d1, &d2);

	D3DXVec3Normalize( (D3DXVECTOR3 *) pNormal, (CONST D3DXVECTOR3 *) pNormal);
}

////////////////////////////////////////////////
INT32 i3GeometrySplitter::Split( INT32 idx0, INT32 idx1, INT32 idx2)
{
	INT32 i, j, k, idxCoincident, idxTemp;
	VEC3D p[3], hit0, hit1;
	VEC4D * pPlane;

	// 처음 Triangle을 추가한다.
	{
		idxTemp = addPool();
		setPoolVertex( idxTemp + 0, m_pSrcVA, idx0);
		setPoolVertex( idxTemp + 1, m_pSrcVA, idx1);
		setPoolVertex( idxTemp + 2, m_pSrcVA, idx2);
	}

	for( i = 0; i < getPoolCount(); i++)
	{
		INT32 idx = (i * 3);

		getPool( idx, p);

		// World-Space로 변환한다.
		i3Matrix::TransformPoints( p, p, 3, getMatrix());

		#if defined( VLOG)
		i3VLog::SetColor( (UINT8) 0, 0, 255, 255);
		i3VLog::Tri( &p[0], &p[1], &p[2]);
		#endif

		// 각 Plane에 대해 Triangle을 자른다.
		bool bTouched = false;

		for( j = 0; j < getPlaneCount(); j++)
		{
			pPlane = getPlane( j);

			I3_FACE_SIDE side[3];
			INT32 coincidentCount = 0;
			INT32 frontCount = 0, backCount = 0;

			// 각 Vertex가 Plane과 어떤 관계에 있는지 확인
			idxCoincident = -1;
			for( k = 0; k < 3; k++)
			{
				side[k] = i3Plane::getSide( pPlane, &p[k], m_EPS);

				switch( side[k])
				{
					case I3_FACE_COINCIDENT:
						coincidentCount++;				// 평면 상의 점은 나머지 점의 분포에 따라 다르게 해석한다.
						idxCoincident = k;
						break;

					case I3_FACE_FRONT :
						frontCount ++;
						break;

					case I3_FACE_BACK :
						backCount ++;
						break;
				}
			}

			switch( coincidentCount)
			{
				case 3 :
					// 모든 점이 평면 상에 있다.
					// Normal을 보고 Front/Back을 결정한다.
					{
						VEC3D norm;

						_getNormal( &p[0], &p[1], &p[2], &norm);

						REAL32 dot = i3Vector::Dot( &norm, (VEC3D *) pPlane);

						if( dot > 0.0f)
							frontCount += coincidentCount;			// 모두 앞면으로 간주
						else
							backCount += coincidentCount;			// 모두 뒷면으로 간주.
					}
					break;

				default :
					// 평면 상에 있지 않은 다른 Vertex의 분포를 보고
					// 여타 Vertex가 Back이면 Back으로 간주, Front면 Front로 간주해준다.
					if( frontCount > 0)
						frontCount += coincidentCount;
					else
						backCount += coincidentCount;
					break;
			}


			if( frontCount == 3)
			{
				// 모든 Vertex가 평면의 안쪽에 있다.
				// 자를 필요 없다.
				// 다음 Plane으로 검사를 넘긴다.
				continue;
			}
			else if( backCount == 3)
			{
				// 모든 Vertex가 평면의 바깥쪽에 있다.
				// 자를 필요 없다.
				// Exc Buffer에 추가하고 처리를 종료.
				idxTemp = addExc();
				setExcVertex( idxTemp + 0, m_pPoolVA, idx + 0);
				setExcVertex( idxTemp + 1, m_pPoolVA, idx + 1);
				setExcVertex( idxTemp + 2, m_pPoolVA, idx + 2);
				bTouched = true;
				break;
			}
			else
			{
				// 모든 점이 평면의 앞뒤에 존재한다.
				// file://d:\i3\documents\split_triangle.jpg 에서 하위 3개에 해당.

				// 3개 점 중, 반드시 2개는 동일한 Side에 위치한다.
				INT32 p0, p1, p2;
				REAL32 t0 = 0.f;
				REAL32 t1 = 0.f;

				if( side[ 0] == side[1])
				{	
					// 2번 Vertex만 다른쪽.
					p0 = 2;		p1 = 0;		p2 = 1;
				}
				else if( side[1] == side[2])
				{
					// 0번만 다른쪽.
					p0 = 0;		p1 = 1;		p2 = 2;
				}
				else
				{
					// 1번만 다른쪽.
					p0 = 1;		p1 = 2;		p2 = 0;
				}
				
				// 함수 전체에 대한 이해를 하는데 시간이 꽤 걸릴듯하여, 일단 t0, t1에 대해서는 0.f로 초기화만 해둠 (2015.04.06.수빈)
				// 추측으로는 아래 교차함수의 리턴값들이 이 false인 경우 일부 계산이 통째로 스킵되어야 할것 같다.. (2015.04.06.수빈)
				i3Plane::IntersectWithEdge( pPlane, &p[p2], &p[p0], &t0, &hit0);
				i3Plane::IntersectWithEdge( pPlane, &p[p0], &p[p1], &t1, &hit1);

				// 실제 Triangle 추가.
				if( side[ p0] == I3_FACE_FRONT)
				{
					// p0-i1-i0  (Box 안쪽)
					idxTemp = addPool();
					setPoolVertex( idxTemp + 0, m_pPoolVA, idx + p0);					// p0
					setPoolVertex( idxTemp + 1, m_pPoolVA, idx + p0, idx + p1, t1);		// i1
					setPoolVertex( idxTemp + 2, m_pPoolVA, idx + p2, idx + p0, t0);		// i0

					#if defined( VLOG)
					vlogPool( idxTemp, 255, 0, 0);
					#endif

					// i0-i1-p1 (Box 바깥쪽)
					idxTemp = addExc();
					setExcVertex( idxTemp + 0, m_pPoolVA, idx + p2, idx + p0, t0);		// i0
					setExcVertex( idxTemp + 1, m_pPoolVA, idx + p0, idx + p1, t1);		// i1
					setExcVertex( idxTemp + 2, m_pPoolVA, idx + p1);					// p1

					#if defined( VLOG)
					vlogExc( idxTemp, 0, 0, 255);
					#endif

					// p1-p2-i0 (Box 바깥쪽)
					idxTemp = addExc();
					setExcVertex( idxTemp + 0, m_pPoolVA, idx + p1);					// p1
					setExcVertex( idxTemp + 1, m_pPoolVA, idx + p2);					// p2
					setExcVertex( idxTemp + 2, m_pPoolVA, idx + p2, idx + p0, t0);		// i0

					#if defined( VLOG)
					vlogExc( idxTemp, 0, 0, 255);
					#endif
				}
				else
				{
					// p0-i1-i0  (Box 바깥쪽)
					idxTemp = addExc();
					setExcVertex( idxTemp + 0, m_pPoolVA, idx + p0);					// p0
					setExcVertex( idxTemp + 1, m_pPoolVA, idx + p0, idx + p1, t1);		// i1
					setExcVertex( idxTemp + 2, m_pPoolVA, idx + p2, idx + p0, t0);		// i0

					#if defined( VLOG)
					vlogExc( idxTemp, 0, 0, 255);
					#endif

					// i0-i1-p1 (Box 안쪽)
					idxTemp = addPool();
					setPoolVertex( idxTemp + 0, m_pPoolVA, idx + p2, idx + p0, t0);		// i0
					setPoolVertex( idxTemp + 1, m_pPoolVA, idx + p0, idx + p1, t1);		// i1
					setPoolVertex( idxTemp + 2, m_pPoolVA, idx + p1);					// p1

					#if defined( VLOG)
					vlogPool( idxTemp, 255, 0, 0);
					#endif

					// p1-p2-i0 (Box 안쪽)
					idxTemp = addPool();
					setPoolVertex( idxTemp + 0, m_pPoolVA, idx + p1);					// p1
					setPoolVertex( idxTemp + 1, m_pPoolVA, idx + p2);					// p2
					setPoolVertex( idxTemp + 2, m_pPoolVA, idx + p2, idx + p0, t0);		// i0

					#if defined( VLOG)
					vlogPool( idxTemp, 255, 0, 0);
					#endif
				}

				bTouched = true;				// Plane에 의해 분리가 되었으며 Inc buffer로 들어갈 대상이 아님을 표시.
				break;							// 더 이상 현재 Triangle에 대해 처리할 필요가 없다.
			}
		}

		if( bTouched == false)
		{
			// 어떤 Plane에도 건드려지지 않았다.
			// 그 말은 완전히 Plane으로 구성된 Volume 안에 있다는 뜻.
			// Inc Buffer에 추가한다.
			idxTemp = addInc();
			setIncVertex( idxTemp + 0, m_pPoolVA, idx + 0);
			setIncVertex( idxTemp + 1, m_pPoolVA, idx + 1);
			setIncVertex( idxTemp + 2, m_pPoolVA, idx + 2);

			#if defined( VLOG)
			vlogInc( idxTemp, 0, 255, 0);
			#endif
		}
	}

	return 0;
}

INT32 i3GeometrySplitter::Split( i3VertexArray * pVA, MATRIX * pMtx, VEC4D * pPlane)
{
	i3VertexFormat fmt;

	m_pSrcVA = pVA;
	fmt = * m_pSrcVA->GetFormat();

	i3Matrix::Copy( &m_Matrix, pMtx);

	addPlane( pPlane);

	INT32 i, cnt;

	cnt = pVA->GetCount() / 3;

	// Pool의 준비
	{
		// Pool은 하나의 Triangle 단위의 동작에서 필요한 buffer이기 때문에
		// 대단히 많은 버퍼가 필요하지는 않다.
		m_pPoolVA = i3VertexArrayMem::new_object();
		m_pPoolVA->Create( &fmt, 30 * 3, 0);
		m_pPoolVA->Lock();
		m_PoolCount = 0;
	}

	// Included Buffer 준비
	{
		// 전체 Geometry 중에서 Box 영역 안에 들어가는 Triangle들이 모이는 버퍼
		// 쪼개진 결과로 최대 6배가 될 수 있다고 판단.
		m_pIncVA = i3VertexArrayMem::new_object();
		m_pIncVA->Create( &fmt, cnt * 2 * 3, 0);
		m_pIncVA->Lock();
		m_IncCount = 0;
	}

	// Excluded Buffer 준비
	{
		// 전체 Geometry 중에서 Box 영역 안에 들어가는 Triangle들이 모이는 버퍼
		// 쪼개진 결과로 최대 6배가 될 수 있다고 판단.
		m_pExcVA = i3VertexArrayMem::new_object();
		m_pExcVA->Create( &fmt, cnt * 2 * 3, 0);
		m_pExcVA->Lock();
		m_ExcCount = 0;
	}

	// 정밀도 계산.
	/*
	{
		// float type의 특성 상, 5자리정도만 유효하다.
		REAL32 range = max( i3Math::abs( pBox->GetMin()->x), i3Math::abs( pBox->GetMin()->y));
		range = max( range, i3Math::abs( pBox->GetMin()->z));
		range = max( range, i3Math::abs( pBox->GetMax()->x));
		range = max( range, i3Math::abs( pBox->GetMax()->y));
		range = max( range, i3Math::abs( pBox->GetMax()->z));

		INT32 integer = (INT32) range;

		char conv[128];

		sprintf( conv, "%d", integer);

		INT32 digit = strlen( conv);

		m_EPS = 0.9f;

		for( i = 0; i < (5 - digit); i++)
			m_EPS *= 0.1f;

		//I3TRACE( "EXP : %f\n", m_EPS);
	}
	*/

	// Process.............................................
	pVA->Lock( I3G_LOCK_READONLY);

	for( i = 0; i < cnt; i++)
	{
		// 개별 Triangle에 대해 쪼개기 처리...
		Split( (i * 3) + 0, (i * 3) + 1, (i * 3) + 2);

		// 동작의 결과로 Inc Buffer와 Exc bffer는 필요하지만
		// Pool은 필요없다.
		m_PoolCount = 0;
	}

	pVA->Unlock();

	m_pPoolVA->Unlock();
	I3_SAFE_RELEASE( m_pPoolVA);

	// 최종적으로 실제 생성된 Inc/Exc Triangle들을 꼭 맞는 버퍼로 이동시킨다.
	{
		i3VertexArray * pNewVA;

		{	// Included Buffer
			if( m_IncCount > 0)
			{
				pNewVA = i3VertexArrayMem::new_object_ref();
				pNewVA->Create( &fmt, m_IncCount * 3, 0);
				pNewVA->Lock();

				for( i = 0; i < (m_IncCount * 3); i++)
				{
					pNewVA->CopyVertexFrom( i, m_pIncVA, i);
				}

				pNewVA->Unlock();
			}
			else
			{
				pNewVA = nullptr;
			}

			m_pIncVA->Unlock();
			I3_REF_CHANGE( m_pIncVA, pNewVA);
		}

		{	// Excluded Buffer
			if( m_ExcCount > 0)
			{
				pNewVA = i3VertexArrayMem::new_object_ref();
				pNewVA->Create( &fmt, m_ExcCount * 3, 0);
				pNewVA->Lock();

				for( i = 0; i < (m_ExcCount * 3); i++)
				{
					pNewVA->CopyVertexFrom( i, m_pExcVA, i);
				}

				pNewVA->Unlock();
			}
			else
			{
				pNewVA = nullptr;
			}

			m_pExcVA->Unlock();
			I3_REF_CHANGE( m_pExcVA, pNewVA);
		}

	}

	return 0;
}

#if defined( I3_DEBUG)
void i3GeometrySplitter::vlogPool( INT32 idx, UINT8 r, UINT8 g, UINT8 b)
{
	VEC3D pos[3];

	m_pPoolVA->GetPositions( idx, 3, pos);

	i3Matrix::TransformPoints( pos, pos, 3, getMatrix());

	i3VLog::SetColor( r, g, b, 255);
	i3VLog::Line( &pos[0], &pos[1]);
	i3VLog::Line( &pos[1], &pos[2]);
	i3VLog::Line( &pos[2], &pos[0]);
}

void i3GeometrySplitter::vlogInc( INT32 idx, UINT8 r, UINT8 g, UINT8 b)
{
	VEC3D pos[3];

	m_pIncVA->GetPositions( idx, 3, pos);

	i3Matrix::TransformPoints( pos, pos, 3, getMatrix());

	i3VLog::SetColor( r, g, b, 255);
	i3VLog::Line( &pos[0], &pos[1]);
	i3VLog::Line( &pos[1], &pos[2]);
	i3VLog::Line( &pos[2], &pos[0]);
}

void i3GeometrySplitter::vlogExc( INT32 idx, UINT8 r, UINT8 g, UINT8 b)
{
	VEC3D pos[3];

	m_pExcVA->GetPositions( idx, 3, pos);

	i3Matrix::TransformPoints( pos, pos, 3, getMatrix());

	i3VLog::SetColor( r, g, b, 255);
	i3VLog::Line( &pos[0], &pos[1]);
	i3VLog::Line( &pos[1], &pos[2]);
	i3VLog::Line( &pos[2], &pos[0]);
}

#endif
