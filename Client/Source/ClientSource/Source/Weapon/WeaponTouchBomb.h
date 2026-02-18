#pragma once

#include "WeaponGrenade.h"

class WeaponTouchBomb : public WeaponGrenade
{
	I3_CLASS_DEFINE(WeaponTouchBomb, WeaponGrenade);
public:
	enum GRENADESEX_TYPE
	{
		GRENADESEX_MALE = 0,
		GRENADESEX_FEMALE,

		GRENADESEX_MAX
	};

protected:
	virtual MainWeapon*	CreateMainWeapon() override;

public:
	WeaponTouchBomb();
	virtual ~WeaponTouchBomb();

public: 
	virtual void	OnBindResource(void) override;
	virtual void	FireReady(void) override;

public:
	virtual void	_OnPlayWeaponIdleAnim(void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;

private:
	INT8	aiIdle[GRENADESEX_MAX];
	INT8	aiFire[GRENADESEX_MAX];
	INT8	aiLoadMag[GRENADESEX_MAX];
	INT8	aiLoadBullet[GRENADESEX_MAX];
};