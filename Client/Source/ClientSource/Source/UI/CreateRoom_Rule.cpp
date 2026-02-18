#include "pch.h"
#include "CreateRoom_Rule.h"

#include "ChannelContext.h"
#include "MyRoomInfoContext.h"

#include "ClientStageInfoUtil.h"

//RULE======================================================================================================

CreateRoom_Rule::CreateRoom_Rule(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);

	m_pSelectedRule = nullptr;
}

CreateRoom_Rule::~CreateRoom_Rule()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void		CreateRoom_Rule::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	//데이타 조정.
	m_vNormalRules.clear();
	m_vSpecialRules.clear();

	i3::vector<CSI_RULE*>  temp_rule_list;
	i3::vector<CSI_RULE*>::iterator iter	  = nullptr;

	//메인 룰 셋팅.
	temp_rule_list = CStageInfoUtil::GetRuleList();
	iter = temp_rule_list.begin();
	for (iter; iter != temp_rule_list.end(); iter++)
	{
		//클랜매치 일 때는 클랜 매치 불가능한 룰은 아예 넣어 버리지 않는다.
		if ( ChannelContext::i()->GetBattleChannel() )
		{
			if (!(*iter)->IsClanMatchStage())
				continue;
		}

		if (m_bisCreateRoom)
		{
			if (!CStageInfoUtil::IsDiableChaneMode(*MyRoomInfoContext::i()->getStage()))
			{
				if (MyRoomInfoContext::i()->getStage()->m_i32RuleIdx != (*iter)->m_i32RuleIdx)
					continue;
			}
			else
			{
				if ((*iter)->GetAIMode() != AI_USE_NO)
					continue;
			}
		}

		CSI_STAGE_LIST temp;
		if ((*iter)->m_i32RuleIdx != RULE_ID_EVENT)	temp = CStageInfoUtil::GetStageList((*iter)->m_i32RuleIdx);
		else										temp = CStageInfoUtil::GetStageListofEvent();

		if( temp.GetStageListSize() > 0 )
			m_vNormalRules.push_back((*iter));
	}

	//커스텀 룰 셋팅.
	temp_rule_list = CStageInfoUtil::GetRuleList(true);
	iter = temp_rule_list.begin();
	for (iter; iter != temp_rule_list.end(); iter++)
	{
		//클랜매치 일 때는 클랜 매치 불가능한 룰은 아예 넣어 버리지 않는다.
		if (ChannelContext::i()->GetBattleChannel())
		{
			if (!(*iter)->IsClanMatchStage())
				continue;
		}

		if (m_bisCreateRoom)
		{
			if (!CStageInfoUtil::IsDiableChaneMode(*MyRoomInfoContext::i()->getStage()))
			{
				if( MyRoomInfoContext::i()->getStage()->GetWeaponRestriction() != (*iter)->GetWeaponRestriction() )
					continue;
			}
			else
			{
				if ((*iter)->IsSpecialWar() )
					continue;
			}
		}

		CSI_STAGE_LIST temp = CStageInfoUtil::GetStageList((*iter)->m_i32RuleIdx);
		if (temp.GetStageListSize() > 0)
			m_vSpecialRules.push_back((*iter));
	}

	//UI 조정.

	RuleUIUpdate();

	if (m_bisCreateRoom)
	{
		i3::vector<CSI_RULE*>::iterator temp_iter = nullptr;
		i3::vector<CSI_RULE*>::iterator temp_end_iter = nullptr;

		bool is_special = MyRoomInfoContext::i()->getStage()->IsSpecialWar();
		is_special ? temp_iter = m_vSpecialRules.begin() : temp_iter = m_vNormalRules.begin();
		is_special ? temp_end_iter = m_vSpecialRules.end() : temp_end_iter = m_vNormalRules.end();

		int idx = 0;
		for (temp_iter; temp_iter != temp_end_iter; temp_iter++, ++idx)
		{
			if (MyRoomInfoContext::i()->getStage()->m_i32RuleIdx == (*temp_iter)->m_i32RuleIdx)
				SelectRule(idx, is_special);
		}
	}
	else
		SelectRule(0, false);
}

CSI_RULE*	CreateRoom_Rule::SelectRule(int nRule, bool bSpecial)
{
	LuaState * L;
	L = _CallLuaFunction("SetCurSelModeType");

	if (nullptr != L)
	{
		i3Lua::PushInteger(L, nRule);
		i3Lua::PushBoolean(L, bSpecial);
		UILua::EndLuaFunction(L, 2);
	}

	m_pSelectedRule = bSpecial ? m_vSpecialRules[nRule] : m_vNormalRules[nRule];

	return m_pSelectedRule;
}

CSI_RULE*	CreateRoom_Rule::GetSelectedRule()
{
	return m_pSelectedRule;
}

void		CreateRoom_Rule::RuleUIUpdate()
{
	i3UIListView_Box* list_view = (i3UIListView_Box*)m_pScene->FindChildByName("i3UIListView_Box");
	I3ASSERT(list_view);
	list_view->DeleteAllItems();

	i3::vector<CSI_RULE*>::iterator iter = nullptr;

	//기본룰들 UI에 셋팅.
	LuaState* L = _CallLuaFunction("SetListViewItemCount");
	i3Lua::PushInteger(L, (INT32)m_vNormalRules.size());
	_EndLuaFunction(L, 1);

	int idx = 0;
	for (iter = m_vNormalRules.begin(); iter < m_vNormalRules.end(); iter++, ++idx)
	{
		L = _CallLuaFunction("SetListViewItem");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushStringUTF16To8(L, (*iter)->getRuleName());
		_EndLuaFunction(L, 2);
	}

	//스페셜(커스텀)룰들 UI에 셋팅
	i3::rc_wstring wstr_special_rule;
	for (iter = m_vSpecialRules.begin(); iter < m_vSpecialRules.end(); iter++, ++idx)
	{
		wstr_special_rule += (*iter)->getRuleName();
		wstr_special_rule += L';';
	}

	L = _CallLuaFunction("SetSpecialModeItems");
	i3Lua::PushStringUTF16To8(L, wstr_special_rule);
	i3Lua::PushBoolean(L, m_vSpecialRules.size() != 0);
	_EndLuaFunction(L, 2);
}