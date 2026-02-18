#include "pch.h"
#include "DomiUserSkillMgr.h"
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaWeaponContext.h"
#include "../../UI/UIBattlePopupSkillShop.h"
#include "../GameDropItemManager.h"
#include "../../UI/HUD/UIHUD_Domination_Manager.h"
#include "BattleSlotContext.h"
#include "DomiSkillProperties.h"
#include "../../Weapon/WeaponItemID.h"

DomiUserSkillMgr::DomiUserSkillMgr()
{
	// 구매 후 바로 적용하는 것들(예외 없이 모든 스킬 추가되어야 함)
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_HP_UP, &DomiUserSkillMgr::HpUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_DAMAGE_UP, &DomiUserSkillMgr::DamageUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEED_UP, &DomiUserSkillMgr::SpeedUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAGAZINE_UP, &DomiUserSkillMgr::MagazineUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_WEAPON_EFFECT_RATIO_UP, &DomiUserSkillMgr::WeaponSpecialEffectUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_RELOAD_SPEED_FAST, &DomiUserSkillMgr::MagazineReloadSpeedUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GRENADE_DAMAGE_UP, &DomiUserSkillMgr::GrenadeDamageUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_AGGRO_DOWN, &DomiUserSkillMgr::AggroDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GET_SP_UP, &DomiUserSkillMgr::SpGainUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAX_SP_UP, &DomiUserSkillMgr::SpGaugeUp));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEND_SP_DOWN, &DomiUserSkillMgr::SpCostDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_GET_DAMAGE_DOWN, &DomiUserSkillMgr::HitDamageByDinoDown));
	m_mapApplyDirect.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MEATBOMB, &DomiUserSkillMgr::EquipMeatBomb));

	// 리스폰 할 때마다 적용(데이터가 리셋되어 다시 적용해야할 필요가 있는 것들)
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_HP_UP, &DomiUserSkillMgr::HpUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_SPEED_UP, &DomiUserSkillMgr::SpeedUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAGAZINE_UP, &DomiUserSkillMgr::MagazineUp));
	m_mapApplyAtRespawn.insert(i3::unordered_map<INT32, fn>::value_type(USER_SKILL_MAX_SP_UP, &DomiUserSkillMgr::SpGaugeUp));
}

DomiUserSkillMgr::~DomiUserSkillMgr() 
{
	m_mapApplyDirect.clear();
	m_mapApplyAtRespawn.clear();
}

void DomiUserSkillMgr::ApplyDirect(INT32 customerIdx, USER_SKILL type, INT32 lv)
{
	i3::unordered_map<INT32, fn>::iterator it = m_mapApplyDirect.find(type);
	if (it != m_mapApplyDirect.end())
	{
		fn& mf = it->second;
		m_lvRecoder.SetLevel(customerIdx, static_cast<USER_SKILL>(type), lv);
		(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), lv);
	}
}

void DomiUserSkillMgr::ApplyAtRespawn(INT32 customerIdx)
{
	for (INT32 i=0; i<USER_SKILL_COUNT; i++)
	{
		USER_SKILL skill = static_cast<USER_SKILL>(i);

		INT32 currentLevel = m_lvRecoder.GetLevel(customerIdx, skill);
		if (currentLevel <= 0) continue;

		i3::unordered_map<INT32, fn>::iterator it = m_mapApplyAtRespawn.find(skill);
		if (it != m_mapApplyAtRespawn.end())
		{
			fn& mf = it->second;
			(this->*mf)(g_pCharaManager->getCharaByNetIdx(customerIdx), currentLevel);
		}
	}
}

void DomiUserSkillMgr::SetLevel(INT32 customerIdx, USER_SKILL type, INT32 lv)
{
	m_lvRecoder.SetLevel(customerIdx, type, lv);
}

INT32 DomiUserSkillMgr::GetLevel(INT32 customerIdx, USER_SKILL type) const 
{ 
	return m_lvRecoder.GetLevel(customerIdx, type); 
}

// 아래 적용 스킬의 자세한 내용은 아래 문서에서 확인
// http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId=3083
// 내용 없는 함수는 서버에서 처리
void DomiUserSkillMgr::HpUp(CGameCharaBase* customer, INT32 lv)
{
	customer->setCharaInfo()->SetHP(static_cast<REAL32>(customer->getFullHP()));
	BattleSlotContext::i()->setGameInfo_Chara( customer->getCharaNetIndex())->_tNetData._tHP.setHP( static_cast<UINT16>(customer->getFullHP()) );
}

void DomiUserSkillMgr::DamageUp(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::SpeedUp(CGameCharaBase* customer, INT32 lv)
{
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_SPEED_UP).GetIncrRatio(lv);

	customer->getMoveContext()->EnableSpeedReduction(addupStat, -1.0f);
}

void DomiUserSkillMgr::MagazineUp(CGameCharaBase* customer, INT32 lv)
{
	WeaponBase* primary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* secondary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_SECONDARY);
	
	WeaponBase* wp[2] = {primary, secondary};
	REAL32 addupStat = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);

	for (INT32 i=0; i<2; i++)
	{
		// 리스폰시, 무기 업그레이드 적용이 안될 가능성이 있습니다.
		// 추후 수정해야합니다 .. 2013.01.19 양승천
		if(wp[i] == nullptr) continue;

		wp[i]->setLoadedBulletCount(wp[i]->getWeaponInfo()->GetLoadBullet());
		INT32 mazineCapacity = wp[i]->GetLoadBulletWithCashItem(); // 1 개 탄창 최대 탄수
		INT32 maxBullet = static_cast<INT32>(wp[i]->GetMaxBulletWithCashItem() * addupStat); // 스킬 적용된 최대 탄수
		wp[i]->setTotalBulletCount(maxBullet - mazineCapacity);
	}

	// 총기 교체 리액션 및 이펙트
	CGameCharaWeaponContext* weaponCtx = customer->getWeaponContext();
	WEAPON_SLOT_TYPE curWeaponSlot = weaponCtx->getCurrentSlotNum();
	if (WEAPON_SLOT_PRIMARY == curWeaponSlot)
	{	
		WEAPON_SLOT_TYPE trickSlot = static_cast<WEAPON_SLOT_TYPE>(curWeaponSlot+1);
		weaponCtx->setCurrentSlotNum( trickSlot );
		weaponCtx->setSwapSlotNum( trickSlot );
	}
	customer->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY, false);

	CGameDropItem::effect(customer, DINOKILLITEM_AMMO);
}

void DomiUserSkillMgr::WeaponSpecialEffectUp(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::MagazineReloadSpeedUp(CGameCharaBase* customer, INT32 lv) 
{
	// 아래 함수에서 자동 적용
	// REAL32 WeaponBase::GetReloadTime(), REAL32 WeaponBase::GetLoadBulletTime()
}
void DomiUserSkillMgr::GrenadeDamageUp(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::AggroDown(CGameCharaBase* customer, INT32 lv) {}
void DomiUserSkillMgr::SpGainUp(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::SpGaugeUp(CGameCharaBase* customer, INT32 lv)
{
	REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAX_SP_UP).GetIncrRatio(lv);
	
	CUIHudDomi_Skill* hud = static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL));
	hud->SetSPMax(static_cast<INT16>(domi::SkillProp::i()->GetSP(domi_state_info::i()->round).max->GetVal() * ratio));
}

void DomiUserSkillMgr::SpCostDown(CGameCharaBase* customer, INT32 lv) 
{
	REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_SPEND_SP_DOWN).GetIncrRatio(lv);

	// 서플라이 HP / AMMO
	domi::SkillProp::i()->GetSupplycost().hpsupply_sp->SetFactor(ratio);
	domi::SkillProp::i()->GetSupplycost().ammosupply_sp->SetFactor(ratio);

	// 센트리건
	for (INT32 i=1; i<=MAX_LEVEL_SENTRYGUN; i++)
	{
		domi::SkillProp::i()->GetSentrygun(i).install_sp->SetFactor(ratio);
		domi::SkillProp::i()->GetSentrygun(i).repair_sp->SetFactor(ratio);
	}

	// 더미
	for (INT32 i=1; i<=MAX_LEVEL_DUMMY; i++)
	{
		domi::SkillProp::i()->GetDummy(i).install_sp->SetFactor(ratio);
		domi::SkillProp::i()->GetDummy(i).repair_sp->SetFactor(ratio);
	}

	// 유저스킬
	for (INT32 type=0; type<USER_SKILL_COUNT; type++)
	{
		INT8 maxLv = domi::SkillProp::i()->GetUserSkill(type).level_max->GetVal();
		for (INT32 i=1; i<=maxLv; i++)
			domi::SkillProp::i()->GetUserSkill(type).GetCostSp(i).SetFactor(ratio);
	}
}

void DomiUserSkillMgr::HitDamageByDinoDown(CGameCharaBase* customer, INT32 lv) {}

void DomiUserSkillMgr::EquipMeatBomb(CGameCharaBase* customer, INT32 lv) 
{
	// 구매 시점에서 바로 생성/적용.
	// 구매 후 미사용 상태에서 리스폰 시 SetCharaWeaponSet()에서 자동적용하므로 
	// m_mapApplyAtRespawn에 등록하면 안됨.
	T_ItemID ItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, WEAPON_CLASS_THROWING_GRENADE, WEAPON_CLASS_THROWING_GRENADE, WEAPON::getItemIndex(WEAPON::DOMI_MEATBOMB));
	customer->Cmd_CreateWeapon(WEAPON_SLOT_MISSION, ItemID);
	customer->Cmd_ChangeWeapon(WEAPON_SLOT_MISSION, false);
}



UserSkillLvRecoder::UserSkillLvRecoder() {}
UserSkillLvRecoder::~UserSkillLvRecoder() {}

void UserSkillLvRecoder::CurrentLevels::SetValue(USER_SKILL type, INT32 lv)
{
	I3ASSERT(0 <= type && type < USER_SKILL_COUNT);
	I3ASSERT(0 <= lv && lv < MAX_LEVEL_USERSKILL+1);

	levels[type] = lv;
};

INT32 UserSkillLvRecoder::CurrentLevels::GetValue(USER_SKILL type) const
{
	I3ASSERT(0 <= type && type < USER_SKILL_COUNT);

	return levels[type];
};

void  UserSkillLvRecoder::SetLevel(INT32 userIdx, USER_SKILL type, INT32 lv)
{
	INT32 idx = userIdx / 2; // (0, 2, 4, 6) -> (0, 1, 2, 3)
	I3ASSERT(0 <= idx && idx < domiUserMaxCnt);
	
	m_val[idx].SetValue(type, lv);
}

INT32 UserSkillLvRecoder::GetLevel(INT32 userIdx, USER_SKILL type) const
{
	INT32 idx = userIdx / 2; // (0, 2, 4, 6) -> (0, 1, 2, 3)
	I3ASSERT(0 <= idx && idx <= domiUserMaxCnt);

	return m_val[idx].GetValue(type);
}