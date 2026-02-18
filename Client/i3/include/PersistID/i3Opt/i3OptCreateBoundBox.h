#if !defined( __I3_OPT_CREATE_BOUND_BOX_H)
#define __I3_OPT_CREATE_BOUND_BOX_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCreateBoundBox : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptCreateBoundBox);
protected:

public:
	i3OptCreateBoundBox(void);
	virtual ~i3OptCreateBoundBox(void);

	virtual BOOL OnNode( i3Node * pNode);
};

#endif
