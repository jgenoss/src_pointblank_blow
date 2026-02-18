#if !defined( __I3_COLLISION_SOLVER_H)
#define __I3_COLLISION_SOLVER_H

#include "i3Collidee.h"

class I3_EXPORT_SCENE i3CollisionSolver : public i3ElementBase
{
	I3_CLASS_DEFINE( i3CollisionSolver);

public:
	i3CollisionSolver(void);
	virtual ~i3CollisionSolver(void);

};

#endif
