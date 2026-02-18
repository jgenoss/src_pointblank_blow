#ifndef _DOMI_USER_SKILL_MGR_H_
#define _DOMI_USER_SKILL_MGR_H_

#include <unordered_map>

static const INT32 domiUserMaxCnt = 4;

class UserSkillLvRecoder
{
public:
	UserSkillLvRecoder();
	~UserSkillLvRecoder();

	void  SetLevel(INT32 userIdx, USER_SKILL type, INT32 lv);
	INT32 GetLevel(INT32 userIdx, USER_SKILL type) const;

private:
	class CurrentLevels
	{
	public:
		CurrentLevels() { ::memset(&levels, 0, sizeof(levels)); }
		void  SetValue(USER_SKILL type, INT32 lv);
		INT32 GetValue(USER_SKILL type) const;

	private:
		INT32 levels[USER_SKILL_COUNT];
	};

	CurrentLevels m_val[domiUserMaxCnt];

	UserSkillLvRecoder(const UserSkillLvRecoder& src);
	UserSkillLvRecoder& operator=(const UserSkillLvRecoder& src);
};

template<> struct is_tinst_manual<class DomiUserSkillMgr> : std::tr1::true_type {};
class DomiUserSkillMgr
{
public:
	typedef void (DomiUserSkillMgr::*fn)(CGameCharaBase*, INT32);

	static DomiUserSkillMgr* i() { return tinst<DomiUserSkillMgr>(); }

	DomiUserSkillMgr();
	~DomiUserSkillMgr();

	void ApplyDirect(INT32 customerIdx, USER_SKILL type, INT32 lv);
	void ApplyAtRespawn(INT32 customerIdx);

	void  SetLevel(INT32 customerIdx, USER_SKILL type, INT32 lv);
	INT32 GetLevel(INT32 customerIdx, USER_SKILL type) const;

private:
	void HpUp(CGameCharaBase* customer, INT32 lv);
	void DamageUp(CGameCharaBase* customer, INT32 lv);
	void SpeedUp(CGameCharaBase* customer, INT32 lv);
	void MagazineUp(CGameCharaBase* customer, INT32 lv);
	void WeaponSpecialEffectUp(CGameCharaBase* customer, INT32 lv);
	void MagazineReloadSpeedUp(CGameCharaBase* customer, INT32 lv);
	void GrenadeDamageUp(CGameCharaBase* customer, INT32 lv);
	void AggroDown(CGameCharaBase* customer, INT32 lv);
	void SpGainUp(CGameCharaBase* customer, INT32 lv);
	void SpGaugeUp(CGameCharaBase* customer, INT32 lv);
	void SpCostDown(CGameCharaBase* customer, INT32 lv);
	void HitDamageByDinoDown(CGameCharaBase* customer, INT32 lv);
	void EquipMeatBomb(CGameCharaBase* customer, INT32 lv);

private:
	i3::unordered_map<INT32, fn> m_mapApplyDirect;
	i3::unordered_map<INT32, fn> m_mapApplyAtRespawn;
	UserSkillLvRecoder m_lvRecoder;
};

#endif