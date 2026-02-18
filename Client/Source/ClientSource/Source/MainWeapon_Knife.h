#pragma once

#include "MainWeapon.h"

class MainWeapon_Knife : public MainWeapon_Melee
{
public:
	MainWeapon_Knife(WeaponBase* p);

	virtual void OnCreate( bool bCreateInWeapon) override;
	virtual void OnInitVariable() override;
	virtual void ReturnToReady() override;
	virtual void OnBindResource() override;

	virtual void OnPlayChangeAni( REAL32 rTimeScale) override;

	virtual void SetKnifeAttackType( WEAPON_KNIFE_ATTACK_TYPE type) override { m_KnifeAttackType = type;			}

	INT8 GetAttackA_Left() const { return m_aiiAttackA_Left; }			
	INT8 GetAttackB_Left() const { return m_aiiAttackB_Left; }

	INT32 GetCandyEffectIdx() const { return m_idxCandyEffect; }

	WEAPON_KNIFE_ATTACK_TYPE GetKnifeAttackType() const { return m_KnifeAttackType; }


	void			PlayAnim_UI_RebelMale_Left( void);
	void			PlayAnim_UI_RebelFemale_Left( void);

	void			PlayAnim_UI_RebelMale_Left2( void);
	void			PlayAnim_UI_RebelFemale_Left2( void);

	void			PlayAnim_UI_Rebel_Common(void);
	void			PlayAnim_UI_Swat_Common(void);


private:

	WEAPON_KNIFE_ATTACK_TYPE	m_KnifeAttackType;

	// 아래의 멤버변수들은 모두 다시 다른 위치로 옮겨져야한다...
	INT8						m_aiiChangeLeft;                  
	INT8						m_aiiAttackA_Left;
	INT8						m_aiiAttackB_Left;

	INT8						m_aiiUI_RebelMale_Left;
	INT8						m_aiiUI_RebelFemale_Left;

	INT8						m_aiiUI_RebelMale_Left2;
	INT8						m_aiiUI_RebelFemale_Left2;

	INT8						m_aiiUI_Rebel_Common;
	INT8						m_aiiUI_Swat_Common;

	INT32						m_idxCandyEffect;
	
	
};

