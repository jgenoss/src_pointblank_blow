#if !defined( __I3LV_RES_PRIM_RESPAWN_H)
#define __I3LV_RES_PRIM_RESPAWN_H

#include "i3LevelResPrim.h"

class I3_EXPORT_TDK i3LevelResPrimitive_Respawn : public i3LevelResPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResPrimitive_Respawn, i3LevelResPrimitive);
	
public:
	i3LevelResPrimitive_Respawn();

protected:
	virtual i3Node	*	_BuildScene(void) override;
	virtual i3ClassMeta *	GetInstanceMeta(void) override;

	virtual bool		OnExport( i3LevelScene * pScene, const char * pszPath) override;
	virtual i3GameRes*	CreateGameRes() override;
};

#endif
