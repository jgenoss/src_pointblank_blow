#include "i3OptPCH.h"
#include "i3OptConvertShapeSetContainer.h"

//#include "i3PhysixDefine.h"

//I3_CLASS_INSTANCE( i3OptConvertShapeSetContainer, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptConvertShapeSetContainer);

i3OptConvertShapeSetContainer::i3OptConvertShapeSetContainer(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool i3OptConvertShapeSetContainer::OnNode( i3Node * pNode, i3Node * pParent)
{
	return true;
}


void i3OptConvertShapeSetContainer::Trace( i3Node * pRoot)
{
	i3SceneOptimizer::Trace( pRoot);

	i3Transform2 * pTrans = (i3Transform2 *)i3Scene::FindNodeByExactType( pRoot, i3Transform2::static_meta());
	if( pTrans == nullptr)
		return;

	{
		i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();
		pContainer->setShapeSet( pTrans->getShapeSet() );
		i3Node * pParent = pTrans->GetParent();
		if( pParent != nullptr )
		{
			I3_MUST_ADDREF( pTrans);
			pParent->RemoveChild( pTrans );
			I3_MUST_RELEASE(pTrans);
			pParent->AddChild( pContainer );
		}

		if( pTrans == m_pSg->getInstanceSg())
		{
			m_pSg->setInstanceSg( pContainer);
		}
	}

}
