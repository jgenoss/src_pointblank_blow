#ifndef _SKILLSYSTEM_VALUE_SET_H__
#define _SKILLSYSTEM_VALUE_SET_H__

#include "ClientDef.h"


#define SkillMaxLevel 10
#define MaxCommonSkill 5
#define MaxMainSkill 4


class NCHARA_SKILL
{

public:
	UINT8				_ShiftCode;
	INT32				_CurrentLevel;							//	현재레벨

private:
	SKILL_CATEGORYLIST	_Category;								//	카테고리 IDX
	SKILL_TYPE			_SkillType;								//	능력치 종류
	REQUISTE			_RequisiteType;							//	레벨업조건
	INT32				_RequisiteValue[SkillMaxLevel];			//	레벨업 조건값

public:
	NCHARA_SKILL(void);
	~NCHARA_SKILL(void);
	
	void Resetdata(void);
	SKILL_CATEGORYLIST	getApplyCategory(void);
	void				setApplyCategory(SKILL_CATEGORYLIST category);

	SKILL_TYPE			getApplySkill(void);
	void				setApplySkill(SKILL_TYPE skill);

	REQUISTE			getApplyRequisiteType(void);
	void				setApplyRequisiteType(REQUISTE type);

	INT32				getApplyRequisiteValue(INT32 idx);
	void				setApplyRequisiteValue(INT32 idx, INT32 vallue);
	INT32				getApplyRequisiteLevel(INT32 _value);

private:
	SKILL_WEAPONTYPE	iWeaponType;							//	총기 타입
	REAL32 				iValue[SkillMaxLevel];					//	능력치값

public:
	SKILL_WEAPONTYPE	getApplyWeaponType(void){ return iWeaponType;}
	void				setApplyWeaponType(SKILL_WEAPONTYPE _type){ iWeaponType = _type; }

	REAL32				getApplyValue(INT32 _idx){ return iValue[_idx-1];}
	void				setApplyValue(INT32 _idx, REAL32 _vallue){ iValue[_idx] = _vallue;}
	
public:
	//UTIL
	bool				isWeaponType(CHARA_CLASS mainskill);


};

struct SKILL_DATA
{
	UINT8	_ShiftCode;

	NCHARA_SKILL			CommonSkill[MAX_COMMON_SKILL];
	NCHARA_SKILL			MainSkill[MAX_MAIN_SKILL];
	NCHARA_SKILL			AssistSkill[MAX_ASSIST_SKILL];
};

class SkillSystemValueSet :	public i3ElementBase
{
	I3_CLASS_DEFINE( SkillSystemValueSet, i3ElementBase );

public:
	SkillSystemValueSet(void);
	~SkillSystemValueSet(void);

	void InitDefault(void);
	void Save(void);
	void Load(void);
	void RestrictSkill(void);
	NCHARA_SKILL * Category_Load(i3RegKey* Regkey, SKILL_CATEGORYLIST category, INT32 index, INT32 weapontype);
	NCHARA_SKILL * getSkillsystemSet(SKILL_CATEGORYLIST category, INT32 index);
private:
	SKILL_DATA		m_SkillDataSet;
	UINT8			m_encryptShiftCode;

protected:


};

extern SkillSystemValueSet * g_pSkillSystemValueSet;

#endif //_SKILLSYSTEM_VALUE_SET_H__