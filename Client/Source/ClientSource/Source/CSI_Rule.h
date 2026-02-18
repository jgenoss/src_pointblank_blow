#pragma once

#ifndef __CSI_RULE__
#define __CSI_RULE__

struct CSI_RULE : public MAP_RULE_DATA, public i3::class_object_pool<CSI_RULE>
{
	i3::rc_wstring	wsRulename;

	CSI_RULE() { this->m_ui8DefaultStage = 0; this->m_ui32Rule = 0; this->m_i32RuleIdx = 0; this->wsRulename.clear(); };
	CSI_RULE(MAP_RULE_DATA mrd)
	{
		this->m_i32RuleIdx		= mrd.m_i32RuleIdx;
		this->m_ui32Rule		= mrd.m_ui32Rule;
		this->m_ui8DefaultStage = mrd.m_ui8DefaultStage;
		wsRulename.clear();
	};

	void setRuleName( i3::rc_wstring wsrulename )						{ this->wsRulename = wsrulename; }
	const i3::rc_wstring& getRuleName()									{ return (this->wsRulename); };
	
	UINT32 MakeDefaultStageID()
	{
		UINT32 ui32StageID = ((GetStageMode() & 0x3F) << 26 | (GetWeaponRestriction() & 0x1F) << 21 | (GetAIMode() & 0x0F) << 17 | IsClanMatchStage() << 16 | IsAtkDefChangeStage() << 15 | (m_ui8DefaultStage & 0xFF));
		return ui32StageID;
	};

	WEAPON_RESTRICTION	GetWeaponRestriction()							{	return static_cast<WEAPON_RESTRICTION>((m_ui32Rule >> 21) & 0x1F);	};
	STAGE_MODE			GetStageMode()									{	return static_cast<STAGE_MODE>((m_ui32Rule >> 26) & 0x3F);			};
	AI_USE				GetAIMode()										{	return static_cast<AI_USE>((m_ui32Rule >> 17) & 0x0F);				};
	bool				IsClanMatchStage()								{	return static_cast<bool>((m_ui32Rule >> 16) & 0x01);				};
	bool				IsAtkDefChangeStage()							{	return static_cast<bool>((m_ui32Rule >> 15) & 0x01);				};

	bool	IsSpecialWar()												{ return IsWeaponModeforWType(WEAPON_ALL) == false;		};				//특수전이란 개념은 일단 무기 관련 한거라고 일당 정의 함.
	
	bool	IsWeaponModeforWType(WEAPON_RESTRICTION w_type)				{ return GetWeaponRestriction() == w_type;				};
	bool	IsModeforModeType(STAGE_MODE m_type)						{ return GetStageMode() == m_type;						};
};

#endif