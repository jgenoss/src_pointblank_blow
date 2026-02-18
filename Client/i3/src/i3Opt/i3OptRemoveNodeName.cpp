#include "i3OptPCH.h"
#include "i3OptRemoveNodeName.h"

I3_CLASS_INSTANCE( i3OptRemoveNodeName);

i3OptRemoveNodeName::i3OptRemoveNodeName(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool	i3OptRemoveNodeName::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Transform* >(pNode))
		return true;

	if( i3::kind_of<i3BoneRef* >(pNode))
		return true;

	if( i3::kind_of<i3PhysixShapeSet* >(pNode))
		return true;

	//AttrSet인경우는 TextureEnableAttr을 제거한다.( 현재 TextureEnableAttr은 모두 true여야하는상황--포인트 블랭크 관련입니다.-)
	if( i3::kind_of<i3AttrSet* >(pNode))
	{
		INT32 nAttrCnt = ((i3AttrSet*)pNode)->GetAttrCount();

		for( INT32 i = 0; i < nAttrCnt; ++i)
		{
			i3RenderAttr * pAttr = ((i3AttrSet*)pNode)->GetAttr( i);

			if( i3::kind_of<i3TextureEnableAttr* >(pAttr))
			{
				((i3AttrSet*)pNode)->RemoveAttr( pAttr);
				i--;
				nAttrCnt--;
				continue;
			}

			if( i3::kind_of<i3ShadeModeAttr* >(pAttr))
			{
				((i3AttrSet*)pNode)->RemoveAttr( pAttr);
				i--;
				nAttrCnt--;
				continue;
			}

			if( i3::kind_of<i3TextureFilterAttr* >(pAttr))
			{
				((i3AttrSet*)pNode)->RemoveAttr( pAttr);
				i--;
				nAttrCnt--;
				continue;
			}
		}
	}

	//주 작업.
	pNode->SetName( "" );

	return true;
}
