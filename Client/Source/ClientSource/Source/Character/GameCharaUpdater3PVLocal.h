#if !defined( __GAME_CHARA_UPDATER_3PV_LOCAL_H__)
#define __GAME_CHARA_UPDATER_3PV_LOCAL_H__

#include "GameCharaUpdater3PV.h"

class CGameCharaUpdater3PVLocal : public CGameCharaUpdater3PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater3PVLocal, CGameCharaUpdater3PV);
protected:
public:
	CGameCharaUpdater3PVLocal();
	virtual ~CGameCharaUpdater3PVLocal();

	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
};

#endif
