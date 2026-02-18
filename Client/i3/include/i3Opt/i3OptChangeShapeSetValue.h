#if !defined( __I3_OPT_CHANGE_SHAPESETVALUE_H)
#define __I3_OPT_CHANGE_SHAPESETVALUE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptChangeShapeSetValue : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptChangeShapeSetValue, i3SceneOptimizer);
protected:
	void					_FindChangeShapeSet( i3Node * pNode);

public:
	i3OptChangeShapeSetValue(void);

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
