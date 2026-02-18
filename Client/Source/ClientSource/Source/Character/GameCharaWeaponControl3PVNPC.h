#if !defined( __GAME_CHARA_WEAPON_CONTROL_3PV_NPC_H__)
#define __GAME_CHARA_WEAPON_CONTROL_3PV_NPC_H__

#include "GameCharaWeaponControl3PV.h"

class CGameCharaWeaponControl3PVNPC : public CGameCharaWeaponControl3PV
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl3PVNPC, CGameCharaWeaponControl3PV);
protected:
public:
	CGameCharaWeaponControl3PVNPC();
	virtual ~CGameCharaWeaponControl3PVNPC();

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void	OnUpdate( REAL32 rDeltatime) {}

	// 무기 생성(WeaponManager에서 받아옵니다.)
	virtual bool	OnCreateWeapon( INT32 nSlot, INT32 Itemid) { return false;}
	// 무기 삭제(WeaponManager로 반환합니다.)
	virtual void	OnDeleteWeapon( INT32 nSlot) {}
	// 무기를 캐릭터에 붙입니다.
	virtual WeaponBase * OnAttachWeapon( INT32 nSlot, i3Node * pSceneNode, i3GameNode * pGameNode) { return NULL;}
	// 무기를 캐릭터에서 때어냅니다.
	virtual WeaponBase * OnDetachWeapon( INT32 nSlot) { return NULL;}

	// 무기 초기화
	virtual void	OnResetWeapon( void)													{}

	// 등에 주무기를 붙입니다.
	virtual bool	OnAttachPurchasePrimWeapon( INT32 nSlot)								{ return false; }
	// 등에 붙은 주무기를 때어냅니다.
	virtual void	OnDetachPurchasePrimWeapon( void)										{}
	// 권총집에 보조무기를 붙입니다.
	virtual bool	OnAttachPurchaseSideWeapon( INT32 nSlot)								{ return false; }
	// 권총집에 붙은 보조무기를 때어냅니다.
	virtual void	OnDetachPurchaseSideWeapon( void)										{}

	// 해당 슬롯 무기로 교체(현재 프레임에서는 교체하지 않습니다.)
	virtual bool	OnPreSelectWeapon( INT32 nSlot)		{ return false; }
	// 해당 슬롯 무기로 교체(현재 프레임에서 교체됩니다.)
	// return true 해당 무기로 교체 성공
	virtual bool	OnSelectWeapon( INT32 nSlot, bool bPlaySound = true)				{ return false; }

	// 1/3인칭에 따라 무기 프로세스 처리
	virtual bool	OnChangeViewWeapon( void)												{ return false; }

	// Drop
	virtual bool	OnWeaponDropToWorld( INT32 nSlot, VEC3D * pPos, bool bChangeNextWeapon = true)	{ return false; }
	virtual bool	OnWeaponTake( WeaponBase * pWeapon)										{ return false; }
	virtual bool	OnDropedWeaponSearching( REAL32 rDeltatime)								{ return false; }

	virtual bool	OnFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL)			{ return false; }
	virtual bool	OnSecFire( VEC3D * pStartPos = NULL, VEC3D * pTargetPos = NULL)			{ return false; }
	virtual bool	OnFireReady( void)														{ return false; }

	virtual bool	OnZoom( void)															{ return false; }
	virtual bool	OnExtActivate( void)													{ return false; }
	virtual bool	OnExtDeactivate( void)													{ return false; }
	virtual bool	OnLoadMagazine( void)													{ return false; }
	virtual bool	OnLoadMagazineDual( void)												{ return false; }
	virtual bool	OnLoadBullet( void)														{ return false; }

	virtual bool	OnRemoveShell( void)													{ return false; }

	// 캐릭터 리스폰과 같이 호출됩니다.
	// 리스폰시 0번 무기로 교체하는 동작한다.
	virtual bool	OnProcessRespawn( void)													{ return false; }

};

#endif
