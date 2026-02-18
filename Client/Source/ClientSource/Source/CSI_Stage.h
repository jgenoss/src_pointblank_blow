#pragma once

#ifndef __CSI_STAGE__
#define __CSI_STAGE__

struct CSI_STAGE : public MAP_STAGE_MATCHING , public i3::class_object_pool<CSI_STAGE>
{
	CSI_STAGE() { m_i32RuleIdx = 0; m_ui32StageID = 0, m_ui8Event = 0, m_ui8Mark = 0, m_ui8MaxPerson = 0; };
	CSI_STAGE(MAP_STAGE_MATCHING msm)
	{
		m_i32RuleIdx	= msm.m_i32RuleIdx;
		m_ui32StageID	= msm.m_ui32StageID;
		m_ui8Event		= msm.m_ui8Event;
		m_ui8Mark		= msm.m_ui8Mark;
		m_ui8MaxPerson  = msm.m_ui8MaxPerson;
	};

	const STAGE_MODE				GetStageMode()					{ return StageID::GetStageMode(this->m_ui32StageID);			};
	const WEAPON_RESTRICTION		GetWeaponRestriction()			{ return StageID::GetWeaponRestriction(this->m_ui32StageID);	};
	const AI_USE					GetAIMode()						{ return StageID::GetAIMode(this->m_ui32StageID);				};
	const STAGE_UID					GetStageUID()					{ return StageID::GetStageUID(this->m_ui32StageID);				};

	bool	IsSpecialWar()									{ return IsWeaponModeforWType(WEAPON_ALL) == false;				};				//특수전이란 개념은 일단 무기 관련 한거라고 일당 정의 함.
	bool	IsAiMode()										{ return GetAIMode() != AI_USE_NO;								};
	bool	IsClanMatchStage()								{ return StageID::IsClanMatchStage(this->m_ui32StageID);		};
	bool	IsAtkDefChangeStage()							{ return StageID::IsAtkDefChangeStage(this->m_ui32StageID);		};
	bool	IsHeadShotOnly()								{ return StageID::IsHeadShotOnly(this->m_ui32StageID);			};
	bool	IsOneShotOneKill()								{ return StageID::IsOneShotOneKill(this->m_ui32StageID);		};
	bool	IsHudOff()										{ return StageID::IsHudOff(this->m_ui32StageID);				};
	bool	IsSpace()										{ return StageID::IsSpaceMode(this->m_ui32StageID);				};
	bool	IsMultiWeapon()									{ return StageID::IsMultiWeaponSlot(this->m_ui32StageID);		};

	bool	IsEventforEventType(STAGE_EVENT e_type)			{ return m_ui8Event == e_type; };				
	bool	IsModeforModeType(STAGE_MODE m_type)			{ return GetStageMode() == m_type;			};
	bool	IsAiModeforAiType(AI_USE ai_type)				{ return GetAIMode() == ai_type;			};
	bool	IsWeaponModeforWType(WEAPON_RESTRICTION w_type) { return GetWeaponRestriction() == w_type;	};
};

#endif