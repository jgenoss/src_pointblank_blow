#ifndef _GAME_CHARA_DOMI_STING_MARS_H_
#define _GAME_CHARA_DOMI_STING_MARS_H_

#include "GameCharaDomiSting.h"

class WeaponBase;
class CGameCharaDomiStingMars : public CGameCharaDomiSting
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaDomiStingMars, CGameCharaDomiSting);

public:
	virtual void Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void ResetChara() override;
	virtual void OnBindResource() override;
	virtual void OnUpdate(REAL32) override;
};

#endif