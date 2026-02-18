#if !defined( I3OPT_REMOVE_NODENAME_H__)
#define I3OPT_REMOVE_NODENAME_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveNodeName : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveNodeName, i3SceneOptimizer);
	
public:
	i3OptRemoveNodeName(void);

public:
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif