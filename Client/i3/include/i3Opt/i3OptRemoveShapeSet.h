#if !defined( I3OPT_REMOVE_SHAPESET_H)
#define I3OPT_REMOVE_SHAPESET_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveShapeSet : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveShapeSet, i3SceneOptimizer);
	
public:
	i3OptRemoveShapeSet(void);
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif