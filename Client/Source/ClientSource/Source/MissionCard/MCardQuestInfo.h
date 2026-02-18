#pragma once


struct MCardQuestInfo
{
	QUEST_ID			m_QuestID = 0;								// 기존에 굉장이 이상하게 쓰이고 있음..(서버입출력용)
	ACTION_ID			m_ActionID = ACTION_ID_NA;					// 기존에 enum 정리는 가독성이 높지만, 체계에 문제 있음..(서버입출력용)

	STAGE_ID			m_StageID = 0;								// 없을수 있음..
	INT32				m_TargetCompleteCount = 0;	//
	TEAM_TYPE			m_Team = TEAM_NONE;							// Red or Blue or Any
	WEAPON_CLASS_TYPE	m_WeaponClassType = WEAPON_CLASS_UNKNOWN;	// 무기종(없을수 있음)
	INT32				m_WeaponNumber = 0;							// 무기넘버(없을수 있음)

	bool				m_bAccum = false;							// 0 : 연속 1: 누적..	
};

void ResetMCardQuestInfo(MCardQuestInfo* info);

