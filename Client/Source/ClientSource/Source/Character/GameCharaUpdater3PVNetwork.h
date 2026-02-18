#if !defined( __GAME_CHARA_UPDATER_3PV_NETWORK_H__)
#define __GAME_CHARA_UPDATER_3PV_NETWORK_H__

#include "GameCharaUpdater3PV.h"

class CGameCharaUpdater3PVNetwork : public CGameCharaUpdater3PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater3PVNetwork, CGameCharaUpdater3PV);
protected:
public:
	CGameCharaUpdater3PVNetwork();
	virtual ~CGameCharaUpdater3PVNetwork();

	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
};

#endif
