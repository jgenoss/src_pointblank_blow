#if !defined( __WEAPON_DUAL_SWORD_H__)
#define __WEAPON_DUAL_SWORD_H__

#include "WeaponDualKnife.h"

class WeaponDualSwordTH : public WeaponDualKnife
{
	I3_CLASS_DEFINE(WeaponDualSwordTH, WeaponDualKnife);

public:
	WeaponDualSwordTH();
	virtual ~WeaponDualSwordTH();

	virtual void OnBindResource() override;

	void OnAttackBRight(i3AIContext* pCtx, REAL32 tm);

private:
	INT8 m_aiiAttackBRight3PV;
};

#endif
