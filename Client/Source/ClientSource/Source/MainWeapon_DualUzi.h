#pragma once

//
//  이 클래스는 기존클래스 WeaponDualUzi에 맞춰서 일단 작성되었지만...나중에 클래스자체가 제거되어야한다.
//

#include "MainWeapon_DualSMG.h"

class MainWeapon_DualUzi : public MainWeapon_DualSMG
{
public:
	MainWeapon_DualUzi(WeaponBase* p);

protected:
	virtual void		OnCreate(bool bCreateInWeapon) override;
	// 소음기 모델링에 대한 알파처리용
	i3::vector<i3Node*>	m_SilencerMaterialList;
private:
	virtual void		ReturnInstance() override;

};

