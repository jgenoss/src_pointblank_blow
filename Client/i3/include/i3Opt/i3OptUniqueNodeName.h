#if !defined( __I3_OPT_UNIQUE_NODE_NAME_H)
#define __I3_OPT_UNIQUE_NODE_NAME_H

#include "i3SceneOptimizer.h"

struct NodeNameComp 
{
	bool operator()( const i3Node* pNode1, const i3Node* pNode2) const
	{
		if( pNode1->hasName() == false)
			return false;
		if( pNode2->hasName() == false)
			return true;
		return strcmp( pNode1->GetName(), pNode2->GetName()) < 0;
	}
};



class I3_EXPORT_OPT i3OptUniqueNodeName : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptUniqueNodeName, i3SceneOptimizer);
protected:
	i3::vector_set<i3Node*, NodeNameComp>	m_NameList;

	i3Node *		FindNodeByName( char * pszName);

public:
	i3OptUniqueNodeName(void);

	virtual bool OnPreNode( i3Node * pNode, i3Node * pParent) override;
};

#endif