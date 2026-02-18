#if !defined( __UI_PHASE_CLANBATTLELOBBY_H__)
#define __UI_PHASE_CLANBATTLELOBBY_H__

#include "UIPhaseClanBase.h"

class UIPhaseClanBattleLobby : public UIPhaseClanBase
{
	I3_CLASS_DEFINE( UIPhaseClanBattleLobby);

public:
	UIPhaseClanBattleLobby();
	virtual ~UIPhaseClanBattleLobby();

	virtual void	OnCreate( i3GameNode * pParent);
};

#endif
