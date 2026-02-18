#if !defined( __GAME_CHARA_MOVE_CONTROL_NETWORK_H__)
#define __GAME_CHARA_MOVE_CONTROL_NETWORK_H__

#include "GameCharaMoveControl.h"

class CGameCharaMoveControlNetwork : public CGameCharaMoveControl
{
	I3_CLASS_DEFINE( CGameCharaMoveControlNetwork, CGameCharaMoveControl);
protected:
public:
	CGameCharaMoveControlNetwork();
	virtual ~CGameCharaMoveControlNetwork();

	virtual void	OnProcessFloorCheck( REAL32 rDeltatime);
};

#endif
