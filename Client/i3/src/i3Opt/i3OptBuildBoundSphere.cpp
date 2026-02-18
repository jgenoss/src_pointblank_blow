#include "i3OptPCH.h"
#include "i3OptBuildBoundSphere.h"

I3_CLASS_INSTANCE( i3OptBuildBoundSphere);

i3OptBuildBoundSphere::i3OptBuildBoundSphere(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


void i3OptBuildBoundSphere::_BoundSphere( i3VertexArray * pVA)
{
	INT32 i;
	VEC3D pos;
	i3BoundBox box;

	pVA->Lock();

	for( i = 0; i < (INT32) pVA->GetCount(); i++)
	{
		pVA->GetPosition( i, &pos);

		box.ExtendByVec( &pos);		
	}

	box.GetCenter( &pos);
	pVA->setBoundSphereCenter( &pos);

	i3Vector::Sub( &pos, &pos, box.GetMin());
	REAL32 r = i3Vector::Length( &pos);

	pVA->setBoundSphereRadius( r);

	pVA->Unlock();
}

bool i3OptBuildBoundSphere::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3Geometry* >(pNode))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;
		i3GeometryAttr * pGeoAttr;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = pGeo->GetGeometryAttr( i);

			if( !i3::same_of<i3GeometryAttr*>(pAttr))
				continue;

			pGeoAttr = (i3GeometryAttr *) pAttr;

			if( pGeoAttr->GetVertexArray() != nullptr)
				_BoundSphere( pGeoAttr->GetVertexArray());
		}
	}

	return true;
}
