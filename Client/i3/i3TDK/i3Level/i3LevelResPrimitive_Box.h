#if !defined( __I3LV_RES_PRIM_BOX_H)
#define __I3LV_RES_PRIM_BOX_H

#include "i3LevelResPrim.h"

class I3_EXPORT_TDK i3LevelResPrimitive_Box : public i3LevelResPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResPrimitive_Box, i3LevelResPrimitive);
	

protected:
	virtual i3Node	*	_BuildScene(void) override;
	virtual i3ClassMeta *	GetInstanceMeta(void) override;

public:
	i3LevelResPrimitive_Box();

};

#endif
