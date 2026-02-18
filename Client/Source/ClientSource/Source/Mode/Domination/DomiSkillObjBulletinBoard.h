#ifndef _DOMI_SKILL_OBJ_BULLETINBOARD_H_
#define _DOMI_SKILL_OBJ_BULLETINBOARD_H_

#include "DominationSkillObject.h"

class CUIHudDomi_Skill;
// 변경가능한 상태를 표시(설치, 업그레이드, 수리, 사용 등)
template <> struct is_tinst_manual<class DomiSkillObjBulletinBoard> : std::tr1::true_type {};
class DomiSkillObjBulletinBoard
{
public:
	typedef void (DomiSkillObjBulletinBoard::*fn)(CGameCharaBase*, dss::CServiceable*);

	static DomiSkillObjBulletinBoard* i() { return tinst<DomiSkillObjBulletinBoard>(); }

	DomiSkillObjBulletinBoard();
	~DomiSkillObjBulletinBoard();

	void Show(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Hide();

private:
	void None(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Setup(CGameCharaBase* customer, dss::CServiceable* targetObj); // setup이 가능한 상황, E 키와 요구 SP를 출력
	void Upgrade(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Fix(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseHP(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseAMMO(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void OpenSkillShop(CGameCharaBase* customer, dss::CServiceable* targetObj);

private:
	fn m_funcPtrs[DominationSkill::SERVICEABLE_STATE_MAX];
	CUIHudDomi_Skill* m_noticeHUD;
	i3::wstring m_objectName;
};

namespace SkillObjBulletinBoardUtil
{
	bool NeedToSupplyAmmo(WeaponBase* wp, CGameCharaBase* customer);
}

#endif
