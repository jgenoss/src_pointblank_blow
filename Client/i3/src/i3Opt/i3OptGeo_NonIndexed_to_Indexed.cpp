#include "i3OptPCH.h"
#include "i3OptGeo_NonIndexed_to_Indexed.h"
#include "i3OptVertexTree.h"

I3_CLASS_INSTANCE( i3OptGeo_NonIndexed_to_Indexed);

i3OptGeo_NonIndexed_to_Indexed::i3OptGeo_NonIndexed_to_Indexed(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


void i3OptGeo_NonIndexed_to_Indexed::Convert( VAINFO * pInfo)
{
	i3OptVertexTree * pTree;
	i3VertexArray * pVA;

	pVA = pInfo->m_pOldVA;

	pInfo->m_pIndexTable = (INT32 *) i3MemAlloc( sizeof(INT32) * pVA->GetCount());

	pTree = i3OptVertexTree::new_object();
	pTree->Create( pVA, pInfo->m_pIndexTable);

	pInfo->m_pNewVA = pTree->getNewVA();
	I3_MUST_ADDREF( pInfo->m_pNewVA);
	I3_MUST_RELEASE( pTree);
}


void	i3OptGeo_NonIndexed_to_Indexed::MakeIndexedGeo( VAINFO * pInfo, i3GeometryAttr * pGeoAttr, INT32 nAICount)
{
	INT32 i, j;
	i3IndexArray * pNewIA;
	i3GeometryResolver resolver;

	resolver.setGeometryAttr( pGeoAttr);

	pNewIA = i3IndexArray::new_object_ref();
	pNewIA->Create( resolver.getPrimitiveCount() * 3, 0, nAICount);
	pNewIA->Lock();

	for( i = 0; i < resolver.getPrimitiveCount(); i++)
	{
		INT32 idx[3], idxNew;

		resolver.getVertexIndices( i, idx);

		for( j = 0; j < 3; j++)
		{
			idxNew = pInfo->m_pIndexTable[ idx[j]];

			pNewIA->SetIndex( (i * 3) + j, idxNew);
		}
	}

	pNewIA->Unlock();


	pGeoAttr->SetVertexArray( pInfo->m_pNewVA, pGeoAttr->GetPrimitiveType(), resolver.getPrimitiveCount(), 0);
	pGeoAttr->SetIndexArray( pNewIA);
}

void	i3OptGeo_NonIndexed_to_Indexed::Trace( i3Node * pRoot)
{
	INT32 i;
	VAINFO * pInfo;

	GatherAllVertexArray( pRoot);

	// Remove unused vertex component from vertex arrays which are gathered on traversing,
	for( i = 0; i < getVACount(); i++)
	{
		pInfo = (VAINFO *) getVA( i);

		if( pInfo->m_pNewVA != nullptr)
			continue;

		Convert( pInfo);
	}

	for( i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pGeoAttr = getGeoAttr( i);

		INT32 idx = FindVA( pGeoAttr->GetVertexArray());
		I3ASSERT( idx != -1);

		pInfo = getVA( idx);

		i3IndexArray* pAI = pGeoAttr->GetIndexArray();

		MakeIndexedGeo( pInfo, pGeoAttr, pAI->GetCount());
	}

	RemoveAllVA();
	RemoveAllGeo();
}