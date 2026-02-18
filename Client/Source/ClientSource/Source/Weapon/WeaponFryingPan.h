#ifndef _WEAPON_FRYINGPAN_H_
#define _WEAPON_FRYINGPAN_H_

class WeaponFryingPan : public WeaponMiniAxe
{
	I3_CLASS_DEFINE(WeaponFryingPan, WeaponMiniAxe);

private:
	MainWeapon* CreateMainWeapon();

public:
	WeaponFryingPan();
	virtual ~WeaponFryingPan();

	virtual void PlayAnim_UI_IdleB() override;
	
protected:
	virtual void _OnPlayWeaponIdleAnim() override;
	virtual void _OnPlayWeaponFireAnim() override;
	virtual void _OnPlayWeaponSecondFireAnim() override;
};

#endif