#if !defined( __I3_OPT_BUILD_BOUND_SPHERE_H)
#define __I3_OPT_BUILD_BOUND_SPHERE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildBoundSphere : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildBoundSphere, i3SceneOptimizer);

protected:
	void		_BoundSphere( i3VertexArray * pVA);

public:
	i3OptBuildBoundSphere(void);

	virtual bool		OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
