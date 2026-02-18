#pragma once

#include "MainWeapon_MiniAxe.h"

class MainWeapon_FryingPan : public MainWeapon_MiniAxe
{
public:
	MainWeapon_FryingPan(WeaponBase* p);

	void OnIdleAnim();
	void OnFireAnim();
	void OnSecondaryFireAnim();

private:
	virtual void OnBindResource() override;

private:
	INT8 m_ai_IdleFemale;
	INT8 m_ai_FireFemale;
	INT8 m_ai_SecondaryFireFemale;
};

