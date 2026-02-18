#include "i3OptPCH.h"
#include "i3OptRemoveShapeSet.h"

I3_CLASS_INSTANCE( i3OptRemoveShapeSet);

i3OptRemoveShapeSet::i3OptRemoveShapeSet(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool	i3OptRemoveShapeSet::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		for( UINT32 i = 0; i < pBody->getLODCount(); i++)
		{
			i3LOD * pLOD = pBody->getLOD( i);
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( INT32 j = 0; j < pSkel->getBoneCount(); j++)
			{
				if( pSkel->isShapeSet( j))
				{
					pSkel->setShapeSet( j, nullptr);
				}
			}
		}
	}

	return true;
}
