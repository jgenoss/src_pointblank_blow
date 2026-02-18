#include "pch.h"
#include "WeaponDualUzi.h"
#include "GlobalVariables.h"
#include "GameCharaWeaponContext.h"

#include "MainWeapon_DualUzi.h"

I3_CLASS_INSTANCE( WeaponDualUzi);//, WeaponDualSMG);

WeaponDualUzi::WeaponDualUzi()
{
}


WeaponDualUzi::~WeaponDualUzi()
{
}

MainWeapon*	WeaponDualUzi::CreateMainWeapon()
{
	MainWeapon_DualUzi * p = new MainWeapon_DualUzi(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_DualUzi));
	return p;
}

// 여기서, ReturnInstance함수는 제거될수가 없다.. (WeaponDual의 함수가 쓰여져선 안됨)

/*virtual*/ void WeaponDualUzi::ReturnInstance()
{
	WeaponBase::ReturnInstance();						// WeaponDual::ReturnInstance를 건너뛴다..(잘못된 구현방식)
}

