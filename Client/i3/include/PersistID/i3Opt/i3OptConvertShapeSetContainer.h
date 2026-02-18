#if !defined( __I3_OPT_CONVERT_SHAPESETCONTAINER_H)
#define __I3_OPT_CONVERT_SHAPESETCONTAINER_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptConvertShapeSetContainer : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptConvertShapeSetContainer);
protected:
	

protected:
	
	i3ShapeSetContainer *			_FindReplaceTransform( i3Node * pNode);

public:
	i3OptConvertShapeSetContainer(void);
	virtual ~i3OptConvertShapeSetContainer(void);

	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
};

#endif
