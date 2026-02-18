#if !defined( __I3LV_PRIM_BOX_H)
#define __I3LV_PRIM_BOX_H

#include "i3LevelPrimitive.h"

class I3_EXPORT_TDK i3LevelPrimitive_Box : public i3LevelPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPrimitive_Box, i3LevelPrimitive);
	
public:

	virtual	void	CreateSymbol(void) override;

	virtual void	OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 *pInfo) override;

	static i3Node *	BuildSg(void);
};

#endif

