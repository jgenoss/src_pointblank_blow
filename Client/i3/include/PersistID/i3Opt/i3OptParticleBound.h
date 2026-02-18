#if !defined( __I3_OPT_PARTICLE_BOUND_H)
#define __I3_OPT_PARTICLE_BOUND_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptParticleBound : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptParticleBound);
protected:

protected:

public:
	i3OptParticleBound(void);
	virtual ~i3OptParticleBound(void);

	virtual BOOL OnNode( i3Node * pNode);
};

#endif
