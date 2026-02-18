#ifndef _SKILL_NOTICE_H_
#define _SKILL_NOTICE_H_

#include "DominationSkillObject.h"
#include <unordered_map>

class CUIHudDomi_Skill;
// 변경가능한 상태를 표시(설치, 업그레이드, 수리, 사용 등)
class SkillNotice
{
public:
	typedef void (SkillNotice::*fn)(CGameCharaBase*, dss::CServiceable*);

	SkillNotice();
	~SkillNotice();

	void Show(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Hide();

private:
	void Setup(CGameCharaBase* customer, dss::CServiceable* targetObj); // setup이 가능한 상황, E 키와 요구 SP를 출력
	void Upgrade(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Fix(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseHP(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseAMMO(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void OpenSkillShop(CGameCharaBase* customer, dss::CServiceable* targetObj);

private:
	std::tr1::unordered_map<UINT32, fn> m_map;
	CUIHudDomi_Skill* m_noticeHUD;
};

#endif
