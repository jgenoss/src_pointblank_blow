#pragma once

#include "MainWeapon.h"
#include "MainWeapon_Knife.h"

namespace
{
	enum { KNIFE_GENDER_COUNT = 2,	KNIFE_AI_COUNT = 5 } ;
};

class MainWeapon_KnifeGender : public MainWeapon_Knife
{
public:
	MainWeapon_KnifeGender(WeaponBase* p);

	virtual void		OnBindResource() override;
	virtual void		SetOwner( CGameCharaBase * pOwner) override;
	
private:
	void	_BindAnimationIndex(CGameCharaBase * pOwner);

	INT8			m_i8AIIndex[KNIFE_GENDER_COUNT][KNIFE_AI_COUNT];

};

