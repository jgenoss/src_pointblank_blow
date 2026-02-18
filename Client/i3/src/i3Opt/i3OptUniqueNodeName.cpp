#include "i3OptPCH.h"
#include <stdio.h>
#include "i3Base.h"
#include "i3OptUniqueNodeName.h"

I3_CLASS_INSTANCE( i3OptUniqueNodeName);

/*
static INT32 _NodeCompareProc( i3Node * pNode1, i3Node * pNode2)
{
	if( pNode1->hasName() == false)
		return 1;

	if( pNode2->hasName() == false)
		return -1;

	return strcmp( pNode1->GetName(), pNode2->GetName());
}
*/

i3OptUniqueNodeName::i3OptUniqueNodeName(void)
{
	m_Class = CLASS_SCENEGRAPH;
}


i3Node * i3OptUniqueNodeName::FindNodeByName( char * pszName)
{
	i3Node Temp;
//	INT32 idx;

	Temp.SetName( pszName);

//	idx = m_NameList.FindItem( &Temp);
	i3::vector_set<i3Node*, NodeNameComp>::iterator it = m_NameList.find(&Temp);

	if( it == m_NameList.end())
		return nullptr;

	return (*it); 
}

bool i3OptUniqueNodeName::OnPreNode( i3Node * pNode, i3Node * pParent)
{
	char szTemp[256], szName[256];
	INT32 count;

	if( pNode->GetName() == nullptr)
		return true;

	strncpy( szTemp, pNode->GetName(), sizeof( szTemp)-1);
	strncpy( szName, pNode->GetName(), sizeof( szName)-1);

	count = 1;

	while( FindNodeByName( szTemp))
	{
		sprintf( szTemp, "%s_%d", szName, count);
		count++;
	}

	if( count > 1)
	{
		// 새로운 이름을 부여받았으므로...
		pNode->SetName( szTemp);
	}

	m_NameList.insert( pNode);

	return true;
}