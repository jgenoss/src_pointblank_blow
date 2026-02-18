#include "i3OptPCH.h"
#include "i3OptNodeFlag.h"

I3_CLASS_INSTANCE( i3OptNodeFlag);

i3OptNodeFlag::i3OptNodeFlag(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool	i3OptNodeFlag::OnNode( i3Node * pNode, i3Node * pParent)
{
	UINT32 flag = pNode->GetFlag() & ~m_RemoveFlag;

	pNode->SetFlag( flag | m_AddFlag);

	return true;
}
