#ifndef _DOMINATION_USER_SKILL_APPLY_H_
#define _DOMINATION_USER_SKILL_APPLY_H_

#include <unordered_map>

extern const REAL32 g_domiSkillLvIncrRatio[USER_SKILL_COUNT][MAX_LEVEL_USERSKILL+1];

class UserSkillApply
{
public:
	typedef void (UserSkillApply::*fn)(CGameCharaBase*, INT8);

	UserSkillApply();
	~UserSkillApply();

	void ApplyDirect(INT8 customerIdx, INT8 skillType, INT8 lv);
	void ApplyAtRespawn(INT8 customerIdx);

private:
	void HpUp(CGameCharaBase* customer, INT8 lv);
	void DamageUp(CGameCharaBase* customer, INT8 lv);
	void SpeedUp(CGameCharaBase* customer, INT8 lv);
	void MagazineUp(CGameCharaBase* customer, INT8 lv);

private:
	std::tr1::unordered_map<INT8, fn> m_map;
};

#endif