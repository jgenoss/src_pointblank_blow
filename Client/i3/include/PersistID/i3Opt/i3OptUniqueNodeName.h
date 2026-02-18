#if !defined( __I3_OPT_UNIQUE_NODE_NAME_H)
#define __I3_OPT_UNIQUE_NODE_NAME_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptUniqueNodeName : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptUniqueNodeName);
protected:
	i3BinList		m_NameList;

	i3Node *		FindNodeByName( char * pszName);

public:
	i3OptUniqueNodeName(void);
	virtual ~i3OptUniqueNodeName(void);

	virtual BOOL OnPreNode( i3Node * pNode);
};

#endif