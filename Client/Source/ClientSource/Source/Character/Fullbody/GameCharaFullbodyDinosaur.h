#ifndef _GAME_CHARA_FULLBODY_DINOSAUR_H_
#define _GAME_CHARA_FULLBODY_DINOSAUR_H_

#include "GameCharaFullbody.h"

class CGameCharaFullbodyDinosaur : public CGameCharaFullbody
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaFullbodyDinosaur, CGameCharaFullbody);

private:
	REAL32 hp_max = 0.0f;

public:
	virtual void		Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void		CreateFirst(const CCharaInfo*, bool bInLobby = false) override;
	virtual void		CreateLater(const CCharaInfo*, bool bLoadWeapon = true) override;
	virtual void		ResetChara() override;
	virtual void		OnBindResource() override;
	virtual void		OnUpdate(REAL32) override;

	virtual void	setHpTagMaxHp( REAL32 max) override { hp_max = max;}
	virtual REAL32	getHpTagMaxHp() const override { return hp_max;}
};

#endif