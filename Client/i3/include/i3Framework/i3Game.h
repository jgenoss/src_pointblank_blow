#if !defined( __I3_GAME_H)
#define __I3_GAME_H

#include "i3Framework.h"
#include "i3EffectManager.h"

namespace i3Game
{
	I3_EXPORT_FRAMEWORK		i3Framework *		getFramework(void);
	I3_EXPORT_FRAMEWORK		void				setFramework( i3Framework * pFramework);
	I3_EXPORT_FRAMEWORK		i3EffectManager *	getEffectManager(void);
};

#endif
