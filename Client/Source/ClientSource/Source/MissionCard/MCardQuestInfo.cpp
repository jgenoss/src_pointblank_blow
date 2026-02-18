#include "pch.h"
#include "MCardQuestInfo.h"




void	ResetMCardQuestInfo(MCardQuestInfo* info)
{
	info->m_QuestID = 0;
	info->m_ActionID = ACTION_ID_NA;
	info->m_StageID = STAGE_ID_NA;
	info->m_TargetCompleteCount = 0;

	info->m_Team = TEAM_NONE;			
	info->m_WeaponClassType = WEAPON_CLASS_UNKNOWN;
	info->m_WeaponNumber = 0;

	info->m_bAccum = false;
}
