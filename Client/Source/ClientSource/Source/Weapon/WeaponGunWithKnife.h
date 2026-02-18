#if !defined( __WEAPON_GUN_WITH_KNIFE_H__)
#define __WEAPON_GUN_WITH_KNIFE_H__

#include "WeaponBase.h"

class WeaponGunWithKnife : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponGunWithKnife, WeaponBase);


protected:
	
	// 칼 모델링에 대한 알파처리용
	i3::vector<i3Node*>			m_KnifeMaterialList;

	
public:
	
	virtual CWeaponInfo * getRealWeaponInfo( void) override { if( isWeaponStateMask( WSM_ACTIVATE_EXTENSION)) return m_pExtWeaponInfo; return m_pWeaponInfo; }

public:
	WeaponGunWithKnife();
	virtual ~WeaponGunWithKnife();

	virtual void	PostCreate( void) override;
	virtual void	Reset( void) override;
	virtual void	_OnInitVairable( void) override;
	virtual void	FireEnd() override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	
	virtual void	_OnAddTerrainEffect( WEAPON_TERRAIN_EFFECT_INFO * pInfo) override;
	virtual void	_OnPlayTerrainHitSound( WEAPON::FIRE_ORDER fireOrder, UINT32 type, VEC3D * pPos) override;


	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;

	virtual void	SwapExtensionType(void) override;			//	Extension 기능을 스왑한다.
	virtual void	OnSetVisibleExtension( bool bVal) override;
	virtual REAL32	GetRange() override;
	virtual REAL32	GetDPD_Rate() override;
	virtual REAL32	GetDamage( bool bRandomDamage = true) override;

	virtual REAL32	GetPhysixForce( WEAPON_CLASS_TYPE classType) override;

	void			GetTargetDir( VEC3D * pDir);
	void			MeleeCollision( VEC3D * pStartPos, VEC3D * pTargetPos);

	

	////////////////////////////////////////////////////////////////////////////////////
	//			Animation Callback
	virtual void	OnExtensionActivate( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnExtensionDeactivate(  i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = 1) override;


};

#endif
