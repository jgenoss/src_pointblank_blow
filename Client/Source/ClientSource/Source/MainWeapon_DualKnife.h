#pragma once

#include "MainWeapon_Dual.h"

class MainWeapon_DualKnife : public MainWeapon_Dual
{

public:
	MainWeapon_DualKnife(WeaponBase* p);
		
	void					SetAttackType( WEAPON_KNIFE_ATTACK_TYPE type)		{ m_nAttackType = type;			}
	void					SetHandType( WEAPON_DUAL_HANDTYPE type)		{ m_HandType = type;		}
	WEAPON_DUAL_HANDTYPE	GetHandType( void)					{ return m_HandType;		}	


	void			NET_Fire_DualKnife( VEC3D *pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE);

protected:
	virtual void		OnBindResource() override;
	virtual void		OnPlayChangeAni( REAL32 rTimeScale) override;

	INT8				m_aiiDualChange[WEAPON_DUAL_HANDTYPE_MAX];

	INT8				m_aiiDualAttackA[WEAPON_DUAL_HANDTYPE_MAX];
	INT8				m_aiiDualAttackB[WEAPON_DUAL_HANDTYPE_MAX];
	
private:
	
	virtual void		OnCreate( bool bCreateInWeapon) override;
	virtual void		OnInitVariable() override;

	virtual void		ReturnToReady() override;

	INT8			m_aiiUI_RebelMale[WEAPON_DUAL_HANDTYPE_MAX];
	INT8			m_aiiUI_RebelFemale[WEAPON_DUAL_HANDTYPE_MAX];

	INT8			m_aiiUI_RebelMale2[WEAPON_DUAL_HANDTYPE_MAX];
	INT8			m_aiiUI_RebelFemale2[WEAPON_DUAL_HANDTYPE_MAX];

	WEAPON_DUAL_HANDTYPE		m_HandType;
	WEAPON_KNIFE_ATTACK_TYPE	m_nAttackType;
	INT32						m_iAttackSoundPlayCount;
};

