#include "pch.h"
#include "DomiSkillObjBulletinBoard.h"
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "UI/HUD/UIHUD_Domination_Manager.h"
#include "DominationSkillSystem.h"
#include "../../UI/UIBattleFrame.h"
#include "DomiUserSkillMgr.h"
#include "BattleSlotContext.h"
#include "DomiSkillProperties.h"

namespace SkillObjBulletinBoardUtil
{
	bool NeedToSupplyAmmo(WeaponBase* wp, CGameCharaBase* customer)
	{
		if (wp == nullptr) return false;

		INT32 lv = DomiUserSkillMgr::i()->GetLevel(customer->getCharaNetIndex(), USER_SKILL_MAGAZINE_UP);
		REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);
		INT32 max = static_cast<INT32>(wp->GetMaxBulletWithCashItem() * ratio); // 최대 보유 가능 탄약
		INT32 loaded = wp->GetLoadBulletWithCashItem(); // 1탄창 최대 총알 개수
		INT32 reserve = wp->getTotalBulletCount(); // 예비 탄약

		if (max > loaded+reserve) return true;
		return false;
	}
}

using namespace dss;
using namespace SkillObjBulletinBoardUtil;

DomiSkillObjBulletinBoard::DomiSkillObjBulletinBoard()
{
	m_funcPtrs[NONE]				= &DomiSkillObjBulletinBoard::None;
	m_funcPtrs[INSTALLABLE]			= &DomiSkillObjBulletinBoard::Setup;
	m_funcPtrs[UPGRADABLE]			= &DomiSkillObjBulletinBoard::Upgrade;
	m_funcPtrs[REPAIRABLE]			= &DomiSkillObjBulletinBoard::Fix;
	m_funcPtrs[HP_RECOVERABLE]		= &DomiSkillObjBulletinBoard::UseSupplyBaseHP;
	m_funcPtrs[AMMO_RECOVERABLE]	= &DomiSkillObjBulletinBoard::UseSupplyBaseAMMO;
	m_funcPtrs[SHOP_OPEN]			= &DomiSkillObjBulletinBoard::OpenSkillShop;
	
	m_noticeHUD = static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL));
}

DomiSkillObjBulletinBoard::~DomiSkillObjBulletinBoard() 
{ 
	m_noticeHUD = 0;
}

void DomiSkillObjBulletinBoard::Show(CGameCharaBase* customer, CServiceable* service) 
{ 
	if (customer && service)
	{
		fn& mf = m_funcPtrs[service->GetState()];
		(this->*mf)(customer, service);
	}
}

void DomiSkillObjBulletinBoard::Hide() { m_noticeHUD->HideNotice(); }

void DomiSkillObjBulletinBoard::None(CGameCharaBase* customer, CServiceable* service) {}

void DomiSkillObjBulletinBoard::Setup(CGameCharaBase* customer, CServiceable* service) // setup이 가능한 상황, E 키와 요구 SP를 출력
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") != 0) return;

	INT16 cost = 0;
	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY:
		{
			cost = domi::SkillProp::i()->GetDummy(1).install_sp->GetVal();
		}
		break;

	case DOMI_TYPE_SENTRYGUN: 
		{
			cost = domi::SkillProp::i()->GetSentrygun(1).install_sp->GetVal();	
		}
		break;

	default: return;
	}

	UINT16 sp = domi_state_info::i()->sp.sp[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_OBJECT_4"), cost); // 최초 설치는 무조건 레벨1임.
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_9"), cost);
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void DomiSkillObjBulletinBoard::Upgrade(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	INT16 cost = 0;
	INT32 installerIdx = -1;

	INT32 nextLv = service->GetWeapon()->getWeaponInfo()->GetLevel()+1;

	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY: 
		{
			cost = domi::SkillProp::i()->GetDummy(nextLv).install_sp->GetVal();
			CGameCharaBase* installer = static_cast<WeaponDinoDummy*>(service->GetWeapon())->GetInstaller();
			if (installer)
				installerIdx = installer->getCharaNetIndex();
		}
		break;
	case DOMI_TYPE_SENTRYGUN:  
		{
			cost = domi::SkillProp::i()->GetSentrygun(nextLv).install_sp->GetVal();

			CGameCharaBase* installer = static_cast<WeaponSentryGun*>(service->GetWeapon())->GetInstaller();
			if (installer)
				installerIdx = installer->getCharaNetIndex();
		}
		break;
	default: return;
	}

	// 설치자(소유자)만 업그레이드 가능.
	if (installerIdx == customer->getCharaNetIndex())
	{
		UINT16 sp = domi_state_info::i()->sp.sp[customer->getCharaNetIndex()/2];
		if (sp >= cost)
		{
			m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_OBJECT_8"), cost);
			service->EnableCharaIdx(customer->getCharaNetIndex());
		}
		else
		{
			m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_9"), cost);
			service->DisableCharaIdx(customer->getCharaNetIndex());
		}
	}
	else
	{
		m_objectName.clear();
		i3::rc_wstring nick = BattleSlotContext::i()->getNickForSlot(installerIdx);
		m_objectName.append(nick.begin(), nick.end());
		m_objectName += L"'s ";

		if (service->GetType() == DOMI_TYPE_SENTRYGUN)
			m_objectName += L"SentryGun";
		else if (service->GetType() == DOMI_TYPE_DUMMY)
			m_objectName += L"Dummy";

		m_noticeHUD->WarningNoUse(m_objectName, L"You don't have permission to upgrade/repair");
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void DomiSkillObjBulletinBoard::Fix(CGameCharaBase* customer, CServiceable* service) 
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	INT16 cost = 0;
	INT32 installerIdx = -1;

	INT32 level = service->GetWeapon()->getWeaponInfo()->GetLevel();
	switch (service->GetType())
	{
	case DOMI_TYPE_DUMMY: 
		{
			cost = domi::SkillProp::i()->GetDummy(level).repair_sp->GetVal();

			CGameCharaBase* installer = static_cast<WeaponDinoDummy*>(service->GetWeapon())->GetInstaller();
			if (installer)
				installerIdx = installer->getCharaNetIndex();
		}
		break;
	case DOMI_TYPE_SENTRYGUN: 
		{
			cost = domi::SkillProp::i()->GetSentrygun(level).repair_sp->GetVal();

			CGameCharaBase* installer = static_cast<WeaponSentryGun*>(service->GetWeapon())->GetInstaller();
			if (installer)
				installerIdx = installer->getCharaNetIndex();
		}
		break;
	default: return;
	}

	// 설치자(소유자)만 수리 가능.
	if (installerIdx == customer->getCharaNetIndex())
	{
		UINT16 sp = domi_state_info::i()->sp.sp[customer->getCharaNetIndex()/2];
		if (sp >= cost)
		{
			m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_OBJECT_5"), cost);
			service->EnableCharaIdx(customer->getCharaNetIndex());
		}
		else
		{
			m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_9"), cost);
			service->DisableCharaIdx(customer->getCharaNetIndex());
		}
	}
	else
	{
		m_objectName.clear();
		i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(installerIdx);
		m_objectName.append(wstrNick.begin(), wstrNick.end());
		m_objectName += L"'s ";

		if (service->GetType() == DOMI_TYPE_SENTRYGUN)
			m_objectName += L"SentryGun";
		else if (service->GetType() == DOMI_TYPE_DUMMY)
			m_objectName += L"Dummy";

		m_noticeHUD->WarningNoUse(m_objectName, L"You don't have permission to upgrade/repair");

		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void DomiSkillObjBulletinBoard::UseSupplyBaseHP(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	if (customer->getCurHP() == customer->getFullHP())
	{
		m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_6"));
		service->DisableCharaIdx(customer->getCharaNetIndex());
		return;
	}

	INT16 cost = domi::SkillProp::i()->GetSupplycost().hpsupply_sp->GetVal();
	UINT16 sp = domi_state_info::i()->sp.sp[customer->getCharaNetIndex()/2];
	if (sp >= cost)
	{
		m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_ADD_LIST_1"), cost);
		service->EnableCharaIdx(customer->getCharaNetIndex());
	}
	else
	{
		m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_9"), cost);
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void DomiSkillObjBulletinBoard::UseSupplyBaseAMMO(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return;

	WeaponBase* primary = customer->GetCharaWeapon(WEAPON_SLOT_PRIMARY);
	WeaponBase* secondary = customer->GetCharaWeapon(WEAPON_SLOT_SECONDARY);

	if (primary == nullptr && secondary == nullptr)
	{
		m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_ADD_LIST_3"));
		service->DisableCharaIdx(customer->getCharaNetIndex());
		return;
	}

	if (NeedToSupplyAmmo(primary, customer) || NeedToSupplyAmmo(secondary, customer))
	{
		INT16 cost = domi::SkillProp::i()->GetSupplycost().ammosupply_sp->GetVal();
		UINT16 sp = domi_state_info::i()->sp.sp[customer->getCharaNetIndex()/2];
		if (sp >= cost)
		{
			m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_ADD_LIST_2"), cost);
			service->EnableCharaIdx(customer->getCharaNetIndex());
		}
		else
		{
			m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_9"), cost);
			service->DisableCharaIdx(customer->getCharaNetIndex());
		}
	}
	else
	{
		m_noticeHUD->WarningNoUse(GAME_RCSTRING("STR_DOMI_OBJECT_7"));
		service->DisableCharaIdx(customer->getCharaNetIndex());
	}
}

void DomiSkillObjBulletinBoard::OpenSkillShop(CGameCharaBase* customer, CServiceable* service)
{
	if (::strcmp(service->GetName(), "RAW_OBJECT") == 0) return; // 이럴일은 없겠지만...
	if (UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP)) 
	{
		Hide(); return;
	}

	m_noticeHUD->AllowTheUse(GAME_RCSTRING("STR_DOMI_OBJECT_11"));
	service->EnableCharaIdx(customer->getCharaNetIndex());
}