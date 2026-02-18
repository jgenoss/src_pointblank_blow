#include "i3OptPCH.h"
#include "i3OptSplitGeometry.h"
#include "i3OptTriSplitNode.h"

I3_CLASS_INSTANCE( i3OptSplitGeometry);

//#define VLOG_MESH

i3OptSplitGeometry::i3OptSplitGeometry(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptTriSplitNode * i3OptSplitGeometry::addTri( i3VertexArray * pVA, i3IndexArray * pIA, INT32 idx0, INT32 idx1, INT32 idx2)
{
	VEC3D pos[3];
	i3OptTriSplitNode * pGroup = nullptr;

	pVA->GetPosition( idx0, &pos[0]);
	pVA->GetPosition( idx1, &pos[1]);
	pVA->GetPosition( idx2, &pos[2]);

	// 해당하는 Group이 없다. 추가한다.
	pGroup = i3OptTriSplitNode::new_object();

	m_GroupList.push_back( pGroup);

	pGroup->AddTri( pVA, pIA, idx0, idx1, idx2);

	return pGroup;
}

void i3OptSplitGeometry::removeAllGroup(void)
{
	INT32 i;

	for( i = 0; i < getGroupCount(); i++)
	{
		i3OptTriSplitNode * pGroup = getGroup( i);
		
	//	delete pGroup;
		i3::destroy_instance(pGroup);
	}

	m_GroupList.clear();
}

void i3OptSplitGeometry::_FindAndAdd_SharedTri( i3OptTriSplitNode * pGroup, i3VertexArray * pVA, i3IndexArray * pIA, INT32 idxStart, i3::vector<i3OptTriSplitNode*>& UsedList)
{
	INT32 cnt;
	VEC3D srcPos[3], destPos[3];
	i3::vector<INT32> addList;
	INT32 idx0, idx1, idx2;

	if( pIA != nullptr)
	{
		idx0 = pIA->GetIndex( idxStart + 0);
		idx1 = pIA->GetIndex( idxStart + 1);
		idx2 = pIA->GetIndex( idxStart + 2);

		cnt = pIA->GetCount();
	}
	else
	{
		idx0 = idxStart + 0;
		idx1 = idxStart + 1;
		idx2 = idxStart + 2;

		cnt = pVA->GetCount();
	}

	pVA->GetPosition( idx0, &srcPos[0]);
	pVA->GetPosition( idx1, &srcPos[1]);
	pVA->GetPosition( idx2, &srcPos[2]);

	for(INT32 i = 0; i < cnt; i += 3)
	{
		INT32 idx = (i / 3);

		if( UsedList[ idx] != nullptr)
			continue;		// 이미 확인된 Triangle은 제외.

		if( pIA != nullptr)
		{
			idx0 = pIA->GetIndex( i + 0);
			idx1 = pIA->GetIndex( i + 1);
			idx2 = pIA->GetIndex( i + 2);
		}
		else
		{
			idx0 = i + 0;
			idx1 = i + 1;
			idx2 = i + 2;
		}

		pVA->GetPosition( idx0, &destPos[0]);
		pVA->GetPosition( idx1, &destPos[1]);
		pVA->GetPosition( idx2, &destPos[2]);

		if( i3Math::CheckSharedEdge( srcPos, destPos))
		{
			UsedList[ idx] = pGroup;

			addList.push_back(i);
		}
	}

	for(size_t  i = 0; i < addList.size(); i++)
	{
		INT32 idxFind = addList[i];

		if( pIA != nullptr)
		{
			idx0 = pIA->GetIndex( idxFind + 0);
			idx1 = pIA->GetIndex( idxFind + 1);
			idx2 = pIA->GetIndex( idxFind + 2);
		}
		else
		{
			idx0 = idxFind + 0;
			idx1 = idxFind + 1;
			idx2 = idxFind + 2;
		}

		pGroup->AddTri( pVA, pIA, idx0, idx1, idx2);

		// Recursive process
		// Edge를 공유한다고 판단된 Triangle들에 대해 다시 Recursive하게 들어가 판단해
		// 몽땅 걸러낸다.
		_FindAndAdd_SharedTri( pGroup, pVA, pIA, idxFind, UsedList);
	}
}

void i3OptSplitGeometry::_Split( i3Geometry * pSrcGeo, i3Node * pParentNode)
{
	// pSrcGeo에 포함되어 있는 Triangle들의 연결성을 확인해
	// 하나의 Mesh로 이루어진 단위로 개별적인 i3Geometry Node로 구분해준다.
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	i3::vector<i3OptTriSplitNode*>	UsedList;
	INT32 i, j, cnt;
	i3OptTriSplitNode * pGroup;
	INT32	srcTriCount = 0, destTriCount = 0;

	for( i = 0; i < pSrcGeo->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = pSrcGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

		I3ASSERT( pGeoAttr->GetPrimitiveType() == I3G_PRIM_TRILIST);

		pVA = pGeoAttr->GetVertexArray();
		pVA->Lock( I3G_LOCK_READONLY);

		pIA = pGeoAttr->GetIndexArray();
		if( pIA != nullptr)
			pIA->Lock();

		// Used Flag Update
		{
			if( pIA != nullptr)
				cnt = pIA->GetCount();
			else
				cnt = pVA->GetCount();

			srcTriCount += (cnt / 3);
			UsedList.resize( (cnt / 3) );

			for( j = 0; j < (cnt / 3); j++)
			{
				UsedList[ j] = nullptr;
			}
		}

		ReportSubStart( pVA->GetCount());

		for( j = 0; j < (INT32) cnt; j += 3)
		{
			ReportSubProgress( j);

			if( UsedList[ j / 3] != nullptr)
			{
				continue;			// 이미 검색이 된 Tri이다.
			}

			INT32 idx0, idx1, idx2;

			if( pIA != nullptr)
			{
				idx0 = pIA->GetIndex( j);
				idx1 = pIA->GetIndex( j + 1);
				idx2 = pIA->GetIndex( j + 2);
			}
			else
			{
				idx0 = j;
				idx1 = j + 1;
				idx2 = j + 2;
			}

			// 한번도 검색되지 않는 놈이다.
			pGroup = addTri( pVA, pIA, idx0, idx1, idx2);

			UsedList[ j / 3] = pGroup;

			// 현 Tri의 아래에 존재하는 Tri들 중이 이 Tri와 Edge를 공유하는 것이 있는지 확인한다.
			_FindAndAdd_SharedTri( pGroup, pVA, pIA, j, UsedList);
		}

		pVA->Unlock();

		if( pIA != nullptr)
			pIA->Unlock();
	}

	// Group들을 확인하며 새로운 i3Geometry로 구성해 준다.
	{
		for( i = 0; i < getGroupCount(); i++)
		{
			pGroup = getGroup( i);

			i3Geometry * pNewGeo = pGroup->CreateGatheredGeometry();

			destTriCount += pGroup->getTriCount();

			pParentNode->AddChild( pNewGeo);
		}
	}

	{
		I3TRACE( "%d triangles -> %d triangles\n", srcTriCount, destTriCount);
		I3ASSERT( srcTriCount == destTriCount);
	}

	ReportSubEnd();

	removeAllGroup();
}

void i3OptSplitGeometry::Trace( i3Node * pRoot)
{
	i3::vector<i3Node*>		geoList;
	
	i3Geometry * pGeo;
	i3Node * pParent;

	i3Scene::FindNodeByExactType( pRoot, i3Geometry::static_meta(), geoList);

	ReportStart( INT32(geoList.size()), "Splitting...");

	for(size_t i = 0; i < geoList.size(); i++)
	{
		pGeo = (i3Geometry *) geoList[i];

		ReportProgress( i, pGeo->GetName());

		I3_MUST_ADDREF( pGeo);

		pParent = pGeo->GetParent();
		pGeo->RemoveFromParent();

		_Split( pGeo, pParent);

		I3_MUST_RELEASE( pGeo);
	}

	INT32 oldCount = INT32(geoList.size());

	geoList.clear();

	i3Scene::FindNodeByExactType( pRoot, i3Geometry::static_meta(), geoList);

	{
		char conv[256];

		sprintf( conv, "* Total %d geometries are splitted up in %d pieces.", oldCount, geoList.size());
		ReportProgress( oldCount, conv);
	}

	ReportEnd();
}

const char *	i3OptSplitGeometry::getDesc(void)
{
	return "공간적으로 떨어진 Geometry를 분석해 별도의 Mesh로 분류";
}

bool i3OptSplitGeometry::isSupportProgress(void)
{
	return true;
}