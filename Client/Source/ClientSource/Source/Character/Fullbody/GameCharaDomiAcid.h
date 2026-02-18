#ifndef _GAME_CHARA_DOMI_ACID_H_
#define _GAME_CHARA_DOMI_ACID_H_

#include "GameCharaFullbodyDinosaur.h"

class CGameCharaDomiAcid : public CGameCharaFullbodyDinosaur
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaDomiAcid, CGameCharaFullbodyDinosaur);

public:
	virtual void Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void ResetChara() override;
	virtual void OnBindResource() override;
	virtual void OnUpdate(REAL32) override;
};

#endif