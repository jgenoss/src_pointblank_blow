#include "i3OptPCH.h"
#include "i3OptOctreeNode.h"
#include "i3OptBuildIndexedGeometry.h"
#include "i3OptBuildOcTree.h"
#include "i3OptSortNodeByAttr.h"
#include "i3OptSpatialPartition.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptOctreeNode);

i3OptOctreeNode::~i3OptOctreeNode(void)
{
	removeAllMesh();
}

// 말단 i3Geometry Node에 적용되는 모든 Attr과 Transform을
// 추출하여 별도의 Sg.로 구성해 Return한다.
//
// 이렇게 Isolated Sg.는 자유롭게 Geometry Node를 이동시키는데
// 편리한 장점이 있다.
void i3OptOctreeNode::addMesh( i3Geometry * pGeo)
{
	i3Node * pParent = pGeo->GetParent();
	i3AttrSet * pAttrSet = nullptr;
	i3Node *	pIsolateRoot;
	INT32 i;
	i3Geometry * pNewGeo;
	MATRIX mtx;

	{
		pNewGeo = i3Geometry::new_object_ref();

		pGeo->CopyTo( pNewGeo, I3_COPY_INSTANCE);

		pIsolateRoot = pNewGeo;

		// Bound Box
		{
			i3BoundBox * pBox = i3BoundBox::new_object_ref();

			pNewGeo->GetWrappingBound( pBox);
			pNewGeo->SetBound( pBox);
		}

		// 손쉬운 처리를 위해 Non-Indexed Gometry로 변환
		{
			i3OptBuildIndexedGeometry opt;
			INT32 i;
			i3RenderAttr * pAttr;

			for( i = 0; i < pNewGeo->GetGeometryAttrCount(); i++)
			{
				pAttr = pNewGeo->GetGeometryAttr( i);

				if( i3::kind_of<i3GeometryAttr*>(pAttr))
				{
					opt.ConvertIndexed2Vertex( (i3GeometryAttr *) pAttr);
				}
			}
		}
	}


	while( pParent != nullptr)
	{
		if( i3::kind_of<i3AttrSet*>(pParent))
		{
			if( pAttrSet == nullptr)
			{
				pAttrSet = i3AttrSet::new_object_ref();

				pAttrSet->AddChild( pNewGeo);
				pAttrSet->SetName( pParent->GetNameString());
				
				pIsolateRoot = pAttrSet;
			}

			i3AttrSet * pTempAttrSet = (i3AttrSet *) pParent;
			
			// Parent AttrSet에 포함되어 있는 모든 Attr들을 모두 모은다.
			for( i = 0; i < pTempAttrSet->GetAttrCount(); i++)
			{
				i3RenderAttr * pAttr = pTempAttrSet->GetAttr( i);

				if( pAttrSet->FindAttr( pAttr->meta()) == nullptr)		// 중복되지 않도록...
					pAttrSet->AddAttr( pAttr);	
			}
		}
		else if( i3::kind_of<i3Transform* >(pParent))
		{
			i3Transform * pNewTrans, * pTrans;

			pTrans = (i3Transform *) pParent;

			pNewTrans = pTrans->create_instance(); // (i3Transform *) CREATEINSTANCE( pParent->GetMeta());

			pTrans->CopyTo( pNewTrans, I3_COPY_REF);

			// 새로운 Root로..
			pNewTrans->AddChild( pIsolateRoot);
			pIsolateRoot = pNewTrans;
		}

		pParent = pParent->GetParent();
	}

	/////////////////////////////
	MESH_INFO * pInfo = new MESH_INFO;

	pInfo->setNode( pIsolateRoot);
	pInfo->refresh();

	addMesh( pInfo);
}

void i3OptOctreeNode::addMesh( MESH_INFO * pInfo)
{
	pInfo->m_idxPlace[0] = pInfo->m_idxPlace[1] = pInfo->m_idxPlace[2] = PLACE_NA;

	m_MeshList.push_back( pInfo);

	m_Bound.ExtendByBoundBox( &pInfo->m_Bound);
}

void i3OptOctreeNode::removeMesh( MESH_INFO * pInfo)
{
//	m_MeshList.Remove( pInfo);
	i3::vu::remove(m_MeshList, pInfo);

	delete pInfo;
}

void i3OptOctreeNode::removeAllMesh(void)
{
	INT32 i;
	MESH_INFO * pMesh;

	for( i = 0; i < getMeshCount(); i++)
	{
		pMesh = getMesh( i);

		delete pMesh;
	}

	m_MeshList.clear();
}

INT32 i3OptOctreeNode::getTotalTriCount(void)
{
	INT32 i, cnt = 0;
	MESH_INFO * pMesh;

	for( i = 0; i < getMeshCount(); i++)
	{
		pMesh = getMesh( i);

		cnt += pMesh->m_TriCount;
	}

	return cnt;
}

void i3OptOctreeNode::vlogAllMesh(void)
{
	#if defined( I3_DEBUG)
	INT32 i;
	MESH_INFO * pMesh;

	for( i = 0; i < getMeshCount(); i++)
	{
		pMesh = getMesh( i);

		pMesh->VLog();
	}
	#endif
}

/////////////////////////////////////////////////////////////////////////////
//
namespace
{
	struct PLANEINFO
	{
		REAL32	m_Span = 0.0f;
		REAL32	m_Pos = 0.0f;
	};
}

static void	_addSpan( i3::vector<PLANEINFO*>& List, REAL32 pos, REAL32 span)
{	
	PLANEINFO * pPlane;

	for(size_t i = 0; i < List.size(); i++)
	{
		pPlane = List[i];

		if( i3Math::abs( pPlane->m_Pos - pos) <= 0.0001f)
		{
			pPlane->m_Span += span;
			return;
		}
	}

	// 해당하는 위치가 없다. 추가.
	pPlane = new PLANEINFO;

	pPlane->m_Pos = pos;
	pPlane->m_Span = span;

	List.push_back( pPlane);
}

static void _removeAllSpan( i3::vector<PLANEINFO*>& List)
{
	
	PLANEINFO * pPlane;

	for(size_t i = 0; i < List.size(); i++)
	{
		pPlane = List[i];

		delete pPlane;
	}
	List.clear(); 
}

// 지정 Axis 방향에서 Triangle의 좌표를 확인해 OcTree의 중앙점이 될 좌표를 선정한다.
REAL32 i3OptOctreeNode::_FindPartitionPoint( i3BoundBox * pBox, VEC3D * pAxis, i3OptBuildOcTree * pOpt)
{
	INT32 j, k;
	MESH_INFO * pMesh;
	i3VertexArray * pVA;
	VEC3D norm, pos[3];
	REAL32 halfsz;
	i3::vector<PLANEINFO*>	PlaneList;
	VEC3D diff;

	// BoundBox의 전체 크기에서 pAxis Basis에서의 크기를 구한다.
	{
		pBox->getSize( &diff);

		halfsz = i3Vector::Dot( &diff, pAxis);

		if( halfsz < pOpt->m_CellSize)
		{
			// 기준 크기보다도 작다.
			VEC3D defCenter;

			defCenter.x = pBox->GetMax()->x;
			defCenter.y = pBox->GetMin()->y;
			defCenter.z = pBox->GetMax()->z; 
			
			return i3Vector::Dot( &defCenter, pAxis);
		}

		halfsz *= 0.5f;
	}

	for(INT32 i = 0; i < getMeshCount(); i++)
	{
		pMesh = getMesh( i);

		for( j = 0; j < pMesh->m_pGeo->GetGeometryAttrCount(); j++)
		{
			i3RenderAttr * pAttr = pMesh->m_pGeo->GetGeometryAttr( j);

			if( !i3::kind_of<i3GeometryAttr*>(pAttr))
				continue;

			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			pVA = ((i3GeometryAttr *) pAttr)->GetVertexArray();

			pVA->Lock( I3G_LOCK_READONLY);

			for( k = 0; k < (INT32) (pGeoAttr->GetPrimitiveCount() * 3); k += 3)
			{
				pVA->GetPositions( k, 3, pos);

				i3Matrix::TransformPoints( pos, pos, 3, &pMesh->m_Transform);

				VEC3D dir1, dir2;

				i3Vector::Sub( &dir1, &pos[0], &pos[1]);
				i3Vector::Sub( &dir2, &pos[1], &pos[2]);
				i3Vector::Cross( &norm, &dir1, &dir2);
				i3Vector::Normalize( &norm, &norm);

				REAL32 dot = i3Vector::Dot( &norm, pAxis);

				if( i3Math::abs( 1.0f - dot) > 0.0001f)
					continue;

				// 방향이 같다.
				
				// 삼각형의 면적을 구한다.
				REAL32 span = ( i3Vector::Length( &dir1) * i3Vector::Length( &dir2) * 0.5f);
				REAL32 w, point;

				// 면적은 중심점에서부터의 거리로 Weight를 준다.
				{
					VEC3D perp;

					REAL32 dist = NearestPlanePoint( &pos[0], &norm, pBox->getCenter(), &perp);

					w = (halfsz - i3Math::abs(dist)) / halfsz;			// 중앙점에서 가까울수록 높은 weight를 받는다.

					if( w < 0.001f)
						w = 0.0f;
				}

				span *= w;

				if( span > I3_EPS)
				{
					point = i3Vector::Dot( &pos[0], pAxis);		// Axis-Basis에서의 좌표를 구한다.

					_addSpan( PlaneList, point, span);
				}
			}

			pVA->Unlock();
		}
	}

	// 얻어진 전체 Span 정보에서 가장 큰 놈을 찾는다.
	PLANEINFO * pMaxPlane = nullptr;

	for(size_t i = 0; i < PlaneList.size(); i++)
	{
		PLANEINFO * pPlane = PlaneList[i];

		if( pMaxPlane == nullptr)
		{
			pMaxPlane = pPlane;
		}
		else
		{
			if( pPlane->m_Span > pMaxPlane->m_Span)
			{
				pMaxPlane = pPlane;
			}
		}
	}

	REAL32 rv;
	
	if( pMaxPlane == nullptr)
	{
		rv = i3Vector::Dot( pBox->getCenter(), pAxis);
	}
	else
		rv = pMaxPlane->m_Pos;

	_removeAllSpan( PlaneList);

	return rv;
}

static void		_setPartitionBox( INT32 idx, i3BoundBox * pSplitBox, VEC3D * pCenter, i3BoundBox * pWholeBox)
{
	static PBSRC	s_idxSrc[8][6] =
	{
		{	PBS_MIN, PBS_MIN, PBS_MIN,		PBS_CTR, PBS_CTR, PBS_CTR },	//	BOTTOM_LEFT_FRONT = 0,		// 0
		{	PBS_MIN, PBS_MIN, PBS_CTR,		PBS_CTR, PBS_CTR, PBS_MAX },	//  BOTTOM_LEFT_BACK,			// 1
		{	PBS_CTR, PBS_MIN, PBS_MIN,		PBS_MAX, PBS_CTR, PBS_CTR },	//  BOTTOM_RIGHT_FRONT,
		{	PBS_CTR, PBS_MIN, PBS_CTR,		PBS_MAX, PBS_CTR, PBS_MAX },	//  BOTTOM_RIGHT_BACK,			// etc...
		{	PBS_MIN, PBS_CTR, PBS_MIN,		PBS_CTR, PBS_MAX, PBS_CTR },	//	TOP_LEFT_FRONT = 0,		// 0
		{	PBS_MIN, PBS_CTR, PBS_CTR,		PBS_CTR, PBS_MAX, PBS_MAX },	//  TOP_LEFT_BACK,			// 1
		{	PBS_CTR, PBS_CTR, PBS_MIN,		PBS_MAX, PBS_MAX, PBS_CTR },	//  TOP_RIGHT_FRONT,
		{	PBS_CTR, PBS_CTR, PBS_CTR,		PBS_MAX, PBS_MAX, PBS_MAX },	//  TOP_RIGHT_BACK,			// etc...
	};

	REAL32 coord[6];
	INT32 i;
	VEC3D * pSrc;

	for( i = 0; i < 6; i++)
	{
        switch( s_idxSrc[idx][i])
		{
			case PBS_MIN :		pSrc = pWholeBox->GetMin();	break;
			case PBS_MAX :		pSrc = pWholeBox->GetMax();	break;
			default :			pSrc = pCenter;
		}

		coord[i] = pSrc->vec[ i % 3];
	}

	pSplitBox->SetMinMax( (VEC3D *) &coord[0], (VEC3D *) &coord[3]);
}

void i3OptOctreeNode::_SplitMesh( VEC4D * pPlane, INT32 axis, i3OptBuildOcTree * pOpt)
{
	INT32 cnt = 0;
	MESH_INFO * pInnerMesh, * pOuterMesh;
	i3::vector<MESH_INFO*> outList;

	for(INT32 i = 0; i < getMeshCount(); i++)
	{
		pInnerMesh = getMesh( i);

		pOuterMesh = pInnerMesh->Cut( pPlane);

		if( pOuterMesh != nullptr)
		{
			pOuterMesh->m_idxPlace[ axis] = PLACE_NEGATIVE;

			outList.push_back( pOuterMesh);
		}

		if( pInnerMesh->m_TriCount <= 0)
		{
			// 모두 사라졌다. 제거.
			//m_MeshList.Delete( i);
			m_MeshList.erase( m_MeshList.begin() + i);
			i--;

			delete pInnerMesh;
		}
		else
		{
			pInnerMesh->m_idxPlace[ axis] = PLACE_POSITIVE;
		}

		cnt ++;
		pOpt->ReportSubProgress( cnt);
	}

	for(size_t i = 0; i < outList.size(); i++)
	{
		MESH_INFO * pMesh = outList[i];

		m_MeshList.push_back( pMesh);
	}
}

bool i3OptOctreeNode::_IsUsedCenter( VEC3D * pCenter)
{
	i3OptOctreeNode * pParent = (i3OptOctreeNode *) GetParent();

	while( pParent != nullptr)
	{
		if( i3Vector::isAlike( pCenter, pParent->getCenter(), 0.0001f))
			return true;

		pParent = (i3OptOctreeNode *) pParent->GetParent();
	}

	return false;
}

void i3OptOctreeNode::_DistributeMesh_Split( bool bSplitX, bool bSplitY, bool bSplitZ, i3OptBuildOcTree * pOpt, VEC3D * pCenter, VEC3D * pSpaceSize)
{
	VEC4D plane;

	// Y 평면으로 자르기
	if( bSplitX && (pSpaceSize->y > pOpt->m_CellSize))
	{
		i3Plane::Set( &plane, pCenter, &I3_YAXIS);

		pOpt->ReportSubStart( getMeshCount(), "Split by the X-Z plane (Y-axis)...");
		_SplitMesh( &plane, 1, pOpt);		// Y-Axis
		pOpt->ReportSubEnd();
	}

	// X 평면으로 자르기
	if( bSplitY && (pSpaceSize->x > pOpt->m_CellSize))
	{
		i3Plane::Set( &plane, pCenter, &I3_XAXIS);

		pOpt->ReportSubStart( getMeshCount(), "Split by the Y-Z plane (X-axis)...");
		_SplitMesh( &plane, 0, pOpt);		// X-Axis
		pOpt->ReportSubEnd();
	}

	// Z 평면으로 자르기
	if( bSplitZ && (pSpaceSize->z > pOpt->m_CellSize))
	{
		i3Plane::Set( &plane, pCenter, &I3_ZAXIS);

		pOpt->ReportSubStart( getMeshCount(), "Split by the X-Y plane (Z-axis)...");
		_SplitMesh( &plane, 2, pOpt);		// Z-Axis
		pOpt->ReportSubEnd();
	}
}

void i3OptOctreeNode::_DistributeMesh_NonSplit( i3OptBuildOcTree * pOpt, VEC3D * pCenter, i3BoundBox * pSpace)
{
	INT32 i, j, idxClosest = -1;
	i3BoundBox splitBox;
	REAL32 dist, minDist;

	for( i = 0; i < getMeshCount(); i++)
	{
		MESH_INFO * pMesh = getMesh( i);

		idxClosest = -1;
		minDist = 1.0E+20f;

		for( j = 0; j < 8; j++)
		{
			_setPartitionBox( j, &splitBox, pCenter, pSpace);

			// Loose octree
			splitBox.Extend( getK());

			if( splitBox.IsWhollyContained( &pMesh->m_Bound))
			{
				VEC3D diff;

				i3Vector::Sub( &diff, splitBox.getCenter(), pMesh->m_Bound.getCenter());
				dist = i3Vector::LengthSq( &diff);

				if( dist < minDist)
				{
					minDist = dist;
					idxClosest = j;
				}
			}
		}

		if( idxClosest != -1)
		{
			pMesh->setOrdinalIndex( idxClosest);
		}
	}
}

void i3OptOctreeNode::Build( i3BoundBox * pSpace, i3OptBuildOcTree * pOpt, char * pszPath)
{
	VEC3D center, SpaceSize;
	INT32 idxChild, i;
	i3BoundBox splitBox;
	bool bSplitX, bSplitY, bSplitZ;

	bSplitX = bSplitY = bSplitZ = true;

	// 공간 분할의 기점이 되는 Position을 찾는다.
	switch( pOpt->m_CenterFindMethod)
	{
		case i3OptBuildOcTree::CFM_MAX_SPAN :
			// 공간적 중앙점에서 가장 거리가 가까우면서도 Polygon Split의 개수가 적은 경우를 찾는다.

			// X-Z 평면에 평행한 Polygon들을 통계. -> 가장 좋은 Y 좌표 결정.
			center.y = _FindPartitionPoint( pSpace, &I3_YAXIS, pOpt);

			// Y-Z 평면에 평행한 Polygon들을 통계. -> 가장 좋은 X 좌표 결정.
			center.x = _FindPartitionPoint( pSpace, &I3_XAXIS, pOpt);

			// X-Y 평면에 평행한 Polygon들을 통계. -> 가장 좋은 Z 좌표 결정.
			center.z = _FindPartitionPoint( pSpace, &I3_ZAXIS, pOpt);
			break;

		case i3OptBuildOcTree::CFM_FIXED :
			i3Vector::Copy( &center, pSpace->getCenter());
			break;
	}

	// 검색된 Center가 부모 Node들에서 이미 사용된 Position이라면 이 Node를 Leaf Node로 삼는다.
	if( _IsUsedCenter( &center))
	{
		bSplitX = bSplitY = bSplitZ = false;	// <- 분할하지 않는다는 것은 모든 Mesh들이 이 Node에 Attach된다.
	}

	I3ASSERT( pSpace->isInside( &center));

	setMinMax( pSpace->GetMin(), pSpace->GetMax());
	setCenter( &center);

	pSpace->getSize( &SpaceSize);

	// 더 깊은 Child Node들로 잘라 들어갈지를 결정
	{
		bool bSplitChild = true;

		// Mesh들의 총 Triangle Count가 일정 제한 이하라면...
		if( (pOpt->getLimitTriangleCountState()) && (getTotalTriCount() <= pOpt->getMinTriangleCount()))
			bSplitChild = false;
		
		// Limit Depth로 도달했다면...
		// 이것은 Option 여부와 상관없이 항상 적용한다.
		if( (INT32) getDepth() >= (pOpt->m_MaxDepth - 1))
			bSplitChild = false;

		if( bSplitChild)
		{
			// Limit Depth에 걸렸다면 분할하지 않고 무조건 현재 Node에 Mesh들을 배치한다.
			if( pOpt->getSplitState())
			{
				_DistributeMesh_Split( bSplitX, bSplitY, bSplitZ, pOpt, &center, &SpaceSize);
			}
			else
			{
				_DistributeMesh_NonSplit( pOpt, &center, pSpace);
			}
		}
	}

	// 분할된 Mesh들을 8개 Child Node에 배당하고 Recursive하게 잘라 들어간다.
	INT32 cnt = 0;
	pOpt->ReportSubStart( getMeshCount(), "Dispatching meshes...");

	for( i = 0; i < getMeshCount(); i++)
	{
		MESH_INFO * pMesh = getMesh( i);

		cnt++;
		pOpt->ReportSubProgress( cnt);

		if( (pMesh->m_idxPlace[0] == PLACE_NA) && (pMesh->m_idxPlace[1] == PLACE_NA) && (pMesh->m_idxPlace[2] == PLACE_NA))
		{
			// 어디에도 배당되지 않았다. 즉, 현재 Node에 더해져야 한다.
			this->AddChild( pMesh->m_pNode);

			// 다른 Node에 배당되지 않기 때문에 Mesh 정보는 삭제한다.
			delete pMesh;
		}
		else
		{

			idxChild = pMesh->getOrdinalIndex();

			i3OptOctreeNode * pChild = (i3OptOctreeNode *) getChildOctree( (DIVIDE_NODE) idxChild);

			if( pChild == nullptr)
			{
				static char *	s_SpaceName[8] =
				{
					"DLF",							// TOP_LEFT_FRONT = 0,		// 0
					"DLB",							// TOP_LEFT_BACK,			// 1
					"DRF",							// TOP_RIGHT_BACK,			// etc...
					"DRB",							// TOP_RIGHT_FRONT,
					"ULF",							// BOTTOM_LEFT_FRONT,
					"ULB",							// BOTTOM_LEFT_BACK,
					"URF",							// BOTTOM_RIGHT_BACK,
					"URB",							// BOTTOM_RIGHT_FRONT,
				};

				pChild = i3OptOctreeNode::new_object_ref();

				pChild->SetName( s_SpaceName[ idxChild]);
				pChild->setDepth( getDepth() + 1);
				pChild->setMaxDepth( getMaxDepth());
				pChild->setK( getK());

				SetChildOctree( (DIVIDE_NODE) idxChild, pChild);
			}

			// 해당 Child Node에 Mesh를 넘긴다.
			pChild->addMesh( pMesh);
		}
	}

	// Mesh 정보가 더 이상 필요하지 않다.
	m_MeshList.clear();

	// 8개 Child Node를 Recursive하게 돌아준다.
	{
		INT32 childCount = 0;

		for( i = 0; i < 8; i ++)
		{
			i3OptOctreeNode * pChild = (i3OptOctreeNode *) getChildOctree( (DIVIDE_NODE) i);

			if( pChild == nullptr)
				continue;

			_setPartitionBox( i, &splitBox, &center, pSpace);

			char conv[512];

			sprintf( conv, "%s-%s[%d]", pszPath, pChild->GetName(), i);

			pOpt->_ProgressInc( conv);

			pChild->Build( &splitBox, pOpt, conv);

			childCount++;
		}

		if( childCount == 0)
			addOctreeStyle( I3_OCTST_LEAF);
	}
}

i3OcTreeNode *	i3OptOctreeNode::ConvertTo( i3OcTreeNode * pParent)
{
	INT32 i;
	i3OcTreeNode * pDest;

	pDest = i3OcTreeNode::new_object_ref();

	pDest->setOctreeStyle( getOctreeStyle());

	// Size
	pDest->setMinMax( getMin(), getMax());

	// Center
	pDest->setCenter( getCenter());

	// Depth
	pDest->setDepth( getDepth());
	pDest->setMaxDepth( getMaxDepth());

	pDest->setK( getK());

	// Bound Box
	//pDest->SetBound( GetBound());

	// Child Node들을 모두 이동.
	{
		// Optimize를 위해 임시로 사라질 수 있는 Node를 준비해준다.
		i3SceneGraphInfo * pSgInfo = i3SceneGraphInfo::new_object();
		i3Node * pTempRoot = i3Node::new_object();
		pTempRoot->SetName( "OctreeGeo");

		pSgInfo->setInstanceSg( pTempRoot);

		while( GetChildCount() > 0)
		{
			i3Node * pChild = GetChild( 0);
			pChild->ChangeParent( pTempRoot);
		}

		// Attribute Gathering
		{
			i3OptSortNodeByAttr opt;

			opt.Optimize( pSgInfo);
		}

		// Gathering geometry
		{
			i3OptSpatialPartition opt;

			opt.setSplitEnable( false);
			opt.Optimize( pSgInfo);
		}

		/// Optimize가 완료된 Node를 pDest의 Child로 붙인다.
		pDest->AddChild( pSgInfo->getInstanceSg());

		I3_SAFE_RELEASE( pTempRoot);
		I3_SAFE_RELEASE( pSgInfo);
	}

	// Recursive
	for( i = 0; i < 8; i++)
	{
		i3OptOctreeNode * pChild = (i3OptOctreeNode *) getChildOctree( (DIVIDE_NODE) i);

		if( pChild == nullptr)
			continue;

		i3OcTreeNode * pNewChild = pChild->ConvertTo( pDest);

		pDest->SetChildOctree( (DIVIDE_NODE) i, pNewChild);
	}

	return pDest;
}
