#include "i3OptPCH.h"
#include "i3OptGeometryBase.h"

I3_CLASS_INSTANCE( i3OptGeometryBase);

i3OptGeometryBase::~i3OptGeometryBase(void)
{
	RemoveAllVA();
	RemoveAllGeo();
}

INT32 i3OptGeometryBase::AddVA( i3VertexArray * pVA, MATRIX * pMatrix)
{
	VAINFO * pInfo;

	pInfo = (VAINFO *) i3MemAlloc( sizeof(VAINFO));
	pInfo->m_pOldVA = pVA;
	pInfo->m_pNewVA = nullptr;

	if( pMatrix != nullptr)
		i3Matrix::Copy( &pInfo->m_World, pMatrix);
	INT32 idx = INT32(m_VAList.size());
	m_VAList.push_back( pInfo);
	return idx;
}

INT32 i3OptGeometryBase::FindVA( i3VertexArray * pVA)
{
	INT32 i;

	for( i = 0; i < getVACount(); i++)
	{
		VAINFO * pInfo = getVA( i);

		if( pInfo->m_pOldVA == pVA)
			return i;
	}

	return -1;
}

void i3OptGeometryBase::RemoveAllVA(void)
{
	INT32 i;

	for( i = 0; i < getVACount(); i++)
	{
		VAINFO * pInfo = getVA( i);

		pInfo->Destroy();

		i3MemFree( pInfo);
	}

	m_VAList.clear();
}

void i3OptGeometryBase::GetWorldMatrix( i3Node * pNode, MATRIX * pMatrix)
{
	i3Matrix::Identity( pMatrix);

	while( pNode->GetParent() != nullptr)
	{
		i3Node * pParent = (i3Node *) pNode->GetParent();

		if( i3::kind_of<i3Transform* >(pParent))
		{
			i3Transform * pTrans = (i3Transform *) pParent;

			i3Matrix::Mul( pMatrix, pMatrix, pTrans->GetMatrix());
		}

		pNode = pParent;
	}
}

// Look for i3Geometry node from a given scene-graph.
// Founded i3VertexArray is registered to VAList.
void i3OptGeometryBase::GatherAllVertexArray( i3Node * pRoot)
{
	INT32 j;
	i3::vector<i3Node*> list;
	i3Geometry * pGeo;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	MATRIX mtx;

	i3Scene::FindNodeByType( pRoot, i3Geometry::static_meta(), list);

	for(size_t i = 0; i < list.size(); i++)
	{
		pGeo = (i3Geometry *) list[i];

		GetWorldMatrix( pGeo, &mtx);

		for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

			if( i3::kind_of<i3GeometryAttr* >(pAttr))
			{
				pGeoAttr = (i3GeometryAttr *) pAttr;

				m_GeoList.push_back( pGeoAttr);

				pVA = pGeoAttr->GetVertexArray();

				INT32 idx = FindVA( pVA);

				if( idx == -1)
					AddVA( pVA, &mtx);
			}
		}
	}
}

void i3OptGeometryBase::ReplaceAllVertexArrayFromGeoAttr(void)
{
	INT32 i;

	// Replace all reference of vertex array from i3GeometryAttr
	for( i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pGeoAttr = getGeoAttr( i);

		INT32 idx = FindVA( pGeoAttr->GetVertexArray());
		if( idx == -1)
			continue;
		
		VAINFO * pInfo = getVA( idx);

		if( pInfo->m_pNewVA != nullptr)
			pGeoAttr->SetVertexArray( pInfo->m_pNewVA);
	}
}