#include "i3FrameworkPCH.h"
#include "i3GameNodeTraversal.h"


I3_CLASS_INSTANCE( i3GameNodeTraversal);


bool i3GameNodeTraversal::_Rec_Traverse( i3GameNode * pNode)
{
	if( m_pUserProc != nullptr)
	{
		if( m_pUserProc( pNode, this, m_pUserData) == false)
			return false;
	}

	m_NodeCount++;

	if( pNode->getFirstChild() != nullptr)
	{
		m_Depth++;
		if( _Rec_Traverse( pNode->getFirstChild()) == false)
			return false;

		m_Depth--;
	}

	if( pNode->getNext() != nullptr)
	{
		if( _Rec_Traverse( pNode->getNext()) == false)
			return false;
	}

	return true;
}

void i3GameNodeTraversal::Traverse( i3GameNode * pRoot)
{
	i3GameNode * pChild = nullptr;

	m_pRoot = pRoot;
	m_Depth = 0;
	m_NodeCount = 0;

	if( m_pUserProc != nullptr)
	{
		if( m_pUserProc( pRoot, this, m_pUserData) == false)
			return;
	}

	m_NodeCount++;

	pChild = pRoot->getFirstChild();
	while( pChild != nullptr)
	{
		_Rec_Traverse( pChild);

		pChild = pChild->getNext();
	}
}

