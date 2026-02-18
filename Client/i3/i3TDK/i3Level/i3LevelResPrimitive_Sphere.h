#if !defined( __I3LV_RES_PRIM_SPHERE_H)
#define __I3LV_RES_PRIM_SPHERE_H

#include "i3LevelResPrim.h"

class I3_EXPORT_TDK i3LevelResPrimitive_Sphere : public i3LevelResPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResPrimitive_Sphere, i3LevelResPrimitive);
protected:
	NxSphereShape *		m_pShape = nullptr;

protected:
	virtual i3Node	*	_BuildScene(void) override;
	virtual i3ClassMeta *	GetInstanceMeta(void) override;

public:
	i3LevelResPrimitive_Sphere();
};

#endif
