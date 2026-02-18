#ifndef __GAME_CHARA_UPDATER_FULLBODY_H__
#define __GAME_CHARA_UPDATER_FULLBODY_H__

#include "GameCharaUpdater.h"

class CGameCharaUpdaterFullbody : public CGameCharaUpdater
{
	I3_CLASS_DEFINE(CGameCharaUpdaterFullbody, CGameCharaUpdater);

public:
	virtual void	OnFirstUpdate(REAL32) override;
	virtual void	OnUpdate(REAL32) override;
	virtual void	OnLastUpdate(REAL32) override;
};

#endif