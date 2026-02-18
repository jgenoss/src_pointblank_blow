#include "pch.h"
#include "CreateRoom_ObserverCD.h"

#include "MyRoomInfoContext.h"

#include "ClientStageInfoUtil.h"

//OBSERVER_CD======================================================================================================

CreateRoom_ObserverCD::CreateRoom_ObserverCD(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);
}

CreateRoom_ObserverCD::~CreateRoom_ObserverCD()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void	CreateRoom_ObserverCD::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	m_LockObserver = 0;
}

void	CreateRoom_ObserverCD::UpdateforStage(CSI_STAGE * stage, bool firstupdate)
{
	bool visible_observer = false;
	if (CStageInfoUtil::IsObserverModeStageByOnlyMode(*stage)) visible_observer = true;

	LuaState * L = _CallLuaFunction("SetObservervisible");
	i3Lua::PushBoolean(L, visible_observer);	// 관전 허용
	_EndLuaFunction(L, 1);

	if (m_bisCreateRoom)
	{

		if (stage->m_i32RuleIdx == MyRoomInfoContext::i()->getStage()->m_i32RuleIdx)
		{
			ROOM_INFO_ADD	roomAddInfo;
			MyRoomInfoContext::i()->getMyRoomInfoAdd(&roomAddInfo);
			m_LockObserver = roomAddInfo._LockObserver;
			ObserverCDUIUpdate();
			return;
		}
	}
	
	m_LockObserver = 0;

	ObserverCDUIUpdate();
}

bool	CreateRoom_ObserverCD::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	info._Add._LockObserver = GetLockAtLua();
	return true;
}

void	CreateRoom_ObserverCD::ObserverCDUIUpdate()
{
	LuaState * L = _CallLuaFunction("setLimitObserver");
	i3Lua::PushInteger(L, TESTBIT(m_LockObserver, LOCK_OBSERVER_CHASE) ? 1 : 0);	// 3인칭 회전 관전
	i3Lua::PushInteger(L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY) ? 1 : 0);	// 상대팀 관전
	i3Lua::PushInteger(L, TESTBIT(m_LockObserver, LOCK_OBSERVER_FREE) ? 1 : 0);		// 자유 시점
	i3Lua::PushInteger(L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP) ? 1 : 0);	// HP표시
	i3Lua::PushInteger(L, TESTBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY) ? 1 : 0);	// 3인칭 시점 제한
	_EndLuaFunction(L, 5);
}

UINT8	CreateRoom_ObserverCD::GetLockAtLua()
{
	LuaState * L = nullptr;

	UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
	UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
	UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
	UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
	UNSETBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY);
	//
	// 관전기능
	{
		L = _CallLuaFunction("isLimitObserver3");
		if (UILua::ReturnLuaBoolean(L, 0))
			SETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
	}

	{
		L = _CallLuaFunction("isLimitObserver");
		if (UILua::ReturnLuaBoolean(L, 0))
			SETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
	}

	{
		L = _CallLuaFunction("isLimitObserverEnemy");
		if (UILua::ReturnLuaBoolean(L, 0))
			SETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
	}

	{
		L = _CallLuaFunction("isLimitObserverHP");
		if (UILua::ReturnLuaBoolean(L, 0))
			SETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
	}

	//@eunil. 3인칭 시점제한 추가 1.1
	{
		L = _CallLuaFunction("isLimitObserver1only");
		if (UILua::ReturnLuaBoolean(L, 0))
			SETBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_1PVONLY);
	}

	return m_LockObserver;
}
