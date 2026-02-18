#include "i3OptPCH.h"
#include "i3OptCollapseNode.h"

//I3_CLASS_INSTANCE( i3OptCollapseNode, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptCollapseNode);

static void _RemoveNode( i3Node * pDest, i3Node * pParent);

i3OptCollapseNode::i3OptCollapseNode(void)
{
	m_Class = CLASS_SCENEGRAPH;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool i3OptCollapseNode::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( pNode == nullptr)	return true;
	
	//	불필요한 노드들을 제거한다.
	CollapseNode( pNode);

	return true;
}

void i3OptCollapseNode::CollapseNode( i3Node * pNode)
{
	INT32 i = 0;
	i3Node * pCurNode = nullptr;
	i3ClassMeta * pMeta = pNode->meta();
	I3ASSERT( pMeta != nullptr);
    
	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pCurNode = pNode->GetChild( i);

		//	재귀호출하여 자식 노드도 검사
		CollapseNode( pCurNode);
   
		//	바로 아래 자식 노드중 쓸모없는 i3Geometry 노드를 삭제한다.
		if( _RemoveUselessGeometry( pNode, i))
		{
			i--;
			continue;
		}

		//	바로 아래 자식 노드중 쓸모없는 i3AttrSet 노드를 삭제한다.
		if( _RemoveUselessAttrSet( pNode, i))
		{
			i--;
			continue;
		}

		//	바로 아래 자식 노드중 쓸모없는 i3Node 노드를 삭제한다.
		if( _RemoveUselessI3Node( pNode, i))
		{
			i--;
			continue;
		}
	}
}

bool i3OptCollapseNode::_RemoveUselessGeometry( i3Node * pNode, INT32 nChildIdx)
{
	if( pNode == nullptr || nChildIdx < 0) return false;

	i3Node * pChildNode = pNode->GetChild( nChildIdx);
	if( pChildNode == nullptr) return false;

	//	자식노드가 i3Geometry인지 검사
	if( i3::same_of<i3Geometry*>(pChildNode))
	{
		i3Geometry * pCurGeometry = (i3Geometry *) pChildNode;
		if( _IsUselessGeometry( pCurGeometry))
		{  
			I3TRACE( "Collapse Node : (%s) %s\n", pChildNode->meta()->class_name(), pChildNode->GetName());

			_RemoveNode( pChildNode, pNode);

			return true;
		}
	}

	return false;
}

bool i3OptCollapseNode::_RemoveUselessAttrSet( i3Node * pNode, INT32 nChildIdx)
{
	if( pNode == nullptr || nChildIdx < 0) return false;

	i3Node * pChildNode = pNode->GetChild( nChildIdx);
	if( pChildNode == nullptr) return false;

	//	자식노드가 i3AttrSet인지 검사
	if( i3::same_of<i3AttrSet*>(pChildNode))
	{
		i3AttrSet * pCurAttrSet = (i3AttrSet *) pChildNode;
		if( _IsUselessAttrSet( pCurAttrSet))
		{ 
			I3TRACE( "Collapse Node : (%s) %s\n", pChildNode->meta()->class_name(), pChildNode->GetName());

			_RemoveNode( pChildNode, pNode);

			return true;
		}
	}

	return false;
}

bool i3OptCollapseNode::_RemoveUselessI3Node( i3Node * pNode, INT32 nChildIdx)
{
	if( pNode == nullptr || nChildIdx < 0) return false;

	i3Node * pChildNode = pNode->GetChild( nChildIdx);
	if( pChildNode == nullptr) return false;

	//	자식노드가 i3Node인지 검사
	if( i3::same_of<i3Node*>(pChildNode))
	{
		i3Node* pCurI3Node = (i3Node *) pChildNode;
		if( _IsUselessI3Node( pCurI3Node))
		{
			I3TRACE( "Collapse Node : (%s) %s\n", pChildNode->meta()->class_name(), pChildNode->GetName());

			_RemoveNode( pChildNode, pNode);

			return true;
		}
	}

	return false;
}

bool i3OptCollapseNode::_IsUselessGeometry( i3Geometry * pGeometry)
{
	//	GeometryAttr 이 없는 경우 쓸모 없다
	if( pGeometry->GetGeometryAttrCount() > 0)
		return false;

	return true;
}

bool i3OptCollapseNode::_IsUselessAttrSet( i3AttrSet * pAttrSet)
{
	//	Attr 이 없는 경우 쓸모 없다
	if( pAttrSet->GetAttrCount() > 0)
		return false;

	return true;
}

bool i3OptCollapseNode::_IsUselessI3Node( i3Node * pI3Node)
{
	//	자식노드가 없는 경우 쓸모 없다
	if( pI3Node->GetChildCount() > 0)
		return false;

	return true;
}

static void _RemoveNode( i3Node * pDest, i3Node * pParent)
{
	i3Node * pChild;

	I3_MUST_ADDREF( pDest);

	pParent->RemoveChild( pDest);

	// pDest의 자식 노드를 pParent의 자식 노드로 전부 옮긴다.
	for( ; pDest->GetChildCount() > 0;)
	{
		pChild = pDest->GetChild( 0);
		pChild->ChangeParent( pParent);
	}

	I3_MUST_RELEASE(pDest);
}

