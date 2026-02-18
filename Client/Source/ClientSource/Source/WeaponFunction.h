#pragma once

//
//  WeaponBase의 멤버함수가 너무나도 많은데, 이중에 일반함수로 뺄수 있다고 판단되는 것은 
//  일반 함수로 분리하고, 유틸화한다..
//

namespace WeaponFunction 
{
	INT8	FindAIStateIndex(WeaponBase* pWeapon, const char* aiName);	

	bool	PlayWeaponAnim( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale = 1.f);
	bool	PlayWeaponAnimDirect( WeaponBase* pWeapon, INT8 aiStateIdx, REAL32 rTimeScale = 1.f);
	
	void	PlayWeaponAnim(WeaponBase* pWeapon, const char * pszName);
	
	void	SetCollideeGroup_NoUseCollidee(WeaponBase* pWeapon);
	void	SetCollideeGroup_NormalWeapon(WeaponBase* pWeapon);
	void	SetCollideeGroup_HitWeapon(WeaponBase* pWeapon);
	void	SetCollideeGroup_DinoDummy(WeaponBase* pWeapon);
	void	SetCollideeGroup_SentryGun(WeaponBase* pWeapon);
	void	SetCollideeGroup_Supply(WeaponBase* pWeapon);

}

namespace WF = WeaponFunction;