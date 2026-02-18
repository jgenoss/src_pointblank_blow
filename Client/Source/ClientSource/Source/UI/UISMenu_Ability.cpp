#include "pch.h"

#include "UISMenu_Ability.h"

#include "UISideMenu.h"

#include "UIUtil.h"
#include "UIMath.h"

#include "UIMainFrame.h"
#include "UISideMenu.h"
#include "UICharaInfoMgr.h"
#include "UserInfoContext.h"
#include "../BattleSlotContext.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/ftoa.h"

#if 0
#define CASHITEM_TRACE	I3TRACE
#else
#define CASHITEM_TRACE	__noop
#endif

namespace
{
	SMenuAbility* g_this = 0;
	MultiSceneInfo g_scn_info;
	i3UIControl *g_ctrl =0;

	i3::wstring to_ability_string(REAL32 factor, REAL32 extra_factor = 0.f)
	{
		i3::wstring ret;
		i3::ftoa(factor + extra_factor, ret, 1);
		return i3::move(ret);
	}

	i3::wstring to_ability_string(INT32 factor, REAL32 extra_factor = 0.f)
	{
		i3::wstring ret;
		i3::ftoa( (REAL32)factor + extra_factor, ret, 0);
		return i3::move(ret); 
	}

	i3::wstring to_ability_string(INT32 factor1, INT32 factor2, REAL32 extra_factor = 0.f)
	{
		i3::wstring ret;	
		i3::itoa(factor1, ret);
		ret.append(L"/");
		ret.append( to_ability_string( factor2, extra_factor ) );
		return i3::move(ret);
	}

	//INT32 WeaponBase::GetMaxBulletWithCashItem(void)에서 가져옴. 흐미...
	INT32 GetMaxBulletWithCashItem(CGameCharaBase* chara, CWeaponInfo* pInfo, const REAL32* abilities)
	{
		I3ASSERT( pInfo != nullptr);

		INT32 count = pInfo->GetMaxBullet();

		// 보유탄수 증가권이 있는지를 확인하여 보유탄수 40% 증가 (자신 사용)
		if( chara != nullptr)
		{
			switch( pInfo->GetTypeClass() )
			{
			//case WEAPON_CLASS_HANDGUN :
			case WEAPON_SUBCLASS_ASSAULT :
			case WEAPON_CLASS_ASSAULT :
			case WEAPON_SUBCLASS_SMG :
			case WEAPON_CLASS_SMG :
			case WEAPON_SUBCLASS_SNIPER :
			case WEAPON_CLASS_SNIPER :
			case WEAPON_SUBCLASS_SHOTGUN :
			case WEAPON_CLASS_SHOTGUN :
			case WEAPON_CLASS_MG :
			//case WEAPON_CLASS_CIC :
			//case WEAPON_CLASS_DUALHANDGUN :
			//case WEAPON_CLASS_DUALSMG :
				count = count + (INT32) (abilities[EQUIP::eABILITY_MAXBULLET] * count);
				break;

			case WEAPON_CLASS_THROWING_GRENADE :
				if( pInfo->GetDual())
					count = count + (INT32) (abilities[EQUIP::eABILITY_THROWCOUNT] * 2);
				else
					count = count + (INT32) abilities[EQUIP::eABILITY_THROWCOUNT];
				break;

			default :	// 다른 무기는 적용하지 않음.
				break;
			}

			if( IsWeaponByIncreaseGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
			{
				////-----------------	Increase grenade slot
				//	사용자가 사용중이면 Max 갯수가 늘어난다. (자신 사용)
				count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
				CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));

			}
			else if( IsWeaponByIncreaseDualGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
			{
				////-----------------	Increase grenade slot
				//	사용자가 사용중이면 Max 갯수가 늘어난다. (자신 사용)
				//	듀얼 수류탄도 적용
				count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD))*2;
				CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			}

			// 보유탄수 증가권이 있는지를 확인하여 보유탄수 40% 증가 (자신 사용)
			if( pInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
			{
				count += (INT32)(BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_BULLETADD) * count);
				CASHITEM_TRACE("CASHITEMSKILL_BULLETADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_BULLETADD));
			}
		}

		return count;
	}

	INT32 GetLoadBulletWithCashItem(CGameCharaBase* chara, CWeaponInfo* pInfo, REAL32* abilities)
	{
		I3ASSERT( pInfo != nullptr);

		INT32 count = pInfo->GetLoadBullet();

		// 월드에 붙는 수류탄인 경우 그냥 원래 값으로 리턴
		if( chara != nullptr)
		{
			if( chara->getCharaNetIndex() == 0xffffffff)
				return count;

			//11766 노현서, 수류탄은 Max Count 에만 +1 증가 처리합니다 
			//if( pInfo->GetTypeClass() == WEAPON_CLASS_THROWING_GRENADE)
			//{
			//	if( pInfo->GetDual())
			//		count = count + (INT32) (abilities[EQUIP::eABILITY_THROWCOUNT] * 2);
			//	else
			//		count = count + (INT32) abilities[EQUIP::eABILITY_THROWCOUNT];
			//}

			//if( IsWeaponByIncreaseGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
			//{
			//	////-----------------	Increase grenade slot
			//	//	사용자가 사용중이면 Max 갯수가 늘어난다. (자신 사용)
			//	count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			//	CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			//}
			//else if( IsWeaponByIncreaseDualGrenadeSlot( pInfo->GetTypeClass(), pInfo->GetRealNumber()))
			//{
			//	////-----------------	Increase grenade slot
			//	//	사용자가 사용중이면 Max 갯수가 늘어난다. (자신 사용)
			//	//	듀얼 수류탄도 적용
			//	count += static_cast<INT32>(BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD))*2;
			//	CASHITEM_TRACE("CASHITEMSKILL_GRENADE_ADD = %f\n", BattleSlotContext::i()->GetUseCashItemValue(chara->getCharaNetIndex(), CASHITEMSKILL_GRENADE_ADD));
			//}
		}

		return count;
	}

	REAL32 GetRespawnTime(REAL32* abilities)
	{
		REAL32 rRespawnLimit = 2.0; // 디폴트 값이 2.0으로 코드에 박혀있다. CGameCharaAnimControl::OnDeathFinish() 확인 요망.

		// 1.0 version
		//if( LocaleValue::Enable( "EnableImmediateRespawn"))
		//{//모든 유저 즉시 리스폰 적용( 이벤트용 ) //EVENT_IMMEDIATE_RESPAWN//
		//	rRespawnLimit = 0.0f;
		//}
		//else
		//{
			if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_100) )
			{
				rRespawnLimit = 0.0f;
			}
			else if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_50))
			{
				// 리스폰 단축권이 있는지를 체크하여 50% 감소
				rRespawnLimit = rRespawnLimit - ( VALUE_CASH_ITEM_SHORT_RESPAWN_50 * rRespawnLimit );
			}
			else if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN) ||
				CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_30_EV))
			{
				rRespawnLimit = rRespawnLimit - ( VALUE_CASH_ITEM_SHORT_RESPAWN_30 * rRespawnLimit );
			}
			else if( CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_20) ||
				CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_SHORT_RESPAWN_20_EV))
			{
				rRespawnLimit = rRespawnLimit - ( VALUE_CASH_ITEM_SHORT_RESPAWN_20 * rRespawnLimit );
			}
		//}

		// 1.5 version
		rRespawnLimit += abilities[EQUIP::eABILITY_RESPAWNTIME];

		return  rRespawnLimit;
	}

	REAL32 GetWeaponSwapTime(REAL32* abilities)
	{
		REAL32 timeScale = 1.0f; // CGameCharaActionControl::OnActionSwapWeapon() 참조.
		
		if (CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_QUICK_CHANGE_WEAPON) ||
			CInvenList::i()->IsUsingCashItem(CASHITEM_ITEMID_QUICK_CHANGE_WEAPON_EV))
		{
			REAL32 ratio = ((REAL32)-VALUE_CASH_ITEM_QUICK_CHANGE_WEAPON * 0.01f);
			ratio += (ratio > 0 ? I3_EPS : -I3_EPS);

			timeScale += (timeScale * ratio);
		}

		timeScale += abilities[EQUIP::eABILITY_SWAPTIME];

		return timeScale;
	}
}

/***********************//***********************/
extern "C" 
{
	int sm_ability_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg sm_ability_glue[] =
{
	{ "Click",			sm_ability_click	},
	{ nullptr,							nullptr}
};

void SMenuAbility::clicked(int idx)
{
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "Enable");
	i3Lua::PushBoolean(L, idx != 0 );
	UILua::EndLuaFunction(L , 1);
}

/*************************//*************************/
void SMenuAbility::smenu_ability_update(void* arg1, void* arg2)
{
	if (g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() != USP_SHOP &&
		g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() != USP_GIFTSHOP)
	{
		UISideMenu::instance()->get_frm_wnd(SIDEMENU_BTN_ABILITY)->EnableCtrl(false);
		EnableExInfo(false);
		EnableMainInfo(false);
		return;
	}
	if (m_curShopCategory == item_def::CTG_CHARA
//		|| m_curShopCategory == item_def::CTG_NEWHOT
		|| m_curShopCategory == item_def::CTG_ITEM
		|| m_partsInfo == 0) return;

	if (m_equippedWeaponInfo == 0 || m_previewWeaponInfo == 0)
	{
		const ITEM_INFO & item_info = CharaInfoMgr::instance()->find( CCharaInfoContext::i()->GetMainCharaSlotIdx(),
			item_def::SCTG_WEAPON, item_def::WT_PRIMARY);
		m_equippedWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(item_info.m_TItemID);
		if (m_equippedWeaponInfo == nullptr) return;
		m_previewWeaponInfo = m_equippedWeaponInfo;
	}
	USER_INFO_BASIC user_info;
	UserInfoContext::i()->GetMyUserInfoBasic(&user_info);
	// vv3 - check
	T_ItemID chara_itemid = m_partsInfo->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	
	const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_itemid);
	if(pCharaInfo == nullptr)
		return;

	// abilities
	REAL32 ability_value[ EQUIP::eABILITY_COUNT ];
	memset(ability_value, 0, sizeof(REAL32) * EQUIP::eABILITY_COUNT ); 
	// vv3 - check
	for(size_t i = CHAR_EQUIPMENT_PARTS_HEAD; i<CHAR_EQUIPMENT_PARTS_COUNT; ++i)
	{
		if( CHAR_EQUIPMENT_PARTS_SKIN == i ) continue;

		T_ItemID itemid = m_partsInfo->GetPartsItemID(static_cast<CHAR_EQUIPMENT_PARTS>(i));
		if( itemid == 0 ) continue;

		const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo(itemid);
		if( pInfo != nullptr)
		{
			for(size_t j = EQUIP::eABILITY_DAMAGE; j<EQUIP::eABILITY_COUNT; ++j)
			{
				if( (j == EQUIP::eABILITY_PROTECTION_RATE) &&
					pInfo->IsDefaultEquipment() )
				{// 기본 헤드기어의 방어율 표시 안함(10452.버그 감추기)
				}
				else
				{	
					ability_value[j] += pInfo->GetAbilityValue( (EQUIP::eABILITY)j );
				}
			}
		}
	}

	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "SetExInfo");


	//defense
	REAL32 defense = ability_value[EQUIP::eABILITY_DEFENSE_BODY] + ability_value[EQUIP::eABILITY_DEFENSE_ARM] + ability_value[EQUIP::eABILITY_DEFENSE_LEG];
	defense = (defense == 0.f) ? 100.f : (defense * 100.f) + 100.f;
	
	{
		i3::stack_wstring wstr_stack;
		i3::ftoa(defense, wstr_stack, 1);
		i3Lua::PushStringUTF16To8( L, wstr_stack ); 
	}
	
	i3::wstring ability_str;

	if( m_previewWeaponInfo != nullptr)
	{
	
		//range
		ability_str = to_ability_string(m_previewWeaponInfo->GetRange(), ability_value[EQUIP::eABILITY_RANGE]);
		i3Lua::PushStringUTF16To8( L, ability_str ); 

		//bullet
		INT32 loadedBullet = GetLoadBulletWithCashItem(g_pCharaManager->GetLobbyAvatar(), m_previewWeaponInfo, ability_value);
		INT32 maxBullet =  GetMaxBulletWithCashItem(g_pCharaManager->GetLobbyAvatar(), m_previewWeaponInfo, ability_value);
		ability_str = to_ability_string(loadedBullet, maxBullet);
		i3Lua::PushStringUTF16To8( L, ability_str ); 

		//reload
		ability_str = to_ability_string(m_previewWeaponInfo->GetReloadTime(), ability_value[EQUIP::eABILITY_RELOADTIME]);
		i3Lua::PushStringUTF16To8( L, ability_str ); 

		//throw
		ability_str = to_ability_string(m_previewWeaponInfo->GetThrowSpeed(), ability_value[EQUIP::eABILITY_THROWSPEED]);
		i3Lua::PushStringUTF16To8( L, ability_str ); 
	}
	else
	{
		i3Lua::PushStringUTF16To8( L, L""); 
		i3Lua::PushStringUTF16To8( L, L""); 
		i3Lua::PushStringUTF16To8( L, L""); 
		i3Lua::PushStringUTF16To8( L, L""); 
	}

	//respawn
	ability_str = to_ability_string(GetRespawnTime(ability_value));
	i3Lua::PushStringUTF16To8( L, ability_str ); 

	/********************************/
	//swap
	ability_str = to_ability_string(GetWeaponSwapTime(ability_value));
	i3Lua::PushStringUTF16To8( L, ability_str); 

	//helmet_defence
	REAL32 helmetPercentage =
		(pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_HEAD) + ability_value[EQUIP::eABILITY_PROTECTION_RATE]);
	ability_str = to_ability_string(helmetPercentage);
	i3Lua::PushStringUTF16To8( L, ability_str ); 

	//body
	REAL32 bodyPercentage =
		(pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_BODY) + ability_value[EQUIP::eABILITY_DEFENSE_BODY]);
	ability_str = to_ability_string(bodyPercentage);
	i3Lua::PushStringUTF16To8( L, ability_str ); 

	//arms
	REAL32 armsPercentage =
		(pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_ARM) + ability_value[EQUIP::eABILITY_DEFENSE_ARM]);
	ability_str = to_ability_string(armsPercentage);
	i3Lua::PushStringUTF16To8( L, ability_str ); 

	//legs
	REAL32 legsPercentage =
		(pCharaInfo->GetPartDamageRate( CHARA::eDAMAGEPART_LEG) + ability_value[EQUIP::eABILITY_DEFENSE_LEG]);
	ability_str = to_ability_string(legsPercentage);
	i3Lua::PushStringUTF16To8( L, ability_str ); 

	UILua::EndLuaFunction( L, 11);

	INT32 damage = 0;
	INT32 accuracy = 0;
	INT32 rpm = 0;
	INT32 speed = 0;
	
	if( m_previewWeaponInfo != nullptr)
	{
		//Part와 Skin만 현재 파츠의 계산을 하였지만 기획상
		//Default로 모든 계산을 적용한 수치가 들어가야 합니다.
		//데미지증가
		damage = m_previewWeaponInfo->GetPercentage(WEAPON::INFO_DAMAGE);
		if (ability_value[EQUIP::eABILITY_DAMAGERATE] > 0.0f)
			damage += static_cast<INT32>(ability_value[EQUIP::eABILITY_DAMAGERATE]*100.0f);
		if (damage < 0) damage = 0;

		//명중률증가 
		accuracy = m_previewWeaponInfo->GetPercentage(WEAPON::INFO_HIT_RATE);
		if (ability_value[EQUIP::eABILITY_ACCURACY] > 0.0f)
			accuracy += static_cast<INT32>(ability_value[EQUIP::eABILITY_ACCURACY]*100.0f);
		if (accuracy < 0) accuracy = 0;

		rpm = m_previewWeaponInfo->GetPercentage(WEAPON::INFO_RAPID);
		if (ability_value[EQUIP::eABILITY_FIREDELAY] > 0.0f)
			rpm += static_cast<INT32>(ability_value[EQUIP::eABILITY_FIREDELAY]*100.0f);
		if (rpm < 0) rpm = 0;

		speed = m_previewWeaponInfo->GetPercentage(WEAPON::INFO_SPEED);
		if (ability_value[EQUIP::eABILITY_MOVESPEED_FRONT] > 0.0f)
			speed += static_cast<INT32>(ability_value[EQUIP::eABILITY_MOVESPEED_FRONT]*100.0f);
		if (speed < 0) speed = 0;
	}
	
	// 거지 같아졌어 ...
	switch (m_maininfoUpdateType)
	{
	case AMUT_PREVIEW_WEAPON:
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainPreviewInfo");
			UILua::EndLuaFunction(L, 0);

			// 클릭한 무기 능력치 출력
			i3::stack_wstring wstr_stack;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainPreviewInfo");
			i3Lua::PushNumber(L, (float)damage/100.0f);
			i3::itoa(damage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)accuracy/100.0f);
			i3::itoa(accuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)rpm/100.0f);
			i3::itoa(rpm, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)speed/100.0f);
			i3::itoa(speed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);

			// 기본 장착 무기 능력치 출력
			INT32 dfDamage = m_equippedWeaponInfo->GetPercentage(WEAPON::INFO_DAMAGE);

			INT32 dfAccuracy = m_equippedWeaponInfo->GetPercentage(WEAPON::INFO_HIT_RATE);
			if (ability_value[EQUIP::eABILITY_ACCURACY] > 0.0f)
				dfAccuracy = dfAccuracy + static_cast<INT32>(ability_value[EQUIP::eABILITY_ACCURACY]*100.0f);
			if (dfAccuracy < 0) dfAccuracy = 0;

			INT32 dfSpeed = m_equippedWeaponInfo->GetPercentage(WEAPON::INFO_SPEED);
			dfSpeed += static_cast<INT32>(ability_value[EQUIP::eABILITY_MOVESPEED_FRONT]);

			INT32 dfRPM = m_equippedWeaponInfo->GetPercentage(WEAPON::INFO_RAPID);
			if (dfRPM < 0) dfRPM = 0;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainDefaultInfo");
			i3Lua::PushNumber(L, (float)dfDamage/100.0f);
			i3::itoa(dfDamage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)dfAccuracy/100.0f);
			i3::itoa(dfAccuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)dfRPM/100.0f);
			i3::itoa(dfRPM, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)dfSpeed/100.0f);
			i3::itoa(dfSpeed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);
		}
		break;

	case AMUT_PREVIEW_PARTS:
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainPreviewInfo");
			UILua::EndLuaFunction(L, 0);

			// 클릭한 무기 능력치 출력
			i3::stack_wstring wstr_stack;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainPreviewInfo");
			i3Lua::PushNumber(L, (float)damage/100.0f);

			i3::itoa(damage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)accuracy/100.0f);

			i3::itoa(accuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)rpm/100.0f);

			i3::itoa(rpm, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)speed/100.0f);

			i3::itoa(speed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);
		}
		break;

	case AMUT_PURCHASED:
		{
			// 구입 물품이 바로 장착되기 때문에 기본 장착 정보로 출력.
			L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainPreviewInfo");
			UILua::EndLuaFunction(L, 0);

			i3::stack_wstring wstr_stack;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainDefaultInfo");
			i3Lua::PushNumber(L, (float)damage/100.0f);
			i3::itoa(damage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
		
			i3Lua::PushNumber(L, (float)accuracy/100.0f);
			i3::itoa(accuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)rpm/100.0f);
			i3::itoa(rpm, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)speed/100.0f);
			i3::itoa(speed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);

			m_equippedWeaponInfo = m_previewWeaponInfo;
		}
		break;

	case AMUT_REVERT:
		{
			L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainDefaultInfoOnly");
			UILua::EndLuaFunction(L, 0);

			i3::stack_wstring wstr_stack;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainDefaultInfo");
			i3Lua::PushNumber(L, (float)damage/100.0f);
			i3::itoa(damage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)accuracy/100.0f);
			i3::itoa(accuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)rpm/100.0f);
			i3::itoa(rpm, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)speed/100.0f);
			i3::itoa(speed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);
		}
		break;

	case AMUT_SLOTCHARA_CHANGED:
		{

			L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainDefaultInfoOnly");
			UILua::EndLuaFunction(L, 0);

			i3::stack_wstring wstr_stack;

			L = UILua::CallLuaFunction(g_scn_info.scn, "SetMainDefaultInfo");
			i3Lua::PushNumber(L, (float)damage/100.0f);

			i3::itoa(damage, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)accuracy/100.0f);
			
			i3::itoa(accuracy, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)rpm/100.0f);
			
			i3::itoa(rpm, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			i3Lua::PushNumber(L, (float)speed/100.0f);
			
			i3::itoa(speed, wstr_stack);
			i3Lua::PushStringUTF16To8(L, wstr_stack);
			UILua::EndLuaFunction(L, 8);

			m_equippedWeaponInfo = m_previewWeaponInfo;
		}
		break;
	}
}

/*virtual*/ bool SMenuAbility::IsStatusEnable(void) const
{
	switch( m_curShopCategory )
	{
	case item_def::CTG_CHARA	:
	case item_def::CTG_ITEM		:	return false;
		//{
			//UISideMenu::instance()->get_frm_wnd(SIDEMENU_BTN_ABILITY)->EnableCtrl(false);
			//EnableExInfo(false);
			//EnableMainInfo(false);
		//}
	default						: break;
	}

	return true;
}

/*************************//*************************/
void SMenuAbility::hit_test_enter( UINT32 inx, i3UIControl* pWnd )
{
}

void SMenuAbility::hit_test_leave( UINT32 inx, i3UIControl* pWnd )
{
	GameUI::ResetMousePressedUp();
}



//------------------------------------------------------------------------------//
SMenuAbility::SMenuAbility(UISideMenu* p) : iSMenu(p), m_previewWeaponInfo(0), m_equippedWeaponInfo(0)
	, m_partsInfo(0), m_nowShopCategory(item_def::CTG_WEAPON) , m_curShopCategory(item_def::CTG_WEAPON)
{
	g_this = this;	

	m_HitTestCallback.SetObj( this );
	m_tmHitTest.SetCallback( &m_HitTestCallback );

	RegisterGameEventFunc( EVENT_ITEM_AUTH,						&SMenuAbility::ReceivedGameEvent_Update_Ability);
	RegisterGameEventFunc( EVENT_CLAN_PERSON_PLUS,				&SMenuAbility::ReceivedGameEvent_Update_Ability);
}

SMenuAbility::~SMenuAbility()
{
}

void SMenuAbility::OnCreate( i3GameNode * pParent)
{
	g_scn_info.idx = mParent->GetSceneCount();
	mParent->AddScene("Scene/Main/PointBlankRe_SideMenu_Ability.i3UIs", sm_ability_glue, true, false);
}

void SMenuAbility::OnLoadAllScenes()
{
	//Lua 초기화 한다.
	g_scn_info.scn = mParent->GetScene(g_scn_info.idx);
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "LoadAllScenes");
	UILua::EndLuaFunction(L , 0);

	//m_tmHitTest.add( g_scn_info.scn, "i3UIFrameWnd_Ability_Default" );
	//m_tmHitTest.add( g_scn_info.scn, "i3UIFrameWnd_Ability_Ex" );
}

void SMenuAbility::OnUnloadAllScenes()
{
	//UI::remove_cb(this);
	//m_tmHitTest.clear();
}

void SMenuAbility::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == UIEVENT_UPDATE_ABILITY)
		{
			smenu_ability_update();
		}
	}
}

void SMenuAbility::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	i3::unordered_map< GAME_EVENT, func >::iterator it;
	it = m_RecvGameEventMap.find( evt);

	if ( it != m_RecvGameEventMap.end() )
	{
		func & f = it->second;
		(this->*f)(arg, UserData);
	}
}


void SMenuAbility::Entrance(bool arg1)	//frame Open, data update
{
	//들어오는 구멍이 2개다.	
	//1. 첫 시작시 전체적인 Entrance  : 스킵 
	//2. 중 간 : 갱신

	m_equippedWeaponInfo = nullptr;	//장착중인 무기 포인터를 nullptr로 처리해서 
									//무기를 바꿔 장착했을때 이전에 장착했던 무기의 능력치가 나타나지 않도록 하고
									//smenu_ability_update()에서 다시 현재의 무기 포인터를 받도록 함

	EnableExInfo(arg1);

	smenu_ability_update();

	g_scn_info.scn->Attach( false );
}

void SMenuAbility::Exit(void* arg1, void* arg2)
{
	EnableExInfo(false);
}

void SMenuAbility::EnableExInfo(bool enable)
{
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "EnableExInfo");
	i3Lua::PushBoolean(L, enable );
	UILua::EndLuaFunction(L , 1);

	m_bSubMenuOpened = enable;
}

void SMenuAbility::EnableMainInfo(bool enable)
{
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainInfo");
	i3Lua::PushBoolean(L, enable);
	UILua::EndLuaFunction(L, 1);
}

void SMenuAbility::EnableMainDefaultInfoOnly()
{
	LuaState* L = UILua::CallLuaFunction(g_scn_info.scn, "EnableMainDefaultInfoOnly");
	UILua::EndLuaFunction(L, 0);
}

void SMenuAbility::EnableCtrlsByShopCategory(item_def::Category shopCategory, bool nowFlag)
{
	if(nowFlag)
		m_nowShopCategory = shopCategory;
	else
	{
		switch( shopCategory)
		{
		case item_def::CTG_CHARA:
		case item_def::CTG_ITEM:
			{
				UISideMenu::instance()->get_frm_wnd(SIDEMENU_BTN_ABILITY)->EnableCtrl(false);
				EnableExInfo(false);
				EnableMainInfo(false);
			}
			break;

		case item_def::CTG_WEAPON:
		case item_def::CTG_PARTS:
		case item_def::CTG_SKIN:
		case item_def::CTG_NEWHOT:
			{
				UISideMenu::instance()->get_frm_wnd(SIDEMENU_BTN_ABILITY)->EnableCtrl(true);
				EnableMainDefaultInfoOnly();
			}
			break;
		}

		m_curShopCategory = shopCategory;
	}
}

void SMenuAbility::RegisterGameEventFunc( GAME_EVENT evt, func f)
{
	m_RecvGameEventMap.insert( i3::unordered_map< GAME_EVENT, func>::value_type( evt, f) );
}

void SMenuAbility::ReceivedGameEvent_Update_Ability( INT32, const i3::user_data&)
{
	smenu_ability_update();
}