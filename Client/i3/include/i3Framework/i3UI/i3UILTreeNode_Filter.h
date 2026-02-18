#if !defined( __I3UIL_TREE_NODE_FILTER_H__)
#define __I3UIL_TREE_NODE_FILTER_H__

//#include "i3UILTreeNode_Template.h"
class i3UILTreeNode_Template;

class I3_EXPORT_FRAMEWORK i3UILTreeNode_Filter : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3UILTreeNode_Filter, i3TreeNode);
	
public:

	void	RemoveDescendants( bool bIncludeRoot = false);
};

#endif	//__I3UIL_TREE_NODE_FILTER_H__