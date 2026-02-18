#include "pch.h"
#include "CreateRoom_WeaponCD.h"

#include "MyRoomInfoContext.h"

//WEAPON_CD======================================================================================================

CreateRoom_WeaponCD::CreateRoom_WeaponCD(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);
}

CreateRoom_WeaponCD::~CreateRoom_WeaponCD()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void	CreateRoom_WeaponCD::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	m_LockWeapon = LOCK_BASIC;
	m_limitatt.Primary = TESTBIT(m_LockWeapon, LOCK_PRIMARY) ? 1 : 0;
	m_limitatt.Secondary = TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0;
	m_limitatt.Melee = TESTBIT(m_LockWeapon, LOCK_MELEE) ? 1 : 0;
	m_limitatt.Throw = TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0;
	m_limitatt.RPG7 = TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0;

}

void	CreateRoom_WeaponCD::UpdateforStage(CSI_STAGE * stage, bool firstupdate)
{
	m_pStage = stage;

	SetLimits();

	if (m_bisCreateRoom)
	{

		if (stage->m_i32RuleIdx == MyRoomInfoContext::i()->getStage()->m_i32RuleIdx)
		{
			ROOM_INFO_BASIC	roomInfo;
			MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
			m_LockWeapon = roomInfo._WeaponFlag;

			m_limitatt.Primary = TESTBIT(m_LockWeapon, LOCK_PRIMARY) ? 1 : 0;
			m_limitatt.Secondary = TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0;
			m_limitatt.Melee = TESTBIT(m_LockWeapon, LOCK_MELEE) ? 1 : 0;
			m_limitatt.Throw = TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0;
			m_limitatt.RPG7 = TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0;

			WeaponCDUIUpdate();
			return;
		}
	}
	
	m_LockWeapon = LOCK_BASIC;
	m_limitatt.Primary = TESTBIT(m_LockWeapon, LOCK_PRIMARY) ? 1 : 0;
	m_limitatt.Secondary = TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0;
	m_limitatt.Melee = TESTBIT(m_LockWeapon, LOCK_MELEE) ? 1 : 0;
	m_limitatt.Throw = TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0;
	m_limitatt.RPG7 = TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0;

	WeaponCDUIUpdate();
}

bool	CreateRoom_WeaponCD::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	info._Basic._WeaponFlag = GetLockAtLua();

	if (m_pStage->IsWeaponModeforWType(WEAPON_SNIPER_ONLY))
	{
		SETBIT(info._Basic._WeaponFlag, LOCK_SNIPER);
	}
	else if (m_pStage->IsWeaponModeforWType(WEAPON_SHOTGUN_ONLY))
	{
		info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN(info._Basic._WeaponFlag);
	}
	else if (m_pStage->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY))
	{
		info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_KNUCKLE(info._Basic._WeaponFlag);
	}

	return true;
}

void	CreateRoom_WeaponCD::WeaponCDUIUpdate()
{
	LuaState * L = _CallLuaFunction("setLimitWeapon");
	i3Lua::PushInteger(L, m_limitatt.Primary == 0 ? 1 : 0);			// 林公扁
	i3Lua::PushInteger(L, m_limitatt.Secondary == 0 ? 1 : 0);		// 焊炼公扁
	i3Lua::PushInteger(L, m_limitatt.Melee == 0 ? 1 : 0);			// 辟立 公扁
	i3Lua::PushInteger(L, m_limitatt.Throw == 0 ? 1 : 0);			// 捧么 公扁
	i3Lua::PushInteger(L, m_limitatt.RPG7 == 0 ? 1 : 0);			// RPG7
	_EndLuaFunction(L, 5);

	L = _CallLuaFunction("SpecialWeaponMode");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_weaponMode);
		_EndLuaFunction(L, 1);
	}
}

void	CreateRoom_WeaponCD::SetLimits()
{
	m_weaponMode = 0;

	if (m_pStage->IsWeaponModeforWType(WEAPON_SNIPER_ONLY) ||
		m_pStage->IsWeaponModeforWType(WEAPON_SHOTGUN_ONLY) ||
		m_pStage->IsModeforModeType(STAGE_MODE_ESCAPE) ||
		m_pStage->IsModeforModeType(STAGE_MODE_DEFENCE) ||
		m_pStage->IsModeforModeType(STAGE_MODE_DESTROY) ||
		(m_pStage->IsModeforModeType(STAGE_MODE_CONVOY) && m_pStage->IsEventforEventType(STAGE_EVENT_NORMAL) ||
		m_pStage->IsMultiWeapon())
		)
	{
		m_weaponMode = 1;	// 林公扁 Lock
	}
	else if (m_pStage->IsWeaponModeforWType(WEAPON_KNUCKLE_ONLY))
	{
		m_weaponMode = 2;	// 林公扁,焊炼公扁 off
	}
	else if (m_pStage->IsModeforModeType(STAGE_MODE_CONVOY) && m_pStage->IsEventforEventType(STAGE_EVENT_SEASON))
	{
		m_weaponMode = 3;	// 林公扁,焊炼公扁 off
	}
	else if (m_pStage->IsWeaponModeforWType(WEAPON_SNIPER_ONLY))
	{
		m_weaponMode = 4;
	}
	else if (m_pStage->IsWeaponModeforWType(WEAPON_RPG_ONLY))
	{
		m_weaponMode = 5;
	}
}

UINT8	CreateRoom_WeaponCD::GetLockAtLua()
{
	//// 公扁 力茄	
	LuaState * L = nullptr;
	m_LockWeapon = 0;

	UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
	UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
	UNSETBIT(m_LockWeapon, LOCK_RPG7);
	UNSETBIT(m_LockWeapon, LOCK_THROW);

	{
		L = _CallLuaFunction("isLimitPrimary");
		if (UILua::ReturnLuaBoolean(L, 0) == false)
			SETBIT(m_LockWeapon, LOCK_PRIMARY);
		else
			UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
	}

	{
		L = _CallLuaFunction("isLimitSecondary");
		if (UILua::ReturnLuaBoolean(L, 0) == false)
			SETBIT(m_LockWeapon, LOCK_SECONDARY);
		else
			UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
	}

	{
		L = _CallLuaFunction("isLimitThrow");
		if (UILua::ReturnLuaBoolean(L, 0) == false)
			SETBIT(m_LockWeapon, LOCK_THROW);
		else
			UNSETBIT(m_LockWeapon, LOCK_THROW);
	}

	{
		L = _CallLuaFunction("isLimitRPG7");
		if (UILua::ReturnLuaBoolean(L, 0) == false)
			SETBIT(m_LockWeapon, LOCK_RPG7);
		else
			UNSETBIT(m_LockWeapon, LOCK_RPG7);
	}

	SETBIT(m_LockWeapon, LOCK_MELEE);

	return m_LockWeapon;
}