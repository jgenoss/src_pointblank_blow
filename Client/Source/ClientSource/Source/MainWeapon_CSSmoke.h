#pragma once

#include "MainWeapon_Grenade.h"
#include "MainWeapon_WPSmokeShell.h"
class MainWeapon_CSSmoke : public MainWeapon_Grenade
{
public:
	MainWeapon_CSSmoke(WeaponBase* p);

	virtual void	Explosion() override;

private:
	virtual void	OnInitVariable() override;
	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds) override;
	virtual void	Reset() override;
	void			_ProcessHazeBySmoke( void);

	REAL32			m_rEffectTime;
	REAL32			m_rRadiusSlope;
};