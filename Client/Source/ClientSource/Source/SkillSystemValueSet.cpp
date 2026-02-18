#include "pch.h"
#include "GlobalVariables.h"
#include "SkillSystemValueSet.h"

#define			FILE_NAME_DESIGNATION_VALUE		"Config/Skill.pef"
SkillSystemValueSet * g_pSkillSystemValueSet = nullptr;

NCHARA_SKILL::NCHARA_SKILL(void)
{
}
NCHARA_SKILL::~NCHARA_SKILL(void)
{

}

void	NCHARA_SKILL::Resetdata(void)
{

	_ShiftCode = (UINT8)((i3Math::Rand() % 7) + 1);
	_CurrentLevel = 0;
	_Category = CATEGORYLIST_UNKNOWN;
	_SkillType = SKILLLIST_UNKNOWN;
	_RequisiteType = REQUISTE_UNKNOWN;
	for(INT32 i = 0; i <SkillMaxLevel; i ++)
		_RequisiteValue[i] = 0;
}

SKILL_CATEGORYLIST	NCHARA_SKILL::getApplyCategory(void)
{ 
	return (SKILL_CATEGORYLIST) DECRYPT_INTVALUE( _Category, _ShiftCode);
}

void NCHARA_SKILL::setApplyCategory(SKILL_CATEGORYLIST category)
{
	GlobalFunc::EncryptValue( (INT32*)&category, _ShiftCode);

	_Category = category;
}


SKILL_TYPE NCHARA_SKILL::getApplySkill(void)
{
	return (SKILL_TYPE) DECRYPT_INTVALUE( _SkillType, _ShiftCode);
}

void NCHARA_SKILL::setApplySkill(SKILL_TYPE skill)
{
	GlobalFunc::EncryptValue( (INT32*)&skill, _ShiftCode);

	_SkillType = skill;
}


REQUISTE NCHARA_SKILL::getApplyRequisiteType(void)
{
	return (REQUISTE) DECRYPT_INTVALUE( _RequisiteType, _ShiftCode);
}

void NCHARA_SKILL::setApplyRequisiteType(REQUISTE type)
{
	GlobalFunc::EncryptValue( (INT32*)&type, _ShiftCode);

	_RequisiteType = type;
}


INT32 NCHARA_SKILL::getApplyRequisiteValue(INT32 idx)
{
	return DECRYPT_INTVALUE( _RequisiteValue[idx-1], _ShiftCode);
}

INT32 NCHARA_SKILL::getApplyRequisiteLevel(INT32 _value)
{
	INT32 currentLevel = -1;

	for(INT32 i = 1; i <= SkillMaxLevel; i++)
	{
		INT32 ilvValue = getApplyRequisiteValue(i);

		if( ilvValue < 0 )
			continue;

		if( _value >= ilvValue)
			currentLevel = i;
	}

	return currentLevel;
}

void NCHARA_SKILL::setApplyRequisiteValue(INT32 idx, INT32 vallue)
{
	GlobalFunc::EncryptValue( &vallue, _ShiftCode);

	_RequisiteValue[idx] = vallue;
}


bool NCHARA_SKILL::isWeaponType(CHARA_CLASS mainskill)
{
	SKILL_WEAPONTYPE tempweapon= SKILL_WEAPONTYPE_UNKNOWN;
	switch(mainskill)
	{
	case CHARA_CLASS_ASSAULT:
		tempweapon = SKILL_WEAPONTYPE_ASSAULT;
		break;
	case CHARA_CLASS_SMG:
		tempweapon = SKILL_WEAPONTYPE_SMG;
		break;
	case CHARA_CLASS_SNIPER:
		tempweapon = SKILL_WEAPONTYPE_RIFLE;
		break;
	case CHARA_CLASS_MACHINE:
		tempweapon = SKILL_WEAPONTYPE_MG;
		break;
	case CHARA_CLASS_SHOTGUN:
		tempweapon = SKILL_WEAPONTYPE_SHOTGUN;
		break;
	}
	return (iWeaponType == tempweapon);
}


I3_CLASS_INSTANCE( SkillSystemValueSet);

SkillSystemValueSet::SkillSystemValueSet(void)
{
	g_pSkillSystemValueSet = this;

	m_encryptShiftCode = (UINT8)((i3Math::Rand() % 7) + 1);

	// 초기값을 설정합니다.
	InitDefault();

	// 저장된값을 로드합니다.
	Load();

}

SkillSystemValueSet::~SkillSystemValueSet(void)
{
}

void SkillSystemValueSet::InitDefault(void)
{
	INT32 i = 0;
	INT32 imainidx = 0;

	m_SkillDataSet._ShiftCode = (UINT8)((i3Math::Rand() % 7) + 1);
	

	for( i = 0; i < MAX_COMMON_SKILL; i++)
	{
		m_SkillDataSet.CommonSkill[i].Resetdata();
	}
	for( i = 0; i < MAINCATEGORY_MAX; i++)
	{
		for(INT32 j = 0; j < MaxMainSkill; j++)
		{
			imainidx = (i * MaxMainSkill) + j;
			m_SkillDataSet.MainSkill[imainidx].Resetdata();
		}
	}
	for( i = 0; i < MAX_ASSIST_SKILL; i++)
	{
		m_SkillDataSet.AssistSkill[i].Resetdata();
	}
}

void SkillSystemValueSet::Load(void)
{
	INT32 _Value = 0;
	i3RegKey * pRootKey = Config::_GetRegRoot("Skill/SkillData");
	INT32 iCategory = 0;

	for(INT32 i=0; i<pRootKey->getChildCount(); i++)
	{
		//SkillList Value Load
		i3RegKey* pChildRegKey1 = (i3RegKey*)pRootKey->getChild(i);

		FIND_REG_DATA(pChildRegKey1, LS_STR("Category"), &_Value);
		iCategory = _Value;

		for(INT32 j=0; j<pChildRegKey1->getChildCount(); j++)
		{
			//Skill_1 Value Load
			i3RegKey* pChildRegKey2 = (i3RegKey*)pChildRegKey1->getChild(j);
			INT32 index = 0;

			FIND_REG_DATA(pChildRegKey2, "_Index", &index);
			FIND_REG_DATA(pChildRegKey2, LS_STR("_WeaponIndex"), &_Value);
			Category_Load(pChildRegKey2, (SKILL_CATEGORYLIST)iCategory, index, _Value);
		}
	}
}

NCHARA_SKILL * SkillSystemValueSet::Category_Load(i3RegKey* Regkey, SKILL_CATEGORYLIST category, INT32 index, INT32 weapontype)
{
	INT32 _Value = 0;
	REAL32 lvvalue = 0;
	char szTableName[ MAX_STRING_COUNT] = {0};
	NCHARA_SKILL * skillinfo = nullptr;


	switch( category)
	{
	case CATEGORYLIST_COMMON:
		skillinfo = &m_SkillDataSet.CommonSkill[index];
		break;

	case CATEGORYLIST_MAIN:
		skillinfo = &m_SkillDataSet.MainSkill[index];
		break;
	case CATEGORYLIST_ASSIST:
		skillinfo = &m_SkillDataSet.AssistSkill[index];
		break;

	default:
	{
		I3PRINTLOG(I3LOG_FATAL, "error: Skill Data");
		return nullptr;
	}
		break;
	}

	for(INT32 lv=0; lv<SkillMaxLevel; lv++)
	{
		sprintf_s( szTableName, "ValueLv_%d", lv);
		FIND_REG_DATA(Regkey, szTableName, &lvvalue);
		skillinfo->setApplyValue(lv, lvvalue);

		sprintf_s( szTableName, "RequisiteLv_%d", lv);
		FIND_REG_DATA(Regkey, szTableName, &lvvalue);
		skillinfo->setApplyRequisiteValue(lv, static_cast<INT32>(lvvalue) );
	
	}


	skillinfo->setApplyCategory((SKILL_CATEGORYLIST)category);
	skillinfo->setApplyRequisiteType((REQUISTE)_Value);

	FIND_REG_DATA(Regkey, "RequisiteType", &_Value);
	skillinfo->setApplyRequisiteType((REQUISTE)_Value);

	FIND_REG_DATA(Regkey, LS_STR("SkillType"), &_Value);
	skillinfo->setApplySkill((SKILL_TYPE)_Value);

	skillinfo->setApplyWeaponType((SKILL_WEAPONTYPE)weapontype);

	return skillinfo;

}

NCHARA_SKILL * SkillSystemValueSet::getSkillsystemSet(SKILL_CATEGORYLIST category, INT32 index)
{
	NCHARA_SKILL * skillinfo = nullptr;


	if( index > -1)
	{
		switch( category)
		{
		case CATEGORYLIST_COMMON:
			I3_BOUNDCHK( index, MAX_COMMON_SKILL);
			if( index < MAX_COMMON_SKILL)
				skillinfo = &m_SkillDataSet.CommonSkill[index];
			break;
		case CATEGORYLIST_MAIN:
			I3_BOUNDCHK( index, MAX_MAIN_SKILL);
			if( index < MAX_MAIN_SKILL)
				skillinfo = &m_SkillDataSet.MainSkill[index];
			break;
		case CATEGORYLIST_ASSIST:
			I3_BOUNDCHK( index, MAX_ASSIST_SKILL);
			if( index < MAX_ASSIST_SKILL)
				skillinfo = &m_SkillDataSet.AssistSkill[index];
			break;
		default:
			I3ASSERT("error: Skill Data");
			break;
		}
	}

	return skillinfo;
}

// 여기서 스킬 제한을 푸는 함수를 만들어 둡니다.
void SkillSystemValueSet::RestrictSkill(void)
{
	NCHARA_SKILL * skillinfo = nullptr;
	INT32 idx[3];
	idx[0] = MAX_COMMON_SKILL;
	idx[1] = MAX_MAIN_SKILL;
	idx[2] = MAX_ASSIST_SKILL;

	for (INT32 i = 0; i < CATEGORYLIST_MAX; i++)
	{
		for (INT32 categoryidx = 0; categoryidx < idx[i]; categoryidx++)
		{
			switch (i)
			{
			case CATEGORYLIST_COMMON:
				I3_BOUNDCHK(categoryidx, MAX_COMMON_SKILL);
				if (categoryidx < MAX_COMMON_SKILL)
					skillinfo = &m_SkillDataSet.CommonSkill[categoryidx];
				break;
			case CATEGORYLIST_MAIN:
				I3_BOUNDCHK(categoryidx, MAX_MAIN_SKILL);
				if (categoryidx < MAX_MAIN_SKILL)
					skillinfo = &m_SkillDataSet.MainSkill[categoryidx];
				break;
			case CATEGORYLIST_ASSIST:
				I3_BOUNDCHK(categoryidx, MAX_ASSIST_SKILL);
				if (categoryidx < MAX_ASSIST_SKILL)
					skillinfo = &m_SkillDataSet.AssistSkill[categoryidx];
				break;
//			default:
//				I3ASSERT("error: Skill Data");
//				break;
			}

			for (INT32 lv = 0; lv < SkillMaxLevel; lv++)
			{
				if (skillinfo->getApplyRequisiteValue(lv + 1) == -1)
					continue;
				// -1 이 설정되어 있지 않으면 0으로 강제 세팅합니다.
				skillinfo->setApplyRequisiteValue(lv, 0);
			}
		}
	}
}