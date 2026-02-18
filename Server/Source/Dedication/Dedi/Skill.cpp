#include "pch.h"
#include "Skill.h"

#include "SkillTable.h"

CDediSkill::CDediSkill()
: m_pSkillBase(NULL)
, m_i32SkillLevel(0)
{
}

CDediSkill::~CDediSkill()
{
}

REAL32 CDediSkill::GetValue(SKILL_TYPE eSkillType)
{
	if( !m_pSkillBase )
		return 0.0f;

	if( m_pSkillBase->m_eSkillType != eSkillType )
		return 0.0f;

	INT32 i32SkillLv =0;

	if(m_i32SkillLevel > 0)
	{
		i32SkillLv = m_i32SkillLevel-1;
	}

	return m_pSkillBase->m_ar32Value[i32SkillLv];
}

UINT32 CDediSkill::GetApplyWeaponSlot()
{
	return m_pSkillBase ? m_pSkillBase->m_ui32WeaponSlot : WEAPON_SLOT_UNKNOWN;
}

void CDediSkill::SetBaseData(CSkillInfo* pSkillBase)
{
	m_pSkillBase = pSkillBase;
}

void CDediSkill::SetSkillLevel(INT32 i32SkillLevel)
{
	m_i32SkillLevel = i32SkillLevel;
}
