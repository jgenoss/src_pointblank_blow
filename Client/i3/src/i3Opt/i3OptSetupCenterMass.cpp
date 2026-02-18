#include "i3OptPCH.h"
#include "i3OptSetupCenterMass.h"

I3_CLASS_INSTANCE( i3OptSetupCenterMass);

i3OptSetupCenterMass::i3OptSetupCenterMass()
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool i3OptSetupCenterMass::OnNode( i3Node * pNode, i3Node * pParent)
{
	i3::vector<i3PhysixShapeSet*> shapeList;

	if( i3::same_of<i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		i3LOD * pLOD = pBody->getFrameLOD();
		if( pLOD != nullptr)
		{
			I3ASSERT( pLOD->getSkeleton() != nullptr);
			for( INT32 i = 0; i < pLOD->getSkeleton()->getBoneCount(); i++)
			{
				if( pLOD->getSkeleton()->isShapeSet(i))
				{
					i3PhysixShapeSet * pShapeSet = pLOD->getSkeleton()->getShapeSet( i);
					shapeList.push_back( pShapeSet);
				}
			}
		}
	}
	else if( i3::same_of<i3Transform2*>(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2*) pNode;
		if( pTrans->getShapeSet() != nullptr)
			shapeList.push_back( pTrans->getShapeSet());
	}

	for( UINT32 i = 0; i < shapeList.size(); i++)
	{
		i3PhysixShapeSet * pShapeSet = shapeList.at(i);

		if( pShapeSet->getJointInfo() != nullptr)
			continue;

		NxActor * pActor = pShapeSet->getNxActor();
		if( pActor == nullptr)
			continue;

		pActor->updateMassFromShapes( 0, pShapeSet->getMass());
	}

	return true;
}
