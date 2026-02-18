#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_H__

#include "GameCharaWeaponControl.h"

class CGameCharaWeaponControl3PV : public CGameCharaWeaponControl
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PV, CGameCharaWeaponControl);
public:
	virtual bool	OnChangeViewWeapon( void) override;

	// 해당 슬롯 무기로 교체(현재 프레임에서 교체됩니다.)
	// return true 해당 무기로 교체 성공
	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true) override;

	virtual bool	OnZoom( void) override;

	virtual bool	OnProcessRespawn( void) override;
};

#endif
