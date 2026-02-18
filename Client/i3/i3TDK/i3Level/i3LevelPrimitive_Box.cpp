#include "stdafx.h"
#include "i3LevelPrimitive_Box.h"
#include "i3LevelResPrimitive_Box.h"

I3_CLASS_INSTANCE( i3LevelPrimitive_Box);

void i3LevelPrimitive_Box::CreateSymbol(void)
{
	i3Node * pNode = BuildSg();
	I3ASSERT( pNode != nullptr);

	SetSymbol( pNode);
}

void i3LevelPrimitive_Box::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	i3LevelPrimitive::OnSetInstanceInfo( pInfo);

	pInfo->nTriggerType			= 0;
}

i3Node * i3LevelPrimitive_Box::BuildSg(void)
{
	i3VertexArray * pVA;
	i3Transform2 *	pTrans2;

	{
		VEC3D vmin, vmax;

		i3Vector::Set( &vmax, 0.5f, 0.5f, 0.5f);
		i3Vector::Scale( &vmin, &vmax, -1.0f);

		pVA = i3VertexArray::CreateBox( &vmin, &vmax, FALSE, TRUE, nullptr);
	}

	i3Geometry * pGeo;
	i3GeometryAttr * pGeoAttr;

	pGeoAttr = i3GeometryAttr::new_object_ref();
	pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pVA->GetCount() / 3);

	pGeo = i3Geometry::new_object_ref();
	pGeo->AppendGeometryAttr( pGeoAttr);

	// Physics¿ë Shape Primitive »ý¼º
	{
		VEC3D center, size;
		i3PhysixShapeSet * pShapeSet;

		i3Vector::Set( &center, 0.0f, 0.0f, 0.0f);
		i3Vector::Set( &size, 0.5f, 0.5f, 0.5f);

		pShapeSet = i3PhysixShapeSet::new_object_ref();
		pShapeSet->CreateBoxShape( &center, &size, 0, true);

		pTrans2 = i3Transform2::new_object_ref();
		pTrans2->setShapeSet( pShapeSet);
		pTrans2->AddChild( pGeo);

		i3Scene::ModifyDynamicState( pTrans2, I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	return pTrans2;
}
