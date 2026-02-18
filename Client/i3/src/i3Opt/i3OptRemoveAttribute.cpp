#include "i3OptPCH.h"
#include "i3OptRemoveAttribute.h"

I3_CLASS_INSTANCE( i3OptRemoveAttribute);

i3OptRemoveAttribute::i3OptRemoveAttribute(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
	m_pMeta = i3MaterialAttr::static_meta();
}

bool i3OptRemoveAttribute::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;
		INT32 i;
		i3RenderAttr * pAttr;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr( i);

			if( pAttr->kind_of( m_pMeta))
			{
				pAttrSet->RemoveAttr( pAttr);
				i--;
			}
		}
	}

	return true;
}
