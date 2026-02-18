#include "i3OptPCH.h"
#include "i3OptRemoveRedundantAttr.h"

I3_CLASS_INSTANCE( i3OptRemoveRedundantAttr);

i3OptRemoveRedundantAttr::i3OptRemoveRedundantAttr(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool i3OptRemoveRedundantAttr::OnPreNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3AttrSet* >(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		INT32 i;
		
		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pAttrSet->GetAttr( i);

			i3AttrStack * pStack = m_AttrStack.GetAttrStackByID( pAttr->GetID());

			if( pStack == nullptr)
			{
				continue;
			}

			i3RenderAttr * pTop = pStack->GetTop();

			if( pTop == nullptr)
			{
				m_AttrStack.PushAttr( pAttr);
				continue;
			}

			if( pAttr->IsSame( pTop))
			{
				// 같은 값이 이미 있기 때문에 있을 필요가 없다. 제거
				pAttrSet->RemoveAttr( pAttr);
				i--;
			}
			else
			{
				// 다른 값을 가지고 있기 때문에 AttrStack에 Push
				m_AttrStack.PushAttr( pAttr);
			}
		}
	}

	return true;
}

bool i3OptRemoveRedundantAttr::OnPostNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3AttrSet* >(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		INT32 i;
		
		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pAttrSet->GetAttr( i);

			m_AttrStack.PopAttr( pAttr->GetID());
		}
	}

	return true;
}
