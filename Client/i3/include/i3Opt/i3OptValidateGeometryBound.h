#if !defined( __I3_OPT_VALIDATE_GEOMETRY_BOUND_H)
#define __I3_OPT_VALIDATE_GEOMETRY_BOUND_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptValidateGeometryBound : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptValidateGeometryBound, i3SceneOptimizer);

protected:
	void	_CalcBoundSphere( i3GeometryAttr * pAttr);

public:
	i3OptValidateGeometryBound(void);

	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
};


#endif
