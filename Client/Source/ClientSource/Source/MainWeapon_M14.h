#pragma once

#include "MainWeapon_MedicalKit.h"

#define MINE_POS_ASSISTTIME		0.18f
#define MINE_POS_ASSISTTIMEINV		5.5555555555555555555555555555555f

class MainWeapon_M14 : public MainWeapon_MedicalKit
{
public:
	MainWeapon_M14(WeaponBase * p);
	virtual void	Explosion(/*bool bShot = false*/) override;
	void			OnGrenadeAniRotate(REAL32 rDeltaTime);

protected:
	bool	IsPossibleHit(CGameCharaBase * pChara);
	
	virtual void	UpdateNetworkData(REAL32 rDeltatime) override;
	virtual void	CheckWorldCollision(REAL32 rDeltaSeconds) override;
	virtual void	OnUpdate_Grenade(REAL32 rDeltaSeconds) override;

private :
	bool	OnHitChara(CGameCharaBase * pChara);
};