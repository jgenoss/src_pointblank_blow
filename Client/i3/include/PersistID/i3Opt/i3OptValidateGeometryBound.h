#if !defined( __I3_OPT_VALIDATE_GEOMETRY_BOUND_H)
#define __I3_OPT_VALIDATE_GEOMETRY_BOUND_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptValidateGeometryBound : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptValidateGeometryBound);
protected:

protected:
	void	_CalcBoundSphere( i3VertexArray * pVA);

public:
	i3OptValidateGeometryBound(void);
	virtual ~i3OptValidateGeometryBound(void);

	virtual BOOL OnNode( i3Node * pNode);
};


#endif
