#ifndef _GAME_CHARA_DOMI_ACID_NOT_BOMB_H_
#define _GAME_CHARA_DOMI_ACID_NOT_BOMB_H_

#include "GameCharaFullbodyDinosaur.h"

class CGameCharaDomiAcidNotBomb : public CGameCharaFullbodyDinosaur
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaDomiAcidNotBomb, CGameCharaFullbodyDinosaur);

public:
	virtual void Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void ResetChara() override;
	virtual void OnBindResource() override;
	virtual void OnUpdate(REAL32) override;
};

#endif