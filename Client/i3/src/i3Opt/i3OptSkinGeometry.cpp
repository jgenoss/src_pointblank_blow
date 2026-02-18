#include "i3OptPCH.h"
#include "i3OptSkinGeometry.h"
#include "i3OptBuildIndexedGeometry.h"

I3_CLASS_INSTANCE( i3OptSkinGeometry);

//////////////////////////////////////////////////////////////////////////////

i3OptSkinGeometry::i3OptSkinGeometry(void)
{
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
	m_Class = CLASS_GEOMETRY;
}

i3SkinGeometryAttr * i3OptSkinGeometry::_BuildSkin( i3GeometryAttr * pGeoAttr)
{
	i3SkinGeometryAttr * pSkin = nullptr;

	pSkin = i3SkinGeometryAttr::new_object_ref();

	pSkin->Setup( pGeoAttr);

	return pSkin;
}

bool i3OptSkinGeometry::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Geometry* >(pNode))
	{
		i3SkinGeometryAttr * pSkin;
		i3Geometry * pGeo = (i3Geometry *) pNode;
		INT32 i;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pGeo->GetGeometryAttr( i);

			if( i3::same_of<i3SkinGeometryAttr* >(pAttr))
				continue;

			if( !i3::kind_of<i3GeometryAttr*>(pAttr))
				continue;

			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;
			i3VertexArray * pVA = pGeoAttr->GetVertexArray();

			if( pVA->GetFormat()->GetBlendIndexCount() <= 0)
				continue;

			pSkin = _BuildSkin( pGeoAttr);

			pGeo->RemoveGeometryAttr( pGeoAttr);
			pGeo->AppendGeometryAttr( pSkin);

			i--;
		}
	}
	
	return true;
}

void i3OptSkinGeometry::Trace( i3Node * pRoot)
{
	SetTraceRoot( pRoot);

	i3SceneOptimizer::Trace( pRoot);
}

const char * i3OptSkinGeometry::getDescName(void)
{
	return "Convert to Skin Geometries";
}

bool i3OptSkinGeometry::SaveToXML( i3XMLElement * pXML)
{	
	return true;
}

bool i3OptSkinGeometry::LoadFromXML( i3XMLElement * pXML)
{
	return true;
}

