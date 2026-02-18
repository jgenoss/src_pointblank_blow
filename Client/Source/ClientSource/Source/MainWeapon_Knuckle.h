#pragma once

#include "MainWeapon_DualKnife.h"

class MainWeapon_Knuckle : public MainWeapon_DualKnife
{
public:
	MainWeapon_Knuckle(WeaponBase* p);

	void	NET_Fire_Knuckle( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);
	void			PlayIdleAnim( void);
	void			PlayFireAnim( void);
	void			PlayUIAnim(void);

	void			OnUpdate(REAL32 rDeltaSeconds);

private:
	virtual void		OnCreate( bool bCreateInWeapon) override;
	virtual void		OnBindResource() override;
	virtual void		OnPlayChangeAni( REAL32 rTimeScale) override;

	
protected:
	WEAPON_HUMAN_TYPE	GetOwnerHumanType(void);

	INT8			m_aiiIdle [ WEAPON_HUMAN_MAX][ WEAPON_DUAL_HANDTYPE_MAX];
	INT8			m_aiiChange [ WEAPON_HUMAN_MAX][ WEAPON_DUAL_HANDTYPE_MAX];
	INT8			m_aiiAttackA [ WEAPON_HUMAN_MAX][ WEAPON_DUAL_HANDTYPE_MAX];
	INT8			m_aiiAttackB [ WEAPON_HUMAN_MAX][ WEAPON_DUAL_HANDTYPE_MAX];

	bool			m_bSoundstop;
};


