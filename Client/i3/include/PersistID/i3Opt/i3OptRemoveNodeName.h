#if !defined( I3OPT_REMOVE_NODENAME_H__)
#define I3OPT_REMOVE_NODENAME_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveNodeName : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveNodeName);

protected:

public:
	i3OptRemoveNodeName(void);
	virtual ~i3OptRemoveNodeName(void);

public:
	virtual BOOL	OnNode( i3Node * pNode);
};

#endif