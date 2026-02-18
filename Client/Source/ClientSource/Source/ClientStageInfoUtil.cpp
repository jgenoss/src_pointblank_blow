#include "pch.h"
#include "ClientStageInfoContext.h"

#include "ClientStageInfoUtil.h"

#include "../GameGUI.h"

#include "UserInfoContext.h"

namespace CStageInfoUtil
{

	void	CreateCSIContextMapData()
	{
		ClientStageInfoContext::i()->CreateMapDataAtPefData();
	};

	void	AddRuleDatatoCSICotext(MAP_RULE_DATA* ruledata)
	{
		ClientStageInfoContext::i()->AddRuleDataAtServerData(ruledata);
	};

	void	AddStageListtoCSICotext(MAP_STAGE_MATCHING* matchingdata)
	{
		ClientStageInfoContext::i()->AddStageListAtServerData(matchingdata);
	};



	i3::vector< CSI_RULE* >&		GetRuleList(bool custom_list)
	{
		return ClientStageInfoContext::i()->GetRuleLists(custom_list);
	}

	CSI_STAGE_LIST					GetStageList(INT32 ruleidx)
	{
		CSI_STAGE_LIST s_list;
		s_list.CreateStageList(ClientStageInfoContext::i()->GetStageList(ruleidx));
		return s_list;
	};

	CSI_STAGE_LIST					GetStageListofEvent()
	{
		CSI_STAGE_LIST s_list;
		s_list.CreateStageList(ClientStageInfoContext::i()->GetStageListofEvent());
		return s_list;
	};

	CSI_STAGE_LIST					GetStageListofEvent(INT32 ruleidx)
	{
		CSI_STAGE_LIST s_list;
		s_list.CreateStageList(ClientStageInfoContext::i()->GetStageListofEvent(ruleidx));
		return s_list;
	};

	CSI_MAP*						GetMapData(STAGE_UID stage_uid)
	{
		return const_cast<CSI_MAP*>(ClientStageInfoContext::i()->GetMapData(stage_uid));
	};

	CSI_MAP*						GetMapData(STAGE_ID stageid)
	{
		CSI_STAGE temp_stage; temp_stage.m_ui32StageID = stageid;
		return GetMapData(temp_stage.GetStageUID());
	}
	CSI_MAP*						GetMapData(const char * map_code_name)
	{
		return const_cast<CSI_MAP*>(ClientStageInfoContext::i()->GetMapData(map_code_name));
	}
	;

	CSI_RULE*						GetRuleData(INT32 ruleidx)
	{
		return const_cast<CSI_RULE*>(ClientStageInfoContext::i()->GetRuleData(ruleidx));
	};

	CSI_RULE*						GetRuleData(UINT32 rule)
	{
		return const_cast<CSI_RULE*>(ClientStageInfoContext::i()->GetRuleData((UINT32)rule));
	};

	CSI_RULE*						GetRuleData(STAGE_ID stageid, bool isStageid)
	{
		if (isStageid)
		{
			if (GetStageData(stageid))
				return	GetRuleData(GetStageData(stageid)->m_i32RuleIdx);
			else
				return  nullptr;
		}
		else
		{
			return				const_cast<CSI_RULE*>(ClientStageInfoContext::i()->GetRuleData((UINT32)stageid));
		}
	};

	CSI_STAGE*						GetStageData(STAGE_ID stageid)
	{
		return const_cast<CSI_STAGE*>(ClientStageInfoContext::i()->GetStageData(stageid));
	}

	const INT32 GetServerRuleMaxIdx()
	{
		return ClientStageInfoContext::i()->GetServerRuleMaxIdx();
	}

	void					GatherRuleDatasAtUID(i3::vector< CSI_RULE*>* vec, STAGE_UID stage_uid)
	{
		for (int i = 0; i < 2; i++)
		{
			bool is_custom = (i != 0);
			const i3::vector< CSI_RULE* > rule_list = GetRuleList(is_custom);
			for (size_t rule_idx = 0; rule_idx < rule_list.size(); rule_idx++)
			{
				INT32 find_rule_idx = rule_list.at(rule_idx)->m_i32RuleIdx;
				if (find_rule_idx == RULE_ID_EVENT)	continue;
				if (find_rule_idx > GetServerRuleMaxIdx()) continue;

				CSI_RULE*		find_rule = CStageInfoUtil::GetRuleData(find_rule_idx);
				CSI_STAGE_LIST	find_stage_list = GetStageList(find_rule_idx);
				CSI_STAGE*		find_stage = find_stage_list.GetStageData((STAGE_UID)stage_uid);
				if (find_stage != nullptr)
					vec->push_back(find_rule);
			}
		}
	}

	STAGE_SLOT_MODE	GetSlotMode(CSI_STAGE stage)
	{
		if (stage.m_ui8MaxPerson == 8)	return STAGE_SLOT_MODE_4VS4;
		if (stage.m_ui8MaxPerson == 10)	return STAGE_SLOT_MODE_5VS5;

		return STAGE_SLOT_MODE_NORMAL;
	}

	bool	IsAvailableStageMode(STAGE_MODE stagemode)
	{
		i3::vector<CSI_RULE*> list = (ClientStageInfoContext::i()->GetRuleLists());
		i3::vector<CSI_RULE*>::iterator iter;
		for (iter = list.begin(); iter < list.end(); iter++)
			if ((*iter)->IsModeforModeType(stagemode)) return true;

		return false;
	}

	bool	IsStageEventforStageId(STAGE_ID stageid, STAGE_EVENT eventtype)
	{
		return GetStageData(stageid)->IsEventforEventType(eventtype);
	}

	bool	IsDiableChaneMode(CSI_STAGE stage)
	{
		if (stage.IsAiMode())		return false;
		if (stage.IsSpecialWar())	return false;

		return true;
	};

	bool	IsDiableChaneMode(CSI_RULE  rule)
	{
		CSI_STAGE temp;	temp.m_ui32StageID = rule.m_ui32Rule;
		return IsDiableChaneMode(temp);
	};

	bool	IsModeRoundType(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_BOMB))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_ANNIHILATION))		return true;
		if (stage.IsModeforModeType(STAGE_MODE_DESTROY))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_DEFENCE))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_ESCAPE))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_CROSSCOUNT))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_CONVOY))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_RUN_AWAY))			return true;

		return false;
	}

	bool	IsRoundTypeandEternerlRespawn(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_DESTROY))		return true;
		if (stage.IsModeforModeType(STAGE_MODE_DEFENCE))		return true;
		if (stage.IsModeforModeType(STAGE_MODE_ESCAPE))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_CROSSCOUNT))		return true;

		return false;
	};

	bool	IsEternerlRespawn(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_DEATHMATCH))				return true;
		if (stage.IsHeadShotOnly())/*헤드헌터*/							return true;
		if (stage.IsHudOff() && stage.IsOneShotOneKill())/*카오스모드*/	return true;

		return IsRoundTypeandEternerlRespawn(stage);
	}

	bool	IsFixedRoundTypeMode(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_ESCAPE))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_CROSSCOUNT))		return true;

		return false;
	};

	bool	IsUsingMissionObject(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_DESTROY))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_ESCAPE))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_DEFENCE))			return true;

		return false;
	}

	bool	IsDeletableDroppedWeapon(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_DEATHMATCH))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_DESTROY))			return true;
		if (stage.IsModeforModeType(STAGE_MODE_ESCAPE))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_DEFENCE))			return true;
		if (stage.IsHeadShotOnly())/*헤드헌터*/						return true;
		if (stage.IsModeforModeType(STAGE_MODE_CROSSCOUNT))			return true;

		return false;
	}

	bool	IsObserverModeStageByOnlyMode(CSI_STAGE stage)
	{
		//순서 중요
		if (IsRoundTypeandEternerlRespawn(stage)) return false;
		else if (IsModeRoundType(stage))		  return true;
		else									  return false;
	}

	bool	IsObserverModeStage(CSI_STAGE stage)
	{
#if defined( DEF_OBSERVER_MODE)
		if (BattleSlotContext::i()->isObserverMe())	return true;
#else
		if (UserInfoContext::i()->IsGM_Observer()) {
			return true;
		}
#endif
		if (IsObserverModeStageByOnlyMode(stage)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool	IsKillMode(CSI_STAGE stage)
	{
		if (stage.IsModeforModeType(STAGE_MODE_CROSSCOUNT))				return true;
		if (stage.IsModeforModeType(STAGE_MODE_DEATHMATCH))				return true;

		if (IsModeRoundType(stage))	return false;

		return false;
	}

	UINT8	getMissionRoundType(CSI_STAGE stage)
	{
		UINT8 SubType = 0;
		//이런일이 일어나면 안된다.
		if (stage.GetStageMode() == STAGE_MODE_NA) return 0;

		switch (stage.GetStageMode())
		{
		case STAGE_MODE_ANNIHILATION:
#if defined( LOCALE_NORTHAMERICA )
			SubType = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_5;
			break;
#endif
		case STAGE_MODE_BOMB:
		case STAGE_MODE_CONVOY:
		case STAGE_MODE_RUN_AWAY:		SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break;
		case STAGE_MODE_DESTROY:
		case STAGE_MODE_DEFENCE:		SubType = BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break;
		case STAGE_MODE_ESCAPE:			SubType = BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_2;		break;
		case STAGE_MODE_CROSSCOUNT:		SubType = BATTLE_TIME_TYPE_5 | BATTLE_ROUND_TYPE_2;				break;
		default:	// 기본단체전 스타일
			SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
			break;
		}

		return SubType;
	}

};