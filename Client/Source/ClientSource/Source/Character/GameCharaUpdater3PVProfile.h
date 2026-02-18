#if !defined( __GAME_CHARA_UPDATER_3PV_PROFILE_H__)
#define __GAME_CHARA_UPDATER_3PV_PROFILE_H__

#include "GameCharaUpdater3PV.h"

class CGameCharaUpdater3PVProfile : public CGameCharaUpdater3PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater3PVProfile, CGameCharaUpdater3PV);

public:
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	virtual void	OnProcessJump( REAL32 rDeltatime) override	{}
};

#endif
