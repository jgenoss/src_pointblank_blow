#pragma once

#include "MainWeapon_Knuckle.h"

class MainWeapon_DualThrowKnife : public MainWeapon_Knuckle
{
public:
	MainWeapon_DualThrowKnife(WeaponBase* p);

	void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx);

private:
	void	_Throw( VEC3D *vStart, VEC3D *vDir, REAL32 rSpeed, INT32 nNetSlotIdx);
};