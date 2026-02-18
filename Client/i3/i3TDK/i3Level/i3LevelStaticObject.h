#if !defined( __I3_Level_STATIC_OBJECT_H)
#define __I3_Level_STATIC_OBJECT_H

#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelStaticObject : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelStaticObject, i3LevelElement3D);
	
public:
	i3LevelStaticObject(void);

	virtual void	BindRes( i3LevelRes * pRes) override;
};

#endif