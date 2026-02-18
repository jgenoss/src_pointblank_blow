#include "pch.h"
#include "QJSlot.h"
#include "UIPopupQuickJoin.h"
#include "../../GameGUI.h"
#include "../UIUtil.h"
#include "../../TextSet.h"
#include "QJSlotScene.h"
#include "../../QuickJoinContext.h"
#include "i3Base/itl/bind/bind.h"

#include "ClientStageInfoUtil.h"

namespace 
{
	Qjoin::Slot* g_this = 0;
}

extern "C"
{
	int UIPopupQuickJoinSlot_StartClicked(LuaState* L)
	{
		INT32 idxSlot = i3Lua::GetIntegerArg(L, 1);

		g_this->StartClicked(idxSlot);

		return 0;
	}

	int UIPopupQuickJoinSlot_ModeSelClicked(LuaState* L)
	{
		INT32 idxSlotScene = i3Lua::GetIntegerArg(L, 1);

		g_this->ModeSelClicked(idxSlotScene);

		return 0;
	}

	int UIPopupQuickJoinSlot_StageSelClicked(LuaState* L)
	{
		INT32 idxSlotScene = i3Lua::GetIntegerArg(L, 1);

		g_this->StageSelClicked(idxSlotScene);

		return 0;
	}
}

LuaFuncReg QuickStartSlotGlue[] =
{
	{ "StartClicked", UIPopupQuickJoinSlot_StartClicked },
	{ "ModeSelClicked", UIPopupQuickJoinSlot_ModeSelClicked },
	{ "StageSelClicked", UIPopupQuickJoinSlot_StageSelClicked },
	{ nullptr, nullptr }
};


namespace Qjoin
{
	Slot::Slot(UIPopupQuickJoin* p) : m_parent(p)
	{
		g_this = this;

		for (INT32 i = 0; i < SCT_MAX; ++i)
		{
			m_unit[i].startBtn = nullptr;
			m_unit[i].mode = nullptr;
			m_unit[i].stage = nullptr;
			m_unit[i].image = nullptr;
			m_unit[i].mgr = nullptr;
		}
	}

	Slot::~Slot()
	{
		for (INT32 i=0; i<SCT_MAX; ++i)
		{
			I3_SAFE_DELETE( m_unit[i].startBtn);
			I3_SAFE_DELETE( m_unit[i].mode);
			I3_SAFE_DELETE( m_unit[i].stage);
			I3_SAFE_DELETE( m_unit[i].image);
			I3_SAFE_DELETE( m_unit[i].mgr);
		}
	}

	void Slot::OnCreate(i3GameNode* pParent)
	{
		mSlot.EnableScroll(false);
		mSlot.OnCreate((UIBase*)m_parent, 1, SCT_MAX);

		// Load Scene
		mSlot.RegisterSlotScene("Scene/Popup/PBRe_PopUp_QuickEnter_slot.i3UIs", QuickStartSlotGlue, true);

		for (INT32 i=0; i<SCT_MAX; ++i)
		{
			i3UIScene* scene = mSlot.GetSlotScene(i);

			m_unit[i].mgr = new CollMgr(i);

			m_unit[i].startBtn = new Coll_StartBtn(m_unit[i].mgr, scene, "StartBtn");
			m_unit[i].image = new Coll_Imgbox(m_unit[i].mgr, scene, "Imgbox");
			m_unit[i].mode = new Coll_ModeCombobox(m_unit[i].mgr, scene, "ModeCombobox");
			m_unit[i].stage = new Coll_StageCombobox(m_unit[i].mgr, scene, "StageCombobox");

			MEMDUMP_NEW( m_unit[i].mgr, sizeof(CollMgr) );
			MEMDUMP_NEW( m_unit[i].startBtn, sizeof(Coll_StartBtn) );
			MEMDUMP_NEW( m_unit[i].mode, sizeof(Coll_ModeCombobox) );
			MEMDUMP_NEW( m_unit[i].stage, sizeof(Coll_StageCombobox) );
			MEMDUMP_NEW( m_unit[i].image, sizeof(Coll_Imgbox) );
		}
	}

	void Slot::OnLoadAllScenes()
	{
		INT32 left = 0, top =0;
		m_parent->GetLeftTopPos(left, top); 
		mSlot.OnLoadAllScenes(left+25,top+58,0,0);
		mSlot.SetEnableMouseOver(true);
		iSlot::reset_select(SCT_MAX);

		for (INT32 i=0; i<SCT_MAX; ++i)
		{
			i3UIScene* scene = mSlot.GetSlotScene(i);

			LuaState *L = UILua::CallLuaFunction(scene, "SetSlotSceneInfo");
			if (L != nullptr)
			{
				i3Lua::PushInteger(L, i);
				UILua::EndLuaFunction(L , 1);
			}
		}

		// 서버로부터 내려받은 모드/스테이지 정보 선택.
		QuickJoinContext* info = QuickJoinContext::i();
		for (INT32 i=0; i<SCT_MAX; i++)
		{
			STAGE_ID  stageid	= static_cast<STAGE_ID>(info->getQuickJoinInfo(i)->m_ui32StageID);
			STAGE_UID stageuid  = StageID::GetStageUID(stageid);
			CSI_RULE  stagerule;
			if (StageID::GetRule(stageid) == 0)	
				stagerule.m_i32RuleIdx = -1;
			else								
				stagerule = *CStageInfoUtil::GetRuleData((UINT32)StageID::GetRule(stageid), false);
			
			m_unit[i].mgr->ConstructOnLoadScenes(stagerule, stageuid);
		}
		
	}

	void Slot::OnUnloadAllScenes()
	{
		mSlot.OnUnloadAllScenes();
	}

	void Slot::OnRevive(i3RenderContext * pCtx)
	{
		INT32 left = 0, top = 0;
		m_parent->GetLeftTopPos(left, top);
		mSlot.SetPosition(left, top);
	}

	void Slot::OnUpdate(REAL32 rDeltaSeconds)
	{
		mSlot.OnUpdate();
	}

	void Slot::OnKeyInput(i3InputKeyboard* keyboard)
	{
		SLOT_SCENE_TYPE type = SCT_NONE;

		if (keyboard->GetStrokeState(I3I_KEY_1))
			type = SCT_1ST;
		else if (keyboard->GetStrokeState(I3I_KEY_2))
			type = SCT_2ND;
		else if (keyboard->GetStrokeState(I3I_KEY_3))
			type = SCT_3RD;
		else 
			return;

		StartClicked(type);
	}

	void Slot::StartClicked(INT32 idxSlotScene)
	{
		QuickJoinContext::i()->setQuickJoinIndex((UINT8)idxSlotScene);

		m_unit[idxSlotScene].startBtn->OnClicked();
	}

	void Slot::ModeSelClicked(INT32 idxSlotScene)
	{
		Coll_ModeCombobox* modebox = m_unit[idxSlotScene].mode;
		if (modebox)
			modebox->OnClicked();
	}

	void Slot::StageSelClicked(INT32 idxSlotScene)
	{
		Coll_StageCombobox* stagebox = m_unit[idxSlotScene].stage;
		if (stagebox)
			stagebox->OnClicked();
	}

	void Slot::RefreshAllStartButton()
	{
		CollMgr* mgr = nullptr;
		for (INT32 i = 0; i < SCT_MAX; i++)
		{
			mgr = m_unit[i].mgr;
			if (mgr)
				mgr->RefreshStartBtn();
		}
	}

	void Slot::cb_Enabled_impl(INT32 idx, i3UIScene* scn)
	{
		scn->EnableScene(true);
	}

	void Slot::cb_Disabled_impl(INT32 idx, i3UIScene* scn)
	{
		scn->EnableScene(true);
	}

	void Slot::cb_Enter_impl(INT32 idx, i3UIScene* scn)
	{
		LuaState * L = UILua::CallLuaFunction(scn, "Enter");
		UILua::EndLuaFunction(L , 0);
	}

	void Slot::cb_Leave_impl(INT32 idx, i3UIScene* scn)
	{
		LuaState * L = UILua::CallLuaFunction(scn,  "Leave");
		UILua::EndLuaFunction(L , 0);
	}
}