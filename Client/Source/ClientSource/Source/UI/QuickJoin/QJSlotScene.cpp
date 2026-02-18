#include "pch.h"
#include "QJSlotScene.h"
#include "../../GameGUI.h"
#include "../UIUtil.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "../../QuickJoinContext.h"
#include "i3Base/itl/bind/bind.h"

#include "ClientStageInfoUtil.h"

namespace Qjoin
{
	//--------------------------------------------------
	// ICollegue
	//--------------------------------------------------
	ICollegue::ICollegue(CollMgr* m, i3UIScene* scene, const char* name) : m_mgr(m), m_scene(scene), m_name(name)
	{

	}

	void ICollegue::OnClicked()
	{ 
		if (m_scene->isEnable()) (*m_mgr)(this); 
	}

	//--------------------------------------------------
	// 스타트 버튼
	//--------------------------------------------------
	Coll_StartBtn::Coll_StartBtn(CollMgr* m, i3UIScene* scene, const char* name) : ICollegue(m, scene, name)
	{
		m->RegisterColl(this, i3::bind(&CollMgr::_OnStartBtnClicked, m));
	}

	Coll_StartBtn::~Coll_StartBtn() {}

	void Coll_StartBtn::Enable()
	{
		LuaState* L = UILua::CallLuaFunction(m_scene, "EnableStartButton");
		i3Lua::PushBoolean(L, true);
		UILua::EndLuaFunction(L, 1);
	}

	void Coll_StartBtn::Disable()
	{
		LuaState* L = UILua::CallLuaFunction(m_scene, "EnableStartButton");
		i3Lua::PushBoolean(L, false);
		UILua::EndLuaFunction(L, 1);
	}

	bool Coll_StartBtn::IsEnable() const
	{
		return m_scene->isEnable() == 1;
	}

	//--------------------------------------------------
	// 모드 콤보박스
	//--------------------------------------------------
	Coll_ModeCombobox::Coll_ModeCombobox(CollMgr* m, i3UIScene* scene, const char* name) : ICollegue(m, scene, name)
	{
		m->RegisterColl(this, i3::bind(&CollMgr::_OnModeSelClicked, m));

		m_combobox = (i3UIComboBox*)m_scene->FindChildByName("i3UIComboBox_Mode");
		m_combobox->SetEditBoxReadOnly(true);

		i3::rc_wstring mode = GAME_RCSTRING("STBL_IDX_FASTADMISSION_MODE");
		mode += L";";

		m_dummyrule = new CSI_RULE;
		m_dummyrule->m_i32RuleIdx = -1;
		m_dummyrule->m_ui32Rule = 0;
		m_dummyrule->setRuleName(mode);
	}

	Coll_ModeCombobox::~Coll_ModeCombobox() 
	{
		I3_SAFE_DELETE(m_dummyrule);
	}

	void Coll_ModeCombobox::AddAllMode()
	{
		m_rules.clear();

		i3::wstring wstrModeList;

		m_rules.push_back(m_dummyrule);
		wstrModeList += m_dummyrule->getRuleName();

		i3::vector<CSI_RULE*>  temp_rule_list;
		i3::vector<CSI_RULE*>::iterator iter  = nullptr;
		for (INT i = 0; i < 2; i++)
		{
			bool is_custom = i == 0 ? false : true;
			temp_rule_list = CStageInfoUtil::GetRuleList(is_custom);
			iter = temp_rule_list.begin();
			for (iter; iter != temp_rule_list.end(); iter++)
			{
				CSI_STAGE_LIST temp;
				if ((*iter)->m_i32RuleIdx != RULE_ID_EVENT && (*iter)->m_i32RuleIdx <= CStageInfoUtil::GetServerRuleMaxIdx())
					temp = CStageInfoUtil::GetStageList((*iter)->m_i32RuleIdx);
				else										continue;

				if (temp.GetStageListSize() > 0)
					m_rules.push_back((*iter));
			}
		}

		for (size_t i=1; i<m_rules.size(); ++i)
		{
			i3::rc_wstring wname = m_rules[i]->getRuleName();
			wstrModeList.append(wname.begin(), wname.end());
			wstrModeList += L";";
		}

		m_scene->EnableScene(true);
		LuaState* L = UILua::CallLuaFunction(m_scene, "SetModeComboBox");
		i3Lua::PushStringUTF16To8(L, wstrModeList);
		UILua::EndLuaFunction(L, 1);
	}

	void Coll_ModeCombobox::AddModesByStage(const STAGE_UID stageUID)
	{
		i3::wstring wstrModeList;

		m_rules.clear();

		m_rules.push_back(m_dummyrule);
		wstrModeList += m_dummyrule->getRuleName();

		CStageInfoUtil::GatherRuleDatasAtUID(&m_rules, stageUID);
		
		for (size_t i=1; i<m_rules.size(); i++)
		{
			i3::rc_wstring wname;
			wname = m_rules[i]->getRuleName();
			wstrModeList.append(wname.begin(), wname.end());
			wstrModeList += L';';
		}

		m_scene->EnableScene(true);
		LuaState* L = UILua::CallLuaFunction(m_scene, "SetModeComboBox");
		i3Lua::PushStringUTF16To8(L, wstrModeList);
		UILua::EndLuaFunction(L, 1);
		
	}
	
	void Coll_ModeCombobox::SelectMode(const CSI_RULE& rule, bool select_dummy)
	{

		i3::vector<CSI_RULE*>::iterator it;
		for(it = m_rules.begin() ; it < m_rules.end(); ++it)
		{
			if (!select_dummy)
			{
				if ((*it)->m_i32RuleIdx == rule.m_i32RuleIdx)
					break;
			}
			else
			{
				if ((*it)->m_i32RuleIdx == m_dummyrule->m_i32RuleIdx)
					break;
			}
		}

		if (it == m_rules.end()) return;

		size_t pos = it - m_rules.begin();
		m_combobox->SetCurSel(static_cast<INT32>(pos));
		
	}

	CSI_RULE* Coll_ModeCombobox::GetCurrMode() const
	{
		if (m_rules.empty()) return m_dummyrule;

		INT32 sel = m_combobox->GetCurSel();

		if (sel != -1)
			return  m_rules[sel];

		return m_dummyrule;
	}
	
	//--------------------------------------------------
	// 스테이지 콤보박스
	//--------------------------------------------------
	Coll_StageCombobox::Coll_StageCombobox(CollMgr* m, i3UIScene* scene, const char* name) : ICollegue(m, scene, name)
	{
		m->RegisterColl(this, i3::bind(&CollMgr::_OnStageSelClicked, m));

		m_combobox = (i3UIComboBox*)m_scene->FindChildByName("i3UIComboBox_Stage");
		m_combobox->SetEditBoxReadOnly(true);
	}

	Coll_StageCombobox::~Coll_StageCombobox() 
	{
	}

	void Coll_StageCombobox::AddAllStages()
	{
		i3::wstring strStageList;

		m_stage_uids.clear();
		
		i3::rc_wstring stage = GAME_RCSTRING("STBL_IDX_FASTADMISSION_STAGE");
		stage += L";";

		m_stage_uids.push_back(STAGE_UID_NONE);
		strStageList += stage;

		for (int i = 1; i < STAGE_UID_MAX; ++i)
		{
			CSI_MAP* map_data = CStageInfoUtil::GetMapData((STAGE_UID)i);

			if ( map_data && map_data->isOpen )
				m_stage_uids.push_back((STAGE_UID)i);
		}

		for (size_t i=1; i<m_stage_uids.size(); ++i)
		{
			if (CStageInfoUtil::GetMapData((STAGE_UID)m_stage_uids.at(i))->getDisplayName().size() > 0)
			{
				strStageList += CStageInfoUtil::GetMapData((STAGE_UID)m_stage_uids.at(i))->getDisplayName();
				strStageList += L";";
			}
		}

		m_scene->EnableScene(true);
		LuaState* L = UILua::CallLuaFunction(m_scene, "SetStageComboBox");
		i3Lua::PushStringUTF16To8(L, strStageList);
		UILua::EndLuaFunction(L, 1);
	}
	
	void Coll_StageCombobox::AddStagesBydMode(const CSI_RULE* rule)
	{
		if (rule->m_i32RuleIdx == RULE_ID_EVENT) return;
		if (rule->m_i32RuleIdx > CStageInfoUtil::GetServerRuleMaxIdx()) return;

		// 모드에 해당하는 스테이지로 변경/저장.
		i3::wstring strStageList;

		m_stage_uids.clear();

		m_stage_uids.push_back(STAGE_UID_NONE);
		strStageList += L"STAGES;";

		CSI_STAGE_LIST stagelist = CStageInfoUtil::GetStageList(rule->m_i32RuleIdx);
		for (size_t i = 0; i < stagelist.GetStageListSize(); ++i)
			m_stage_uids.push_back(stagelist.GetStageData(i)->GetStageUID());

		for (size_t i = 1; i<m_stage_uids.size(); ++i)
		{
			strStageList += CStageInfoUtil::GetMapData(m_stage_uids.at(i))->getDisplayName();
			strStageList += L";";
		}

		m_scene->EnableScene(true);
		LuaState* L = UILua::CallLuaFunction(m_scene, "SetStageComboBox");
		i3Lua::PushStringUTF16To8(L, strStageList);
		UILua::EndLuaFunction(L, 1);
	}
	
	STAGE_UID Coll_StageCombobox::SelectStage(const STAGE_UID stageUID)
	{
		i3::vector<STAGE_UID>::const_iterator it = 
			i3::find(m_stage_uids.begin(), m_stage_uids.end(), stageUID);

		if (it == m_stage_uids.end()) return STAGE_UID_NONE;

		size_t pos = it - m_stage_uids.begin();
		m_combobox->SetCurSel(static_cast<INT32>(pos));

		return m_stage_uids[pos];
	}

	STAGE_UID Coll_StageCombobox::GetCurrStageUID() const
	{
		if (m_stage_uids.empty()) return STAGE_UID_NONE;

		INT32 sel = m_combobox->GetCurSel();

		if (sel != -1)
			return m_stage_uids[sel];

		return m_stage_uids[STAGE_UID_NONE];
	}

	//--------------------------------------------------
	// 이미지 박스
	//--------------------------------------------------
	Coll_Imgbox::Coll_Imgbox(CollMgr* m, i3UIScene* scene, const char* name) : ICollegue(m, scene, name) 
	{
		m->RegisterColl(this, i3::bind(&CollMgr::_None, m));
	}

	Coll_Imgbox::~Coll_Imgbox() {}

	void Coll_Imgbox::SetImage(const STAGE_UID uid)
	{
		
		INT32 idxImage = -1;
		idxImage = g_pFramework->GetMapShapeIdx(uid);

		m_scene->EnableScene(true);

		LuaState* L = UILua::CallLuaFunction(m_scene, "SetStageImage");
		i3Lua::PushInteger(L, idxImage);
		UILua::EndLuaFunction(L, 1);
		
	}


	//--------------------------------------------------
	// 중재자
	//--------------------------------------------------
	CollMgr::CollMgr(const INT32 sceneType) : m_sceneType(sceneType)
	{

	}

	CollMgr::~CollMgr()
	{
		i3::cu::for_each_delete_clear(m_colls);
	}

	void CollMgr::RegisterColl(ICollegue* coll, i3::function<> fn)
	{
		Coll* c = new Coll;
		c->coll = coll;
		c->fn = fn;

		m_colls.push_back(c);
	}

	void CollMgr::operator()(const ICollegue* coll)
	{
		i3::vector<Coll*>::const_iterator it = 
			std::find_if(m_colls.begin(), m_colls.end(), i3::bind2nd(find_coll(), coll));

		if (it != m_colls.end())
		{
			i3::function<> f = (*it)->fn;
			f();
		}
	}

	ICollegue* CollMgr::_GetColl(const char* collname) const
	{
		struct fn : i3::binary_function<const Coll*, const char*, bool>
		{
			bool operator()(const Coll* elem, const char* name) const
			{
				return elem->coll->GetName() == name;
			}
		};

		i3::vector<Coll*>::const_iterator it = 
			std::find_if(m_colls.begin(), m_colls.end(), i3::bind2nd(fn(), collname));

		if (it != m_colls.end()) return (*it)->coll;
		return nullptr;
	}
	
	void CollMgr::ConstructOnLoadScenes(const CSI_RULE& rule, const STAGE_UID stageUID)
	{
		Coll_ModeCombobox* modebox = static_cast<Coll_ModeCombobox*>(_GetColl("ModeCombobox"));
		Coll_StageCombobox* stagebox = static_cast<Coll_StageCombobox*>(_GetColl("StageCombobox"));
		Coll_Imgbox* imgbox = static_cast<Coll_Imgbox*>(_GetColl("Imgbox"));

		if (rule.m_i32RuleIdx == -1)
		{
			modebox->AddAllMode();
			modebox->SelectMode(rule, true);

			stagebox->AddAllStages();
			stagebox->SelectStage(STAGE_UID_NONE);
			imgbox->SetImage(STAGE_UID_NONE);
		}
		else
		{
			if (stageUID == STAGE_UID_NONE)
				modebox->AddAllMode();
			else
				modebox->AddModesByStage(stageUID);

			modebox->SelectMode(rule, rule.m_i32RuleIdx == -1 ? true : false);

			stagebox->AddStagesBydMode(&rule);
			stagebox->SelectStage(stageUID);

			imgbox->SetImage(stageUID);
		}

		RefreshStartBtn();
	}
	
	void CollMgr::RefreshStartBtn()
	{
		
		Coll_ModeCombobox* modebox = static_cast<Coll_ModeCombobox*>(_GetColl("ModeCombobox"));
		Coll_StageCombobox* stagebox = static_cast<Coll_StageCombobox*>(_GetColl("StageCombobox"));
		Coll_StartBtn* btn = static_cast<Coll_StartBtn*>(_GetColl("StartBtn"));

		const CSI_RULE*		currRule	 = modebox->GetCurrMode();
		const STAGE_UID&	currStage	 = stagebox->GetCurrStageUID();

		if (currRule->m_i32RuleIdx == -1 && currStage == STAGE_UID_NONE)
		{
			btn->Disable();
		}
		else
		{
			btn->Enable();
		}
		
	}

	void CollMgr::_None() {}

	void CollMgr::_OnStartBtnClicked()
	{
		Coll_StartBtn* btn = static_cast<Coll_StartBtn*>(_GetColl("StartBtn"));
		Coll_ModeCombobox* modebox = static_cast<Coll_ModeCombobox*>(_GetColl("ModeCombobox"));
		Coll_StageCombobox* stagebox = static_cast<Coll_StageCombobox*>(_GetColl("StageCombobox"));

		if (btn->IsEnable())
		{
			
			const CSI_RULE* m	= modebox->GetCurrMode();
			const STAGE_UID s	= stagebox->GetCurrStageUID();
			UINT32 rule			= m->m_ui32Rule;
			UINT32 stage_id		= rule + s;

			QuickJoinContext::i()->setQuickJoinInfo(m_sceneType)->m_ui32StageID = stage_id;
			GameEventSender::i()->SetEvent(EVENT_QUICKJOIN_START);
			
		}
	}

	void CollMgr::_OnModeSelClicked()
	{
	
		Coll_ModeCombobox* modebox = static_cast<Coll_ModeCombobox*>(_GetColl("ModeCombobox"));
		Coll_StageCombobox* stagebox = static_cast<Coll_StageCombobox*>(_GetColl("StageCombobox"));
		Coll_Imgbox* imgbox = static_cast<Coll_Imgbox*>(_GetColl("Imgbox"));

		const CSI_RULE* currRule = modebox->GetCurrMode();

		if (currRule->m_i32RuleIdx == -1)
		{
			modebox->AddAllMode();
			modebox->SelectMode(*currRule, true);

			stagebox->AddAllStages();
			stagebox->SelectStage(STAGE_UID_NONE);

			imgbox->SetImage(STAGE_UID_NONE);
		}
		else
		{
			const STAGE_UID prevStageUID = stagebox->GetCurrStageUID();

			stagebox->AddStagesBydMode(currRule);
			const STAGE_UID currStageUID = 
				stagebox->SelectStage(prevStageUID);
			imgbox->SetImage(currStageUID);
		}

		RefreshStartBtn();
		
	}

	void CollMgr::_OnStageSelClicked()
	{
	
		Coll_StageCombobox* stagebox = static_cast<Coll_StageCombobox*>(_GetColl("StageCombobox"));
		Coll_ModeCombobox* modebox = static_cast<Coll_ModeCombobox*>(_GetColl("ModeCombobox"));
		Coll_Imgbox* imgbox = static_cast<Coll_Imgbox*>(_GetColl("Imgbox"));

		const STAGE_UID currStageUID = stagebox->GetCurrStageUID();

		if (currStageUID == STAGE_UID_NONE)
		{
			modebox->AddAllMode();
			CSI_RULE temp;
			modebox->SelectMode(temp, true);

			stagebox->AddAllStages();
			stagebox->SelectStage(STAGE_UID_NONE);

			imgbox->SetImage(STAGE_UID_NONE);
		}
		else
		{
			const CSI_RULE* prevMode = modebox->GetCurrMode();

			modebox->AddModesByStage(currStageUID);
			modebox->SelectMode(*prevMode);
			imgbox->SetImage(currStageUID);
		}

		RefreshStartBtn();
		
	}
}