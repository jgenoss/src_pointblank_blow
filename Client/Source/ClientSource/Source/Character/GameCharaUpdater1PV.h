#if !defined( __GAME_CHARA_UPDATER_1PV_H__)
#define __GAME_CHARA_UPDATER_1PV_H__

#include "GameCharaUpdater.h"

class CGameCharaUpdater1PV : public CGameCharaUpdater
{
	I3_CLASS_DEFINE( CGameCharaUpdater1PV, CGameCharaUpdater);
private:
	void	Process1PVOnly( REAL32 rDeltatime);

public:
	virtual void	OnUpdate( REAL32 rDeltatime) override;

};

#endif
