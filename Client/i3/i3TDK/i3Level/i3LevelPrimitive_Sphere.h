#if !defined( __I3LV_PRIM_SPHERE_H)
#define __I3LV_PRIM_SPHERE_H

#include "i3LevelPrimitive.h"

class I3_EXPORT_TDK i3LevelPrimitive_Sphere : public i3LevelPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPrimitive_Sphere, i3LevelPrimitive);

public:
	virtual	void	CreateSymbol(void) override;
	virtual void	OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 *pInfo) override;

	static i3Node *	BuildSg(void);
};

#endif

