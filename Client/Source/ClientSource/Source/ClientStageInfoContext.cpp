#include "pch.h"
#include "ClientStageInfoContext.h"
#include "ClientStageInfoUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"

namespace
{
	void set_rule_data(CSI_RULE* target, UINT32 RuleIdx, UINT32 Rule, UINT8 DefaultStage, i3::rc_wstring& setRuleName)
	{
		target->m_i32RuleIdx	  = RuleIdx;
		target->m_ui32Rule		  = Rule;
		target->m_ui8DefaultStage = DefaultStage;
		target->setRuleName(setRuleName);
	}
}

ClientStageInfoContext::ClientStageInfoContext()
{
	m_nRuleMaxIdx		= 0;
	m_nServerRuleMaxIdx = 0;
}

ClientStageInfoContext::~ClientStageInfoContext()
{
	i3::cu::for_each_delete(m_vecRuleData);
}

/* seter */

void	ClientStageInfoContext::CreateMapDataAtPefData()
{
	i3RegKey * pPef = Config::_GetPefFileRegKey("Stage");
	i3RegKey * pRoot = Config::FindKey("Stage", pPef);

	INT32 cnt = pRoot->getChildCount();

	i3::rc_wstring		map_codename;
	i3::rc_wstring		map_displayname;
	INT32 stageuid;
	for (INT32 i = 0; i < cnt; ++i)
	{
		map_codename.clear();
		map_displayname.clear();
		stageuid = 0;

		i3RegKey * pKey = (i3RegKey*)pRoot->getChild(i);
		I3ASSERT(pKey != nullptr);

		i3::mb_to_utf16(pKey->GetName(), map_codename);
		FIND_REG_DATA(pKey, "_Name"		, map_displayname);
		FIND_REG_DATA(pKey, "StageUID"	, &stageuid);
		CSI_MAP mapdata( false , (UINT8)stageuid, map_codename, map_displayname);
		i3::vu::set_value_force(m_vecMapData, mapdata.ui8StageUID, mapdata);
	}
};

void	ClientStageInfoContext::AddRuleDataAtServerData(MAP_RULE_DATA* ruledata)
{
	CSI_RULE* c_rule = new CSI_RULE(*ruledata);
	c_rule->setRuleName(GAME_RCSTRING(ruledata->m_strRuleLabel));

	m_vecRuleData.push_back(c_rule);

	if( m_nRuleMaxIdx < c_rule->m_i32RuleIdx )
		m_nServerRuleMaxIdx = m_nRuleMaxIdx = c_rule->m_i32RuleIdx;
};

void	ClientStageInfoContext::AddStageListAtServerData(MAP_STAGE_MATCHING* matchingdata)
{
	CSI_STAGE c_stage(*matchingdata);
	
	STAGE_UID uid = c_stage.GetStageUID();
	if ( GetMapData(uid) == nullptr ) return;

	m_vecStageData.push_back(c_stage);

	if ( c_stage.IsEventforEventType(STAGE_EVENT_NORMAL) )
		m_hmmStageforRuleId.insert( i3::pair<INT32, CSI_STAGE>(c_stage.m_i32RuleIdx, c_stage) );
	else 
	if ( c_stage.IsEventforEventType(STAGE_EVENT_SEASON) )
		m_hmmStageofEventforRuleId.insert(i3::pair<INT32, CSI_STAGE>(c_stage.m_i32RuleIdx, c_stage));
};

void	ClientStageInfoContext::StrightRuleData()
{
	//룰과 스테이지 리스트를 서버로 부터 전부 다 받은 뒤 몇가지 처리를 해 줘야 한다.

	m_vecRuleDataCustom.clear();
	m_vecRuleDataMain.clear();

	i3::vector<CSI_RULE*> tempdata;
	i3::vector<CSI_RULE*>::iterator iter;
	for (iter = m_vecRuleData.begin(); iter < m_vecRuleData.end(); ++iter)
	{
		if (m_hmmStageforRuleId.find((*iter)->m_i32RuleIdx) != m_hmmStageforRuleId.end())
		{
			MakeMaxPersonRule(m_hmmStageforRuleId, tempdata, (*iter));
		}
		if (m_hmmStageofEventforRuleId.find((*iter)->m_i32RuleIdx) != m_hmmStageofEventforRuleId.end())
		{
			MakeMaxPersonRule(m_hmmStageofEventforRuleId, tempdata, (*iter));
		}

		tempdata.push_back(*iter);
	}

	m_vecRuleData.clear();
	m_vecRuleData.swap(tempdata);

	for (iter = m_vecRuleData.begin(); iter < m_vecRuleData.end(); iter++)
	{
		//튜토리얼은 노출되면 안되고 정보만 가져야 합니다.
		if (!((*iter)->IsModeforModeType(STAGE_MODE_TUTORIAL)))
			(*iter)->IsSpecialWar() ? m_vecRuleDataCustom.push_back(*iter) : m_vecRuleDataMain.push_back(*iter);
	}

	//맨 마지막에 이벤트용 룰을 넣어줍니다.
	CSI_STAGE_LIST temp_event_stage_list = CStageInfoUtil::GetStageListofEvent();
	if (!(temp_event_stage_list.GetStageListSize() <= (size_t)0))
	{
		i3::rc_wstring rule_name = GAME_RCSTRING("STBL_IDX_GAMEMODE_EVENTMODE");

		CSI_RULE* c_rule = new CSI_RULE();
		set_rule_data(c_rule, 0, 0, 0, rule_name);

		m_vecRuleData.push_back(c_rule);
		m_vecRuleDataMain.push_back(c_rule);
	}
}

void	ClientStageInfoContext::MakeMaxPersonRule(	rule_stage_data_mmap& selecte_hmmstage,
													i3::vector<CSI_RULE*>& target_v_rule, 
													CSI_RULE*	find_rule )
{
	//4vs4, 5vs5 등에 대한 룰을 추가 하여 만들어준다.
	int maked_rule[16] = { 0, };
	i3::vector < i3::pair<INT32, CSI_STAGE> > insert_data;
	
	iterator_rule_stage_data_pair	stage_iter = selecte_hmmstage.equal_range(find_rule->m_i32RuleIdx);
	for ( iterator_rule_stage_data_mmap	siter = stage_iter.first; siter != stage_iter.second; ++siter)
	{
		CSI_STAGE checking_stage = siter->second;
		if (checking_stage.m_ui8MaxPerson != 16 && checking_stage.m_ui8MaxPerson < 16)
		{
			if (maked_rule[checking_stage.m_ui8MaxPerson - 1] == 0)
			{
				maked_rule[checking_stage.m_ui8MaxPerson - 1] = ++m_nRuleMaxIdx;

				i3::string s_count;
				i3::itoa(checking_stage.m_ui8MaxPerson, s_count);
		
				i3::string label;
				label += "STBL_IDX_GAMEMODE_MAXPERSON";
				label += s_count;

				i3::rc_wstring set_rule_name;
				set_rule_name += find_rule->getRuleName();
				set_rule_name += L" - ";
				set_rule_name += GAME_RCSTRING(label);

				CSI_RULE* c_rule = new CSI_RULE();
				set_rule_data(c_rule, m_nRuleMaxIdx, find_rule->m_ui32Rule, find_rule->m_ui8DefaultStage, set_rule_name);
				target_v_rule.push_back(c_rule);
			}

			CSI_STAGE*	stage = const_cast<CSI_STAGE*>(GetStageData(checking_stage.m_ui32StageID));

			stage->m_i32RuleIdx			= maked_rule[checking_stage.m_ui8MaxPerson - 1];
			checking_stage.m_i32RuleIdx = maked_rule[checking_stage.m_ui8MaxPerson - 1];

			insert_data.push_back(i3::pair<INT32, CSI_STAGE>(checking_stage.m_i32RuleIdx, checking_stage));
			selecte_hmmstage.erase(siter);
		}
	}

	for each (const i3::pair<INT32, CSI_STAGE>& data in insert_data)
	{
		selecte_hmmstage.insert(data);
	}
}
/* seter */

/* geter */

const	iterator_rule_stage_data_pair	ClientStageInfoContext::GetStageList(INT32 ruleidx)
{
	iterator_rule_stage_data_pair  iter_stage = m_hmmStageforRuleId.equal_range(ruleidx);
	return iter_stage;
}

const	iterator_rule_stage_data_pair	ClientStageInfoContext::GetStageListofEvent()
{
	iterator_rule_stage_data_pair  iter_stage;
	iter_stage.first	= m_hmmStageofEventforRuleId.begin();
	iter_stage.second	= m_hmmStageofEventforRuleId.end();
	return iter_stage;
}

const	iterator_rule_stage_data_pair	ClientStageInfoContext::GetStageListofEvent(INT32 ruleidx)
{
	iterator_rule_stage_data_pair  iter_stage = m_hmmStageofEventforRuleId.equal_range(ruleidx);
	return iter_stage;
}

i3::vector< CSI_RULE* >&				ClientStageInfoContext::GetRuleLists(bool custom_list)
{
	if( custom_list )	return m_vecRuleDataCustom;
	else				return m_vecRuleDataMain;
};

const CSI_MAP*							ClientStageInfoContext::GetMapData(STAGE_UID stage_uid)
{
	CSI_MAP* resultmap = i3::find(m_vecMapData.begin(), m_vecMapData.end(), (UINT8)stage_uid);
	if (resultmap == m_vecMapData.end()) resultmap = nullptr;
	return resultmap;
}

const CSI_MAP*							ClientStageInfoContext::GetMapData(const char * map_code_name)
{
	i3::string     s_input_code_name(map_code_name);
	i3::rc_wstring ws_input_code_name;
	i3::utf8_to_utf16(s_input_code_name, ws_input_code_name);

	CSI_MAP* resultmap = i3::find(m_vecMapData.begin(), m_vecMapData.end(), ws_input_code_name);
	if (resultmap == m_vecMapData.end()) resultmap = nullptr;
	return resultmap;
}


void ClientStageInfoContext::SetRandomMapEventData(UINT32 n32Exp, UINT32 n32Point)
{
	m_nRandomMapEventExp = n32Exp; 
	m_nRandomMapEventPoint = n32Point; 

	i3::sprintf(m_RandomMapEventString, GAME_RCSTRING("STR_TBL_GUI_RANDOMMAP_EVENTICON_TEXT")
		, m_nRandomMapEventExp, m_nRandomMapEventPoint); //  경험치 %d %% 포인트 %d %% 
}

const	CSI_RULE*						ClientStageInfoContext::GetRuleData(INT32 rule_idx)
{
	i3::vector< CSI_RULE*>::iterator iter;
	for (iter = m_vecRuleData.begin(); iter < m_vecRuleData.end(); iter++)
	{
		if ((*iter)->m_i32RuleIdx == rule_idx)
			return (*iter);
	}
	return nullptr;
}

const	CSI_RULE*						ClientStageInfoContext::GetRuleData(UINT32 rule)
{
	//i3::vector< CSI_RULE*>::iterator iter;
	//for (iter = m_vecRuleData.begin(); iter < m_vecRuleData.end(); iter++)
	//{
	//	if ((*iter)->m_ui32Rule == rule)
	//		return (*iter);
	//}
	//return nullptr;

	/*
		기존 코드(위 주석처리)
		m_vecRuleData의 룰데이터가 룰값이 같지만 룰인덱스가 다른 룰들이 있는데
		기존 룰데이터의 벡터의 값이 아래와 같이 이루어져 순서대로 찾을시 원하는 룰인덱스를 찾지못하는 경우 발생,
		빠른입장에서 해당 함수를 쓰기에 정확한 룰 인덱스값을 얻기위해 역순으로 찾도록 변경함.
		
		ex)
		m_vecRuleData[0] 모드 = 단체전4vs4	룰인덱스 = 49	룰값 = 6417440
		m_vecRuleData[1] 모드 = 단체전		룰인덱스 = 1   	룰값 = 6417440 <- 찾고자하는 룰인덱스값
		
		16/11/08 김민제
	*/

	i3::vector< CSI_RULE*>::reverse_iterator riter;
	for (riter = m_vecRuleData.rbegin(); riter < m_vecRuleData.rend(); ++riter)
	{
		if ((*riter)->m_ui32Rule == rule)
			return (*riter);
	}
	return nullptr;
}

const	CSI_STAGE*						ClientStageInfoContext::GetStageData(STAGE_ID stageid)
{
	i3::vector< CSI_STAGE>::iterator iter;
	for (iter = m_vecStageData.begin(); iter < m_vecStageData.end(); iter++)
	{
		if ((iter)->m_ui32StageID == stageid)
			return (iter);
	}
	return nullptr;
}

/* geter */