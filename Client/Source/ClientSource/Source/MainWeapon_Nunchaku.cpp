#include "pch.h"
#include "MainWeapon_Nunchaku.h"

MainWeapon_Nunchaku::MainWeapon_Nunchaku(WeaponBase * p) : MainWeapon_Knife(p)
{
}

void MainWeapon_Nunchaku::OnPlayChangeAni(REAL32 rTimeScale)
{
	if (m_pWeaponBase != nullptr)
	{
		m_pWeaponBase->OnPlayChangeAniImpl(rTimeScale);
	}
}
