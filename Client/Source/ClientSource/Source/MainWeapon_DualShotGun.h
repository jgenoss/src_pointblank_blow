#pragma once

#include "MainWeapon_Dual.h"

class MainWeapon_DualShotGun : public MainWeapon_Dual
{
	enum { RIGHT_FIRE = 0x01, LEFT_FIRE = 0x02, DUAL_FIRE = 0x03 };

public:
	enum { STATE_READY = 0, STATE_FIRING = 1, STATE_END = 2};
	MainWeapon_DualShotGun(WeaponBase* p);

private:
	void			_ChangeFireHand(WEAPON::FIRE_ORDER type);	
	const bool		_isFireHand(UINT8 ui8Hand)	const	{ return (m_ui8FireHand & ui8Hand) == ui8Hand; }

protected:
	virtual void	OnCreate(bool bCreateInWeapon) override;

private:
	virtual void	ReturnInstance() override;
	virtual void	Reset() override;
	virtual void	ReturnToReady() override;

public:
	bool			Fire_DualShotGun(WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE );
	void			FireEnd_DualShotGun(void);
	void			NET_Fire_DualShotGun( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type );

	void			PlayAnim_Change_DualShotGun(REAL32 rTimeScale = 1.f);

	const bool		isRightFire(void)			const	{ return _isFireHand(RIGHT_FIRE); }
	const bool		isLeftFire(void)			const	{ return _isFireHand(LEFT_FIRE); }
	const bool		isDualFire(void)			const	{ return _isFireHand(DUAL_FIRE); }

	void			setFireState(UINT8 state)			{ m_ui8FireState = state; }
	const bool		isFireState(UINT8 state)	const	{ return m_ui8FireState == state; }

	const REAL32	GetDualShotGunTimeScale();

private:
	UINT8			m_ui8FireHand;
	UINT8			m_ui8NextFireHand;

	UINT8			m_ui8FireState;	
};

