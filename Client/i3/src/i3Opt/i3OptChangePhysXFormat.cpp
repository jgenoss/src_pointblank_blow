#include "i3OptPCH.h"
#include "i3OptChangePhysXFormat.h"

I3_CLASS_INSTANCE( i3OptChangePhysXFormat);

i3OptChangePhysXFormat::i3OptChangePhysXFormat(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool i3OptChangePhysXFormat::OnNode( i3Node * pNode, i3Node * pParent)
{
	i3PhysixShapeSet * pShapeSet = nullptr;

	if( i3::same_of<i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body*)pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( !pSkel->isShapeSet( i))		continue;

			pShapeSet = pSkel->getShapeSet( i);
			if( pShapeSet == nullptr)			continue;

			pShapeSet->setFormat( m_Format);
		}
	}
	else if( i3::same_of<i3Transform2*>(pNode))
	{
		i3Transform2 * pTrans = (i3Transform2 *) pNode;
		if( pTrans != nullptr )
		{	
			pShapeSet = pTrans->getShapeSet();

			if( pShapeSet != nullptr )
			{
				pShapeSet->setFormat( m_Format);
			}
		}
	}
	else if( i3::same_of<i3ShapeSetContainer*>(pNode))
	{
		i3ShapeSetContainer * pShapeSetContainer = (i3ShapeSetContainer*) pNode;
		pShapeSet = pShapeSetContainer->getShapeSet();

		if( pShapeSet != nullptr )
		{
			pShapeSet->setFormat( m_Format);
		}
	}

	return i3SceneOptimizer::OnNode( pNode, pParent);
}