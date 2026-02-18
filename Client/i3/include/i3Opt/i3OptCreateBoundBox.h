#if !defined( __I3_OPT_CREATE_BOUND_BOX_H)
#define __I3_OPT_CREATE_BOUND_BOX_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCreateBoundBox : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptCreateBoundBox, i3SceneOptimizer);

public:
	i3OptCreateBoundBox(void);
	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
