#if !defined( I3OPT_REMOVE_SHAPESET_H)
#define I3OPT_REMOVE_SHAPESET_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveShapeSet : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveShapeSet);

protected:

public:
	i3OptRemoveShapeSet(void);
	virtual ~i3OptRemoveShapeSet(void);

public:
	virtual BOOL	OnNode( i3Node * pNode);
};

#endif