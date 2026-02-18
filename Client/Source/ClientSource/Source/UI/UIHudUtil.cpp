#include "pch.h"
#include "UIHudUtil.h"
#include "UIHudUtil_UnitTest.hpp"

#include "UIHudManager.h"
#include "GameCharaWeaponContext.h"
#include "Camera/GameCamera_OnObject.h"

#include "GameMode.h"
#include "TutorialMode.h"
#include "MedalManager.h"
#include "MedalSetBase.h"

#include "StageBattle.h"
#include "Weapon/WeaponOnObject.h"
#include "Weapon/WeaponM197.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"

#include "WeaponM197.h"
#include "GameCharaDinoCCRaptor.h"

#include "../StageBattle/DinoModeContext.h"
#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/UserContext.h"
#include "BattleSlotContext.h"
#include "BattleResultContext.h"
#include "GameMissionManager.h"
#include "MainWeapon_C5.h"

#include "ShopContext.h"
#include "UserInfoContext.h"
#include "ClanGameContext.h"
#include "GameMaster.h"
#include "../Weapon/WeaponItemID.h"
#include "../Weapon/WeaponInfo/M197Info.h"
#include "../Weapon/WeaponInfo/MeleeWeaponInfo.h"

#include "UIUtil.h"

#include "i3Base/string/ext/format_string.h"

//- extern -----------------//
hu_dino::attribute hu_dino::att;

//- hu_dino -----------------//
void hu_dino::reset_dino_attribute() 
{
	hu_dino::att.wstr_name.clear(); hu_dino::att.idx1 = hu_dino::att.idx2 = DinoState::None; hu_dino::att.ratio = 1.f;
}
void hu_dino::set_dino_attribute(const i3::rc_wstring& name, DinoState::Attack idx1, DinoState::Attack idx2) 
{ 
	hu_dino::att.wstr_name = name; hu_dino::att.idx1 = idx1; hu_dino::att.idx2 = idx2; hu_dino::att.ratio = 1.f;
}
void hu_dino::set_dino_raio(REAL32 ratio) 
{ 
	hu_dino::att.ratio = ratio; 
}

i3::rc_wstring hu_dino::get_dino_attck_string(DinoState::Attack idx)
{
	switch( idx )
	{
	case DinoState::HOWL: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL");
	case DinoState::SCRATCH: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_SCRATCH");
	case DinoState::BITE: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_BITE");
	case DinoState::ELITE_SCRATCH: return GAME_RCSTRING("STR_TBL_INGAME_HUD_ELITE_ATTACK_A");
	case DinoState::RAPTOR_BUFF: return GAME_RCSTRING("BUFF"); 
	case DinoState::POISON_PRICKLE: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE");
	case DinoState::STING_BITE: return GAME_RCSTRING("BITE");
	case DinoState::BLAST: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_BLAST");
	case DinoState::BUTT: return GAME_RCSTRING("BUTT");
	case DinoState::RUSH: return GAME_RCSTRING("RUSH");
	case DinoState::POISON_GAS: return GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_GAS");
	}
	return i3::rc_wstring();
}

INT32 hu_dino::get_dino_shape_index()
{
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ACID") )  return 2;
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_ELITE") )  return 6;
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR") )  return 3;
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_STING") ) return 3;
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_TANK") )  return 4;
	if( hu_dino::att.wstr_name == GAME_RCSTRING("STR_TBL_INGAME_HUD_DINO_TYPE_TREX") )  return 5;

	return -1;
}

//- hc -----------------//
void hc::btn_set_shape(i3UIButtonImageSet* p, INT32 idx)
{
	if( idx < 0) { p->EnableCtrl(false); return; }

	p->EnableCtrl(true);
	p->SetShapeIdx(idx);
}


//- detail -----------------//
namespace
{
	enum 
	{
		MAX_TUTORIAL_MESSAGE = 29,
	};
}

namespace detail
{
	WeaponOnObject*	g_pM197 = 0;

	bool get_range_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
	void get_range_bullet_dualcnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
	bool get_melee_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
	bool get_throwing_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
	bool get_item_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
	bool get_mission_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);
}

//- detail -----------------//
bool detail::get_range_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	bullet = pWeapon->getLoadedBulletCount();
	max_bullet = pWeapon->getTotalBulletCount();

	max_load_bullet = pInfo->GetLoadBullet();

	if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon))
	{
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER 
			&& pWeapon->isExtensionActivate())
		{
			WeaponGrenadeLauncher* w = static_cast<WeaponGrenadeLauncher*>(pWeapon);
			bullet = w->getLoadedShellCount();
			max_bullet = w->getTotalShellCount();
		}
	}
	else if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER 
		     && i3::kind_of<WeaponShotGunLauncher*>(pWeapon))
	{
		if( pWeapon->isExtensionActivate())
		{
			WeaponShotGunLauncher* w = static_cast<WeaponShotGunLauncher*>(pWeapon);
			bullet = w->getLoadedShellCount();
			max_bullet = w->getTotalShellCount();
		}
	}
	else if( i3::kind_of<WeaponBow*>(pWeapon))
	{
		if( pWeapon->isExtensionActivate())
		{
			WeaponBow* w = static_cast<WeaponBow*>(pWeapon);
			bullet = w->getLoadedShellCount();
			max_bullet = w->getTotalShellCount();
		}
	}
	

 	switch (pWeapon->getMagazineType())
 	{
 	case WEAPON_DUAL_MAGAZINE_IDLE:	break;
 	case WEAPON_DUAL_MAGAZINE_RIGHT:
 		bullet = pWeapon->getLoadedBulletDualCount();
 		//max_load_bullet = pWeapon->getLoadedBulletDualCount();
 		break;
 	}

	return true;
}

void detail::get_range_bullet_dualcnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	get_range_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);

	if (i3::kind_of<WeaponDualMagazine*>(pWeapon))	//듀얼탄창
	{
		max_bullet += max_load_bullet;
		if(pWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT)
			bullet += pWeapon->getLoadedBulletCount();
		else
			bullet += pWeapon->getLoadedBulletDualCount();
	}
}


bool detail::get_melee_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	if(pInfo->GetTypeClass() == WEAPON_CLASS_KNUCKLE)
	{
		return false;
	}
	else if( pInfo->GetLoadBullet() > 0 )
	{
		//bullet 은 현재 장전된 총알, max_bullet은 최대 총알 수를 말한다.
		//근데. Miniaxe와 ballistic(탄도단검)은 던질 수 있기에 현재 장전된 총알은 1, 최대 총알 수는 현재 장전되 총알 수로 해야 될 듯
		//bullet = pWeapon->getLoadedBulletCount(); //기존 방법
		//max_bullet = pWeapon->getTotalBulletCount();

		//if(i3::same_of<WeaponDualThrowKnife*>(pWeapon) == true)
		//{
		//	bullet = 2; // 강제 고정 이건 차후에 빼야 할 듯 싶다.
		//	max_bullet = pWeapon->getLoadedBulletCount() - 2;
		//	max_load_bullet = pWeapon->GetLoadBulletWithCashItem() - 2;
		//}
		//else
		//{
		//	bullet = 1; //새로운 방법
		//	max_bullet = pWeapon->getLoadedBulletCount() - 1;
		//	max_load_bullet = pWeapon->GetLoadBulletWithCashItem() -1;
		//}


		//PP-23132 투척형 근접 무기 출력 수정
		INT32 autoFireCount = pInfo->GetAutoFireCount2();	//SecondaryAttack의 공격 횟수

		//투척 가능한 횟수 계산
		max_bullet = pWeapon->getLoadedBulletCount() / autoFireCount;
		max_load_bullet = pWeapon->GetLoadBulletWithCashItem() / autoFireCount;

		//마지막 것을 던질 수 없으면 마지막 1개를 투척횟수에서 제외한다
		if (((CMeleeWeaponInfo*)pInfo)->DoNotThrowLast())
		{
			max_bullet--;
			max_load_bullet--;
		}

		//남은 투척 횟수가 1회 이상일 경우 bullet의 값을 1로 만든 뒤 잔여 투척횟수를 1만큼 줄인다
		if (max_bullet >= 1)
		{
			bullet = 1;
			max_bullet--;
			max_load_bullet--;
		}
		else
		{
			bullet = 0;
		}

		return true;
	}

	return true;
}

bool detail::get_throwing_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	bullet = pWeapon->getLoadedBulletCount();
	max_bullet = pWeapon->getTotalBulletCount();
	max_load_bullet = pWeapon->GetLoadBulletWithCashItem();

	if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{
	}
	else
	{
	}

	return true;
}


bool detail::get_item_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	bullet = pWeapon->getLoadedBulletCount();
	max_bullet = pWeapon->getTotalBulletCount();
	max_load_bullet = pWeapon->GetLoadBulletWithCashItem();

	return true;
}

bool detail::get_mission_bullet_cnt(WeaponBase * pWeapon, CWeaponInfo* pInfo, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	bullet = pWeapon->getLoadedBulletCount();
	max_bullet = pWeapon->getTotalBulletCount();
	max_load_bullet = pWeapon->GetLoadBulletWithCashItem();

	return true;
}


/****************************************/
void hu::init_m179_object()
{
	detail::g_pM197 = 0;
	CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();

	STAGE_ID StageID = hu_ut::get_stage_id();
	if( StageID::GetStageUID(StageID) == STAGE_UID_OUTPOST || 
		StageID::GetStageUID(StageID) == STAGE_UID_TUTORIAL)
	{
		StageObjectListPtr pList = pStage->GetStageObjectList();

		for( size_t i = 0; i < pList->size(); i++)
		{
			i3Object * pObj = (*pList)[i];

			if( i3::same_of<CGameObjectRidingHelicopter*>(pObj))
			{
				detail::g_pM197 = ((CGameObjectRidingHelicopter*)pObj)->GetWeaponOnObject();
				break;
			}
		}
	}
	else if( StageID::GetStageUID(StageID) == STAGE_UID_HELISPOT)
	{
		StageObjectListPtr pList = pStage->GetStageObjectList();

		for( size_t i = 0; i < pList->size(); i++)
		{
			i3Object * pObj = (*pList)[i];

			if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
			{
				if( (((CGameObjectRespawnHelicopter*)pObj)->getTeamIdx() == 0) && (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED) )
				{
					detail::g_pM197 = ((CGameObjectRespawnHelicopter*)pObj)->GetWeaponOnObject();
					break;
				}
				else if( (((CGameObjectRespawnHelicopter*)pObj)->getTeamIdx() == 1) && (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE) )
				{
					detail::g_pM197 = ((CGameObjectRespawnHelicopter*)pObj)->GetWeaponOnObject();
					break;
				}
			}
		}
	}
}

bool hu::is_m179_mode()
{
	return detail::g_pM197 ? true : false;
}

bool hu::is_target_mode(HUD::TargetShapeType & type, bool & is_default_shape)
{
	if (StageID::GetStageMode(hu_ut::get_stage_id()) != STAGE_MODE_DEFENCE &&
		StageID::GetStageMode(hu_ut::get_stage_id()) != STAGE_MODE_DESTROY)
	{
		is_default_shape = true;
		type = HUD::TST_NONE;
		return false;
	}
		
	is_default_shape = true;
	switch( StageID::GetStageUID(hu_ut::get_stage_id()) )
	{
	case STAGE_UID_BREAKDOWN:
		type = HUD::TST_GENERATOR;
		return true;
		break;

	case STAGE_UID_GIRAN:	
	case STAGE_UID_GIRAN2 :
		type = HUD::TST_GIRAN;
		return true;
		break;

	case STAGE_UID_HELISPOT:
		type = HUD::TST_HELICOPTER;
		return true;
		break;

	case STAGE_UID_BLACKPANTHER :
		type = HUD::TST_TANK;
		is_default_shape = false;
		return true;
		break;
	}

	type = HUD::TST_NONE;
	return false;
}


/****************************************/
void hu::get_m179_gauge(REAL32 & gauge, REAL32 & max_gauge)
{
	gauge = max_gauge = 0.f;

	gauge = ((WeaponM197*)detail::g_pM197)->getFireGauge();
	max_gauge = ((CM197Info*)detail::g_pM197->getWeaponInfo())->GetContinueFireTime();
}

void hu::get_target_object_hp(INT32 & red_hp, INT32 & max_red_hp, INT32 & blue_hp, INT32 & max_blue_hp)
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
	{
		red_hp = hu_ut::test_factor.red_object_hp;
		blue_hp = hu_ut::test_factor.blue_object_hp;
		max_red_hp = max_blue_hp = 100;
		return;
	}
#endif

	red_hp = max_red_hp = blue_hp = max_blue_hp = 0;

	CGameMissionManager * pMissionMng = CGameMissionManager::i(); 

	i3Object *pDefObj1 = 0, *pDefObj2 = 0;
	if( hu_ut::is_destroy_mission_mode() == true )
	{
		pDefObj1 = pMissionMng->getRedObject();
		pDefObj2 = pMissionMng->getBlueObject();
	}
	else if( hu_ut::is_defence_mission_mode() == true )
	{
		pDefObj1 = pMissionMng->getDefenceObj(0);
		pDefObj2 = pMissionMng->getDefenceObj(1);
	}
	if(!pDefObj1 || !pDefObj2 ) return;

	red_hp = (INT32)pDefObj1->getCurrentHP();
	max_red_hp = (INT32)pDefObj1->getMaxHP();

	blue_hp = (INT32)pDefObj2->getCurrentHP();
	max_blue_hp = (INT32)pDefObj2->getMaxHP();
}

bool hu::get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet)
{
	INT32 max_load_bullet = 0;
	return hu::get_bullet_cnt(pChara, bullet, max_bullet, max_load_bullet);
}

bool hu::get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet)
{
	bullet = max_bullet = max_load_bullet = 0;

	if( pChara == nullptr) return false;
	if( pChara->getWeaponContext() == nullptr) return false;
	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return false;

	pWeapon = pWeapon->getUsedWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return false;

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
	{
		bullet = hu_ut::test_factor.bullet;
		max_bullet = max_load_bullet = 100;
		return true;
	}
		
#endif

	if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
		return detail::get_range_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);

	if( pInfo->isRange() ) return detail::get_range_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);
	if( pInfo->isMelee() ) return detail::get_melee_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);
	if( pInfo->isThrowing() ) return detail::get_throwing_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);
	if( pInfo->isItem() ) return detail::get_item_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);
	if( pInfo->isMission() ) return detail::get_mission_bullet_cnt(pWeapon, pInfo, bullet, max_bullet, max_load_bullet);

	return false;
}

bool hu::get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet)
{
	REAL32 max_load_bullet = 0;
	return hu::get_bullet_cntF(pChara, bullet, max_bullet, max_load_bullet);
}

bool hu::get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet, REAL32 & max_load_bullet)
{
	if( pChara == nullptr) return false;
	if( pChara->getWeaponContext() == nullptr) return false;
	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return false;

	pWeapon = pWeapon->getUsedWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return false;

	bullet = max_bullet = max_load_bullet = 0;

	bullet = pWeapon->getLoadedBulletCountF();
	//max_bullet = static_cast<REAL32>(pWeapon->getTotalBulletCount());

	max_bullet = static_cast<REAL32>(pInfo->GetLoadBullet());
	return true;
}
WEAPON_SLOT_TYPE hu::get_weapon_slot_type(CGameCharaBase* pChara)
{
	if( pChara == nullptr) return WEAPON_SLOT_UNKNOWN;
	if( pChara->getWeaponContext() == nullptr) return WEAPON_SLOT_UNKNOWN;
	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return WEAPON_SLOT_UNKNOWN;

	pWeapon = pWeapon->getUsedWeapon();
	if( pWeapon == nullptr )	return WEAPON_SLOT_UNKNOWN;

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return WEAPON_SLOT_UNKNOWN;

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		pInfo = hu_ut::test_factor.pInfo;
#endif

	return pInfo->GetTypeUsage();
}

bool hu::get_weapon_bullet_type(CGameCharaBase* pChara)
{
	if( pChara == nullptr) return false;
	if( pChara->getWeaponContext() == nullptr) return false;
	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return false;

	pWeapon = pWeapon->getUsedWeapon();
	if( pWeapon == nullptr )	return false;

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return false;

	if( pInfo->GetGaugeReload() == 1)
		return true;

	return false;
}

void hu::get_chara_hp(CGameCharaBase* pChara, INT32 & hp, INT32 & max_hp)
{
	if(pChara == nullptr)	
	{
		hp = max_hp = 0;
		return;
	}

	hp = pChara->getCurHP();
	if( hp < 0) hp = 0;

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		hp = hu_ut::test_factor.hp;
#endif

	if( hu_ut::is_dino(pChara) == true )
		max_hp = (INT32)pChara->GetDinoFullHP();
	else
		max_hp = pChara->getFullHP();

	if( hp > max_hp )
		max_hp = hp;
}

REAL32 hu::get_chara_hp_ratio(CGameCharaBase* pChara)
{
	REAL32 percentage = 0.0f;
	get_chara_hp(pChara, percentage);

	REAL32 ratio = percentage * 0.01f;

	if (ratio < 0.0f)
		ratio = 0.0f;
	else if (ratio > 1.0f)
		ratio = 1.0f;

	return ratio;
}

void hu::get_chara_hp(CGameCharaBase* pChara, REAL32 & per)
{
	if(pChara == nullptr)	
	{
		per = 0.0f;
		return;
	}

	INT32 hp = 0, max_hp = 0;
	get_chara_hp(pChara, hp, max_hp);

	per = (max_hp > 0.f) ? ((hp * 100.f) / max_hp): 0.f;
	per = ceil(per);
}

i3::rc_wstring hu::get_m179_name()
{
	i3::rc_wstring name;
	detail::g_pM197->getWeaponInfo()->GetWeaponName(name);
	return name;
}

i3::rc_wstring hu::get_weapon_name(CGameCharaBase* pChara)
{
	if( pChara == nullptr) return i3::rc_wstring();
	if( pChara->getWeaponContext() == nullptr)	return i3::rc_wstring();

	WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
	if( pWeapon == nullptr )	return i3::rc_wstring();

	pWeapon = pWeapon->getUsedWeapon();
	if( pWeapon == nullptr )	return i3::rc_wstring();

	// Trigger 일경우 getUsedWeapon을 추가하려 했지만 Trigger가 작동 안하므로
	// HUD Util 내부 이름 가져오는 부분을 수정합니다.
	if(i3::same_of<WeaponBombTrigger*>(pWeapon) == true)
	{
		WeaponBombTrigger* trigger = static_cast<WeaponBombTrigger*>(pWeapon);
		// C5가 던져지지 않고 들고 있으면
		if(trigger->getDropedBombCount() == 0)
			pWeapon = trigger->getBomb();

		if( pWeapon == nullptr	) return i3::rc_wstring();
	}

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return i3::rc_wstring();

	// WeaponInfo를 가로챕니다. 무기 이름을 바꿔야 합니다.
	if (i3::same_of<WeaponBow*>(pWeapon) == true)
	{
		WeaponBow * bow = static_cast<WeaponBow*>(pWeapon);
		if (bow->isExtensionActivate() == true)
		{
			pInfo = g_pWeaponInfoDataBase->getWeaponInfo(bow->getExtensionArrowItemID());
			if (pInfo == nullptr)	return i3::rc_wstring();
		}
	}

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		pInfo = hu_ut::test_factor.pInfo;
#endif

	i3::rc_wstring wstr_name;
	pInfo->GetWeaponName(wstr_name);
	return wstr_name;
}


i3::rc_wstring hu::get_challenge_difficulty_text(INT32 diff)
{
	i3::rc_wstring wstrRes;

	switch( diff )
	{
	case 10:	case 9:	case 8:		wstrRes = GAME_RCSTRING("STR_TBL_GUI_AI_LEVEL_HARD");		break;
	case 7:	case 6:	case 5:	case 4:		wstrRes = GAME_RCSTRING("STR_TBL_GUI_AI_LEVEL_NORMAL");		break;
	case 3:	case 2:	case 1:		wstrRes = GAME_RCSTRING("STR_TBL_GUI_AI_LEVEL_EASY");		break;
	default:		break;
	}

	return wstrRes;
}

INT32 hu::get_challenge_difficulty_idx(INT32 diff)
{
	switch( diff )
	{
	case 10:	case 9:	case 8:		return 2; break;
	case 7:	case 6:	case 5:	case 4:	return 1; break;
	case 3:	case 2:	case 1:		return 0; break;
	}
	return -1;
}

INT32 hu::get_challenge_point()
{
	INT32 total_score = ScoreContext::i()->GetAccTrainingScore();
	//INT32 add_score = ScoreContext::i()->GetAddTrainingScore();	//추가되는 점수 혹시 몰라 구해 놓자.

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
	{
		total_score = hu_ut::test_factor.challenge_total_score;
	}
#endif

	return total_score;
}

void hu::get_kill_death_assist(INT32 slot_idx, INT32 & kill, INT32 & death, INT32 & assist)
{
	const SCORE_INFO * pInfo = ScoreContext::i()->getIndividualScore(slot_idx);
	kill = pInfo->m_KillCount;
	death = pInfo->m_DeathCount;
	assist = pInfo->m_AssistCount;
}

void hu::get_kill_death_assist(INT32 & kill, INT32 & death, INT32 & assist)
{
	INT32 slot_idx = BattleSlotContext::i()->getMySlotIdx();
	get_kill_death_assist(slot_idx, kill, death, assist);

#if ENABLE_UNIT_TEST
	if (UnitTest::is_act_hud_unit_test() == true)
	{
		kill = hu_ut::test_factor.kill;
		death = hu_ut::test_factor.death;
	}
#endif
}

void hu::get_KDA_to_string(i3::wstring & strKill, i3::wstring & strDeath, i3::wstring & strAssist)
{
	INT32 nKill = 0, nDeath = 0, nAssist = 0;
	hu::get_kill_death_assist(nKill, nDeath, nAssist);

	strKill		= i3::format_string(L"%d Kill", nKill);
	strAssist	= i3::format_string(L"%d Assist", nAssist);
	strDeath	= i3::format_string(L"%d Death", nDeath);
}

i3::rc_wstring hu::get_challenge_msg(HUD::ChallengeMessage idx)
{
	switch( idx )
	{
	case HUD::CHM_20_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT14");	break;
	case HUD::CHM_40_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT15");	break;
	case HUD::CHM_60_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT16");	break;
	case HUD::CHM_80_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT17");	break;

	case HUD::CHM_100_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT18");	break;
	case HUD::CHM_120_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT19");	break;
	case HUD::CHM_140_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT20");	break;
	case HUD::CHM_160_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT21");	break;
	case HUD::CHM_200_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT22");	break;

	case HUD::CHM_5_DEATH:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT3");	break;
	case HUD::CHM_20_DEATH:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT2");	break;

	case HUD::CHM_5_CHAIN_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT6");	break;
	case HUD::CHM_20_CHAIN_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT7");	break;

	case HUD::CHM_PIERCING_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT8");	break;
	case HUD::CHM_MASS_KILL:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT9");	break;

	case HUD::CHM_CHAIN_SLUGGER:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT13");	break;
	case HUD::CHM_CHAIN_STOPPER:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT1");	break;
	case HUD::CHM_BATTLE_START:	return GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_TEXT24");	break;
	}

	return i3::rc_wstring();
}

i3::rc_wstring hu::get_tutorial_msg(INT32 idx)
{
	if( MAX_TUTORIAL_MESSAGE <= idx ) return i3::rc_wstring();

	i3::string strTemp;
	i3::sprintf(strTemp, "STR_TBL_INGAME_HUD_TUTORIAL_TEXT%d", idx);
	return GAME_RCSTRING(strTemp);	
}

namespace detail
{
	struct ElapseChecker
	{
		void start() { start_time = timeGetTime(); }
		void pause() { paused_time = timeGetTime(); }
		void resume() { start_time += timeGetTime() - paused_time; }
		DWORD elapse()
		{
			if ( CGameMaster::i()->IsPaused()) 
			{
				return paused_time - start_time;
			}
			else
			{
				return timeGetTime() - start_time;
			}
		}
		DWORD start_time;
		DWORD paused_time;
	} elapse_checker;
	UINT32 running_time;
}

void hu::set_running_time(UINT32 time)
{
	detail::running_time = time;
	detail::elapse_checker.start();
}

void hu::set_time_pause(bool paused)
{
	if ( paused )
	{
		detail::elapse_checker.pause();
	}
	else
	{
		detail::elapse_checker.resume();
	}
}


void hu::get_running_time(INT32 & minute, INT32 & second)
{
	minute = second = 0;

	INT32 nRemainBattleTime = detail::running_time - (INT32)(detail::elapse_checker.elapse() / 1000);

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
	{
		nRemainBattleTime = hu_ut::test_factor.remain_battle_time;
	}
#endif

	if( nRemainBattleTime < 0)	return;

	minute = (INT32)(nRemainBattleTime/60);
	second = (INT32)(nRemainBattleTime%60);
}

void hu::get_team_score(INT32 & red, INT32 & blue)
{
	enum	{ RED, BLUE };
	
	red = blue = 0;
	switch( hu_ut::get_stage_type())
	{
	case STAGE_MODE_BOMB :
	case STAGE_MODE_ANNIHILATION :
	case STAGE_MODE_DESTROY :
	case STAGE_MODE_DEFENCE	:
	case STAGE_MODE_CONVOY :
	case STAGE_MODE_RUN_AWAY :

		red = ScoreContext::i()->GetRedTeamWinRound();
		blue = ScoreContext::i()->GetBlueTeamWinRound();
		break;

	case STAGE_MODE_ESCAPE :
		red = ScoreContext::i()->GetDinoEscapeScore(TEAM_RED);
		blue = ScoreContext::i()->GetDinoEscapeScore(TEAM_BLUE);
		break;
	case STAGE_MODE_CROSSCOUNT :
		red = ScoreContext::i()->GetTeamScore(TEAM_RED)->m_KillCount;
		blue = ScoreContext::i()->GetTeamScore(TEAM_BLUE)->m_KillCount;
		break;

	default :
		red = ScoreContext::i()->GetTeamScore(TEAM_RED)->m_KillCount;
		blue = ScoreContext::i()->GetTeamScore(TEAM_BLUE)->m_KillCount;
	}

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
	{
		red = hu_ut::test_factor.red_score;
		blue = hu_ut::test_factor.blue_score;
	}
#endif
}

bool hu::is_enable_mission()
{
	for(size_t i = 0; i<MAX_NOTIFY_MADAL_COUNT; ++i)
	{
		NOTIFY_MEDAL * pMedal = CMedalManager::i()->getNotifyMedal(i);
		UINT8 _type = MEDAL_SET_TYPE_NORMAL;

		if(pMedal->m_ui8MedalType == MEDAL_TYPE_SET)
			_type = (UINT8)CMedalManager::i()->getMedalSetTypeByIndex(pMedal->m_ui16Idx);

		if(_type == 0 && pMedal->m_ui16Idx == 0)
			continue;
		else
			return true;
	}

	return false;
}

QUEST_STATE hu::get_mission_info(INT32 mission_idx, i3::wstring & text)
{
	text.clear();
	NOTIFY_MEDAL * pMedal = CMedalManager::i()->getNotifyMedal(mission_idx);
	UINT8 _type = MEDAL_SET_TYPE_NORMAL;
	if(pMedal->m_ui8MedalType == MEDAL_TYPE_SET)
		_type = (UINT8)CMedalManager::i()->getMedalSetTypeByIndex(pMedal->m_ui16Idx);

	if (_type >= MEDAL_SET_TYPE_MAX)		// 커버리티에 더 이상 시달리기 싫기 때문에, 조건으로 거르는 것과 동시에 ASSERT처리한다...(2016.07.05)
	{										// 앞으로도 I3ASSERT를 제대로 걸러내지 못하는 커버리티 때문에 웬간하면 인덱스 안맞으면 어떻게든 어써트와 함께 리턴탈출을 하도록 수정하겠다. (2016.07.05).수빈.
		I3_BOUNDCHK(_type, MEDAL_SET_TYPE_MAX);
		text = L"";
		return QST_NONE;
	}
	
	MedalSetBase * pBase = nullptr;
	if(_type == 0 && pMedal->m_ui16Idx == 0)
	{
		text = L"";
		return QST_NONE;
	}

	pBase = CMedalManager::i()->getMedalSet(_type);
	MEDAL_INFO * pMedalInfo = pBase->getMedal(pMedal->m_ui16Idx);

	if(pMedalInfo == nullptr)
		return QST_NONE;

	UINT8 lv = 0;

	// 일반 메달만 4레벨까지 있다.
	if (_type == MEDAL_SET_TYPE_NORMAL)
		lv = pBase->getCurrentLevel(pMedalInfo->m_ui16Idx);

	i3::stack_wstring wstr_medalDescStr;
	i3::stack_wstring wstr_medalCntStr;	

	INT32 begin_cnt= pBase->getMedalProcessCnt(pMedalInfo->m_ui16Idx) - pBase->getLevelProcessCnt(pMedalInfo, lv);
	INT32 dest_cnt = 0;
	if(lv == MEDAL_LEVEL_MAX)
	{
		dest_cnt = pBase->getLevelProcessCnt(pMedalInfo, lv) - pBase->getLevelProcessCnt(pMedalInfo, lv - 1);
		if(pBase->getMedalProcessCnt(pMedalInfo->m_ui16Idx) == pBase->getLevelProcessCnt(pMedalInfo, lv))
			begin_cnt = dest_cnt;
	}
	else
	{
		dest_cnt = pBase->getLevelProcessCnt(pMedalInfo, lv + 1) - pBase->getLevelProcessCnt(pMedalInfo, lv);
	}
	if(begin_cnt >= dest_cnt)
		begin_cnt = dest_cnt;
	if(begin_cnt < 0)
		begin_cnt = 0;
	

	const char * desc = CMedalManager::i()->getNotifyMedalDesc(_type, pMedal->m_ui16Idx);
	i3::sprintf(wstr_medalDescStr, GAME_QUEST_RCSTRING(desc), dest_cnt);
	i3::sprintf(wstr_medalCntStr, L"%d/%d", begin_cnt, dest_cnt);

	i3::sprintf( text, L"%s %s\n", wstr_medalDescStr, wstr_medalCntStr );
	if(begin_cnt == 0)
		return QST_NOT_STARTED;
	else if(begin_cnt < dest_cnt)
		return QST_DOING;
	else
	return QST_COMPLETED;
}

QUEST_STATE hu::get_tutorial_info(INT32 mission_idx, i3::wstring & text)
{
	text.clear();

	i3::rc_wstring wstr_msg;
	switch(mission_idx)
	{
	case 0: wstr_msg =  GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_MOVE"); break;
	case 1: wstr_msg =  GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_SHOOT"); break;
	case 2: wstr_msg =  GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_SPECIAL"); break;
	case 3: wstr_msg =  GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_TIME_ATTACK"); break;
	}

	CTutorialMode* mode = gmode::i()->GetTutorialMode();
	i3::sprintf( text, L"%s\n{col:206,206,206,255}%d/%d{/col}", wstr_msg, 
		mode->GetMissionCompleteCount(mission_idx), 
		mode->GetMissionMaxCount(mission_idx) );

	ChallengeBulletType type = mode->IsMissionComplete(mission_idx);
	switch( type )
	{
	case CBT_CHALLENGING: return QST_DOING; break;
	case CBT_COMPLATE: return QST_COMPLETED; break;
	}
	
	return QST_NONE;
}


INT32 hu::get_round_goal_count(bool & is_round)
{
	is_round = false;

	if( hu_ut::is_stage_mask_training() == true ) return -1;
	if( hu_ut::is_stage_mask_madness() == true )  return -1;
	if( hu_ut::is_stage_mask_diehard() == true )  return -1;
	if( hu_ut::is_tutorial_mode() == true ) return 0;
	
	INT32 ret;
	UINT8 RoomSubType = hu_ut::get_my_room_sub_type();

	if(hu_ut::is_kill_mode() == true)
	{
		ret = LONIBBLE(RoomSubType);
		ret = MINMAX(BATTLE_KILL_TYPE_60, ret, BATTLE_KILL_TYPE_160);
		ret = g_TeamKillCountTable[ret];
	}
	else
	{
		if( hu_ut::is_cross_count_mode() == false )	//CC 모드는 킬 모드이다.
		{ 
			is_round = true;
			ret = LONIBBLE(RoomSubType);
			ret = MINMAX(BATTLE_ROUND_TYPE_1, ret, BATTLE_ROUND_TYPE_9);
			ret = g_MissionRoundCountTable[ret];
		}
		else
		{
			is_round = true;
			ret = ScoreContext::i()->getCurrentRound();
		}
		
	}

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		ret = hu_ut::test_factor.round_goal_count;
#endif


	return ret;
}

CHARACTER_TEAM_TYPE hu::getMyTeam()
{
	return  BattleSlotContext::i()->getMyTeam();
}

TEAM_TYPE hu::get_team_mark(CGameCharaBase* pPlayer, INT32 & red, INT32 & blue)
{
	red = blue = -1;
	if( hu_ut::is_tutorial_mode() == true ) return TEAM_NONE;

	TEAM_TYPE myTeamType = CHARA::CharaTeamType2TeamType( BattleSlotContext::i()->getMyTeam());

	bool bDinoMode = hu_ut::is_cross_count_mode() || hu_ut::is_dino_escape_mode();
	if( !bDinoMode) return myTeamType;

	bool dino = false;
	
	if(pPlayer != nullptr)
		dino = hu_ut::is_dino(pPlayer);

	switch( myTeamType )
	{
	case TEAM_RED:
		if( dino )
		{
			red = HUD::RED_DINO_MYTEAM;			blue = HUD::BLUE_HUMAN_OTHERTEAM;
		}
		else
		{
			red = HUD::RED_HUMAN_MYTEAM;			blue = HUD::BLUE_DINO_OTHERTEAM;
		}
		break;

	case TEAM_BLUE:
		if( dino )
		{
			red = HUD::RED_HUMAN_OTHERTEAM;			blue = HUD::BLUE_DINO_MYTEAM;
		}
		else
		{
			red = HUD::RED_DINO_OTHERTEAM;			blue = HUD::BLUE_HUMAN_MYTEAM;
		}
		break;
	}

	return myTeamType;
}

void hu::update_escape_block_time(REAL32 time)
{
	DinoModeContext::i()->rDelayTime -= time;	
}

INT32 hu::get_escape_block_time_shape_idx()
{
	enum { INDEX_PERIDO = 7, };
	TOUCHDOWN_BLOCK_STATE eState = DinoModeContext::i()->eEscapeBlock;

	switch( eState)
	{
	case TOUCHDOWN_BLOCK_STATE_INIT :
		{
			REAL32 elapsed = DinoModeContext::i()->rDelayTime * -1.f;

#if ENABLE_UNIT_TEST
			if( UnitTest::is_act_hud_unit_test() == true ) 
				elapsed = hu_ut::test_factor.escape_block_time;
#endif

			if( elapsed > 29.9f ) return 4;
			if( elapsed >= 28.0f ) return 3;

			return (INT32)elapsed / INDEX_PERIDO;
		}
		break;

	case TOUCHDOWN_BLOCK_STATE_FULL:
		DinoModeContext::i()->eEscapeBlock	= TOUCHDOWN_BLOCK_STATE_INIT;
		DinoModeContext::i()->rDelayTime		= 0.0f;
		break;
	}

	return 0;
}

bool hu::sceneToScreen(REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn)
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;

	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, i3Matrix::GetPos( g_pCamera->getCamMatrix()) , vIn );
	i3Vector::Normalize( &vSub, &vSub );

	REAL32 rDot = i3Vector::Dot( pvCamAt, &vSub);
	if( rDot <= 0.0f)	return false;

	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

	i3Vector::Copy( &vTempIn, vIn);
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

	*fOutX	= i3Vector::GetX( &vTemp );
	*fOutY	= i3Vector::GetY( &vTemp );

	*fOutX *= 0.5f;		*fOutX += 0.5f;
	*fOutY *= -0.5f;	*fOutY += 0.5f;

	return true;
}

bool hu::get_target_object_position(CGameCharaBase* pChara, VEC2D & pos, VEC2D & size)
{
	pos.x = pos.y = 0.f;
	size.x = size.y = 0.f;

	CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
	i3Object * pObj = pMissionMng->getRedObject();
	if( pObj == nullptr )
		return false;

	VEC3D * pCharaPos = pChara->GetPos();
	VEC3D * pObjPos = pObj->GetPos();

	INT32 centeridx = 0;

	if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
	{
		pObjPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
		if( (pChara->getAttachedObject() == pObj) )
			pCharaPos = pObjPos;
	}

	if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED)
	{
		pObj		= pMissionMng->getBlueObject();
		if( pObj == nullptr) return false;

		pObjPos		= pObj->GetPos();

		if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
		{
			pObjPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
			if( (pChara->getAttachedObject() == pObj) )
				pCharaPos = pObjPos;
		}
		centeridx	= 1;
	}


	REAL32 X, Y, length;
	VEC3D	vLength;
	i3Vector::Sub( &vLength, pCharaPos, pObjPos);

	length = i3Vector::Length( &vLength);

	if( length > 10.0f)
	{
		VEC3D vPos;

		if( sceneToScreen( &X, &Y, &vPos))
		{
			REAL32 rate = 1.1f - (length / 10.0f) * 0.1f;
			REAL32 tmp_size = MINMAX( 26.5f, 53.0f * rate, 53.0f);

			size.x = g_pViewer->GetViewWidth() * X - tmp_size * 0.5f;
			size.y = g_pViewer->GetViewHeight() * Y - tmp_size * 0.5f;

			pos.x = vPos.x;
			pos.y = vPos.y;

			return true;
		}
	}

	return false;
}


//INT32 : slot 인덱스, i3::rc_string : 닉네임
INT32 hu::get_ingame_user_list(i3::vector< i3::pair<INT32, i3::rc_wstring> > & list, bool add_me)
{
	list.clear();

	INT32 my_slot_idx = BattleSlotContext::i()->getMySlotIdx();
	INT32 odd_even = my_slot_idx % 2;
	INT32 slot_index = 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i);
		if (pChara == NULL)	
			continue;

		if (pChara->isAI()) continue;						//AI는 리스트에서 제외
		if( my_slot_idx == i && add_me == false ) continue; //나 포함 유무에 따라서..

		i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(i); //닉네암 없으면 포함 안 한다.
		if( wstrNick.empty() ) continue;

		const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(i);
		if( pInfo == 0 ) continue;
		if( pInfo->_State != SLOT_STATE_BATTLE ) continue; //battle 상태만 추가
		if( pInfo->_Rank == RANK_GAMEMASTER ) continue; //gm 은 제외
		if( pInfo->_Rank == RANK_MOD )	continue;		// MOD 도 제외

		i3::pair<INT32, i3::rc_wstring> param;
		param.first = slot_index;
		param.second = wstrNick;
		list.push_back(param);
		// 12788:리스트에 빈 곳이 없기 정렬해야해서 방식 수정합니다
		slot_index++;

	}

	return odd_even;
}

void hu::partition_ingame_user_list(const i3::vector< i3::pair<INT32, i3::rc_wstring> > & src,
								i3::vector< i3::pair<INT32, i3::rc_wstring> > & lhs,
								i3::vector< i3::pair<INT32, i3::rc_wstring> > & rhs)
{
	lhs.clear();
	rhs.clear();

	struct Fn
	{
		typedef i3::vector< i3::pair<INT32, i3::rc_wstring> > user_list;
		user_list* list[2];

		Fn(user_list* left, user_list* right) { list[0] = left; list[1] = right; }
		void operator()(const i3::pair<INT32, i3::rc_wstring> & user) const
		{
			list[ user.first % 2 ]->push_back( user );
		}
	};
	
	i3::for_each(src.begin(), src.end(), Fn(&lhs, &rhs) );
}

namespace inner
{
	bool enable_check_primary_weapon_first_shot = true;
}

void hu::enable_check_primary_weapon_first_shot()
{
	inner::enable_check_primary_weapon_first_shot = true;
}
void hu::diable_check_primary_weapon_first_shot()
{
	inner::enable_check_primary_weapon_first_shot = false;
}

bool hu::is_primary_weapon_first_shot(CGameCharaBase* pChara)
{
	if( inner::enable_check_primary_weapon_first_shot == false ) return false;

	enum { FIRST_SHOT_FACTOR = 1,};

	CGameCharaWeaponContext* pWeaponContext = pChara->getWeaponContext();
	if( pWeaponContext == nullptr ) return false;

	//WeaponBase * pWeapon = pWeaponContext->getWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* pWeapon = pWeaponContext->getCurrent();
	if( pWeapon == nullptr ) return false;

	CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
	if( pInfo == nullptr)	return false;
	if( pInfo->GetTypeUsage() != WEAPON_SLOT_PRIMARY) return false; // hansoft. 7208. 주무기의 초탄 킬 이외의 상황에 Oneshot OneKill 메시지가 출력됨.

#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true ) 
		pInfo = hu_ut::test_factor.pInfo;
#endif

	INT32 max_bullet = pInfo->GetLoadBullet(); //장전 가능 최대 총알
	INT32 max_havable_bullet = pWeapon->GetMaxBulletWithCashItem() - max_bullet; //보유 최대 장탄수

	INT32 bullet = 0, havable_bullet = 0, max_load_bullet = 0;
	detail::get_range_bullet_dualcnt(pWeapon, pInfo, bullet, havable_bullet, max_load_bullet);

	if(i3::kind_of<WeaponShotGun3*>(pWeapon))
	{
		WeaponShotGun3 * w = static_cast<WeaponShotGun3*>(pWeapon);
		if(w->getFireOrder() == WEAPON::SECONDARY_FIRE)
		{
			return false;
		}
	}
	else if(i3::kind_of<WeaponShotGunWithButt*>(pWeapon))
	{
		WeaponShotGunWithButt * w = static_cast<WeaponShotGunWithButt*>(pWeapon);
		if(w->getFireOrder() == WEAPON::SECONDARY_FIRE)
		{
			return false;
		}
	}
	else if (i3::kind_of<WeaponDualMagazine*>(pWeapon))	//듀얼탄창
	{
		max_bullet *= 2; //두탄창임
	}

	return( (max_havable_bullet == havable_bullet) && //최대 보유 장타수가 같고,
		(max_bullet - bullet) == FIRST_SHOT_FACTOR); //장전 가능 최대 총알 - 현재 장전된 총알 수
}

#include "GameCharaActionContext.h"
CHARA_ACTION_BODYLOWER hu::get_chara_action_state(CGameCharaBase* pChara)
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.chara_action_body_lower;
#endif

	if(pChara == nullptr) 
		return CHARA_BODYLOWER_STAND;
	else
		return pChara->getActionContext()->getBodyLower();
}

#include "GameCharaEquipContext.h"
HUD::ItemState hu::get_helmet_protect_state(CGameCharaBase* pChara)
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.headgear_parts_state;
#endif

	if( pChara == nullptr) return HUD::IS_NONE;
	if( hu_ut::is_dino(pChara) == true ) return HUD::IS_NONE; //dino는 없다.
	if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == true ) return HUD::IS_NONE; //죽은 상태면 없다.
	if( pChara->getEquipContext() == nullptr ) return HUD::IS_NONE; //아직 생성이 안 된 상태
	if( pChara->isAttachedProtectParts() == true )	return HUD::IS_HAVE;

	return HUD::IS_NOT_HAVE;
}

#include "BattleHud.h"
HUD::ItemState hu::get_c5_state(CGameCharaBase* pChara)
{
#if ENABLE_UNIT_TEST
	if( UnitTest::is_act_hud_unit_test() == true )
		return hu_ut::test_factor.c5_state;
#endif


	if( pChara == nullptr) return HUD::IS_NONE;
	if( hu_ut::is_dino(pChara) == true ) return HUD::IS_NONE; //dino는 없다.
	if( CHud::i() == nullptr || CHud::i()->getHudMode() == HUD_MODE_MISSION_OBSERVER ) return HUD::IS_NONE; //옵져버 상태면 없다. 
	if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == true ) return HUD::IS_NONE; //죽은 상태면 없다.
	
	CGameCharaWeaponContext * pCtx = pChara->getWeaponContext();
	if( pCtx == nullptr)	return HUD::IS_NONE;

	if( i3::same_of<WeaponBombTrigger*>( pCtx->getWeapon( WEAPON_SLOT_THROWING1) ) == true )
	{	
		WeaponBombTrigger * pBombTrigger = (WeaponBombTrigger*)pCtx->getWeapon( WEAPON_SLOT_THROWING1);
		if( i3::same_of<WeaponC5*>(pBombTrigger->getBomb()) == true )
		{
			for( INT32 i = 0; i < pBombTrigger->getDropedBombCount(); i++)
			{
				WeaponC5 * pC5 = (WeaponC5*) pBombTrigger->getDropedBomb(i);

				if( pC5->isEnable() &&  static_cast<MainWeapon_C5*>(pC5->GetMainWeapon())->isBombState( WEAPON_BOMBSTATE_SETTLED) )
				{	
					return HUD::IS_USE; //사용중..
				}
			}

			if( pCtx->getCurrentSlotNum() == WEAPON_SLOT_THROWING1)
			{	
				return HUD::IS_NOT_USE; //해당 슬롯일 경우만 보여준다.
			}
		}
	}

	return HUD::IS_NONE;
}

i3::rc_wstring hu::get_text_using_respawn_item()
{
	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_100) == true )
		return i3::rc_wstring(); // 100%의 경우 바로 리스폰 되기 때문에 텍스트를 표시 하지 않습니다.

	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_50) == true )
		return GAME_RCSTRING("STR_TBL_BATTLEGUI_RESPAWN50_ITEM");  // 50% 빠르게 리스폰 됩니다.

	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN) == true )
		return GAME_RCSTRING("STR_TBL_BATTLEGUI_RESPAWN30_ITEM"); // 30% 빠르게 리스폰 됩니다.
	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_30_EV) == true )
		return GAME_RCSTRING("STR_TBL_BATTLEGUI_RESPAWN30_ITEM"); // 30% 빠르게 리스폰 됩니다.

	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_20) == true )
		return GAME_RCSTRING("STR_TBL_BATTLEGUI_RESPAWN20_ITEM"); // 20% 빠르게 리스폰 됩니다.
	if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_20_EV) == true )
		return GAME_RCSTRING("STR_TBL_BATTLEGUI_RESPAWN20_ITEM"); // 20% 빠르게 리스폰 됩니다.

	return i3::rc_wstring();
}

//킬, 데스, point, exp, nick name 정렬
void hu::sort_ingame_result_inform(i3::vector< ResultState::UserInform > & list)
{
	struct fn
	{
		// Ingame 결과창 유저 정렬 조건
		// 1. Kill 많은 순
		// 2. Assist 많은 순
		// 3. Death 적은 순
		// 4. SlotIndex 순
		bool operator()(const ResultState::UserInform& lhs, const ResultState::UserInform& rhs) const
		{
			if (lhs.kill_count == rhs.kill_count)			// 킬 카운트
			{
				if (lhs.assist_count == rhs.assist_count)	// 어시스트 카운트
				{
					if (lhs.death_count == rhs.death_count) // 데스 카운트
					{
						return lhs.idx < rhs.idx;			// 슬롯 인덱스
					}
					return lhs.death_count < rhs.death_count;
				}
				return lhs.assist_count > rhs.assist_count;
			}
			return lhs.kill_count > rhs.kill_count;
		}
	};

	std::sort(list.begin(), list.end(), fn());
}

void hu::get_ingame_result_inform(ResultState::UserInformList & red_list, ResultState::UserInformList & blue_list)
{
	red_list.clear(); blue_list.clear();

	ResultState::UserInform inform;

	INT32 my_chara_slot = BattleSlotContext::i()->getMySlotIdx();

	bool training_mode = hu_ut::is_stage_mask_training();
	bool mission_mode = hu_ut::is_bomb_mission_mode() || hu_ut::is_annihilation_mode() || hu_ut::is_dino_escape_mode();
	
	for(INT32 i=0; i< SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i);
		if (pChara == nullptr ) continue;

		const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);
		if (pSlotInfo == nullptr ) continue;

		bool bIsNPC = false;
		if(training_mode == true)
		{
			if( (BattleSlotContext::i()->getMainSlotIdx() % 2) != (i % 2) )
				bIsNPC = true;
		}

		if( UserContext::i()->IsPlayingSlot(i) || BattleResultContext::i()->getBattleEndUserFlag() & (0x01 <<i) || bIsNPC) //실제 코드
		{
			const SCORE_INFO* pScoreInfo = ScoreContext::i()->getIndividualScore(i);

			inform.idx = i;

			inform.wstr_nick = BattleSlotContext::i()->getNickForSlot(i);
			inform.nick_color = BattleSlotContext::i()->getNickColorForSlot(i);

			inform.clan_idx = pSlotInfo->_clanidx;
			inform.clan_mark = pSlotInfo->_clanMark;

			inform.rank = BattleSlotContext::i()->getSlotInfo(i)->_Rank;

			if( UserInfoContext::i()->IsContentsEndabled(CONTENTS_CLAN_MATCH_NEW)  && 
				g_pFramework->GetClanContext()->InBattleTeam())
			{
				if(ClanGameContext::i()->GetMercRank(i) != -1)
					inform.rank = ClanGameContext::i()->GetMercRank(i);
			}

			inform.kill_count = pScoreInfo->m_KillCount;
			inform.death_count = pScoreInfo->m_DeathCount;
			inform.assist_count = pScoreInfo->m_AssistCount;

			inform.point = BattleResultContext::i()->GetInfo().m_BattlePoint[i];
			inform.exp = BattleResultContext::i()->GetInfo().m_BattleExp[i];

			inform.total_bonus_point = BattleResultContext::i()->GetInfo().getTotalBonusPoint(i);
			inform.total_bonus_exp = BattleResultContext::i()->GetInfo().getTotalBonusExp(i);

			inform.score = BattleResultContext::i()->GetInfo().m_BattleScore[i];				
			inform.result_mark = BattleResultContext::i()->GetInfo().m_pBattleResultIcon[i];

			if( BattleResultContext::i()->GetInfo().m_ui16QuestEndFlag & (0x01 << i))
				inform.QuestEndFlag = true;
			else
				inform.QuestEndFlag = false;

			inform.e_sport_mark = pSlotInfo->_eSportID;

			inform.mission = (mission_mode == true) ? BattleResultContext::i()->GetInfo().m_AccMissionCount[i] : 0;
			inform.is_my_inform = (my_chara_slot == i);

			CHARACTER_TEAM_TYPE teamType = g_pCharaManager->getCharaInfo( i)->GetTeam();
			if( teamType == CHARACTER_TEAM_RED)
				red_list.push_back(inform);
			else if( teamType == CHARACTER_TEAM_BLUE)
				blue_list.push_back(inform);
		}
	}
}

INT32 hu::get_benefit_mark(INT32 benefit_mark, INT32 benefit_mask)
{
	INT32 mask_result = (benefit_mark & benefit_mask);
	switch( mask_result )
	{
	case RESULT_ICON_PC			: return UIEM_PCCAFE;
	case RESULT_ICON_PC_PLUS	: return UIEM_PCCAFE_PLUS;
	case RESULT_ICON_ITEM		: return UIEM_ITEM;
	case RESULT_ICON_EVENT		: return UIEM_EVENT;
	case RESULT_ICON_PC_ID		: return UIEM_PCCAFE;
	case RESULT_ICON_GWARNET	: return UIEM_PCCAFE;
	case RESULT_ICON_PC_HOUSE	: return UIEM_PCCAFE;
	case RESULT_ICON_GARENA_GCA	: return UIEM_GARENA_GCA;
	default						: break;
	}

	return UIEM_NONE;
}

bool hu::is_my_team_win()
{
	INT32 red_count = 0, blue_count = 0;
	hu::get_team_score(red_count, blue_count);

	TEAM_TYPE myTeam = CHARA::CharaTeamType2TeamType(BattleSlotContext::i()->getMyTeam());
	if( myTeam == TEAM_RED)
	{
		return red_count > blue_count;
	}
	else
		return red_count < blue_count;
}

bool hu::is_zero_durability_weapon( T_ItemID ItemID,T_ItemDBIdx ware_idx)
{
	REAL32 durable_rate = 0.f;
	bool is_durability_weapon = CShopContext::i()->GetDurability(ItemID, ware_idx, durable_rate);
	if( is_durability_weapon == false ) return false; //내구도 무기다 아니다.
	if(durable_rate == 0.f) return true; //내구도가 0 이다.
	return false;
}
