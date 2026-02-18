#include "i3OptPCH.h"
#include "i3OptBSPNode.h"
#include "i3SceneOptimizer.h"
#include "i3OptBSPGeometry.h"
#include "I3OPT_BSP_CONTEXT.h"

#include "i3Base/itl/vector_util.h"

//#define _SPLIT_VLOG
//#define _FINDPLANE_VLOG

I3_CLASS_INSTANCE( i3OptBSPNode);

i3OptBSPNode::~i3OptBSPNode(void)
{
	I3_SAFE_RELEASE( m_pFront);
	I3_SAFE_RELEASE( m_pBack);
	I3MEM_SAFE_FREE( m_pPoly);

	removeAllGeometry();
}

void	i3OptBSPNode::addGeometry( i3OptBSPGeometry * pGeo)
{
	I3_MUST_ADDREF( pGeo);
	m_GeoList.push_back( pGeo);
}

void	i3OptBSPNode::addGeometries(i3::vector<i3OptBSPGeometry*>& List)
{
	for(size_t i = 0; i < List.size(); i++)
		addGeometry(List[i]);
}

void	i3OptBSPNode::removeGeometry( i3OptBSPGeometry * pGeo)
{
	size_t idx = i3::vu::index_of(m_GeoList, pGeo);
		//m_GeoList.IndexOf( pGeo);

	I3ASSERT( idx != m_GeoList.size() );

	I3_MUST_RELEASE( pGeo);
	m_GeoList.erase( m_GeoList.begin() + idx);
//	m_GeoList.Delete( idx);
}

void	i3OptBSPNode::removeAllGeometry(void)
{
	i3OptBSPGeometry * pGeo;

	for(size_t i = 0; i < m_GeoList.size(); i++)
	{
		pGeo = m_GeoList[i];
		I3_MUST_RELEASE( pGeo);
	}
	m_GeoList.clear();
}

//#pragma optimize( "", on)

void i3OptBSPNode::BuildBSPTri(void)
{
	INT32 i, idx;
	i3OptBSPGeometry * pGeo;
	i3Geometry * pGeoNode;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	I3OPT_BSP_TRI * pPoly;

	I3MEM_SAFE_FREE( m_pPoly);
	m_PolyCount = 0;

	for( i = 0; i < getGeometryCount(); i++)
	{
		pGeo = getGeometry( i);

		m_PolyCount += pGeo->GetTriangleCount();
	}

	if( m_PolyCount <= 0)
		return;

	m_pPoly = (I3OPT_BSP_TRI *) i3MemAlloc( sizeof(I3OPT_BSP_TRI) * m_PolyCount);
	memset( m_pPoly, 0, sizeof(I3OPT_BSP_TRI) * m_PolyCount);

	pPoly = m_pPoly;

	for( i = 0, idx = 0; i < getGeometryCount(); i++)
	{
		pGeo = getGeometry( i);

		pGeoNode = pGeo->getGeometry();

		for( INT32 j = 0; j < pGeoNode->GetGeometryAttrCount(); j++)
		{
			i3RenderAttr * pAttr = pGeoNode->GetGeometryAttr( j);

			if( !i3::same_of<i3GeometryAttr*>(pAttr))
				continue;

			pGeoAttr = (i3GeometryAttr *) pAttr;

			pVA = pGeoAttr->GetVertexArray();

			pVA->Lock( I3G_LOCK_READONLY);

			INT32 primcount = pVA->GetCount() / 3;

			for( INT32 k = 0; k < primcount; k++)
			{
				pVA->GetPosition( (k * 3) + 0, & pPoly->m_Point[0]);
				pVA->GetPosition( (k * 3) + 1, & pPoly->m_Point[1]);
				pVA->GetPosition( (k * 3) + 2, & pPoly->m_Point[2]);

				// World-Space Position으로 Transform한다.
				i3Matrix::TransformPoints( pPoly->m_Point, pPoly->m_Point, 3, pGeo->getMatrix());

				// 삼각형을 포함하는 Plane을 구한다.
				i3Plane::Set( & pPoly->m_Plane, & pPoly->m_Point[0], & pPoly->m_Point[1], & pPoly->m_Point[2]);
				
				pPoly->m_GeometryIndex = i;
				pPoly->m_GeometryNodeIndex = j;
				pPoly->m_TriIndex = k;

				pPoly++;
			}

			pVA->Unlock();
		}
	}
}

bool i3OptBSPNode::_isConvexGeometry(void)
{
	INT32 i, j;
	I3OPT_BSP_TRI * pPoly1, *pPoly2;
	I3_FACE_SIDE side;

	for( i = 0; i < m_PolyCount; i++)
	{
		pPoly1 = & m_pPoly[i];

		for( j = 0; j < m_PolyCount; j++)
		{
			if( j == i) 	continue;

			pPoly2 = & m_pPoly[j];

			side = i3Plane::getSide( &pPoly1->m_Plane, &pPoly2->m_Point[0], &pPoly2->m_Point[1], &pPoly2->m_Point[2]);

			if((side != I3_FACE_FRONT) && (side != I3_FACE_COINCIDENT))
				return false;
		}
	}

	return true;
}

bool i3OptBSPNode::_isValidPlane( VEC4D * pPlane)
{
	// Parent Node들을 찾아가며, 해당 Plane을 이미 사용했는지
	// 확인한다.
	i3OptBSPNode * pTemp = m_pParent;

	while( pTemp != nullptr)
	{
		if( i3Math::isAlike( pPlane->x, pTemp->getPlane()->x) &&
			i3Math::isAlike( pPlane->y, pTemp->getPlane()->y) &&
			i3Math::isAlike( pPlane->z, pTemp->getPlane()->z) &&
			i3Math::isAlike( pPlane->w, pTemp->getPlane()->w))
		{
			return false;
		}

		pTemp = pTemp->getParent();
	}

	return true;
}

#define MIN_RELATION		0.8f
#define MIN_RELATION_SCALE	2

bool i3OptBSPNode::_FindBestHyperPlane( DIVIDE_RESULT_INFO * pInfo)
{
	INT32 i, j;
	I3_FACE_SIDE side;
	INT32 front, back, span, splitCount;
	REAL32 relation, minRelation, bestRelation;
	bool bFind = false;
	I3OPT_BSP_TRI * p1, * p2;

	minRelation = MIN_RELATION;
	bestRelation = 0.0f;
	splitCount = 0x7FFFFFFF;

	#if defined( _FINDPLANE_VLOG)
	i3VLog::Begin();

	i3VLog::SetColor( (UINT8) 100, 100, 100, 255);

	for( i = 0; i < m_PolyCount; i++)
	{
		p1 = & m_pPoly[i];

		i3VLog::Line( &p1->m_Point[0], &p1->m_Point[1]);
		i3VLog::Line( &p1->m_Point[1], &p1->m_Point[2]);
		i3VLog::Line( &p1->m_Point[2], &p1->m_Point[0]);

		VEC3D mid, mid2;

		i3Vector::Add( &mid, &p1->m_Point[0], &p1->m_Point[1]);
		i3Vector::Add( &mid, &mid, &p1->m_Point[2]);
		i3Vector::Scale( &mid, &mid, 0.3333333f);
		i3Vector::Add( &mid2, &mid, (VEC3D *) &p1->m_Plane);

		i3VLog::Line( &mid, &mid2);
	}
	#endif

	while( bFind == false)
	{
		for( i = 0; i < m_PolyCount; i++)
		{
			p1 = & m_pPoly[i];

			if( _isValidPlane( &p1->m_Plane) == false)
				continue;

			front = back = span = 0;

			for( j = 0; j < m_PolyCount; j++)
			{
				if( i == j)		continue;

				p2 = & m_pPoly[j];

				side = i3Plane::getSide( &p1->m_Plane, &p2->m_Point[0], &p2->m_Point[1], &p2->m_Point[2]);

				switch( side)
				{
					case I3_FACE_FRONT :		front++;	break;
					case I3_FACE_BACK :			back++;		break;
					case I3_FACE_SPAN :			span++;		break;
				}
			}

			// Relation Factor를 계산
			if( front < back)
			{
				relation = (REAL32)front / back;
			}
			else
			{
				if( front > 0)
					relation = (REAL32)back / front;
				else
					relation = 0.f;
			}

			// 이전까지의 결과와 비교해, 더욱 좋은 조건인지 확인
			if( ((span < splitCount) && (relation > minRelation)) ||
				((span == splitCount) && (relation > bestRelation)))
			{
				i3Vector::Copy( &pInfo->m_Point[0], &p1->m_Point[0]);
				i3Vector::Copy( &pInfo->m_Point[1], &p1->m_Point[1]);
				i3Vector::Copy( &pInfo->m_Point[2], &p1->m_Point[2]);
				i3Vector::Copy( &pInfo->m_Plane, &p1->m_Plane);
				pInfo->m_SplitCount = span;
				pInfo->m_BestRelation = relation;
				bFind = true;
				bestRelation = relation;
				splitCount = span;
			}
		}

		minRelation = minRelation / MIN_RELATION_SCALE;
	}

	#if defined( _FINDPLANE_VLOG)
	i3VLog::End();
	#endif

	return bFind;
}

bool i3OptBSPNode::_FindBestPlaneInGeo( DIVIDE_RESULT_INFO * pInfo)
{
	INT32 i, j;
	I3_FACE_SIDE side;
	INT32 front, back, span, splitCount;
	REAL32 relation, minRelation, bestRelation;
	bool bFind = false;
	I3OPT_BSP_TRI * p1, * p2;

	minRelation = MIN_RELATION;
	bestRelation = 0.0f;
	splitCount = 0x7FFFFFFF;

	#if defined( _FINDPLANE_VLOG)
	i3VLog::Begin();

	i3VLog::SetColor( (UINT8) 100, 100, 100, 255);

	for( i = 0; i < m_PolyCount; i++)
	{
		p1 = & m_pPoly[i];

		i3VLog::Line( &p1->m_Point[0], &p1->m_Point[1]);
		i3VLog::Line( &p1->m_Point[1], &p1->m_Point[2]);
		i3VLog::Line( &p1->m_Point[2], &p1->m_Point[0]);

		VEC3D mid, mid2;

		i3Vector::Add( &mid, &p1->m_Point[0], &p1->m_Point[1]);
		i3Vector::Add( &mid, &mid, &p1->m_Point[2]);
		i3Vector::Scale( &mid, &mid, 0.3333333f);
		i3Vector::Add( &mid2, &mid, (VEC3D *) &p1->m_Plane);

		i3VLog::Line( &mid, &mid2);
	}
	#endif

	for( i = 0; i < m_PolyCount; i++)
	{
		p1 = & m_pPoly[i];

		if( _isValidPlane( &p1->m_Plane) == false)
			continue;

		front = back = span = 0;

		for( j = 0; j < m_PolyCount; j++)
		{
			if( i == j)		continue;

			p2 = & m_pPoly[j];

			side = i3Plane::getSide( &p1->m_Plane, &p2->m_Point[0], &p2->m_Point[1], &p2->m_Point[2]);

			switch( side)
			{
				case I3_FACE_FRONT :		front++;	break;
				case I3_FACE_BACK :			back++;		break;
				case I3_FACE_SPAN :			span++;		break;
			}
		}

		if( (front == 0) && (back == 0))
		{
			I3TRACE( "ZERO!!!\n");
		}
		else
		{
			// Relation Factor를 계산
			if( front < back)
				relation = (REAL32)front / back;
			else
				relation = (REAL32)back / front;

			// 이전까지의 결과와 비교해, 더욱 좋은 조건인지 확인
			if( (span <= splitCount) && (relation > bestRelation))
			{
				i3Vector::Copy( &pInfo->m_Point[0], &p1->m_Point[0]);
				i3Vector::Copy( &pInfo->m_Point[1], &p1->m_Point[1]);
				i3Vector::Copy( &pInfo->m_Point[2], &p1->m_Point[2]);
				i3Vector::Copy( &pInfo->m_Plane, &p1->m_Plane);
				pInfo->m_SplitCount = span;
				pInfo->m_BestRelation = relation;
				bestRelation = relation;
				splitCount = span;
			}
		}
	}

	#if defined( _FINDPLANE_VLOG)

	i3VLog::SetColor( 1.0f, 0.0f, 0.0f, 1.0f);
	i3VLog::Plane( & pInfo->m_Plane, 10.0f);
	i3VLog::End();
	#endif

	return bFind;
}

void i3OptBSPNode::_getBound( VEC3D * pMin, VEC3D * pMax)
{
	INT32 i, j;
	bool bFirst = true;
	I3OPT_BSP_TRI * p;
	
	for( i = 0; i < m_PolyCount; i++)
	{
		p = m_pPoly + i;

		for( j = 0; j < 3; j++)
		{
			if( bFirst)
			{
				i3Vector::Copy( pMin, &p->m_Point[j]);
				i3Vector::Copy( pMax, &p->m_Point[j]);

				bFirst = false;
			}
			else
			{
				i3Vector::Minimize( pMin, pMin, &p->m_Point[j]);
				i3Vector::Maximize( pMax, pMax, &p->m_Point[j]);
			}
		}
	}
}

bool i3OptBSPNode::_FindHalfPlane( DIVIDE_RESULT_INFO * pInfo)
{
	// 전체 Bound를 구한다.
	VEC3D vmin, vmax, diff, mid;
	SPLIT_PLANE axis;

	_getBound( &vmin, &vmax);

	i3Vector::Add( &mid, &vmin, &vmax);
	i3Vector::Scale( &mid, &mid, 0.5f);

	i3Vector::Sub( &diff, &vmax, &vmin);

	diff.x = i3Math::abs( diff.x);
	diff.y = i3Math::abs( diff.y);
	diff.z = i3Math::abs( diff.z);

	// 가장 분산도가 높은 축을 찾는다.
	if( diff.x > diff.y)
	{
		if( diff.x > diff.z)
			axis = SPLIT_PLANE_XAXIS;
		else
			axis = SPLIT_PLANE_ZAXIS;
	}
	else
	{
		if( diff.y > diff.z)
			axis = SPLIT_PLANE_YAXIS;
		else
			axis = SPLIT_PLANE_ZAXIS;
	}

	switch( axis)
	{
		case SPLIT_PLANE_XAXIS :
			i3Plane::Set( & pInfo->m_Plane, &mid, &I3_XAXIS);
			break;

		case SPLIT_PLANE_YAXIS :
			i3Plane::Set( & pInfo->m_Plane, &mid, &I3_YAXIS);
			break;

		case SPLIT_PLANE_ZAXIS :
			i3Plane::Set( & pInfo->m_Plane, &mid, &I3_ZAXIS);
			break;
	}

	return true;
}

void i3OptBSPNode::_SplitGeometry(void)
{
	INT32 i;
	I3_FACE_SIDE side;
	i3OptBSPGeometry * pGeo, *pFront, * pBack;
	bool bFrontAdd, bBackAdd;

	// 찾아진 m_Plane을 기준으로 현재의 Geometry들을 분할한다.

	for( i = 0; i < getGeometryCount(); i++)
	{
		pGeo = getGeometry( i);

		side = pGeo->Split( &m_Plane, &pFront, &pBack);

		bFrontAdd = bBackAdd = false;

		switch( side)
		{
			case I3_FACE_FRONT :		bFrontAdd = true;				break;
			case I3_FACE_BACK :			bBackAdd = true;				break;
			case I3_FACE_COINCIDENT :									break;
			case I3_FACE_SPAN :			bFrontAdd = true;	bBackAdd = true;	break;
		}

		if( bFrontAdd)
		{
			if( m_pFront == nullptr)
			{
				m_pFront = i3OptBSPNode::new_object();
				m_pFront->setParent( this);
			}

			I3ASSERT( pFront != nullptr);
			m_pFront->addGeometry( pFront);
		}

		if( bBackAdd)
		{
			if( m_pBack == nullptr)
			{
				m_pBack = i3OptBSPNode::new_object();
				m_pBack->setParent( this);
			}

			I3ASSERT( pBack != nullptr);
			m_pBack->addGeometry( pBack);
		}

		if((bFrontAdd == false) && (bBackAdd == false))
		{
			// 어느 곳에도 추가되지 않았다면, this가 해당 Geometry를
			// 가지고 있는 것으로...
		}
		else
		{
			// Front 또는 Back 어느 한 쪽으로 이전(분리) 되었다면
			// this의 Geometry List에서는 제거한다.
//			m_GeoList.Delete( i);
			m_GeoList.erase(m_GeoList.begin() + i);
			I3_MUST_RELEASE( pGeo);

			i--;
		}
	}

	if((m_pFront == nullptr) || (m_pBack == nullptr))
	{
		I3TRACE( "Empty\n");
	}

}

void i3OptBSPNode::BuildBSP( I3OPT_BSP_CONTEXT * pCtx)
{
	DIVIDE_RESULT_INFO rst;
	bool bRv;

	// 소속된 모든 Geometry들을 하나로 끌어모은다.
	BuildBSPTri();

	pCtx->m_NodeCount++;

	i3SceneOptimizer::ReportSubProgress( pCtx->m_NodeCount, nullptr);

	pCtx->m_Depth++;

	// 만약 ConvexSet이라면 더 이상 분할할 필요가 없다.
	if( pCtx->m_Condition & SPLIT_COND_CONVEX)
	{
		if( _isConvexGeometry())
			goto ExitPart;
	}

	if( pCtx->m_Condition & SPLIT_COND_DEPTH)
	{
		// Depth가 지정 이상이면 분할을 정지한다.
		if( pCtx->m_Depth > pCtx->m_LimitDepth)
			goto ExitPart;
	}

	if( pCtx->m_Condition & SPLIT_COND_POLYCOUNT)
	{
		// 지정 Polygon Count를 넘어서면 분할을 멈춘다.
		if( m_PolyCount <= pCtx->m_LimitCount)
			goto ExitPart;
	}

	// 가장 적합한 Split-Plane을 찾는다.
	switch( pCtx->m_Mode)
	{
		case SPLIT_MODE_HYPERPLANE :
			bRv = _FindBestHyperPlane( &rst);
			if( bRv == false)
			{
				goto ExitPart;
			}
			break;

		case SPLIT_MODE_HALFPLANE :
			//_FindHalfPlane( &rst);
			_FindBestPlaneInGeo( &rst);
			break;
	}

	i3Vector::Copy( &m_Plane, &rst.m_Plane);

	I3MEM_SAFE_FREE( m_pPoly);
	m_PolyCount = 0;

	#if defined( _SPLIT_VLOG)
		i3VLog::Begin();

		i3VLog::SetColor( (UINT8) 255, 128, 128, 255);
		i3VLog::Tri( &rst.m_Point[0], &rst.m_Point[1], &rst.m_Point[2]);
	#endif

	_SplitGeometry();

	{
		char conv[256], szText[256];
		INT32 i, lcnt = 0, rcnt = 0;

		for( i = 0; i < pCtx->m_Depth; i++)
			conv[i] = '\t';

		conv[i] = 0;

		if( m_pFront != nullptr)
			lcnt = m_pFront->getGeometryCount();

		if( m_pBack != nullptr)
			rcnt = m_pBack->getGeometryCount();

		sprintf( szText, "%s : node split (%d, %d)", conv, lcnt, rcnt);

		i3SceneOptimizer::ReportSubProgress( pCtx->m_NodeCount, szText);
	}

	#if defined( _SPLIT_VLOG)
		i3VLog::End();
	#endif

	if( m_pFront != nullptr)
		m_pFront->BuildBSP( pCtx);

	if( m_pBack != nullptr)
		m_pBack->BuildBSP( pCtx);

	// 해제
ExitPart:
	I3MEM_SAFE_FREE( m_pPoly);
	m_PolyCount = 0;

	i3SceneOptimizer::ReportSubEnd();

	pCtx->m_Depth --;
}

i3BinaryPartitionNode *	 i3OptBSPNode::BuildSg(void)
{
	i3BinaryPartitionNode * pNew;

	pNew = i3BinaryPartitionNode::new_object_ref();

	// Hyper Plane
	pNew->setPlane( getPlane());

	if( m_pFront != nullptr)
	{
		i3BinaryPartitionNode * pFront = m_pFront->BuildSg();

		pNew->setFront( pFront);
	}

	if( m_pBack != nullptr)
	{
		i3BinaryPartitionNode * pBack = m_pBack->BuildSg();

		pNew->setBack( pBack);
	}

	// Geometries
	{
		INT32 i;
		i3OptBSPGeometry * pGeo;

		for( i = 0; i < getGeometryCount(); i++)
		{
			pGeo = getGeometry( i);

			i3Node * pNode = pGeo->BuildSg();

			pNew->AddChild( pNode);
		}
	}

	return pNew;
}

void i3OptBSPNode::VLog(void)
{
	if( getParent() == nullptr)
	{
		i3VLog::Begin();
	}

	for( INT32 i = 0; i < getGeometryCount(); i++)
	{
		i3OptBSPGeometry * pGeo = getGeometry( i);

		pGeo->VLog();
	}

	if( m_pFront != nullptr)
		m_pFront->VLog();

	if( m_pBack != nullptr)
		m_pBack->VLog();

	if( getParent() == nullptr)
	{
		i3VLog::End();
	}
}

void i3OptBSPNode::Dump( INT32 depth)
{
	INT32 i;

	for( i = 0; i < depth; i++)		I3TRACE( "\t");

	I3TRACE( "BSP[ %.2f %.2f %.2f %.2f] G(%d)\n", m_Plane.x, m_Plane.y, m_Plane.z, m_Plane.w,	getGeometryCount());

	if( getFrontNode() != nullptr)
		getFrontNode()->Dump( depth + 1);

	if( getBackNode() != nullptr)
		getBackNode()->Dump( depth + 1);
}
