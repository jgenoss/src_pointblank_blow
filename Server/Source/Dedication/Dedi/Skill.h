#pragma once

class CSkillInfo;

class CDediSkill
{
public:
	REAL32				GetValue(SKILL_TYPE eSkillType);
	UINT32				GetApplyWeaponSlot();

	void				SetBaseData(CSkillInfo* pSkillBase);
	void				SetSkillLevel(INT32 i32SkillLevel);

public:
	CDediSkill();
	virtual ~CDediSkill();

private:
	CSkillInfo*			m_pSkillBase;
	INT32				m_i32SkillLevel;
};
