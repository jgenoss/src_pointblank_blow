#include "i3OptPCH.h"
#include "i3OptRemoveAnim.h"

I3_CLASS_INSTANCE( i3OptRemoveAnim);

i3OptRemoveAnim::i3OptRemoveAnim(void)
{
	m_Class = CLASS_ANIMATION;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool	i3OptRemoveAnim::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Transform*>(pNode))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		if( pTrans->IsAnimated())
		{
			pTrans->RemoveAllAnimation();

			if( pTrans->hasName())
			{
				I3TRACE( "TRANS : %s\n", pTrans->GetName());
			}

			m_Count++;
		}
	}
	else if( i3::kind_of<i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body *) pNode;

		if( pBody->GetCurrentAnim() != nullptr)
		{
			pBody->StopAnim();

			I3TRACE( "BODY\n");

			m_Count++;
		}
	}

	return true;
}
