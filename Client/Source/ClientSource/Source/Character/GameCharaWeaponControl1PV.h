/**
 * \file	Character\GameCharaWeaponControl1PV.h
 *
 * Declares the game chara weapon control 1 pv class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTROL_1PV_H__)
#define __GAME_CHARA_WEAPON_CONTROL_1PV_H__

#include "GameCharaWeaponControl.h"

class CGameCharaWeaponControl1PV : public CGameCharaWeaponControl
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl1PV, CGameCharaWeaponControl);
public:
	virtual bool	OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync = 0) override;
	virtual WeaponBase * OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode) override;
	virtual WeaponBase * OnDetachWeapon( WEAPON_SLOT_TYPE nSlot) override;

	virtual bool	OnChangeViewWeapon( void) override;

	/** \brief 등에 주무기를 붙입니다.
		\note 1인칭에서는 무기가 보이면 안됩니다. */
	virtual bool	OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot) override;

	/** \brief 보조 무기를 권총집에 붙입니다. */
	virtual bool	OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot) override;

	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true) override;

	virtual bool	OnZoom( void) override;
	virtual bool	OnExtActivate( void) override;
	virtual bool	OnExtDeactivate( void) override;
	virtual bool	OnRemoveShell( void) override;
	

	virtual bool	OnProcessRespawn( void) override;
};

#endif
