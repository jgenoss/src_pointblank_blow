/**
 * \file	Character\GameCharaWeaponControl.h
 *
 * Declares the game chara weapon control class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTROL_H__ )
#define __GAME_CHARA_WEAPON_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaWeaponControlDef.h"

#define WEAPON_CHECKTAKING_TIME		0.25f	// 드랍 웨폰 검색 (초당 4회 검사)

class CGameCharaWeaponManager;

class CGameCharaWeaponControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaWeaponControl, CGameCharaControlBase);

protected:
	inline WEAPON_SLOT_TYPE	getCurrentSlotNumber( void)			{ return getWeaponContext()->getCurrentSlotNum(); }
	inline void		setCurrentSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setCurrentSlotNum( nSlot); }
	inline WEAPON_SLOT_TYPE	getOldSlotNumber( void)				{ return getWeaponContext()->getOldSlotNum(); }
	inline void		setOldSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setOldSlotNum( nSlot); }
	inline WEAPON_SLOT_TYPE	getSwapSlotNumber( void)			{ return getWeaponContext()->getSwapSlotNum(); }
	inline void		setSwapSlotNumber( WEAPON_SLOT_TYPE nSlot)	{ getWeaponContext()->setSwapSlotNum( nSlot); }

	inline WeaponBase *	getWeapon( WEAPON_SLOT_TYPE nSlot)
	{
		I3_BOUNDCHK( nSlot, WEAPON_SLOT_COUNT);
		return getWeaponContext()->getWeapon( nSlot);
	}

	inline void		setWeapon( WEAPON_SLOT_TYPE nSlot, WeaponBase * pWeapon)
	{
		I3_BOUNDCHK_RETURN( nSlot, WEAPON_SLOT_COUNT);
		getWeaponContext()->setWeapon( nSlot, pWeapon);
	}

	inline WeaponBase *	getCurrentWeapon( void)			{ return getWeaponContext()->getCurrent(); }

	inline i3Node *	getWeaponAttachRoot( void)			{ return getWeaponContext()->getAttachRoot(); }

	inline WeaponBase *	getPurchasePrimWeapon( void)	{ return getWeaponContext()->getBackpack(); }
	inline void		setPurchasePrimWeapon( WeaponBase * pWeapon)	{ getWeaponContext()->setBackpack( pWeapon); }
	inline i3Node *	getDummyPrimPurchase( void)		{ return getWeaponContext()->getDummyPrimPurchase(); }


protected:
	bool			_ProcessDropedWeaponSearching( void);
	bool			_ProcessFireForNetwork( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER fireOrder = WEAPON::PRIMARY_FIRE);

	/** \brief 무기를 버리고 처리하는 함수
		\note 무기 교체를 실행합니다. */
	void			_AfterProcessDropWeapon( WeaponBase * pWeapon, bool bDirectPickup, bool bChangeNextWeapon = true);

	/** \brief 사용하지 않는 무기로 설정한다.
		\note 무기 Physic shape을 공중으로 옮기는 작업을 한다. */
	void			_NoUseWeapon( WeaponBase * pWeapon);

public:

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/*virtual*/ void	OnUpdate( REAL32 rDeltatime);

	// 무기 생성(WeaponManager에서 받아옵니다.) - nMultiWeaponSync 다중 무기에서만 사용 필요 없으면 Default 값
	virtual bool	OnCreateWeapon( WEAPON_SLOT_TYPE nSlot, T_ItemID Itemid, UINT8 nMultiWeaponSync = 0);
	// 무기 삭제(WeaponManager로 반환합니다.)
	/*virtual*/ void	OnDeleteWeapon( WEAPON_SLOT_TYPE nSlot);

	void			OnDeleteMultiWeapon( WEAPON_SLOT_TYPE nSlot);

	/** \brief 무기를 캐릭터에 붙입니다.
		\return 붙은 무기 pointer */
	virtual WeaponBase * OnAttachWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode);
	// 무기를 캐릭터에서 때어냅니다.

	/*virtual*/ WeaponBase * OnAttachBackWeapon( WEAPON_SLOT_TYPE nSlot, i3Node * pSceneNode, i3GameNode * pGameNode);
	// 무기를 캐릭터에서 때어냅니다.
	virtual WeaponBase * OnDetachWeapon( WEAPON_SLOT_TYPE nSlot);

	// 무기 초기화
	/*virtual*/ void	OnResetWeapon( void);

	/** \brief 무기상태 변경에 따른 캐릭터 기본 자세로 변경 */
	/*virtual*/ void	OnAttackIdleWeapon( void);

	// 등에 주무기를 붙입니다.
	virtual bool	OnAttachPurchasePrimWeapon( WEAPON_SLOT_TYPE nSlot);
	// 등에 붙은 주무기를 때어냅니다.
	virtual void	OnDetachPurchasePrimWeapon( void);
	// 권총집에 보조무기를 붙입니다.
	virtual bool	OnAttachPurchaseSideWeapon( WEAPON_SLOT_TYPE nSlot);
	// 권총집에 붙은 보조무기를 때어냅니다.
	virtual void	OnDetachPurchaseSideWeapon( void);

	/** \brief 해당 슬롯 무기로 교체
		\note 현재 프레임에서는 교체하지 않고 다음 프레임에 교체됩니다.
		\return 교체 가능*/
	/*virtual*/ bool	OnPreSelectWeapon( WEAPON_SLOT_TYPE nSlot);

	// 해당 슬롯 무기로 교체(현재 프레임에서 교체됩니다.)
	// return true 해당 무기로 교체 성공
	virtual bool	OnSelectWeapon( WEAPON_SLOT_TYPE nSlot, bool bPlaySound = true);

	// 1/3인칭에 따라 무기 프로세스 처리
	virtual bool	OnChangeViewWeapon( void)		{ return false; }

	/** \brief 무기를 월드에 버립니다. */
	/*virtual*/ bool	OnWeaponDropToWorld( WEAPON_SLOT_TYPE nSlot, VEC3D * pPos, bool bChangeNextWeapon = true, bool bDirectPickup = false);

	/** \brief 무기를 줍는다. */
	/*virtual*/ bool	OnWeaponTake( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList);

	/** \brief 월드에 떨어진 무기를 찾습니다. */
	/*virtual*/ bool	OnDropedWeaponSearching( REAL32 rDeltatime);

	/** \brief 무기 발사를 처리합니다. */
	/*virtual*/ bool	OnFire( VEC3D * pStartPos = nullptr, VEC3D * pTargetPos = nullptr);

	/** \brief 무기 발사를 처리합니다. */
	/*virtual*/ bool	OnSecFire( VEC3D * pStartPos = nullptr, VEC3D * pTargetPos = nullptr);

	/** \brief 공격 준비를 처리합니다. */
	/*virtual*/ bool	OnFireReady( void);

	/** \brief 무기 견착을 처리합니다 .*/
	virtual bool	OnZoom( void)					{ return false; }

	/** \brief Extension 활성화를 설정합니다. */
	virtual bool	OnExtActivate( void);

	/** \brief Extension 비활성화를 설정합니다. */
	virtual bool	OnExtDeactivate( void);

	/** \brief 장전을 설정합니다. */
	virtual bool	OnLoadMagazine( void);

	/** \brief Dual 탄창의 퀵로드를 설정합니다. */
	/*virtual*/ bool	OnLoadMagazineDual( void);

	/** \brief 약실에 탄약을 장전하도록 설정합니다. */
	/*virtual*/ bool	OnLoadBullet( void);

	/** \brief 약실에서 탁약을 제거합니다.
		\note 1인칭 Animation만 있습니다. */
	virtual bool	OnRemoveShell( void)			{ return false;}

	/** \brief 캐릭터 리스폰과 같이 호출됩니다.
		\note 리스폰시 0번 무기로 교체하는 동작한다. */
	virtual bool	OnProcessRespawn( void);

	/** \brief 장전 준비를 설정합니다. */
	virtual bool	OnLoadMagazine_Ready( void);

	bool			SetBlendAlpha( WEAPON_SLOT_TYPE nSlot, REAL32 fVal);
	void			SetBlendAlphaAll( REAL32 fVal);	
	bool			SetMaterialColor( WEAPON_SLOT_TYPE nSlot, COLORREAL * pfDiffuse, COLORREAL * pfSpecular);
	void			SetMaterialColorAll( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);

	void			NoUseWeapon(WeaponBase* pWeapon);

};

#endif
