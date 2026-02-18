#if !defined( __GAME_CHARA_MOVE_CONTROL_LOCAL_H__)
#define __GAME_CHARA_MOVE_CONTROL_LOCAL_H__

#include "GameCharaMoveControl.h"

class CGameCharaMoveControlLocal : public CGameCharaMoveControl
{
	I3_CLASS_DEFINE( CGameCharaMoveControlLocal, CGameCharaMoveControl);
protected:
public:
	CGameCharaMoveControlLocal();
	virtual ~CGameCharaMoveControlLocal();

	virtual void	OnUpdate( REAL32 rDeltatime);

	virtual void	OnProcessWorldCollision( REAL32 rDeltatime);
	virtual void	OnChangeHeight( REAL32 rHeight);

	virtual void	OnProcessFloorCheck( REAL32 rDeltatime);
};

#endif
