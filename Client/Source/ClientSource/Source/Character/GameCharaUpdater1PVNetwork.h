#if !defined( __GAME_CHARA_UPDATER_1PV_NETWORK_H__)
#define __GAME_CHARA_UPDATER_1PV_NETWORK_H__

#include "GameCharaUpdater1PV.h"

class CGameCharaUpdater1PVNetwork : public CGameCharaUpdater1PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater1PVNetwork, CGameCharaUpdater1PV);
protected:
public:
	CGameCharaUpdater1PVNetwork();
	virtual ~CGameCharaUpdater1PVNetwork();

	virtual void	OnUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
	virtual void	OnProcess1PVOnly( REAL32 rDeltatime);
};

#endif
