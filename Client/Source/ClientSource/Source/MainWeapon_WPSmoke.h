#pragma once

#include "MainWeapon_Grenade.h"
#include "MainWeapon_WPSmokeShell.h"
class MainWeapon_WPSmoke : public MainWeapon_Grenade
{
	friend class MainWeapon_WPSmokeShell;

public:
	MainWeapon_WPSmoke(WeaponBase* p);

	virtual void	Explosion() override;

private:
	virtual void	OnInitVariable() override;
	virtual void	OnUpdate_Grenade( REAL32 rDeltaSeconds) override;
	virtual void	Reset() override;
	void			_ProcessDamageBySmoke( void);

	REAL32			m_rFlushTime;
	REAL32			m_rDamageCheckTime;
	REAL32			m_rUserOverlapTime[ SLOT_MAX_COUNT];
	REAL32			m_rRadiusSlope;
	bool			m_bUserFirstOverlap[ SLOT_MAX_COUNT];

};