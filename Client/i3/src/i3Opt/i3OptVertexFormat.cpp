#include "i3OptPCH.h"
#include "i3OptVertexFormat.h"

I3_CLASS_INSTANCE( i3OptVertexFormat);

i3OptVertexFormat::i3OptVertexFormat(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


i3VertexArray *	i3OptVertexFormat::_ClampTexCoordSet( i3VertexArray * pSrc, INT32 maxSet)
{
	i3VertexFormat fmt;

	fmt = *pSrc->GetFormat();

	fmt.SetTextureCoordSetCount( (INT8) min( fmt.GetTextureCoordSetCount(), maxSet));

	i3VertexArray * pNewVA;

	pNewVA = i3VertexArray::new_object();

	pNewVA->Create( &fmt, pSrc->GetCount(), 0);

	INT32 i;

	pSrc->Lock();
	pNewVA->Lock();

	for( i = 0; i < (INT32) pSrc->GetCount(); i++)
	{
		pNewVA->CopyVertexFrom( i, pSrc, i);
	}

	pSrc->Unlock();
	pNewVA->Unlock();

	return pNewVA;
}

bool	i3OptVertexFormat::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( !i3::kind_of<i3Geometry*>(pNode))
		return true;

	i3Geometry * pGeo = (i3Geometry *) pNode;
	INT32 i;
	i3RenderAttr * pAttr;
	i3GeometryAttr * pGeoAttr;
	INT32 maxTexSet = 1;

	{
		i3LuxMapEnableAttr * pLuxEnable;

		pLuxEnable = (i3LuxMapEnableAttr *) i3Scene::FindLastAttr( pNode, i3LuxMapEnableAttr::static_meta());

		if( (pLuxEnable != nullptr) && pLuxEnable->Get())
			maxTexSet++;
	}

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pAttr = pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		pGeoAttr = (i3GeometryAttr *) pAttr;

		m_GeoList.push_back( pGeoAttr);

		i3VertexArray * pVA = pGeoAttr->GetVertexArray();

		INT32 idx = FindVA( pVA);
		VAINFO * pInfo = nullptr;

		if( idx == -1)
		{
			// This vertex array have not been added to VAList
			idx = AddVA( pVA, nullptr);
		}

		pInfo = getVA( idx);

		pInfo->m_Param1 = MAX( pInfo->m_Param1, maxTexSet);
	}

	return true;
}


void	i3OptVertexFormat::Trace( i3Node * pRoot)
{
	INT32 i;
	VAINFO * pInfo;

	i3SceneOptimizer::Trace( pRoot);

	// Remove unused vertex component from vertex arrays which are gathered on traversing,
	for( i = 0; i < getVACount(); i++)
	{
		pInfo = (VAINFO *) getVA( i);

		if( pInfo->m_Param1 < pInfo->m_pOldVA->GetFormat()->GetTextureCoordSetCount())
		{
			// It contains unused texture coordinates.
			i3VertexArray * pNewVA = _ClampTexCoordSet( pInfo->m_pOldVA, pInfo->m_Param1);

			pInfo->m_pNewVA = pNewVA;
		}
	}

	// Replace vertex array of i3GemoetryAttr, if it was reconstructed.
	for( i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pAttr = getGeoAttr( i);

		INT32 idx = FindVA( pAttr->GetVertexArray());

		if( idx != -1)
		{
			pInfo = getVA( idx);

			if( pInfo->m_pNewVA != nullptr)
			{
				pAttr->SetVertexArray( pInfo->m_pNewVA);
			}
		}
	}

	RemoveAllVA();
	RemoveAllGeo();
}