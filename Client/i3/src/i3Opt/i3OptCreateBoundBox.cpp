#include "i3OptPCH.h"
#include "i3OptCreateBoundBox.h"

//I3_CLASS_INSTANCE( i3OptCreateBoundBox, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptCreateBoundBox);

i3OptCreateBoundBox::i3OptCreateBoundBox(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool i3OptCreateBoundBox::OnNode( i3Node * pNode, i3Node * pParent)
{
	// i3Body 밑에 있는 경우는 LOD들의 ShapeNode로 간주하여 처리. 이때 바운드 박스는 설정하지 않도록 한다.
	if( i3::same_of<i3Body*>(pParent))
		return false;

	if( !i3::kind_of<i3Transform*>(pNode))
	{
		if( pParent != nullptr)
		{
			if( !i3::same_of<i3ShapeSetContainer*>(pNode))
			{
				i3BoundBox * pBox = i3BoundBox::new_object_ref();

				pBox->Reset();
				
				pNode->GetWrappingBound( pBox);

				pNode->SetBound( pBox);
			}
			else
			{
				pNode->SetBound(nullptr);
			}
		}
		else
		{
			pNode->SetBound(nullptr);
		}
	}

	return true;
}
