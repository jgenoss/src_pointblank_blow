#include "stdafx.h"
#include "i3LevelPrimitive_Sphere.h"
#include "i3LevelResPrimitive_Sphere.h"

I3_CLASS_INSTANCE( i3LevelPrimitive_Sphere);

void i3LevelPrimitive_Sphere::CreateSymbol(void)
{
	i3Node * pNode = BuildSg();
	I3ASSERT( pNode != nullptr);

	SetSymbol( pNode);
}

void i3LevelPrimitive_Sphere::OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo)
{
	i3LevelPrimitive::OnSetInstanceInfo( pInfo);

	pInfo->nTriggerType			= 1;
}


i3Node * i3LevelPrimitive_Sphere::BuildSg(void)
{
	COLORREAL colDiffuse, colAmbient;
	i3Transform2 *	pTrans2;
	i3Node * pNode;

	i3Color::Set( &colDiffuse, 0.33f, 0.66f, 0.99f, 1.0f);
	i3Color::Set( &colAmbient, 1.0f, 1.0f, 1.0f, 1.0f);

	pNode = i3SceneUtil::BuildSphereMesh( 1.0f, TRUE, FALSE, 20, 10, nullptr, &colAmbient, &colDiffuse);

	// Physix 관련 Shape 생성
	{
		i3PhysixShapeSet * pShapeSet;
		VEC3D zero;

		pShapeSet = i3PhysixShapeSet::new_object_ref();

		i3Vector::Set( &zero, 0.0f, 0.0f, 0.0f);
		pShapeSet->CreateSphereShape( &zero, 1.0f, 0);

		pTrans2 = i3Transform2::new_object_ref();
		pTrans2->SetName( "Root");
		pTrans2->setShapeSet( pShapeSet);

		pTrans2->AddChild( pNode);
		I3_MUST_RELEASE(pNode);			// 없으면 릭..반드시 필요..(2012.09.11.수빈)
		i3Scene::ModifyDynamicState( pTrans2, I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	return pTrans2;
}
