#include "i3OptPCH.h"
#include "i3OptValidateGeometryBound.h"

I3_CLASS_INSTANCE( i3OptValidateGeometryBound);

i3OptValidateGeometryBound::i3OptValidateGeometryBound(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

void i3OptValidateGeometryBound::_CalcBoundSphere( i3GeometryAttr * pAttr)
{
	i3VertexArray * pVA = pAttr->GetVertexArray();

	pAttr->RefreshBound();

	VEC3D center, vmin, vmax;

	vmin = * pAttr->getBoundMin();
	vmax = * pAttr->getBoundMax();

	i3Vector::Add( &center, &vmin, &vmax);
	i3Vector::Scale( &center, &center, 0.5f);

	pVA->setBoundSphereCenter( &center);

	VEC3D sub;

	i3Vector::Sub( &sub, &vmax, &vmin);
	pVA->setBoundSphereRadius( i3Vector::Length( &sub));
}

bool i3OptValidateGeometryBound::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3Geometry*>(pNode))
	{ 
		i3Geometry * pGeo = (i3Geometry *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( i3::kind_of<i3GeometryAttr* >(pAttr))
			{
				i3GeometryAttr * pGeo = (i3GeometryAttr *) pAttr;

				_CalcBoundSphere( pGeo);
			}
		}
	}

	return true;
}
