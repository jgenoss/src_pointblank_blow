#include "i3OptPCH.h"
#include "i3OptCompactMesh.h"

//I3_CLASS_INSTANCE( i3OptCompactMesh, i3OptGeometryBase);
I3_CLASS_INSTANCE( i3OptCompactMesh);

i3OptCompactMesh::i3OptCompactMesh(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool	i3OptCompactMesh::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( !i3::kind_of<i3Geometry* >(pNode))
		return true;

	i3Geometry * pGeo = (i3Geometry *) pNode;
	INT32 i;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		m_GeoList.push_back( pGeoAttr);

		i3VertexArray * pVA = pGeoAttr->GetVertexArray();

		INT32 idx = FindVA( pVA);

		if( idx == -1)
		{
			// This vertex array have not been added to VAList
			idx = AddVA( pVA, nullptr);
		}
	}

	return true;
}

void i3OptCompactMesh::_SearchSameFormat( VAINFO * pStart, i3::vector<VAINFO*>& Group)
{
	INT32 i;
	i3VertexFormat * pSrcFormat, * pDestFormat;

	Group.push_back( pStart);

	pSrcFormat = pStart->m_pOldVA->GetFormat();

	for( i = 0; i < getVACount(); i++)
	{
		VAINFO * pTemp = getVA( i);

		if( pTemp == pStart)
			continue;

		pDestFormat = pTemp->m_pOldVA->GetFormat();

		if( *pSrcFormat == *pDestFormat)
		{
			Group.push_back( pTemp);
		}
	}
}

INT32	i3OptCompactMesh::_FindVertex( INT32 idx, i3VertexArray * pOldVA, i3VertexArray * pNewVA, INT32 newVACount)
{
	INT32 i;

	for( i = 0; i < newVACount; i++)
	{
		if( pNewVA->IsSameVertex( i, pOldVA, idx))
			return i;
	}

	return -1;
}

INT32	i3OptCompactMesh::_MergeVA( i3VertexArray * pOldVA, i3VertexArray * pNewVA, INT32 newVACount, INT32 * pIndexTable)
{
	INT32 i, idx;

	for( i = 0; i < (INT32) pOldVA->GetCount(); i++)
	{
		idx = _FindVertex( i, pOldVA, pNewVA, newVACount);

		if( idx == -1)
		{
			// 새로운 vertex -> 추가
			pNewVA->CopyVertexFrom( newVACount, pOldVA, i);

			idx = newVACount;
			newVACount++;
		}
	
		pIndexTable[ i] = idx;
	}

	return newVACount;
}

void	i3OptCompactMesh::_MergeGroup( i3::vector<VAINFO*>& Group)
{
	INT32 totalCount = 0;
	VAINFO * pInfo;
	i3VertexArray * pVA, * pNewVA;
	i3VertexFormat fmt;

	if( Group.size() < 2)
		return;

	// 모두 합친 Vertex Array를 추정
	for(size_t i = 0; i < Group.size(); i++)
	{
		pInfo = Group[i];

		pVA = pInfo->m_pOldVA;

		fmt = * pVA->GetFormat();

		totalCount += pVA->GetCount();
	}

	pNewVA = i3VertexArray::new_object();
	pNewVA->Create( &fmt, totalCount, 0);

	pNewVA->Lock();

	totalCount = 0; 

	for(size_t i = 0; i <  Group.size(); i++)
	{
		pInfo = Group[i];

		pVA = pInfo->m_pOldVA;
		pInfo->m_pNewVA = pNewVA;

		// Index Table 생성
		pInfo->m_pIndexTable = (INT32 *) i3MemAlloc( sizeof(INT32) * pVA->GetCount());

		pVA->Lock();

		totalCount = _MergeVA( pVA, pNewVA, totalCount, pInfo->m_pIndexTable);

		pVA->Unlock();
	}

	I3TRACE( "Count : %d - %d\n", totalCount, pNewVA->GetCount());
	I3ASSERT( totalCount <= (INT32) pNewVA->GetCount());

	pNewVA->Unlock();

	pNewVA->ForceCount( totalCount);
}

void	i3OptCompactMesh::_ReplaceVA( i3GeometryAttr * pAttr, VAINFO * pInfo)
{
	INT32 i;
	i3IndexArray * pIA = pAttr->GetIndexArray();
	i3VertexArray * pVA = pAttr->GetVertexArray();

	I3ASSERT( pVA == pInfo->m_pOldVA);
	I3ASSERT( pInfo->m_pNewVA != nullptr);

	if( pIA == nullptr)
	{
		pIA = i3IndexArray::new_object_ref();
		pIA->Create( pVA->GetCount(), 0, pVA->GetCount());

		pIA->Lock();

		for( i = 0; i < (INT32) pIA->GetCount(); i++)
		{
			pIA->SetIndex( i, pInfo->m_pIndexTable[ i]);
		}

		pIA->Unlock();

		pAttr->SetIndexArray( pIA);
	}
	else
	{
		pIA->Lock();

		for( i = 0; i < (INT32) pIA->GetCount(); i++)
		{
			UINT16 idxNew, idxOld;

			idxOld = (UINT16) pIA->GetIndex( i);

			idxNew = (UINT16) pInfo->m_pIndexTable[ idxOld];

			pIA->SetIndex( i, idxNew);
		}

		pIA->Unlock();
	}

	pAttr->SetVertexArray( pInfo->m_pNewVA);
}

void	i3OptCompactMesh::Trace( i3Node * pRoot)
{
	INT32 i;
	VAINFO * pInfo;
	i3::vector<VAINFO*>	group;

	i3SceneOptimizer::Trace( pRoot);

	// Remove unused vertex component from vertex arrays which are gathered on traversing,
	for( i = 0; i < getVACount(); i++)
	{
		pInfo = (VAINFO *) getVA( i);

		if( pInfo->m_pNewVA != nullptr)
			continue;

		// 같은 Format의 VA들을 검색해 List로 구성,
		_SearchSameFormat( pInfo, group);

		_MergeGroup( group);

		group.clear();
	}

	// 새롭게 생성된 VertexArray를 각 GeometryAttr에 설정.
	for( i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pAttr = getGeoAttr( i);

		if( pAttr->GetVertexArray() == nullptr)
			continue;

		INT32 idxVA = FindVA( pAttr->GetVertexArray());
		I3ASSERT( idxVA != -1);

		pInfo = getVA( idxVA);

		if( pInfo->m_pNewVA != nullptr)
		{
			// 새로운 Vertex Array가 있으므로 대체.
			_ReplaceVA( pAttr, pInfo);
		}
	}

	RemoveAllVA();
	RemoveAllGeo();
}