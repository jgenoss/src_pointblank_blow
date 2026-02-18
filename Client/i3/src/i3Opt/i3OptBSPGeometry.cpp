#include "i3OptPCH.h"
#include "i3OptBSPGeometry.h"

//#define _SPLIT_VLOG
//#define _SPLIT_TRI_VLOG

I3_CLASS_INSTANCE( i3OptBSPGeometry);

i3OptBSPGeometry::i3OptBSPGeometry(void)
{
}

i3OptBSPGeometry::~i3OptBSPGeometry(void)
{
	I3_SAFE_RELEASE( m_pGeo);

	removeAllAttrs();
}

void i3OptBSPGeometry::removeAllAttrs(void)
{
	INT32 i;
	i3RenderAttr * pAttr;

	for( i = 0; i < getAttrCount(); i++)
	{
		pAttr = getAttr( i);
		I3_MUST_RELEASE( pAttr);
	}

	m_AttrList.clear();
}

void i3OptBSPGeometry::setGeometry( i3Geometry * pGeo)
{
	I3_REF_CHANGE( m_pGeo, pGeo);
}

INT32 i3OptBSPGeometry::GetTriangleCount(void)
{
	INT32 i, count = 0;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::same_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		count += pGeoAttr->GetPrimitiveCount();
	}

	return count;
}

i3OptBSPGeometry *	i3OptBSPGeometry::_CreateNewGeometry(void)
{
	i3OptBSPGeometry * pNew;

	pNew = i3OptBSPGeometry::new_object_ref();

	// Attributes
	if( getAttrCount() > 0)
	{
		pNew->getAttrList().resize( getAttrCount());

		for( INT32 i = 0; i < getAttrCount(); i++)
		{
			i3RenderAttr * pAttr = getAttr( i);
			I3_MUST_ADDREF( pAttr);
			pNew->getAttrList()[i] = pAttr;
		}
	}

	// Matrix
	pNew->setMatrix( getMatrix());

	// Geometry
	{
		i3Geometry * pNewGeo = i3Geometry::new_object_ref();

		if( m_pGeo->hasName())
			pNewGeo->SetName( m_pGeo->GetNameString());

		pNew->setGeometry( pNewGeo);
	}

	pNew->setTransparent( isTransparent());

	return pNew;
}

inline void _getNormal( VEC3D * p1, VEC3D * p2, VEC3D * p3, VEC3D * pNormal)
{
	VEC3D d1, d2;

	i3Vector::Sub( &d1, p2, p1);
	i3Vector::Sub( &d2, p3, p2);
	i3Vector::Cross( pNormal, &d1, &d2);

	D3DXVec3Normalize( (D3DXVECTOR3 *) pNormal, (CONST D3DXVECTOR3 *) pNormal);
}

#if 0
static void _getNormalLine( VEC3D * p1, VEC3D * p2, VEC3D * p3, VEC3D * pStart, VEC3D * pEnd)
{
	VEC3D d1, d2;

	i3Vector::Add( pStart, p1, p2);
	i3Vector::Add( pStart, pStart, p3);
	i3Vector::Scale( pStart, pStart, 0.33333f);

	i3Vector::Sub( &d1, p2, p1);
	i3Vector::Sub( &d2, p3, p2);
	i3Vector::Cross( pEnd, &d1, &d2);
	D3DXVec3Normalize( (D3DXVECTOR3 *) pEnd, (CONST D3DXVECTOR3 *) pEnd);
	i3Vector::Add( pEnd, pEnd, pStart);
}
#endif // 0

void	i3OptBSPGeometry::_SplitTriangle( VEC4D * pPlane, i3VertexArray * pOrgVA, INT32 idxOrg, i3VertexArray * pFrontVA, INT32 * pidxFront, i3VertexArray * pBackVA, INT32 * pidxBack)
{
	VEC3D	p[3], hit0;
	INT32	idx[3], i, idxFront, idxBack, coincidentCount = 0;
	I3_FACE_SIDE side[3], side2[3];
	bool bCoincident = false;
	REAL32 t = 0.f;
	REAL32 t1, t2;

	VEC3D n1;

	pOrgVA->GetPositions( idxOrg, 3, p);

	i3Matrix::TransformPoints( p, p, 3, getMatrix());

	_getNormal( &p[0], &p[1], &p[2], &n1);

	for( i = 0; i < 3; i++)
	{
		side[i] = i3Plane::getSide( pPlane, &p[i]);

		if( side[i] == I3_FACE_COINCIDENT)
		{
			bCoincident = true;
			coincidentCount++;
		}
	}

	#if defined( _SPLIT_TRI_VLOG)
	{
		i3VLog::Begin();

		i3VLog::SetColor( (UINT8) 255, 255, 128, 255);
		i3VLog::Plane( pPlane, 5.0f);

		i3VLog::SetColor( (UINT8) 128, 128, 128, 255);
		i3VLog::Line( &p[0], &p[0]);
		i3VLog::Line( &p[1], &p[1]);
		i3VLog::Line( &p[2], &p[2]);

		VEC3D mid, mid2;

		_getNormalLine( &p[0], &p[1], &p[2], &mid, &mid2);

		i3VLog::Line( &mid, &mid2);
	}
	#endif

	I3ASSERT( coincidentCount <= 1);

	idxFront = *pidxFront;
	idxBack = *pidxBack;

	// 삼각형과 Plane의 Intersection 결과에서 어느 한 Vertex가 평면 상에 있는 경우와
	// 그렇지 않은 경우로 크게 나뉜다.
	if( bCoincident)
	{
		// 3개의 Vertex 중, 어느 하나는 평면 상에, 나머지 2개는 각각 Front와 Back Side에 있다.
		// 2개 이상의 Vertex가 평면 상에 존재하는 경우에는 이 함수로 들어오지 않는다. 
		// (Triangle 전체가 Front, 또는 Back Side에 존재하는 것으로 간주되어, 자르지 않는다.)
		i3VertexArray * pTempVA;
		INT32 temp;

		if( side[0] == I3_FACE_COINCIDENT)
		{
			idx[2] = idxOrg + 0;
			idx[0] = idxOrg + 1;		idx[1] = idxOrg + 2;			// Vertex 1이 Front Side이다.

			if( side[1] == I3_FACE_BACK)
			{
				pTempVA = pFrontVA;		pFrontVA = pBackVA;		pBackVA = pTempVA;
				temp = idxFront;		idxFront = idxBack;		idxBack = temp;
			}
		}
		else if( side[1] == I3_FACE_COINCIDENT)
		{
			idx[2] = idxOrg + 1;
			idx[0] = idxOrg + 2;		idx[1] = idxOrg + 0;			// Vertex 0가 Front Side이다.

			if( side[2] == I3_FACE_BACK)
			{
				pTempVA = pFrontVA;		pFrontVA = pBackVA;		pBackVA = pTempVA;
				temp = idxFront;		idxFront = idxBack;		idxBack = temp;
			}
		}
		else
		{
			idx[2] = idxOrg + 2;
			idx[0] = idxOrg + 0;		idx[1] = idxOrg + 1;			// vertex 0가 Front Side이다.

			if( side[0] == I3_FACE_BACK)
			{
				pTempVA = pFrontVA;		pFrontVA = pBackVA;		pBackVA = pTempVA;
				temp = idxFront;		idxFront = idxBack;		idxBack = temp;
			}
		}

		pOrgVA->GetPosition( idx[0], &p[0]);
		pOrgVA->GetPosition( idx[1], &p[1]);

		i3Matrix::TransformPoints( p, p, 2, getMatrix());

		// 교점을 구한다.
		// 실제 교점은 그다지 의미가 없고, t 값을 구한다.
		// 이 t 값을 사용해, Textyre UV 등을 재구성해야 한다.
		i3Plane::IntersectWithEdge( pPlane, &p[0], &p[1], &t, &hit0);

		// Front Side
		{
			pFrontVA->CopyVertexFrom(		idxFront + 0, pOrgVA, idx[2]);
			pFrontVA->CopyVertexFrom(		idxFront + 1, pOrgVA, idx[0]);


			// 함수 전체에 대한 이해를 하는데 시간이 꽤 걸릴듯하여, 일단 t에 대해서는 0.f로 초기화만 해둠 (2015.04.06.수빈)
			// 추측으로는 아래 교차함수의 리턴값들이 이 false인 경우 일부 계산이 통째로 스킵되어야 할것 같다.. (2015.04.06.수빈)
			pFrontVA->InterpolateVertex(	idxFront + 2, pOrgVA, idx[0], idx[1], t);

			#if defined( _SPLIT_VLOG) || defined( _SPLIT_TRI_VLOG)
			{
				pFrontVA->GetPositions( idxFront, 3, p);

				i3Matrix::TransformPoints( p, p, 3, getMatrix());

				i3VLog::SetColor( (UINT8) 0, 0, 255, 255);
				i3VLog::Line( &p[0], &p[1]);
				i3VLog::Line( &p[1], &p[2]);
				i3VLog::Line( &p[2], &p[0]);

				VEC3D s, e;
				_getNormalLine( &p[0], &p[1], &p[2], &s, &e);
				i3VLog::Line( &s, &e);

				_getNormal( &p[0], &p[1], &p[2], &n2);
				if( i3Vector::isAlike( &n1, &n2, 0.1f) == false)
				{
					I3TRACE( "sdfsf\n");
				}
			}
			#endif
		}

		// Back Side
		{
			pBackVA->CopyVertexFrom(	idxBack + 0, pOrgVA, idx[2]);
			pBackVA->InterpolateVertex( idxBack + 1, pOrgVA, idx[0], idx[1], t);
			pBackVA->CopyVertexFrom(	idxBack + 2, pOrgVA, idx[1]);

			#if defined( _SPLIT_VLOG) || defined( _SPLIT_TRI_VLOG)
			{
				pBackVA->GetPositions( idxBack, 3, p);

				i3Matrix::TransformPoints( p, p, 3, getMatrix());

				i3VLog::SetColor( (UINT8) 255, 0, 0, 255);
				i3VLog::Line( &p[0], &p[1]);
				i3VLog::Line( &p[1], &p[2]);
				i3VLog::Line( &p[2], &p[0]);

				VEC3D s, e;
				_getNormalLine( &p[0], &p[1], &p[2], &s, &e);
				i3VLog::Line( &s, &e);

				_getNormal( &p[0], &p[1], &p[2], &n2);
				if( i3Vector::isAlike( &n1, &n2, 0.1f) == false)
				{
					I3TRACE( "sdfsf\n");
				}
			}
			#endif
		}

		*pidxFront += 3;
		*pidxBack += 3;

		#if defined( _SPLIT_TRI_VLOG)
		i3VLog::End();
		#endif

		return;
	}

	// 이 모든 경우를 벗어날 때에는 삼각형인 이상, 아래의 조건을 벗어날 수 없다.
	//
	// - 2개의 동일 Side Vertex
	// - 1개의 반대 Side Vertex
	//
	//   file://d:\i3\documents\split_triangle.jpg
	// 
	// 다른 1개의 Vertex를 찾는다.
	if( side[0] == side[1])
	{
		// vertex 2
		idx[0] = 2;		idx[1] = 0;		idx[2] = 1;
	}
	else if( side[0] == side[2])
	{
		// vertex 1
		idx[0] = 1;		idx[1] = 2;		idx[2] = 0;
	}
	else
	{
		// vertex 0
		idx[0] = 0;		idx[1] = 1;		idx[2] = 2;
	}

	for( INT32 i = 0; i < 3; i++)
		side2[i] = side[ idx[i]];

	#if defined( _SPLIT_VLOG)
		i3VLog::SetColor( (UINT8) 0, 255, 0, 255);
		i3VLog::Plane( pPlane, 5.0f);

		i3VLog::SetColor( (UINT8) 128, 128, 255, 255);
		i3VLog::Line( &p[0], &p[1]);
		i3VLog::Line( &p[1], &p[2]);
		i3VLog::Line( &p[2], &p[0]);
	#endif

	i3Plane::IntersectWithEdge( pPlane, &p[ idx[2]], &p[ idx[0]], &t1, &hit0);
	i3Plane::IntersectWithEdge( pPlane, &p[ idx[0]], &p[ idx[1]], &t2, &hit0);

	{
		// 구성된 Splitted Triangle의 추가
		i3VertexArray * pOneVA, * pTwoVA;
		INT32 idxOne, idxTwo;

		if( side2[0] == I3_FACE_FRONT)
		{
			// Front Side에 1개, Back Side에 2개의 Triangle이 추가된다.
			pOneVA = pFrontVA;		pTwoVA = pBackVA;
			idxOne = idxFront;		*pidxFront += 3;
			idxTwo = idxBack;		*pidxBack += 6;
		}
		else
		{
			// Back Side에 1개, Front Side에 2개의 Triangle이 추가된다.
			pOneVA = pBackVA;		pTwoVA = pFrontVA;
			idxOne = idxBack;		*pidxBack += 3;
			idxTwo = idxFront;		*pidxFront += 6;
		}

		pOneVA->CopyVertexFrom(		idxOne + 0, pOrgVA, idxOrg + idx[0]);							// p2
		pOneVA->InterpolateVertex(	idxOne + 1, pOrgVA, idxOrg + idx[0], idxOrg + idx[1], t2);		// i1
		pOneVA->InterpolateVertex(	idxOne + 2, pOrgVA, idxOrg + idx[2], idxOrg + idx[0], t1);		// i0

		pTwoVA->CopyVertexFrom(		idxTwo + 0,	pOneVA, idxOne + 2);								// i0
		pTwoVA->CopyVertexFrom(		idxTwo + 1, pOneVA, idxOne + 1);								// i1
		pTwoVA->CopyVertexFrom(		idxTwo + 2, pOrgVA, idxOrg + idx[1]);							// p0

		pTwoVA->CopyVertexFrom(		idxTwo + 3, pOrgVA, idxOrg + idx[1]);							// p0
		pTwoVA->CopyVertexFrom(		idxTwo + 4, pOrgVA, idxOrg + idx[2]);							// p1
		pTwoVA->CopyVertexFrom(		idxTwo + 5, pOneVA, idxOne + 2);								// i0

		#if defined( _SPLIT_VLOG) || defined( _SPLIT_TRI_VLOG)
		{
			if( side2[0] == I3_FACE_FRONT)
				i3VLog::SetColor( (UINT8) 0, 0, 255, 255);
			else
				i3VLog::SetColor( (UINT8) 255, 0, 0, 255);

			pOneVA->GetPositions( idxOne, 3, p);
			i3Matrix::TransformPoints( p, p, 3, getMatrix());
			i3VLog::Line( &p[0], &p[1]);
			i3VLog::Line( &p[1], &p[2]);
			i3VLog::Line( &p[2], &p[0]);

			VEC3D s, e;
			_getNormalLine( &p[0], &p[1], &p[2], &s, &e);
			i3VLog::Line( &s, &e);

			_getNormal( &p[0], &p[1], &p[2], &n2);
				if( i3Vector::isAlike( &n1, &n2, 0.1f) == false)
				{
					I3TRACE( "sdfsf\n");
				}

			if( side2[0] != I3_FACE_FRONT)
				i3VLog::SetColor( (UINT8) 0, 0, 255, 255);
			else
				i3VLog::SetColor( (UINT8) 255, 0, 0, 255);

			pTwoVA->GetPositions( idxTwo, 3, p);

			i3Matrix::TransformPoints( p, p, 3, getMatrix());
			i3VLog::Line( &p[0], &p[1]);
			i3VLog::Line( &p[1], &p[2]);
			i3VLog::Line( &p[2], &p[0]);

				_getNormalLine( &p[0], &p[1], &p[2], &s, &e);
				i3VLog::Line( &s, &e);

				_getNormal( &p[0], &p[1], &p[2], &n2);
				if( i3Vector::isAlike( &n1, &n2, 0.1f) == false)
				{
					I3TRACE( "sdfsf\n");
				}

			pTwoVA->GetPositions( idxTwo + 3, 3, p);

			i3Matrix::TransformPoints( p, p, 3, getMatrix());
			i3VLog::Line( &p[0], &p[1]);
			i3VLog::Line( &p[1], &p[2]);
			i3VLog::Line( &p[2], &p[0]);

				_getNormalLine( &p[0], &p[1], &p[2], &s, &e);
				i3VLog::Line( &s, &e);

				_getNormal( &p[0], &p[1], &p[2], &n2);
				if( i3Vector::isAlike( &n1, &n2, 0.1f) == false)
				{
					I3TRACE( "sdfsf\n");
				}
		}
		#endif
	}

	#if defined( _SPLIT_TRI_VLOG)
	i3VLog::End();
	#endif
}

I3_FACE_SIDE i3OptBSPGeometry::Split( VEC4D * pPlane, i3OptBSPGeometry ** ppFront, i3OptBSPGeometry ** ppBack)
{
	i3OptBSPGeometry * pFront = nullptr;
	i3OptBSPGeometry * pBack = nullptr;
	INT32 i, j, front, back, primCount;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	VEC3D p0, p1, p2;
	I3_FACE_SIDE side, sideResult = I3_FACE_COINCIDENT;
	INT32	_SideCount[64][2];
	
	// 전체 Geometry들을 검사해, Front Face와 Back Face의 개수를 구한다.
	front = back = 0;

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::same_of<i3GeometryAttr* >(pAttr))
			continue;

		_SideCount[i][0] = 0;
		_SideCount[i][1] = 0;

		pGeoAttr = (i3GeometryAttr *) pAttr;
		pVA = pGeoAttr->GetVertexArray();

		primCount = pVA->GetCount() / 3;

		// 속도 향상을 위해, Unlock을 하지 않는다.
		// 모든 처리가 끝난 후, Unlock 한다.
		pVA->Lock( I3G_LOCK_READONLY);

		for( j = 0; j < primCount; j++)
		{
			pVA->GetPosition( (j * 3) + 0, &p0);
			pVA->GetPosition( (j * 3) + 1, &p1);
			pVA->GetPosition( (j * 3) + 2, &p2);

			i3Vector::TransformCoord( &p0, &p0, getMatrix());
			i3Vector::TransformCoord( &p1, &p1, getMatrix());
			i3Vector::TransformCoord( &p2, &p2, getMatrix());

			side = i3Plane::getSide( pPlane, &p0, &p1, &p2);

			switch( side)
			{
				case I3_FACE_COINCIDENT :
				case I3_FACE_FRONT :		_SideCount[i][0] ++; 	break;
				case I3_FACE_BACK :			_SideCount[i][1] ++;	break;
				case I3_FACE_SPAN :	
					{
						I3_FACE_SIDE face1, face2;

						// Split되는 경우, 최대 3개의 Triangle로 분할될 수 있다.
						side = i3Plane::getSide( pPlane, &p0);
						face1 = i3Plane::getSide( pPlane, &p1);
						face2 = i3Plane::getSide( pPlane, &p2);

						_SideCount[i][0]++;	
						_SideCount[i][1]++;	
						
						// 어느 1개라도 Coincident가 있다면..
						if((side == I3_FACE_COINCIDENT) || (face1 == I3_FACE_COINCIDENT) || (face2 == I3_FACE_COINCIDENT))
						{
						}
						else
						{
							if( side == I3_FACE_FRONT)
							{
								if( face1 != face2)
									_SideCount[i][0]++;
								else
									_SideCount[i][1]++;
							}
							else
							{
								if( face1 != face2)
									_SideCount[i][1]++;
								else
									_SideCount[i][0]++;
							}
						}
					}
					break;
			}
		}

		front	+= _SideCount[i][0];
		back	+= _SideCount[i][1];
	}

	if((front > 0) && (back == 0))
	{
		// 모두 Front Face이다.
		*ppFront = this;
		*ppBack = nullptr;
		sideResult = I3_FACE_FRONT;
		goto ExitPart;
	}
	else if((front == 0) && (back > 0))
	{
		// 모두 Back-Face이다.
		*ppFront = nullptr;
		*ppBack = this;
		sideResult = I3_FACE_BACK;
		goto ExitPart;
	}

	sideResult = I3_FACE_SPAN;

	// Plane에 걸치는 Polygon이 있기 때문에 분할한다.
	*ppFront	= pFront	= _CreateNewGeometry();
	*ppBack		= pBack		= _CreateNewGeometry();

	INT32 idxFront, idxBack;

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::same_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;
		pVA = pGeoAttr->GetVertexArray();

		if((_SideCount[i][0] > 0) && (_SideCount[i][1] == 0))
		{
			// 완전히 Front Side에 있다. 단순 추가
			pFront->getGeometry()->AppendGeometryAttr( pGeoAttr);
			continue;
		}
		else if((_SideCount[i][0] == 0) && (_SideCount[i][1] > 0))
		{
			// 완전히 Back Side에 있다. 단순 추가
			pBack->getGeometry()->AppendGeometryAttr( pGeoAttr);
			continue;
		}

		// 양쪽에 걸쳐 있는 경우이므로 분할
		i3VertexArray * pFrontVA = i3VertexArray::new_object_ref();
		i3VertexArray * pBackVA = i3VertexArray::new_object_ref();

		idxFront = idxBack = 0;

		// Front용, Back용 Vertex Array 생성
		pFrontVA->Create( pVA->GetFormat(), _SideCount[i][0] * 3, 0);
		pFrontVA->Lock();

		pBackVA->Create( pVA->GetFormat(), _SideCount[i][1] * 3, 0);
		pBackVA->Lock();

		primCount = pVA->GetCount() / 3;

		for( j = 0; j < primCount; j++)
		{
			pVA->GetPosition( (j * 3) + 0, &p0);
			pVA->GetPosition( (j * 3) + 1, &p1);
			pVA->GetPosition( (j * 3) + 2, &p2);

			i3Vector::TransformCoord( &p0, &p0, getMatrix());
			i3Vector::TransformCoord( &p1, &p1, getMatrix());
			i3Vector::TransformCoord( &p2, &p2, getMatrix());

			side = i3Plane::getSide( pPlane, &p0, &p1, &p2);

			switch( side)
			{
				case I3_FACE_COINCIDENT :
				case I3_FACE_FRONT :	
					pFrontVA->CopyVertexFrom( idxFront + 0, pVA, (j * 3) + 0);
					pFrontVA->CopyVertexFrom( idxFront + 1, pVA, (j * 3) + 1);
					pFrontVA->CopyVertexFrom( idxFront + 2, pVA, (j * 3) + 2);

					#if defined( _SPLIT_VLOG)
						i3VLog::SetColor( (UINT8) 0, 0, 255, 255);
						i3VLog::Line( &p0, &p1);
						i3VLog::Line( &p1, &p2);
						i3VLog::Line( &p2, &p0);
					#endif

					idxFront += 3;
					break;

				case I3_FACE_BACK :	
					pBackVA->CopyVertexFrom( idxBack + 0, pVA, (j * 3) + 0);
					pBackVA->CopyVertexFrom( idxBack + 1, pVA, (j * 3) + 1);
					pBackVA->CopyVertexFrom( idxBack + 2, pVA, (j * 3) + 2);

					#if defined( _SPLIT_VLOG)
						i3VLog::SetColor( (UINT8) 255, 0, 0, 255);
						i3VLog::Line( &p0, &p1);
						i3VLog::Line( &p1, &p2);
						i3VLog::Line( &p2, &p0);
					#endif

					idxBack += 3;
					break;

				case I3_FACE_SPAN :	
					// _SplitTriangle 내부에서 idxFront와 idxBack이 변경된다.
					_SplitTriangle( pPlane, pVA, (j * 3), pFrontVA, &idxFront, pBackVA, &idxBack);
					break;
			}
		}

		// 개수가 일치하는지 Debugging용 확인
		I3ASSERT( (idxFront / 3) == _SideCount[i][0]);
		I3ASSERT( (idxBack / 3) == _SideCount[i][1]);

		// Front GeometryAttr
		{
			i3GeometryAttr * pAttr = i3GeometryAttr::new_object_ref();

			pFrontVA->Unlock();
			pAttr->SetVertexArray( pFrontVA, I3G_PRIM_TRILIST, _SideCount[i][0]);

			pFront->getGeometry()->AppendGeometryAttr( pAttr);
		}

		// Back GeometryAttr
		{
			i3GeometryAttr * pAttr = i3GeometryAttr::new_object_ref();

			pBackVA->Unlock();
			pAttr->SetVertexArray( pBackVA, I3G_PRIM_TRILIST, _SideCount[i][1]);

			pBack->getGeometry()->AppendGeometryAttr( pAttr);
		}
	}
	
ExitPart:

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::same_of< i3GeometryAttr*>(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;
		pVA = pGeoAttr->GetVertexArray();

		pVA->Unlock();
	}

	return sideResult;
}

i3Node * i3OptBSPGeometry::BuildSg(void)
{
	i3Node * pResult = nullptr;

	// Transform
	if( i3Matrix::IsIdentity( getMatrix()) == false)
	{
		i3Transform * pTrans = i3Transform::new_object_ref();

		pResult = pTrans;

		pTrans->SetMatrix( getMatrix());
	}

	// Attributes
	i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

	for( INT32 i = 0; i < getAttrCount(); i++)
	{
		pAttrSet->AddAttr( getAttr( i));
	}

	if( pResult == nullptr)
		pResult = pAttrSet;
	else
		pResult->AddChild( pAttrSet);
	
	// Geometry
	pAttrSet->AddChild( m_pGeo);

	return pResult;
}

void i3OptBSPGeometry::VLog(void)
{
	INT32 i, j;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	VEC3D p[3];

	i3VLog::SetColor( (i3Math::Randf() * 0.7f) + 0.3f, (i3Math::Randf() * 0.7f) + 0.3f, (i3Math::Randf() * 0.7f) + 0.3f, 1.0f);

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::same_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		pVA = pGeoAttr->GetVertexArray();

		pVA->Lock( I3G_LOCK_READONLY);

		for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j++)
		{
			pVA->GetPositions( (j * 3) + 0, 3, p);

			i3VLog::Line( &p[0], &p[1]);
			i3VLog::Line( &p[1], &p[2]);
			i3VLog::Line( &p[2], &p[0]);
		}

		pVA->Unlock();
	}
}
