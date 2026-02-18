#include "pch.h"
#include "CreateRoom_Stage.h"

#include "MyRoomInfoContext.h"
#include "RandomMapContext.h"
#include "ClientStageInfoContext.h"

#include "ClientStageInfoUtil.h"

//STAGE======================================================================================================

CreateRoom_Stage::CreateRoom_Stage(i3UIScene * pBase)
{
	CreateRoom_Base::CreateRoom_Base(pBase);

	m_pMapScrollbar  = nullptr;
	m_pSelectedStage = nullptr;
	m_bCurRandomMap  = false;
}

CreateRoom_Stage::~CreateRoom_Stage()
{
	CreateRoom_Base::~CreateRoom_Base();
}

void		CreateRoom_Stage::InitData(bool isCreateRoom)
{
	m_bisCreateRoom = isCreateRoom;

	m_pMapScrollbar = (i3UIScrollBar*)m_pScene->FindChildByName("i3UIScrollBar_RoomList");
	I3ASSERT(m_pMapScrollbar);

	m_pMapScrollbar->setTickSize(1);
	{
		char wndName[MAX_NAME];

		for (int i = 0; i < MAPLIST_SLOT_COUNT; ++i)
		{
			i3::snprintf(wndName, MAX_NAME, "MapSlot%d", i);
			i3UIFrameWnd* wnd = (i3UIFrameWnd*)m_pScene->FindChildByName(wndName);
			bool bRes = m_MapSlot[i].Init(wnd);

			I3ASSERT(bRes);
		}
	}

	if (true != isCreateRoom)
		m_pSelectedStage = nullptr;
	
}

void		CreateRoom_Stage::UpdateforRule(CSI_RULE * rule, bool firstupdate)
{
	m_vStages.clear();

	//데이타 조정.
	CSI_STAGE_LIST stage_list;
	if (rule->m_i32RuleIdx == RULE_ID_EVENT)
		stage_list = CStageInfoUtil::GetStageListofEvent();
	else
		stage_list = CStageInfoUtil::GetStageList(rule->m_i32RuleIdx);

	for (size_t idx = 0; idx < stage_list.GetStageListSize(); idx++)
		m_vStages.push_back(stage_list.GetStageData(idx));

	//스크롤바 초기화.
	m_pSelectedStage = m_vStages[0];
	{
		INT32 i32Pos = 0;
		INT32 i32RangeMax = MAPLIST_SLOT_COUNT;
		bool bScrollEnable = false;

		if (m_vStages.size() > MAPLIST_SLOT_COUNT)
		{
			bScrollEnable = true;
			i32RangeMax = m_vStages.size();

			// map list auto scroll
			for (size_t i = 0; i < m_vStages.size(); ++i)
			{
				if (m_pSelectedStage->m_ui32StageID == m_vStages[i]->m_ui32StageID)
				{
					i32Pos = i;

					if (i == m_vStages.size() - 1)
						i32Pos -= 2;
					else if (i == m_vStages.size() - 2)
						i32Pos -= 1;
					if (i32Pos < 0)
						i32Pos = 0;

					break;
				}
			}
		}

		m_pMapScrollbar->setPageSize(MAPLIST_SLOT_COUNT);
		m_pMapScrollbar->setScrollPos(i32Pos);
		m_pMapScrollbar->setScrollRange(0, i32RangeMax);
		m_pMapScrollbar->EnableCtrl(bScrollEnable != false);
	}

	int idx = 0;
	if (m_bisCreateRoom)
	{
		if (MyRoomInfoContext::i()->getStage()->m_i32RuleIdx == m_vStages.at(0)->m_i32RuleIdx)
		{			
			for (idx = 0; idx < (int)m_vStages.size(); ++idx)
			{
				if (MyRoomInfoContext::i()->getStage()->m_ui32StageID == m_vStages.at(idx)->m_ui32StageID)
					break;
			}

			m_pMapScrollbar->setScrollPos(idx);
		}
	}

	//랜덤 맵 초기화.
	InitRandomMapList();

	//UI 셋팅.
	if(!m_bCurRandomMap ) ClickMapSlot((m_bisCreateRoom)? idx - m_pMapScrollbar->getScrollPos() : 0);
	MapListScroll();

}

bool		CreateRoom_Stage::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	if (m_pSelectedStage == nullptr) 
		return false;

	info._Basic._StageID = m_pSelectedStage->m_ui32StageID;	

	if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(info._Basic._InfoFlag, m_bCurRandomMap);
	}
	else
	{
		if (!m_bCurRandomMap)
		{
			for (INT32 i = 0; i < MAX_OPEN_STAGE; ++i)
				info._Add._ui64OpenStage[i] = 0;

			info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(info._Basic._InfoFlag, false);
		}
		else
		{
			RandomMapContext::i()->BackupStageList();
			RandomMapContext::i()->ClearStageList();

			if (m_vRandomStages.size() <= 0)
			{
				info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(info._Basic._InfoFlag, false);
				return true;
			}

			for (INT32 i = 0; i < MAX_OPEN_STAGE; ++i)
				info._Add._ui64OpenStage[i] = 0;

			for (size_t i = 0; i < m_vRandomStages.size(); ++i)
			{
				CSI_STAGE* stage = m_vRandomStages[i];
				RandomMapContext::i()->AddStage(stage);

				SET_ENABLE_RANDOM_MAP(info._Add._ui64OpenStage, stage->GetStageUID(), TRUE);
			}

			info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(info._Basic._InfoFlag, true);
		}
	}
	
	return true;
}

CSI_STAGE * CreateRoom_Stage::GetSelectedStage()
{
	return m_pSelectedStage;
}

void		CreateRoom_Stage::ClickMapSlot(int nSlot)
{
	if (nSlot < 0 || MAPLIST_SLOT_COUNT <= nSlot)
	{
		I3ASSERT(false);
		return;
	}

	INT32 idxScrollPos = m_pMapScrollbar->getScrollPos() + nSlot;

	CSI_STAGE* stage = m_vStages[idxScrollPos];
	if (m_bCurRandomMap)
	{
		if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
		{
			INT32 idx = i3::vu::int_index_of(m_vRandomStages, stage);

			if (0 <= idx)
				i3::vu::erase_index(m_vRandomStages, idx);
			else
				m_vRandomStages.push_back(stage);
		}
	}
	else
	{
		m_pSelectedStage = stage;
	}
	
	StageUIUpdate();
}

void		CreateRoom_Stage::MapListDBClick(int nSlot)
{
	ClickMapSlot(nSlot);
}

void		CreateRoom_Stage::MapListScroll()
{
	StageUIUpdate();
	
	int stage_list_idx = 0;
	for (size_t map_slot_idx = 0; map_slot_idx < MAPLIST_SLOT_COUNT; ++map_slot_idx)
	{
		bool mark = false;
		if (map_slot_idx < m_vStages.size())
		{
			stage_list_idx = m_pMapScrollbar->getScrollPos() + map_slot_idx;
			mark = (m_vStages[stage_list_idx]->m_ui8Mark == GOODS_MARK_NEW) ? true : false;
		}

		LuaState * L = _CallLuaFunction("SetNewMark");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, map_slot_idx);
			i3Lua::PushBoolean(L, mark);
			_EndLuaFunction(L, 2);
		}
	}
}

void		CreateRoom_Stage::MapListWheel(int zDelta)
{
	if (MAPLIST_SLOT_COUNT < m_vStages.size())
	{
		if (m_pMapScrollbar)
		{
			int nPos = m_pMapScrollbar->getScrollPos() - (int)(zDelta / 120);
			nPos = MINMAX(0, nPos, (int)m_vStages.size() - MAPLIST_SLOT_COUNT);

			m_pMapScrollbar->setScrollPos(nPos);
		}
	}

	StageUIUpdate();
}

void		CreateRoom_Stage::OnMouseOverSlot(int nSlot, bool bMouseOver)
{
	if (nSlot < 0 || MAPLIST_SLOT_COUNT <= nSlot)
	{
		I3ASSERT(false);
		return;
	}

	m_MapSlot[nSlot].OnMouseOver(bMouseOver);
}

void		CreateRoom_Stage::ClickRandomMap()
{
	LuaState * L = _CallLuaFunction("isRandomMap");

	if (nullptr != L)
	{
		bool bRandommap = UILua::ReturnLuaBoolean(L, 0);
		m_bCurRandomMap = bRandommap;
	}

	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		m_vRandomStages.clear();
		RandomMapContext::i()->BackupStageList();
		RandomMapContext::i()->ClearStageList();

		if (m_bCurRandomMap)
		{
			for (size_t i = 0; i < m_vStages.size(); ++i)
				m_vRandomStages.push_back(m_vStages[i]);
		}

	}
	
	m_pSelectedStage = m_vStages[0];

	StageUIUpdate();
}

void		CreateRoom_Stage::CancelRandomMap()
{
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		m_bCurRandomMap = false;
	}
	else
	{
		if (m_bCurRandomMap)
		{
			RandomMapContext::i()->ClearStageList();
			m_vRandomStages.clear();
			m_bCurRandomMap = false;
		}
	}
	
}

void CreateRoom_Stage::FocuseStageWheel()
{
	if (m_pMapScrollbar )
	{
		i3UIManager* pMgr = i3UI::getManager();

		if (pMgr)
			pMgr->SetFocusControl(m_pMapScrollbar);
	}
}

void		CreateRoom_Stage::StageUIUpdate()
{
	size_t idx;
	for (int i = 0; i < MAPLIST_SLOT_COUNT; ++i)
	{
		idx = m_pMapScrollbar->getScrollPos() + i;

		if (idx < m_vStages.size())
		{
			m_MapSlot[i].SetMapInfo(m_vStages[idx]);
			m_MapSlot[i].SetEnable(true);

			CSI_STAGE* stage = m_vStages[idx];
			if (m_bCurRandomMap)
			{
				// 랜덤맵 개선 시스템을 사용하지 않으면 기존 랜덤맵 방식을 쓴다.
				if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
				{
					m_MapSlot[i].SetSelect(true);
					m_MapSlot[i].SetActivate(false);
				}
				else
				{
					m_MapSlot[i].SetSelect(0 <= i3::vu::int_index_of(m_vRandomStages, stage));
					m_MapSlot[i].SetActivate(true);
				}
			}
			else
			{
				m_MapSlot[i].SetSelect((m_pSelectedStage->m_ui32StageID == stage->m_ui32StageID));
				
				if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
					m_MapSlot[i].SetActivate(true);
			}
		}
		else
		{
			m_MapSlot[i].SetSelect(false);
			m_MapSlot[i].SetEnable(false);
		}
	}

	LuaState * L = _CallLuaFunction("SetCheckRandomMap");
	if (nullptr != L)
	{
		if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
			i3Lua::PushInteger(L, (INT32)m_bCurRandomMap > 0 ? 1 : 0);
		else
			i3Lua::PushInteger(L, (INT32)m_vRandomStages.size() > 0 ? 1 : 0);

		_EndLuaFunction(L, 1);
	}

	L = _CallLuaFunction("SetRandomMapCount");
	if (nullptr != L)
	{
		if (true == ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
			i3Lua::PushInteger(L, m_vStages.size());
		else
			i3Lua::PushInteger(L, (INT32)m_vRandomStages.size());

		i3Lua::PushInteger(L, m_vStages.size());

		UILua::EndLuaFunction(L, 2);
	}

}

void		CreateRoom_Stage::InitRandomMapList()
{
	m_bCurRandomMap = false;
	m_vRandomStages.clear();

	if (MyRoomInfoContext::i()->IsRandomMap() && m_bisCreateRoom)
	{
		if (MyRoomInfoContext::i()->getStage()->m_i32RuleIdx == m_pSelectedStage->m_i32RuleIdx)
		{
			m_bCurRandomMap = true;

			if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
			{
				RandomMapContext::i()->TakeOverStageList();

				for (size_t i = 0; i < RandomMapContext::i()->GetStageListSize(); ++i)
					m_vRandomStages.push_back(RandomMapContext::i()->GetStage(i));
			}
		}
	}
}