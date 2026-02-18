#if !defined( __I3LV_RES_PRIM_CAPSULE_H)
#define __I3LV_RES_PRIM_CAPSULE_H

#include "i3LevelResPrim.h"

class I3_EXPORT_TDK i3LevelResPrimitive_Capsule : public i3LevelResPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResPrimitive_Capsule, i3LevelResPrimitive);
	
protected:
	virtual i3Node * _BuildScene() override;
	virtual i3ClassMeta * GetInstanceMeta() override;

public :
	i3LevelResPrimitive_Capsule();

};

#endif