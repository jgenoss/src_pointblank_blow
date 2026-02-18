#if !defined( __GAME_CHARA_UPDATER_1PV_LOCAL_H__)
#define __GAME_CHARA_UPDATER_1PV_LOCAL_H__

#include "GameCharaUpdater1PV.h"

class CGameCharaUpdater1PVLocal : public CGameCharaUpdater1PV
{
	I3_CLASS_DEFINE( CGameCharaUpdater1PVLocal, CGameCharaUpdater1PV);
protected:
public:
	CGameCharaUpdater1PVLocal();
	virtual ~CGameCharaUpdater1PVLocal();

	/*virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);*/
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
	virtual void	OnProcess1PVOnly( REAL32 rDeltatime);
};

#endif
