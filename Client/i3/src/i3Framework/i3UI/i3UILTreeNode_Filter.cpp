#include "i3FrameworkPCH.h"
#include "i3UILTreeNode_Filter.h"
#include "i3UILTreeNode_Template.h"


// COMMENT : i3UILTreeFilter 와 i3UILTreeNode_Filter 모두 사용하게끔 만든 상황이 재앙임...
//           호환성 유지 코드의 아주 나쁜 사례로, 리팩토링이 어렵다.  (i3UIEditor까지 고려해서 집중해서 고치지 않으면 ..
//           버그 유발과 기능 누락 되기 쉽다..  ) .. 2016.05.16.수빈.
//           툴에서 편집은 i3UILTreeFilter를 사용해서 하고, 저장과 읽기는 i3UILTreeNode_Filter로 처리됨..
//           이쪽 루틴 개선은 당분간 잠정 포기..
//

I3_CLASS_INSTANCE( i3UILTreeNode_Filter);

//void i3UILTreeNode_Filter::AddNode( i3UILTreeNode_Filter * pNode)
//{
//	I3ASSERT( pNode != nullptr);
//	AddChild( pNode);
//}
//
//void i3UILTreeNode_Filter::AddNode( i3UILTreeNode_Template * pNode)
//{
//	I3ASSERT( pNode != nullptr);
//	AddChild( pNode);
//}
//
//void i3UILTreeNode_Filter::AddNode( i3TreeNode* pNode)
//{
//	I3ASSERT( pNode != nullptr);
//	if( pNode->IsExactTypeOf( i3UILTreeNode_Template::GetClassMeta()))
//	{
//		AddNode( (i3UILTreeNode_Template*)pNode);
//	}
//	else if( pNode->IsExactTypeOf( i3UILTreeNode_Filter::GetClassMeta()))
//	{
//		AddNode( (i3UILTreeNode_Filter*)pNode);
//	}
//	else
//	{
//		I3ASSERT( 0);
//	}
//}
//
//void i3UILTreeNode_Filter::RemoveNode( i3UILTreeNode_Filter * pNode)
//{
//	RemoveChild( pNode);
//}
//
//void i3UILTreeNode_Filter::RemoveNode( i3UILTreeNode_Template * pNode)
//{
//	RemoveChild( pNode);
//}

void i3UILTreeNode_Filter::RemoveDescendants( bool bIncludeRoot)
{
	//자기 자신은 지우면 안됨
	for( INT32 i=0; i< getChildCount(); i++)
	{
		i3TreeNode* pChild = getChild( i);

		if( i3::same_of<i3UILTreeNode_Filter* >(pChild))
		{
			i3UILTreeNode_Filter* pFilterNode = (i3UILTreeNode_Filter*)pChild;
			pFilterNode->RemoveDescendants( true);
		}
		else
		{
			pChild->RemoveFromParent();
		}
	}

	if( bIncludeRoot)
	{
		RemoveFromParent();
	}
}