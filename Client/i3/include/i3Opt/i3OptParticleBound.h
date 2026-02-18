#if !defined( __I3_OPT_PARTICLE_BOUND_H)
#define __I3_OPT_PARTICLE_BOUND_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptParticleBound : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptParticleBound, i3SceneOptimizer);

public:
	i3OptParticleBound(void);
	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
