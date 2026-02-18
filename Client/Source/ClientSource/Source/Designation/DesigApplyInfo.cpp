#include "pch.h"
#include "DesigApplyInfo.h"


void ResetDesigApplyInfo_Chara(DesigApplyInfo_Chara* info_chara)
{
	info_chara->charaMoveSpeed = 0.f;
	info_chara->charaJump = 0.f;
	info_chara->charaAddSpeed = 0.f;
	info_chara->charaFallDamageReduce = 0.f;
}

void ResetDesigApplyInfo_Weapon(DesigApplyInfo_Weapon* info_weapon)
{
	info_weapon->applyType = DESIGNATION_APPLY_TYPE_ALL;
	info_weapon->weaponFireSpeed = 0.f;
	info_weapon->weaponMoveSpeed = 0.f;
	info_weapon->weaponReloadTime = 0.f;
	info_weapon->weaponAttackRange = 0.f;
	info_weapon->weaponHorzReaction = 0.f;
	info_weapon->weaponVertReaction = 0.f;
	info_weapon->weaponDeclination = 0.f;
	info_weapon->weaponThrowSpeed = 0.f;
	info_weapon->weaponBombTime = 0.f;
	info_weapon->weaponBombRange = 0.f;
	info_weapon->weaponReaction = 0.f;
	info_weapon->weaponDPDRate = 0.f;
	info_weapon->weaponFirstShotRate = 0.f;
}

void ResetDesigApplyInfo(DesigApplyInfo* info)
{
	ResetDesigApplyInfo_Chara(&info->desigChara);
	ResetDesigApplyInfo_Weapon(&info->desigWeapon);
}

