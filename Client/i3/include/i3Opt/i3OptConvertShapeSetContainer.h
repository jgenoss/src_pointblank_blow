#if !defined( __I3_OPT_CONVERT_SHAPESETCONTAINER_H)
#define __I3_OPT_CONVERT_SHAPESETCONTAINER_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptConvertShapeSetContainer : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptConvertShapeSetContainer, i3SceneOptimizer);
protected:
	
	i3ShapeSetContainer *			_FindReplaceTransform( i3Node * pNode);

public:
	i3OptConvertShapeSetContainer(void);

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;
};

#endif
