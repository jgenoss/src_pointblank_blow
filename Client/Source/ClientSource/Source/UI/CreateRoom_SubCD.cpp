#include "pch.h"
#include "CreateRoom_SubCD.h"

#include "MyRoomInfoContext.h"
#include "UserInfoContext.h"

//SUB_CD======================================================================================================

CreateRoom_SubCD::CreateRoom_SubCD(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);

	button[CR_PushButton::TYPE_JUMP_IN].PushBtn("Btn_JumpInNO", CR_PushButton::JUMPIN_IMPOSSIBLE);
	button[CR_PushButton::TYPE_JUMP_IN].PushBtn("Btn_JumpInYES", CR_PushButton::JUMPIN_POSSIBLE);

	button[CR_PushButton::TYPE_TEAM_BALANCE].PushBtn("Btn_teamBal_None", CR_PushButton::TEAM_BALANCE_NONE);
	button[CR_PushButton::TYPE_TEAM_BALANCE].PushBtn("Btn_teamBal_Skill", CR_PushButton::TEAM_BALANCE_SKILL);
	button[CR_PushButton::TYPE_TEAM_BALANCE].PushBtn("Btn_teamBal_Member", CR_PushButton::TEAM_BALANCE_MEMBER);

	button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].PushBtn("Btn_AutoTeamChange_No", CR_PushButton::AUTO_TEAM_CHANGE_NOUSE);
	button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].PushBtn("Btn_AutoTeamChange_Yes", CR_PushButton::AUTO_TEAM_CHANGE_USE);
}

CreateRoom_SubCD::~CreateRoom_SubCD()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void		CreateRoom_SubCD::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	for (int i = 0; i < CR_PushButton::TYPE_MAX; i++)
		button[i].InitList(m_pScene);

	button[CR_PushButton::TYPE_JUMP_IN].SetValue(CR_PushButton::JUMPIN_IMPOSSIBLE);
	button[CR_PushButton::TYPE_TEAM_BALANCE].SetValue(CR_PushButton::TEAM_BALANCE_MEMBER);
	button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].SetValue(CR_PushButton::AUTO_TEAM_CHANGE_NOUSE);
}

void		CreateRoom_SubCD::UpdateforStage(CSI_STAGE * stage, bool firstupdate)
{
	LuaState * L;

	if (stage->IsAiMode())
	{
		L = _CallLuaFunction("SetAutoTeamChangeVisible");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}

		L = _CallLuaFunction("SetTeamBalanceVisible");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}

		L = _CallLuaFunction("SetJumpintVisible");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, false);
			_EndLuaFunction(L, 1);
		}
	}
	else
	{
		L = _CallLuaFunction("SetTeamBalanceVisible");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		L = _CallLuaFunction("SetJumpintVisible");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, true);
			_EndLuaFunction(L, 1);
		}

		L = _CallLuaFunction("SetAutoTeamChangeVisible");
		if (L != nullptr)
		{
			if (stage->IsModeforModeType(STAGE_MODE_ESCAPE) || stage->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
				i3Lua::PushBoolean(L, false);
			else
				i3Lua::PushBoolean(L, isTeamChangeMap(stage));
			_EndLuaFunction(L, 1);
		}
	}

	if (m_bisCreateRoom)
	{

		if (stage->m_i32RuleIdx == MyRoomInfoContext::i()->getStage()->m_i32RuleIdx)
		{
			ROOM_INFO_ADD		roomAddInfo;
			ROOM_INFO_BASIC		roomInfo;
			MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
			MyRoomInfoContext::i()->getMyRoomInfoAdd(&roomAddInfo);

			//서버는 0 : 없음 1 : 인원 2 : 스킬 이기 때문에 값을 고쳐줘야함
			if (roomAddInfo._LockTeamBalance == 2)
				roomAddInfo._LockTeamBalance = 1;
			else if (roomAddInfo._LockTeamBalance == 1)
				roomAddInfo._LockTeamBalance = 2;

			button[CR_PushButton::TYPE_JUMP_IN].SetValue(roomAddInfo._LockInterEnter);
			button[CR_PushButton::TYPE_TEAM_BALANCE].SetValue(roomAddInfo._LockTeamBalance);

			UINT8 team_change_flag = CR_PushButton::AUTO_TEAM_CHANGE_NOUSE;
			if ((GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(roomInfo._InfoFlag) == ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE))
				team_change_flag = CR_PushButton::AUTO_TEAM_CHANGE_USE;

			button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].SetValue(team_change_flag);

			return;
		}

	}
		
	button[CR_PushButton::TYPE_JUMP_IN].SetValue(CR_PushButton::JUMPIN_IMPOSSIBLE);
	button[CR_PushButton::TYPE_TEAM_BALANCE].SetValue(CR_PushButton::TEAM_BALANCE_MEMBER);
	if (isTeamChangeMap(stage)) button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].SetValue(CR_PushButton::AUTO_TEAM_CHANGE_USE);
	else						button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].SetValue(CR_PushButton::AUTO_TEAM_CHANGE_NOUSE);

}

bool		CreateRoom_SubCD::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	info._Add._LockInterEnter = button[CR_PushButton::TYPE_JUMP_IN].GetValue();
	info._Add._LockTeamBalance = button[CR_PushButton::TYPE_TEAM_BALANCE].GetValue();
	
	//서버는 0 : 없음 1 : 인원 2 : 스킬 이다.
	if (info._Add._LockTeamBalance == 2)
		info._Add._LockTeamBalance = 1;
	else if(info._Add._LockTeamBalance == 1)
		info._Add._LockTeamBalance = 2;

	UINT8 auto_team_change_flag = 0;
	if (button[CR_PushButton::TYPE_AUTO_TEAM_CHANGE].GetValue() == CR_PushButton::AUTO_TEAM_CHANGE_USE)
		auto_team_change_flag = ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE;
	info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(info._Basic._InfoFlag, auto_team_change_flag);

	return true;
}

void		CreateRoom_SubCD::SetPushedButton(INT32 slot, INT32 btn)
{
	button[slot].SetValue((UINT8)btn);
}

bool		CreateRoom_SubCD::isTeamChangeMap(CSI_STAGE* stage)
{
	if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE) == false)
	{
		// 탈출 / 크로스카운터 모드의 경우에는 강제로 활성화시킵니다.
		if (stage->IsModeforModeType(STAGE_MODE_ESCAPE) || stage->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			return true;

		return false;
	}

	return stage->IsAtkDefChangeStage();
}