#include "i3OptPCH.h"
#include "i3OptSpatialPartition.h"
#include "i3OptCreateBoundBox.h"

I3_CLASS_INSTANCE( i3OptSpatialPartition);

i3OptSpatialPartition::i3OptSpatialPartition(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	SetMatrixStackEnable(true);
}

INT32	i3OptSpatialPartition::_GetChildGeometry( i3Node * pParent)
{
	
	INT32 i, cnt = 0;
	i3Node * pChild;

	for( i = 0; i < pParent->GetChildCount(); i++)
	{
		pChild = pParent->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_ATTACH_POINT))
			continue;

		if( i3::kind_of<i3Geometry*>(pChild))
		{
			cnt++;
		}
	}

	return cnt;
}

INT32	i3OptSpatialPartition::_GetChildGeometry( i3Node * pParent, i3::vector<i3Geometry*>& List)
{
	INT32 i, cnt = 0;
	i3Node * pChild;

	for( i = 0; i < pParent->GetChildCount(); i++)
	{
		pChild = pParent->GetChild( i);

		if( pChild->IsFlag( I3_NODEFLAG_ATTACH_POINT))
			continue;

		if( i3::kind_of<i3Geometry* >(pChild))
		{
			cnt++;
			List.push_back( static_cast<i3Geometry*>(pChild));
		}
	}

	return cnt;
}

I3OPT_VERTEX_GROUP *	i3OptSpatialPartition::_GetVertexGroup( i3::vector<I3OPT_VERTEX_GROUP*>& GroupList, i3VertexFormat * pFormat)
{
	I3OPT_VERTEX_GROUP * pGroup;

	for(size_t i = 0; i < GroupList.size(); i++)
	{
		pGroup = GroupList[i];

		if( pGroup->m_Format == *pFormat)
		{
			return pGroup;
		}
	}

	pGroup = new I3OPT_VERTEX_GROUP;
	pGroup->m_Format = * pFormat;
	pGroup->m_idxCur = 0;
	pGroup->m_MaxCount = 0;
	pGroup->m_pVA = nullptr;

	GroupList.push_back( pGroup);

	return pGroup;
}

void i3OptSpatialPartition::_MergeGeometry( i3Node * pNode)
{
	i3::vector<i3Geometry*> geoList;
	i3::vector<I3OPT_VERTEX_GROUP*> groupList;
	INT32  j;
	i3Geometry * pGeo;

	_GetChildGeometry( pNode, geoList);

	if( geoList.size() <= 1)
		return;

	// Merge할 Triangle의 최대 개수를 구한다.
	for(size_t i = 0; i < geoList.size(); i++)
	{
		pGeo = geoList[i];

		for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

			if( !i3::kind_of<i3GeometryAttr*>(pAttr))
				continue;

			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			I3OPT_VERTEX_GROUP * pGroup = _GetVertexGroup( groupList, pGeoAttr->GetVertexArray()->GetFormat());

			if( pGeoAttr->GetPrimitiveLengthArray() != nullptr)
			{
				for( UINT32 k = 0; k < pGeoAttr->GetPrimitiveCount(); k++)
				{
					pGroup->m_MaxCount += pGeoAttr->GetPrimitiveLength( k);
				}
			}
			else
			{
				pGroup->m_MaxCount += pGeoAttr->GetPrimitiveCount();
			}
		}
	}

	// 새로운 Vertex Array의 준비
	for(size_t i = 0; i < groupList.size(); i++)
	{
		I3OPT_VERTEX_GROUP * pGroup = groupList[i];

		I3ASSERT( pGroup->m_MaxCount > 0);

		pGroup->m_pVA = i3VertexArray::new_object_ref();
		pGroup->m_pVA->Create( & pGroup->m_Format, pGroup->m_MaxCount * 3, 0);
		pGroup->m_pVA->Lock();
	}

	// 다시 i3Geometry Node들을 돌면서 커다란 새로운 Geometry를 구성한다.
	for(size_t i = 0; i < geoList.size(); i++)
	{
		pGeo = geoList[i];

		for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

			if( !i3::kind_of<i3GeometryAttr*>(pAttr))
				continue;

			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			// 문제를 간단하게 만들기 위해 처리가 끝난 i3GeometryAttr은 제거한다.
			I3_MUST_ADDREF(pGeoAttr);

			pGeo->RemoveGeometryAttr( pGeoAttr);
			j--;

			i3GeometryResolver resolver;

			I3OPT_VERTEX_GROUP * pGroup = _GetVertexGroup( groupList, pGeoAttr->GetVertexArray()->GetFormat());
			
			resolver.setGeometryAttr( pGeoAttr);

			for( INT32 k = 0; k < resolver.getPrimitiveCount(); k++)
			{
				INT32 idx[3];

				resolver.getVertexIndices( k, idx);

				for( INT32 tri = 0; tri < 3; tri++)
				{
					pGroup->m_pVA->CopyVertexFrom( (pGroup->m_idxCur * 3) + tri, pGeoAttr->GetVertexArray(), idx[tri]);
				}

				pGroup->m_idxCur ++;

				I3ASSERT( pGroup->m_idxCur <= pGroup->m_MaxCount);
			}
			
			I3_MUST_RELEASE(pGeoAttr);
		}
	}

	// 불필요해진 i3Geometry Child Node들을 제거한다.
	for(size_t i = 0; i < geoList.size(); i++)
	{
		pGeo = geoList[i];

		if( pGeo->GetGeometryAttrCount() == 0)
		{
			pNode->RemoveChild( pGeo);
		}
	}

	// 새로 만들어진 커다란 Geometry를 ChildNode로 추가한다.
	for(size_t i = 0; i < groupList.size(); i++)
	{
		I3OPT_VERTEX_GROUP * pGroup = groupList[i];

		pGeo = i3Geometry::new_object_ref();

		pGeo->SetName( "MergedGeo");
		pNode->AddChild( pGeo);

		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pGeoAttr->SetVertexArray( pGroup->m_pVA, I3G_PRIM_TRILIST, pGroup->m_MaxCount);

		delete pGroup;
	}
}

inline INT32 _getOrdinal( INT32 x, INT32 z, INT32 y)
{
	return (y * 4) + (z * 2) + (x);
}

static UINT8		s_Color[8][3] =
{
	{	255,	0,		0	},
	{	0,		255,	0	},
	{	0,		0,		255 },
	{	255,	128,	0	},

	{	128,	255,	0	},
	{	0,		128,	255 },
	{	255,	0,		128 },
	{	0,		255,	128 },
};

i3Node * i3OptSpatialPartition::_Rec_SplitGeo( i3Geometry * pGeo, i3Node * pParent)
{
	i3BoundBox box;
	VEC4D plane[3];
	i3Geometry *	pChild[8];
	i3VertexArray *	pNewVA[8], * pTemplateVA = nullptr;
	INT32 cnt[8], totalCnt, sideCnt;
	bool	bCheck[3];
	INT32 i, x, y, z, ord;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;
	VEC3D p[3];
	VEC3D c;
	bool bDump = false;

	pGeo->GetWrappingBound( &box);

	// 공간적으로 이미 충분히 작거나, 또는 Triangle의 개수가 
	// 기준보다 적다면 더 이상 자르지 않는다.
	{
		VEC3D sz;
		
		i3Vector::Sub( &sz, box.GetMax(), box.GetMin());

		if( m_Size.x > 0.0f)
			bCheck[0] = ( sz.x > m_Size.x);
		else
			bCheck[0] = false;

		if( m_Size.z > 0.0f)
			bCheck[1] = ( sz.z > m_Size.z);
		else
			bCheck[1] = false;

		if( m_Size.y > 0.0f)
			bCheck[2] = ( sz.y > m_Size.y);
		else
			bCheck[2] = false;

		if( !bCheck[0] && !bCheck[1] && !bCheck[2])
			return nullptr;
	}

	// Split Planes
	{
		i3Vector::Add( &c, box.GetMin(), box.GetMax());
		i3Vector::Scale( &c, &c, 0.5f);

		i3Plane::Set( &plane[0], &c, & I3_ZAXIS);
		i3Plane::Set( &plane[1], &c, & I3_XAXIS);
		i3Plane::Set( &plane[2], &c, & I3_YAXIS);
	}

	i3mem::FillZero( cnt, sizeof(cnt));
	i3mem::FillZero( pNewVA, sizeof(pNewVA));

	i3GeometryResolver resolver;

	// 8개 면에 대한 Triangle의 개수를 구한다.
	totalCnt = 0;
	sideCnt = 0;

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr*>(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		resolver.setGeometryAttr( pGeoAttr);

		if( pTemplateVA == nullptr)
		{
			pTemplateVA = pGeoAttr->GetVertexArray();
		}

		for( INT32 j = 0; j < resolver.getPrimitiveCount(); j++)
		{
			resolver.getPositions( j, p);

			if( bCheck[0] && (i3Plane::getSide( &plane[0], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				x = 0;
			else
				x = 1;

			if( bCheck[1] && (i3Plane::getSide( &plane[1], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				z = 0;
			else
				z = 1;

			if( bCheck[2] && (i3Plane::getSide( &plane[2], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				y = 0;
			else
				y = 1;

			ord = _getOrdinal( x, z, y);

			cnt[ord] ++;
			totalCnt ++;
		}
	}
	
	I3TRACE( "totalCnt : %d\n", totalCnt);

	if( totalCnt <= m_LimitTriCount)
		return nullptr;

	i3Node * pNewParent = i3Node::new_object_ref();

	// 0 이상의 Triangle을 가진 Side에 대해 Node를 준비하고 분할한다.
	for( i = 0; i < 8; i++)
	{
		if( cnt[i] > 0)
		{
			pNewVA[i] = i3VertexArray::new_object_ref();
			pNewVA[i]->Create( pTemplateVA->GetFormat(), cnt[i] * 3, 0);
			pNewVA[i]->Lock();

			pGeoAttr = i3GeometryAttr::new_object_ref();
			pGeoAttr->SetVertexArray( pNewVA[i], I3G_PRIM_TRILIST, cnt[i]);

			pChild[i] = i3Geometry::new_object_ref();
			pChild[i]->AppendGeometryAttr( pGeoAttr);

			pNewParent->AddChild( pChild[i]);
		}
		else
		{
			pChild[i] = nullptr;
			pNewVA[i] = nullptr;
		}

		// Reset
		cnt[i] = 0;
	}

	if( bDump)
	{
		i3VLog::Begin();

		i3VLog::SetColor( (UINT8) 64, 64, 64, 255);
		i3VLog::Draw( &box);
	}

	// 실질적인 Triangle의 배분.
	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		resolver.setGeometryAttr( pGeoAttr);

		pTemplateVA = pGeoAttr->GetVertexArray();

		for( INT32 j = 0; j < resolver.getPrimitiveCount(); j++)
		{
			INT32 idx[3];

			resolver.getPositions( j, p);
			resolver.getVertexIndices( j, idx);

			if( bCheck[0] && (i3Plane::getSide( &plane[0], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				x = 0;
			else
				x = 1;

			if( bCheck[1] && (i3Plane::getSide( &plane[1], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				z = 0;
			else
				z = 1;

			if( bCheck[2] && (i3Plane::getSide( &plane[2], &p[0], &p[1], &p[2]) == I3_FACE_FRONT))
				y = 0;
			else
				y = 1;

			ord = _getOrdinal( x, z, y);

			I3ASSERT( pNewVA[ord] != nullptr);

			for( INT32 k = 0; k < 3; k++)
			{
				pNewVA[ ord]->CopyVertexFrom( (cnt[ord] * 3) + k, pTemplateVA, idx[k]);
			}

			if( bDump)
			{
				i3VLog::SetColor( s_Color[ ord][0], s_Color[ ord][1], s_Color[ ord][2], 255);
				i3VLog::Tri( &p[0], &p[1], &p[2]);
			}

			cnt[ord]++;
		}
	}

	if( bDump)
		i3VLog::End();

	for( i = 0; i < 8; i++)
	{
		if( pChild[i] != nullptr)
		{
			pNewVA[i]->Unlock();
		}
	}

	return pNewParent;
}

void i3OptSpatialPartition::_SplitGeo( i3Node * pParent)
{
	INT32 i;
	i3Node * pNewChild;
//	List childList; -->쓰이지 않는다..

	// 처리 도중 Child List가 변경될 수 있기 때문에..
	// 별도의 Child List를 만들어 처리한다.
	for( i = 0; i < pParent->GetChildCount(); i++)
	{
		i3Node * pChild = pParent->GetChild( i);

		if( i3::kind_of<i3Geometry* >(pChild))
		{
			pNewChild = _Rec_SplitGeo( (i3Geometry *) pChild, nullptr);

			if( pNewChild != nullptr)
			{
				I3_MUST_RELEASE(pChild);
				I3_MUST_ADDREF(pNewChild);

				pParent->SetChild( i, pNewChild);

				pChild = pNewChild;
			}
		}

		_SplitGeo( pChild);
	}
}

bool	i3OptSpatialPartition::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( _GetChildGeometry( pNode) <= 0)
		return true;

	// 우선 모든 Child Node들을 합한다.
	_MergeGeometry( pNode);
	
	return true;
}

void i3OptSpatialPartition::Trace( i3Node * pRoot)
{
	i3SceneOptimizer::Trace( pRoot);

	if( m_bSplit)
	{
		_SplitGeo( pRoot);
	}

	if( m_bCreateBoundBox)
	{
		i3OptCreateBoundBox opt;

		opt.Trace( pRoot);
	}
}

bool i3OptSpatialPartition::SaveToXML( i3XMLElement * pXML)
{
	pXML->addAttr( "SizeX", m_Size.x);
	pXML->addAttr( "SizeY", m_Size.y);
	pXML->addAttr( "SizeZ", m_Size.z);
	pXML->addAttr( "LimitTriCount", m_LimitTriCount);
	pXML->addAttr( "Split", m_bSplit);
	pXML->addAttr( "CreateBoundBox", m_bCreateBoundBox);

	return true;
}

bool i3OptSpatialPartition::LoadFromXML( i3XMLElement * pXML)
{
	INT32 val;

	pXML->getAttr( "SizeX", &m_Size.x);
	pXML->getAttr( "SizeY", &m_Size.y);
	pXML->getAttr( "SizeZ", &m_Size.z);
	pXML->getAttr( "LimitTriCount", &m_LimitTriCount);

	pXML->getAttr( "Split", &val);
	m_bSplit = (val != 0);

	pXML->getAttr( "CreateBoundBox", &val);
	m_bCreateBoundBox = (val != 0);

	return true;
}
