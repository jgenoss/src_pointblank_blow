#ifndef _GAME_CHARA_DOMI_STING_H_
#define _GAME_CHARA_DOMI_STING_H_

#include "GameCharaFullbodyDinosaur.h"

class WeaponBase;
class CGameCharaDomiSting : public CGameCharaFullbodyDinosaur
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaDomiSting, CGameCharaFullbodyDinosaur);

public:
	CGameCharaDomiSting();
	virtual ~CGameCharaDomiSting();

	virtual void Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void ResetChara() override;
	virtual void OnBindResource() override;
	virtual void OnUpdate(REAL32) override;

	void	Spit(GAMEINFO_CHARA_NETDATA* info);
	void	ThrowPoisonGrenade(GAMEINFO_CHARA_NETDATA* info);
	virtual WeaponBase* GetCurrentCharaWeapon() const override;
	MATRIX*	GetMatWeaponReleasePoint() const;

private:
	// domi_sting만 무기(WeaponBase)를 가진다.(domi_raptor 계열, domi_acid 계열은 모션만 취하므로 무기가 없다)
	// spit은 총알궤적과 지형이펙트를 생성해야한다. 이런 기능은 WeaponBase에 구현되어 있는데
	// 따로 빼서 구현하기가 어렵게 되어있다. 그래서 그냥 WeaponBase를 사용한다.
	WeaponBase*		m_wpSpit;		// [initialize at constructor]
	i3Transform*	m_wpReleasePoint = nullptr;
};

#endif