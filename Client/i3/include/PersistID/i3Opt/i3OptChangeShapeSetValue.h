#if !defined( __I3_OPT_CHANGE_SHAPESETVALUE_H)
#define __I3_OPT_CHANGE_SHAPESETVALUE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptChangeShapeSetValue : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptChangeShapeSetValue);
protected:
	

protected:
	void					_FindChangeShapeSet( i3Node * pNode);

public:
	i3OptChangeShapeSetValue(void);
	virtual ~i3OptChangeShapeSetValue(void);

	virtual BOOL	OnNode( i3Node * pNode);
};

#endif
