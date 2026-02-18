#pragma once


struct DesigApplyInfo_Chara
{
	REAL32		charaMoveSpeed = 0.0f;
	REAL32		charaJump = 0.0f;
	REAL32		charaAddSpeed = 0.0f;
	REAL32		charaFallDamageReduce = 0.0f;
};

void ResetDesigApplyInfo_Chara(DesigApplyInfo_Chara* info_chara);

struct DesigApplyInfo_Weapon
{
	DESIGNATION_APPLY_TYPE	applyType = DESIGNATION_APPLY_TYPE_ALL;		// 기존 호칭시스템 무기종 구별 안하는 버그때문에 실사용은 하지 않음.

	REAL32		weaponFireSpeed = 0.0f;
	REAL32		weaponMoveSpeed = 0.0f;
	REAL32		weaponReloadTime = 0.0f;
	REAL32		weaponAttackRange = 0.0f;
	REAL32		weaponHorzReaction = 0.0f;
	REAL32		weaponVertReaction = 0.0f;
	REAL32		weaponDeclination = 0.0f;
	REAL32		weaponThrowSpeed = 0.0f;
	REAL32		weaponBombTime = 0.0f;
	REAL32		weaponBombRange = 0.0f;
	REAL32		weaponReaction = 0.0f;
	REAL32		weaponDPDRate = 0.0f;
	REAL32		weaponFirstShotRate = 0.0f;
	//	REAL32		weaponDamage;			// 쓰지 않아 보임..(코드삭제는 하지말것)
	//	REAL32		weaponScoopeRange;		// 쓰지 않아 보임..(코드삭제는 하지말것)
	//	REAL32		weaponCrossHairRestore;	// 쓰지 않아 보임..(코드삭제는 하지말것)
};

void ResetDesigApplyInfo_Weapon(DesigApplyInfo_Weapon* info_weapon);

struct DesigApplyInfo
{
	DesigApplyInfo_Chara	desigChara;
	DesigApplyInfo_Weapon	desigWeapon;
};

void ResetDesigApplyInfo(DesigApplyInfo* info);
